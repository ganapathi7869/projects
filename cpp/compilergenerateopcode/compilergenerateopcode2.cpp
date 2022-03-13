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
void buildsymboltable(char *command, char * arg1, struct symbol *symboltable, unsigned int symboltablelen){
	unsigned int address;
	if (symboltablelen == 0) address = 0;
	else  address = symboltable[symboltablelen - 1].address + symboltable[symboltablelen - 1].size;
	unsigned int size = 1;
	if (!strcmp(command, "DATA")){
		if (strchr(arg1, '[')){
			char *end = strchr(arg1, '[');
			sscanf(end, "[%d]", &size);
			*end = '\0';
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
		char name[20]; int val;
		sscanf(arg1, "%s = %d", name, &val);
		strcpy(symboltable[symboltablelen].name, name);
		symboltable[symboltablelen].address = address;
		symboltable[symboltablelen].size = size;
		symboltable[symboltablelen].constval = (char)val;
	}
}
void buildlabeltable(char *name, unsigned int address, struct block *labeltable, unsigned int labeltablelen){
	strcpy(labeltable[labeltablelen].name, name);
	labeltable[labeltablelen].address = address;
}
unsigned int getaddressofsymbol(char *name, struct symbol *symboltable, unsigned int symboltablelen){
	if (strchr(name, '[')){
		char *end = strchr(name, '[');
		unsigned int offset;
		sscanf(end, "[%d]", &offset);
		*end = '\0';
		for (unsigned int ind = 0; ind < symboltablelen; ind++){
			if (!strcmp(name, symboltable[ind].name)){
				return symboltable[ind].address + offset;
			}
		}
	}
	for (unsigned int ind = 0; ind < symboltablelen; ind++){
		if (!strcmp(name, symboltable[ind].name)){
			return symboltable[ind].address;
		}
	}
	printf("symbol not found\n");
	exit(1);
}
unsigned int getaddressofblock(char *name, struct block *labeltable, unsigned int labeltablelen){
	for (unsigned int ind = labeltablelen-1; ind >=0; ind--){
		if (!strcmp(name, labeltable[ind].name)){
			return labeltable[ind].address;
		}
	}
	printf("block not found\n");
	exit(1);
}

void processmov(char *arg1, char * arg2, struct targetline *targetlanguage, unsigned int targetlanguagelen, struct symbol *symboltable, unsigned int symboltablelen){
	int isarg1register = 0, ind;
	char registercodes[][3] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };
	for (ind = 0; ind < 8; ind++){
		if (!strcmp(arg1, registercodes[ind])){
			isarg1register = 1; break;
		}
	}
	if (isarg1register){
		targetlanguage[targetlanguagelen].opcode = 2;
		targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
		targetlanguage[targetlanguagelen].arg1 = ind;
		targetlanguage[targetlanguagelen].arg2 = getaddressofsymbol(arg2, symboltable, symboltablelen);
	}
	else{
		targetlanguage[targetlanguagelen].opcode = 1;
		targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
		targetlanguage[targetlanguagelen].arg1 = getaddressofsymbol(arg1, symboltable, symboltablelen);
		for (ind = 0; ind < 8; ind++){
			if (!strcmp(arg2, registercodes[ind])){
				break;
			}
		}
		targetlanguage[targetlanguagelen].arg2 = ind;
	}
}
void processarith(char opcode, char *arg1, char * arg2, char *arg3, struct targetline *targetlanguage, unsigned int targetlanguagelen){
	targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
	targetlanguage[targetlanguagelen].opcode = opcode;
	targetlanguage[targetlanguagelen].arg1 = arg1[0] - 'A';
	targetlanguage[targetlanguagelen].arg2 = arg2[0] - 'A';
	targetlanguage[targetlanguagelen].arg3 = arg3[0] - 'A';
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
void processif(char *arg1, char *arg2, char *arg3, struct targetline *targetlanguage, unsigned int targetlanguagelen, unsigned int *stack, unsigned int stacklen){
	targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
	targetlanguage[targetlanguagelen].opcode = 7;
	targetlanguage[targetlanguagelen].arg1 = arg1[0] - 'A';
	targetlanguage[targetlanguagelen].arg2 = arg3[0] - 'A';
	char operators[][5] = { "EQ", "LT", "GT", "LTEQ", "GTEQ" };
	for (unsigned int ind = 0; ind < 5; ind++){
		if (!strcmp(arg2, operators[ind])){
			targetlanguage[targetlanguagelen].arg3 = 8 + ind;
			break;
		}
	}
	targetlanguage[targetlanguagelen].arg4 = 0;
	stack[stacklen] = targetlanguagelen + 1;
}
void processelse(struct targetline *targetlanguage, unsigned int targetlanguagelen, struct block *labeltable, unsigned int labeltablelen, unsigned int *stack, unsigned int stacklen){
	processjump(NULL, targetlanguage, targetlanguagelen, labeltable, labeltablelen);
	stack[stacklen] = targetlanguagelen + 1;
}
void processendif(struct targetline *targetlanguage, unsigned int targetlanguagelen,  unsigned int *stack, unsigned int *stacklen){
	unsigned int storelineno = targetlanguagelen + 1;
	if (targetlanguage[stack[*stacklen - 1]-1].opcode == 6){
		targetlanguage[stack[*stacklen - 1]-1].arg1 = storelineno;
		storelineno = stack[*stacklen - 1] + 1;
		*stacklen -= 1;
	}
	targetlanguage[stack[*stacklen - 1]-1].arg4 = storelineno;
	*stacklen -= 1;
}
void processprint(char *arg2, struct targetline *targetlanguage, unsigned int targetlanguagelen, struct symbol *symboltable, unsigned int symboltablelen){
	targetlanguage[targetlanguagelen].opcode = 13;
	targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
	char registercodes[][3] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };
	for (int ind = 0; ind < 8; ind++){
		if (!strcmp(arg2, registercodes[ind])){
			targetlanguage[targetlanguagelen].arg1 = 0;
			targetlanguage[targetlanguagelen].arg2 = ind;
			return;
		}
	}
	targetlanguage[targetlanguagelen].arg1 = 1;
	targetlanguage[targetlanguagelen].arg2 = getaddressofsymbol(arg2, symboltable, symboltablelen);
}
void processread(char *arg1, struct targetline * targetlanguage, unsigned int targetlanguagelen){
	targetlanguage[targetlanguagelen].opcode = 14;
	targetlanguage[targetlanguagelen].lineno = targetlanguagelen + 1;
	targetlanguage[targetlanguagelen].arg1 = arg1[0] - 'A';
}
int main(){
	FILE *inputfile = fopen("maximumofthreenumbers.txt", "r");
	char line[50], command[20], arg1[20], arg2[20], arg3[20], catchnewline[2];
	unsigned int stack[100];
	struct symbol symboltable[100];
	struct block labeltable[100];
	struct targetline targetlanguage[100];
	unsigned int stacklen = 0, symboltablelen = 0, labeltablelen = 0, targetlanguagelen = 0;
	while (!feof(inputfile)){
		fscanf(inputfile, "%[^\n]s", line);
		fscanf(inputfile, "%[\n]s", catchnewline);
		sscanf(line, "%s", command);
		if (!strcmp(command, "DATA")){
			sscanf(line, "%*s %s", arg1);
			buildsymboltable(command, arg1, symboltable, symboltablelen);
			symboltablelen += 1;
		}
		else if (!strcmp(command, "CONST")){
			strcpy(arg1, &strchr(line, ' ')[1]);
			buildsymboltable(command, arg1, symboltable, symboltablelen);
			symboltablelen += 1;
		}
		else if (!strcmp(command, "MOV")){
			sscanf(line, "%*s %s %s", arg1, arg2);
			char *arg = strchr(arg1, ',');
			*arg = '\0';
			processmov(arg1, arg2, targetlanguage, targetlanguagelen, symboltable, symboltablelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(command, "START:")){
			continue;
		}
		else if (!strcmp(command, "ADD")){
			sscanf(line, "%*s %s %s %s", arg1, arg2, arg3);
			char *arg = strchr(arg1, ',');
			*arg = '\0';
			arg = strchr(arg2, ',');
			*arg = '\0';
			processarith(3, arg1, arg2, arg3, targetlanguage, targetlanguagelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(command, "end")){
			sscanf(line, "%*s %s %s %s", arg1, arg2, arg3);
			char *arg = strchr(arg1, ',');
			*arg = '\0';
			arg = strchr(arg2, ',');
			*arg = '\0';
			processarith(4, arg1, arg2, arg3, targetlanguage, targetlanguagelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(command, "MUL")){
			sscanf(line, "%*s %s %s %s", arg1, arg2, arg3);
			char *arg = strchr(arg1, ',');
			*arg = '\0';
			arg = strchr(arg2, ',');
			*arg = '\0';
			processarith(5, arg1, arg2, arg3, targetlanguage, targetlanguagelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(command, "JUMP")){
			sscanf(line, "%*s %s", arg1);
			processjump(arg1, targetlanguage, targetlanguagelen, labeltable, labeltablelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(command, "IF")){
			sscanf(line, "%*s %s %s %s", arg1, arg2, arg3);
			processif(arg1, arg2, arg3, targetlanguage, targetlanguagelen, stack, stacklen);
			stacklen += 1;
			targetlanguagelen += 1;
		}
		else if (!strcmp(command, "ELSE")){
			processelse(targetlanguage, targetlanguagelen, labeltable, labeltablelen, stack, stacklen);
			targetlanguagelen += 1;
			stacklen += 1;
		}
		else if (!strcmp(command, "ENDIF")){
			processendif(targetlanguage, targetlanguagelen,  stack, &stacklen); 
		}
		else if (!strcmp(command, "PRINT")){
			sscanf(line, "%*s %s", arg1);
			processprint(arg1, targetlanguage, targetlanguagelen, symboltable, symboltablelen);
			targetlanguagelen += 1;
		}
		else if (!strcmp(command, "READ")){
			sscanf(line, "%*s %s", arg1);
			processread(arg1, targetlanguage, targetlanguagelen);
			targetlanguagelen += 1;
		}
		else if (strchr(command, ':')){
			char *arg = strchr(command, ':');
			arg[0] = '\0';
			buildlabeltable(command, targetlanguagelen + 1, labeltable, labeltablelen);
			labeltablelen += 1;
		}
		else if (!strcmp(command, "END")){
			break;
		}
		else{
			printf("invalid command found\n");
		}
	}
	fclose(inputfile);
	FILE *outputfile = fopen("output.txt", "w");
	fprintf(outputfile, "line:opcode:arg1:arg2:arg3:arg4\n");
	for (unsigned int ind = 0; ind < targetlanguagelen; ind++){
		if (targetlanguage[ind].opcode == 1 || targetlanguage[ind].opcode == 2 || targetlanguage[ind].opcode == 13){
			fprintf(outputfile, "%4d:%6d:%4d:%4d\n", targetlanguage[ind].lineno, targetlanguage[ind].opcode, targetlanguage[ind].arg1, targetlanguage[ind].arg2);
		}
		else if (targetlanguage[ind].opcode >= 3 && targetlanguage[ind].opcode <= 5){
			fprintf(outputfile, "%4d:%6d:%4d:%4d:%4d\n", targetlanguage[ind].lineno, targetlanguage[ind].opcode, targetlanguage[ind].arg1, targetlanguage[ind].arg2, targetlanguage[ind].arg3);
		}
		else if (targetlanguage[ind].opcode == 6 || targetlanguage[ind].opcode == 14){
			fprintf(outputfile, "%4d:%6d:%4d\n", targetlanguage[ind].lineno, targetlanguage[ind].opcode, targetlanguage[ind].arg1);
		}
		else {
			fprintf(outputfile, "%4d:%6d:%4d:%4d:%4d:%4d\n", targetlanguage[ind].lineno, targetlanguage[ind].opcode, targetlanguage[ind].arg1, targetlanguage[ind].arg2, targetlanguage[ind].arg3, targetlanguage[ind].arg4);
		}
	}
	fclose(outputfile);
	return 0;
}