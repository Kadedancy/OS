/*#include "disk.h"

#define PCI_DISK_CLASS 1
#define PCI_DISK_SUBCLASS 1

static u32 portBase;        //First IO port to use
static u32 statusBase;      //IO port for getting status
static u32 interruptNumber;

static void getNativeResources(u32 addr) {
    portBase = pci_read_addr(addr,4) & ~0x3f;
    statusBase = (pci_read_addr(addr,5) & ~0x3f) + 2;
    interruptNumber = pci_read_addr(addr,15) & 0xff;
}

static void getLegacyResources() {
    portBase = 0x1f0;
    statusBase = 0x3f6;
    interruptNumber = 14;
}

static u32 dmaBase;
static void enable_busmaster(u32 addr) {
    u32 tmp = pci_read_addr(addr,1);
    tmp |= 4;
    pci_write_addr(addr,1,tmp);
    dmaBase = pci_read_addr(addr,8) & ~0x3;
}

void disk_init() {
    u32 addr = pci_scan_for_device(PCI_DISK_CLASS,
                                    PCI_DISK_SUBCLASS);
    if(addr == 0) {
        panic("No disk devices");
    }

    u32 tmp = pci_read_addr(addr,2);

    if(tmp & 0x100)
        getNativeResources(addr);
    else
        getLegacyResources();

    enable_busmaster(addr);
    register_interrupt_handler(interruptNumber+32,
                                disk_interrupt);
}

struct Request{
    unsigned sector;
    unsigned count;
    disk_callback_t callback;
    void* callback_data;
    char* buffer;
};

static struct Queue requestQueue;
static struct Request* currentRequest;

#define BUSY                0x80
#define CONTROLLER_ERROR    0x01
#define DISK_ERROR          0x20
#define DISK_READY          0x08
#define DATA                (portBase)
#define ERROR               (portBase)+1
#define COUNT               (portBase)+2
#define SECTOR0             (portBase)+3
#define SECTOR1             (portBase)+4
#define SECTOR2             (portBase)+5
#define SECTOR3SEL          (portBase)+6
#define CMDSTATUS           (portBase)+7
#define FLAGS               (statusBase)
#define COMMAND_IDENTIFY    (0xec)
#define COMMAND_READ_DMA    (0xc8)
#define COMMAND_WRITE_DMA   (0xca)
#define COMMAND_FLUSH       (0xe7)

static void dispatch_request(struct Request* req) {
    if(currentRequest != 0)
        panic("BUG: Cannot dispatch when a request is outstanding\n");
    
    req->buffer = kmalloc(req->count*512);
    if(!req->buffer) {
        req->callback(ENOMEM, NULL, req->callback_data);
        kfree(req);

        //just because this request failed doesn't mean
        //we won't have more luck with the next request...
        struct Request* nextReq = (struct Request*) queue_get(&requestQueue);
        if(nextReq) {
            dispatch_request(nextReq);
        }
        return;
    }

    currentRequest=req;
    while(inb(CMDSTATUS) & BUSY) {
    }

    outb(dmaBase,0);    //disable DMA
    outb(dmaBase,8);    //8=read,0=write

    physicalRegionDescriptor = (struct PhysicalRegionDescriptor*) kmalloc(sizeof(struct PhysicalRegionDescriptor));
    unsigned seg1 = ((unsigned)(physicalRegionDescriptor))/65536;
    unsigned seg2 = ((unsigned)(physicalRegionDescriptor)+sizeof(physicalRegionDescriptor))/65536;
    if(seg1 != seg2)
        panic("Physical region descriptor crosses 64KB boundary");
    if(seg1 % 4)
        panic("kmalloc gave address that is not multiple of 4");

    physicalRegionDescriptor->address = req->buffer;
    physicalRegionDescriptor->byteCount = req->count*512;
    physicalRegionDescriptor->flags = 0x8000;   //this is the last PRD
    
    outl(dmaBase+4, (u32) physicalRegionDescriptor);
    outb(dmaBase+2, 4+2);  //clear interrupt and error bits
    
    while(inb(CMDSTATUS) & BUSY) {
    }

    outb(SECTOR3SEL, 0xe0 | (req->sector>>24));
    
    outb(FLAGS,0);  //use interrupts
    outb(COUNT,req->count);

    outb(SECTOR0, req->sector & 0xff);
    outb(SECTOR1,(req->sector>>8)&0xff);
    outb(SECTOR2,(req->sector>>16)&0xff);

    outb(CMDSTATUS, COMMAND_READ_DMA);
    outb(dmaBase,9);  //start DMA: 9=read, 1=write
}

void disk_interrupt(struct InterruptContext* ctx) {
    int status = inb(dmaBase+2);
    int dmaError = status & 2;
    if(0 == (status & 4)) {
        panic("No disk IRQ");
    }

    //clear IRQ and error
    outb(dmaBase+2, (1<<2) | 2);

    struct Request* req = currentRequest;
    currentRequest=0;
    struct Request* nextReq = (struct Request*) queue_get(&requestQueue);

    if(nextReq) {
        dispatch_request(nextReq);
    }

    if(req == 0) {
        //BUG!
        panic("No current request?");
    } else {
        if(dmaError) {
            req->callback(EIO, NULL, req->callback_data);
        } else {
            req->callback(SUCCESS, req->buffer,  req->callback_data);
        }
        kfree(req->buffer);
        kfree(req);
    }
}

void disk_read_sectors(
                    unsigned firstSector,
                    unsigned numSectors,
                    disk_callback_t callback,
                    void* callback_data) {
    if(!callback) {
        panic("BUG: disk_read_sectors with no callback\n");
    }
    if(numSectors == 0 || numSectors > 127) {
        callback(EINVAL, NULL, callback_data);
        return;
    }
    
    struct Request* req = (struct Request*) kmalloc(
                sizeof(struct Request)
   );
    if(!req) {
        callback(ENOMEM, NULL, callback_data);
        return;
    }

    req->sector = firstSector;
    req->count = numSectors;
    req->callback = callback;
    req->callback_data = callback_data;

    if(currentRequest != NULL) {
        queue_put(&requestQueue, req);
    } else {
        dispatch_request(req);
    }
}

static struct VBR vbr;
static void read_vbr_callback( int errorcode,void* sectorData, void* kmain_callback){
    if( errorcode != SUCCESS ){
        kprintf("Cannot read VBR: %d\n",errorcode);
        panic("Cannot continue");
        return;
    }
    else{
        kmemcpy(&vbr,sectorData,sizeof(vbr));
        disk_metadata_callback_t f = (disk_metadata_callback_t) kmain_callback;
        f();
    }
}

static void read_partition_table_callback(int errorcode,void* sectorData,void* kmain_callback)
{
    if( errorcode != SUCCESS ){
        kprintf("Cannot read partition table: %d\n",errorcode);
        panic("Cannot continue");
    }

    else{
        struct GPTEntry* entry = (struct GPTEntry*) sectorData;
        disk_read_sectors( entry->firstSector, 1, read_vbr_callback, kmain_callback );
    }
}

void disk_read_metadata( disk_metadata_callback_t kmain_callback ){
    disk_read_sectors( 2, 1, read_partition_table_callback,kmain_callback );
}

u32 clusterNumberToSectorNumber(u32 clustnum){
    if(clustnum < 2){
        panic("BAD CLUSTER NUMBER!!!!!!");
    }
    
    u32 delta = vbr.first_sector + vbr.reserved_sectors + (vbr.sectors_per_fat * 2);
    delta += (clustnum - 2) * vbr.sectors_per_cluster;
    return delta; 
}
*//*static void read_root_callback( int errorcode,void* sectorData, void* sectorNum){
    if( errorcode != SUCCESS ){
        kprintf("Cannot read Root Cluster: %d\n",errorcode);
        panic("Cannot continue");
        return;
    }

    char* data = (char*) sectorData;

    for(int i = 0; i < 512; i++){
        kprintf("%c",data[i]);
    } 

    kprintf("\nDONE\n");
}*//*

static void parseFilename(struct DirEntry* entry, char* buffer) {
    unsigned bufferCharCout = 0;

    for(int i = 0; i < 8; i++) {
        if(entry->base[i] != '\0' && entry->base[i] != ' ')
            buffer[bufferCharCout++] = entry->base[i];
    } 

    buffer[bufferCharCout++] = '.';

    for(int i = 0; i < 3; i++) {
        if(entry->ext[i] != '\0' && entry->base[i] != ' ')
            buffer[bufferCharCout++] = entry->ext[i];
    }

    while(bufferCharCout < 13) {
         buffer[bufferCharCout++] = ' ';
    }

    buffer[--bufferCharCout] = '\0';
}

static unsigned len(char* string) {
    unsigned length = 0;

    while(string[length] != '\0')
        length++;
     
    return length;
}

static char longFilenameReversed[30];
static unsigned longFilenameReverseIndex = 0;

static void listFiles(int errorcode, void* buffer, void* callback){
    // Check for a read error
    if(errorcode != SUCCESS) {
        panic("ERROR!");
        return;
    }

    // Print the header
    kprintf("Filename      Size   Modified                Long name\n");
    kprintf("-------------------------------------------------------------------------------\n");

    // Cast to the buffer
    struct DirEntry* de = (struct DirEntry*)buffer;

    // Initialize the reversed long filename
    for(int i = 0; i < 100; i++)
        longFilenameReversed[i] = '\0';

    // Initialize reversed long filename to none
    longFilenameReversed[0] = ']';
    longFilenameReversed[1] = 'e';
    longFilenameReversed[2] = 'n';
    longFilenameReversed[3] = 'o';
    longFilenameReversed[4] = 'N';
    longFilenameReversed[5] = '[';

    // Iterate through the directory entries
    for(int i = 0; i < 128; i++) {
        // Check if the iteration is at the end of de
        if(de[i].base[0] == 0x00)
            break;

        // Check if the iteration is on a deleted file
        if((unsigned char)de[i].base[0] == 0xe5)
            continue;

        // Skip long filenames
        if(de[i].attributes != 0x0f) {
            // Create a name buffer
            char filenameBuffer[13];
            for(int j = 0; j < 13; j++)
                filenameBuffer[j] = '\0';

            // Get the no spaced filename
            parseFilename(&de[i], filenameBuffer);

            // Get the modified date
            unsigned short year = ((de[i].lastModifiedDate >> 9) & 0x7F) + 1980;
            unsigned short month = (de[i].lastModifiedDate >> 5) & 0x0f;
            unsigned short day = de[i].lastModifiedDate & 0x1F;

            // Get the modified time
            unsigned short hour = ((de[i].lastModifiedTime >> 11) & 0x1F);
            unsigned short minute = ((de[i].lastModifiedTime >> 5) & 0x3F);
            unsigned short second = de[i].lastModifiedTime & 0x3F;

            // Check the hour
            char hourType[3];
            if(hour > 12) {
                hour -= 12;
                hourType[0] = 'P';
                hourType[1] = 'M';
                hourType[2] = '\0';
            } else {
                hourType[0] = 'A';
                hourType[1] = 'M';
                hourType[2] = '\0';
            }

            // Get the length of the long filename
            unsigned longFilenameLength = len(longFilenameReversed);

            // Reverse the longFilenameLength
            char longFilename[100];
            for(int i = 0; i < 100; i++)
                longFilename[i] = '\0';

            int p1 = 0;
            int p2 = longFilenameLength - 1;
            while(p2 >= 0) {
                longFilename[p1] = longFilenameReversed[p2];
                p1++;
                p2--;
            }
        
            // Print the returned buffer
            kprintf("%-12s %6d  %02d/%02d/%04d %02d:%02d:%02d %s  %s\n",
                filenameBuffer, de[i].size,
                month, day, year, hour, minute, second, hourType, longFilename);
            

            // Reset the reversed long filename
            for(int i = 0; i < 100; i++)
                longFilenameReversed[i] = '\0';

            // Reset the reversed long filename to none
            longFilenameReversed[0] = ']';
            longFilenameReversed[1] = 'e';
            longFilenameReversed[2] = 'n';
            longFilenameReversed[3] = 'o';
            longFilenameReversed[4] = 'N';
            longFilenameReversed[5] = '[';

            // Reset the reversed long filename index to 0
            longFilenameReverseIndex = 0;
        }
        else {
            // Construct the lfn entry
            struct LFNEntry* lfn = (struct LFNEntry*)&de[i];

            // Add the namen backwards to the reversed long filename
            for(int i = 3; i >= 0; i--) {
                // Skip the null character
                if(lfn->name2[i] != 0x0000 && lfn->name2[i] != -1)
                    // kprintf("%c", lfn->name2[i]);
                    longFilenameReversed[longFilenameReverseIndex++] = lfn->name2[i];
            }

            for(int i = 11; i >= 0; i--) {
                // Skip the null character
                if(lfn->name1[i] != 0x0000 && lfn->name1[i] != -1)
                    // kprintf("%c", lfn->name1[i]);
                    longFilenameReversed[longFilenameReverseIndex++] = lfn->name1[i];
            }

            for(int i = 9; i >= 0; i--) {
                // Skip the null character
                if(lfn->name0[i] != 0x0000 && lfn->name0[i] != -1)
                    // kprintf("%c", lfn->name0[i]);
                    longFilenameReversed[longFilenameReverseIndex++] = lfn->name0[i];
            }
            longFilenameReversed[longFilenameReverseIndex] = '\0';
        }
    }
}

void do_this(){
    u32 sectorNum = clusterNumberToSectorNumber(vbr.root_cluster);
    disk_read_sectors(sectorNum,2,listFiles,(void*)sectorNum);
}*/
#include "disk.h"
#include "kprintf.h"

