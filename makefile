CC=cc
CFLAGS_INC := -lSDL2
CFLAGS := -pg -Wall -O3
PREFIX = /usr/local/share

all: chip8 c8asm 

chip8: cpu.c
	$(CC) -o chip8 cpu.c $(CFLAGS) $(CFLAGS_INC)

c8asm: asm.c
	$(CC) -o c8asm asm.c $(CFLAGS)

.PHONY:	install
install: chip8 c8asm 
	cp chip8 /usr/local/bin/
	cp c8asm /usr/local/bin/
	cp doc/chip8.1 $(PREFIX)/man/man1/chip8.1
	gzip $(PREFIX)/man/man1/chip8.1
	cp doc/c8asm.1 $(PREFIX)/man/man1/c8asm.1
	gzip $(PREFIX)/man/man1/c8asm.1

.PHONY: uninstall
uninstall:
	rm -f /usr/local/bin/chip8
	rm -f /usr/local/bin/c8asm
	rm -f $(PREFIX)/man/man1/chip8.1
	rm -f $(PREFIX)/man/man1/c8asm.1

.PHONY: clean
clean:
	rm chip8 c8asm
	$(CC) -o chip8 cpu.c $(CFLAGS) $(CFLAGS_INC)
	$(CC) -o c8asm asm.c $(CFLAGS) $(CFLAGS_INC)
