#include <SDL2/SDL_events.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include "cpu.h"

void unknown_op(uint16_t opcode) { printf("Unknown opcode: 0x%X\n", opcode); exit(2); }

void initialize() {

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
		while(SDL_WaitEvent(&e)) {
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
			for(int x = 0; x < 2048; ++x) {
				uint8_t pixel = display[x];
				pixels[x] = (0x00FFFFFF  * pixel) | 0xFF000000;
			}
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


