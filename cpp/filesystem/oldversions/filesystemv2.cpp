#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define NUMOFBLOCKS 6000
#define BLOCKSIZE 16000
#define NUMOFFILES 32
#define HEADERLEN 4
#define ROWLEN (sizeof(char)*20+sizeof(int)*3)
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
	unsigned int startblock;
	unsigned int noofblocks;
	unsigned int filelength;
};
struct header{
	unsigned int magicnumber;
	unsigned int nooffiles;
	unsigned int nooffreeblocks;
	struct file files[NUMOFFILES];
	char metablocks[NUMOFBLOCKS];
};
char *getblock(unsigned int block){
	FILE *hd = fopen("harddisk.hdd", "rb+");
	fseek(hd, block*BLOCKSIZE, SEEK_SET);
	char *buf = (char *)malloc(sizeof(char)*BLOCKSIZE);
	fread(buf, sizeof(char), BLOCKSIZE, hd);
	fclose(hd);
	return buf;
}
void writetoblock(char *buf, unsigned int block){
	FILE *hd = fopen("harddisk.hdd", "rb+");
	fseek(hd, block*BLOCKSIZE, SEEK_SET);
	fwrite(buf, sizeof(char), BLOCKSIZE, hd);
	fclose(hd);
}
void initialiseheader(){
	char *buf=(char *)calloc(BLOCKSIZE,sizeof(char));
	struct header *h=(struct header *)buf;
	h->magicnumber = 0x444e524d;
	h->nooffreeblocks = NUMOFBLOCKS;
	writetoblock((char *)h, 0);
	free(buf);
}
unsigned int getstartblock(unsigned int reqblocks){
	struct header *buf = (struct header *)getblock(0);
	char *metablocks = buf->metablocks;
	unsigned int startblock = 0,consecutivezeroes=0;
	for (unsigned int i = 0; i < NUMOFBLOCKS; i++){
		if (metablocks[i] == 0){
			consecutivezeroes += 1;
		}
		else{
			consecutivezeroes = 0;
			startblock = 0;
		}
		if (consecutivezeroes == 1) startblock = i+1;
		if (consecutivezeroes == reqblocks) break;
	}
	free(buf);
	return startblock;
}
void insertfiletoheader(char *target, unsigned int startblock, unsigned int reqblocks, unsigned int lenoffile){
	struct header *h = (struct header *)getblock(0);
	(h->nooffiles) += 1;
	(h->nooffreeblocks) -= reqblocks;
	struct file *f=&((h->files)[h->nooffiles - 1]);
	strcpy(f->filename, target);
	f->startblock = startblock;
	f->noofblocks = reqblocks;
	f->filelength = lenoffile;
	for (unsigned int i = 0; i < reqblocks; i++){
		h->metablocks[startblock + i] = 1;
	}
	writetoblock((char *)h, 0);
	free(h);
}
void copytodisk(char *source, char *target){
	unsigned int lenoffile,reqblocks,startblock;
	FILE *src = fopen(source, "rb");
	fseek(src, 0, SEEK_END);
	lenoffile = ftell(src)+1;
	reqblocks = lenoffile / BLOCKSIZE + 1 - (!(lenoffile%BLOCKSIZE) ? 1 : 0);
	startblock = getstartblock(reqblocks);
	if (!startblock){
		printf("no space");
		fclose(src);
		return;
	}
	insertfiletoheader(target, startblock, reqblocks, lenoffile);
	char buf[BLOCKSIZE];
	fseek(src, 0, SEEK_SET);
	for (unsigned int i = 0; i < reqblocks; i++){
		fread(buf, sizeof(char), BLOCKSIZE, src);
		writetoblock(buf, startblock + i);
	}
	fclose(src);
}//"%s \"%*[^\"]\" %s"



void copyfromdisk(char *source, char *target){
	struct header *h = (struct header *)getblock(0);
	unsigned int nooffiles = h->nooffiles,startblock,noofblocks,filelength;
	struct file *files = h->files;
	for (unsigned int i = 0; i < nooffiles; i++){
		if (!strcmp(files[i].filename, source)){
			startblock=files[i].startblock;
			noofblocks = files[i].noofblocks;
			filelength = files[i].filelength;
			break;
		}
	}
	free(h);
	FILE *tgt;
	if (!(tgt = fopen(target, "wb"))) { printf("error opening file"); return; }
	char *buf=getblock(startblock);
	for (unsigned int i = 0; i < noofblocks; i++){
		if (i == noofblocks - 1){
			fwrite(buf, sizeof(char), filelength%BLOCKSIZE, tgt);
			free(buf);
			break;
		}
		else{
			fwrite(buf, sizeof(char), BLOCKSIZE, tgt);
		}
		free(buf);
		buf = getblock(startblock + i);
	}
	fclose(tgt);
	free(h);
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
void format(){
	initialiseheader();
}
void deletefile(char *file){
	struct header *h = (struct header *)getblock(0);
	unsigned int nooffiles = h->nooffiles,i;
	struct file *files = h->files;
	char *metablocks;
	for (i = 0; i < nooffiles; i++){
		if (!strcmp(files[i].filename, file)){
			(h->nooffiles) -= 1;
			(h->nooffreeblocks) -= files[i].noofblocks;
			metablocks = h->metablocks;
			for (unsigned int j = 0; j < files[i].noofblocks; j++){
				metablocks[files[i].startblock + j] = 0;
			}
			strcpy(files[i].filename, files[h->nooffiles].filename);
			files[i].startblock = files[h->nooffiles].startblock;
			files[i].noofblocks = files[h->nooffiles].noofblocks;
			files[i].filelength = files[h->nooffiles].filelength;
			break;
		}
	}
	writetoblock((char *)h, 0);
	if (i == nooffiles) {
		printf("no such file exists");
	}
	free(h);
}
void printdebuginfo(){
	struct header *h = (struct header *)getblock(0);
	printf("no of files: %d       no of freeblocks: %d\n", h->nooffiles, h->nooffreeblocks);
	for (unsigned int i = 0; i < h->nooffiles; i++){
		printf("file%d:  %s       filelength:   %d",i+1, h->files[i].filename, h->files[i].filelength);
	}
	free(h);
}
//copytodisk "input.txt" inputhd.txt
//copyfromdisk inputhd.txt "inputnew.txt"
int main(){
	char command[50];					
	char buf[30];                       
	printf(">");
	initialiseheader();
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