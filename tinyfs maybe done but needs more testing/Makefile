#builds tinyFS.c libDisk.c tinyFS.h and tester.c

tinyFsDemo: tinyFS.c libDisk.c tinyFsDemo.c LinkedList.c
	gcc -o tinyFsDemo -L. -libDisk -libTinyFS tinyFS.c libDisk.c LinkedList.c tinyFsDemo.c -Wall
