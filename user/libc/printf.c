//jh apr 2023

#include <stdarg.h>
#include "syscall.h"

unsigned _fltused;

#define STB_SPRINTF_MIN 8
#define STB_SPRINTF_IMPLEMENTATION 1
#define STB_SPRINTF_NOFLOAT 1

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#include "stb_sprintf.h"
#pragma clang diagnostic pop

static char* callback(char const* buf, void* userp, int len)
{
    while(len>0){
        int rv = do_syscall(SYSCALL_WRITE, 1, (unsigned) buf, (unsigned)len );
        len -= rv;
        buf += rv;
    }
    return (char*)userp;
}

int printf(const char* fmt, ... ){
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
