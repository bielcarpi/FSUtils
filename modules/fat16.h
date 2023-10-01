#ifndef FAT16_H
#define FAT16_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FAT16_PRINT_INFO "\n------ Filesystem Information ------\n\nFilesystem: FAT16\n\nSystem name: %s\nSector Size: %d\nSectors per cluster: %d\nReserved sectors: %d\n# of FATs: %d\nMax root entries: %d\nSector per FAT: %d\nLabel: %s\n\n"

typedef struct {
    char long_name[8];
    char extension[3];
    uint8_t fileAttr;
    uint8_t reserved[10];
    uint16_t tChange;
    uint16_t dChange;
    uint16_t firstCluster;
    uint32_t fSize;
} FatDirectoryEntry;

/******************************** SUPERBLOCK information related ********************************/
typedef struct {
    char BS_oemName[8];             //A useless name string
    uint16_t BPB_bytsPerSec;        //Bytes per sector
    uint8_t BPB_secPerClus;         //Sectors per cluster
    uint16_t BPB_rsvdSecCnt;        //Number of reserved sectors (always 1 for FAT16)
    uint8_t BPB_numFATs;            //Count of FAT data structures on the volume (always 2)
    uint16_t BPB_rootEntCnt;        //Count of 32-byte entries in the root directory
    uint32_t BPB_totSec16;          //Total count of sectors on the volume
    uint16_t BPB_FATSz16;           //Count of sectors occupied by ONE FAT
    char BS_volLab[11];             //Volume label
} Fat16;

int FAT16_isFat16(char* fspath);
void FAT16_printInfo(char* fspath);
void FAT16_printTree(char* fspath);
void FAT16_catFile(char* fspath, char* filename);

#endif