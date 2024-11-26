#include "syscall.h"



void syscall_handler(struct InterruptContext* ctx){
    int req = ctx->eax;
    if(req < 0) {
        ctx->eax = EINVAL;
        return;
    }

    switch(req){
        case SYSCALL_TEST:
            kprintf("Syscall test! %d %d %d\n",
                ctx->ebx, ctx->ecx, ctx->edx );
            ctx->eax = ctx->ebx + ctx->ecx + ctx->edx;
            break;
        case SYSCALL_READ:
            switch(ctx->ebx) {
                case 0: 
                    ctx->eax =  ENOSYS;
                    break;
                case 1: 
                case 2:
                    ctx->eax =  EINVAL;
                    break;
                default:    //calling file_read (not implemented yet)
                    ctx->eax =  ENOSYS;
                    break;
            }
            break;
        case SYSCALL_WRITE:
            switch(ctx->ebx) {
                case 0: 
                    ctx->eax = EINVAL;
                    break;
                case 1: 
                case 2: {
                    char* buffer = (char*)ctx->ecx;
                    for(unsigned i = 0; i < ctx->edx; i++) {
                        kprintf("%c", buffer[i]);
                    }
                    ctx->eax = ctx->edx;
                    break;
                }
                default:    //calling file_write(not implemented yet)
                    ctx->eax = ENOSYS;
                    break;
            }
            break;
        default:
            ctx->eax = ENOSYS;
    }
}