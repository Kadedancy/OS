/*#include "fs.h"
#include "errno.h"
#include "utils.h"
#include "memory.h"
#include "disk.h"

void file_open_part_2(int errorcode,void* data,void* pFileOpenCallbackData){
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

        if (kstrequal(filenameBuffer, d->filename) == 0) {
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
    int fd;
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

void file_read( int fd,         //file to read from
                void* buf,      //buffer for data
                unsigned count, //capacity of buf
                file_read_callback_t callback,
                void* callback_data )//passed to callback
{
    ...verify parameters, etc...
    struct ReadInfo* ri = kmalloc( sizeof(struct ReadInfo) );
    if(!ri){
        callback(ENOMEM, buf, 0, callback_data);
        return;
    }
    ri->fd = fd;
    ri->buffer = buf;
    ri->num_requested=count;
    ri->callback=callback;
    ri->callback_data=callback_data;
    unsigned secnum = clusterNumberToSectorNumber(
        file_table[fd].firstCluster
    );
    disk_read_sectors( secnum,
        vbr.sectors_per_cluster,
        file_read_part_2,
        ri
    );
}

void file_read_part_2(int errorcode,void* sector_data,void* callback_data)
{
    struct ReadInfo* ri = (struct ReadInfo*) callback_data;
    if( errorcode ){
        ri->callback( errorcode, ri->buffer, 0, ri->callback_data );
    } else {
        unsigned  offsetInBuffer = file_table[fd]->offset % 4096 ;
        unsigned numToCopy = bytesPerCluster - offsetInBuffer;
        numToCopy = min( numToCopy, ri->num_requested, file_table[fd]->size - file_table[fd]->offset);
        kmemcpy(ri->buffer,sector_data + offsetInBuffer, numToCopy);
        ri->callback(
            SUCCESS,
            ri->buffer,
            numToCopy,   
            ri->callback_data
        );
    }
    kfree(ri);      //important!
    //note: sector_data will be freed by disk_read_sectors
    //when we return to it from file_read_part_2
    return;
}

void file_write( int fd,         //file to read from
                void* buf,      //buffer for data
                unsigned count, //capacity of buf
                file_write_callback_t callback,
                void* callback_data //passed to callback
){
    //no such system call
    callback( ENOSYS, buf, callback_data );
}

int file_seek(int fd, int delta, int whence){

}*/

#include "fs.h"
#include "errno.h"
#include "disk.h"
#include "kprintf.h"
#include "utils.h"


#define MAX_PATH 64

struct File{
    int in_use;
    int flags;
    char filename[MAX_PATH+1];
};

#define MAX_FILES 16        //real OS's use something like 1000 or so...
struct File file_table[MAX_FILES];

struct FileOpenCallbackData {
    int fd;
    file_open_callback_t callback;
    void* callback_data;
};

struct longFilename {
    char filename[MAX_PATH];
    unsigned index;
    short endOfFilename;
};

