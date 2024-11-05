#include "exec.h"
#include "fs.h"
/*
#define SECTION_CODE (1<<5)
#define SECTION_DATA (1<<6)
#define SECTION_UNINITIALIZED_DATA (1<<7)
#define SECTION_READABLE (1<<30)
#define SECTION_WRITABLE (1<<31)
#define EXE_BASE 0x400000
#define EXE_STACK 0x800000

struct ReadFullyInfo{
    int fd;             //file descriptor
    char* buf;          //destination buffer
    unsigned readSoFar; //how many bytes we've read so far
    unsigned capacity;  //buffer capacity: number to read
    file_read_callback_t callback;  //callback when done reading
    void* callback_data;        //data for callback
};

struct ExecInfo{
    int fd;
    unsigned loadAddress;
    exec_callback_t callback;
    void* callback_data;
    ...more fields to be added...
};

void file_read_fully2(int errorcode, void* buf, unsigned numread, void* callback_data)
{
    struct ReadFullyInfo* rfi = (struct ReadFullyInfo*)callback_data;

    if( errorcode ){
        rfi->callback(errorcode, rfi->buf, rfi->readSoFar, callback_data);
        kfree(rfi);
        return;
    }

    rfi->readSoFar += numread;

    if( numread == 0 && rfi->readSoFar < rfi->capacity ){
        rfi->callback(ENODATA, rfi->buf, rfi->readSoFar, rfi->callback_data);
        kfree(rfi);
        return;
    }

    if( rfi->readSoFar == rfi->capacity ){
        rfi->callback(SUCCESS, rfi->buf, rfi->readSoFar, rfi->callback_data );
        kfree(rfi);
        return;
    }

    file_read(rfi->fd,rfi->buf+rfi->readSoFar,rfi->capacity - rfi->readSoFar,file_read_fully2,rfi);
}

void file_read_fully(int fd, void* buf, unsigned count,file_read_callback_t callback,void* callback_data){
    struct ReadFullyInfo* rfi = kmalloc(sizeof(struct ReadFullyInfo));
    if(!rfi){
        callback(ENOMEM, buf, 0, callback_data );
        return;
    }
    rfi->fd = fd;
    rfi->buf = (char*) buf;
    rfi->readSoFar = 0;
    rfi->capacity=count;
    rfi->callback=callback;
    rfi->callback_data=callback_data;

    file_read( fd, buf, count, file_read_fully2, rf);
}

void exec_transfer_control(int errorcode,
                           u32 entryPoint,
                           void* callback_data
){
    if( errorcode ){
        kprintf("exec failed: %d\n",errorcode);
        return;
    }
    u32 c = EXE_STACK;
    u32 b = entryPoint;
    asm volatile(
        "mov %%ecx, %%esp\n"
        "jmp *%%ebx\n"
        : "+c"(c), "+b"(b) );
}

void doneLoading(struct ExecInfo* ei)
{
    u32 entryPoint = ei->peHeader.optionalHeader.imageBase +
                     ei->peHeader.optionalHeader.entryPoint;
    exec_callback_t callback = ei->callback;
    void* callback_data = ei->callback_data;
    file_close(ei->fd, NULL, NULL);
    kfree(ei);
    callback(SUCCESS, entryPoint, callback_data );
    return;
}


void exec6( int errorcode, void* buf, unsigned nr, void* callback_data)
{
    struct ExecInfo* ei = (struct ExecInfo*) callback_data;

    if( errorcode ){
        ei->callback(errorcode, 0, ei->callback_data);
        file_close(ei->fd, NULL, NULL);
        kfree(ei);
        return;
    }

    ei->numSectionsLoaded++;
    loadNextSection(ei);
    return;
}

void loadNextSection(struct ExecInfo* ei){

    //shorten things up
    int i = ei->numSectionsLoaded;

    if( i == ei->peHeader.coffHeader.numSections ){
        doneLoading(ei);
        return;
    }
    
    struct SectionHeader* s = &(ei->sectionHeaders[i]);
    unsigned flags = s->characteristics;
    if( flags & (SECTION_CODE | SECTION_DATA | SECTION_UNINITIALIZED_DATA) ){
        char* p = (char*) (ei->loadAddress);
        p += s->address;
        int numToLoad = ...smaller of s->sizeInRAM and s->sizeOnDisk
        ...zero out bytes from p+numToLoad to p+sizeInRAM-1 inclusive...
        ...file_seek to s->dataPointer...
        ...check for seek error...
        file_read_fully( ei->fd, p, numToLoad, exec6, ei );
    } 
    else {
        ei->numSectionsLoaded += 1;
        loadNextSection(ei);
    }
}

void exec5( int errorcode, void* buf, unsigned numread, void* callback_data)
{
     struct ExecInfo* ei = (struct ExecInfo*)callback_data;
     if(errorcode){
         ei->callback(errorcode, 0, ei->callback_data);
         file_close(ei->fd, NULL, NULL);
         kfree(ei);
         return;
     }

    ei->numSectionsLoaded = 0;
    loadNextSection(ei);
 }

void exec4( int errorcode, void* buf, unsigned numread, void* callback_data)
{
    struct ExecInfo* ei = (struct ExecInfo*)callback_data;
    if(errorcode){
        ei->callback(errorcode, 0, ei->callback_data);
        file_close(ei->fd, NULL, NULL );
        kfree(ei);
        return;
    }
    if( ei->peHeader.coffHeader.machineType != 0x14c || ei->peHeader.optionalHeader.magic != 0x010b ){
        ei->callback(ENOEXEC, 0, ei->callback_data);
        file_close(ei->fd, NULL, NULL);
        kfree(ei);
        return;
    }
    if( ei->peHeader.optionalHeader.imageBase != EXE_BASE ){
        ei->callback(EFAULT, 0, ei->callback_data);
        file_close(ei->fd, NULL, NULL);
        kfree(ei);
        return;
    }
    if( ei->peHeader.coffHeader.numSections > MAX_SECTIONS ){
        ei->callback(ENOEXEC, 0, ei->callback_data);
        file_close(ei->fd, NULL, NULL);
        kfree(ei);
        return;
    }
    file_read_fully(ei->fd, ei->sectionHeaders,ei->peHeader.coffHeader.numSections * sizeof(struct SectionHeader),exec5, ei );
}

void exec3( int errorcode, void* buf, unsigned numread, void* callback_data)
{
    struct ExecInfo* ei = (struct ExecInfo*)callback_data;
    if(errorcode){
        ei->callback(errorcode, 0, ei->callback_data);
        file_close(ei->fd, NULL, NULL);
        kfree(ei);
        return;
    }
    if( 0 != kmemcmp(ei->dosHeader.magic,"MZ",2) ){
        ei->callback(ENOEXEC, 0, ei->callback_data);
        file_close(ei->fd, NULL, NULL );
        kfree(ei);
        return;
    }
        int rv = file_seek(ei->fd, ei->dosHeader.peOffset, SEEK_SET);
    if(rv<0){
        ei->callback(rv, 0, ei->callback_data);
        file_close(ei->fd, NULL, NULL );
        kfree(ei);
        return;
    }
    file_read_fully( ei->fd, &(ei->peHeader),
                     sizeof(ei->peHeader),
                     exec4, ei );
}

void exec2( int fd, void* callback_data)
{
    struct ExecInfo* ei = (struct ExecInfo*)callback_data;
    if(fd < 0){
        ei->callback(fd, 0, callback_data );
        kfree(ei);
        return;
    }
    ei->fd=fd;
    file_read_fully( fd, &(ei->dosHeader),
                     sizeof(ei->dosHeader),
                     exec3, ei );
}

void exec(  const char* fname,
            unsigned loadAddress,
            exec_callback_t callback,
            void* callback_data ){

    struct ExecInfo* ei = kmalloc(sizeof(struct ExecInfo));
    if(!ei){
        callback(ENOMEM, NULL, callback_data);
        return;
    }
    ei->callback=callback;
    ei->loadAddress=loadAddress;
    ei->callback_data=callback_data;
    file_open(fname, 0, exec2, ei );    //0=flags
}
*/