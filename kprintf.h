#ifndef KPRINTF_H
#define KPRINTF_H

//implementation of printf

//the attribute stuff allows gcc to do compile time checks
//to ensure we don't do something dumb like:
//kprintf( "%s" , 10 );

#ifdef __cplusplus
extern "C"{
#endif

//writes to the console. Relies on console_putc() being defined.
int kprintf(const char* fmt, ... ) __attribute__((format (printf , 1, 2 ) ));

//writes to the string s. Does not check for buffer overflow.
int ksprintf(char* s, const char* fmt, ... ) __attribute__((format (printf , 2, 3 ) ));

//writes to the string s. Does check for buffer overflow.
int ksnprintf(char* s, int bufferSize, const char* fmt, ... ) __attribute__((format (printf , 3, 4 ) ));

#ifdef __cplusplus
}       //extern "C"
#endif

#endif
