#define _CRT_SECURE_NO_WARNINGS

//Disk I/O

#include<stdio.h>
#include<stdlib.h>

char MOUNTEDDISK[20];
unsigned int BLOCKSIZE = 16 * 1024;

// Get the content of a block
char *disk_read(unsigned int block){
	FILE *hd = fopen(MOUNTEDDISK, "rb+");
	fseek(hd, block*BLOCKSIZE, SEEK_SET);
	char *buf = (char *)malloc(sizeof(char)*BLOCKSIZE);
	fread(buf, sizeof(char), BLOCKSIZE, hd);
	fclose(hd);
	return buf;
}

// Update the content of a block
void disk_write(char *buf, unsigned int block){
	FILE *hd = fopen(MOUNTEDDISK, "rb+");
	fseek(hd, block*BLOCKSIZE, SEEK_SET);
	fwrite(buf, sizeof(char), BLOCKSIZE, hd);
	fclose(hd);
}
