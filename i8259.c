/**
***	i8259.c: Includes functions to interact with the 8259 interrupt controller.
**/

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/*
 *	void i8259_init(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Initializes the 8259 PIC.
 */
void i8259_init(void)
{
	outb(MASK_ALL, MASTER_DATA); 									// mask all interrupts on master
	outb(MASK_ALL, SLAVE_DATA);  									// mask all interrupts on slave
	
	outb(ICW1, MASTER_COMMAND);  									// initialize master pic
	outb(ICW1, SLAVE_COMMAND);   									// initialize slave pic
	outb(ICW2_MASTER, MASTER_DATA); 								// set vector offset for master pic
	outb(ICW2_SLAVE, SLAVE_DATA); 									// set vector offset for slave pic
	outb(ICW3_MASTER, MASTER_DATA); 								// tells the master there is a slave pic at IRQ2
	outb(ICW3_SLAVE, SLAVE_DATA); 									// tells the slave it is cascaded
	outb(ICW4, MASTER_DATA); 										// additional info (no clue what it does)
	outb(ICW4, SLAVE_DATA); 
	
	outb(MASK_ALL, MASTER_DATA); 									// restore the masks
	outb(MASK_ALL, SLAVE_DATA);
}

/*
 *	void enable_irq(uint32_t irq_num);
 *  	Inputs: uint32_t irq_num - specified IRQ to enable
 *  	Return Value: none
 *		Function: Enables (unmasks) the specified IRQ.
 */
void enable_irq(uint32_t irq_num)
{	
	if(irq_num < IRQ_MAX)											// if interrupt request is 0-7
	{          	
		master_mask = inb(MASTER_DATA) & ~(BIT_MASK << irq_num); 	// read and change the master mask
		outb(master_mask, MASTER_DATA);  							// write the master mask
	} 
	else 															// otherwise write to the slave pic	 
	{              			     							 			
		irq_num = irq_num - IRQ_MAX; 								// remove offset in irq_num
		slave_mask = inb(SLAVE_DATA) & ~(BIT_MASK << irq_num); 		// read and change slave mask
		outb(slave_mask, SLAVE_DATA); 								// write the slave mask
	}
}

/*
 *	void enable_irq(uint32_t irq_num);
 *  	Inputs: uint32_t irq_num - specified IRQ to disable
 *  	Return Value: none
 *		Function: Disables (masks) the specified IRQ.
 */
void disable_irq(uint32_t irq_num)
{
	if(irq_num < IRQ_MAX)											// if interrupt request is 0-7
	{
		master_mask = inb(MASTER_DATA) | (BIT_MASK << irq_num); 	// read and change the master mask
		outb(master_mask, MASTER_DATA);  							// write the master mask
	}
	else 															// otherwise write to the slave pic
	{
		irq_num = irq_num - IRQ_MAX; 								// remove offset in irq_num
		slave_mask = inb(SLAVE_DATA) | (BIT_MASK << irq_num); 		// read and change slave mask
		outb(slave_mask, SLAVE_DATA); 								// write the slave mask
	}
}

/* Send end-of-interrupt signal for the specified IRQ */


/*
 *	void send_eoi(uint32_t irq_num);
 *  	Inputs: uint32_t irq_num - specified IRQ to send_eoi
 *  	Return Value: none
 *		Function: Sends end-of-interrupt signal for the specified IRQ.
 */
void send_eoi(uint32_t irq_num)
{
	if(irq_num >= IRQ_MAX)
	{
		irq_num = irq_num - IRQ_MAX;
		outb(EOI | irq_num, SLAVE_COMMAND);  						// inform the slave
		outb(EOI | SLAVE_IRQ, MASTER_COMMAND); 						// inform the master
	}
	else
	{
		outb(EOI | irq_num, MASTER_COMMAND);  						// inform the master
	}
}
