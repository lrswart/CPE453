#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tinyFS.h"
#include "disk.h"

static Block block1;
static Block block2;
static diskNum = -1;
static char buf[BLOCKSIZE];

static ResourceTableEntry resourceTable[NUM_FDS];


void printResources(int num)
{
   int i;
   printf("\nPrinting Resource Table\n");
   for(i=0; i<num; i++)
   {
      printf("filename = %s, valid = %d, index block = %d\n", resourceTable[i].filename, resourceTable[i].valid, resourceTable[i].indexBlock);
   }
   printf("\n\n");
}

void write_freeblock(Block *block, int bNum)
{
   memset(buf, 0, BLOCKSIZE);
   buf[0] = (*block).freeblock.blockType;
   buf[1] = (*block).freeblock.magicNum;
   buf[2] = (*block).freeblock.next;
   
   writeBlock(diskNum, bNum, buf);
}

void write_superblock(Block *block)
{
   memset(buf, 0, BLOCKSIZE);
   buf[0] = (*block).superblock.blockType;
   buf[1] = (*block).superblock.magicNum;
   buf[2] = (*block).superblock.fileListBlock;
   buf[3] = (*block).superblock.firstFreeBlock;
   
   writeBlock(diskNum, 0, buf);
}

void write_filetable(Block *block, int bNum)
{
   int i,j;
   memset(buf, 0, BLOCKSIZE);
   buf[0] = (*block).filetable.blockType;
   buf[1] = (*block).filetable.magicNum;
   buf[2] = (*block).filetable.size;
   buf[3] = (*block).filetable.next;
   
   for(i=0; i<MAX_FILETABLE_SIZE; i++)
   {
      for(j=0; j<8; j++)
      {
         buf[(i*8)+4+j] = (*block).filetable.fileName[i][j];
      }
      buf[TABLEBNUMOFFSET + i] = (*block).filetable.blockNum[i];
   }
   
   writeBlock(diskNum, bNum, buf);
}


void write_indexblock(Block* block, int bNum)
{
   int i;
   memset(buf, 0, BLOCKSIZE);
   buf[0] = (*block).indexblock.blockType;
   buf[1] = (*block).indexblock.magicNum;
   buf[2] = (*block).indexblock.size;
   buf[3] = (*block).indexblock.rw;
   memcpy(buf+4, &((*block).indexblock.fileSize), 4);
   for(i=0; i<(*block).indexblock.size; i++)
   {
      buf[8+i] = (*block).indexblock.blocks[i];
   }
   
   writeBlock(diskNum, bNum, buf);
}

void write_filedata(Block* block, int bNum)
{
   int i;
   memset(buf, 0, BLOCKSIZE);
   buf[0] = (*block).filedata.blockType;
   buf[1] = (*block).filedata.magicNum;
   buf[2] = (*block).filedata.indexBlock;
   
   for(i=0; i<DATA_BLOCKSIZE; i++)
   {
      buf[i+4] = (*block).filedata.data[i];
   }
   writeBlock(diskNum, bNum, buf);
}

void read_superblock(Block* block)
{

    readBlock(diskNum, 0, buf);
   
   (*block).superblock.blockType = buf[0]; // todo: check these when reading
   (*block).superblock.magicNum = buf[1];   // and these
   
   if((*block).superblock.blockType != 1)
   {
      //not an index block
      printf("bad super block identifier");
   }
   
   if((*block).superblock.magicNum != 0x44)
   {
      //bad magic num
      printf("bad magic number");
   }
   
   (*block).superblock.fileListBlock = buf[2];
   (*block).superblock.firstFreeBlock = buf[3];

}

