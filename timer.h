#pragma once

#define DIVISOR 12
extern unsigned int clock_ticks;

void timer_init();
unsigned get_uptime();
void inc_ticks();