#ifndef TINYFS_H
#define TINYFS_H
#include "disk.h"

#define DEFAULT_DISK_SIZE 10240
#define DEFAULT_DISK_NAME "tinyFSDisk"
#define NUM_FDS 512
#define TABLEBNUMOFFSET (8*(BLOCKSIZE-4)/9+4) //the offset to get to the start of the block numbers in the FileTable
#define MAX_FILETABLE_SIZE ((BLOCKSIZE-4)/9)
#define DATA_BLOCKSIZE (BLOCKSIZE-4)
#define RO 1
#define RW 0


typedef int fileDescriptor;


typedef struct
{
   char blockType;               // index 0, value = 1
   char magicNum;                // index 1
   unsigned char fileListBlock;  //index 2
   unsigned char firstFreeBlock; //index 3
   
} Superblock;

typedef struct
{ 
   char blockType;                             //index 0, value = 2
   char magicNum;                              //index 1, value = 0x44
   unsigned char size;                         //index 2, number of entries in this table
   unsigned char next;                         //index 3, 0 means no next page
   char fileName[MAX_FILETABLE_SIZE][8];        //index 4  to 227
   unsigned char blockNum[MAX_FILETABLE_SIZE];   //index 228 to 255
   
} FileTable;

typedef struct
{
   char blockType;                      // index 0, value = 3
   char magicNum;                       // index 1
   unsigned char size;                  // index 2, size of index list
   char rw;         //index 3, value = RO (1) or RW (0) (read/write permission)
   unsigned int fileSize;               //index 4-7
   unsigned char blocks[BLOCKSIZE - 8]; // index 8


} IndexBlock;

typedef struct
{
   char blockType;        //index 0, value = 4
   char magicNum;         //index 1, value = 0x44
   unsigned char indexBlock;       //index 2, value = blockNum of index block that points to this file data block
   char data[DATA_BLOCKSIZE]; //index 4
   
} FileData;

typedef struct
{
   char blockType;        //index 0, value = 5
   char magicNum;         //index 1, value = 0x44
   char next;             //index 2, points to next free block
   
} FreeBlock;


typedef union
{
   Superblock superblock;
   FileTable filetable;
   FileData filedata;
   IndexBlock indexblock;
   FreeBlock freeblock;

} Block;

typedef struct
{
   char valid;
   char filename[9];
   unsigned char indexBlock;
   unsigned int filePosition;
   unsigned int fileLength;
} ResourceTableEntry;

fileDescriptor tfs_openFile(char *name);
int tfs_mkfs(char *filename, int nBytes);
int tfs_mount(char *diskname);
int tfs_unmount(void);
int tfs_closeFile(fileDescriptor FD);
int tfs_writeFile(fileDescriptor FD,char *buffer, int size);
int tfs_deleteFile(fileDescriptor FD);
int tfs_readByte(fileDescriptor FD, char *buffer);
int tfs_seek(fileDescriptor FD, int offset);
void printBlocks(int num);
void printResources(int num);
int tfs_writeByte(fileDescriptor FD,int offset, char data);
int tfs_makeRO(char* name);
int tfs_makeRO(char* name);
#endif