void read_filetable(Block* block, int bNum)
{
   int i,j;
   memset(block, 0, sizeof(Block));
   readBlock(diskNum, bNum, buf);
   
   (*block).filetable.blockType = buf[0];
   (*block).filetable.magicNum = buf[1];
   
   if((*block).filetable.blockType != 2)
   {
      //not an index block
      printf("bad file table identifier\n");
   }
   
   if((*block).filetable.magicNum != 0x44)
   {
      //bad magic num
      printf("bad magic number\n");
   }
   
   
   (*block).filetable.size = buf[2]; 
   (*block).filetable.next = buf[3]; 
   
   for(i=0; i<MAX_FILETABLE_SIZE; i++)
   {
      for(j=0; j<8; j++)
      {
         (*block).filetable.fileName[i][j] = buf[(i*8)+4+j];
      }
      (*block).filetable.blockNum[i] = buf[TABLEBNUMOFFSET + i];
   }
}


void read_filedata(Block *block,  int bNum)
{
   int i;
   memset(block, 0, sizeof(Block));
   readBlock(diskNum, bNum, buf);
   
   (*block).filedata.blockType = buf[0];
   (*block).filedata.magicNum = buf[1];
   (*block).filedata.indexBlock = buf[2];
   
   if((*block).filedata.blockType != 4)
   {
      //not a file data block
      printf("bad file data block identifier\n");
   }
   
   if((*block).filedata.magicNum != 0x44)
   {
      //bad magic num
      printf("bad magic number\n");
   }
   
   
   for(i=0; i<DATA_BLOCKSIZE; i++)
   {
      (*block).filedata.data[i] = buf[i+4];
   }
}

void read_indexblock(Block *block,  int bNum)
{
   int i;
   memset(block, 0, sizeof(Block));
   readBlock(diskNum, bNum, buf);
   
   (*block).indexblock.blockType = buf[0]; 
   (*block).indexblock.magicNum = buf[1]; 
   
   if((*block).indexblock.blockType != 3)
   {
      //not an index block
      //assert(0);
      printf("bad index block identifier %d\n", (*block).indexblock.blockType);
   }
   
   if((*block).indexblock.magicNum != 0x44)
   {
      //bad magic num
      printf("bad magic number\n");
   }
   
   (*block).indexblock.size = buf[2];
   (*block).indexblock.rw = buf[3];
   memcpy(&((*block).indexblock.fileSize), buf+4, 4); //copy 4 bytes to get an int
   for(i=0; i<(*block).indexblock.size; i++)
   {
      (*block).indexblock.blocks[i] = buf[8+i];
   }
}

void read_freeblock(Block *block,  int bNum)
{
   memset(block, 0, sizeof(Block));
   readBlock(diskNum, bNum, buf);
   (*block).freeblock.blockType =buf[0]; 
   (*block).freeblock.magicNum = buf[1]; 
   (*block).freeblock.next = buf[2];
}


void printBlocks(int num)
{
   char buffer[BLOCKSIZE];
   Block block;
   int i,j, k;
   
   for(i=0; i<num; i++)
   {
      readBlock(diskNum, i, buffer);
      printf("Block %d:  ", i);
      switch(buffer[0])
      {
         case 1:
            read_superblock(&block);
            printf("Superblock: file list block = %d  first free block = %d",
               (int)block.superblock.fileListBlock, (int)block.superblock.firstFreeBlock);
         break;
         case 2:
            read_filetable(&block, i);
            printf("File Table: size = %d, next = %d\n", (int)block.filetable.size,(int)block.filetable.next);
            for(j=0; j<block.filetable.size; j++)
            {
               for(k=0; k<8; k++)
               {
                  if(block.filetable.fileName[j][k] == 0)
                  {
                     break;
                  }
                  else
                  {
                     printf("%c", block.filetable.fileName[j][k]);
                  }
               }               
               printf("\t index file %d: %d\n",j, (int)block.filetable.blockNum[j]);
            }
         break;
         case 3:
            read_indexblock(&block, i);
            if(block.indexblock.rw == RO)
            {
               printf("Index Block: size = %d blocks, file size = %d bytes, READ ONLY\n",
                  (int)block.indexblock.size, block.indexblock.fileSize);
            }
            else
            {
               printf("Index Block: size = %d blocks, file size = %d bytes\n",
               (int)block.indexblock.size, block.indexblock.fileSize);
            }
            for(j=0; j < block.indexblock.size; j++)
            {
               printf("virtual block index: %d  physical block: %d\n", 
                  j, (int)block.indexblock.blocks[j]);
            }
               
         break;
         case 4:
            read_filedata(&block, i);
            printf("file data block: index block = %d\n", (int)block.filedata.indexBlock);
            for(j=0; j<DATA_BLOCKSIZE; j++)
            {
               printf("%c", block.filedata.data[j]);
            }
         break;
         case 5:
            read_freeblock(&block, i);
            printf("free block:  next = %d", (int)block.freeblock.next);
         break;
         default:
            printf("bad block type\n  %d, 0x%X\n", (unsigned int)buffer[0], (unsigned int)buffer[1]); 
            for(j=0; j<BLOCKSIZE; j++)
            {
               printf("%c", buffer[j]);
            }
         break;
      }
      printf("\n\n");
      
   }
}

