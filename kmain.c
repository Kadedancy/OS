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
#include "exec.h"

static struct MultibootInfo bootInfo;

void kmain2(){
    exec("HELLO.EXE", 0x400000, exec_transfer_control, 0);
}

void kmain(struct MultibootInfo* mbi){
    kmemcpy(&bootInfo, mbi, sizeof(*mbi));

    console_init(&bootInfo);
    interrupt_init();
    timer_init();
    memory_init();
    pageInit(&bootInfo);
    disk_init();
    interrupt_enable();
    disk_read_metadata(kmain2);

    while(1){
        halt();
    }
}