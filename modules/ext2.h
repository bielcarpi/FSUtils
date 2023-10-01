#ifndef EXT2_H
#define EXT2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

#define EXT2_PRINT_INFO "\n------ Filesystem Information ------\n\n"

#define EXT2_PRINT_INFO_INODE "\nINODE INFO:\n\tSize: %d\n\tNum inodes: %d\n\tFirst inode: %d\n\tInodes Group: %d\n\tFree inodes: %d\n"
#define EXT2_PRINT_INFO_BLOCK "\nBLOCK INFO:\n\tBlock Size: %d\n\tReserved blocks: %d\n\tFree blocks: %d\n\tTotal blocks: %d\n\tFirst block: %d\n\tGroup blocks: %d\n\tGroup flags: %d\n"
#define EXT2_PRINT_INFO_VOLUME "\nVOLUME INFO:\n\tVolume name: %s\n\tLast Checked: %s\tLast Mounted: %s\tLast Written: %s\n"

// Superblock related constants
#define EXT2_SUPERBLOCK_OFFSET 1024

// EXT2 related constants
#define EXT2_MAGIC_NUMBER_OFFSET 56
#define EXT2_MAGIC_NUMBER 0xEF53

// Offsets:
// Inode related offsets
#define S_INODE_SIZE 88
#define S_INODE_COUNT 0
#define S_FIRST_INO 84
#define S_INODES_PER_GROUP 40
#define S_FREE_INODES_COUNT 16
// Block related offsets
#define S_LOG_BLOCK_SIZE 24
#define S_R_BLOCKS_COUNT 8
#define S_FREE_BLOCKS_COUNT 12
#define S_BLOCKS_COUNT 4
#define S_FIRST_DATA_BLOCK 20
#define S_BLOCK_PER_GROUP 32
#define S_FLAGS_PER_GROUP 36
// Volume related offsets
#define S_VOLUME_NAME 120
#define S_LASTCHECK 64
#define S_MTIME 44
#define S_WTIME 48



typedef struct {
    uint16_t i_mode;                    // Mode (type of file and permissions)
    uint16_t i_uid;                     // Owner's user ID
    uint32_t i_size;                    // File size (least significant 32 bits)
    uint32_t i_atime;                   // Last access time
    uint32_t i_ctime;                   // Creation time
    uint32_t i_mtime;                   // Last modification time
    uint32_t i_dtime;                   // Deletion time
    uint16_t i_gid;                     // Owner's group ID
    uint16_t i_links_count;             // Reference counter (number of hard links)
    uint32_t i_blocks;                  // Number of 512-byte blocks reserved for this inode
    uint32_t i_flags;                   // Flags
    uint32_t i_osd1;                    // OS-dependant value
    uint32_t i_block[15];               // Data for this inode
    uint32_t i_generation;              // File version (used for NFS)
    uint32_t i_file_acl;                // Block number for extended attributes
    uint32_t i_dir_acl;                 // File size (most significant 32 bits)
    uint32_t i_faddr;                   // Location of file fragment (deprecated)
    unsigned char i_osd2[12];
} Inode; //Inode

typedef struct{
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    unsigned char bg_reserved[12];
} GroupDescriptor; //Group Descriptor

typedef struct{
    uint32_t inode;
    uint16_t rec_len;                       // displacement to the next directory entry from the start of the current directory entry
    uint8_t name_len;
    uint8_t file_type;
    char name[255];
} DirectoryEntry; //Directory Entry

/******************************** SUPERBLOCK information related ********************************/
typedef struct {
    uint16_t s_inode_size;          // size of inode structure
    uint32_t s_inode_count;         // number of inodes
    uint32_t s_first_ino;           // first non-reserved inode
    uint32_t s_inodes_per_group;    // total number of inodes per group
    uint32_t s_free_inodes_count;   // number of free inodes
} InodeInfo;

typedef struct {
    uint32_t s_log_block_size;      // block size = 1024 * 2^s_log_block_size
    uint32_t s_r_blocks_count;      // number of reserved blocks
    uint32_t s_free_blocks_count;   // number of free blocks
    uint32_t s_blocks_count;        // total number of blocks in the system including all used, free and reserved.
    uint32_t s_first_data_block;    // first data block in the system -> id of the block containing the superblock structure
    uint32_t s_block_per_group;     // total number of blocks per group
    uint32_t s_flags_per_group;     // value indicating the total number of fragments per group
} BlockInfo;

typedef struct {
    char s_volume_name[16];           // volume name
    uint32_t s_lastcheck;             // time of last check
    uint32_t s_mtime;                 // last mounting time
    uint32_t s_wtime;                 // last writing time
} VolumeInfo;

typedef struct {
    uint16_t mgnum;                    // magic number -> filesystem identifier
    InodeInfo inode;
    BlockInfo block;
    VolumeInfo volume;
} Ext2;

/**
 * Function that checks if a file is an EXT2 filesystem
 * @param filepath: String with the path to the file
 * @return Whether the filesystem is EXT2 (1) or not (0)
 */
int EXT2_isExt2(char* filepath);

/**
 * Function that prints the information of an EXT2 filesystem
 * @param filepath : String with the representation of the path to the file
 */
void EXT2_printInfo(char* filepath);

/**
 * Function that prints the tree of an EXT2 filesystem
 * @param fspath : String with the representation of the path to the file
 */
void EXT2_printTree(char* fspath);

/**
 * This function aims to show the EXT2 information from the file as the linux command "cat" does
 * @param fspath : The path to the EXT2 file
 * @param filename : The name of the file to cat
 */
void EXT2_catFile(char* fspath, char* filename);

#endif