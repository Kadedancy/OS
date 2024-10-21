#include "fs.h"
#include "errno.h"
#include "utils.h"

void file_open_part_2(int errorcode,void* data,void* pFileOpenCallbackData,){
    struct FileOpenCallbackData* d = (struct FileOpenCallbackData*) pFileOpenCallbackData;
    if( errorcode != 0 ){
        file_table[d->fd].in_use=0;
        d->callback(errorcode, d->callback_data);
        kfree(d);
        return;
    }

    struct DirEntry* entries = (struct DirEntry*)data;
    char filenameBuffer[13];
    int match_found = 0;

    for (int i = 0; i < 128; i++) {
        if (entries[i].base[0] == 0x00) {
            break;
        }
        
        if ((unsigned char)entries[i].base[0] == 0xe5) {
            continue;
        }

        parseFilename(&entries[i], filenameBuffer);

        if (kstr(filenameBuffer, d->filename) == 0) {
            match_found = 1;

            file_table[d->fd].start_cluster = entries[i].firstCluster;
            file_table[d->fd].size = entries[i].size;

            d->callback(d->fd, d->callback_data);
            break;
        }
    }

    if (!match_found) {
        file_table[d->fd].in_use = 0;
        d->callback(ENOENT, d->callback_data);
    }
    kfree(d);
    
}

void file_open(const char* filename, int flags, file_open_callback_t callback, void* callback_data)
{
    if( kstrlen( filename ) >= MAX_PATH ){
        callback(ENOENT, callback_data); 
        return;
    }

    int i;
    int fd = i;
    for(i = 0; i < MAX_FILES; i++){
        if(file_table[i].in_use == 0){
            break;
        }
    }
    if(i == MAX_FILES){
        callback(EMFILE, callback_data);
        return;
    }
    file_table[fd].in_use=1;
    struct FileOpenCallbackData* d = (struct FileOpenCallbackData*) kmalloc(sizeof(struct FileOpenCallbackData));
    if(!d){
        file_table[fd].in_use=0;
        callback(ENOMEM, callback_data);
        return;
    }
    d->fd=fd;
    d->callback=callback;
    d->callback_data=callback_data;
    disk_read_sectors(clusterNumberToSectorNumber(2),vbr.sectors_per_cluster,file_open_part_2,d);
}

void file_close(int fd, file_close_callback_t callback,void* callback_data) {
    if( fd < MAX_FILES && fd >= 0 && file_table[fd].in_use){
        file_table[fd].in_use = 0;  // Mark file as closed
        callback(SUCCESS, callback_data); // Success
    } else {
        callback(EBADF, callback_data); // Invalid file descriptor
    }
}