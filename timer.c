#include "timer.h"
#include "utils.h"
#include "kprintf.h"

unsigned int clock_ticks = 0;

void timer_init(){
    unsigned rate = 16;
    outb( 0x70, 0xa );
    unsigned tmp = inb(0x71);
    outb( 0x70, 0xa );
    outb( 0x71, rate | (tmp & 0xf0) );

    outb(0x70,11);
    unsigned sv = inb(0x71);
    outb(0x70,11);
    outb( 0x71, sv | 0x40);

}

unsigned get_uptime(){
    return ((clock_ticks / 16)* 1000);
}

void inc_ticks(){
    clock_ticks++;
}