int firstUnusedFD()
{
   int i;
   for(i=0; i<NUM_FDS; i++)
   {
      if(resourceTable[i].valid == 0)
      {
         return i;
      }
   }
   return -1; //no available FDs
}

int getFreeBlock()
{
   Block sblock;
   Block fblock;
   int temp;
   
   read_superblock(&sblock);
   if(sblock.superblock.firstFreeBlock == 0)
   {
      return -1; //no free blocks
   }
   read_freeblock(&fblock, sblock.superblock.firstFreeBlock);
   temp = sblock.superblock.firstFreeBlock;
   sblock.superblock.firstFreeBlock = fblock.freeblock.next;
   write_superblock(&sblock);
   return temp;
}

void freeBlock(int bNum)
{
   Block sblock;
   Block fblock;
  
   read_superblock(&sblock);
    
   memset(&fblock, 0, sizeof(Block));
   fblock.freeblock.magicNum = 0x44;
   fblock.freeblock.blockType = 5;     //initialize free block
   fblock.freeblock.next = sblock.superblock.firstFreeBlock; //set next to first free block
    
   write_freeblock(&fblock, bNum);   //write the newly freed block over the old location
   sblock.superblock.firstFreeBlock = bNum; // set the new first free block to the newly freed block
   write_superblock(&sblock);
}



int tfs_mkfs(char *filename, int nBytes)
{
   int i;
   
   diskNum = openDisk(filename, nBytes);
   
   //todo: error checking
   
   block1.superblock.blockType = 1;
   block1.superblock.magicNum = 0x44;
   block1.superblock.fileListBlock = 1;
   block1.superblock.firstFreeBlock = 2;
   write_superblock(&block1);
   
   memset(&block1, 0, sizeof(Block));
   
   block1.filetable.blockType = 2;
   block1.filetable.magicNum = 0x44;
   block1.filetable.next = 0;
   write_filetable(&block1, 1);
   
   memset(&block1, 0, sizeof(Block));
   
   
   block1.freeblock.blockType = 5;
   block1.freeblock.magicNum = 0x44;
   for(i=2; i < (nBytes / BLOCKSIZE) - 1; i++)
   {
      block1.freeblock.next = i+1;
      write_freeblock(&block1, i);
   }
   block1.freeblock.next = 0;
   write_freeblock(&block1, nBytes/BLOCKSIZE - 1);   
   
   diskNum = -1; //unmount newly initialized disk
   
   return 0;
}


int tfs_mount(char *diskname)
{
   if(diskNum != -1)
   {
      //disk already mounted, return error
      return -1;
   }
   
   diskNum = openDisk(diskname, 0);
   
   //todo: error checking
   
   return 0;
}

int tfs_unmount(void)
{
   diskNum = -1;
   return 0;
}

