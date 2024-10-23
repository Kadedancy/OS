

#include "errno.h"
#include "kprintf.h"
#include "file.h"

#ifndef REMOVED
#define REMOVED "ARTICLE3.TXT"
#endif

#ifndef MAGICNUMBER
#define MAGICNUMBER ""
#endif


static const char* nonexist[] = {
        "blargh", "blargleblargleboom",
        "blargleblargleboom.burpblah", "b.burpblah",
        "b.b", "art.txt","article1234.txt",
        "article 2.TXT",
        "article1.T","article1txt",
        "article1.txtt", "bill..txt",
        "article10.txt", "article100.txt",
        "article1.txts","bills.txt",
        "bill    .txt", "bill    txt",
        "const.c ","const.c  ","const.c.",
        "const.c..","const.c...",
        "thisisareallylongfilenamebutitshouldnotcrashyouroperatingsystemifyourcodeiscorrectlywrittenandrobustlydesigned.c",
        "x", "", ".txt",
        "article1.c", "bill.z",
        "foo.txt$",
        0};

static const char* exist[] = {
    "article1.txt", "ARTICLE2.txt",
    "ARTICLE3.TXT", "ArTiClE4.txt",
    "article5.TXT", "bill.txt",
    "foo$.t$",
    "foo.txt",
    "CONST.C", 0
};

typedef struct Test {
    char explanation[80];
    void* func;
    const char* fname;
    int fd;
    int shouldSucceed;
} Test;

#define NUM_TESTS 200
static int currentTest=0;
static int numTests=0;
static Test tests[NUM_TESTS];

#define MAX_USED 16
int used[MAX_USED];           //file descriptors that are in-use

#ifndef NULL
#define NULL ((void*)0)
#endif

static void dispatchCurrentTest();
static void dispatchNextTest();

static void openCallback(int fd, void* data)
{
    Test* T = (Test*) data;
    if( fd < 0 ){
        kprintf("Failed with code %d ",fd);
        if( T->shouldSucceed ){
            kprintf("\n\nIncorrect: Stopping.\n");
            return;
        } else {
            kprintf("[OK]\n");
        }
    } else {
        kprintf("Succeeded with fd %d ",fd);

        int i;

        for(i=0;i<MAX_USED;++i){
            if( used[i] == fd ){
                kprintf("Duplicate descriptor! [BAD]\n");
                return;
            }
        }

        for(i=0;i<MAX_USED;++i){
            if( used[i] == -1 ){
                used[i] = fd;
                break;
            }
        }
        if( i == MAX_USED ){
            kprintf("\nToo many open's succeeded. Test harness error?\n");
            return;
        }

        if ( T->shouldSucceed ){
            kprintf("[OK]\n");
        } else {
            kprintf("\n\nIncorrect: Stopping\n");
            return;
        }
    }

    dispatchNextTest();
}

static void closeCallback(int errorcode, void* data)
{
    Test* T = (Test*) data;

    if( errorcode ){
        kprintf("Failed with code %d ",errorcode);
        if( T->shouldSucceed ){
            kprintf("\n\nIncorrect: Stopping.\n");
            return;
        } else {
            kprintf("[OK]\n");
        }
    } else {
        kprintf("Succeeded ");
        if( T->shouldSucceed ){
            int i;
            for(i=0;i<MAX_USED;++i){
                if( used[i] == T->fd ){
                    used[i] = -1;
                    break;
                }
            }
            if( i == MAX_USED ){
                kprintf("Internal test harness error at line %d\n", __LINE__ );
                return;
            }
            kprintf("[OK]\n");
        } else {
            kprintf("\n\nIncorrect: Stopping\n");
            return;
        }
    }

    dispatchNextTest();

}


static void closeFileIfOpen(int errorcode, void* data)
{
    Test* T = (Test*) data;

    if( T->shouldSucceed ){

        //this is the first time we try to close
        //this file. Don't remove from the used list
        //since we're going to try again later
        //to close that same descriptor in order to
        //verify we get an error.

        if( errorcode == SUCCESS ){

            kprintf("Succeeded [OK]\n");
        } else {
            kprintf("Failed with code %d ",errorcode);
            kprintf("\n\nIncorrect: Stopping.\n");
            return;
        }

    } else {
        //this is the second time we try to close this
        //file. This time, we do remove it from the used
        //list. Note that T->fd is actually the
        //index in used[], not the fd itself.
        if( errorcode == SUCCESS ){
            kprintf("Succeeded\n\nIncorrect: Stopping\n");
            return;
        } else {
            used[ T->fd ] = -1;
            kprintf("Failed with code %d [OK]\n",errorcode);
        }
    }

    dispatchNextTest();

}

unsigned getesp(){
    unsigned esp;
    __asm__( "mov %%esp,%%eax" : "=a"(esp) );
    return esp;
}

static void exhaustionTest( int fd, void* data)
{
    Test* T = (Test*) data;
    T->fd ++;       //num that we did
    if( fd >= 0 ){
        T->shouldSucceed++; //num that succeeded
        dispatchCurrentTest();
    }
    else {
        if( T->shouldSucceed > 15 ){
            kprintf("Exhaustion test succeeded\n");
            dispatchNextTest();
            return;
        }
    }
}

