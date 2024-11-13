/*#include "syscall.h"

void syscall_handler(struct InterruptContext* ctx){
    int req = ctx->eax;
    switch(req){
        case SYSCALL_TEST:
            kprintf("Syscall test! %d %d %d\n",
                ctx->ebx, ctx->ecx, ctx->edx );
            ctx->eax = ctx->ebx + ctx->ecx + ctx->edx;
            break;
        default:
            ctx->eax = ENOSYS;
    }
}*/