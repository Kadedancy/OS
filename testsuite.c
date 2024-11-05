//2024-nov-02

//change this to 1 to get lots of debugging messages
#define VERBOSE 1




#include "kprintf.h"
#include "file.h"

#if VERBOSE
#define vprintf kprintf
#else
static void vprintf(const char* fmt, ... ){
}
#endif

static const char article6[] = {
    "Article. VI.\n\n"
    "All Debts contracted and Engagements entered into, before the Adoption "
    "of this Constitution, shall be as valid against the United States under "
    "this Constitution, as under the Confederation.\n\n"
    "This Constitution, and the Laws of the United States which shall be made "
    "in Pursuance thereof; and all Treaties made, or which shall be made, "
    "under the Authority of the United States, shall be the supreme Law of the "
    "Land; and the Judges in every State shall be bound thereby, any Thing in the "
    "Constitution or Laws of any State to the Contrary notwithstanding.\n\n"
    "The Senators and Representatives before mentioned, and the Members of "
    "the several State Legislatures, and all executive and judicial Officers, "
    "both of the United States and of the several States, shall be bound by Oath or "
    "Affirmation, to support this Constitution; but no religious Test shall ever "
    "be required as a Qualification to any Office or public Trust under the United States." };

static const char article5[] = {
    "Article. V.\n\n"
    "The Congress, whenever two thirds of both Houses shall deem it necessary, "
    "shall propose Amendments to this Constitution, or, on the Application of the "
    "Legislatures of two thirds of the several States, shall call a Convention "
    "for proposing Amendments, which, in either Case, shall be valid to all Intents "
    "and Purposes, as Part of this Constitution, when ratified by the Legislatures "
    "of three fourths of the several States, or by Conventions in three fourths thereof, "
    "as the one or the other Mode of Ratification may be proposed by the Congress; "
    "Provided that no Amendment which may be made prior to the Year One thousand eight "
    "hundred and eight shall in any Manner affect the first and fourth Clauses "
    "in the Ninth Section of the first Article; and that no State, without its Consent, "
    "shall be deprived of its equal Suffrage in the Senate."
};

struct Seeks{
    int delta;
    int whence;
};

#define canary 0x45
static struct Seeks seeks[] = {
    {3,0},
    {14,0},
    {4096,0},
    {159,0},
    {4095,0},
    {-2010,0},
    {26,0},
    {-1,0},
    {53,0},
    {5,0},
    {89,0},
    {5000,0},
    {20000,0},
    {79,0},
    {323,0},
    {0,0},
    {4097,0},
    {sizeof(article5),0},
    {52, 0},
    {sizeof(article5)-1,0},
    {sizeof(article5)-2,0},
    {3, 2},
    {14,2},
    {159,2},
    {20000,2},
    {5020,2},
    {1,2},
    {4096,2},
    {409,2},
    {4095,2},
    {20000,2},
    {100,2},
    {4097,2},
    {100,2},
    {5906,2},
    {0,2},
    {16,2},
    {(int)sizeof(article5),2},
    {20,2},
    {(int)sizeof(article5)+1,2},
    {50,2},
    {(int)sizeof(article5)-1,2},
    {-(int)(sizeof(article5)-4),2},
    {-5,2},
    {77,2},
    {(int)sizeof(article5)-2,2},
    {-(int)(sizeof(article5)-2),2},
    {-10,2},
    {-5,2},
    {26,0},
    {-1,2},
    {-10,1},
    {26,0},
    {50,1},
    {-13,1},
    {1000,1},
    {26,0},
    {-4000,1},
    {26,0},
    {4000,1},
    {26,0},
    {-4099,1},
    {26,0},
    {4099,1},
    {26,0},
    { 0, 1},
    {26,0},
    {-1, 1},
    { 20000, 1},
    {-20000, 1},
    {26,0},
    {-20,1},
    {20,1},
    {-4000,2},
    {0x7fffffff,0},     //2G
    {0x7fffffff,1},     //just short of 4G
    {0x7fffffff,1},     //overflow; should be invalid
    {0,3},            //invalid whence
    {-1,-1}
};


//~ static void memdump(void* p, int count){
    //~ const int charsPerLine=16;
    //~ unsigned char* c = (unsigned char*) p;
    //~ while( count > 0 ){
        //~ int i;
        //~ for(i=0; i<charsPerLine && i < count;++i){
            //~ kprintf("%02x ",c[i]);
        //~ }
        //~ for( ; i<charsPerLine; ++i ){
            //~ kprintf("   ");
        //~ }
        //~ kprintf(" | ");
        //~ for(i=0; i<charsPerLine && i < count;++i){
            //~ if( c[i] >= 32 && c[i] <= 126 )
                //~ kprintf("%c",c[i]);
            //~ else
                //~ kprintf(".");
        //~ }
        //~ kprintf("\n");
        //~ count-=charsPerLine;
        //~ c += charsPerLine;
    //~ }
