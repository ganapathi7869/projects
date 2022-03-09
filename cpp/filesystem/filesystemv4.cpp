#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define NUMOFBLOCKS (100*1024*1024/BLOCKSIZE-1)
//#define BLOCKSIZE 16000
#define NUMOFFILES 32
//#define ROWLEN (sizeof(char)*20+sizeof(int)*3)

char MOUNTEDDISK[20];
unsigned int BLOCKSIZE;
/*
hdd:                 // max 32 files   // 100 MB hdd-6000(data blocks) //10MB hdd-600(data blocks)  //blocksize:16KB
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
void format(){
	struct header *h = (struct header *)getblock(0);
	h->magicnumber = 0x444e524d;
	h->blocksize = BLOCKSIZE;
	h->nooffreeblocks = NUMOFBLOCKS;
	h->nooffiles = 0;
	memset(&(h->startofmetablocks), 0, NUMOFBLOCKS);
	writetoblock((char *)h, 0);
	free(h);
}
void initialiseheader(){
	struct header *h = (struct header *)getblock(0);
	if (h->magicnumber == 0x444e524d) {
		if (h->blocksize != BLOCKSIZE){
			char c;
			printf("The disk is already initialised with another BLOCKSIZE, do you want to format it?\n (y/n):");
			scanf("%c", &c);
			if (c == 'y') format();
		}
	}
	else{
		format();
	}
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
	unsigned int count = 0, i = 0;
	while (count<reqblocks){
		if ((&(h->startofmetablocks))[i] == 0) {
			blocks[count++] = i + 1; // bug
		}
		i++;
	}
	free(h);
	return blocks;
}
void copytodisk(char *source, char *target){
	unsigned int lenoffile, reqblocks, startblock;
	FILE *src = fopen(source, "rb");
	fseek(src, 0, SEEK_END);
	lenoffile = ftell(src);
	reqblocks = lenoffile / BLOCKSIZE + 1 - (!(lenoffile%BLOCKSIZE) ? 1 : 0);
	if (reqblocks > 1) reqblocks += 1;
	//startblock = getstartblock(reqblocks);
	unsigned int *blocks;
	blocks = getfreeblocks(reqblocks);
	/*if (!startblock){
	printf("no space");
	fclose(src);
	return;
	}*/
	startblock = blocks[0];
	insertfiletoheader(target, startblock, reqblocks, lenoffile, blocks);
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
	free(buf);
	fclose(src);
}//"%s \"%*[^\"]\" %s"



void copyfromdisk(char *source, char *target){
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
	if (!(tgt = fopen(target, "wb"))) { printf("error opening file"); return; }
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
	fclose(tgt);
}
void listfiles(){
	struct header *h = (struct header *)getblock(0);
	struct file *files = h->files;
	unsigned int nooffiles = h->nooffiles;
	for (unsigned int i = 0; i < nooffiles; i++){
		printf("%s\n", files[i].filename);
	}
	free(h);
}

void deletefile(char *file){
	struct header *h = (struct header *)getblock(0);
	unsigned int nooffiles = h->nooffiles, i;
	struct file *files = h->files;
	char *metablocks;
	for (i = 0; i < nooffiles; i++){
		if (!strcmp(files[i].filename, file)){
			(h->nooffiles) -= 1;
			(h->nooffreeblocks) -= files[i].noofblocks; // bug
			metablocks = &(h->startofmetablocks);
			if (files[i].noofblocks == 1){
				metablocks[files[i].startblock] = 0; // bug
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
		printf("no such file exists");
	}
	writetoblock((char *)h, 0);
	free(h);
}
void printdebuginfo(){
	struct header *h = (struct header *)getblock(0);
	printf("no of files: %d       no of freeblocks: %d\n", h->nooffiles, h->nooffreeblocks);
	for (unsigned int i = 0; i < h->nooffiles; i++){
		printf("file%d:  %s       filelength:   %d\n", i + 1, h->files[i].filename, h->files[i].filelength);
	}
	free(h);
}
//copytodisk "input.txt" inputhd.txt
//copyfromdisk inputhd.txt "inputnew.txt"
//delete inputhd.txt
int main(){
	char command[50];
	char buf[30];

	while (1){
		printf("Mount a disk with a blocksize\n>");
		scanf("%[^\n]s", command);
		fflush(stdin);
		sscanf(command, "%s", buf);
		if (!strcmp(buf, "mount")){
			char disk[20]; unsigned int blocksize;
			if (sscanf(command, "%*s %s %d", disk, &blocksize) < 2) {
				printf("invalid command format!\n>");
				continue;
			}
			strcpy(MOUNTEDDISK, disk);
			BLOCKSIZE = blocksize;
			initialiseheader();
			printf(">");
			break;
		}
		printf("Invalid Command\n>");
	}
	while (1){
		scanf("%[^\n]s", command);
		fflush(stdin);
		sscanf(command, "%s", buf);
		if (!strcmp(buf, "copytodisk")){
			char source[50], target[20];
			sscanf(command, "%*s \"%[^\"]", source);
			sscanf(command, "%*s \"%*[^\"]\" %s", target);
			copytodisk(source, target);
		}
		else if (!strcmp(buf, "copyfromdisk")){
			char source[20], target[50];
			sscanf(command, "%*s %s", source);
			sscanf(command, "%*s %*s \"%[^\"]", target);
			copyfromdisk(source, target);
		}
		else if (!strcmp(buf, "ls")){
			listfiles();
		}
		else if (!strcmp(buf, "format")){
			unsigned int blocksize;
			if (sscanf(command, "%*s %d", &blocksize) < 1) {
				printf("invalid command format!\n");
				continue;
			}
			BLOCKSIZE = blocksize;
			format();
		}
		else if (!strcmp(buf, "delete")){
			char file[20];
			sscanf(command, "%*s %s", file);
			deletefile(file);
		}
		else if (!strcmp(buf, "debug")){
			printdebuginfo();
		}
		else {
			printf("invalid command\n");
		}
		printf(">");
	}
	return 0;
}