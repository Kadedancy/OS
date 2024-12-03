#include "memory.h"


char* heap = (char*)0x10000;
#define HEAP_ORDER 19
Header* freeList[HEAP_ORDER+1];

static Header* getNext(Header* h){
    unsigned delta = (h->next) << 6;
    Header* h2 = (Header*)(heap+delta);
    if( h == h2 )
        return NULL;
    return h2;
}

static void setNext(Header* h, Header* next ){
    if( next == NULL )
        next = h;
    char* c = (char*) next;
    unsigned delta = c-heap;
    delta >>= 6;
    h->next = delta;
}

static Header* getPrev(Header* h){
    unsigned delta = (h->prev) << 6;
    Header* h2 = (Header*)(heap+delta);
    if( h == h2 )
        return NULL;
    return h2;
}

static void setPrev(Header* h, Header* prev ){
    if( prev == NULL )
        prev = h;
    char* c = (char*) prev;
    unsigned delta = c-heap;
    delta >>= 6;
    h->prev = delta;
}


static void initHeader(Header* h, unsigned order){
    h->used=0;
    h->order=order;
    setPrev(h,NULL);
    setNext(h,NULL);
}

void memory_init(void){
    for(unsigned i=0;i<HEAP_ORDER;++i){
        freeList[i] = NULL;
        freeList[HEAP_ORDER] = (Header*)heap;
        initHeader(freeList[HEAP_ORDER],HEAP_ORDER);
    }
}


void removeFirstNode(unsigned i){
     Header* h = freeList[i];
     Header* n = getNext(h);
     if(n)
         setPrev(n,NULL);
     freeList[i] = n;
}


void removeNode(Header* h){
    Header* p = getPrev(h);
    Header* n = getNext(h);
    if(p)
        setNext(p,n);
    if(n)
        setPrev(n,p);

    //if we're removing the first node,
    //need to update head
    if( freeList[h->order] == h )
        freeList[h->order] = n;
}


void prependNode( Header* h ){
     unsigned i = h->order;
     Header* n = freeList[i];
     setPrev(h, NULL);
     setNext(h, n);
     if(n)
         setPrev(n,h);
     freeList[i] = h;
}


void splitBlock(unsigned i){
    Header* h = freeList[i];
    removeFirstNode(i);
    h->order = h->order-1;
    char* c = (char*) h;
    c += (1<<h->order);
    Header* h2 = (Header*) c;
    initHeader(h2,h->order);
    prependNode(h);
    prependNode(h2);
}


void* kmalloc(u32 size){
    size += sizeof(Header);
    if( size > (1<<HEAP_ORDER) )
        return NULL;

    //minimum size = 64 bytes = 2**6
    unsigned order = 6;
    while( (1<<order) < size )
        order++;

    unsigned i = order;
    while( i <= HEAP_ORDER && freeList[i] == NULL )
        i++;

    if( i > HEAP_ORDER )
        return NULL;        //not enough memory

    while( i > order ){
         splitBlock(i);
         i--;
    }

    Header* h = freeList[i];
    removeFirstNode(i);
    h->used=1;
    char* c = (char*) h;
    return c + sizeof(Header);
}


Header* getBuddy(Header* h){
    char* c = (char*) h;
    unsigned offset = c-heap;
    offset ^= (1<<h->order);
    c = heap + offset;
    return (Header*)c;
}

void kfree(void* v){
    char* c = (char*) v;
    Header* h = (Header*)(c-sizeof(Header));
    h->used = 0;
    prependNode(h);
    while(1){
        if( h->order == HEAP_ORDER )
            return;     //entire heap is free
        Header* b = getBuddy(h);
        if( b->used == 0 && b->order == h->order ){
            removeNode(h);
            removeNode(b);
            if(b<h)
                h=b;
            h->order++;
            prependNode(h);
        } else {
            return;
        }
    }
}

static struct PageTable kernel_page_table;

#define PAGE_DEVICE_MEMORY ( (1<<3) | (1<<4) )
#define PAGE_PRESENT 1
#define PAGE_MUST_BE_ONE (1<<7)
#define PAGE_USER_ACCESS (1<<2)
#define PAGE_WRITEABLE (1<<1)
#define MEGABYTE 1024*1024

void pageInit(struct MultibootInfo* info)
{
    for(int i = 0; i < 1024; i++){
        unsigned entry = (i << 22);
        entry |= PAGE_MUST_BE_ONE;

        unsigned address = i*MEGABYTE*4;

        if (address >= 0x00000000 && address < 0x00400000) {
            entry |= PAGE_PRESENT | PAGE_WRITEABLE;
        } 
        else if (address >= 0x00400000 && address < 0x00800000) {
            entry |= PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER_ACCESS;
        }
        else if (address >= 0x00800000 && address < 0x08000000) {
            entry |= PAGE_PRESENT | PAGE_WRITEABLE;
        }
        else if (address >= 0x08000000 && address < 0x80000000) {
            entry = 0;
        } 
        else if (address >= 0x80000000 && address <= 0xFFFFFFFF) {
            entry |= PAGE_PRESENT | PAGE_WRITEABLE | PAGE_DEVICE_MEMORY;
        }
        kernel_page_table.table[i] = entry;
    }
    set_page_table(&kernel_page_table);
    enable_paging();
}

void set_page_table(struct PageTable* p){
    asm volatile( "mov %%eax,%%cr3"
        :
        : "a"( (unsigned)(p->table) )
        : "memory" );
}
struct PageTable* get_page_table(){
    unsigned p;
    asm volatile( "mov %%cr3,%%eax"
        : "=a"(p)
    );
    return (struct PageTable*)p;
}

void enable_paging(){
    asm volatile(
        "mov %%cr4,%%eax\n"     //copy cr4 to eax
        "orl $16,%%eax\n"       //turn on bit 4
        "mov %%eax,%%cr4\n"     //copy eax back to cr4
        "mov %%cr0,%%eax\n"     //copy cr0 to eax
        "orl $0x80010000,%%eax\n"   //turn on bits 16 and 31
        "mov %%eax,%%cr0\n"     //copy eax back to cr0
        "jmp flushqueue%=\n"    //intel says we need this
        "flushqueue%=:\n"
        "nop"                   //no-op
        :                       //no outputs
        :                       //no inputs
        : "eax","memory"        //clobbers
    );
}

unsigned get_faulting_address(){
    unsigned addr;
    asm volatile(
        "mov %%cr2,%%eax\n"
        : "=a"(addr)
    );
    return addr;
}
