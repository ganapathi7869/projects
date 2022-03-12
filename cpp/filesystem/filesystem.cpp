#define _CRT_SECURE_NO_WARNINGS

//Implementation for all file system commands

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include "disk.h"
#define NUMOFFILES 32
#define MAGICNUMBER 0x444e524d

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

// Get the size of a file in bytes
int getfilesize(char *filename){
	FILE *fl = fopen(filename, "rb");
	fseek(fl, 0, SEEK_END);
	return ftell(fl);
}

// Checks whether the given file is a text file
int istextfile(char *filename){
	int len = strlen(filename);
	if (len <= 4) return 0;
	for (int i = 0; filename[i]; i++){
		filename[i] = tolower(filename[i]);
	}
	if (strcmp(filename + len - 4, ".txt")) return 0;
	return 1;
}

// Format the disk with required BLOCKSIZE.
int fs_format(){
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
	struct header *h = (struct header *)disk_read(0);
	h->magicnumber = MAGICNUMBER;
	h->blocksize = BLOCKSIZE;

	h->nooffreeblocks = totalnoofblocks - 1;
	h->nooffiles = 0;
	memset(&(h->startofmetablocks), 0, totalnoofblocks);
	(&(h->startofmetablocks))[0] = 1;
	disk_write((char *)h, 0);
	free(h);
	return 1;
}

// Validates the disk format
int checkformatcorrectness(){
	struct header *h = (struct header *)disk_read(0);
	if (h->magicnumber == MAGICNUMBER) {
		BLOCKSIZE = h->blocksize;
	}
	else{
		printf("The disk format is not recognized. Do you want to format the disk? (y/n):\n");
		char c; scanf("%c", &c); fflush(stdin);
		if (c == 'y') {
			if (fs_format()) printf("SUCCESS: Format was successful!\n");
			else {
				printf("ERROR: Format was unsuccessful!\n");
				return 0;
			}
		}
		else return 0;
	}
	return 1;
}

// Updates header with a new file
void insertfiletoheader(char *target, unsigned int startblock, unsigned int reqblocks, unsigned int lenoffile, unsigned int *blocks){
	struct header *h = (struct header *)disk_read(0);
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
	disk_write((char *)h, 0);
	free(h);
}

// Gives list of available block numbers
unsigned int *getfreeblocks(unsigned int reqblocks){
	struct header *h = (struct header *)disk_read(0);
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

// Copy a file from user's local to the file system(upload).
void fs_copyin(char *source){
	unsigned int lenoffile, reqblocks, startblock;
	FILE *src = fopen(source, "rb");
	lenoffile = (unsigned int)getfilesize(source);
	reqblocks = lenoffile / BLOCKSIZE + 1 - (!(lenoffile%BLOCKSIZE) ? 1 : 0);
	if (reqblocks > 1) reqblocks += 1;
	unsigned int *blocks;
	blocks = getfreeblocks(reqblocks);
	if (blocks == NULL){
		printf("ERROR: Insufficient disk space!\n");
		return;
	}
	startblock = blocks[0];
	insertfiletoheader(source, startblock, reqblocks, lenoffile, blocks);
	char *buf = (char *)malloc(sizeof(char)*BLOCKSIZE);
	fseek(src, 0, SEEK_SET);
	if (reqblocks == 1){
		fread(buf, sizeof(char), BLOCKSIZE, src);
		disk_write(buf, startblock);
	}
	else{
		memcpy(buf, &blocks[1], sizeof(unsigned int)*(reqblocks - 1));
		disk_write(buf, startblock);
		for (unsigned int i = 1; i < reqblocks; i++){
			fread(buf, sizeof(char), BLOCKSIZE, src);
			disk_write(buf, blocks[i]);
		}
	}
	printf("SUCCESS: File copied to the disk successfully!\n");
	free(buf);
	fclose(src);
}

// Copy a file from file system to the user's local(download).
void fs_copyout(char *source, char *target){
	struct header *h = (struct header *)disk_read(0);
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
	char *indexblock = disk_read(startblock);
	if (noofblocks == 1){
		fwrite(indexblock, sizeof(char), filelength, tgt);
	}
	else{
		char *buf;
		unsigned int *nextblock = (unsigned int *)indexblock;
		for (unsigned int i = 0; i < noofblocks - 1; i++){
			buf = disk_read(nextblock[i]);
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

// Print the content of a text file.
void fs_cat(char *file){
	if (!istextfile(file)){
		printf("ERROR: cat command can only be used with '.txt' files\n");
		return;
	}
	struct header *h = (struct header *)disk_read(0);
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
	char *indexblock = disk_read(startblock);
	if (noofblocks == 1){
		strcat(indexblock, " "); indexblock[filelength] = NULL;
		printf("%s", indexblock);
	}
	else{
		char *buf;
		unsigned int *nextblock = (unsigned int *)indexblock;
		for (unsigned int i = 0; i < noofblocks - 1; i++){
			buf = disk_read(nextblock[i]); strcat(buf, " ");
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

// Delete a file from the file system.
int fs_deletefile(char *file){
	struct header *h = (struct header *)disk_read(0);
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
				unsigned int *indexblock = (unsigned int *)disk_read(files[i].startblock);
				for (unsigned int j = 0; j < files[i].noofblocks - 1; j++){
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
	disk_write((char *)h, 0);
	free(h);
	return 1;
}

// List all the files available in the file system
void fs_listfiles(){
	struct header *h = (struct header *)disk_read(0);
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

// Prints the content of a text file.
void fs_printdebuginfo(){
	struct header *h = (struct header *)disk_read(0);
	printf("no of files: %d       no of freeblocks: %d		BLOCKSIZE: %d KB\n", h->nooffiles, h->nooffreeblocks, (h->blocksize) / 1024);
	for (unsigned int i = 0; i < h->nooffiles; i++){
		printf("file-%d:  \"%s\"       filesize: %d bytes		starting block: %d		no of blocks: %d\n", i + 1, h->files[i].filename, h->files[i].filelength, h->files[i].startblock, h->files[i].noofblocks);
	}
	free(h);
}
