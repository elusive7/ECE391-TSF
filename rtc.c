#include "rtc.h"

/**
***	Local Variable:
**/
static int32_t int_flag = LOW;

/**
***	RTC Initialization and Handling:
**/

/*
 *	void rtc_init();
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Initializes the RTC.
 */
void rtc_init()
{
	outb(NMI_DISABLE | STATUS_B, RTC_INDEX);			// select register b + turn off NMI
	int8_t prev = inb(RTC_DATA);						// read the current value of the register	
	outb(NMI_DISABLE | STATUS_B, RTC_INDEX);			// select the register again
	outb(prev | INT_ON, RTC_DATA);						// write to the register to turn on interrupts
	outb(STATUS_B, RTC_INDEX);							// turn on NMI
}

/*
 *	void rtc_handler();
 *  	Inputs: void
 *   	Return Value: none
 *		Function: Handles RTC interrupts.
 */
void rtc_handler()
{
	//test_interrupts();
	sti();
	enable_irq(RTC);
	int_flag = HIGH;
	outb(STATUS_C, RTC_INDEX);							// pick register c
	inb(RTC_DATA);										// throw away the contents
	send_eoi(RTC);										// send an end of interrupt signal
}



/**
***	Terminal System Calls:
**/

/*
 *	int32_t rtc_open(const uint8_t * filename);
 *  	Inputs: filename - does nothing
 *   	Return Value: Returns 0.
 *		Function: Open function for the RTC.
 *				  Sets RTC to 2 Hz.
 */
int32_t rtc_open(const uint8_t * filename)
{
	rtc_init();
	outb(NMI_DISABLE | STATUS_A, RTC_INDEX);			// select register A + turn off NMI
	int8_t prev = inb(RTC_DATA);						// reads the current value of the register
	outb(NMI_DISABLE | STATUS_A, RTC_INDEX);			// select register A again 
	outb((prev & RATE_MASK) | TWO_HZ, RTC_DATA);		// set rate to 2 hz
	outb(STATUS_B, RTC_INDEX);
	return 0;
}

/*
 *	int32_t rtc_read(int32_t fd, char * buf, int32_t nbytes);
 *  	Inputs: fd     - does nothing
 *				buf    - does nothing
 * 				nbytes - does nothing
 *   	Return Value: Returns 0.
 *		Function: Read function for the RTC.
 *				  Waits for an RTC interrupt, and then returns.
 */
int32_t rtc_read(int32_t fd, char * buf, int32_t nbytes)
{
	sti();
	while(int_flag != HIGH);  // waits for interrupt handler to trigger before proceeding
	int_flag = LOW;
	return 0;
}

/*
 *	int32_t rtc_write(int32_t fd, const char * buf, int32_t nbytes);
 *  	Inputs: fd     - does nothing
 * 				buf    - buffer with the first element being the 
 *					     frequency to be set.
 * 				nbytes - does nothing
 *   	Return Value: Returns 0 on success, -1 on failure.
 *		Function: Write function for the RTC.
 *				  Sets a new frequency for the RTC.
 */
int32_t rtc_write(int32_t fd, const char * buf, int32_t nbytes)
{
	int32_t frequency = (int32_t)buf[0];
	int32_t temp = frequency;
	int32_t rate = 1;

	if (frequency < FREQ_MIN || frequency > FREQ_MAX) 		// checks that the rate is within 2 and 1024 Hz
	{
		printk("Invalid frequency. Outside range.\n");
		return -1;
	}

	if ((frequency & (frequency - 1))) 						// checks that the rate is a power of 2
	{
		printk("Invalid frequency. Not a power of two.\n");
		return -1;
	}

	temp = THEORY_MAX / frequency;
	while (temp != 1)										// calculates the rate the RTC uses
	{
		rate++;
		temp /= 2;
	}

	outb(NMI_DISABLE | STATUS_A, RTC_INDEX);				// select register A + turn off NMI
	char prev = inb(RTC_DATA);								// reads the current value of the register
	outb(NMI_DISABLE | STATUS_A, RTC_INDEX);				// select register A again 
	outb((prev & RATE_MASK) | rate, RTC_DATA);				// set rate to whatever rate is
	outb(STATUS_B, RTC_INDEX);

	return 0;
}

/*
 *	int32_t rtc_close(int32_t fd);
 *  	Inputs: void
 *   	Return Value: Returns 0.
 *		Function: Close function for the RTC.
 */
int32_t rtc_close(int32_t fd)
{
	return 0;
}




