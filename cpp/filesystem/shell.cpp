#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

//#define BLOCKSIZE 16000
#define NUMOFFILES 32
//#define ROWLEN (sizeof(char)*20+sizeof(int)*3)
#define MAGICNUMBER 0x444e524d
char MOUNTEDDISK[20];
unsigned int BLOCKSIZE = 16 * 1024; // default: 16KB
/*
hdd:                  // 100 MB hdd-6000(data blocks) //10MB hdd-600(data blocks)  //blocksize:16KB
444e525d (4 Bytes)
B1:noooffiles nooffreeblocks
B2:table (filename startblock #blocks filelength)
B3:char array of block presence
////
data:
presence    //presence:- 0:no entry 1:present 2:deleted
*/
struct file{
	char filename[20];
	unsigned int noofblocks;
	unsigned int startblock;
	unsigned int filelength;
};
struct header{
	unsigned int magicnumber;
	unsigned int blocksize;
	unsigned int nooffiles;
	unsigned int nooffreeblocks;
	struct file files[NUMOFFILES];
	char startofmetablocks;
};
char *getblock(unsigned int block){
	FILE *hd = fopen(MOUNTEDDISK, "rb+");
	fseek(hd, block*BLOCKSIZE, SEEK_SET);
	char *buf = (char *)malloc(sizeof(char)*BLOCKSIZE);
	fread(buf, sizeof(char), BLOCKSIZE, hd);
	fclose(hd);
	return buf;
}
void writetoblock(char *buf, unsigned int block){
	FILE *hd = fopen(MOUNTEDDISK, "rb+");
	fseek(hd, block*BLOCKSIZE, SEEK_SET);
	fwrite(buf, sizeof(char), BLOCKSIZE, hd);
	fclose(hd);
}
int getfilesize(char *filename){
	FILE *file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	return ftell(file);
}
int format(){
	printf("Do you want to continue with the default block size: 16KB? (y/n):\n");
	char c; scanf("%c", &c); fflush(stdin);
	if (c == 'n') {
		printf("Please enter the block size(atleast 2KB) (in KB): \n");
		scanf("%u", &BLOCKSIZE);
		BLOCKSIZE *= 1024;
	}
	else{
		BLOCKSIZE = 16 * 1024;
	}
	int totalnoofblocks = getfilesize(MOUNTEDDISK) / BLOCKSIZE - 1;
	if (totalnoofblocks < 1){
		printf("ERROR: Insufficient space in the hard disk!\n");
		return 0;
	}
	struct header *h = (struct header *)getblock(0);
	h->magicnumber = MAGICNUMBER;
	h->blocksize = BLOCKSIZE;

	h->nooffreeblocks = totalnoofblocks - 1;
	h->nooffiles = 0;
	memset(&(h->startofmetablocks), 0, totalnoofblocks);
	(&(h->startofmetablocks))[0] = 1;
	writetoblock((char *)h, 0);
	free(h);
	return 1;
}
int checkformat(){
	struct header *h = (struct header *)getblock(0);
	if (h->magicnumber == MAGICNUMBER) {
		BLOCKSIZE = h->blocksize;
	}
	else{
		printf("The disk format is not recognized. Do you want to format the disk? (y/n):\n");
		char c; scanf("%c", &c); fflush(stdin);
		if (c == 'y') {
			if (format()) printf("SUCCESS: Format was successful!\n");
			else {
				printf("ERROR: Format was unsuccessful!\n");
				return 0;
			}
		}
		else return 0;
	}
	return 1;
}

