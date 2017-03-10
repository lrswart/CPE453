#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinyFS_errno.h"
#include "tinyFS.h"

FILE* diskArray[BLOCKSIZE];
int diskNum;

void initDiskArray() {
   int i;
   for (i = 0; i < BLOCKSIZE; i++) {
      diskArray[i] = NULL;
   }
}

FILE *myFileOpener(const char *fname, const char *pgmName) {
   char *errMessage;
   FILE* isOpen = fopen(fname, "w+");
   if (isOpen == NULL) {
      errMessage = malloc(sizeof(char) * (strlen(fname) + strlen(pgmName) + 3));
      sprintf(errMessage, "%s: %s", pgmName, fname);
      perror(errMessage);
      free(errMessage);
      exit(EXIT_FAILURE);
   }
   return isOpen;
}

int findDiskNum() {
   int i;
   for (i = 0; i < BLOCKSIZE; i++) {
      if (diskArray[i] == NULL) {
         return i;
      }
   }
   return -1;
}

int openDisk(char *fileName, int nBytes) {
   int i, temp = nBytes, size = 0;
   char *buff;
   if (nBytes == 0) {
      for (i = 0; i < BLOCKSIZE; i++) {
         if (diskArray[i] != NULL) {
            return i;
         }
      }
      return -1;
   }
   else if (nBytes < BLOCKSIZE) {
      return -1;
   }
   if (nBytes % BLOCKSIZE != 0) {
      while (temp > 0) {
         temp = temp - BLOCKSIZE;
         size++;
      }
      nBytes = size * BLOCKSIZE;
   }
   diskNum = findDiskNum();
   if (diskNum < 0) {
      fprintf(stderr, "Out of disks\n");
   }
   diskArray[diskNum] = myFileOpener(fileName, "tinyFS");
//   memset(buff, '\0', nBytes);
  // fwrite(buff, sizeof(char), nBytes, diskArray[diskNum]); //write null characters over first nBytes
   return diskNum;
}

int readBlock(int disk, int bNum, void *block) {
   FILE *fp;
   int bytesRead;
   if (diskArray[disk] == NULL) {
      fprintf(stderr, "Disk not initialized\n");
      return -1;
   }
   fp = diskArray[disk];
   fseek(fp, bNum * BLOCKSIZE, SEEK_SET); 
   bytesRead = fread(block, sizeof(char), BLOCKSIZE, fp);    
   if (bytesRead < BLOCKSIZE) {
      fprintf(stderr, "Insufficent number of bytes read\n");
      return -1;
   }
   return 0;  
}

int writeBlock(int disk, int bNum, void *block) {
   FILE *fp;
   int bytesWritten; 
   if (diskArray[disk] == NULL) {
      fprintf(stderr, "Disk not initialized\n");
      return -1;
   }
   fp = diskArray[disk];
   fseek(fp, bNum * BLOCKSIZE, SEEK_SET);
   bytesWritten = fwrite(block, sizeof(char), BLOCKSIZE, fp);
   if (bytesWritten < BLOCKSIZE) {
      fprintf(stderr, "Insufficent number of bytes written\n");
      return -1;
   }
   return 0;
}

int tfs_mkfs(char *filename, int nBytes) {
   int disk = openDisk(filename, nBytes);
   // init blocks
   // initialize all data 0x00, set magic numbers
   // initializing writing the superblock and inodes   
}