#define PCI_DISK_CLASS 1
#define PCI_DISK_SUBCLASS 1

static u32 portBase;        //First IO port to use
static u32 statusBase;      //IO port for getting status
static u32 interruptNumber;

static void getNativeResources(u32 addr) {
    portBase = pci_read_addr(addr,4) & ~0x3f;
    statusBase = (pci_read_addr(addr,5) & ~0x3f) + 2;
    interruptNumber = pci_read_addr(addr,15) & 0xff;
}

static void getLegacyResources() {
    portBase = 0x1f0;
    statusBase = 0x3f6;
    interruptNumber = 14;
}

static u32 dmaBase;
static void enable_busmaster(u32 addr) {
    u32 tmp = pci_read_addr(addr,1);
    tmp |= 4;
    pci_write_addr(addr,1,tmp);
    dmaBase = pci_read_addr(addr,8) & ~0x3;
}

void disk_init() {
    u32 addr = pci_scan_for_device(PCI_DISK_CLASS,
                                    PCI_DISK_SUBCLASS);
    if(addr == 0) {
        panic("No disk devices");
    }

    u32 tmp = pci_read_addr(addr,2);

    if(tmp & 0x100)
        getNativeResources(addr);
    else
        getLegacyResources();

    enable_busmaster(addr);
    register_interrupt_handler(interruptNumber+32,
                                disk_interrupt);
}

