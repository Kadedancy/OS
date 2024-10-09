#pragma once

#include "utils.h"
#include "interrupts.h"
#include "errno.h"
#include "pci.h"
#include "memory.h"

void disk_init();

#pragma pack(push,1)
struct PhysicalRegionDescriptor{
    void* address;
    u16 byteCount;
    u16 flags;       //0x8000=last one, 0x0000=not last
};

struct GUID {
    u32 time_low;
    u16 time_mid;
    u16 time_high_version;
    u16 clock;
    u8 node[6];
};
struct GPTEntry{
    struct GUID type;
    struct GUID guid;
    u32 firstSector;
    u32 firstSector64;
    u32 lastSector;
    u32 lastSector64;
    u32 attributes;
    u32 attributes64;
    u16 name[36];
};

struct VBR{
    char jmp[3];
    char oem[8];
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u16 reserved_sectors;
    u8 num_fats;
    u16 UNUSED_num_root_dir_entries;
    u16 UNUSED_num_sectors_small;
    u8 id ;
    u16 UNUSED_sectors_per_fat_12_16;
    u16 sectors_per_track;
    u16 num_heads;
    u32 first_sector;
    u32 num_sectors;
    u32 sectors_per_fat;
    u16 flags;
    u16 version;
    u32 root_cluster;
    u16 fsinfo_sector;
    u16 backup_boot_sector;
    char reservedField[12];
    u8 drive_number;
    u8 flags2;
    u8 signature;
    u32 serial_number;
    char label[11];
    char identifier[8];
    char code[420];
    u16 checksum;
};

#pragma pack(pop)

static struct PhysicalRegionDescriptor* physicalRegionDescriptor;

typedef void (*disk_callback_t)(int, void*, void*);
typedef void (*disk_metadata_callback_t)(void);

void disk_interrupt(struct InterruptContext* ctx);
void disk_read_sectors(
                    unsigned firstSector,
                    unsigned numSectors,
                    disk_callback_t callback,
                    void* callback_data);

void disk_read_metadata( disk_metadata_callback_t kmain_callback);
u32 clusterNumberToSectorNumber(u32 clustnum);
void do_this();