//~ }


static unsigned mystrlen(const char* s){
    unsigned len=0;
    while(*s){
        len++;
        s++;
    }
    return len;
}

static int article6fd1;
static int article5fd;
static int article6fd2;
#define BUFFSIZE 1024
static char a6a[BUFFSIZE];
static char a6b[BUFFSIZE];
static char a5[BUFFSIZE];
static void f1(int fd, void* callback_data);
static void f4( int errorcode, void* buf, unsigned numread, void* data);
static void doNextSeekTest();
static void f5( int errorcode, void* vbuf, unsigned nr, void* data);
static void f6();
static char* p[3];
static int fds[3];          //article 6a, article5, article6b
static unsigned count[3];
#define readsize 27
static int readsLeft;
static int atEOF[3];
static int seekTest;
static int filesize[3];
static void initiateOverlappingReads();
static long long int currentOffset;

static void freeze(){
    while(1){
        __asm__("cli");
        __asm__("hlt");
    }
}

unsigned a6len;
unsigned a5len;

//this function opens article 6 and calls f1
void sweet(){

    vprintf("Beginning sweet()\n");

    a6len = mystrlen(article6);
    a5len = mystrlen(article5);
    seekTest=-1;
    currentOffset = a5len;

    if( a6len != 949 ){
        kprintf("testsuite line %d: article6 had wrong length: %d\n", __LINE__, a6len );
        freeze();
    }
    if( a5len != 850 ){
        kprintf("testsuite line %d: article5 had wrong length: %d\n", __LINE__, a5len );
        freeze();
    }

    vprintf("testsuite: Opening article6.txt\n");
    file_open("article6.txt", 0, f1, 0);
}

unsigned savedflags[16];
int savedflagsctr=0;

static void lock(){
    unsigned eflags;
    asm("pushf\npop %%eax" : "=a"(eflags));
    __asm__ volatile("cli");
    savedflags[savedflagsctr++] = eflags;
}
static void unlock(){
    unsigned eflags = savedflags[--savedflagsctr];
    asm("push %%eax\npopf" : : "a"(eflags));
}


static void f2(int fd, void* x);

//open article5 and call f2
static void f1(int fd, void* callback_data)
{

    article6fd1 = fd;

    vprintf("testsuite: Got descriptor %d for article6.txt\n",article6fd1);

    if( article6fd1 < 0 ){
        kprintf("testsuite line %d: Negative file descriptor from article6.txt\n",__LINE__);
        freeze();
    }

    vprintf("testsuite: Opening article5.txt\n");
    file_open("article5.txt",0,f2,0);
}

static void f3(int fd, void* x);


//open article6 and call f3
static void f2(int fd, void* x){
    article5fd = fd;
    if( article5fd < 0 ){
        kprintf("testsuite line %d: Could not open article5.txt: %d\n",__LINE__,article5fd);
        freeze();
    }
    else{
        vprintf("testsuite: Got descriptor for %d article5.txt\n",article5fd);
    }

    vprintf("testsuite: Opening article6.txt a second time\n");
    file_open("article6.txt",0,f3,0);
}


//begin reading the three files
static void f3(int fd, void* x){


    article6fd2 = fd;
    if( article6fd2 < 0 ){
        kprintf("testsuite line %d: Could not open article6.txt a second time: %d\n",
            __LINE__, article6fd2);
        freeze();
    }

    vprintf("testsuite: Got descriptor for %d article6.txt (second time)\n",article6fd2);

    if( article6fd1 == article6fd2 || article6fd1 == article5fd || article6fd2 == article5fd ){
        kprintf("testsuite line %d: File descriptors should all be different, but they are not\n", __LINE__);
        freeze();
    }

    p[0] = a6a;
    p[1] = a5;
    p[2] = a6b;
    fds[0] = article6fd1;
    fds[1] = article5fd;
    fds[2] = article6fd2;
    filesize[0] = a6len;
    filesize[1] = a5len;
    filesize[2] = a6len;
    count[0]=0;
    count[1]=0;
    count[2]=0;
    atEOF[0]=0;
    atEOF[1]=0;
    atEOF[2]=0;


    initiateOverlappingReads();

}

void f4AllAtEOF();