fileDescriptor tfs_openFile(char *name)
{
   int i, j, fd, bNum;
   read_superblock(&block1);
   
   bNum = block1.superblock.fileListBlock;
   read_filetable(&block1, block1.superblock.fileListBlock);
   
   //search through the filetables to see if file already exists
   while(1)
   {
      
      for(i=0; i< block1.filetable.size; i++)
      {
         if(strlen(name) == 8 ) //string uses full 8 spaces
         {
            if(memcmp(block1.filetable.fileName[i], name, 8) == 0)
            {
               fd = firstUnusedFD();
               if(fd < 0)
               {
                  //error
               }
               //put the file into the resource table
               resourceTable[fd].valid = 1;
               memcpy(resourceTable[fd].filename, name, 8);
               resourceTable[fd].indexBlock = block1.filetable.blockNum[i];
               resourceTable[fd].filePosition = 0;
               return fd; //return resource table index of open file (fd)
            }
         }
         else  //null terminator fits on disk
         {
            if(strcmp(block1.filetable.fileName[i], name) == 0)
            {
               fd = firstUnusedFD();
               if(fd < 0)
               {
                  //error
               }
               //put the file into the resource table
               resourceTable[fd].valid = 1;
               strcpy(resourceTable[fd].filename, name);
               resourceTable[fd].indexBlock = block1.filetable.blockNum[i];
               resourceTable[fd].filePosition = 0;
               return fd; //return resource table index of open file (fd)
            }
         }
      }
      if(block1.filetable.next != 0)   //there is a next file table
      {
         bNum = block1.filetable.next;
         read_filetable(&block1, block1.filetable.next);
      }
      else break;
   }
   
   //printResources(5);
   //file didn't exist already, need to create new one
   if(block1.filetable.size < MAX_FILETABLE_SIZE)
   {
      //room in current filetable for another file
      if(strlen(name) == 8)
      {
         memcpy(block1.filetable.fileName[block1.filetable.size], name, 8); //copy name into filetable
      }
      else
      {
         strcpy(block1.filetable.fileName[block1.filetable.size], name);
      }
      block1.filetable.blockNum[block1.filetable.size] = getFreeBlock();  //get the next free block for IndexBlock
      block1.filetable.size += 1;  //increase file table size
      write_filetable(&block1, bNum);  //write new filetable contents to the disk
   }
   else
   {
      //need to make a new filetable
      block1.filetable.next = getFreeBlock(); //check this later for -1 error
      write_filetable(&block1, bNum);     //write filetable with updated next
           
      memset(&block2, 0, sizeof(Block));
   
      block2.filetable.blockType = 2;
      block2.filetable.magicNum = 0x44;   //inititalize filetable
      if(strlen(name) == 8)
      {
         memcpy(block2.filetable.fileName[block2.filetable.size], name, 8); //copy name into filetable
      }
      else
      {
         strcpy(block2.filetable.fileName[block2.filetable.size], name);
      } //copy name in
      block2.filetable.blockNum[block2.filetable.size] = getFreeBlock(); //space for IndexBlock
      block2.filetable.size += 1;
        
      write_filetable(&block2, block1.filetable.next); //write new filetable to disk
      block1 = block2;  //we want the new filetable block to be in block1 to work with now
   }
   
   //here we're going to create the IndexBlock for the new file in block2
   memset(&block2, 0, sizeof(Block)); //reset block2
   block2.indexblock.blockType = 3;
   block2.indexblock.magicNum = 0x44;
   block2.indexblock.size = 0;      //initially empty file
   block2.indexblock.fileSize = 0;
   block2.indexblock.rw = RW; //read and write permission
   //write the index block to the block pointed to in the filetable
   write_indexblock(&block2, block1.filetable.blockNum[block1.filetable.size - 1]); 
   
   
   //file is now created, just have to "open" it by putting it into resource table
   fd = firstUnusedFD();
   
   //printf("The fd that was chosen: %d\n",fd);
   if(fd < 0)
   {
      //error
   }
   //put the file into the resource table
   resourceTable[fd].valid = 1;
   memcpy(resourceTable[fd].filename, name, 8);
   
   //printf("filename: %s   valid = %d\n", resourceTable[fd].filename, resourceTable[fd].valid);
   //indexBlock still pointed at by last entry in filetable in block1, so
   resourceTable[fd].indexBlock = block1.filetable.blockNum[block1.filetable.size-1]; 
   
   resourceTable[fd].filePosition = 0;
   return fd; //return resource table index of open file (fd)
}

