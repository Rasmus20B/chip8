#include <SDL2/SDL_events.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include "cpu.h"

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
uint8_t jump(void) {
	/* set the program counter to the last 3 nibbles of opcode */
	if(mode == 2) {
		fprintf(stdout,"JMP 0x%x\n", opcode & 0x0FFF);
		pc+=2;
		return 0;
	}
	pc = opcode & 0x0FFF;
	return 0;
}

uint8_t call(void) {
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

uint8_t ske(void) {
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

uint8_t skne(void) {
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

uint8_t skre(void) { 
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

uint8_t load(void) {
	if(mode == 2) {
		fprintf(stdout,"LOAD r%d,%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
	pc+=2;
	return 0;
}

uint8_t add(void) {
	if(mode == 2) {
		fprintf(stdout,"ADD r%d,%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
	pc+=2;
	return 0;
}

uint8_t move(void) {
	if(mode == 2) {
		fprintf(stdout,"MOVE r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
	pc+=2;
	return 0;
}

uint8_t or(void) {
	if(mode == 2) {
		fprintf(stdout,"OR r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
	pc+=2;
	return 0;
}

uint8_t and(void) {
	if(mode == 2) {
		fprintf(stdout,"AND r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
	pc+=2;
	return 0;
}

uint8_t xor(void) {
	if(mode == 2) {
		fprintf(stdout,"XOR r%d,r%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
	pc+=2;
	return 0;
}
uint8_t addr(void) {
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
uint8_t sub(void) {
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

uint8_t shr(void) {
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

uint8_t subr(void) {
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

uint8_t shl(void) { 
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

uint8_t skrne(void) {
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

uint8_t loadi(void) {
	if(mode == 2) {
		fprintf(stdout,"LOADI %d\n", opcode & 0x0FFF);
		pc+=2;
		return 0;
	}
	i = opcode & 0x0FFF;
	pc+=2;
	return 0;
}

uint8_t jumpi(void) {
	if(mode == 2) {
		fprintf(stdout,"JUMPI %d\n", opcode & 0x0FFF);
		pc+=2;
		return 0;
	}
	pc = v[0] + (opcode & 0x0FFF);
	return 0;
}

uint8_t rnd(void) {
	if(mode == 2) {
		fprintf(stdout, "RAND r%d,%d\n", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
	pc += 2;
	return 0;
}

uint8_t drw(void) {
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

uint8_t skpr(void) {
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

uint8_t skup(void) {
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

uint8_t moved(void) {
	if(mode == 2) {
		fprintf(stdout, "MOVED r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	v[(opcode & 0x0F00) >> 8] = delay_timer;
	pc += 2;
	return 0;
}

uint8_t keyd(void) {
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

uint8_t loadd(void) {
	if(mode == 2) {
		fprintf(stdout, "LOADD r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	delay_timer = v[(opcode & 0x0F00) >> 8];
	pc += 2;
	return 0;
}

uint8_t loads(void) {
	if(mode == 2) {
		fprintf(stdout, "LOADS r%d\n",(opcode & 0x0F00) >> 8);
		pc+=2;
		return 0;
	}
	sound_timer = v[(opcode & 0x0F00) >> 8];
	pc += 2;
	return 0;
}

uint8_t addi(void) {
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

uint8_t ldspr(void) {
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

uint8_t bcd(void) {
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

uint8_t stor(void) {
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

uint8_t readr(void) { 
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
uint8_t opcodes_0(void) { 
	uint8_t (*opcodes[3])(void) = { NULL, &cls, &ret };
	(*opcodes[(((opcode & 0x00FF) + 113) >> 3) - 41])();
	return 0; 
}
uint8_t opcodes_8(void) { 
	uint8_t (*opcodes[15])(void) = { &move, &or, &and, &xor, &addr, 	
		&sub, &shr, &subr, NULL, NULL, NULL, NULL, NULL, NULL, 
		&shl };
	(*opcodes[(opcode & 0x000F)])();
	return 0; 
}
uint8_t opcodes_e(void) { 

	uint8_t (*opcodes[5])(void) = { &skpr, NULL, NULL, &skup }; 
	/* 0x00a1 - 158 = 3.
	 * 0x009e - 158 = 0.
	 */
	(*opcodes[(opcode & 0x00FF) - 158])();
	return 0; 
}
uint8_t opcodes_f(void) { 
	uint8_t (*opcodes[15])(void) = { NULL, &moved, &keyd, &loadd, &addi, 
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

void initialize(void) {

	pc = 0x200;	// 512, starting address for programs
	i = 0;
	sp = 0;
	delay_timer = 0;
	sound_timer = 0;

	/* clear stack, keypad, and registers */
	for(int i = 0; i < 16; ++i) {
		stack[i] = 0;
		key[i] = 0;
		v[i] = 0;
	}

	/* clear the display */
	for(int i = 0; i < (64 * 32); ++i) {
		display[i] = 0;
	}
	/* clear memory */
	for(int i = 0; i <= MEM_LEN - 1; i++) {
		ram[i] = 0;
	}
	
	/* write the fontset to memory */
	for(int i = 0; i < 80; i++) {
		ram[i] = fontset[i];
	}
	srand(time(0));
}

void load_file(char game[30]) {
	
	FILE *fp;
	char *buffer;
	uint16_t file_size;
	
	/* open file in read-binary mode */
	fprintf(stderr, "[=] Attempting to open file...\n");
	if((fp = fopen(game, "rb")) == NULL) {
		perror("[!] Could not read file ");
		exit(3);
	}
	
	/* get the size of the file */
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	rewind(fp);
	
	/* allocate enough memory to store instructions in file */
	if((buffer = (char *)malloc(sizeof(char) * file_size)) == NULL) {
		perror("[!] Could not allocate memory ");
		exit(3);
	}
	
	fprintf(stderr, "[=] Attempting to read file\n");
	/* read file into buffer */
	size_t result = fread(buffer, sizeof(char), file_size, fp);
	if(result != file_size) {
		perror("[!] Could not read from file ");
		exit(3);
	}
	
	/* store buffer in memory */
	/* If the total memory minus the 512 bytes used by system is greater than the size of the file then read */
	if((4096-512) > file_size) { 		
		for(int i = 0; i < file_size; i++) {
			ram[i+512] = (uint16_t)buffer[i];
		}
	}
	else {
		fprintf(stderr, "[!] File too large\n");
		exit(3);
	}
	fprintf(stderr, "[+] Successfully read file to buffer.\n");
	
	fclose(fp);
	free(buffer);
}

uint8_t fetch_execute(uint8_t mode) { 
	
	opcode = ram[pc] << 8 | ram[pc+1];
	if(!opcode) {
		fprintf(stderr, "EOF\n");
		exit(0);
	}


	uint8_t (*opcodes_base[20])(void) = { &opcodes_0, &jump, &call, &ske, 
		&skne, &skre, &load, &add, &opcodes_8, &skrne, &loadi, &jumpi, 
		&rnd, &drw, &opcodes_e, &opcodes_f  };

	/* Print the address of program counter if disassembly mode */
	if(mode == 2) {
		printf("0x%x :- ",cur_addr);
		cur_addr += 2; 
	}
	/* Use the first nibble to determine destination function */
	(*opcodes_base[(opcode & 0xF000) >> 12])();

	/* Update timer once per cycle */
	if(delay_timer > 0)
		--delay_timer;
	if(sound_timer > 0) {
		--sound_timer;
	}
	else if(!sound_timer) {
		/* Play Sound */
	}
	return 0;
}
static inline void usage(char *arg1) {
	fprintf(stderr, "[!] Usage : %s [ -r ] [file]\n    Usage : %s [ -d ] [ input ] [ output ]\n", arg1, arg1);
	exit(1);
}

int main(int argc, char **argv) {
	
	SDL_Window *window;
	SDL_Renderer *renderer;
	
	uint32_t pixels[2048];
	int arg;
	char file[128];

	if(argc < 2 || argc > 3)
		usage(argv[0]);

	while ((arg = getopt(argc, argv, "a:d:r:")) != -1)
		switch(arg)
		{
			case 'd':
				fprintf(stderr, "[+] Mode set to 'Disassemble'.\n");
				strncpy(file, optarg, sizeof(file) - 2);
				mode = 2;
				break;
			case 'r':
				fprintf(stderr, "[+] Mode set to 'Run'.\n");
				strncpy(file, optarg, sizeof(file) - 2);
				mode = 1;
				break;
			case 'a':
				fprintf(stderr ,"[+] Mode set to 'Assembly'.\n");
				strncpy(file, optarg, sizeof(file) - 2);
				mode = 3;
				break;
			case '?':
				if (optopt == 'd' || optopt == 'r' || optopt == 'a') {
					/* If these don't have any arguments added on */
					exit(2);
				}
				else if (isprint(optopt)) {
					/* For unknown Options */
					exit(3);
				}
				
				
		}
	fprintf(stderr, "[=] Initializing virtual machine registers and memory...\n");
	/* Set the initial state (registers, memory, timers, etc) */
	initialize(); 
	fprintf(stderr, "[+] Initialized virtual machine.\n");
	fprintf(stderr, "[=] Loading file into memory...\n");
	load_file(file);	// read file specified into memory
	fprintf(stderr, "[+] Successfully loaded file into memory.\n");

	
	/* Run mode */
	if(mode == 1) {
		fprintf(stderr, "[=] Initializing SDL2 library...\n");
		SDL_Init(SDL_INIT_EVERYTHING);
		fprintf(stderr, "[+] Initialized SDL2 library.\n");
		fprintf(stderr, "[=] Starting SDL2 library...\n");
		window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
		if(window == NULL) {
			fprintf(stderr, "[-] Could not create SDL2 window: %s\n", SDL_GetError());
			exit(4);
		}
		/* renderer to be used as a hook to communicate with SDL2 graphics */
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
		/* Set resolution to 640 x 480 */
		SDL_RenderSetLogicalSize(renderer, 640, 480);
		SDL_Texture* sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
		fprintf(stderr, "[+] Successfully started SDL2.\n");
		fprintf(stderr, "[=] Starting Fetch-Execute Cycle...\n");
		
		/* Main loop.
		 *
		 * After instruction is completed via fetch_execute(),
		 * SDL will poll for keystrokes
		 */

	while(1) {
		fetch_execute(mode);
		/* e is the SDL2 event */
		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) exit(0);

			/* if a event is a keypress */
			if(e.type == SDL_KEYDOWN) {
			/* escape character exits program */
				if(e.key.keysym.sym == SDLK_ESCAPE)
					exit(0);

				/* F1 keypress will reset emulator state */
				if(e.key.keysym.sym == SDLK_F1) {
					load_file(argv[1]);
					initialize();
					continue;
				}
				
				/* check if key presses is on hexpad, if yes then set that key to high */
				for(int x = 0; x <= 15; ++x) {
					if(e.key.keysym.sym == keys[x]) {
						key[x] = 1;
					}
				}
			}
			/* if a key is no longer being pressed, set it to low */
			if(e.type == SDL_KEYUP) {
				for(int x = 0; x <= 15; ++x) {
					if(e.key.keysym.sym == keys[x]) {
						key[x] = 0;
					}
				}
			}
		}
		/* if draw is set to high then render new pixel layout */
		if(draw) {
      /* set draw to false for next cycle */
			draw = false;
			/* calculate new pixel layout */
      #ifdef __arm64__
      const uint32x4_t m1 = vdupq_n_u32(0x00FFFFFF);
      const uint32x4_t m2 = vdupq_n_u32(0xFF000000);
			for(int x = 0; x < 2048; x+=4) {
        uint32x4_t ps = { display[x], display[x+1], display[x+2],
                         display[x+3]};
        uint32x4_t res = vorrq_u32(vmulq_u32(ps, m1), m2);
        memcpy(pixels+x, &res, sizeof(uint32_t)*4);
			}
      // #elif __x86_64__
      // for(size_t x = 0; x < 2048; x+=4) {
      // __m256i m1 = { 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF};
      // __m256i m2 = { 0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000 };
      // __m256i ps  = { display[x], display[x+1], display[x+2], display[x+3] };
      // __m256i res = _mm256_mul_epi32(ps, m1);
      // res = _mm256_mul_epi32(res, m2);
      //
      // memcpy(pixels+x, &res, sizeof(uint32_t)*4);
      // pixels[x] = (0x00FFFFFF  * pixel) | 0xFF000000;
      // }
      #else
      for(size_t x = 0; x < 2048; x++) {
        uint8_t pixel = display[x];
        pixels[x] = (0x00FFFFFF  * pixel) | 0xFF000000;
      }
      #endif
			/* update the new pixel layout to the texture */
			SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(uint32_t));
			/* clear old layout */
			SDL_RenderClear(renderer);
			/* Copy new layout to renderer then use */
			SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
			SDL_RenderPresent(renderer);
			}
		}
	}
	/* disassembly mode */
	else if(mode == 2) {
		fprintf(stderr, "[=] Starting fetch-execute cycle...\n");
		while((ram[pc] << 8 | ram[pc+1]) != 0x0000) {
		fetch_execute(mode);
		}
	}
	return 0;
}


