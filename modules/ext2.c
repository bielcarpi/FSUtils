#include "ext2.h"
#include "tree.h"

static Inode getInode(FILE *fp, Ext2 *ext2, int inodeNum);
static int pierceTree(FILE *fp, Ext2 *ext2, int nextInode, int catFile, char *fileName, struct TreeNode *parent);
static void printFileContent(FILE *fp, Ext2 ext2, Inode inode);

/**
 * Function that checks if a file is an EXT2 filesystem
 * @param filepath: String with the path to the file
 * @return Whether the filesystem is EXT2 (1) or not (0)
 */
int EXT2_isExt2(char* filepath){
    //Reading the EXT2 file information
    FILE* fp = fopen(filepath, "rb"); // Binary Reading mode

    //If there's an error while opening the file, return 0
    if(fp == NULL) return 0;

    // 1024 + 56 -> magic number per saber si es ext i llegir 2 bytes de mgnum
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + EXT2_MAGIC_NUMBER_OFFSET, SEEK_SET);
    uint16_t mgnum; // Magic number
    fread(&mgnum, sizeof(uint16_t), 1, fp);
    fclose(fp);

    //If the magic number is 0xEF53, it's an EXT2 filesystem
    if(mgnum == EXT2_MAGIC_NUMBER) return 1;
    //Else, return 0
    return 0;
}

/**
 * Function that reads the information of an EXT2 filesystem
 * @param fp : File pointer to the EXT2 filesystem
 * @return EXT2 struct with the information of the filesystem
 */
static Ext2 readInfo(FILE *fp){
    Ext2 ext2;

    fseek(fp, EXT2_SUPERBLOCK_OFFSET + EXT2_MAGIC_NUMBER_OFFSET, SEEK_SET);
    fread(&(ext2.mgnum), sizeof(uint16_t), 1, fp);

    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_INODE_SIZE, SEEK_SET);
    fread(&(ext2.inode.s_inode_size), sizeof(uint16_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_INODES_PER_GROUP, SEEK_SET);
    fread(&(ext2.inode.s_inodes_per_group), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_INODE_COUNT, SEEK_SET);
    fread(&(ext2.inode.s_inode_count), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_FIRST_INO, SEEK_SET);
    fread(&(ext2.inode.s_first_ino), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_FREE_INODES_COUNT, SEEK_SET);
    fread(&(ext2.inode.s_free_inodes_count), sizeof(uint32_t), 1, fp);

    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_LOG_BLOCK_SIZE, SEEK_SET);
    fread(&(ext2.block.s_log_block_size), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_R_BLOCKS_COUNT, SEEK_SET);
    fread(&(ext2.block.s_r_blocks_count), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_FREE_BLOCKS_COUNT, SEEK_SET);
    fread(&(ext2.block.s_free_blocks_count), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_BLOCKS_COUNT, SEEK_SET);
    fread(&(ext2.block.s_blocks_count), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_FIRST_DATA_BLOCK, SEEK_SET);
    fread(&(ext2.block.s_first_data_block), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_BLOCK_PER_GROUP, SEEK_SET);
    fread(&(ext2.block.s_block_per_group), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_FLAGS_PER_GROUP, SEEK_SET);
    fread(&(ext2.block.s_flags_per_group), sizeof(uint32_t), 1, fp);

    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_VOLUME_NAME, SEEK_SET);
    fread(&(ext2.volume.s_volume_name), 16, 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_LASTCHECK, SEEK_SET);
    fread(&(ext2.volume.s_lastcheck), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_MTIME, SEEK_SET);
    fread(&(ext2.volume.s_mtime), sizeof(uint32_t), 1, fp);
    fseek(fp, EXT2_SUPERBLOCK_OFFSET + S_WTIME, SEEK_SET);
    fread(&(ext2.volume.s_wtime), sizeof(uint32_t), 1, fp);

    return ext2;
}

/**
 * Function that prints the information of an EXT2 filesystem
 * @param filepath : String with the representation of the path to the file
 */
