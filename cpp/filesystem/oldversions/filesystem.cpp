#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*
hdd:                             // max 100 files each 1kB max  // 10 MB hdd
storage fre
presence filename start length   //presence:- 0:no entry 1:present 2:deleted  
*/
struct metadata{         //size: char*16 + int*2 
	char filename[16];
	unsigned int start;
	unsigned int length;
};
unsigned int writefiletohdd(FILE *src, FILE *hd, unsigned int start){
	unsigned int written,lengthoffile=0;
	char buf[100];
	written = fread(buf, sizeof(char), 100, src);
	fseek(hd, start, SEEK_SET);
	while (written){
		fwrite(buf, sizeof(char), written, hd);
		lengthoffile += written;
		written = fread(buf, sizeof(char), 100, src);
	}
	return lengthoffile;
}
void copytodisk(char *source, char *target){
	unsigned int storage, fre;
	FILE *src , *hd;
	if (!(src = fopen(source, "rb")  ))     { printf("error opening file"); return; }
	if (!(hd = fopen("harddisk.hdd", "rb+")  ))   { printf("error opening file"); return; }
	char filenamebuf[16];
	unsigned int intbuf1=0,intbuf2=0;
	char cbuf; 
	fread(&storage, sizeof(int), 1, hd);
	fread(&fre, sizeof(int), 1, hd);
	if (!storage){
		fseek(hd, 0, SEEK_SET);
		storage = 2*sizeof(int) + 100 * (16*sizeof(char) + sizeof(int) + sizeof(int) + sizeof(char));
		fre = 10000000 - storage;
		fwrite(&storage, sizeof(int), 1, hd);
		fwrite(&fre, sizeof(int), 1, hd);
		intbuf1 = storage;
		//printf("%x", storage);
	}
	////finding empty row in hdd
	fread(&cbuf, sizeof(char), 1, hd);
	while (cbuf){
		fread(filenamebuf, sizeof(char), 16, hd);
		fread(&intbuf1, sizeof(int), 1, hd);
		fread(&intbuf2, sizeof(int), 1, hd);
		fread(&cbuf, sizeof(char), 1, hd);
	}
	///filling header
	fseek(hd, -1, SEEK_CUR);
	cbuf = 1;
	fwrite(&cbuf, sizeof(char), 1, hd);
	fwrite(target, sizeof(char), 16, hd);
	unsigned int start = intbuf1 + intbuf2;
	fwrite(&start, sizeof(int), 1, hd);
	unsigned int filelenpos = (unsigned int)ftell(hd);
	unsigned int lengthoffile = writefiletohdd(src, hd, start);
	fseek(hd, filelenpos, SEEK_SET);
	fwrite(&lengthoffile, sizeof(int), 1, hd);
	fclose(hd);
	fclose(src);
}//"%s \"%*[^\"]\" %s"
void givefilelocation(char *source,unsigned int *start,unsigned int *length){
	FILE *hd;
	if (!(hd = fopen("harddisk.hdd", "rb")))  { printf("error in opening file"); return; }
	unsigned int storage, fre;
	char filenamebuf[16];
	unsigned int intbuf1 = 0, intbuf2 = 0;
	char cbuf;
	fread(&storage, sizeof(int), 1, hd);
	fread(&fre, sizeof(int), 1, hd);
	fread(&cbuf, sizeof(char), 1, hd);
	while (cbuf){
		fread(filenamebuf, sizeof(char), 16, hd);
		fread(&intbuf1, sizeof(int), 1, hd);
		fread(&intbuf2, sizeof(int), 1, hd);
		if (!strcmp(filenamebuf, source)){
			*start = intbuf1;
			*length = intbuf2;
			fclose(hd);
			return;
		}
		fread(&cbuf, sizeof(char), 1, hd);
	}
}
void writehddtofile(FILE *hd, FILE *tgt, unsigned int start,unsigned int length){
	unsigned int lengthwritten=0;
	char buf;
	fseek(hd, start, SEEK_SET);

	/*char *buf = (char *)malloc(sizeof(char)*length);
	fread(buf, sizeof(char), length, hd);
	fwrite(buf, sizeof(char), length, tgt);*/

	fread(&buf, sizeof(char), 1, hd);
	while (lengthwritten<length){
		lengthwritten +=sizeof(char);
		fwrite(&buf, sizeof(char), 1, tgt);
		if (lengthwritten == length) break;
		fread(&buf, sizeof(char), 1, hd);
	}
}
void copyfromdisk(char *source, char *target){
	unsigned int start, length,readlength;
	char buf[100];
	givefilelocation(source, &start, &length);
	FILE *hd ,*tgt;
	if (!(hd = fopen("harddisk.hdd", "rb")))  { printf("error in opening file"); return; }
	if (!(tgt = fopen(target, "wb")))  { printf("error in opening file"); return; }
	writehddtofile(hd, tgt, start, length);
	fclose(hd);
	fclose(tgt);
}
int main(){
	char command[50];					//copytodisk "input.txt" inputhd.txt
	char buf[20];                       //copyfromdisk inputhd.txt "inputnew.txt"
	printf(">");						
	while (1){
		scanf("%[^\n]s", command);
		fflush(stdin);
		sscanf(command, "%s", buf);
		if (!strcmp(buf, "copytodisk")){
			char source[50],target[16];
			sscanf(command,"%*s \"%[^\"]", source);
			sscanf(command,"%*s \"%*[^\"]\" %s", target);
			copytodisk(source, target);
		}
		else if (!strcmp(buf, "copyfromdisk")){
			char source[16], target[50];
			sscanf(command, "%*s %s", source);
			sscanf(command, "%*s %*s \"%[^\"]", target);
			copyfromdisk(source, target);
		}
		else if (!strcmp(buf, "ls")){
			
		}
		else if (!strcmp(buf, "format")){

		}
		else if (!strcmp(buf, "delete")){

		}
		else {
			printf("invalid command\n");
		}
		printf(">");
	}
	return 0;
}