#include "modules/ext2.h"
#include "modules/fat16.h"

#define ERR_ARGS "Error. Please, provide correct arguments for fsutils to work. Use --help for more info.\n\n"
#define ERR_FS_NOT_SUPPORTED "Error. %s does not exist or has a Filesystem not supported. Only EXT2 and FAT16 are supported.\n\n"
#define HELP "\nFSUTILS HELP\n------------\nfsutils is a tool that provides multiple utilities for analyzing EXT2 & FAT16 filesystems.\nUsage: fsutils [OPTION] [FILESYSTEM PATH]\n\nOptions:\n\t--info\t\tPrints the information of the filesystem.\n\t--tree\t\tPrints the tree of the filesystem.\n\t--cat\t\tPrints the content of a file.\n\t--help\t\tPrints this help.\n\n"
#define EXT2 0
#define FAT16 1

int main(int argc, char *argv[]) {

    //Print help if the user asks for it
    if(argc == 2 && strcmp(argv[1], "--help") == 0){
        printf(HELP);
        return 0;
    }

    //If the number of arguments is not correct, print an error and return
    if(argc != 3 && argc != 4){
        printf(ERR_ARGS);
        return 1;
    }

    int fs; //0 = EXT2, 1 = FAT16
    //Check if the file is EXT2 or FAT16
    if(EXT2_isExt2(argv[2])){
        fs = EXT2;
    }
    else if(FAT16_isFat16(argv[2])){
        fs = FAT16;
    }
    else{
        printf(ERR_FS_NOT_SUPPORTED, argv[2]);
        return 1;
    }

    //If the info option is selected, try to get the info from the file
    if(argc == 3 && strcmp(argv[1], "--info") == 0){
        if(fs == EXT2) EXT2_printInfo(argv[2]);
        else FAT16_printInfo(argv[2]);
    }
    else if(argc == 3 && strcmp(argv[1], "--tree") == 0){
        if(fs == EXT2) EXT2_printTree(argv[2]);
        else FAT16_printTree(argv[2]);
    }
    else if(argc == 4 && strcmp(argv[1], "--cat") == 0){
        if(fs == EXT2) EXT2_catFile(argv[2], argv[3]);
        else FAT16_catFile(argv[2], argv[3]);
    }
    else{
        printf(ERR_ARGS);
    }
}