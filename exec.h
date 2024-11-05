/*
#pragma pack(push,1)
struct DOSHeader{
    char magic[2];          //'MZ'
    u16 lastsize;
    u16 blockCount;
    u16 numRelocations;
    u16 headerSize;
    u16 minimumAlloc;
    u16 maximumAlloc;
    u16 stackSegment;
    u16 stackPointer;
    u16 checksum;
    u16 entryPoint;         //for DOS stub
    u16 codeSegment;
    u16 relocation;
    u16 numOverlays;
    char reserved[8];
    u16 oemID;
    u16 oemInfo;
    char reserved2[20];
    u32 peOffset;           //we need this
};
#pragma pack(pop)

#pragma pack(push,1)
struct PEHeader{
    char magic[4];
    struct COFFHeader coffHeader;
    struct OptionalHeader optionalHeader;
};
#pragma pack(pop)

#pragma pack(push,1)
struct COFFHeader{
     u16 machineType; //0x14c=i386, 0x8664=x64,
                      //0x1c0=arm32, 0xaa64=arm64
     u16 numSections;
     u32 time;
     u32 symbolTableStart;
     u32 numSymbols;
     u16 optionalHeaderSize;
     u16 flags;               //bit 2=exe, bit 14=dll
};
#pragma pack(pop)

#pragma pack(push,1)
struct OptionalHeader{
    u16 magic;             //0x010b=32 bit, 0x020b=64 bit
    u16 linkerVersion;
    u32 codeSize;
    u32 dataSize;
    u32 bssSize;
    u32 entryPoint;        //relative to imageBase
    u32 codeBase;
    u32 dataBase;
    u32 imageBase;         //where exe should be loaded
    u32 sectionAlign;
    u32 fileAlign;
    u32 osVersion;
    u32 imageVersion;
    u32 subsystemVersion;
    u32 winVersion;
    u32 imageSize;
    u32 headerSize;
    u32 checksum;
    u16 subsystem;          //0=unknown, 1=native,
                            //2=gui, 3=console, 7=posix
    u16 dllCharacteristics;
    u32 stackReserve;
    u32 stackCommit;
    u32 heapReserve;
    u32 heapCommit;
    u32 loadFlags;
    u32 numDirectories;     //always 16
    struct PEDirectory directories[16];
};
#pragma pack(pop)

#pragma pack(push,1)
struct PEDirectory{
    u32 offset;
    u32 size;
};
#pragma pack(pop)

#pragma pack(push,1)
struct SectionHeader{
    char name[8];
    u32 sizeInRAM;     //size in RAM
    u32 address;       //offset of section from imageBase in RAM
    u32 sizeOnDisk;    //size of data on disk
    u32 dataPointer;   //offset from start of file to first page
    u32 relocationPointer; //file pointer for relocation entries
    u32 lineNumberPointer;
    u16 numRelocations;
    u16 numLineNumbers;
    u32 characteristics;  //flag bits (next slide)
};
#pragma pack(pop)

typedef void (*exec_callback_t)(int errorcode, unsigned entryPoint, void* callback_data);
void exec( const char* fname, unsigned loadAddress, exec_callback_t callback, void* callback_data );
void file_read_fully(int fd, void* buf, unsigned count,file_read_callback_t callback,void* callback_data);
*/