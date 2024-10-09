#include "disk.h"
#include "kprintf.h"

static void callback(int errorcode, void* vdata, void* callback_data)
{
    if( errorcode ){
        kprintf("ERROR: %d\n",errorcode);
        return;
    }
    char* data = (char*) vdata;
    kprintf("Signature (should be 'EFI PART'): %.8s\n",
        data+512);
    kprintf("Partition 0 name: ");
    char* p = data+1024+56;
    for(int i=0;i<36;i+=2){
        kprintf("%c",p[i]);
    }
    kprintf("\n");
    return;
}

void sweet(){
    kprintf("BEFORE READ\n");
    disk_read_sectors(0,8,callback,0);
    kprintf("AFTER READ\n");
}