static void file_open_part_2(int errorcode, void* data, void* pFileOpenCallbackData) {
    // Get the callback struct 
    struct FileOpenCallbackData* d = (struct FileOpenCallbackData*) pFileOpenCallbackData;

    // Check for any errors
    if( errorcode != 0 ){
        file_table[d->fd].in_use=0;
        d->callback(errorcode, d->callback_data);
        kfree(d);
        return;
    }

    struct DirEntry* de = (struct DirEntry*)data;
    int fileFound = 0;

    struct longFilename longFilename;
    for(unsigned i = 0; i < MAX_PATH; i++)
        longFilename.filename[i] = '\0';
    longFilename.index = 0;
    longFilename.endOfFilename = 0;

    // Get the desired filename and create a common casing
    char desiredFilename[MAX_PATH];
    kstrcpy(desiredFilename, file_table[d->fd].filename);
    for(unsigned i = 0; i < MAX_PATH; i++)
        desiredFilename[i] = tolower(desiredFilename[i]);

    // Iterate through the directory entries
    for(int i = 0; i < 128; i++) {
        // Check if the iteration is at the end of de
        if((int)de[i].base == 0x00)
            break;

        // Check if the iteration is on a deleted file
        if((unsigned char)de[i].base[0] == 0xe5)
            continue;

        // Skip long filenames
        if(de[i].attributes != 0x0f) {
            // Get the filename
            char filename[13];
            parseFilename(&de[i], filename);

            // Post process the filename
            for(unsigned i = 0; i < 13; i++)
                // Check if the current character is anything but a space
                if(filename[i] != ' ')
                    // Ensure the current letter is lower case
                    filename[i] = tolower(filename[i]);
                else
                    // Replace spaces with nulls
                    filename[i] = '\0';

            // Compare the requested filename
            if(kstrequal(filename, desiredFilename)) {
                fileFound = 1;
                break;
            }
        } else {
            // Construct the LFN entry
            struct LFNEntry* lfn = (struct LFNEntry*)&de[i];

            // Add the name backwards to the long filename
            for(int i = 3; i >= 0; i--) {
                // Skip the null character
                if(lfn->name2[i] != 0x0000 && lfn->name2[i] != -1)
                    longFilename.filename[longFilename.index++] = tolower(lfn->name2[i]);
                else if(lfn->name2[i] == 0x0000 || lfn->name2[i] == (char)0xffff)
                    longFilename.endOfFilename = 1;
            }

            for(int i = 11; i >= 0; i--) {
                // Skip the null character
                if(lfn->name1[i] != 0x0000 && lfn->name1[i] != -1)
                    longFilename.filename[longFilename.index++] = tolower(lfn->name1[i]);
                else if(lfn->name1[i] == 0x0000 || lfn->name1[i] == (char)0xffff)
                    longFilename.endOfFilename = 1;
            }

            for(int i = 9; i >= 0; i--) {
                // Skip the null character
                if(lfn->name0[i] != 0x0000 && lfn->name0[i] != -1)
                    longFilename.filename[longFilename.index++] = tolower(lfn->name0[i]);
                else if(lfn->name0[i] == 0x0000 || lfn->name0[i] == (char)0xffff)
                    longFilename.endOfFilename = 1;
            }
        }

        // Check if the longFilename needs to be compared to the desired filename
        if(longFilename.endOfFilename) {
            // Compare the long filename
            if(kstrequal(longFilename.filename, desiredFilename)) {
                fileFound = 1;
                break;
            }

            // Reset the longFilename
            for(unsigned i = 0; i < MAX_PATH; i++)
                longFilename.filename[i] = '\0';
            longFilename.index = 0;
            longFilename.endOfFilename = 0;
        }
    }

    if(fileFound){
        // File was found, call the callback using the file descriptor
        d->callback( d->fd, d->callback_data );
    } else {
        // File was not found, release the file descriptor calling callback with ENOENT
        kprintf("\nERROR: could not find!\n");
        file_table[d->fd].in_use=0;
        d->callback( ENOENT, d->callback_data );
    }

    // Free the callback
    kfree(d);
}

extern struct VBR vbr;

int file_open(const char* filename, int flags, file_open_callback_t callback, void* callback_data) {
    // Check the fileanme
    if(kstrlen(filename) >= MAX_PATH) {
        kprintf("\nERROR W LEN: %d\n", kstrlen(filename));
        callback(ENOENT, callback_data);
        return ENOENT;
    }

    int fd;

    // Look for File entry
    for(fd = 0; fd < MAX_FILES; fd++) {
        if(file_table[fd].in_use == 0)
            break;
    }

    // Check if there are no File entries
    if(fd == MAX_FILES) {
        callback(EMFILE, callback_data);
        return EMFILE;
    }

    // Mark the entry in use
    file_table[fd].in_use = 1;

    // Copy filename over
    kstrcpy(file_table[fd].filename, (char*)filename);

    // Copy over the callback struct
    struct FileOpenCallbackData* d = (struct FileOpenCallbackData*) kmalloc(
                    sizeof(struct FileOpenCallbackData)
    );

    // Call the callback with errors
    if(!d){
        file_table[fd].in_use=0;
        callback(ENOMEM, callback_data);
        return ENOMEM;
    }

    // Copy over the data on success
    d->fd = fd;
    d->callback = callback;
    d->callback_data = callback_data;
    disk_read_sectors(clusterNumberToSectorNumber(2), vbr.sectors_per_cluster, file_open_part_2, d);

    return SUCCESS;
}

void file_close(int fd, file_close_callback_t callback, void* callback_data) {
    // Check that the FD is valid
    if(!(fd >= 0 && fd < MAX_FILES)) {
        if(callback)
            callback(EINVAL, callback_data);
        
        return;
    }

    // Check that the FD is in use
    if(!(file_table[fd].in_use)) {
        if(callback)
            callback(EINVAL, callback_data);

        return;
    }

    file_table[fd].in_use = 0;
    if(callback)
        callback(SUCCESS, callback_data);
}