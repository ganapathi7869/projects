# File System

**Introduction:**  A simplified UNIX like file system based on C language using a .hdd file as a hard disk. The file system uses the following strategies:
 - Simplified Inode data structure
 - Non-contiguous memory allocation to avoid fragmentation issues.
 - Block-IO operations with the disk
```
Commands:
---------
mount			- Mount a disk
copyin			- Copy a file from user's local to the file system(upload).
copyout			- Copy a file from file system to the user's local(download).
ls				- List all the files available in the file system
cat				- Prints the content of a text file.
format			- Format the disk with required BLOCKSIZE.
deletefile		- Delete a file from the file system.
debug			- Show all the information related to file system, files and their blocks.
unmount			- Unmount already mounted disk.
help			- Show all the commands and their format.
exit			- Exit from the application.
```

The file system consists of 3 main components:
 1. **Shell** (layer 1): A shell application that allows the user to perform operations on the ***file system*** such as printing debugging information about the file system, formatting a new file system, mounting a file system, creating files, and copying data in or out of the file system. To do this, it will translate the user commands into file system operations such as `fs_printdebuginfo`, `fs_format`, `fs_listfiles`, `fs_copyin`, `fs_copyout`, `fs_cat`,`fs_deletefile`,...
 2. **File System** (layer 2): This component takes the operations specified by the user through the  ***shell***  and performs them on the  ***file System***  disk image. This component is charged with organizing the on-disk data structures and performing all the bookkeeping necessary to allow for persistent storage of data. To store the data, it will need to interact with the  ***disk emulator***  via functions such as  `disk_read`  and  `disk_write`, which allow the file system read and write to the disk image in  `16KB`  blocks.
 3. **Disk Emulator** (layer 3): The third component emulates a disk by dividing a normal file (called a  ***disk image***) into  `16KB`  blocks and only allows the  ***file system***  to read and write in terms of blocks. This emulator will persistently store the data to the disk image using the system calls.
