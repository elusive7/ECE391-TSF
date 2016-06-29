#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "i8259.h"
#include "lib.h"
#include "types.h"
#include "syscall.h"

/* Keyboard Constants */
#define KB_ENCODER 0x60
#define KB_CONTROLLER 0x64
#define KEYBOARD 0x1

/* Keyboard Scan Codes */ 
#define A 0x1E
#define B 0x30
#define C 0x2E
#define D 0x20
#define E 0x12
#define F 0x21
#define G 0x22
#define H 0x23
#define I 0x17
#define J 0x24
#define K 0x25
#define L 0x26
#define M 0x32
#define N 0x31
#define O 0x18
#define P 0x19
#define Q 0x10
#define R 0x13
#define S 0x1F
#define T 0x14
#define U 0x16
#define V 0x2F
#define W 0x11
#define X 0x2D
#define Y 0x15
#define Z 0x2C

#define ONE 	0x02
#define TWO 	0x03
#define THREE 	0x04
#define FOUR 	0x05
#define FIVE 	0x06
#define SIX 	0x07
#define SEVEN 	0x08
#define EIGHT 	0x09
#define NINE 	0x0A
#define ZERO 	0x0B

#define GRAVEACCENT 	0x29
#define DASH 			0x0C
#define EQUALS			0x0D
#define LEFTBRACKET		0x1A
#define RIGHTBRACKET	0x1B
#define BACKSLASH		0x2B
#define SEMICOLON		0x27
#define	APOSTROPHE		0x28
#define COMMA			0x33
#define	PERIOD			0x34
#define FORWARDSLASH	0x35

#define ALT				0x38
#define ALTRELEASE		0xB8
#define BACKSPACE 		0x0E
#define CAPSLOCK 		0x3A
#define CAPSLOCKRELEASE 0xBA
#define CTRL 			0x1D
#define CTRLRELEASE		0x9D
#define ENTER 			0x1C
#define ENTERRELEASE	0x9C
#define LSHIFT 			0x2A
#define LSHIFTRELEASE 	0xAA
#define RSHIFT 			0x36
#define RSHIFTRELEASE 	0xB6

#define SPACE			0x39
#define F1				0x3B
#define F2				0x3C
#define F3				0x3D

/* Other Miscellaneous Constants */
#define NUM_COLS 		80
#define NUM_ROWS 		25
#define BUFFERSIZE 		128
#define EIGHTBITS 		256
#define KEYRELEASE 		0x80

#define	CTRLINDEX 		0
#define SHIFTINDEX 		1
#define CAPSLOCKINDEX 	2
#define	ALTINDEX 		3
#define FUNCTIONSIZE 	4

#define OFF 			0
#define ON 				1

#define LINEATTRIBUTE	0x07

/* Constants for multiple terminals */
#define TERM_1 			0
#define TERM_2			1
#define TERM_3			2
#define VID_MEM_SIZE	0x1000
#define VID_MEM 		0xB8000  
#define B_BUF_1			0x09000000
#define B_BUF_2			0x09100000
#define B_BUF_3			0x09200000
#define NUMTERMINALS	3

/* Array Initialization */
void init_arrays();

/* Keyboard Handler */
void keyboard_handler(void);

/* Terminal System Calls */
int32_t terminal_open(const uint8_t * filename);
int32_t terminal_read(int32_t fd, char * buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const char * buf, int32_t nbytes);
int32_t terminal_close(int32_t fd);

/* Multiple Terminal Functions */
int32_t terminal_switch(int32_t term_num); 

/* Helper functions */
void keyboard_helper(void);

extern int32_t current_terminal;

#endif