void insertfiletoheader(char *target, unsigned int startblock, unsigned int reqblocks, unsigned int lenoffile, unsigned int *blocks){
	struct header *h = (struct header *)getblock(0);
	(h->nooffiles) += 1;
	(h->nooffreeblocks) -= reqblocks;
	struct file *f = &((h->files)[h->nooffiles - 1]);
	strcpy(f->filename, target);
	f->startblock = startblock;
	f->noofblocks = reqblocks;
	f->filelength = lenoffile;
	for (unsigned int i = 0; i < reqblocks; i++){
		(&(h->startofmetablocks))[blocks[i]] = 1;
	}
	writetoblock((char *)h, 0);
	free(h);
}
unsigned int *getfreeblocks(unsigned int reqblocks){
	struct header *h = (struct header *)getblock(0);
	unsigned int *blocks = (unsigned int *)malloc(sizeof(int)*reqblocks);
	if (h->nooffreeblocks < reqblocks) return NULL;
	unsigned int count = 0, i = 0;
	while (count<reqblocks){
		if ((&(h->startofmetablocks))[i] == 0) {
			blocks[count++] = i;
		}
		i++;
	}
	free(h);
	return blocks;
}
void copyin(char *source){
	unsigned int lenoffile, reqblocks, startblock;
	FILE *src = fopen(source, "rb");
	lenoffile = (unsigned int)getfilesize(source);
	reqblocks = lenoffile / BLOCKSIZE + 1 - (!(lenoffile%BLOCKSIZE) ? 1 : 0);
	if (reqblocks > 1) reqblocks += 1;
	//startblock = getstartblock(reqblocks);
	unsigned int *blocks;
	blocks = getfreeblocks(reqblocks);
	if (blocks == NULL){
		printf("ERROR: Insufficient disk space!\n");
		return;
	}
	/*if (!startblock){
	printf("no space");
	fclose(src);
	return;
	}*/
	startblock = blocks[0];
	insertfiletoheader(source, startblock, reqblocks, lenoffile, blocks);
	char *buf = (char *)malloc(sizeof(char)*BLOCKSIZE);
	fseek(src, 0, SEEK_SET);
	if (reqblocks == 1){
		fread(buf, sizeof(char), BLOCKSIZE, src);
		writetoblock(buf, startblock);
	}
	else{
		memcpy(buf, &blocks[1], sizeof(unsigned int)*(reqblocks - 1));
		writetoblock(buf, startblock);
		for (unsigned int i = 1; i < reqblocks; i++){
			fread(buf, sizeof(char), BLOCKSIZE, src);
			writetoblock(buf, blocks[i]);
		}
	}
	/*for (unsigned int i = 0; i < reqblocks; i++){
	fread(buf, sizeof(char), BLOCKSIZE, src);
	writetoblock(buf, startblock + i);
	}*/
	printf("SUCCESS: File copied to the disk successfully!\n");
	free(buf);
	fclose(src);
}//"%s \"%*[^\"]\" %s"



void copyout(char *source, char *target){
	struct header *h = (struct header *)getblock(0);
	unsigned int nooffiles = h->nooffiles, startblock, noofblocks, filelength;
	struct file *files = h->files;
	for (unsigned int i = 0; i < nooffiles; i++){
		if (!strcmp(files[i].filename, source)){
			startblock = files[i].startblock;
			noofblocks = files[i].noofblocks;
			filelength = files[i].filelength;
			break;
		}
	}
	free(h);
	FILE *tgt;
	if (!(tgt = fopen(target, "wb"))) { printf("ERROR: error opening file\n"); return; }
	char *indexblock = getblock(startblock);
	if (noofblocks == 1){
		fwrite(indexblock, sizeof(char), filelength, tgt);
	}
	else{
		char *buf;
		unsigned int *nextblock = (unsigned int *)indexblock;
		for (unsigned int i = 0; i < noofblocks - 1; i++){
			buf = getblock(nextblock[i]);
			if (i == noofblocks - 2){
				if (filelength%BLOCKSIZE) fwrite(buf, sizeof(char), filelength%BLOCKSIZE, tgt);
				else fwrite(buf, sizeof(char), BLOCKSIZE, tgt);
			}
			else{
				fwrite(buf, sizeof(char), BLOCKSIZE, tgt);
			}
			free(buf);
		}
	}
	printf("SUCCESS: File copied from the disk to the local machine successfully!\n");
	fclose(tgt);
}
int istextfile(char *filename){
	int len = strlen(filename);
	if (len <= 4) return 0;
	for (int i = 0; filename[i]; i++){
		filename[i] = tolower(filename[i]);
	}
	if (strcmp(filename + len - 4, ".txt")) return 0;
	return 1;
}
void cat(char *file){
	if (!istextfile(file)){
		printf("ERROR: cat command can only be used with '.txt' files\n");
		return;
	}
	struct header *h = (struct header *)getblock(0);
	unsigned int nooffiles = h->nooffiles, startblock, noofblocks, filelength;
	struct file *files = h->files;
	for (unsigned int i = 0; i < nooffiles; i++){
		if (!strcmp(files[i].filename, file)){
			startblock = files[i].startblock;
			noofblocks = files[i].noofblocks;
			filelength = files[i].filelength;
			break;
		}
	}
	free(h);
	char *indexblock = getblock(startblock);
	if (noofblocks == 1){
		strcat(indexblock, " "); indexblock[filelength] = NULL;
		printf("%s", indexblock);
	}
	else{
		char *buf;
		unsigned int *nextblock = (unsigned int *)indexblock;
		for (unsigned int i = 0; i < noofblocks - 1; i++){
			buf = getblock(nextblock[i]); strcat(buf, " ");
			if (i == noofblocks - 2){
				if (filelength%BLOCKSIZE) {
					buf[filelength%BLOCKSIZE] = NULL;
				}
				else buf[BLOCKSIZE] = NULL;
			}
			else{
				buf[BLOCKSIZE] = NULL;
			}
			printf("%s", buf);
			free(buf);
		}
	}
	printf("\nEnd of File reached!\n");
}