struct Request{
    unsigned sector;
    unsigned count;
    disk_callback_t callback;
    void* callback_data;
    char* buffer;
};

static struct Queue requestQueue;
static struct Request* currentRequest;

#define BUSY                0x80
#define CONTROLLER_ERROR    0x01
#define DISK_ERROR          0x20
#define DISK_READY          0x08
#define DATA                (portBase)
#define ERROR               (portBase)+1
#define COUNT               (portBase)+2
#define SECTOR0             (portBase)+3
#define SECTOR1             (portBase)+4
#define SECTOR2             (portBase)+5
#define SECTOR3SEL          (portBase)+6
#define CMDSTATUS           (portBase)+7
#define FLAGS               (statusBase)
#define COMMAND_IDENTIFY    (0xec)
#define COMMAND_READ_DMA    (0xc8)
#define COMMAND_WRITE_DMA   (0xca)
#define COMMAND_FLUSH       (0xe7)

static void dispatch_request(struct Request* req) {
    if(currentRequest != 0)
        panic("BUG: Cannot dispatch when a request is outstanding\n");
    
    req->buffer = kmalloc(req->count*512);
    if(!req->buffer) {
        req->callback(ENOMEM, NULL, req->callback_data);
        kfree(req);

        //just because this request failed doesn't mean
        //we won't have more luck with the next request...
        struct Request* nextReq = (struct Request*) queue_get(&requestQueue);
        if(nextReq) {
            dispatch_request(nextReq);
        }
        return;
    }

    currentRequest=req;
    while(inb(CMDSTATUS) & BUSY) {
    }

    outb(dmaBase,0);    //disable DMA
    outb(dmaBase,8);    //8=read,0=write

    physicalRegionDescriptor = (struct PhysicalRegionDescriptor*) kmalloc(sizeof(struct PhysicalRegionDescriptor));
    unsigned seg1 = ((unsigned)(physicalRegionDescriptor))/65536;
    unsigned seg2 = ((unsigned)(physicalRegionDescriptor)+sizeof(physicalRegionDescriptor))/65536;
    if(seg1 != seg2)
        panic("Physical region descriptor crosses 64KB boundary");
    if(seg1 % 4)
        panic("kmalloc gave address that is not multiple of 4");

    physicalRegionDescriptor->address = req->buffer;
    physicalRegionDescriptor->byteCount = req->count*512;
    physicalRegionDescriptor->flags = 0x8000;   //this is the last PRD
    
    outl(dmaBase+4, (u32) physicalRegionDescriptor);
    outb(dmaBase+2, 4+2);  //clear interrupt and error bits
    
    while(inb(CMDSTATUS) & BUSY) {
    }

    outb(SECTOR3SEL, 0xe0 | (req->sector>>24));
    
    outb(FLAGS,0);  //use interrupts
    outb(COUNT,req->count);

    outb(SECTOR0, req->sector & 0xff);
    outb(SECTOR1,(req->sector>>8)&0xff);
    outb(SECTOR2,(req->sector>>16)&0xff);

    outb(CMDSTATUS, COMMAND_READ_DMA);
    outb(dmaBase,9);  //start DMA: 9=read, 1=write
}