static void dispatchNextTest(){
    currentTest++;
    if( currentTest >= NUM_TESTS ){
        //done
        return;
    }
    dispatchCurrentTest();
}
static void dispatchCurrentTest(){


    Test* T = &(tests[currentTest]);
    if( T->explanation[0] ){
        kprintf("%s", T->explanation);
    }

    if( T->func == NULL ){
        //ignore and go on to next one
        dispatchNextTest();
        return;
    }

    int idx = T - tests;
    kprintf("Running test %d...\n",idx);


    if( T->func == file_open ){
        kprintf("Trying to open '%s' ", T->fname);
        if( T->shouldSucceed )
            kprintf("(should succeed)");
        else
            kprintf("(should fail)");
        kprintf(" ... ");

        file_open( T->fname,0, openCallback, T );
        return;
    } else if( T->func == file_close ){
        kprintf("Closing fd %d ", T->fd);
        if( T->shouldSucceed )
            kprintf("[should succeed]");
        else
            kprintf("[should fail]");
        kprintf(" ... ");

        file_close( T->fd, closeCallback, T );
        return;
    } else if( T->func == closeFileIfOpen ){

        //T->fd is actually the *index* in used[],
        //not the file descriptor itself
        int fd = used[ T->fd ];
        if( fd == -1 ){
            //go on to the next test
            dispatchNextTest();
            return;
        }

        kprintf("Closing fd %d ", fd);
        if( T->shouldSucceed )
            kprintf("[should succeed]");
        else
            kprintf("[should fail]");
        kprintf(" ... ");

        file_close( fd, closeFileIfOpen, T );
        return;
    } else if( T->func == exhaustionTest ){
        file_open("ARTICLE1.TXT",0,exhaustionTest,T);
        return;
    } else {
        kprintf("Internal test harness error at line %d\n",__LINE__);
        return;
    }
}

static void scopy(char* d, const char* s ){
    while( ( (*d++) = (*s++) ) )
        ;
}

static int up(char x){
    if( x >= 'a' && x <= 'z' )
        return x-32;
    else
        return x;
}

static int scmp(const char* a, const char* b){
    while(1){
        if( !*a || !*b )
            break;
        if( up(*a) != up(*b) )
            return 0;
        ++a;
        ++b;
    }
    if( *a == 0 && *b == 0 )
        return 1;
    else
        return 0;
}


extern void panic(const char*);

void sweet(){

    kprintf("In sweet()\n");
    if( currentTest != 0 )
        panic("currentTest is not zero: Buffer overflow?");


    //can set this to a larger value to start in the middle of the tests
    currentTest=0;

    int ti=0;
    for(int j=0;j<16;++j)
        used[j] = -1;

    scopy( tests[ti++].explanation , "==================================\n");
    scopy( tests[ti++].explanation , "Testing nonexistent file open...\n"  );
    scopy( tests[ti++].explanation , "==================================\n");

    for(int i=0;nonexist[i];++i){
        //~ int fd = file_open(nonexist[i],0);
        tests[ti].func = file_open;
        tests[ti].fname = nonexist[i];
        tests[ti].shouldSucceed = 0;
        ti++;
    }


    scopy( tests[ti++].explanation , "==================================\n");
    scopy( tests[ti++].explanation , "Testing close of non-open files\n"   );
    scopy( tests[ti++].explanation , "==================================\n");

    for(int i=-4;i<10;++i){
        tests[ti].func = file_close;
        tests[ti].fd = i;
        tests[ti].shouldSucceed = 0;
        ti++;
    }


    scopy( tests[ti++].explanation , "==================================\n");
    scopy( tests[ti++].explanation , "Testing open of existing files\n"    );
    scopy( tests[ti++].explanation , "==================================\n");

    for(int i=0;exist[i];++i){
        tests[ti].func = file_open;
        tests[ti].fname = exist[i];
        tests[ti].shouldSucceed = !scmp(REMOVED,exist[i]);
        ti++;
    }


    scopy( tests[ti++].explanation , "==================================\n");
    scopy( tests[ti++].explanation , "Testing close of opened files\n"     );
    scopy( tests[ti++].explanation , "==================================\n");


    for(int i=15;i>=0;i--){
        tests[ti].func = closeFileIfOpen;
        tests[ti].fd = i;       //want index, not fd
        tests[ti].shouldSucceed = 1;
        ti++;
    }

    for(int i=15;i>=0;i--){
        tests[ti].func = closeFileIfOpen;
        tests[ti].fd = i;   //want index, not fd
        tests[ti].shouldSucceed = 0;
        ti++;
    }


    scopy( tests[ti++].explanation , "==================================\n");
    scopy( tests[ti++].explanation , "Exhaustion test...\n"                );
    scopy( tests[ti++].explanation , "==================================\n");

    tests[ti].func = exhaustionTest;
    tests[ti].shouldSucceed=0;          //num that succeeded
    tests[ti].fd = 0;               //num done
    ti++;

    scopy( tests[ti++].explanation , "\n\nAll OK! " MAGICNUMBER "\n\n");

    numTests = ti;
    kprintf("%d tests\n",numTests);

    dispatchCurrentTest();

}