static void initiateOverlappingReads()
{
    //fire off three overlapping reads

    lock();

    readsLeft=3;

    for(int i=0;i<3;++i){
        int fd = fds[i];
        if( count[i] + readsize + 1 >= BUFFSIZE ){
            kprintf("Buffer overflow\n");
            while(1){ __asm__("hlt");}
        }
        p[i][count[i]+readsize+1] = canary;
        vprintf("Calling file_read( %d, %p, %d)\n",fd,p[i]+count[i],readsize);
        file_read(fd, p[i]+count[i], readsize, f4, (void*) i);
    }
    unlock();
}

static int QUANTUM=8;


static void dumpq(const char* b, int bufferlength)
{
    for(int j=0;j<QUANTUM ;++j){
        if( j >= bufferlength )
            kprintf("   ");
        else {
            char c = b[j];
            kprintf("%02x ", (unsigned)(c) & 0xff );
        }
    }
    kprintf(" | ");
    for(int j=0;j<QUANTUM ;++j){
        if( j >= bufferlength ){
            kprintf(" ");
        } else {
            char c = b[j];
            if( c >= 32 && c <= 126 ){
                kprintf("%c", c);
            } else {
                kprintf(".");
            }
        }
    }
    return;
}

static void dumpComparison(const char* b1, const char* b2, int offset, int bufferlength){
    b1 += offset;
    b2 += offset;
    while(bufferlength > 0 ){
        kprintf("%05d: ",offset);
        dumpq(b1,bufferlength);
        if( b2 ){
            kprintf(" || ");
            dumpq(b2,bufferlength);
        }
        kprintf("\n");
        b1 += QUANTUM;
        if(b2)
            b2 += QUANTUM;
        bufferlength -= QUANTUM;
        offset += QUANTUM;
    }
}


static void compareFiles(const char* buffer1, const char* buffer2, int length, const char* name, int fd)
{
    for(int i=0;i<length;i+=QUANTUM){
        for(int j=0;j<QUANTUM && i+j < length;++j){
            if( buffer1[i+j] != buffer2[i+j] ){

                int start = i - 30;
                if(start<0)
                    start=0;
                int end=i+30;
                if( end > length )
                    end=length;
                dumpComparison(buffer1,buffer2,start,end-start);
                kprintf("\ntestsuite line %d: Wrong contents in %s at buffer index %d from fd %d\n",__LINE__, name,i+j,fd);
                freeze();
            }
        }
    }
}



static void f4( int errorcode, void* buf, unsigned numread, void* data)
{
    int i = (int)data;
    int fd = fds[i];

    if( errorcode != 0 ){
        kprintf("testsuite line %d: Read error for fd %d: %d\n",__LINE__,fd,errorcode);
        freeze();
        return;
    }

    readsLeft--;

    vprintf("testsuite line %d: Got %d bytes from fd %d\n",__LINE__,numread,fd);

    //if we've hit eof, increment the count


    count[i] += numread;
    vprintf("Have read total of %d bytes from fd %d\n",count[i],fd);

    if( numread == 0 ){
        atEOF[i]=1;
        vprintf("fd %d is at EOF\n",fd);
    }

    if( numread > readsize ){
        kprintf("testsuite line %d: Too much data read: Got %d, expected %d or less\n",__LINE__,numread,readsize);
        freeze();
    }

    if( count[i] > filesize[i] ){
        kprintf("testsuite line %d: Did not reach EOF in time: fd %d gave us %d bytes but file has %d bytes\n",
            __LINE__, fds[i], count[i], filesize[i]
        );
        freeze();
    }

    //if there are outstanding reads, wait on them
    if( readsLeft > 0 ){
        return;
    }

    //if we get here, all three reads have completed

    vprintf("Three reads have completed\n");

    //if we haven't reached EOF on all files, dispatch again
    if( !atEOF[0] || !atEOF[1] || !atEOF[2] ){
        initiateOverlappingReads();
        return;
    }

    //if we get here, all three files have reached EOF

    f4AllAtEOF();
}

void f4AllAtEOF()
{


    if( count[0] != a6len ){
        kprintf("testsuite line %d: Read wrong amount of data for article6.txt #1 (fd %d): Got %d, expected %d\n", __LINE__,fds[0], count[0], a6len );
        freeze();
    }
    if( count[1] != a5len ){
        kprintf("testsuite line %d: Read wrong amount of data for article5.txt (fd %d): Got %d, expected %d\n",__LINE__, fds[1], count[1], a5len );
        freeze();
    }
    if( count[2] != a6len ){
        kprintf("testsuite line %d: Read wrong amount of data for article6.txt #2 (fd %d): Got %d, expected %d\n",__LINE__, fds[2], count[2], a6len );
        freeze();
    }


    kprintf("testsuite line %d: Comparing results of three reads...\n",__LINE__);

    compareFiles( a6a, article6, a6len, "article6.txt", fds[0]);
    compareFiles( a6b, article6, a6len, "article6.txt", fds[2]);
    compareFiles( a5, article5, a5len, "article5.txt", fds[1]);


    kprintf("testsuite line %d: All three files are OK\n",__LINE__);

    kprintf("testsuite line %d: Beginning seek test...\n",__LINE__);

    int fd = article5fd;
    if( file_seek(fd,0,2) != 0 ){
        kprintf("testsuite line %d: file_seek reported error\n",__LINE__);
        freeze();
    }
    unsigned size;
    if( file_tell( fd, &size ) != 0 ){
        kprintf("testsuite line %d: file_tell reported error\n",__LINE__);
        freeze();
    }
    if( size != a5len ){
        kprintf("testsuite line %d: Seek to end: Reported size as %d but expected %d\n", __LINE__,size,a5len);
        freeze();
    }

    doNextSeekTest();
}