void disk_interrupt(struct InterruptContext* ctx) {
    int status = inb(dmaBase+2);
    int dmaError = status & 2;
    if(0 == (status & 4)) {
        panic("No disk IRQ");
    }

    //clear IRQ and error
    outb(dmaBase+2, (1<<2) | 2);

    struct Request* req = currentRequest;
    currentRequest=0;
    struct Request* nextReq = (struct Request*) queue_get(&requestQueue);

    if(nextReq) {
        dispatch_request(nextReq);
    }

    if(req == 0) {
        //BUG!
        panic("No current request?");
    } else {
        if(dmaError) {
            req->callback(EIO, NULL, req->callback_data);
        } else {
            req->callback(SUCCESS, req->buffer,  req->callback_data);
        }
        kfree(req->buffer);
        kfree(req);
    }
}

void disk_read_sectors(
                    unsigned firstSector,
                    unsigned numSectors,
                    disk_callback_t callback,
                    void* callback_data) {
    if(!callback) {
        panic("BUG: disk_read_sectors with no callback\n");
    }
    if(numSectors == 0 || numSectors > 127) {
        callback(EINVAL, NULL, callback_data);
        return;
    }
    
    struct Request* req = (struct Request*) kmalloc(
                sizeof(struct Request)
   );
    if(!req) {
        callback(ENOMEM, NULL, callback_data);
        return;
    }

    req->sector = firstSector;
    req->count = numSectors;
    req->callback = callback;
    req->callback_data = callback_data;

    if(currentRequest != NULL) {
        queue_put(&requestQueue, req);
    } else {
        dispatch_request(req);
    }
}

