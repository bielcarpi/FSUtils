#include "fat16.h"
#include "tree.h"

static int pierceTree(FILE *fp, Fat16 fat16, int blockNum, int catFile, char *fileName, struct TreeNode *parent);
static void cleanString(char *string, int size);
static Fat16 readInfo(FILE *f);
static void printFileContent(FILE *f, Fat16 fat16, int dataSectorStart, FatDirectoryEntry entry);

/**
 * This function is used to check if the filesystem is FAT16 or not
 * @param filepath : The path to the file
 * @return Returns the value of 1 if it's FAT16, 0 otherwise
 */
int FAT16_isFat16(char* filepath){
    //To check whether it's FAT16, we need to look into the file system itself.
    //If the number of clusters is equal or more than 4085, but less than 65525 it's FAT16
    //If number of clusters was less than 4085 it's FAT12, and if it's more than 65525 it's FAT32

    //Reading the FAT16 file information
    FILE* f = fopen(filepath, "rb"); // Binary Reading mode
    if(f == NULL) return 0;

    Fat16 fat16 = readInfo(f);

    int32_t FatStartSector = fat16.BPB_rsvdSecCnt;
    int32_t FatSectors = fat16.BPB_FATSz16 * fat16.BPB_numFATs;
    int32_t RootDirStartSector = FatStartSector + FatSectors;
    int32_t RootDirSectors = (32 * fat16.BPB_rootEntCnt + fat16.BPB_bytsPerSec - 1) / fat16.BPB_bytsPerSec;
    int32_t DataStartSector = RootDirStartSector + RootDirSectors;
    int32_t DataSectors = fat16.BPB_totSec16 - DataStartSector;

    int32_t countOfClusters = DataSectors / fat16.BPB_secPerClus;

    fclose(f);

    //Like said, if the number of clusters is equal or more than 4085, but less than 65525 it's FAT16
    if(countOfClusters >= 4085 && countOfClusters < 65525) return 1;
    return 0;
}

/**
 * This function is used to read the FAT16 file information
 * @param f : The file pointer
 * @return Returns the Fat16 structure
 */
static Fat16 readInfo(FILE *f){
    //Reading the FAT16 file information
    Fat16 fat16;

    fseek(f, 3, SEEK_SET);
    fread(&(fat16.BS_oemName), sizeof(char) * 8, 1, f);
    fseek(f, 11, SEEK_SET);
    fread(&(fat16.BPB_bytsPerSec), sizeof(uint16_t), 1, f);
    fseek(f, 13, SEEK_SET);
    fread(&(fat16.BPB_secPerClus), sizeof(uint8_t), 1, f);
    fseek(f, 22, SEEK_SET);
    fread(&(fat16.BPB_FATSz16), sizeof(uint16_t), 1, f);
    fseek(f, 14, SEEK_SET);
    fread(&(fat16.BPB_rsvdSecCnt), sizeof(uint16_t), 1, f);
    fseek(f, 16, SEEK_SET);
    fread(&(fat16.BPB_numFATs), sizeof(uint8_t), 1, f);
    fseek(f, 17, SEEK_SET);
    fread(&(fat16.BPB_rootEntCnt), sizeof(uint16_t), 1, f);
    fseek(f, 19, SEEK_SET);
    fread(&(fat16.BPB_totSec16), sizeof(uint16_t), 1, f);
    fseek(f, 43, SEEK_SET);
    fread(&(fat16.BS_volLab), sizeof(char) * 11, 1, f);
    fat16.BS_volLab[10] = '\0';

    if(fat16.BPB_totSec16 == 0){
        fseek(f, 32, SEEK_SET);
        fread(&(fat16.BPB_totSec16), sizeof(uint32_t), 1, f);
    }

    return fat16;
}

void FAT16_printInfo(char* filepath){
    FILE *f = fopen(filepath, "rb"); // Binary Reading mode
    if(f == NULL){
        printf("Error while opening the file %s\n", filepath);
        return;
    }

    Fat16 fat16 = readInfo(f);

    printf(FAT16_PRINT_INFO, fat16.BS_oemName, fat16.BPB_bytsPerSec, fat16.BPB_secPerClus,
           fat16.BPB_rsvdSecCnt, fat16.BPB_numFATs, fat16.BPB_rootEntCnt, fat16.BPB_FATSz16,
           fat16.BS_volLab);
}

void FAT16_printTree(char* fspath){
    FILE *f = fopen(fspath, "rb"); // Binary Reading mode
    if(f == NULL){
        printf("Error while opening the file %s\n", fspath);
        return;
    }

    // Read the FAT16 info
    Fat16 fat16 = readInfo(f);
    struct TreeNode rootNode;
    rootNode.name = NULL;
    rootNode.numChilds = 0;

    // Pierce the tree in order to construct the tree
    pierceTree(f, fat16, 2, 0, NULL, &rootNode);

    // Print & free the tree
    TREE_print(&rootNode);
    TREE_free(&rootNode);

    fclose(f);
}

static void cleanString(char *string, int size) {
    int j = 0;

    // Iterate through the string
    for(int i = 0; i <= size; i++){
        // If it's a space, exit the loop and add the null terminator
        if(string[i] != ' '){
            // If it's a capital letter, make it lower case
            if(string[i] >= 'A' && string[i] <= 'Z') string[j] = string[i] - 'A' + 'a';
            else string[j] = string[i];

            j++;
        }
        else {
            string[j] = '\0';
            break;
        }
    }

    string[j] = '\0';
}

