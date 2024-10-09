#include "utils.h"
#include "errno.h"
#include "memory.h"

void outb(u16 port, u8 value){
    __asm__ volatile("outb %%al,%%dx" : : "a"(value), "d"(port) );
}

u8 inb(u16 port){
    u32 tmp;
    __asm__ volatile("inb %%dx,%%al" : "=a"(tmp) : "d"(port) );
    return (u8)tmp;
}

void kmemcpy(void* dest, const void* src, unsigned size) 
{
    char* dp = (char*) dest;
    char* sp = (char*) src;
    
    while(size--)
        *dp++ = *sp++;
}

void halt(){
    __asm__ volatile("hlt");
}

u16 inw(u16 port){
    u32 tmp;
    asm volatile ("inw %%dx,%%ax"
        : "=a"(tmp)         //one output
        : "d"(port)         //one input
    );
    return (u16)tmp;
}

void outw(u16 port, u16 value){
    asm volatile("outw %%ax,%%dx"
        :   //no outputs
        : "a"(value), "d"(port) //two inputs
    );
}

u32 inl(u16 port){
    u32 tmp;
    asm volatile ("inl %%dx,%%eax"
        : "=a"(tmp)         //one output
        : "d"(port)         //one input
    );
    return tmp;
}

void outl(u16 port, u32 value){
    asm volatile("outl %%eax,%%dx"
        :   //no outputs
        : "a"(value), "d"(port) //two inputs
    );
}

int queue_put(struct Queue* Q, void* data){
    struct QueueNode* n = (struct QueueNode*)kmalloc(sizeof(struct QueueNode));
    if(!n){
        return ENOMEM;
    }
    n->item = data;
    n->next = NULL;
    if(Q->head == NULL){
        Q->head = n;
        Q->tail = n;
    }
    Q->tail->next = n;
    Q->tail = n;
    return SUCCESS;

}

void* queue_get(struct Queue* Q){
    if(Q->head == NULL){
        return NULL;
    }
    void* temp = Q->head->item;
    struct QueueNode* n = Q->head->next;
    kfree(Q->head);
    Q->head = n;
    if(!n){
        Q->tail = NULL;
    }

    return temp;

}