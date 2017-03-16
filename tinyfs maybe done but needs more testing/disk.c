#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "LinkedList.h"

static ListNode *dlist;
static int nextDiskNum;

int cmpDiskNum(void*disk1, void*number)
{
   Disk * disk = (Disk*)disk1;
   int * num = (int*)number;
   if(disk->num == *num)
   {
      return 1;
   }
   else return 0;
}

int find_lastBlock(int numBlocks) {
   int i, currentBlock;
   for (i = 0; i < numBlocks; i++) {
      readBlock(diskNum, i, buff);
      if (buff[0] != 5) { 
         currentBlock = i;
      }
   }
   return currentBlock;
}

int remove_freeBlock(int bNum) {
   Block fBlock;
   Block sBlock;
   Block tempBlock;
   unsigned char list, prev;
   
   read_superblock(&sBlock);
   list = sBlock.superblock.firstFreeBlock;
   read_freeBlock(&fBlock, bNum);
   
   if (list <= 0) {
      return -1;
   }
   if (list == bNum) { 
      sBlock.superblock.firstFreeBlock = fBlock.freeblock.next;
      return list;
   }
   
   while (list != bNum) {
      prev = list;
      read_freeBlock(&fBlock, list);
      list = fBlock.freeblock.next;
      if (list == 0) {
         return -1; 
      }
   }
   read_freeBlock(&fBlock, list);
   tempBlock.freeblock.next = fBlock.freeblock.next;
   write_freeBlock(&tempBlock, prev);
   return list;
}

void tfs_defrag(int nBytes) {
   int i, lastB, numBlocks = nBytes / BLOCKSIZE;
   char buffer[BLOCKSIZE], buff[BLOCKSIZE];
   for (i = 0; i < numBlocks; i++) {
      readBlock(diskNum, i, buffer);
      if (buff[0] == 5) {
        
         lastB = find_lastBlock(numBlocks); 
         remove_freeBlock(i);
         readBlock(diskNum, lastB, buff);
         if (buff[0] == 2 && i > 1){ 
         
         }
         if (buff[0] == 3) { 
            read_indexBlock(lastB);
            
            write_indexBlock(i);
            freeBlock(lastB)
            
         }
         if(buff[0] == 4) {
         
         }
      }
     
   }
}

int cmpDiskName(void*disk1, void*filename)
{
   Disk * disk = (Disk*)disk1;
   char * name = (char*)filename;
   if(!strcmp(disk->filename, name))
   {
      return 1;
   }
   else return 0;
}

int openDisk(char *filename, int nBytes)
{
   FILE * file;
   int diskNum;
   Disk *disk;
   int i;
   
   if(nBytes == 0)
   {
      diskNum = getDiskNum(filename); //check if this file is already an open disk
      if(diskNum >= 0)
         return diskNum;
      
      file = fopen(filename, "r+");   //if not, open the file and add it to the disk list
      if(file == NULL)
      {
         return -1;
      }

   }
   else if(nBytes < BLOCKSIZE)
   {
      return -1;
   }
   
   else
   {
      nBytes = (nBytes / BLOCKSIZE) * BLOCKSIZE;
      file = fopen(filename, "w+");
      if(file == NULL)
      {
         return -1;
      }
      for(i=0; i<nBytes; i++)
      {
         fprintf(file, "%c", 0);
      }
      
   }
   diskNum = nextDiskNum;
   nextDiskNum++;
   disk = malloc(sizeof(Disk));
   disk -> num = diskNum;
   disk->filename = malloc(strlen(filename)+1);
   strcpy(disk->filename, filename);
   disk->file = file;
   dlist = addHead(dlist, (void*)disk);
   return diskNum;
}

int readBlock(int disk, int bNum, void *block)
{
   Disk * d;
   int index;
   cmpfun fun = &cmpDiskNum;
   index = indexOf(dlist, &disk, fun);
   if(index == -1)
   {
      printf("readBlock: file number not an open disk\n");
      return -1; //file number not open
   }
   
   d = (Disk*)get(dlist, index);
   
   fseek(d->file, bNum*BLOCKSIZE, SEEK_SET);
   if(fread(block, 1, BLOCKSIZE, d->file) != BLOCKSIZE)
   {
      printf("buffer too small\n");
      return -1; //couldn't read full block from file
   }
}

int getDiskNum(char * filename)
{
   int index;
   Disk *d;
   index = indexOf(dlist, filename, &cmpDiskName);
   
   d = (Disk*)get(dlist, index);
   
   if(d == NULL)
   {
      //this file hasn't been opened yet
      return -1;
   }
   return d->num;
}

int writeBlock(int disk, int bNum, void *block)
{
   Disk * d;
   int index;
   cmpfun fun = &cmpDiskNum;
   index = indexOf(dlist, &disk, fun);
   if(index == -1)
   {
      printf("file number not an open disk\n");
      return -1; //file number not open
   }
   
   d = (Disk*)get(dlist, index);
   
   fseek(d->file, bNum*BLOCKSIZE, SEEK_SET);
   if(fwrite(block, 1, BLOCKSIZE, d->file) != BLOCKSIZE)
   {
      printf("couldn't read full block from file\n");
      return -1; //couldn't read full block from file
   }
}
