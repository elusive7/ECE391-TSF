#include "pit.h" 

/*
 *	void set_pit_rate(int hz);
 *  	Inputs: hz - rate PIT will be set to
 *   	Return Value: none
 *		Function: Sets the pit rate.
 */
void set_pit_rate(int hz)
{
	int divisor = OSCILLATOR / hz;				// uses the oscillator frequency to calculate the value to send to channel 0

	outb(MODE2, COMMANDREG);					// sets a rate generator byte to the command register
	outb(divisor & LOWERMASK, CHANNEL0);		// writes the lower byte to channel 0
	outb(divisor >> UPPERSHIFT, CHANNEL0);		// writes the upper byte to channel 0
}

/*
 *	void pit_init();
 *  	Inputs: none
 *   	Return Value: none
 *		Function: Initializes the PIT to 18.2 Hz.
 */
void pit_init()
{
	outb(MODE2, COMMANDREG);					// sets a rate generator byte to the command register
	outb(MAXDIV & LOWERMASK, CHANNEL0);			// writes the lower byte to channel 0
	outb(MAXDIV >> UPPERSHIFT, CHANNEL0);		// writes the upper byte to channel 0
}

/*
 *	void pit_handler();
 *  	Inputs: none
 *   	Return Value: none
 *		Function: Handles PIT interrupts.
 */
void pit_handler()
{
	//scheduler();
	send_eoi(PIT);
}
