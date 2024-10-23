#pragma once

/*#define MAX_FILES 16           
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2
#define O_CREAT  64
#define O_EXCL   128
#define O_TRUNC  512
#define O_APPEND 1024
#define MAX_PATH 64

struct File{
    int in_use;
    int flags;
    char filename[MAX_PATH+1];
    unsigned offset;
    unsigned size;
    unsigned firstCluster;
};

struct FileOpenCallbackData{
    int fd;             //index in file_table
    file_open_callback_t callback;  //was passed into file_open
    void* callback_data;     //was passed into file_open
};

typedef void (*file_read_callback_t)(
        int ecode,          //error code
        void* buf,          //buffer with data
        unsigned numread,   //num read
        void* callback_data         //user data
);

typedef void (*file_write_callback_t)(
        int error_code,
        unsigned numwritten,    //num written
        void* callback_data,    //user data
);


struct ReadInfo{
    int fd;
    void* buffer;
    unsigned num_requested;     //amount of data requested
    file_read_callback_t callback;
    void* callback_data;
};

struct File file_table[MAX_FILES];
typedef void (*file_open_callback_t)(int fd, void* callback_data);
typedef void (*file_close_callback_t)( int errorcode, void* callback_data);

void file_read( int fd,         //file to read from
                void* buf,      //buffer for data
                unsigned count, //capacity of buf
                file_read_callback_t callback,
                void* callback_data //passed to callback
);

void file_open(const char* filename, int flags,file_open_callback_t callback, void* callback_data);
void file_close(int fd, file_close_callback_t callback,void* callback_data);

void file_write( int fd,         //file to read from
                void* buf,      //buffer for data
                unsigned count, //capacity of buf
                file_write_callback_t callback,
                void* callback_data //passed to callback
);
int file_seek(int fd, int delta, int whence);
int file_tell(int fd, unsigned* offset);
*/

typedef void (*file_open_callback_t)(int fd, void* callback_data);
int file_open(const char* filename, int flags, file_open_callback_t callback, void* callback_data);

typedef void (*file_close_callback_t)( int errorcode, void* callback_data);
void file_close(int fd, file_close_callback_t callback, void* callback_data);