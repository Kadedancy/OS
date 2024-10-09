#include "utils.h"
#include "serial.h"
#include "console.h"
#include "etec3701_10x20.h"
#include "kprintf.h"

static volatile u8* framebuffer;
static unsigned pitch;
static unsigned width = 800;
static unsigned height = 600;
static u16 foregroundColor = 0b1010110101010101;
static u16 backgroundColor = 0b0000000000010101;
static int remember;

static u16 dimColors[8] = {
    0x0000, //black
    0b1010100000000000, //dark red
    0b0000010101000000, //dark green
    0b1010110101000000, //Olive
    0b0000000000010101, //Royal blue
    0b1010100000010101, //purple
    0b0000010101010101, //cyan
    0b1010110101010101, //grey
};

static u16 briteColors[8] = {
    0b0101001010001010, //dark grey
    0b1111101010001010, //pink
    0b0101011111101010, //lime
    0b1111111111101010, //yellow
    0b0101001010011111, //light blue
    0b1111101010011111, //magenta
    0b0101011111111111, //light cyan
    0b1111111111111111, //white

};

enum StateMachineState{
    NORMAL_CHARS,
    GOT_ESC,
    GOT_LBRACKET,
    GOT_3,
    GOT_3x,
    GOT_4,
    GOT_4x,
    GOT_9,
    GOT_9x,
    GOT_1,
    GOT_1x,
    GOT_1xx
};

enum StateMachineState currentState = NORMAL_CHARS;

static void clear_screen(){
    volatile u8* f8 = framebuffer;
    for(unsigned y=0; y<height; y++){
        volatile u16* f16 = (volatile u16*) f8;
        for(unsigned x=0; x<width; x++){
            f16[x] = backgroundColor;
        }
        f8 += pitch;
    }
}

void console_init(struct MultibootInfo* info){
    framebuffer = (volatile u8*) (info->fb.addr);
    pitch =  (info->fb.pitch);
    clear_screen();

}

void console_invert_pixel(unsigned x, unsigned y){
    volatile u16* p = (volatile u16*)(framebuffer + (y*pitch + x*2));
    *p ^= 0xffff;
}

void console_putc(char ch){
    static int cc = 0;
    static int cr = 0;
    serial_putc(ch);
    switch(currentState)
    {
        case NORMAL_CHARS:
            switch(ch) {
                case '\n':
                    cc = 0;
                    cr = cr + 1;
                    break;
                case '\r':
                    cc = 0;
                    break;
                case '\f':
                    clear_screen();
                    cc = 0;
                    cr = 0;
                    break;
                case '\t':
                    cc = cc + 8 - cc % 8;
                    if(cc >= 80)
                    {
                        cc = 0;
                        cr = cr + 1;
                    }
                    break;
                case '\x7f':
                    if(cc != 0 && cr != 0)
                    {
                        cc--;
                    }
                    else if(cc == 0 && cr != 0)
                    {
                        cr = cr - 1;
                        cc = 79;
                    }
                    draw_character(' ', cc * CHAR_WIDTH, cr * CHAR_HEIGHT);
                    break;

                case '\e':
                    currentState = GOT_ESC;
                    break;

                default:
                    draw_character(ch,cc * CHAR_WIDTH, cr * CHAR_HEIGHT);
                    cc += 1;
                    if( cc >= 80 )
                    {
                        cc = 0;
                        cr = cr + 1;
                    }
                    break;
                }
            break;
        case GOT_ESC:
            switch(ch){
                case '[':
                    currentState = GOT_LBRACKET;
                    break;
                default:
                    currentState = NORMAL_CHARS;
                    break;
            }
            break;
        case GOT_LBRACKET:
            switch(ch){
                case '3':
                    currentState = GOT_3;
                    break;
                case '4':
                    currentState = GOT_4;
                    break;
                case '9':
                    currentState = GOT_9;
                    break;
                case '1':
                    currentState = GOT_1;
                    break;
                default:
                    currentState = NORMAL_CHARS;
                    break;
            }
            break;
        case GOT_3:
        case GOT_4:
        case GOT_9:
        case GOT_1x:
            switch(ch){
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                    remember = ch - '0';
                    currentState++;
                    break;
                default: 
                    currentState = NORMAL_CHARS;
                    break;
            }
            break;
        case GOT_3x:
            switch(ch){
                case 'm':
                    foregroundColor = dimColors[remember];
                    
                default:
                    currentState = NORMAL_CHARS;
                    break;
            }
            break;
        case GOT_4x:
            switch(ch){
                case 'm':
                    backgroundColor = dimColors[remember];
                   
                default:
                    currentState = NORMAL_CHARS;
                    break;
            }
            break;
        case GOT_9x:
            switch(ch){
                case 'm':
                    foregroundColor = briteColors[remember];
                    
                default:
                    currentState = NORMAL_CHARS;
                    break;
            }
            break;
        case GOT_1: 
            switch(ch){
                case '0':
                    currentState = GOT_1x;
                    break;
                default:
                    currentState = NORMAL_CHARS;
                    break;
            }
            break;
        case GOT_1xx:
            switch(ch){
                case 'm':
                    backgroundColor = briteColors[remember];
                    
                default:
                    currentState = NORMAL_CHARS;
                break;
            }
            break;
        }
    }

void set_pixel(unsigned x, unsigned y, u16 color){
    volatile u16* p = ((volatile u16*)framebuffer + y * (pitch / sizeof(u16)) + x);
    *p = color;
}

void draw_character( unsigned char ch, int x, int y){
    int r,c;
    for(r=0;r<CHAR_HEIGHT;++r){
        for(c=0;c<CHAR_WIDTH;++c){
            if(font_data[ (unsigned) ch][r]& (1<<(CHAR_WIDTH - c - 1)))
                set_pixel( x+c, y+r, foregroundColor );
            else
                set_pixel( x+c, y+r, backgroundColor );
        }
    }
}