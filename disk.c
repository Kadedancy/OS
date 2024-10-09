#include "disk.h"

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
    
    u32 delta = vbr.first_sector + vbr.reserved_sectors + (vbr.sectors_per_cluster * 2);
    return delta += (clustnum - 2) * vbr.sectors_per_cluster;
}
static void read_root_callback( int errorcode,void* sectorData, void* kmain_callback){
    if( errorcode != SUCCESS ){
        kprintf("Cannot read Root Cluster: %d\n",errorcode);
        panic("Cannot continue");
        return;
    }

    unsigned char* data = (unsigned char*) sectorData;
    for(int i = 0; i < 512; i++){
        kprintf("%c",data[i]);
    } 

    kprintf("\nDONE!\n");
}

void do_this(){
    u32 sectorNum = clusterNumberToSectorNumber(vbr.root_cluster);
    disk_read_sectors(sectorNum,1,read_root_callback,0);
}