// Filesystem items
struct GUID efiBootGUID = {
    0xc12a7328, 0xf81f, 0x11d2, 0xba4b,
    { 0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b }
};

struct GUID windowsDataGUID = {
    0xebd0a0a2, 0xb9e5, 0x4433, 0x87c0,
    { 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7 }
};

struct GUID linuxGUID = {
    0x0fc63daf, 0x8483, 0x4772, 0x8e79,
    { 0x3d, 0x69,0xd8, 0x47, 0x7d, 0xe4 }
};

struct VBR vbr;

static void read_vbr_callback( int errorcode, void* sectorData, void* kmain_callback) {
    // Check for errors
    if( errorcode != SUCCESS ){
        panic("Cannot continue");
        return;
    }

    // Copy the VBR data out
    kmemcpy(&vbr,sectorData,sizeof(vbr));

    // Call the kmain call back
    disk_metadata_callback_t f = (disk_metadata_callback_t) kmain_callback;
    f();    
}

static void read_partition_table_callback(int errorcode, void* sectorData, void* kmain_callback) {
    // Check for error
    if( errorcode != SUCCESS ) {
        panic("Cannot continue");
    }

    // Set the sector data up as a array
    struct GPTEntry* entry = (struct GPTEntry*) sectorData;

    // Grab the first sector (VBR) out of the partition
    disk_read_sectors( entry->firstSector, 1, read_vbr_callback, kmain_callback );
}

