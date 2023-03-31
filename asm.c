#include <string.h>
#include <stdlib.h>

#include "asm.h"  // used for asm global data and static inline functions

int main(int argc, char **argv) {


	FILE *fp; // Used as file descriptor for input file
	char *buffer; // buffer used to read in contents of input file
	char *copy; // Used as a backup because original buffer will be effected by first loop
	char *line; // current line contents of file
	char *opcode; // used to determine which instruction to run
	char *asm_arg1, *asm_arg2, *asm_arg3; // arguments used for opcodes
	char **saveptr = &buffer; // used to keep track of file while also tokening specific lines
	char **saveptrbak = &copy;
	int line_count = 0;
	uint8_t reg1 = 0, reg2 = 0, val1 = 0;
	uint8_t *output; // output array
	uint8_t err_count = 0; // increment whenever an error is encountered
	uint16_t big_arg, x1, x2, y1, y2, z1;
	uint16_t cur_addr = 0;
	uint8_t new_label = 0; // used to determine whether the line being read is a label, in which case the current address won't be incremented. Used as true or false.
	uint8_t jmp = 1; //used to handle output with zeroes in addresses being jumped to. used as a true or false.

	if(argc != 2) {
		fprintf(stderr, "[-] Usage : %s [ input_file ]\n", argv[0]);
		exit(1);
	}
	
	fprintf(stderr, "[=] Opening file...\n");
	fp = fopen(argv[1], "rb");
	if(!fp) {
		fprintf(stderr, "[-] Count not open file %s.\n", argv[1]);
		perror(" ");
		exit(1);
	}
	
	/* calculate offset of end of file from start */
	fseek(fp, 0, SEEK_END);
	int file_size = ftell(fp);
	/* go back to the start of file */
	rewind(fp);

	/* Allocate memory for input buffer */
	buffer = malloc(file_size);
	if(!buffer) {
		fprintf(stderr, "[-] Could not allocate memory to buffer.");
		perror(" ");
		exit(2);
	}

	fprintf(stderr, "[=] Reading...\n");
	fread(buffer, sizeof(char), file_size, fp);
	/* Close the file */
	fclose(fp);

	fprintf(stderr, "[+] Done reading.\n");
	
	/* Allocate memory for output buffer (Integer) */
	output = malloc(sizeof buffer * 4);
	if(!output) {
		fprintf(stderr, "[-] Could not allocate memory to output.");
		perror(" ");
		exit(2);
	}
	memset(output, 0, sizeof buffer * 4);

	/* Allocate memory for buffer backup to be used in first loop */
	copy = malloc(file_size);
	if(!copy) {
		fprintf(stderr, "[-] Could not allocate memory to buffer copy.");
		perror(" ");
		exit(2);
	}

	copy = memcpy(copy, buffer, file_size);
	/* Begin tokenising buffer by each line */
	line = strtok_r(copy, "\n", saveptrbak);
	/* First loop to find labels */
	while(line != NULL) {
		opcode = strtok(line, " ");
		if(strchr(opcode, ':')) {
			labellookuptable[label_count].val = cur_addr + 0x200 + 2;
			labellookuptable[label_count].key = opcode;
			label_count++;
		}
		else if(strchr(opcode, ';')) {
		}
		else{
			cur_addr+=2;
		}
		line = strtok_r(NULL, "\n", saveptrbak);
	}
	/* Reinitialize variables for translation loop */
	cur_addr = 0; 
	line = strtok_r(buffer, "\n", saveptr);
	line_count++;
	while(line != NULL) {

		/* opcode is the first word before whitespace */
		opcode = strtok(line, " ");

		/* Lookup key using the opcode given. */
		switch(lookup(opcode, opcodelookuptable, OPCODE_COUNT)) {
			case 1:
				/* CLS */
				*output = 0x0f;
				output++;
				*output = 0xe0;
				output++;
				break;
			case 2:
				/* RET */
				*output = 0x0f;
				output++;
				*output = 0xee;
				output++;
				break;
			case 3:
				/* JMP */
				asm_arg1 = strtok(NULL, " ");
				if(!asm_arg1) {
					fprintf(stderr, "[-] Line %d - JMP : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				big_arg = lookup(asm_arg1, labellookuptable, label_count);
				if(!big_arg) {
					fprintf(stderr, "[-] Line %d - Label %s not found\n", line_count, asm_arg1);
					err_count++;
					break;
				}
				/* seperate digits of hexadecimal form of argument */
				x1 = big_arg % 16; // find remainder
				x2 = big_arg / 16; // find quotient
				y1 = x2 % 16;
				y2 = x2 / 16;
				z1 = y2 % 16;
				*output = 0x10 | z1;
				output++;
				*output = x1 | (y1 << 4);
				output++;
				break;
			case 4:
				/* CALL */
				asm_arg1 = strtok(NULL, " ");
				if(!asm_arg1) {
					fprintf(stderr, "[-] Line %d - CALL : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				big_arg = lookup(asm_arg1, labellookuptable, label_count);
				if(!big_arg) {
					fprintf(stderr, "[-] Line %d - Label %s not found\n", line_count, asm_arg1);
					err_count++;
					break;
				}
				/* seperate digits of hexadecimal form of argument */
				x1 = big_arg % 16; // find remainder
				x2 = big_arg / 16; // find quotient
				y1 = x2 % 16;
				y2 = x2 / 16;
				z1 = y2 % 16;
				*output = 0x20 | z1;
				output++;
				*output = x1 | (y1 << 4);
				output++;
				break;
			case 5:
				/* SKE */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] Line %d - SKE : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - SKE : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0 || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SKE : Register Number Invalid.\n", line_count);
					err_count++;
				}

				*output = 0x30 | reg1; // E.g. reg1 = 5. 0x30 | 0x5 = 0x35
				output++;

				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] Line %d - SKE : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				val1 = string_to_int(0, asm_arg2);
				*output = val1;
				output++;
				
				break;
			case 6:
				/* SKNE */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] Line %d - SKNE : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - SKNE : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SKNE : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x40 | reg1;
				output++;

				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] Line %d - SKNE : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				val1 = string_to_int(0, asm_arg2);
				*output = val1;
				output++;
				break;
			case 7:
				/* SKRE */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] Line %d - SKRE : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - SKRE : Expected Register. E.g. r5\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] Line %d - SKRE : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - SKRE : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SKRE : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x50 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SKRE : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				output++;
				break;
			case 8:
				/* LOAD */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] Line %d - LOAD : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - LOAD : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] Line %d - LOAD : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - LOAD : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x60 | reg1;
				output++;

				val1 = string_to_int(0, asm_arg2);
				*output = val1;
				output++;
				break;
			case 9:
				/* ADD */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] Line %d - ADD : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - ADD : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] Line %d - ADD : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - ADD : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x70 | reg1;
				output++;

				val1 = string_to_int(0, asm_arg2);
				*output = val1;
				output++;
				break;
			case 10:
				/* MOVE */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] Line %d - MOVE : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - MOVE : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] Line %d - MOVE : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - MOVE : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - MOVE : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x80 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - MOVE : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				output++;
				break;
			case 11:
				/* OR */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] Line %d - OR : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - OR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] Line %d - OR : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - OR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - OR : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x80 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - OR : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				*output = *output | 0x1;
				output++;
				break;
			case 12:
				/* AND */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] Line %d - AND : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - AND : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] Line %d - AND : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - AND : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - AND : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x80 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - AND : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				*output = *output | 0x2;
				output++;
				break;
			case 13:
				/* XOR */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - XOR : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - XOR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] Line %d - XOR : Expected Argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - XOR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - XOR : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x80 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - XOR : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				*output = *output | 0x3;
				output++;
				break;
			case 14:
				/* ADDR */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - ADDR : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - ADDR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] line %d - ADDR : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - ADDR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - ADDR : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x80 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - ADDR : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				*output = *output | 0x4;
				output++;
				break;
			case 15:
				/* SUB */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - SUB : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - SUB : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] line %d - SUB : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - SUB : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SUB : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x80 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SUB : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				*output = *output | 0x5;
				output++;
				break;
			case 16:
				/* SHR */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - SHR : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - SHR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] line %d - SHR : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - SHR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SHR : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x80 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg2 > 0xf) {
					fprintf(stderr, "[-] Line %d - SHR : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				*output = *output | 0x6;
				output++;
				break;
			case 17:
				/* SUBR */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - SUBR : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - SUBR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] line %d - SUBR : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - SUBR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SUBR : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x80 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SUBR : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				*output = *output | 0x7;
				output++;
				break;
			case 18:
				/* SHL */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - SHL : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - SHL : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] line %d - SHL : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - SHL : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SHL : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x80 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg2 > 0xf) {
					fprintf(stderr, "[-] Line %d - SHL : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				*output = *output | 0xe;
				output++;
				break;
			case 19:
				/* SKRNE */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - SKRNE : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - SKRNE : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] line %d - SKRNE : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - SKRNE : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SKRNE : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0x90 | reg1;
				output++;

				reg2 = string_to_int(1, asm_arg2);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SKRNE : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = reg2 << 4;
				output++;
				break;
			case 20:
				/* LOADI */
				asm_arg1 = strtok(NULL, " ");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - LOADI : expected argument\n", line_count);
					err_count++;
					break;
				}
				big_arg = string_to_int(0, asm_arg1); 
				if(big_arg > 4095) {
					fprintf(stderr, "[-] Line %d - LOADI : Invalid Integer.\n", line_count);
					err_count++;
					break;
				}
				x1 = big_arg % 16; // find remainder
				x2 = big_arg / 16; // find quotient
				y1 = x2 % 16;
				y2 = x2 / 16;
				z1 = y2 % 16;
				*output = 0xa0 | z1;
				output++;
				*output = x1 | (y1 << 4);
				output++;
				break;
			case 21:
				/* JUMPI */

				asm_arg1 = strtok(NULL, " ");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - JUMPI : expected argument\n", line_count);
					err_count++;
					break;
				}
				big_arg = string_to_int(0, asm_arg1); 
				if(big_arg > 4095) {
					fprintf(stderr, "[-] Line %d - JUMPI : Invalid Integer.\n", line_count);
					err_count++;
					break;
				}
				x1 = big_arg % 16; // find remainder
				x2 = big_arg / 16; // find quotient
				y1 = x2 % 16;
				y2 = x2 / 16;
				z1 = y2 % 16;
				*output = 0xb0 | z1;
				output++;
				*output = x1 | (y1 << 4);
				output++;
				break;
			case 22:
				/* RAND */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - RAND : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - RAND : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] line %d - RAND : expected argument\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - RAND : Register Number Invalid.\n", line_count);
					err_count++;
				}
				*output = 0xc0 | reg1;
				output++;

				val1 = string_to_int(0, asm_arg2);
				if(val1 > 254){
					fprintf(stderr, "[-] Line %d - RAND : Invalid Integer.\n", line_count);
					err_count++;
					break;
				}
				*output = val1;
				output++;
				break;
			case 23:
				/* DRAW */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - DRAW : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - DRAW : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg2 = strtok(NULL, ",");
				if(!asm_arg2) {
					fprintf(stderr, "[-] line %d - DRAW : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg2, 'r')) {
					fprintf(stderr, "[-] Line %d - DRAW : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				asm_arg3 = strtok(NULL, ",");
				if(!asm_arg3) {
					fprintf(stderr, "[-] line %d - DRAW : expected argument\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				reg2 = string_to_int(1, asm_arg2);
				val1 = string_to_int(0, asm_arg3);
				if(reg1 > 0xf || reg1 < 0x0 || reg2 > 0xf) {
					fprintf(stderr, "[-] Line %d - DRAW : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				if(val1 > 0x0F) {
					fprintf(stderr, "[-] Line %d - Invalid Integer.\n", line_count);
					err_count++;
					break;
				}

				*output = 0xd0 | reg1;
				output++;
				*output = val1 |  (reg2 << 4);
				output++;
				break;
			case 24:
				/* SKPR */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - SKPR : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - SKPR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SKPR : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xe0 | reg1;
				output++;
				*output = 0xe | 0x9 << 4;
				output++;
				break;
			case 25:
				/* SKUP */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - SKUP : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - SKUP : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - SKUP : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xe0 | reg1;
				output++;
				*output = 0x1 | 0xa << 4;
				output++;
				break;
			case 26:
				/* MOVED */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - MOVED : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - MOVED : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - MOVED : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xf0 | reg1;
				output++;
				*output = 0x7 | 0x0 << 4;
				output++;
				break;
			case 27:
				/* KEYD */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - KEYD : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - KEYD : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - KEYD : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xf0 | reg1;
				output++;
				*output = 0xa | 0x0 << 4;
				output++;
				break;
			case 28:
				/* LOADD */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - LOADD : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - LOADD : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - LOADD : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xf0 | reg1;
				output++;
				*output = 0x5 | 0x1 << 4;
				output++;
				break;
			case 29:
				/* LOADS */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - LOADS : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - LOADS : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - LOADS : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xf0 | reg1;
				output++;
				*output = 0x8 | 0x1 << 4;
				output++;
				break;
			case 30:
				/* ADDI */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - ADDI : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - ADDI : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - ADDI : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xf0 | reg1;
				output++;
				*output = 0xe | 0x1 << 4;
				output++;
				break;
			case 31:
				/* LDSPR */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - LDSPR : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - LDSPR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - LDSPR : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xf0 | reg1;
				output++;
				*output = 0x9 | 0x2 << 4;
				output++;
				break;
			case 32:
				/* BCD */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - BCD : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - BCD : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - BCD : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xf0 | reg1;
				output++;
				*output = 0x3 | 0x3 << 4;
				output++;
				break;
			case 33:
				/* STOR */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - STOR : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - STOR : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - STOR : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xf0 | reg1;
				output++;
				*output = 0x5 | 0x5 << 4;
				output++;
				break;
			case 34:
				/* READ */
				asm_arg1 = strtok(NULL, ",");
				if(!asm_arg1) {
					fprintf(stderr, "[-] line %d - READ : expected argument\n", line_count);
					err_count++;
					break;
				}
				if(!strchr(asm_arg1, 'r')) {
					fprintf(stderr, "[-] Line %d - READ : Expected Register. E.g. r4\n", line_count);
					err_count++;
					break;
				}
				reg1 = string_to_int(1, asm_arg1);
				if(reg1 > 0xf || reg1 < 0x0) {
					fprintf(stderr, "[-] Line %d - READ : Register Number Invalid.\n", line_count);
					err_count++;
					break;
				}
				*output = 0xf0 | reg1;
				output++;
				*output = 0x5 | 0x6 << 4;
				output++;
				break;
			default:
				opcode = strtok(line, " ");
				if(strchr(opcode, ':') || strchr(opcode, ';')) {
					new_label = 1; 
				}
				else { 
					fprintf(stderr, "[-] Line %d - Unknown Opcode %s\n", line_count, opcode);
					err_count++;
					break;
				}
				new_label = 1;
				break;
		}
		line = strtok_r(NULL, "\n", saveptr);
		line_count++;
		if(!new_label)
			cur_addr += 2;
		new_label = 0;
	}
	if(err_count) {
		fprintf(stderr, "[-] %d Errors found\n", err_count);
		exit(4);
	}

	/* rewind to the start of allocated memory */
	output -= cur_addr;
	while(*output || jmp == 1) {
		jmp = 0;
		if(*output == 0x0F)
			*output = 0x00;
		printf("%c", *output);
		if(*output >= 0x10 && *output <= 0x2f)
			jmp = 1;
		output++;
	}
	return 0;
}
