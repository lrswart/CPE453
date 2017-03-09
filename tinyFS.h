#ifndef
#define

#define BLOCKSIZE 256

int openDisk(char *fileName, int nBytes);
int readBlock(int disk, int bNum, void *block);
int writeBlock(int disk, int bNum, void *block);


#endif