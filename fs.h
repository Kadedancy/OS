#pragma once

#define MAX_FILES 16           
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
};

struct FileOpenCallbackData{
    int fd;             //index in file_table
    file_open_callback_t callback;  //was passed into file_open
    void* callback_data;     //was passed into file_open
};

struct File file_table[MAX_FILES];
typedef void (*file_open_callback_t)(int fd, void* callback_data);
typedef void (*file_close_callback_t)( int errorcode, void* callback_data);
void file_open(const char* filename, int flags,file_open_callback_t callback, void* callback_data);
void file_close(int fd, file_close_callback_t callback,void* callback_data);