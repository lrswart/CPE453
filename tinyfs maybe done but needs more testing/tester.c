#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tinyFS.h"


int main()
{
   int check, i, fd1, fd2, fd3, fd4;
   char buf1[500];
   char buf2[150];
   char c;
   
   check = tfs_mkfs("disk1", DEFAULT_DISK_SIZE);
   
   assert(check == 0);
   
   check = tfs_mkfs("disk2", DEFAULT_DISK_SIZE);
   
   assert(check == 0);
   
   //check mount and unmount
   check = tfs_mount("disk1");
   assert(check == 0);
   check = tfs_unmount();
   assert(check == 0);
   check = tfs_mount("disk2");
   assert(check == 0);
   
   //open four files for testing
   fd1 = tfs_openFile("file1");
   fd2 = tfs_openFile("file2");
   fd3 = tfs_openFile("file3");
   fd4 = tfs_openFile("file4");
   
   //make sure fds are appropriate
   assert(fd1 >= 0);
   assert(fd2 >= 0);
   assert(fd3 >= 0);
   assert(fd4 >= 0);
   
   //here we initialize buf1 and buf2 with stuff
   for(i=0; i<500; i++)
   {
      if(i % 2)
      {
         buf1[i] = 'a';
      }
      else
      {
         buf1[i] = 'b';
      }
   }
   
   for(i=0; i<150; i++)
   {
      buf2[i] = i;
   }
   
   //then we write that stuff to the files
   printf("\n\nWriting Files..........................................................\n\n");
   tfs_writeFile(fd1, buf1, 500);
   tfs_writeFile(fd2, buf1, 500);
   tfs_writeFile(fd3, buf2, 150);
   tfs_writeFile(fd4, buf2, 150);
   

   printBlocks(5);   //print the first five blocks of the disk to make sure index files were properly created
   
   
   
   //here we confirm that all bits were properly written to the disk and can be properly read back from the disk
   for(i=0; i<500; i++)
   {
      tfs_readByte(fd1, &c);
      assert(c == buf1[i]);
      
      tfs_readByte(fd2, &c);
      assert(c == buf1[i]);
   }
   
   for(i=0; i<150; i++)
   {
      tfs_readByte(fd3, &c);
      assert(c == buf2[i]);
      
      tfs_readByte(fd4, &c);
      assert(c == buf2[i]); 
   }
   
   //close all the files
   check = tfs_closeFile(fd1);
   assert(check == 0);
   check = tfs_closeFile(fd2);
   assert(check == 0);
   check = tfs_closeFile(fd3);
   assert(check == 0);
   check = tfs_closeFile(fd4);
   assert(check == 0);
   
   //reopen all the files
   fd4 = tfs_openFile("file4");
   fd1 = tfs_openFile("file1");  
   fd3 = tfs_openFile("file3");
   fd2 = tfs_openFile("file2");
   
   
   //make sure files opened properly and all data remained by running same test as before files
   //were closed and reopened
   for(i=0; i<500; i++)
   {
      tfs_readByte(fd1, &c);
      assert(c == buf1[i]);
      
      tfs_readByte(fd2, &c);
      assert(c == buf1[i]);
   }
   
   for(i=0; i<150; i++)
   {
      tfs_readByte(fd3, &c);
      assert(c == buf2[i]);
      
      tfs_readByte(fd4, &c);
      assert(c == buf2[i]); 
   }
   
   printf("\n\nDeleting Files..........................................................\n\n");
   tfs_deleteFile(fd1);
   tfs_deleteFile(fd2);
   
   printBlocks(10);     
   //this print out shows that the files were moved up in the file table and that the free
   //blocks properly point to each other after two files have been deleted
   
   tfs_deleteFile(fd3);
   tfs_deleteFile(fd4);
   
   
   
   printf("part 1 complete\n");
   
   // Testing Read Only functions
   
   printf("\ntesting Read Only stuff........................................\n\n");
   
   //make files 1 and 2 like before
   fd1 = tfs_openFile("file1");
   fd2 = tfs_openFile("file2");
   
   tfs_writeFile(fd1, buf1, 500);
   tfs_writeFile(fd2, buf1, 500);
   
   //make file 1 read only
   tfs_makeRO("file1");
   
   printBlocks(12); //print blocks to show Read Only field
   
   //check that we can still read read only file fine
   for(i=0; i<500; i++)
   {
      tfs_readByte(fd1, &c);
      assert(c == buf1[i]);
      
      tfs_readByte(fd2, &c);
      assert(c == buf1[i]);
   }
   
   //make sure writing to read only file fails
   if(tfs_writeFile(fd1, buf2, 150) > 0)
   {
      printf("wrote to a read only file\n");
      exit(1);
   }
   
   // Test give read write permission back
   tfs_makeRW("file1");
   
   if(tfs_writeFile(fd1, buf2, 150) < 0)
   {
      printf("write to read/write permission file failed\n");
      exit(1);
   }
   
   
   
   check = tfs_writeByte(fd1, 100, 151); //write 151 in the 100th position of file1
   assert(check == 0);
   
   //check first 100 bytes
   for(i-0; i<100; i++)
   {
      tfs_readByte(fd1, &c);
      assert(c == buf1[i]);
   }
   tfs_seek(fd1, 100); //writing a byte resets the file seek
   //check byte at position 100
   tfs_readByte(fd1, &c);
   assert(c == (char)151);
   
   //check last 50 bytes
   for(i=101; i<151; i++)
   {
      tfs_readByte(fd1, &c);
      assert(c == (char)(i-1));
   }
   
   printf("\n\nDone checking Read/Write permissions and writeByte()\n\n");
   
   printBlocks(15);
   
   printf("Testing Defrag..................................\n\n");
   tfs_defrag();
   
   printBlocks(15);
   
   
}




