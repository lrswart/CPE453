#ifndef DISK_H
#define DISK_H

#define BLOCKSIZE 256
typedef struct
{
	char * filename;
	FILE * file;
	int num;
} Disk;

int openDisk(char *filename, int nBytes);

int getDiskNum(char* filename);

int readBlock(int disk, int bNum, void *block);

int writeBlock(int disk, int bNum, void *block);

#endif