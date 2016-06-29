/**
***	keyboard.c: Includes functions for keyboard handling as well as terminal system calls.
**/

#include "keyboard.h"

/**
***	Global Variables:
**/

/* Flags for ctrl, shift, caps lock, and alt keys, in that order */
static int functionflags[NUMTERMINALS][FUNCTIONSIZE] = { {OFF, OFF, OFF, OFF}, 
													     {OFF, OFF, OFF, OFF},
													     {OFF, OFF, OFF, OFF} };

static volatile int enter_flag[NUMTERMINALS] = {OFF, OFF, OFF};
static volatile int read_flag[NUMTERMINALS] = {OFF, OFF, OFF};

/* Buffer and index */
static char keyboardbuffer[NUMTERMINALS][BUFFERSIZE];
static int bufferindex[NUMTERMINALS] = {0, 0, 0};
static char writebuffer[NUMTERMINALS][BUFFERSIZE];
static int writeindex[NUMTERMINALS] = {0, 0, 0};

/* Four arrays used for four cases with caps lock and shift */
static char firstarray[EIGHTBITS]; 	//	lowercase letters and numbers
static char secondarray[EIGHTBITS];	// 	uppercase letters and special characters
static char thirdarray[EIGHTBITS];	// 	uppercase letters and numbers
static char fourtharray[EIGHTBITS];	// 	lowercase letters and special characters

int32_t current_terminal = 0;



/**
***	Keyboard Handling:
**/

/*
 *	void clear_keyboard_buffer(int terminal);
 *  	Inputs: terminal - specific terminal to clear
 *  	Return Value: none
 *		Function: Clears keyboard buffer on a specific terminal.
 */
void clear_keyboard_buffer(int terminal)
{
	int i;
	for (i = 0; i < BUFFERSIZE; i++)
	{
		keyboardbuffer[current_terminal][i] = 0;								// initialize keyboard buffer
	}
	bufferindex[current_terminal] = 0;											// reset index
}

/*
 *	void keyboard_handler(void);
 *  	Inputs: void
 *   	Return Value: none
 *		Function: Handles keys pressed and released on the keyboard.
 */
