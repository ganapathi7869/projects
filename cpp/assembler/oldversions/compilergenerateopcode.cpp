#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct symbol{
	char name[20];
	unsigned int address;
	unsigned int size;
	char constval;
};
struct block{
	char name[20];
	unsigned int address;
};
struct targetline{
	unsigned int lineno;
	unsigned int opcode;
	unsigned int arg1;
	unsigned int arg2;
	unsigned int arg3;
	unsigned int arg4;
};
void buildsymboltable(char *command,char * arg1,struct symbol *symboltable,unsigned int symboltablelen){
	unsigned int address;
	if (symboltablelen == 0) address = 0;
	else  address = symboltable[symboltablelen - 1].address + symboltable[symboltablelen - 1].size;
	unsigned int size=1;
	if (!strcmp(command, "DATA")){
		if (strchr(arg1, '[')){
			char *sub = strchr(arg1, '[');
			sscanf(sub, "[%d]", &size);
			*sub = '\0';
			strcpy(symboltable[symboltablelen].name, arg1);
			symboltable[symboltablelen].address = address;
			symboltable[symboltablelen].size = size;
		}
		else{
			strcpy(symboltable[symboltablelen].name, arg1);
			symboltable[symboltablelen].address = address;
			symboltable[symboltablelen].size = size;
		}
	}
	else if (!strcmp(command, "CONST")){
		char name[20]; char val;
		sscanf(arg1, "%s = %d", name, &val);
		strcpy(symboltable[symboltablelen].name, name);
		symboltable[symboltablelen].address = address;
		symboltable[symboltablelen].size = size;
		symboltable[symboltablelen].constval = val;
	}
}
void buildlabeltable(char *name,unsigned int address,struct block *labeltable,unsigned int labeltablelen){
	strcpy(labeltable[labeltablelen].name, name);
	labeltable[labeltablelen].address = address;
}
unsigned int getaddressofsymbol(char *name ,struct symbol *symboltable, unsigned int symboltablelen){
	if (strchr(name, '[')){
		char *arg = strchr(name, '[');
		unsigned int offset;
		sscanf(arg, "[%d]", &offset);
		*arg = '\0';
		for (unsigned int i = 0; i < symboltablelen; i++){
			if (!strcmp(name, symboltable[i].name)){
				return symboltable[i].address + offset;
			}
		}
	}
	for (unsigned int i = 0; i < symboltablelen; i++){
		if (!strcmp(name, symboltable[i].name)){
			return symboltable[i].address;
		}
	}
	printf("symbol not found\n");
	exit(1);
}
unsigned int getaddressofblock(char *name, struct block *labeltable, unsigned int labeltablelen){
	for (unsigned int i = 0; i < labeltablelen; i++){
		if (!strcmp(name, labeltable[i].name)){
			return labeltable[i].address;
		}
	}
	printf("block not found\n");
	exit(1);
}
//void deleteelselabel(struct block *labeltable, unsigned int *labeltablelen){  ///////////////////////
//	for (unsigned int i = *labeltablelen - 1; i >-1; i--){ 
//		if (!strcmp("ELSE", labeltable[i].name)){
//			strcpy(labeltable[i].name, labeltable[*labeltablelen - 1].name);
//			labeltable[i].address = labeltable[*labeltablelen - 1].address;
//			*labeltablelen = *labeltablelen - 1;
//			break;
//		}
//	}
//}
void processmov(char *arg1, char * arg2, struct targetline *targetlanguage, unsigned int targetlanguagelen, struct symbol *symboltable, unsigned int symboltablelen){
	int isregister = 0,i;
	char registercodes[][3] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };
	for ( i = 0; i < 8; i++){
		if (!strcmp(arg1, registercodes[i])){
			isregister = 1; break;
		}
	}
	if (isregister){
		targetlanguage[targetlanguagelen].opcode = 2;
		targetlanguage[targetlanguagelen ].lineno = targetlanguagelen + 1;
		targetlanguage[targetlanguagelen ].arg1 = i;
		targetlanguage[targetlanguagelen ].arg2 = getaddressofsymbol(arg2, symboltable, symboltablelen);
	}
	else{
		targetlanguage[targetlanguagelen ].opcode = 1;
		targetlanguage[targetlanguagelen ].lineno = targetlanguagelen + 1;
		targetlanguage[targetlanguagelen ].arg1 = getaddressofsymbol(arg1, symboltable, symboltablelen);
		for (i = 0; i < 8; i++){
			if (!strcmp(arg2, registercodes[i])){
				 break;
			}
		}
		targetlanguage[targetlanguagelen ].arg2 = i;
	}
}
void processarith(char opcode, char *arg1, char * arg2, char *arg3, struct targetline *targetlanguage, unsigned int targetlanguagelen){
	targetlanguage[targetlanguagelen ].lineno = targetlanguagelen + 1;
	targetlanguage[targetlanguagelen ].opcode = opcode;
	targetlanguage[targetlanguagelen ].arg1 = arg1[0] - 'A';
	targetlanguage[targetlanguagelen ].arg2 = arg2[0] - 'A';
	targetlanguage[targetlanguagelen ].arg3 = arg3[0] - 'A';
}
void processjump(char *arg1, struct targetline *targetlanguage, unsigned int targetlanguagelen, struct block *labeltable, unsigned int labeltablelength){
	if (!arg1){
		targetlanguage[targetlanguagelen].opcode = 6;
		targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
		targetlanguage[targetlanguagelen].arg1 = 0;
		return;
	}
	targetlanguage[targetlanguagelen].opcode = 6;
	targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
	targetlanguage[targetlanguagelen].arg1 = getaddressofblock(arg1, labeltable, labeltablelength);
}
void processif(char *arg1, char *arg2, char *arg3, struct targetline *targetlanguage, unsigned int targetlanguagelen,unsigned int *stack, unsigned int stacklen){
	targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
	targetlanguage[targetlanguagelen].opcode = 7;
	targetlanguage[targetlanguagelen].arg1 = arg1[0] - 'A';
	targetlanguage[targetlanguagelen].arg2 = arg3[0] - 'A';
	char operators[][5] = { "EQ", "LT", "GT", "LTEQ", "GTEQ" };
	for (unsigned int i = 0; i < 5; i++){
		if (!strcmp(arg2, operators[i])){
			targetlanguage[targetlanguagelen].arg3 = 8 + i;
			break;
		}
	}
	targetlanguage[targetlanguagelen].arg4 = 0;
	stack[stacklen] = targetlanguagelen + 1;
}
void processelse(struct targetline *targetlanguage, unsigned int targetlanguagelen, struct block *labeltable, unsigned int labeltablelen, unsigned int *stack, unsigned int stacklen){
	processjump(NULL, targetlanguage, targetlanguagelen, labeltable, labeltablelen);
	stack[stacklen] = targetlanguagelen + 1;
	buildlabeltable("ELSE", targetlanguagelen + 2,labeltable,labeltablelen);
}
void processendif(struct targetline *targetlanguage, unsigned int targetlanguagelen, struct block *labeltable, unsigned int labeltablelen, unsigned int *stack, unsigned int *stacklen){
	buildlabeltable("ENDIF", targetlanguagelen + 1, labeltable, labeltablelen);
	unsigned int linehold = targetlanguagelen + 1;
	if (targetlanguage[stack[*stacklen - 1]].opcode == 6 ){
		targetlanguage[stack[*stacklen - 1]].arg2 = linehold ;
		linehold = stack[*stacklen - 1] + 1;
		*stacklen -= 1;
	}
	targetlanguage[stack[*stacklen - 1]].arg4 = linehold;
	*stacklen -= 1;
}
void processprint(char *arg1, struct targetline *targetlanguage, unsigned int targetlanguagelen, struct symbol *symboltable, unsigned int symboltablelen){
	targetlanguage[targetlanguagelen].opcode = 13;
	targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
	targetlanguage[targetlanguagelen].arg1 = getaddressofsymbol(arg1, symboltable, symboltablelen);
}
void processread(char *arg1, struct targetline *targetlanguage, unsigned int targetlanguagelen){
	targetlanguage[targetlanguagelen].opcode = 14;
	targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
	targetlanguage[targetlanguagelen].arg1 = arg1[0] - 'A';
}
int main(){
	FILE *inp = fopen("input.txt", "r");
	char command[50],buf[20],arg1[20],arg2[20],arg3[20],arg4[20];
	unsigned int stack[100];
	struct symbol symboltable[100];
	struct block labeltable[100];
	struct targetline targetlanguage[100];
	unsigned int stacklen = 0, symboltablelen = 0, labeltablelen = 0, targetlanguagelen = 0;
	//char registercodes[][3] = { "AX", "BX", "CX", "DX", "EX", "FX","GX","HX" };
	while (!feof(inp)){
		//fgets(command, sizeof(char) * 50, inp);
		fscanf(inp, "%[^\n]s%*c", command);
		//fflush(inp);
		sscanf(command, "%s", buf);
		if (!strcmp(buf, "DATA")){
			sscanf(command, "%*s %s", arg1);
			buildsymboltable(buf, arg1,symboltable,symboltablelen);
			symboltablelen += 1;
		}
		else if (!strcmp(buf, "CONST")){
			strcpy(arg1, &strchr(buf, ' ')[1]);
			buildsymboltable(buf, arg1, symboltable, symboltablelen);
			symboltablelen += 1;
		}
		else if (!strcmp(buf, "MOV")){
			sscanf(command, "%*s %s %s", arg1, arg2);
			char *arg = strchr(arg1, ',');
			*arg = '\0';
			processmov(arg1,arg2,targetlanguage,targetlanguagelen,symboltable,symboltablelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(buf, "START:")){
			continue;
		}
		else if (!strcmp(buf, "ADD")){
			sscanf(command, "%*s %s %s %s", arg1, arg2,arg3);
			char *arg = strchr(arg1, ',');
			*arg = '\0';
			arg = strchr(arg2, ',');
			*arg = '\0';
			processarith(3,arg1, arg2, arg3, targetlanguage, targetlanguagelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(buf, "SUB")){
			sscanf(command, "%*s %s %s %s", arg1, arg2, arg3);
			char *arg = strchr(arg1, ',');
			*arg = '\0';
			arg = strchr(arg2, ',');
			*arg = '\0';
			processarith(4, arg1, arg2, arg3, targetlanguage, targetlanguagelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(buf, "MUL")){
			sscanf(command, "%*s %s %s %s", arg1, arg2, arg3);
			char *arg = strchr(arg1, ',');
			*arg = '\0';
			arg = strchr(arg2, ',');
			*arg = '\0';
			processarith(5, arg1, arg2, arg3, targetlanguage, targetlanguagelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(buf, "JUMP")){
			sscanf(command, "%*s %s", arg1);
			processjump(arg1, targetlanguage, targetlanguagelen,labeltable,labeltablelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(buf, "IF")){
			sscanf(command, "%*s %s %s %s", arg1, arg2, arg3);
			processif(arg1, arg2, arg3, targetlanguage, targetlanguagelen, stack, stacklen);
			stacklen += 1;
			targetlanguagelen += 1;
		}
		else if (!strcmp(buf, "ELSE")){
			processelse(targetlanguage, targetlanguagelen, labeltable, labeltablelen, stack, stacklen);
			targetlanguagelen += 1;
			stacklen += 1;
			labeltablelen += 1;
		}
		else if (!strcmp(buf, "ENDIF")){
			processendif(targetlanguage, targetlanguagelen, labeltable, labeltablelen, stack, &stacklen);
			labeltablelen += 1;
		}
		else if (!strcmp(buf, "PRINT")){
			sscanf(command, "%*s %s", arg1);
			processprint(arg1,targetlanguage, targetlanguagelen, symboltable, symboltablelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(buf, "READ")){
			sscanf(command, "%*s %s", arg1);
			processread(arg1, targetlanguage, targetlanguagelen);
			targetlanguagelen += 1;
		}
		else if(strchr(buf,':')){
			char *arg = strchr(buf, ':');
			arg[0] = '\0';
			buildlabeltable(buf, targetlanguagelen + 2, labeltable, labeltablelen);
			labeltablelen += 1;
		}
		else{
			printf("invalid command found\n");
		}
	}
	fclose(inp);
	FILE *out = fopen("output.txt", "w");
	for (unsigned int i = 0; i < targetlanguagelen; i++){
		if (targetlanguage[i].opcode == 1 || targetlanguage[i].opcode == 2){
			fprintf(out, "%d %d %d\n", targetlanguage[i].opcode, targetlanguage[i].arg1, targetlanguage[i].arg2);
		}
		else if (targetlanguage[i].opcode >= 3 && targetlanguage[i].opcode <= 5){
			fprintf(out, "%d %d %d %d\n", targetlanguage[i].opcode, targetlanguage[i].arg1, targetlanguage[i].arg2,targetlanguage[i].arg3);
		}
		else if (targetlanguage[i].opcode == 6 || targetlanguage[i].opcode == 13 || targetlanguage[i].opcode == 14){
			fprintf(out, "%d %d\n", targetlanguage[i].opcode, targetlanguage[i].arg1);
		}
		else {
			fprintf(out, "%d %d %d %d %d\n", targetlanguage[i].opcode, targetlanguage[i].arg1, targetlanguage[i].arg2, targetlanguage[i].arg3, targetlanguage[i].arg4);
		}
	}
	fclose(out);
	return 0;
}