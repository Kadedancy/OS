

#define SERIAL_IO 0x3f8
#define SERIAL_FLAGS 0x3fd
#define SERIAL_READY (1<<5)

typedef unsigned short u16;
typedef unsigned char u8;

void outb(u16 port, u8 value){
    asm("outb %%al,%%dx" : : "a"(value), "d"(port) );
}

u8 inb(u16 port){
    unsigned tmp;
    asm("inb %%dx,%%al" : "=a"(tmp) : "d"(port) );
    return (u8)tmp;
}

void output(char c){
    while( 1 ){
        int f = inb(SERIAL_FLAGS);
        if( f & SERIAL_READY)
            break;
    }
    outb(SERIAL_IO, c);
}
    
const char* S = "Hello, world!\n";
int main(){
    const char* p = S;
    while(*p){
        output(*p);
        p++;
    }
    while(1){
        asm("hlt");
    }
}
    
