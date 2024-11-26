// jh etec 3701 au 2008, au 2013

//stdarg header to allow variable length arg list processing

#ifndef STDARG_H
#define STDARG_H

//this keeps track of where we were in the argument list:
//curr is a pointer to the next thing to return when someone
//calls va_arg.
typedef struct _va_list{
	char* curr;
} va_list ;

#define va_start( ap , last )  real_va_start( &ap, &last, sizeof(last) )

static void real_va_start(va_list* ap, void* last, int size){
    ap->curr = ((char*)last) + size;
}

void va_end(va_list x){
}

#define va_arg( ap, type ) *((type*)(real_va_arg(&ap,sizeof(type))))

static char* real_va_arg(va_list* ap, int size){
    char* x = ap->curr;
    ap->curr += size;
    return x;
}

#endif
 
