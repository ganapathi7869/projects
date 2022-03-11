#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "filesystem.h"
#include "disk.h"


/*
BLOCKSIZE = 16 KB (default)

Commands:
---------
mount		- Mount a disk
copyin		- Copy a file from user's local to the file system(upload).
copyout		- Copy a file from file system to the user's local(download).
ls			- List all the files available in the file system
cat			- Prints the content of a text file.
format		- Format the disk with required BLOCKSIZE.
deletefile	- Delete a file from the file system.
debug		- Show all the information related to file system, files and their blocks.
unmount		- Unmount already mounted disk.
help		- Show all the commands and their format.
exit		- Exit from the application.
*/


void printhelp(){
	printf("List of commands:\n");
	printf("----------------------\n");
	printf("copyin \"<filename>\"\n");
	printf("copyout \"<filename>\"\n");
	printf("ls\n");
	printf("cat \"<filename>\"\n");
	printf("format\n");
	printf("deletefile \"<filename>\"\n");
	printf("debug\n");
	printf("unmount\n");
	printf("help\n");
	printf("exit\n");
}

int main(){
	char command[50];
	char buf[30];

	// mount
	while (1){
		printf("Mount a disk - Command Format: mount \"<diskname>\"\n>");
		scanf("%[^\n]s", command);
		fflush(stdin);
		sscanf(command, "%s", buf);
		if (!strcmp(buf, "mount")){
			char diskname[20];
			if (sscanf(command, "%*s \"%[^\"]", diskname) < 1) {
				printf("ERROR: invalid command format!\n>");
				continue;
			}
			strcpy(MOUNTEDDISK, diskname);

			if (checkformatcorrectness()) {
				printf("SUCCESS: Disk mounted successfully!\n");
				printf("Type 'help' for list of commands\n>");
			}
			else {
				printf("ERROR: Disk mounting unsuccessful!\n>");
				continue;
			}
		}
		else{
			printf("ERROR: Invalid Command - Please mount a disk!\n>");
			continue;
		}

		// execute commands on mounted disk
		while (1){
			scanf("%[^\n]s", command);
			fflush(stdin);
			sscanf(command, "%s", buf);
			if (!strcmp(buf, "copyin")){
				char source[50];
				sscanf(command, "%*s \"%[^\"]", source);
				copyin(source);
			}
			else if (!strcmp(buf, "copyout")){
				char source[20], target[27] = "output-";
				sscanf(command, "%*s \"%[^\"]", source);
				strcat(target, source);
				copyout(source, target);
			}
			else if (!strcmp(buf, "ls")){
				listfiles();
			}
			else if (!strcmp(buf, "cat")){
				char source[20];
				sscanf(command, "%*s \"%[^\"]", source);
				cat(source);
			}
			else if (!strcmp(buf, "format")){
				if (format()) printf("SUCCESS: Format was successful!\n");
				else printf("ERROR: Format was unsuccessful!\n");
			}
			else if (!strcmp(buf, "deletefile")){
				char file[20];
				sscanf(command, "%*s \"%[^\"]", file);
				if (deletefile(file)) printf("SUCCESS: File deleted successfully!\n");
				else printf("ERROR: Couldn't delete file!\n");
			}
			else if (!strcmp(buf, "debug")){
				printdebuginfo();
			}
			else if (!strcmp(buf, "unmount")){
				printf("SUCCESS: Disk unmounted successfully!\n");
				break;
			}
			else if (!strcmp(buf, "help")){
				printhelp();
			}
			else if (!strcmp(buf, "exit")){
				exit(0);
			}
			else {
				printf("ERROR: invalid command\n");
			}
			printf(">");
		}
	}

	return 0;
}