void EXT2_printInfo(char* filepath){
    FILE* fp = fopen(filepath, "rb"); // Binary Reading mode
    if(fp == NULL){
        printf("Error while opening the file %s\n", filepath);
        return;
    }

    printf(EXT2_PRINT_INFO); // Print the EXT2 information
    //Reading the EXT2 file information
    Ext2 ext2 = readInfo(fp);
    printf("Filesystem: EXT2\n");

    // Inode print information
    printf(EXT2_PRINT_INFO_INODE,
           ext2.inode.s_inode_size,
           ext2.inode.s_inode_count,
           ext2.inode.s_first_ino,
           ext2.inode.s_inodes_per_group,
           ext2.inode.s_free_inodes_count);

    // Block print information
    printf(EXT2_PRINT_INFO_BLOCK,
           1024 << ext2.block.s_log_block_size,
           ext2.block.s_r_blocks_count,
           ext2.block.s_free_blocks_count,
           ext2.block.s_blocks_count,
           ext2.block.s_first_data_block,
           ext2.block.s_block_per_group,
           ext2.block.s_flags_per_group);

    // Volume print information
    printf(EXT2_PRINT_INFO_VOLUME,
           ext2.volume.s_volume_name,
           asctime(gmtime(&(time_t) {ext2.volume.s_lastcheck})),
           asctime(gmtime(&(time_t) {ext2.volume.s_mtime})),
           asctime(gmtime(&(time_t) {ext2.volume.s_wtime}))
           );

    fclose(fp);
}

/**
 * Function that prints the tree of an EXT2 filesystem
 * @param fspath : String with the representation of the path to the file
 */
void EXT2_printTree(char* fspath){
    struct TreeNode rootNode;
    rootNode.name = NULL;
    rootNode.numChilds = 0;

    FILE* fp = fopen(fspath, "rb"); // Binary Reading mode
    if(fp == NULL){
        printf("Error while opening the file %s\n", fspath);
        return;
    }

    // Reading the EXT2 file information
    Ext2 ext2 = readInfo(fp);

    // Reading the root inode (inode 2)
    // Fill the tree (don't cat a file)
    pierceTree(fp, &ext2, 2, 0, NULL, &rootNode);

    // Print & free the tree
    TREE_print(&rootNode);
    TREE_free(&rootNode);
    fclose(fp);
}

/**
 * Pierce the EXT2 tree recursively. It can have two behaviors:
 *  1. If catFile is 1, it will cat the file fileName if found and return 1 if successful, 0 otherwise
 *  2. If catFile is 0, it will construct the tree recursively, from the parent node. Return value will always be 0
 * @param fp : File pointer
 * @param ext2 : EXT2 information
 * @param nextInode : Next inode to read (recursive call)
 * @param catFile : Whether to cat a file (1) or construct a tree (0)
 * @param fileName : The name of the file to cat
 * @param parent : The parent node to construct the tree (recursive call)
 * @return Whether the cat was successful (1) or not (0)
 */
static int pierceTree(FILE *fp, Ext2 *ext2, int nextInode, int catFile,
                      char *fileName, struct TreeNode *parent){

    //Get the inode
    Inode inode = getInode(fp, ext2, nextInode);
    //Get the block size using the s_log_block_size attribute from the superblock
    int blockSz = 1024 << ext2->block.s_log_block_size;

    uint32_t offset = 0;
    DirectoryEntry de;

    //Loop through the whole inode (in rec_len steps)
    while(1){
        //Read the next DirectoryEntry, until there are no more entries
        fseek(fp, blockSz + blockSz*(inode.i_block[0]-1) + offset, SEEK_SET);
        //Read the directory entry
        fread(&de, sizeof(DirectoryEntry), 1, fp);
        de.name[de.name_len] = '\0';
        //Update the offset
        offset += de.rec_len;
        //Break if the offset is too big or if the rec_len is 0 -> no more entries to read
        if(de.rec_len == 0 || offset > inode.i_size) break;
        // Skip the entry if it's not a file or a directory
        if(strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0 && strcmp(de.name, "lost+found") != 0){
            //If we're in mode cat file
            if(catFile){
                //If we found the file we were searching, print it and return 1
                if(de.file_type == 1 && strcmp(de.name, fileName) == 0){
                    printFileContent(fp, *ext2, inode);
                    return 1;
                }
                else if(de.file_type == 2){ //If the entry is a directory, call the function recursively
                    if(pierceTree(fp, ext2, de.inode, 1, fileName, NULL))
                        return 1;
                }
            }
            else{ //If we're in mode build tree
                //If the entry is a directory, call the function recursively
                if(de.file_type == 2){
                    struct TreeNode *newNode = TREE_addChild(parent, de.name);
                    pierceTree(fp, ext2, de.inode, 0, NULL, newNode);
                }
                else if(de.file_type == 1){ //If the entry is a file, add it to the tree
                    TREE_addChild(parent, de.name);
                }
            }
        }
    }
    return 0;
}