void keyboard_handler(void)
{
	int i;
	uint8_t scancode = inb(KB_ENCODER);
	int32_t ascii;
	init_arrays();

	switch (scancode) 																				// identify the key pressed and handle it accordingly
	{
		case ALT:
			functionflags[current_terminal][ALTINDEX] = ON;											// turn on the alt flag
			send_eoi(KEYBOARD);
			return;
		case ALTRELEASE:
			functionflags[current_terminal][ALTINDEX] = OFF;										// turn off the alt flag
			send_eoi(KEYBOARD);
			return;

		case LSHIFT:
		case RSHIFT:
			functionflags[current_terminal][SHIFTINDEX] = ON;										// turn on the shift flag
			send_eoi(KEYBOARD);
			return;
		case LSHIFTRELEASE:
		case RSHIFTRELEASE:
			functionflags[current_terminal][SHIFTINDEX] = OFF;										// turn off the shift flag
			send_eoi(KEYBOARD);
			return;

		case CTRL:
			functionflags[current_terminal][CTRLINDEX] = ON;										// turn on the ctrl flag
			send_eoi(KEYBOARD);
			return;
		case CTRLRELEASE:
			functionflags[current_terminal][CTRLINDEX] = OFF;										// turn off the ctrl flag
			send_eoi(KEYBOARD);
			return;

		case CAPSLOCK: 																				// we want to invert the caps lock flag
			if (functionflags[current_terminal][CAPSLOCKINDEX] == ON)
			{
				functionflags[current_terminal][CAPSLOCKINDEX] = OFF;
			}
			else if (functionflags[current_terminal][CAPSLOCKINDEX] == OFF)
			{
				functionflags[current_terminal][CAPSLOCKINDEX] = ON;
			}
			send_eoi(KEYBOARD);
			return;

		case BACKSPACE: 																			// we want to erase the last element pressed
			if (bufferindex[current_terminal] != 0)
			{
				(bufferindex[current_terminal])--;
			}
			keyboardbuffer[current_terminal][bufferindex[current_terminal]] = NULL; 				// erase the element at that index
			setcoords(writeindex[current_terminal], getycoord(current_terminal), current_terminal); // reset screen_x

			if (bufferindex[current_terminal] >= NUM_COLS - writeindex[current_terminal])			// another edge case for reaching the right side of terminal
			{
				clearline(0, getycoord(current_terminal) + 1);
			}
			else
			{
				clearline(writeindex[current_terminal], getycoord(current_terminal));
			}

			for (i = 0; i < bufferindex[current_terminal]; i++)										// loop through each element of the buffer
			{
				if (i >= NUM_COLS - writeindex[current_terminal])
				{
					if (getycoord(current_terminal) == NUM_ROWS - 1)								// scroll if we reached the bottom of the terminal
					{
						scroll(current_terminal);
					}
					else
					{
						setcoords(getxcoord(current_terminal), getycoord(current_terminal) + 1, current_terminal);
					}
				}
				putc(keyboardbuffer[current_terminal][i]);											// reprint the buffer
				if (i >= NUM_COLS - writeindex[current_terminal])
				{
					setcoords(getxcoord(current_terminal), getycoord(current_terminal) - 1, current_terminal);
				}
			}

			setcoords(writeindex[current_terminal], getycoord(current_terminal), current_terminal); // set coords to the end of writeindex 

			send_eoi(KEYBOARD);
			return;

		case ENTER:
			keyboardbuffer[current_terminal][bufferindex[current_terminal]] = '\n';					// insert new line into the buffer
			(bufferindex[current_terminal])++;

			enter_flag[current_terminal] = ON;

			term_putc('\n');
			if (bufferindex[current_terminal] + writeindex[current_terminal] > NUM_COLS + 1)		// edge case
			{
				term_putc('\n');	
			}

			if (read_flag[current_terminal] == OFF)													// case for terminal_read
			{
				clear_keyboard_buffer(current_terminal);
			}

			send_eoi(KEYBOARD);
			return;
		case ENTERRELEASE:
			enter_flag[current_terminal] = OFF;														// turn off the enter flag
			send_eoi(KEYBOARD);
			return;

		default:
			if (functionflags[current_terminal][ALTINDEX] == ON)									// alt + FN key -> calls a terminal switch
			{
				switch (scancode)
				{
					case F1:
						terminal_switch(TERM_1);
						send_eoi(KEYBOARD);
						return;
					case F2:
						terminal_switch(TERM_2);	
						send_eoi(KEYBOARD);
						return;
					case F3:
						terminal_switch(TERM_3);
						send_eoi(KEYBOARD);
						return;
				}
			}

			if (functionflags[current_terminal][CTRLINDEX] == ON && scancode == L)					// reset the screen to the first line and retain anything typed
			{
				clear();
				setcoords(0, 0, current_terminal);													// set coords to upper left

				for (i = 0; i < writeindex[current_terminal]; i++)									// print anything in the write buffer (usually just 391OS>)
				{
					putc(writebuffer[current_terminal][i]);
				}

				for (i = 0; i <= bufferindex[current_terminal]; i++)
				{
					putc(keyboardbuffer[current_terminal][i]);
				}

				setcoords(getxcoord(current_terminal) - bufferindex[current_terminal] - 1, getycoord(current_terminal), current_terminal);

				send_eoi(KEYBOARD);
				return;
			}

			if (scancode & KEYRELEASE) 																// if a key was just released, we don't want to print anything
			{
				send_eoi(KEYBOARD);
				return;
			}

			// depending on which function keys are active, select the appropriate array
			if (functionflags[current_terminal][CAPSLOCKINDEX] == OFF && functionflags[current_terminal][SHIFTINDEX] == OFF)
			{
				ascii = firstarray[scancode];
			}
			else if (functionflags[current_terminal][CAPSLOCKINDEX] == OFF && functionflags[current_terminal][SHIFTINDEX] == ON)
			{
				ascii = secondarray[scancode];
			}
			else if (functionflags[current_terminal][CAPSLOCKINDEX] == ON && functionflags[current_terminal][SHIFTINDEX] == OFF)
			{
				ascii = thirdarray[scancode];
			}
			else if (functionflags[current_terminal][CAPSLOCKINDEX] == ON && functionflags[current_terminal][SHIFTINDEX] == ON)
			{
				ascii = fourtharray[scancode];
			}

			if (ascii == 0)																			// this is for keys that aren't mapped (insert, delete, etc.)
			{
				send_eoi(KEYBOARD);
				return;
			}

			if (bufferindex[current_terminal] >= BUFFERSIZE)										// don't add anything to the buffer if we have already reached 128 characters
			{
				send_eoi(KEYBOARD);
				return;
			}
			
			writeindex[current_terminal] = getxcoord(current_terminal);

			keyboardbuffer[current_terminal][bufferindex[current_terminal]] = ascii;				// add character to the buffer
			bufferindex[current_terminal]++;

			for (i = 0; i < bufferindex[current_terminal]; i++)
			{
				if (i >= NUM_COLS - writeindex[current_terminal])									// offsets if the characters reach the right side of the terminal
				{
					setcoords(getxcoord(current_terminal), getycoord(current_terminal) + 1, current_terminal);
				}
				putc(keyboardbuffer[current_terminal][i]);
				if (i >= NUM_COLS - writeindex[current_terminal])
				{
					setcoords(getxcoord(current_terminal), getycoord(current_terminal) - 1, current_terminal);
				}
			}

			setcoords(writeindex[current_terminal], getycoord(current_terminal), current_terminal);	// reset the coordinates back to the writebuffer

			send_eoi(KEYBOARD);
			return;
	}
}



