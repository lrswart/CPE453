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
   char name[8];
   char c;
   
   check = tfs_mkfs("disk1", DEFAULT_DISK_SIZE*5);
   
   assert(check == 0);
   
   check = tfs_mkfs("disk2", DEFAULT_DISK_SIZE*5);
   
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
   printf("\n\nWriting Files..........................................................\n");
   printf("Notice files 1-4 are in the file table, and have index blocks pointing to their data blocks\n\n");
   tfs_writeFile(fd1, buf1, 500);
   tfs_writeFile(fd2, buf1, 500);
   tfs_writeFile(fd3, buf2, 150);
   tfs_writeFile(fd4, buf2, 150);
   

   printBlocks(12);   //print the first five blocks of the disk to make sure index files were properly created
   
   
   
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
   
   printf("\n\nDeleting Files..........................................................\n");
   printf("Notice files 1 and 2 are no longer in the file table, and no longer have index blocks or data blocks.\n\n");

   tfs_deleteFile(fd1);
   tfs_deleteFile(fd2);
   
   printBlocks(12);     
   //this print out shows that the files were moved up in the file table and that the free
   //blocks properly point to each other after two files have been deleted
   
   tfs_deleteFile(fd3);
   tfs_deleteFile(fd4);
   
   
   
   printf("part 1 complete\n");
   
   // Testing Read Only functions
   
   printf("\ntesting Read Only stuff........................................\n");
   printf("Notice file1 has READ ONLY attribute. Attempts to write to it fail.\n\n");

   
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
   
   
   printf("\ntesting Write Byte........................................\n");
   printf("Extra byte added in file1's data block between lowercase c and d, look closely, it's there\n");
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
   
   
   printBlocks(15);
   
   printf("\n\nDone checking Read/Write permissions and writeByte()\n\n");
   
   
   printf("Testing Defrag.....................................................\n\n");
   printf("Here the tfs_displayFragments() function is used to show the defragmentation. 0 for free blocks, other numbers for other block types\n");
   
   printf("\nBefore:\n");
   tfs_displayFragments();
   printf("\nAfter:\n");
   tfs_defrag();
   tfs_displayFragments();
   
   printf("\n\nPrint out the blocks to show their data hasn't been altered by the defrag...............................\n\n");
   printBlocks(10);
   
   printf("\n\nTesting Rename................................................................\n");
   tfs_rename(fd1, "new_name");
   printf("\nnotice the name change for file1 in the  file table\n\n");
   printBlocks(6);
   
   printf("\n\nTesting readdir............................................................\n\n");
   printf("\nCreated 50 new files to show that they all print out in a list when tfs_readdir is called..............\n");
   
   strcpy(name, "file_ ");
   c = 'A';
   for(i=0; i<50; i++)
   {
      name[5] = c;
      tfs_closeFile(tfs_openFile(name));
      c += 1;
   }
   
   tfs_readdir();
   
   
   printf("\nEnd of demonstration\n");
}