int tfs_closeFile(fileDescriptor FD)
{
   if(resourceTable[FD].valid == 0)
   {
      return -1; //file already closed
   }
   
   resourceTable[FD].valid = 0;
   return 0;
}
int tfs_writeFile(fileDescriptor FD,char *buffer, int size)
{
   int i, position, numBlocks;
   if(resourceTable[FD].valid == 0)
   {
      return -1; //bad FD
   }
   
   //get index block from disk using resource table index block value
   read_indexblock(&block1, resourceTable[FD].indexBlock);
   
   if(block1.indexblock.rw == RO)
   {
      return -1; // this block is read only
   }
   //free up space from old file
   for(i=0; i<block1.indexblock.size; i++)
   {
      freeBlock(block1.indexblock.blocks[i]);
      block1.indexblock.blocks[i] = 0;
   }
   
   //make space for new file
   memset(&block2, 0, sizeof(Block));
   block2.filedata.blockType = 4;
   block2.filedata.magicNum = 0x44;
   numBlocks = size / DATA_BLOCKSIZE + 1;
   for(i=0; i<numBlocks; i++)
   {
      block1.indexblock.blocks[i] = getFreeBlock();
      write_filedata(&block2, block1.indexblock.blocks[i]); //this line might be unecessary since we write all the file data blocks down below anyway. I don't feel like changing it and testing right now though.
   }
   
   //write updated index block to disk
   block1.indexblock.size = numBlocks;
   block1.indexblock.fileSize = size;
   write_indexblock(&block1, resourceTable[FD].indexBlock);
   
   
   //gonna loop through and fill data blocks with stuff from buffer
   position = 0;
   i=0;
   while(position != size)
   {
      if(size - position > DATA_BLOCKSIZE) //while there's still more than 1 data block worth of stuff to write
      {
         memset(&block2, 0, sizeof(Block)); //clear block 2 for use
         block2.filedata.blockType = 4;
         block2.filedata.magicNum = 0x44;
         block2.filedata.indexBlock = resourceTable[FD].indexBlock;
         memcpy(block2.filedata.data, buffer+position, DATA_BLOCKSIZE); //copy write buffer to block
        
         write_filedata(&block2, block1.indexblock.blocks[i]);   //write block to disk
         position += DATA_BLOCKSIZE;   //incrememnt buffer position
         i++;           //increment index block index
      }
      else
      {
         memset(&block2, 0, sizeof(Block)); //clear block 2 for use
         block2.filedata.blockType = 4;
         block2.filedata.magicNum = 0x44;
         block2.filedata.indexBlock = resourceTable[FD].indexBlock;
         memcpy(block2.filedata.data, buffer+position, size-position); //copy write buffer to block
        
         write_filedata(&block2, block1.indexblock.blocks[i]);   //write block to disk
         position += size-position;   //incrememnt buffer position
      }
   }
   
   resourceTable[FD].filePosition = 0;  //set file position to start of file
   resourceTable[FD].fileLength = size; //set file length to the buffer size
   return 0;
}
int tfs_deleteFile(fileDescriptor FD)
{
   int i, bNum, flag, j, bNum2;
   char *name;
   
   name = resourceTable[FD].filename; // name variable for convenience
   
   
   if(resourceTable[FD].valid == 0)
   {
      return -1; //bad FD
   }
   
   read_indexblock(&block1, resourceTable[FD].indexBlock);
   
   if(block1.indexblock.rw == RO)
   {
      return -1; // this block is read only
   }
   
   //first free all the data blocks of the file
   for(i=0; i<block1.indexblock.size; i++)
   {
      freeBlock(block1.indexblock.blocks[i]);
   }
   
   freeBlock(resourceTable[FD].indexBlock); //then free the index block
   
   //last remove the index block from the file table, and move the last thing on the file
   //table into the hole so that our file table doesn't wind up like swiss cheese
   memset(&block1, 0, sizeof(Block)); //clear block 1 and 2 just in case
   memset(&block2, 0, sizeof(Block));
   read_superblock(&block1);
   
   bNum = block1.superblock.fileListBlock;
   read_filetable(&block1, block1.superblock.fileListBlock); //read the fist file table into block1
   flag = 0;   //gonna use this to break out of the loop when we find the matching filename
   while(1)
   {    
      for(i=0; i< block1.filetable.size; i++)
      {
         if(strlen(name) == 8 ) //string uses full 8 spaces
         {
            if(memcmp(block1.filetable.fileName[i], name, 8) == 0)
            {
               flag = 1;
               break;
            }
         }
         else  //null terminator fits on disk
         {
            if(strcmp(block1.filetable.fileName[i], name) == 0)
            {
               flag = 1;
               break;
            }
         }
      }
      if(flag == 1)
      {
         break;
      }
      if(block1.filetable.next != 0)   //there is a next file table
      {
         bNum = block1.filetable.next;
         read_filetable(&block1, block1.filetable.next);
      }
      else
      {
         printf("deleted file but then couldn't find the file in the table, something's out of whack\n");
      }
   }
   
   // i now holds the index of the file that was deleted in the file table in block1.
   // now we need the last thing in the last table
   
   block2 = block1;
   bNum2 = bNum;     //bNum2 holds the physical block of block2, which is the last file table
   while(block2.filetable.next != 0)
   {
      bNum2 = block2.filetable.next;
      read_filetable(&block2, block2.filetable.next);
   }
   
   if(bNum2 == bNum) //only one file table
   {
      for(j=0; j<8; j++)
      {
         //copy filename from end of last table into where the deleted file used to be
         block1.filetable.fileName[i][j] = block1.filetable.fileName[block1.filetable.size -1][j];
      }
      block1.filetable.blockNum[i] = block1.filetable.blockNum[block1.filetable.size -1];
      block1.filetable.size -= 1;
      write_filetable(&block1, bNum);
   }
   else
   {
      //last file table is in block2, last entry should be at size-1 index
      for(j=0; j<8; j++)
      {
         //copy filename from end of last table into where the deleted file used to be
         block1.filetable.fileName[i][j] = block2.filetable.fileName[block2.filetable.size -1][j];
         
      }
      //copy block number over
      block1.filetable.blockNum[i] = block2.filetable.blockNum[block2.filetable.size -1];
      
      block2.filetable.size -= 1;   //decrement the size of the last filetable since we're pulling the last entry off
      
      
      write_filetable(&block1, bNum);
      write_filetable(&block2, bNum2);
   }
   
   resourceTable[FD].valid = 0;
   return 0;
}
int tfs_readByte(fileDescriptor FD, char *buffer)
{
   unsigned int blockIndex;
   unsigned int blockOffset;
   if(resourceTable[FD].valid == 0)
   {
      return -1; // bad FD
   }
   if(resourceTable[FD].filePosition == resourceTable[FD].fileLength)
   {
      return -1; // end of file
   }
   if(resourceTable[FD].filePosition > resourceTable[FD].fileLength)
   {
      return -1; // bad file position
   }
   
   blockIndex = resourceTable[FD].filePosition / DATA_BLOCKSIZE; // find out which block of the file to look in
   blockOffset = resourceTable[FD].filePosition % DATA_BLOCKSIZE; // index into the filedata block data
   
   read_indexblock(&block1, resourceTable[FD].indexBlock);
   
   read_filedata(&block2, block1.indexblock.blocks[blockIndex]);
   *buffer = block2.filedata.data[blockOffset];
   
   resourceTable[FD].filePosition += 1;  //increment position in file
   return 0;
}