/*
 *	int32_t terminal_open();
 *  	Inputs: none
 *   	Return Value: Returns 0.
 *		Function: Open function for the terminal driver.
 */
int32_t terminal_open(const uint8_t * filename)
{
	int i;
	
	for (i = 0; i < FUNCTIONSIZE; i++) 							// initialize function keys to OFF
	{
		functionflags[current_terminal][i] = OFF;
	}

	clear_keyboard_buffer(current_terminal);
	return 0;
}

/**
***	Terminal System Calls:
**/

/*
 *	int32_t terminal_read(int32_t fd, char * buf, int32_t nbytes);
 *  	Inputs: fd 	   - specific file descriptor
 				buf    - buffer that is read into
 *				nbytes - number of bytes to read
 *   	Return Value: Returns the number of bytes read.
 *		Function: Read function for the terminal driver.
 */
int32_t terminal_read(int32_t fd, char * buf, int32_t nbytes)
{
	int current_terminal_read = current_terminal; 				// sets this for the entire duration of terminal_read
	int i;
	int bytes_read = 0;

	if (buf == NULL) 											// error checking
	{
		return -1;
	}
	if (nbytes < 0)
	{
		return -1;
	}
	sti();

	enter_flag[current_terminal_read] = OFF;
	read_flag[current_terminal_read] = ON;

	while (enter_flag[current_terminal_read] == OFF); 			// spin in this while loop until enter is pressed

	for (i = 0; i < nbytes; i++) 								// clear the buffer
	{
		buf[i] = NULL;
	}

	for (i = 0; i < nbytes; i++) 								// copy the keyboardbuffer into the buffer
	{
		buf[i] = keyboardbuffer[current_terminal_read][i];
		bytes_read++;
		if (keyboardbuffer[current_terminal_read][i] == '\n') 	// if enter is pressed, break out of loop
		{
			break;
		}
	}

	clear_keyboard_buffer(current_terminal);

	read_flag[current_terminal_read] = OFF;
	return bytes_read;
}

