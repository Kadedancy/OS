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
    else{
        Q->tail->next = n;
        Q->tail = n;
    }
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

unsigned kstrlen( const char* s){
    unsigned c = 0;
    while(*s++) c++;
    return c;
}

void kstrcpy(char* d, const char* s){
    while((*d++ = *s++));
    return;
}

int kstrequal(const char* a, const char* b) {
    while(*a && *b && *a == *b) ((void) ((a++) , (b++)));
    return (!*a) && (!*b);
}

char tolower(char c) {
    return (c >= 'A' && c <= 'Z') ? c+32 : c;
}

char toupper(char c) {
    return (c >= 'a' && c <= 'z') ? c-32 : c;
}

//ChatGpt prompt: Implement kmemcmp functionality
int kmemcmp(const void* ptr1, const void* ptr2, unsigned int num) {
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;

    for (unsigned int i = 0; i < num; i++) {
        if (p1[i] != p2[i]) {
            return (p1[i] - p2[i]);
        }
    }

    return 0;
}

void* kmemset(void* ptr, int value, unsigned int num) {
    unsigned char* p = (unsigned char*)ptr;

    for (unsigned int i = 0; i < num; i++) {
        p[i] = (unsigned char)value;
    }

    return ptr;
}