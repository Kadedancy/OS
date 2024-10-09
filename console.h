#pragma once

void console_putc(char ch);

struct MultibootInfo;       //forward declaration
void console_init(struct MultibootInfo* info);
void console_invert_pixel(unsigned x, unsigned y);
void set_pixel(unsigned x, unsigned y, u16 color);
void draw_character( unsigned char ch, int x, int y);