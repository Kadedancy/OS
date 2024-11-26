
int do_syscall(int req, unsigned p1, unsigned p2, unsigned p3){
    asm volatile("int $48" : 
        "+a"(req), "+b"(p1), "+c"(p2), "+d"(p3) );
    return req;
}
