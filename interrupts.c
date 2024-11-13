#include "interrupts.h"
#include "console.h"

static struct IDTEntry idt[NUM_INTERRUPTS];
extern void* lowlevel_addresses[];
static struct GDTEntry gdt[6];

static struct TaskStateSegment tss = {
    0,
    0x10000,    //64KB mark: Stack address
    16          //offset of GDT entry for stack segment
};

void gdt_init(){

    gdt[0].limitBits0to15 = 0;
    gdt[0].baseBits0to15 = 0;
    gdt[0].baseBits16to23 = 0;
    gdt[0].flags = 0;
    gdt[0].flagsAndLimitBits16to19 = 0;
    gdt[0].baseBits24to31 = 0;

    gdt[1].baseBits0to15=0;
    gdt[1].baseBits16to23=0;
    gdt[1].baseBits24to31=0;
    gdt[1].limitBits0to15=0xffff;
    gdt[1].flags = SEGMENT_TYPE_CODE |
                   SEGMENT_IS_CODE_OR_DATA |
                   SEGMENT_RING_0 |
                   SEGMENT_PRESENT ;
    gdt[1].flagsAndLimitBits16to19 =
                    SEGMENT_LIMIT_HIGH_NIBBLE |
                    SEGMENT_LIMIT_32_BIT |
                    SEGMENT_32_BIT;

    gdt[2].baseBits0to15=0;
    gdt[2].baseBits16to23=0;
    gdt[2].baseBits24to31=0;
    gdt[2].limitBits0to15=0xffff;
    gdt[2].flags = SEGMENT_TYPE_DATA |
                   SEGMENT_IS_CODE_OR_DATA |
                   SEGMENT_RING_0 |
                   SEGMENT_PRESENT ;
    gdt[2].flagsAndLimitBits16to19 =
                    SEGMENT_LIMIT_HIGH_NIBBLE |
                    SEGMENT_LIMIT_32_BIT |
                    SEGMENT_32_BIT;

    gdt[3].baseBits0to15=0;
    gdt[3].baseBits16to23=0;
    gdt[3].baseBits24to31=0;
    gdt[3].limitBits0to15=0xffff;
    gdt[3].flags = SEGMENT_TYPE_CODE |
                   SEGMENT_IS_CODE_OR_DATA |
                   SEGMENT_RING_3 |
                   SEGMENT_PRESENT ;
    gdt[3].flagsAndLimitBits16to19 =
                    SEGMENT_LIMIT_HIGH_NIBBLE |
                    SEGMENT_LIMIT_32_BIT |
                    SEGMENT_32_BIT;

    gdt[4].baseBits0to15=0;
    gdt[4].baseBits16to23=0;
    gdt[4].baseBits24to31=0;
    gdt[4].limitBits0to15=0xffff;
    gdt[4].flags = SEGMENT_TYPE_DATA |
                   SEGMENT_IS_CODE_OR_DATA |
                   SEGMENT_RING_3 |
                   SEGMENT_PRESENT ;
    gdt[4].flagsAndLimitBits16to19 =
                    SEGMENT_LIMIT_HIGH_NIBBLE |
                    SEGMENT_LIMIT_32_BIT |
                    SEGMENT_32_BIT;

    u32 tssAddr = (u32)(&tss);
    gdt[5].baseBits0to15=tssAddr & 0xffff;
    gdt[5].baseBits16to23=(tssAddr>>16) & 0xff;
    gdt[5].baseBits24to31=(tssAddr>>24) & 0xff;
    gdt[5].limitBits0to15=sizeof(tss)-1;
    gdt[5].flags = 0x89;
    gdt[5].flagsAndLimitBits16to19 = 0;

    struct LGDT lgdt;
    lgdt.size = sizeof(gdt);
    lgdt.addr = &gdt[0];
    void* tmp = &lgdt;
    asm volatile (
         "lgdt (%%eax)\n"       //load gdt register
         "ltr %%bx\n"           //new!
         "mov $16,%%eax\n"      //set eax to 16: Offset to gdt[2]
         "mov %%eax,%%ds\n"     //store 16 to ds
         "mov %%eax,%%es\n"     //store 16 to es
         "mov %%eax,%%fs\n"     //store 16 to fs
         "mov %%eax,%%gs\n"     //store 16 to gs
         "mov %%eax,%%ss\n"     //store 16 to ss
         "jmpl $0x8,$reset_cs\n"    //Intel says we must do a jmp here
         "reset_cs:\n"
         "nop\n"                //no operation
         : "+a"(tmp)
         : "b"( 40 | 3 )
         : "memory"
    );
}

