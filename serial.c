#include "serial.h"
#include "utils.h"

#define SERIAL_STATUS 0x3fd
#define SERIAL_DATA 0x3f8
#define CLEAR_TO_SEND (1<<5)

void serial_init()
{
    //nothing to do for now...
}

void serial_putc(char ch)
{
    while( !(inb(SERIAL_STATUS) & CLEAR_TO_SEND) ){

    }

    outb( SERIAL_DATA, ch);

}