int tfs_seek(fileDescriptor FD, int offset)
{
   if(resourceTable[FD].valid == 0)
   {
      return -1; // bad FD
   }
   
   if(offset >= resourceTable[FD].fileLength)
   {
      return -1; // offset is past end of file
   }
   resourceTable[FD].filePosition = offset;
   return 0;
}

int changePermission(char * name, int permission)
{
   int i, bNum;
   
   //I wish this function just took a FD, but no, gotta find the file on the disk by name
   //this code is lifted from open file, lightly modified to change read/write permission,
   //instead of opening the file
   read_superblock(&block1);
   
   bNum = block1.superblock.fileListBlock;
   read_filetable(&block1, block1.superblock.fileListBlock);

   while(1) 
   {
      
      for(i=0; i< block1.filetable.size; i++)
      {
         if(strlen(name) == 8 ) //string uses full 8 spaces
         {
            if(memcmp(block1.filetable.fileName[i], name, 8) == 0)
            {
               read_indexblock(&block2, block1.filetable.blockNum[i]);
               block2.indexblock.rw = permission;
               write_indexblock(&block2, block1.filetable.blockNum[i]);
            }
         }
         else  //null terminator fits on disk
         {
            if(strcmp(block1.filetable.fileName[i], name) == 0)
            {
               read_indexblock(&block2, block1.filetable.blockNum[i]);
               block2.indexblock.rw = permission;
               write_indexblock(&block2, block1.filetable.blockNum[i]);
            }
         }
      }
      if(block1.filetable.next != 0)   //there is a next file table
      {
         bNum = block1.filetable.next;
         read_filetable(&block1, block1.filetable.next);
      }
      else
      {
         //If the file doesn't exist, don't create one.
         //instead, throw an error because there's no file to set the read/write permission of.
         return -1; //file with that name does not exist
      }
   }
}


