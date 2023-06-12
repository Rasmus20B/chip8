#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#ifdef __x86_64__
  #include <immintrin.h>
  #include <emmintrin.h>
#elif __arm64__ || __arm__
  #include <arm_neon.h>
#endif
#define MEM_LEN 4096 // Amount of memory in the system
#define DIS_WIDTH 64 // Width of screen 
#define DIS_HEIGHT 32 // heigh of screen 

/* Keys to be recognized by SDL2, form a hexadecimal keypad.
 */
uint8_t keys[16] = {
	SDLK_x,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_q,
	SDLK_w,
	SDLK_e,
	SDLK_a,
	SDLK_s,
	SDLK_d,
	SDLK_z,
	SDLK_c,
	SDLK_4,
	SDLK_r,
	SDLK_f,
	SDLK_v,
};

/* sprites of hexadecimal numbers to be stored in the first 80 bytes of memory.
 * Each line represents a sprite.
 * Example:
 * A = 
 * 1111
 * 1..1
 * 1111
 * 1..1
 * 1..1
 *
 */
unsigned char fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, 	// 0
	0x20, 0x60, 0x20, 0x20, 0x70,	// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, 	// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,	// 3
	0x90, 0x90, 0xF0, 0x10, 0x10, 	// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,	// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, 	// 6
	0xF0, 0x10, 0x20, 0x40, 0x40, 	// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,	// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,	// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,	// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,	// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,	// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,	// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,	// E
	0xF0, 0x80, 0xF0, 0x80, 0x80	// F
};

void load_file(char game[30]);
void initialize(void);
uint8_t fetch_execute(uint8_t mode);

uint16_t i;	// register for memory operations
unsigned short v[16];	// registers V0 to VF
unsigned short pc;	// program counter
unsigned short stack[48];
unsigned short sp;	// stack pointer
unsigned short delay_timer;
unsigned short sound_timer;	//plays buzz when reaches 0;
uint8_t key[16];	// 16 input keys
uint8_t ram[MEM_LEN];
uint8_t display[64 * 32];
uint16_t opcode;
_Bool draw;
uint8_t mode = 0;

uint16_t cur_addr = 0x200;

void unknown_op(uint16_t opcode);

uint8_t cls(void);
uint8_t ret(void);
uint8_t old_jump(void);
uint8_t jump(void);
uint8_t call(void);
uint8_t ske(void);
uint8_t skne(void);
uint8_t skre(void);
uint8_t load(void);
uint8_t add(void);
uint8_t move(void);
uint8_t or(void);
uint8_t and(void);
uint8_t xor(void);
uint8_t addr(void);
uint8_t sub(void);
uint8_t shr(void);
uint8_t subr(void);
uint8_t shl(void);
uint8_t skrne(void);
uint8_t loadi(void);
uint8_t jumpi(void);
uint8_t rnd(void);
uint8_t drw(void);
uint8_t skpr(void);
uint8_t skup(void);
uint8_t moved(void);
uint8_t keyd(void);
uint8_t loadd(void);
uint8_t loads(void);
uint8_t addi(void);
uint8_t ldspr(void);
uint8_t bcd(void);
uint8_t stor(void);
uint8_t readr(void);
uint8_t opcodes_0(void);
uint8_t opcodes_8(void);
uint8_t opcodes_e(void);
uint8_t opcodes_f(void);
#endif
