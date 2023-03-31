#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define OPCODE_COUNT 34

typedef struct { 
	char *key; 
	uint16_t val; 
}dictionary;

static const dictionary opcodelookuptable[] = {
		{ "CLS", 1 },
		{ "RET", 2 },
		{ "JMP", 3 },
		{ "CALL", 4 },
		{ "SKE", 5 },
		{ "SKNE", 6 },
		{ "SKRE", 7 },
		{ "LOAD", 8 },
		{ "ADD", 9 },
		{ "MOVE", 10 },
		{ "OR", 11 },
		{ "AND", 12 },
		{ "XOR", 13 },
		{ "ADDR", 14 },
		{ "SUB", 15 },
		{ "SHR", 16 },
		{ "SUBR", 17 },
		{ "SHL", 18 },
		{ "SKRNE", 19 },
		{ "LOADI", 20 },
		{ "JUMPI", 21 },
		{ "RAND", 22 },
		{ "DRAW", 23 },
		{ "SKPR", 24 },
		{ "SKUP", 25 },
		{ "MOVED", 26 },
		{ "KEYD", 27 },
		{ "LOADD", 28 },
		{ "LOADS", 29 },
		{ "ADDI", 30 },
		{ "LDSPR", 31 },
		{ "BCD", 32 },
		{ "STOR", 33 },
		{ "READ", 34 }
};

dictionary labellookuptable[30];
uint8_t label_count = 0;

static inline uint16_t lookup(char *string, const dictionary *lookuptable, uint8_t count) {
	for(int i = 0; i < count; i++) {
		char *test = lookuptable[i].key;
		if(strcmp(test, string) == 0) {
			return lookuptable[i].val;
		}
	}
	return 0;
}

static inline uint8_t string_to_int(int start, char *str) {
	uint8_t output = 0;
	for(uint8_t i = start; str[i] != 0; i++){ output = output * 10 + (str[i] - '0');}
	return output; 
}

static inline char* store_file(FILE *fp) {

	char *output;
	
	/* calculate offset of end of file from start */
	fseek(fp, 0, SEEK_END);
	int file_size = ftell(fp);
	/* go back to the start of file */
	rewind(fp);

	/* Allocate memory for input buffer */
	output = malloc(file_size);
	if(!output) {
		fprintf(stderr, "[-] Could not allocate memory to buffer.");
		perror(" ");
		exit(2);
	}

	fprintf(stderr, "[=] Reading...\n");
	fread(output, sizeof(char), file_size, fp);
	/* Close the file */
	fclose(fp);

	return output;
}