int tfs_makeRO(char* name)
{
   return changePermission(name, RO);
}

int tfs_makeRW(char* name)
{
   return changePermission(name, RW);
}


//writing a byte resets the file seek. I chose this behavior because it was easy and the specs don't say what to
//do with the file seek. It makes no sense to leave it where it is though since much of the file ends up being
//shifted by the byte that gets added
int tfs_writeByte(fileDescriptor FD,int offset, char data)
{
   int i, result;
   char *fileData;
   
   //we're gonna copy the whole file to a buffer, add in the one new byte at the desired position,
   //and then write the whole thing again.
   
   //first get the size of the file and use it to make space in our buffer, plus one spot for new byte
   fileData = malloc(sizeof(char) * (resourceTable[FD].fileLength + 1));
   
   // set the seek to the beginning of the file
   tfs_seek(FD, 0);
   
   
   for(i=0; i<offset; i++)
   {
      //put the next byte of the file in the buffer at position i in fileData
      //tfs_readByte increments file position each time
      tfs_readByte(FD, fileData+i); //check this for errors?
   }
   
   fileData[offset] = data; //I dont know why they ask for data to be an int, I guess just cast it to char
   
   //the seek should still be set to position offset, but now we're going to be copying it
   // one place further into the buffer. So  the file offset should equal i - 1 in this loop.
   //if this code doesn't work, remember to check to make sure that's true, because I'm not sure
   for(i=offset+1; i<resourceTable[FD].fileLength+1; i++)
   {
      tfs_readByte(FD, fileData+i);
   }
   
   
   result = tfs_writeFile(FD, fileData, resourceTable[FD].fileLength+1);
   free(fileData);
   return result;
   
}