int deletefile(char *file){
	struct header *h = (struct header *)getblock(0);
	unsigned int nooffiles = h->nooffiles, i;
	struct file *files = h->files;
	char *metablocks;
	for (i = 0; i < nooffiles; i++){
		if (!strcmp(files[i].filename, file)){
			(h->nooffiles) -= 1;
			(h->nooffreeblocks) += files[i].noofblocks;
			metablocks = &(h->startofmetablocks);
			if (files[i].noofblocks == 1){
				metablocks[files[i].startblock] = 0;
			}
			else{
				metablocks[files[i].startblock] = 0;
				unsigned int *indexblock = (unsigned int *)getblock(files[i].startblock);
				for (int j = 0; j < files[i].noofblocks - 1; j++){
					metablocks[indexblock[j]] = 0;
				}
			}
			strcpy(files[i].filename, files[h->nooffiles].filename);
			files[i].startblock = files[h->nooffiles].startblock;
			files[i].noofblocks = files[h->nooffiles].noofblocks;
			files[i].filelength = files[h->nooffiles].filelength;
			break;
		}
	}
	if (i == nooffiles) {
		printf("ERROR: no such file exists\n");
		return 0;
	}
	writetoblock((char *)h, 0);
	free(h);
	return 1;
}
void listfiles(){
	struct header *h = (struct header *)getblock(0);
	struct file *files = h->files;
	unsigned int nooffiles = h->nooffiles;
	if (nooffiles == 0){
		printf("ERROR: Disk has no files to show!\n");
	}
	for (unsigned int i = 0; i < nooffiles; i++){
		printf("%d)  \"%s\"       filesize: %d bytes\n", i + 1, h->files[i].filename, h->files[i].filelength);
	}
	free(h);
}
void printdebuginfo(){
	struct header *h = (struct header *)getblock(0);
	printf("no of files: %d       no of freeblocks: %d		BLOCKSIZE: %d KB\n", h->nooffiles, h->nooffreeblocks, (h->blocksize) / 1024);
	for (unsigned int i = 0; i < h->nooffiles; i++){
		printf("file-%d:  \"%s\"       filesize: %d bytes		starting block: %d		no of blocks: %d\n", i + 1, h->files[i].filename, h->files[i].filelength, h->files[i].startblock, h->files[i].noofblocks);
	}
	free(h);
}
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
}
//copytodisk "input.txt" inputhd.txt
//copyfromdisk inputhd.txt "inputnew.txt"
//delete inputhd.txt
int main(){
	char command[50];
	char buf[30];

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

			if (checkformat()) {
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
			else {
				printf("ERROR: invalid command\n");
			}
			printf(">");
		}
	}

	return 0;
}