static void doNextSeekTest()
{
    ++seekTest;
    int fd = fds[1];    //article 5

    int i = seekTest;
    if( seeks[i].whence == -1 ){
        //done
        f6();
        return;
    }

    int delta = seeks[i].delta;
    int whence = seeks[i].whence;
    int rv = file_seek(fd,delta,whence);
    vprintf("testsuite: #%d: file_seek(%d, %d, %d) -> %d\n", i, fd,delta,whence,rv);


    long long int tempOffset;
    if( whence == 0 )
        tempOffset = delta;
    else if( whence == 1 ){
        tempOffset = currentOffset;
        tempOffset += delta;
    } else if( whence == 2 ){
        tempOffset = a5len;
        tempOffset += delta;
    } else {
        tempOffset = -1000;
    }

    if( tempOffset > 0xffffffffLL )
        tempOffset = -1;

    if( tempOffset < 0 ){
        if( rv == 0 ){
            kprintf("testsuite line %d: At offset %lld: Did file_seek(%d, %d, %d) and got success, but should have gotten error\n",
                __LINE__, currentOffset, fd,delta,whence
            );
            freeze();
        } else {
            //nothing to do; don't adjust expected offset
        }
    } else {
        if( rv != 0 ){
            kprintf("testsuite line %d: At offset %lld: Did file_seek(%d, %d, %d) and got error %d\n",
                __LINE__, currentOffset, fd,delta,whence, rv );
            freeze();
        } else {
            currentOffset = tempOffset;
        }
    }

    unsigned newOffset=0x12345;
    if( file_tell(fd,&newOffset) != 0 ){
        kprintf("testsuite line %d: file_tell returned error\n",__LINE__);
        return;
    }

    if( newOffset != currentOffset ){
        kprintf("testsuite line %d: file_tell gave incorrect offset: Got %d, expected %lld\n",__LINE__,
            newOffset,currentOffset);
        freeze();
    }
    static char buf[32];
    for(int i=0;i<sizeof(buf);i++){
        buf[i] = 0x42;
    }
    vprintf("testsuite: file_read(%d,0x%p,%d)\n",fd,buf,23);
    file_read(fd,buf,23,f5,0);
}

static void f5( int errorcode, void* vbuf, unsigned nr, void* data)
{
    char* buf = (char*)vbuf;

    if( errorcode){
        kprintf("testsuite line %d: After seeking, file_read gave error %d\n",__LINE__,errorcode);
        freeze();
    }

    vprintf("testsuite: num read=%d\n",nr);

    if( nr > 23 ){
        kprintf("testsuite line %d: file_read: Read too much: Read %d but max is 23\n",__LINE__,
            nr );
        freeze();
    }

    unsigned maxData;
    if( currentOffset >= a5len )
        maxData = 0;
    else
        maxData = a5len-currentOffset;

    if( maxData > 23 )
        maxData = 23;

    if( nr > maxData ){
        kprintf("testsuite line %d: file_read: Went past EOF\n",__LINE__);
        freeze();
    }

    for(int i=23;i<32;++i){
        if( buf[i] != 0x42 ){
            kprintf("buf[%d] is %d\n",i,buf[i]);

            dumpComparison(buf,0,0,32);
            kprintf("testsuite line %d: When reading at offset %lld: file_read touched byte %d but it should not have gone past index 22\n",__LINE__,
                currentOffset,
                i);
            freeze();
        }
    }

    compareFiles(buf, article5+currentOffset, maxData, "article5.txt", fds[1] );
    currentOffset += nr;
    doNextSeekTest();
}


static void f6(){
    if( file_tell( fds[1], 0 ) >= 0 ){
        kprintf("testsuite line %d: file_tell didn't catch bad pointer\n",__LINE__);
        return;
    }

    kprintf("\n\n");

    //!!
    kprintf("All OK!\n");
    //!!

}
