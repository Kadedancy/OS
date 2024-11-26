#include "../syscalldefs.h"

extern void printf(const char* fmt, ... );
int main(){
    printf("Hello from userland!\n");
    while(1){
    }
    return 0;
}