void disk_read_metadata( disk_metadata_callback_t kmain_callback ) {
    disk_read_sectors( 2, 1, read_partition_table_callback, kmain_callback );
}

u32 clusterNumberToSectorNumber(u32 clnum) {
    // Ensure that the clnum is greater than two
    if(clnum < 2)
        panic("Bad cluster!");

    // Get the starting location of the clusters
    u32 clusterStart = vbr.first_sector + vbr.reserved_sectors + (vbr.sectors_per_fat * 2);

    // Break down the cluster starting position
    return clusterStart + (clnum - 2) * vbr.sectors_per_cluster;
}

void parseFilename(struct DirEntry* entry, char* buffer) {
    unsigned bufferCharCout = 0;

    // Iterate through the filename
    for(int i = 0; i < 8; i++) {
        // Get the filename character
        if(entry->base[i] != '\0' && entry->base[i] != ' ')
            buffer[bufferCharCout++] = entry->base[i];
    } 

    // Add the period
    buffer[bufferCharCout++] = '.';

    // Iterate through the file extension
    for(int i = 0; i < 3; i++) {
        // Get the extension character
        if(entry->ext[i] != '\0' && entry->base[i] != ' ')
            buffer[bufferCharCout++] = entry->ext[i];
    }

    // Padd the end of the buffer
    while(bufferCharCout < 13) {
         buffer[bufferCharCout++] = ' ';
    }

    // End the buffer
    buffer[--bufferCharCout] = '\0';
}

static unsigned len(char* string) {
    unsigned length = 0;

    while(string[length] != '\0')
        length++;
     
    return length;
}

// String to hold the reversed long filename
static char longFilenameReversed[30];

// Variable to hold the current position inside of the reversed long file name
static unsigned longFilenameReverseIndex = 0;

