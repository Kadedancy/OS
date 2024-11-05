#pragma once

#define MAX_PATH 64

struct File{
    int in_use;
    int flags;
    char filename[MAX_PATH+1];
    unsigned offset;
    unsigned size;
    unsigned firstCluster;
};

#define MAX_FILES 16        //real OS's use something like 1000 or so...

typedef void (*file_open_callback_t)(int fd, void* callback_data);
int file_open(const char* filename, int flags, file_open_callback_t callback, void* callback_data);

typedef void (*file_close_callback_t)( int errorcode, void* callback_data);
void file_close(int fd, file_close_callback_t callback, void* callback_data);

typedef void (*file_read_callback_t)(
        int ecode,          //error code
        void* buf,          //buffer with data
        unsigned numread,   //num read
        void* callback_data         //user data
);

void file_read( int fd,         //file to read from
                void* buf,      //buffer for data
                unsigned count, //capacity of buf
                file_read_callback_t callback,
                void* callback_data //passed to callback
);

typedef void (*file_write_callback_t)(
        int error_code,
        unsigned numwritten,    //num written
        void* callback_data    //user data
);

void file_write( int fd,         //file to read from
                void* buf,      //buffer for data
                unsigned count, //capacity of buf
                file_write_callback_t callback,
                void* callback_data //passed to callback
);

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

int file_seek(int fd, int delta, int whence);
int file_tell(int fd, unsigned* offset);