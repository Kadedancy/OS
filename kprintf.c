//jh apr 2023
//glue to use stb_sprintf code as kprintf

#include <stdarg.h>


#define STB_SPRINTF_MIN 8
#define STB_SPRINTF_IMPLEMENTATION 1
#define STB_SPRINTF_NOFLOAT 1

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wunused"

//https://stackoverflow.com/questions/1583196/building-visual-c-app-that-doesnt-use-crt-functions-still-references-some
unsigned _fltused;

#include "stb_sprintf.h"
#pragma clang diagnostic pop

extern void console_putc(char c);

static char* callback(char const* buf, void* userp, int len)
{
    int i;
    for(i=0;i<len;++i){
        console_putc(buf[i]);
    }
    return (char*)userp;
}

//writes to the console. Relies on console_putc() being defined.
int kprintf(const char* fmt, ... ){
    char buff[STB_SPRINTF_MIN];
    va_list args;
    int rv;

    va_start(args,fmt);
    rv = stbsp_vsprintfcb(callback, buff, buff, fmt, args );
    va_end(args);
    return rv;
}


//writes to the string s. Does not check for buffer overflow.
int ksprintf(char* s, const char* fmt, ... )
{
    va_list args;
    int rv;
    va_start(args,fmt);
    rv = stbsp_vsprintf(s,fmt,args);
    va_end(args);
    return rv;
}


int ksnprintf(char* s, int bufferSize, const char* fmt, ... )
{
    va_list args;
    int rv;
    va_start(args,fmt);
    rv = stbsp_vsnprintf(s,bufferSize,fmt,args);
    va_end(args);
    return rv;
}