// Root Directory items
static void listFiles(int errorCode, void* buffer, void* callback) {   // FUTURE: callback -> filename
    // Check for a read error
    if(errorCode != SUCCESS) {
        panic("ERROR!");
        return;
    }

    // Print the header
    kprintf("Filename      Size   Modified                Long name\n");
    kprintf("-------------------------------------------------------------------------------\n");

    // Cast to the buffer
    struct DirEntry* de = (struct DirEntry*)buffer;

    // Initialize the reversed long filename
    for(int i = 0; i < 100; i++)
        longFilenameReversed[i] = '\0';

    // Initialize reversed long filename to none
    longFilenameReversed[0] = ']';
    longFilenameReversed[1] = 'e';
    longFilenameReversed[2] = 'n';
    longFilenameReversed[3] = 'o';
    longFilenameReversed[4] = 'N';
    longFilenameReversed[5] = '[';

    // Iterate through the directory entries
    for(int i = 0; i < 128; i++) {
        // Check if the iteration is at the end of de
        if(de[i].base[0] == 0x00)
            break;

        // Check if the iteration is on a deleted file
        if((unsigned char)de[i].base[0] == 0xe5)
            continue;

        // Skip long filenames
        if(de[i].attributes != 0x0f) {
            // Create a name buffer
            char filenameBuffer[13];
            for(int j = 0; j < 13; j++)
                filenameBuffer[j] = '\0';

            // Get the no spaced filename
            parseFilename(&de[i], filenameBuffer);

            // Get the modified date
            unsigned short year = ((de[i].lastModifiedDate >> 9) & 0x7F) + 1980;
            unsigned short month = (de[i].lastModifiedDate >> 5) & 0x0f;
            unsigned short day = de[i].lastModifiedDate & 0x1F;

            // Get the modified time
            unsigned short hour = ((de[i].lastModifiedTime >> 11) & 0x1F);
            unsigned short minute = ((de[i].lastModifiedTime >> 5) & 0x3F);
            unsigned short second = de[i].lastModifiedTime & 0x3F;

            // Check the hour
            char hourType[3];
            if(hour > 12) {
                hour -= 12;
                hourType[0] = 'P';
                hourType[1] = 'M';
                hourType[2] = '\0';
            } else {
                hourType[0] = 'A';
                hourType[1] = 'M';
                hourType[2] = '\0';
            }

            // Get the length of the long filename
            unsigned longFilenameLength = len(longFilenameReversed);

            // Reverse the longFilenameLength
            char longFilename[100];
            for(int i = 0; i < 100; i++)
                longFilename[i] = '\0';

            int p1 = 0;
            int p2 = longFilenameLength - 1;
            while(p2 >= 0) {
                longFilename[p1] = longFilenameReversed[p2];
                p1++;
                p2--;
            }
        
            // Print the returned buffer
            kprintf("%-12s %6d  %02d/%02d/%04d %02d:%02d:%02d %s  %s\n",
                filenameBuffer, de[i].size,
                month, day, year, hour, minute, second, hourType, longFilename);
            

            // Reset the reversed long filename
            for(int i = 0; i < 100; i++)
                longFilenameReversed[i] = '\0';

            // Reset the reversed long filename to none
            longFilenameReversed[0] = ']';
            longFilenameReversed[1] = 'e';
            longFilenameReversed[2] = 'n';
            longFilenameReversed[3] = 'o';
            longFilenameReversed[4] = 'N';
            longFilenameReversed[5] = '[';

            // Reset the reversed long filename index to 0
            longFilenameReverseIndex = 0;
        }
        else {
            // Construct the lfn entry
            struct LFNEntry* lfn = (struct LFNEntry*)&de[i];

            // Add the name backwards to the reversed long filename
            for(int i = 3; i >= 0; i--) {
                // Skip the null character
                if(lfn->name2[i] != 0x0000 && lfn->name2[i] != -1)
                    longFilenameReversed[longFilenameReverseIndex++] = lfn->name2[i];
            }

            for(int i = 11; i >= 0; i--) {
                // Skip the null character
                if(lfn->name1[i] != 0x0000 && lfn->name1[i] != -1)
                    longFilenameReversed[longFilenameReverseIndex++] = lfn->name1[i];
            }

            for(int i = 9; i >= 0; i--) {
                // Skip the null character
                if(lfn->name0[i] != 0x0000 && lfn->name0[i] != -1)
                    longFilenameReversed[longFilenameReverseIndex++] = lfn->name0[i];
            }
            longFilenameReversed[longFilenameReverseIndex] = '\0';
        }
    }
}

void do_this() {
    // Get the root sector num
    u32 rootSectorNum = clusterNumberToSectorNumber(vbr.root_cluster);

    // Read the root sector
    disk_read_sectors(rootSectorNum, 2, listFiles, 0);
}