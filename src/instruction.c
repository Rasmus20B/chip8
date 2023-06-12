#include "instructions.h"

uint8_t cls(void) { 
	if(mode == 2) { 
		fprintf(stdout,"CLS\n");
		pc+=2;
		return 0;
	} 
	for(int x = 0; x <= (DIS_HEIGHT * DIS_WIDTH) - 1; x++) {
		display[x] = 0;
		}
	pc+=2;
	return 0;
}

uint8_t ret(void) {
	/* point stack pointer to the previous value, set program counter to said previous value */
	if(mode == 2) {
		fprintf(stdout,"RET\n");
		pc+=2;
		return 0;
	}
	--sp;
	pc = stack[sp];
	pc+=2;
	return 0;
}
uint8_t old_jump(void) {
	/* set the program counter to the last 3 nibbles of opcode */
	if(mode == 2) {
		fprintf(stdout,"SYS 0x%X\n", opcode & 0x0FFF);
		pc+=2;
		return 0;
	} 
	pc = opcode & 0x0FFF;
	return 0;
}
uint8_t jump() {
	/* set the program counter to the last 3 nibbles of opcode */
	if(mode == 2) {
		fprintf(stdout,"JMP 0x%x\n", opcode & 0x0FFF);
		pc+=2;
		return 0;
	}
	pc = opcode & 0x0FFF;
	return 0;
}

uint8_t call() {
	/* set value of address of stack pointer to program counter 
	 * (used in ret() to return to calling function), 
	 * increment stack pointer (so the calling function address isn't overwritten). */

	if(mode == 2) {
		fprintf(stdout,"CALL 0x%x\n", opcode & 0x0FFF);
		pc+=2;
		return 0;
	}
	stack[sp] = pc;
	++sp;
	pc = opcode & 0x0FFF;
	return 0;
}