/*
 *	int32_t terminal_write(int32_t fd, const char * buf, int32_t nbytes);
 *  	Inputs: fd 	   - specific file descriptor
 				buf    - buffer that is read from
 *				nbytes - number of bytes to write
 *   	Return Value: Returns the number of bytes written.
 *		Function: Write function for the terminal driver.
 */
int32_t terminal_write(int32_t fd, const char * buf, int32_t nbytes)
{
	int i;
	if (buf == NULL)													// error checking
	{
		return -1;
	}
	if (nbytes < 0)
	{
		return -1;
	}

	for (i = 0; i < nbytes; i++)										// clear out the writebuffer
	{
		writebuffer[current_terminal][i] = 0;
	}

	writeindex[current_terminal] = 0;
	for (i = 0; i < nbytes; i++)
	{
		writebuffer[current_terminal][i] = *(buf + i);					// add the current character to the writebuffer and increment the index
		writeindex[current_terminal]++;

		if (*(buf + i) != '\n' || getxcoord(current_terminal) != 0)		// have term_putc handle everything unless there's a new line character 
		{
			term_putc(*(buf + i));
		}
	}

	return i + 1; // this is equal to nbytes if it puts all the characters to the screen
}

/*
 *	int32_t terminal_close(int32_t fd);
 *  	Inputs: fd - specific file descriptor
 *   	Return Value: Returns -1.
 *		Function: Close function for the terminal driver.
 */
int32_t terminal_close(int32_t fd)
{
	return -1;
}

/*
 *	int32_t terminal_switch(int32_t term_num);
 *  	Inputs: term_num - terminal number we want to switch to
 *   	Return Value: Returns 0 on success.
 *		Function: Switches the active terminal.
 */
int32_t terminal_switch(int32_t term_num)
{
	int32_t * buf_loc = NULL;										// pointer to the background buffer location
	int32_t espsave;	
	int32_t ebpsave;

	if(current_terminal == term_num)
	{
		return 0;
	}

	switch(current_terminal) 										// save the current video memory into corresponding background buffer
	{
		case TERM_1:
			buf_loc = (int32_t*) B_BUF_1;
			break;
		case TERM_2:
			buf_loc = (int32_t*) B_BUF_2;
			break;
		case TERM_3:
			buf_loc = (int32_t*) B_BUF_3;
			break;
	}	
	memcpy((void *)buf_loc, (const void*) VID_MEM, VID_MEM_SIZE);

	switch(term_num)												// load corresponding background buffer into video memory
	{
		case TERM_1:
			buf_loc = (int32_t*) B_BUF_1;
			break;
		case TERM_2:
			buf_loc = (int32_t*) B_BUF_2;
			break;
		case TERM_3:
			buf_loc = (int32_t*) B_BUF_3;
			break;
	}

	memcpy((void *) VID_MEM, (const void*) buf_loc, VID_MEM_SIZE);

	uint32_t i;
	for(i = 0; i < VID_MEM_SIZE; i++)								// After memcpy, set video attribute, or else nothing will show up
    {
        *(uint8_t *)(VID_MEM + (i << 1) + 1) = LINEATTRIBUTE;
    }

	asm volatile("movl %%esp, %0" 
                     : "=a"(espsave)
                     :
                     : "cc" );

	asm volatile("movl %%ebp, %0" 
                     : "=a"(ebpsave)
                     :
                     : "cc" );

	pcb_loc[current_terminal]->curr_esp = espsave; 					// save esp
	pcb_loc[current_terminal]->curr_ebp = ebpsave;					// save ebp

	current_terminal = term_num;									// change the current terminal
	send_eoi(KEYBOARD);

	if (prog_count[current_terminal] == 0)
	{
		if (prog_count[TERM_1] + prog_count[TERM_2] + prog_count[TERM_3] < MAX_PROCESSES)
		{
			while(1)
			{
				execute((uint8_t*)"shell");
				printk("Relaunching Shell...\n");
			}
		}
		else
		{
			clear();
			keyboard_helper();
			return 0;
		}
	}

	tss.ss0 = KERNEL_DS;
	tss.esp0 = 0x00800000 - (pcb_loc[current_terminal]->pid + 1) * MAX_STACK;
	asm volatile("					\n\
			movl 	%0, %%ebp		\n\
			movl 	%1, %%esp		\n\
			movl	%2, %%cr3		\n\
			"
			:
			: "r"(pcb_loc[current_terminal]->curr_ebp), "r"(pcb_loc[current_terminal]->curr_esp), "r"(pcb_loc[current_terminal]->curr_pd)
			: "memory");  

	return 0;
}