/**
 * This function aims to find the inode by its inode number in the inode table
 * @param fp : File pointer
 * @param ext2 : EXT2 information
 * @param inodeNum : The inode number to find
 * @return Structure containing the information of the inode
 */
static Inode getInode(FILE *fp, Ext2 *ext2, int inodeNum) {

    //Calculate the block size
    int blockSz = 1024 << ext2->block.s_log_block_size;

    //Move to the superblock + 1 (where the group descriptor is), and read it (we need the inode table offset)
    GroupDescriptor gd;
    fseek(fp, (ext2->block.s_first_data_block + 1) * blockSz, SEEK_SET);
    fread(&gd, sizeof(GroupDescriptor), 1, fp);

    //Calculate relative inode position (inside a group) and block group in which it is
    int relativeInode = (inodeNum - 1) % ext2->inode.s_inodes_per_group;    // Position of the inode inside the group
    int blockGroup = (inodeNum - 1) / ext2->inode.s_inodes_per_group;       // Block group in which the inode is

    //Calculate the position of the inode in the inode table
    int inodePos = relativeInode * ext2->inode.s_inode_size;

    //Calculate the position of the inode table
    int inodeTablePos =  gd.bg_inode_table + (blockGroup * ext2->block.s_block_per_group);

    //Move to the inode, read & return it
    Inode in;
    fseek(fp, (inodeTablePos * blockSz) + inodePos, SEEK_SET);
    fread(&in, sizeof(Inode), 1, fp);
    return in;
}

/**
 * This function aims to show the EXT2 information from the file as the linux command "cat" does
 * @param fspath : The path to the EXT2 file
 * @param filename : The name of the file to cat
 */
void EXT2_catFile(char* fspath, char* filename){
    FILE* fp = fopen(fspath, "rb"); // Binary Reading mode
    if(fp == NULL){
        printf("Error while opening the file %s\n", fspath);
        return;
    }

    //Reading the EXT2 file information
    Ext2 ext2 = readInfo(fp);

    //Start searching the file from the root inode (2)
    int found = pierceTree(fp, &ext2, 2, 1, filename, NULL);
    if(!found) printf("File not found\n\n");
    fclose(fp);
}

/**
 * This function aims to print the content of a file
 */
static void printFileContent(FILE *fp, Ext2 ext2, Inode inode){
    //Allocate memory for the first data block & add it
    uint32_t dataBlocks[15];
    dataBlocks[0] = inode.i_block[0];
    uint32_t blockSize = 1024 << ext2.block.s_log_block_size;

    //Add the next 11 data blocks if they exist
    int k;
    for (k = 1; k < 12; k++) {
        if (inode.i_block[k - 1] == 0) break;
        dataBlocks[k] = inode.i_block[k];
    }

    //TODO: Add the indirect blocks (12-15)

    //Loop through all the data blocks, and print them
    uint32_t readBytes;
    uint32_t remainingSize = inode.i_size;
    for(int i = 0; i < k; i++){

        //If the file size is bigger than the block size, we read the block size
        if(inode.i_size > blockSize) readBytes = blockSize;
        else readBytes = inode.i_size;

        //Allocate memory for the read bytes, read them, and print them
        char *read = (char *) malloc(readBytes * sizeof(char));
        fseek(fp, dataBlocks[i], SEEK_SET);
        fread(read, readBytes * sizeof(char), 1, fp);
        printf("%s", read);
        free(read);

        //Update the file size (we read the block size)
        remainingSize = remainingSize - readBytes;
    }
}