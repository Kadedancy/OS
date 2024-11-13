#pragma once

#include "utils.h"
#include "kprintf.h"
#include "timer.h"

#define SEGMENT_PRESENT 0x80
#define SEGMENT_RING_0 0x00
#define SEGMENT_RING_3 0x60
#define SEGMENT_IS_CODE_OR_DATA 0x10
#define SEGMENT_TYPE_DATA 2
#define SEGMENT_TYPE_CODE 10
#define SEGMENT_32_BIT 0x40
#define SEGMENT_LIMIT_32_BIT 0x80
#define SEGMENT_LIMIT_HIGH_NIBBLE 0xf
#define NUM_INTERRUPTS 49
#define MAX_HANDLERS 4


#pragma pack(push,1)
struct GDTEntry{
    u16 limitBits0to15;
    u16 baseBits0to15;
    u8 baseBits16to23;
    u8 flags;
    u8 flagsAndLimitBits16to19;
    u8 baseBits24to31;
};

struct LGDT{
    u16 size;
    struct GDTEntry* addr;
};

struct InterruptContext{
    //we pushed these in midlevel handler
    u32 eax, ebx, ecx, edx, esi, edi, ebp;
    //we pushed this in lowlevel handler
    u32 interruptNumber;
    //cpu pushed these (except maybe errorcode)
    u32 errorcode, eip, cs, eflags;
    //cpu pushed these if there was a ring transition
    u32 esp, ss;
};

struct IDTEntry{
    u16 addrLow;     //address of lowlevel handler
    u16 selector;    //GDT entry
    u8 zero;         //must be zero
    u8 flags;        //0x8e for interrupt handler
    u16 addrHigh;    //address of lowlevel handler
};

struct LIDT{
    u16 size;               //table size, bytes
    struct IDTEntry* addr;  //table start
};

struct TaskStateSegment{
    u32 mustBeZero;
    u32 esp;
    u32 ss;     //stack segment
    u32 unused[15];
};

typedef void (*InterruptHandler)(struct InterruptContext* ctx);
static InterruptHandler handlers[NUM_INTERRUPTS][MAX_HANDLERS];
void register_interrupt_handler(unsigned interrupt,InterruptHandler func);
void panic(const char *);
void panic2(void* eip, const char* msg);
void ackPic1(struct InterruptContext* ctx);
void ackPic2(struct InterruptContext* ctx);
void highlevel_handler(struct InterruptContext* ctx);
void divideByZero(struct InterruptContext* ctx);
void register_interrupt_handler(unsigned interrupt,InterruptHandler func);
void interrupt_enable();
void timer0Handler(struct InterruptContext* ctx);
void interrupt_init();


#pragma pack(pop)