asm (
    ".global _midlevel_handler\n"
    "_midlevel_handler:\n"
    "   push %ebp\n"
    "   push %edi\n"
    "   push %esi\n"
    "   push %edx\n"
    "   push %ecx\n"
    "   push %ebx\n"
    "   push %eax\n"
    "   push %esp\n"        //address of stack top = addr of eax
    "   cld\n"      //clear direction flag; C expects this
    "   call _highlevel_handler\n"
    "   addl $4, %esp\n"    //discard parameter for C
    "   pop %eax\n"
    "   pop %ebx\n"
    "   pop %ecx\n"
    "   pop %edx\n"
    "   pop %esi\n"
    "   pop %edi\n"
    "   pop %ebp\n"
    "   add $8,%esp\n"
    "   iret\n"
);

__asm__(    ".global _panic\n"
            "_panic:\n"
            "mov (%esp),%eax\n"     //eip -> eax
            "mov 4(%esp),%edx\n"    //string parameter
            "push %edx\n"
            "push %eax\n"
            "call _panic2\n"
);

void panic2(void* eip, const char* msg){
    kprintf("Kernel panic: At eip=%p: %s\n",
        eip,msg);
    while(1){
        __asm__("hlt");
    }
}

void ackPic1(struct InterruptContext* ctx){
    outb( 0x20, 32 );
}
void ackPic2(struct InterruptContext* ctx){
    outb( 0x20, 32 );
    outb( 0xa0, 32 );
}

void highlevel_handler(struct InterruptContext* ctx){
    int handled=0;
    unsigned interruptNumber = ctx->interruptNumber;
    for(int i=0;i<MAX_HANDLERS;++i){
        if( handlers[interruptNumber][i] ){
            handlers[interruptNumber][i](ctx);
            handled=1;
        }
    }
    if(!handled){
        kprintf("Warning: Unhandled interrupt: %d\n",interruptNumber);
    }
}

void divideByZero(struct InterruptContext* ctx){
    kprintf("Divide by zero\n");
}

void illegalOpcode(struct InterruptContext* ctx){
    kprintf("Illegal Opcode\n");
}

void generalFault(struct InterruptContext* ctx){
    kprintf("General protection fault at 0x%x\n", ctx->eip);
    while(1);
}

void register_interrupt_handler(unsigned interrupt,InterruptHandler func){
    if( interrupt >= NUM_INTERRUPTS ){
        panic("Bad interrupt number\n");
    }
    for(int i=0;i<MAX_HANDLERS;++i){
        if( !handlers[interrupt][i] ){
            handlers[interrupt][i] = func;
            return;
        }
    }
    panic("Too many handlers!\n");
}

void interrupt_enable(){
    asm volatile ("sti");
}

void timer0Handler(struct InterruptContext* ctx){
    console_invert_pixel(400,300);
}

void timer40Handler(struct InterruptContext* ctx){
    outb( 0x70, 0xc);
    inb( 0x71 );
    inc_ticks();
}

void interrupt_init(){
    gdt_init();

    for(int i=32;i<40;++i)
        register_interrupt_handler(i,ackPic1);
    for(int i=40;i<48;++i)
        register_interrupt_handler(i,ackPic2);

    for (int i = 0; i < NUM_INTERRUPTS; i++)
    {
        u32 a = (u32)(lowlevel_addresses[i]);
        idt[i].addrLow = (u16)(a & 0xFFFF);   
        idt[i].addrHigh = (u16)((a >> 16) & 0xFFFF); 
        idt[i].selector = 8;   
        idt[i].zero = 0;       
        idt[i].flags = 0x8e;  
    }

    outb(0x20, 0x11);
    outb(0x21, 32);
    outb(0x21, 4);
    outb(0x21, 1);
    outb(0x21, 0);

    outb( 0xa0, 0x11 );
    outb( 0xa1, 40 );
    outb( 0xa1, 2 );
    outb( 0xa1, 1 );
    outb( 0xa1, 0 );


    struct LIDT lidt;
    lidt.size = sizeof(idt);
    lidt.addr = idt;
    asm volatile("lidt (%%eax)" : : "a"(&lidt));

    register_interrupt_handler(0, divideByZero);
    register_interrupt_handler(6, illegalOpcode);
    register_interrupt_handler(13, generalFault);
    //register_interrupt_handler(32, timer0Handler);
    register_interrupt_handler(32, timer40Handler);
}




