extern void printf(const char* fmt, ... );
int main(){
    printf("About to crash...\n");
    char* p = (char*) 0x1234;
    *p = 42;
    printf("We should not get here\n");
    while(1){}
}