uint8_t ske() {
	if(mode == 2) {
		fprintf(stdout,"SKE r%d,%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
		pc+=2;
		return 0;
	}
	if(v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
		pc+=4;
	else
		pc+=2;
	return 0;
}

uint8_t skne() {
		if(mode == 2) {
		fprintf(stdout,"SKNE r%d,%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
		pc+=2;
		return 0;
	}
	 if(v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
		 pc+=4;
	 else
		 pc+=2;
	 return 0;
}

uint8_t skre() { 
	if(mode == 2) {
		fprintf(stdout,"SKRE r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	if(v[(opcode & 0x0F00) >> 8] == (opcode & 0x00F0))
		pc+=4;
	else
		pc+=2;
	return 0;
}

uint8_t load() {
	if(mode == 2) {
		fprintf(stdout,"LOAD r%d,%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
	pc+=2;
	return 0;
}

uint8_t add() {
	if(mode == 2) {
		fprintf(stdout,"ADD r%d,%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
	pc+=2;
	return 0;
}

uint8_t move() {
	if(mode == 2) {
		fprintf(stdout,"MOVE r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
	pc+=2;
	return 0;
}

uint8_t or() {
	if(mode == 2) {
		fprintf(stdout,"OR r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
	pc+=2;
	return 0;
}

uint8_t and() {
	if(mode == 2) {
		fprintf(stdout,"AND r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
	pc+=2;
	return 0;
}

uint8_t xor() {
	if(mode == 2) {
		fprintf(stdout,"XOR r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
	pc+=2;
	return 0;
}
uint8_t addr() {
	if(mode == 2) {
		fprintf(stdout,"ADDR r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] += v[(opcode & 0x00F0) >> 4];
	if(v[(opcode & 0x00F0) >> 4] > (0xFF - v[(opcode & 0x0F00) >> 8]))
		v[0xF] = 1;
	else
		v[0xF] = 0;
	pc+=2;
	return 0;
}
uint8_t sub() {
	if(mode == 2) {
		fprintf(stdout,"SUB r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	/* if number is subtracting number is bigger, set the carry bit */
	if(v[(opcode & 0x00F0) >> 4] > v[(opcode & 0x0F00) >> 8]) {	
		v[0xF] = 0;
	}
	else {
		v[0xF] = 1;
	}
	v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
	pc += 2;
	return 0;
}

uint8_t shr() {
	if(mode == 2) {
		fprintf(stdout,"SHR r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	/* Find the least significant bit (right-most bit) of v[x] and
	 * store in v[0xF].
	 * 1 OR 0 ; right-most bit represents 0x1, therefore simply AND
	 * half the register given by opcode */

	v[0xF] = v[(opcode & 0x0F00) >> 8] & 0x1;
	v[(opcode & 0x0F00) >> 8] >>= 1;
	pc+=2;
	return 0;
}

uint8_t subr() {
	if(mode == 2) {
		fprintf(stdout,"SUBR r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}

	/* Check for carry */
	if(v[(opcode & 0x00F0) >> 4] < v[(opcode & 0x0F00) >> 8]) {
		v[0xF] = 0;
	}
	else {
		v[0xF] = 1;
	}
	/* Perform subtraction */
	v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];
	pc+=2;
	return 0;
}

uint8_t shl() { 
	if(mode == 2) {
		fprintf(stdout,"SHL r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	/* Most significant (left-most) bit
	 * 1 OR 0 ; moving the highest bit to lowest position 
	 * will get rid of all other bits from result. 
	 * Shift left-most bit to right-most bit's position. */

	v[0xF] = v[(opcode & 0x0F00) >> 8] >> 7;
	/* double value in register */
	v[(opcode & 0x0F00) >> 8] <<= 1;
	pc+=2;
	return 0;
}

uint8_t skrne() {
	if(mode == 2) {
		fprintf(stdout,"SKRNE r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	if(v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4]) {
		pc+=4;
	}
	else {
		pc+=2;
	}
	return 0;
}

uint8_t loadi() {
	if(mode == 2) {
		fprintf(stdout,"LOADI %d\n", opcode & 0x0FFF);
		pc+=2;
		return 0;
	}
	i = opcode & 0x0FFF;
	pc+=2;
	return 0;
}

uint8_t jumpi() {
	if(mode == 2) {
		fprintf(stdout,"JUMPI %d\n", opcode & 0x0FFF);
		pc+=2;
		return 0;
	}
	pc = v[0] + (opcode & 0x0FFF);
	return 0;
}

uint8_t rnd() {
	if(mode == 2) {
		fprintf(stdout, "RAND r%d,%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
	pc += 2;
	return 0;
}

uint8_t drw() {

	if(mode == 2) {
		fprintf(stdout, "DRAW r%d,r%d,%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4, (opcode & 0x000F));
		pc+=2;
		return 0;
	}
	/* Draws sprint at VX, VY with width 8px and height N pixels. */

							// D
	unsigned short x = v[(opcode & 0x0F00) >> 8]; 	// X
	unsigned short y = v[(opcode & 0x00F0) >> 4]; 	// Y 
	unsigned short h = opcode & 0x000F; 		// N
	unsigned short pixel;

	v[0xF] = 0; // reset v[0xF]
	for(int yline = 0; yline < h; yline++) {
		pixel = ram[i + yline];
		for(int xline = 0; xline < 8; xline++) {
			if((pixel & (0x80 >> xline)) != 0) {
			/* Check to see if any pixels are flipped */
				if(display[(x + xline + ((y + yline) * 64))] == 1) {
					v[0xF] = 1;
				}
				display[x + xline + ((y + yline) * 64)] ^= 1;
			}
		}
	}
	// set draw flag to true so main loop knows to check display
	draw = true;
	pc +=2;
	return 0;
}

uint8_t skpr() {
	if(mode == 2) {
		fprintf(stdout, "SKPR r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	if(key[v[(opcode & 0x0F00) >> 8]] != 0)
		pc += 4;
	else
		pc += 2;
	return 0;
}

uint8_t skup() {
	if(mode == 2) {
		fprintf(stdout, "SKUP r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	if(key[v[(opcode & 0x0F00) >> 8]] == 0)
		pc += 4;
	else
		pc += 2;
	return 0;
}

uint8_t moved() {
	if(mode == 2) {
		fprintf(stdout, "MOVED r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] = delay_timer;
	pc += 2;
	return 0;
}

uint8_t keyd() {
	if(mode == 2) {
		fprintf(stdout, "KEYD r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}

	/* loop through each key; if a specific key is pressed, 
	 * set v[X] to the key number, and update key_pressed variable. */
	bool key_pressed = false; 
	for(int x = 0; x < 16; ++x) {
		if(key[x] != 0) {
			v[(opcode & 0x0F00) >> 8] = x;
			key_pressed = true;
		}
	}
	if(!key_pressed) {
		return 0;
	}
	pc += 2;
	return 0;
}

uint8_t loadd() {
	if(mode == 2) {
		fprintf(stdout, "LOADD r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	delay_timer = v[(opcode & 0x0F00) >> 8];
	pc += 2;
	return 0;
}

uint8_t loads() {
	if(mode == 2) {
		fprintf(stdout, "LOADS r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	sound_timer = v[(opcode & 0x0F00) >> 8];
	pc += 2;
	return 0;
}

uint8_t addi() {
	if(mode == 2) {
		fprintf(stdout, "ADDI r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	if(i + v[(opcode & 0x0F00) >> 8] > 0xFFF)
		v[0xF] = 1;
	else
		v[0xF] = 0;
	i += v[(opcode & 0x0F00) >> 8];
	pc += 2;
	return 0;
}

uint8_t ldspr() {
	if(mode == 2) {
		fprintf(stdout, "LDSPR r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	/* set i to the starting address of sprite of number in vx */
	i = v[(opcode & 0x0F00) >> 8] * 0x5;
	pc += 2;
	return 0;
}

uint8_t bcd() {
	if(mode == 2) {
		fprintf(stdout, "BCD r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}

	/* store each digit of decimal number in memory location:
	 *
	 * 128 : ram[i]   = 1
	 * 	 ram[i+1] = 2
	 * 	 ram[i+2] = 8 */

	ram[i] = v[(opcode & 0x0F00) >> 8] / 100;
	ram[i+1] = (v[(opcode & 0x0F00) >> 8] / 10) % 10;
	ram[i+2] = v[(opcode & 0x0F00) >> 8] % 10;
	pc += 2;
	return 0;
}

uint8_t stor() {
	if(mode == 2) {
		fprintf(stdout, "STOR r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	for(int x = 0; x <= ((opcode & 0x0F00) >> 8); ++x) {
		ram[i+x] = v[x];
	}
	i += ((opcode & 0x0F00) >> 8) + 1;
	pc += 2;
	return 0;
}

uint8_t readr() { 
	if(mode == 2) {
		fprintf(stdout, "READ r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	for(int x = 0; x <= ((opcode & 0x0F00) >> 8); ++x) {
		v[x] = ram[i+x];
	}
	i += ((opcode & 0x0F00) >> 8) + 1;
	pc += 2;
	return 0;
}

/* Calculations used to minimize memory used by arrays */
uint8_t opcodes_0() { 
	uint8_t (*opcodes[3])() = { NULL, &cls, &ret };
	(*opcodes[(((opcode & 0x00FF) + 113) >> 3) - 41])();
	return 0; 
}
uint8_t opcodes_8() { 
	uint8_t (*opcodes[15])() = { &move, &or, &and, &xor, &addr, 	
		&sub, &shr, &subr, NULL, NULL, NULL, NULL, NULL, NULL, 
		&shl };
	(*opcodes[(opcode & 0x000F)])();
	return 0; 
}
uint8_t opcodes_e() { 

	uint8_t (*opcodes[5])() = { &skpr, NULL, NULL, &skup }; 
	/* 0x00a1 - 158 = 3.
	 * 0x009e - 158 = 0.
	 */
	(*opcodes[(opcode & 0x00FF) - 158])();
	return 0; 
}
uint8_t opcodes_f() { 
	uint8_t (*opcodes[15])() = { NULL, &moved, &keyd, &loadd, &addi, 
		NULL, &ldspr, &bcd, NULL, &loads, NULL, &stor, NULL, &readr};
	/* ceil(0x00FF / 7) : Closest together array elements */
	if((opcode & 0x00FF) == 0x18) {
		(*opcodes[9])();
		return 0;
	}
	(*opcodes[((opcode & 0x00FF) + 7) >> 3])();
	return 0; 
}
void unknown_op(uint16_t opcode) { printf("Unknown opcode: 0x%X\n", opcode); exit(2); }
