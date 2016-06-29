#ifndef PIT_H
#define PIT_H

#include "i8259.h"
#include "lib.h"

/* PIT Constants */
#define MINDIV 		1
#define MAXDIV 		65536

#define PIT 		0x00
#define CHANNEL0	0x40
#define COMMANDREG	0x43

#define MODE2		0x34
#define MODE3		0x36

#define OSCILLATOR 	1193182
#define LOWERMASK	0xFF
#define UPPERSHIFT	8

/* PIT Functions */
void set_pit_rate(int hz);
void pit_init();
void pit_handler();

#endif
