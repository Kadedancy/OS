__asm__(
    ".global _start\n"
    "_start:\n"
    "mov $0x10000,%esp\n"
    "push %ebx\n"
    "call _kmain"
);

#include "utils.h"
#include "kprintf.h"
#include "console.h"
#include "serial.h"
#include "interrupts.h"
#include "timer.h"
#include "memory.h"
#include "disk.h"
#include "fs.h"

static struct MultibootInfo bootInfo;
void sweet();

void kmain2(){
    kprintf("START\n");
}

void kmain(struct MultibootInfo* mbi){
    kmemcpy(&bootInfo, mbi, sizeof(*mbi));

    console_init(&bootInfo);
    interrupt_init();
    timer_init();
    memory_init();
    disk_init();
    interrupt_enable();
    disk_read_metadata(sweet);

    while(1){
        halt();
    }
}