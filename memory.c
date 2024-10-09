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