/**
 * Pierce the FAT16 tree recursively. It can have two behaviors:
 *  1. If catFile is 1, it will cat the file fileName if found and return 1 if successful, 0 otherwise
 *  2. If catFile is 0, it will construct the tree recursively, from the parent node. Return value will always be 0
 * @param fp : The file pointer
 * @param fat16 : The FAT16 structure
 * @param blockNum : The block number
 * @param catFile : Whether to cat the file (1) or not (0)
 * @param parent : The parent node to construct the tree (recursive call)
 * @return Whether the cat was successful (1) or not (0)
 */
static int pierceTree(FILE *fp, Fat16 fat16, int blockNum, int catFile, char *fileName, struct TreeNode *parent){

    // The root directory has a size of 32 bytes per entry
    // So, the root region starts at the first sector after the reserved sectors
    // Multiplying by 32 we get the number of bytes that the root region occupies
    int rootRegion = fat16.BPB_rootEntCnt * 32;

    // Calculate the root region start -> After the reserved area and the 2 FATs
    // The root directory region starts at the first sector after the reserved sectors & FAT tables
    int rootRegionStart = (fat16.BPB_rsvdSecCnt + (fat16.BPB_numFATs * fat16.BPB_FATSz16))
            * fat16.BPB_bytsPerSec;

    // The data area region starts at the first sector after the root region
    int dataAreaRegionEntry = ((blockNum - 2) * fat16.BPB_secPerClus * fat16.BPB_bytsPerSec)
                                   + rootRegionStart;

    //If we're not in the root directory, we have to add the root region size
    //If blockNum == 2, we're in the directly followed address from the root directory,
    // so we don't have to add it because it's already added in the dataAreaRegionEntry
    //If blockNum != 2, we're in a different directory, so we have to add the root region size
    if(blockNum != 2) dataAreaRegionEntry += rootRegion;

    //Position cursor at the desired region
    fseek(fp, dataAreaRegionEntry, SEEK_SET);

    FatDirectoryEntry de;
    char strCopy[12];
    int lastCluster = -1;

    for(int i = 0; 1; i++) {
        fread(&de, sizeof(FatDirectoryEntry), 1, fp);

        if (de.long_name[0] == '\0') break;

        //Clean the strings (remove spaces and convert to lowercase)
        cleanString(de.long_name, 8);
        cleanString(de.extension, 3);

        memset(strCopy, 0, 12);
        //Move de.long_name to a temporary string (ending with \0)
        int j;
        for(j = 0; j < 8 && de.long_name[j] != '~' && de.long_name[j] != '\0'; j++)
            strCopy[j] = de.long_name[j];

        //If we have an extension
        if(de.extension[0] != '\0' && (de.extension[0] < '1' || de.extension[0] > '9')){
            strCopy[j++] = '.';
            for(int k = 0; k < 3; k++)
                strCopy[j + k] = de.extension[k];
        }

        if(de.firstCluster != 0 && lastCluster == de.firstCluster) continue;
        lastCluster = de.firstCluster;

        // Directory: File Attribute = 16
        // File: File Attribute = 32
        //If we have a directory (and it is not . or ..), we have to go inside
        if (de.fileAttr == 16 && strcmp(de.long_name, ".") != 0 && strcmp(de.long_name, "..") != 0) {
            if(catFile == 0){
                struct TreeNode *newNode = TREE_addChild(parent, strCopy);
                pierceTree(fp, fat16, de.firstCluster, 0, NULL, newNode);
            }
            else{ //catFile == 1, search for the file in the directory
                //If we found the file, return 1 immediately
                if(pierceTree(fp, fat16, de.firstCluster, 1, fileName, NULL))
                    return 1;
            }

            fseek(fp, dataAreaRegionEntry + (i * sizeof(FatDirectoryEntry)), SEEK_SET);
        }
        else if(de.fileAttr == 32){ //If we have a file
            if(catFile == 1 && strcmp(strCopy, fileName) == 0){ //If we found the file
                printFileContent(fp, fat16, rootRegionStart + rootRegion, de);
                return 1;
            }
            else if(catFile == 0){ //If we're constructing the tree
                TREE_addChild(parent, strCopy);
            }
        }
    }

    return 0;
}

/**
 * This function is used to cat a file from a FAT16 filesystem
 * @param fspath : The path to the FAT16 filesystem
 * @param filename : The name of the file to cat
 */
void FAT16_catFile(char* fspath, char* filename){
    FILE *f = fopen(fspath, "rb"); // Binary Reading mode
    if(f == NULL){
        printf("Error while opening the file %s\n", fspath);
        return;
    }

    // Read the FAT16 info
    Fat16 fat16 = readInfo(f);
    struct TreeNode rootNode;
    rootNode.name = NULL;
    rootNode.numChilds = 0;

    // Pierce the tree in cat file mode (whenever we find the file, we print it)
    int found = pierceTree(f, fat16, 2, 1, filename, NULL);
    if(!found) printf("File not found\n\n");
    fclose(f);
}

static void printFileContent(FILE *f, Fat16 fat16, int dataSectorStart, FatDirectoryEntry entry){
    // Calculate the base byte of the file (first byte of the first cluster)
    int bytePos = (((entry.firstCluster - 2) * fat16.BPB_secPerClus) * fat16.BPB_bytsPerSec) + dataSectorStart;

    // Print the file contents, byte by byte
    char c;
    for (uint32_t i = 0; i < entry.fSize; i++){
        fseek(f, bytePos + i, SEEK_SET);
        fread(&c, sizeof(char), 1, f);
        printf("%c", c);
    }

}