/*
 *	void init_arrays();
 *  	Inputs: none
 *   	Return Value: none
 *		Function: Initializes each of the four scancode to ascii arrays.
 */
void init_arrays()
{
	// No keys: lowercase letters and numbers
	firstarray[A] = 0x61; // a
	firstarray[B] = 0x62; // b
	firstarray[C] = 0x63; // c
	firstarray[D] = 0x64; // d
	firstarray[E] = 0x65; // e
	firstarray[F] = 0x66; // f
	firstarray[G] = 0x67; // g
	firstarray[H] = 0x68; // h
	firstarray[I] = 0x69; // i
	firstarray[J] = 0x6A; // j
	firstarray[K] = 0x6B; // k
	firstarray[L] = 0x6C; // l
	firstarray[M] = 0x6D; // m
	firstarray[N] = 0x6E; // n
	firstarray[O] = 0x6F; // o
	firstarray[P] = 0x70; // p
	firstarray[Q] = 0x71; // q
	firstarray[R] = 0x72; // r
	firstarray[S] = 0x73; // s
	firstarray[T] = 0x74; // t
	firstarray[U] = 0x75; // u
	firstarray[V] = 0x76; // v
	firstarray[W] = 0x77; // w
	firstarray[X] = 0x78; // x
	firstarray[Y] = 0x79; // y
	firstarray[Z] = 0x7A; // z
 
	firstarray[ONE]   = 0x31; // 1
	firstarray[TWO]   = 0x32; // 2
	firstarray[THREE] = 0x33; // 3
	firstarray[FOUR]  = 0x34; // 4
	firstarray[FIVE]  = 0x35; // 5
	firstarray[SIX]   = 0x36; // 6
	firstarray[SEVEN] = 0x37; // 7
	firstarray[EIGHT] = 0x38; // 8
	firstarray[NINE]  = 0x39; // 9
	firstarray[ZERO]  = 0x30; // 0

	firstarray[GRAVEACCENT]  = 0x60; // `
	firstarray[DASH]   		 = 0x2D; // -
	firstarray[EQUALS] 		 = 0x3D; // =
	firstarray[LEFTBRACKET]  = 0x5B; // [
	firstarray[RIGHTBRACKET] = 0x5D; // ]
	firstarray[BACKSLASH]    = 0x5C; // 
	firstarray[SEMICOLON]    = 0x3B; // ;
	firstarray[APOSTROPHE]   = 0x27; // '
	firstarray[COMMA]  		 = 0x2C; // ,
	firstarray[PERIOD]  	 = 0x2E; // .
	firstarray[FORWARDSLASH] = 0x2F; // /

	firstarray[SPACE] 		 = 0x20; // 

	// Shift key: uppercase letters and special characters
	secondarray[A] = 0x41; // A
	secondarray[B] = 0x42; // B
	secondarray[C] = 0x43; // C
	secondarray[D] = 0x44; // D
	secondarray[E] = 0x45; // E
	secondarray[F] = 0x46; // F
	secondarray[G] = 0x47; // G
	secondarray[H] = 0x48; // H
	secondarray[I] = 0x49; // I
	secondarray[J] = 0x4A; // J
	secondarray[K] = 0x4B; // K
	secondarray[L] = 0x4C; // L
	secondarray[M] = 0x4D; // M
	secondarray[N] = 0x4E; // N
	secondarray[O] = 0x4F; // O
	secondarray[P] = 0x50; // P
	secondarray[Q] = 0x51; // Q
	secondarray[R] = 0x52; // R
	secondarray[S] = 0x53; // S
	secondarray[T] = 0x54; // T
	secondarray[U] = 0x55; // U
	secondarray[V] = 0x56; // V
	secondarray[W] = 0x57; // W
	secondarray[X] = 0x58; // X
	secondarray[Y] = 0x59; // Y
	secondarray[Z] = 0x5A; // Z

	secondarray[ONE]   = 0x21; // !
	secondarray[TWO]   = 0x40; // @
	secondarray[THREE] = 0x23; // #
	secondarray[FOUR]  = 0x24; // $
	secondarray[FIVE]  = 0x25; // %
	secondarray[SIX]   = 0x5E; // ^
	secondarray[SEVEN] = 0x26; // &
	secondarray[EIGHT] = 0x2A; // *
	secondarray[NINE]  = 0x28; // (
	secondarray[ZERO]  = 0x29; // )

	secondarray[GRAVEACCENT]  = 0x7E; // ~
	secondarray[DASH]   	  = 0x5F; // _
	secondarray[EQUALS] 	  = 0x2B; // +
	secondarray[LEFTBRACKET]  = 0x7B; // {
	secondarray[RIGHTBRACKET] = 0x7D; // }
	secondarray[BACKSLASH]    = 0x7C; // |
	secondarray[SEMICOLON]    = 0x3A; // :
	secondarray[APOSTROPHE]   = 0x22; // "
	secondarray[COMMA]  	  = 0x3C; // <
	secondarray[PERIOD]  	  = 0x3E; // >
	secondarray[FORWARDSLASH] = 0x3F; // ?

	secondarray[SPACE] 		  = 0x20; // 

	// Caps Lock key: uppercase letters and numbers
	thirdarray[A] = 0x41; // A
	thirdarray[B] = 0x42; // B
	thirdarray[C] = 0x43; // C
	thirdarray[D] = 0x44; // D
	thirdarray[E] = 0x45; // E
	thirdarray[F] = 0x46; // F
	thirdarray[G] = 0x47; // G
	thirdarray[H] = 0x48; // H
	thirdarray[I] = 0x49; // I
	thirdarray[J] = 0x4A; // J
	thirdarray[K] = 0x4B; // K
	thirdarray[L] = 0x4C; // L
	thirdarray[M] = 0x4D; // M
	thirdarray[N] = 0x4E; // N
	thirdarray[O] = 0x4F; // O
	thirdarray[P] = 0x50; // P
	thirdarray[Q] = 0x51; // Q
	thirdarray[R] = 0x52; // R
	thirdarray[S] = 0x53; // S
	thirdarray[T] = 0x54; // T
	thirdarray[U] = 0x55; // U
	thirdarray[V] = 0x56; // V
	thirdarray[W] = 0x57; // W
	thirdarray[X] = 0x58; // X
	thirdarray[Y] = 0x59; // Y
	thirdarray[Z] = 0x5A; // Z

	thirdarray[ONE]   = 0x31; // 1
	thirdarray[TWO]   = 0x32; // 2
	thirdarray[THREE] = 0x33; // 3
	thirdarray[FOUR]  = 0x34; // 4
	thirdarray[FIVE]  = 0x35; // 5
	thirdarray[SIX]   = 0x36; // 6
	thirdarray[SEVEN] = 0x37; // 7
	thirdarray[EIGHT] = 0x38; // 8
	thirdarray[NINE]  = 0x39; // 9
	thirdarray[ZERO]  = 0x30; // 0

	thirdarray[GRAVEACCENT]  = 0x60; // `
	thirdarray[DASH]   		 = 0x2D; // -
	thirdarray[EQUALS] 		 = 0x3D; // =
	thirdarray[LEFTBRACKET]  = 0x5B; // [
	thirdarray[RIGHTBRACKET] = 0x5D; // ]
	thirdarray[BACKSLASH]    = 0x5C; // 
	thirdarray[SEMICOLON]    = 0x3B; // ;
	thirdarray[APOSTROPHE]   = 0x27; // '
	thirdarray[COMMA]  		 = 0x2C; // ,
	thirdarray[PERIOD]  	 = 0x2E; // .
	thirdarray[FORWARDSLASH] = 0x2F; // /

	thirdarray[SPACE] 		 = 0x20; // 

	// Caps Lock key and Shift key: lowercase letters and special characters
	fourtharray[A] = 0x61; // a
	fourtharray[B] = 0x62; // b
	fourtharray[C] = 0x63; // c
	fourtharray[D] = 0x64; // d
	fourtharray[E] = 0x65; // e
	fourtharray[F] = 0x66; // f
	fourtharray[G] = 0x67; // g
	fourtharray[H] = 0x68; // h
	fourtharray[I] = 0x69; // i
	fourtharray[J] = 0x6A; // j
	fourtharray[K] = 0x6B; // k
	fourtharray[L] = 0x6C; // l
	fourtharray[M] = 0x6D; // m
	fourtharray[N] = 0x6E; // n
	fourtharray[O] = 0x6F; // o
	fourtharray[P] = 0x70; // p
	fourtharray[Q] = 0x71; // q
	fourtharray[R] = 0x72; // r
	fourtharray[S] = 0x73; // s
	fourtharray[T] = 0x74; // t
	fourtharray[U] = 0x75; // u
	fourtharray[V] = 0x76; // v
	fourtharray[W] = 0x77; // w
	fourtharray[X] = 0x78; // x
	fourtharray[Y] = 0x79; // y
	fourtharray[Z] = 0x7A; // z

	fourtharray[ONE]   = 0x21; // !
	fourtharray[TWO]   = 0x40; // @
	fourtharray[THREE] = 0x23; // #
	fourtharray[FOUR]  = 0x24; // $
	fourtharray[FIVE]  = 0x25; // %
	fourtharray[SIX]   = 0x5E; // ^
	fourtharray[SEVEN] = 0x26; // &
	fourtharray[EIGHT] = 0x2A; // *
	fourtharray[NINE]  = 0x28; // (
	fourtharray[ZERO]  = 0x29; // )

	fourtharray[GRAVEACCENT]  = 0x7E; // ~
	fourtharray[DASH]   	  = 0x5F; // _
	fourtharray[EQUALS] 	  = 0x2B; // +
	fourtharray[LEFTBRACKET]  = 0x7B; // {
	fourtharray[RIGHTBRACKET] = 0x7D; // }
	fourtharray[BACKSLASH]    = 0x7C; // |
	fourtharray[SEMICOLON]    = 0x3A; // :
	fourtharray[APOSTROPHE]   = 0x22; // "
	fourtharray[COMMA]  	  = 0x3C; // <
	fourtharray[PERIOD]  	  = 0x3E; // >
	fourtharray[FORWARDSLASH] = 0x3F; // ?

	fourtharray[SPACE] 		  = 0x20; // 
}

// this is because spec said so. 
void keyboard_helper(void){
	printk("\n\n\n\n      Okay, let me put it this way. You done messed up A-A-RON. \n\n");
	printk("      You clearly, CLEARLY, state in the doc that at max, only four processes\n");
	printk("      are supported on one terminal.\n");
	printk("      But guess what? You didn't follow the doc. And now we're going to be\n");
	printk("      penalized, for YOUR mistakes.\n\n");
	printk("      Does that seem fair to you?\n\n");
	printk("      I mean, why would you do this to us? WHY?!\n");
	printk("      All we wanted, was to see our OS grow into something beautiful.\n");
	printk("      But instead, it's going to page fault, and the worst part is,\n");
	printk("      it's     N E V E R    E V E R    E V E R     going to recover.\n");
	printk("\n      THANK YOU, AND HAVE A NICE DAY.\n\n\n");
}

