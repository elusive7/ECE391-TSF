/**
***	paging.c: Includes functions for paging.
*** COMMENTED 
*** MAGICKED
**/

#include "paging.h"

/**
***	Paging Functions:
**/

/*
 *	void paging_init();
 *  	Inputs: none
 *   	Return Value: none
 *		Function: Initializes paging. Sets up the page table and the page directory.
 *				  Enables paging and extended paging.
 *				  Updates CR3 with the address of the page directory.
 */
void paging_init()
{
	uint32_t i;
	uint32_t val = (uint32_t)page_table;

	val = val &	ADDR_MASK;
	val = val | PRESENT_BIT;			// present
	val = val | RW_BIT;					// rw
	page_directory[0] = val;

	val = 0;
	for(i = 0; i < TABLE_SIZE; i++) 	// setup first table from 0-4MB
	{									
		page_table[i] = val | PRWS_BIT; // sets present and super
		val += PAGE_SIZE;				// 0-4MB should be reserved for kernel
	}

	val = KERNEL_BEGIN;					// Kernel begins at 4MB 
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | GLOBAL_BIT; 			// global
	page_directory[1] = val;

	val = _8MB; 						// Initial page setup for first user program
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | USER_BIT; 				// user
	page_directory[_128PDENTRY] = val; 

	page_table[0] = 0;					// initial page value should be NULL

	val = VIDMEM;						// video memory is mapped to 0xB8000
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	vid_table[0] = val;

	val = (uint32_t)vid_table;			// We assigned 136MB to be user vid mem
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	page_directory[_136PDENTRY] = val;

	asm volatile("mov %0, %%cr3":: "b"(page_directory));	// moves page directory address into the cr3 register
	
	uint32_t cr4;											// enables 4MB pages by toggling bit 4 in cr4
	asm volatile("mov %%cr4, %0": "=b"(cr4));
	cr4 |= cr4_BITSET;
	asm volatile("mov %0, %%cr4":: "b"(cr4));

	uint32_t cr0;											//enables paging by toggling bit 31 in cr0
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= cr0_ENABLE;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
}


/* 
 * prog1_init
 * Adds a page of program mem of 4MB to physical address
 * and map it to virtual address 128MB
 * INPUTS: physical_address
 * OUTPUTS: none
 * EFFECTS: page specified by physical address gets mapped to 128MB
 * 			Note that all following prog#_init() functions are exact 
 *  		copies of this function except each has its own distinct
 *			page directory and page tables. 
 */
void prog1_init()
{
 	uint32_t val = (uint32_t)prog1_pt;
	val = val &	ADDR_MASK;
	val = val | PRESENT_BIT;			// present
	val = val | RW_BIT; 				// rw 
	prog1_pd[0] = val;

	val = KERNEL_BEGIN;
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | GLOBAL_BIT; 			// global
	prog1_pd[1]= val;

	val = 0;
	uint32_t i;
	for (i = 0; i < TABLE_SIZE; i++)	// setup first table from 0-4MB
	{
		prog1_pt[i] = val|PRWS_BIT; 	// sets present and super
		val+=PAGE_SIZE;
	}

	val = _8MB + _4MB * 0; 				// Note that for each subsequent user page,
	val = val | PRESENT_BIT; 			// we must increment the physical mapping
	val = val | RW_BIT; 				// by 4MB 
	val = val | SIZE_BIT; 				 
	val = val | USER_BIT; 				 
	prog1_pd[_128PDENTRY] = val; 
	prog1_pt[0] = 0;					// initial page value should be null

	val = VIDMEM;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog1_vt[0] = val;

	val = (uint32_t)prog1_vt;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog1_pd[_136PDENTRY] = val;

	val = _16MB + _16MB;				// Memory mapping for video buffers is 
	for (i = 0; i < TABLE_SIZE; i++)	// set at 32MB 
	{									// The virtual address is set to 144MB
		val = val & ADDR_MASK;
		val = val | PRESENT_BIT;
		val = val | RW_BIT;
		prog1_term[i] = val;
		val += PAGE_SIZE;
	}

	val = (uint32_t)prog1_term;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	prog1_pd[_144PDENTRY] = val;

	asm volatile("mov %0, %%cr3":: "b"(prog1_pd)); 	//moves page_directory address into the cr3 register
}

void prog2_init()
{
 	uint32_t val = (uint32_t)prog2_pt;
	val = val &	ADDR_MASK;
	val = val | PRESENT_BIT;			// present
	val = val | RW_BIT; 				// rw 
	prog2_pd[0] = val;

	val = KERNEL_BEGIN;
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | GLOBAL_BIT; 			// global
	prog2_pd[1]= val;

	val = 0;
	uint32_t i;
	for (i = 0; i < TABLE_SIZE; i++)	// setup first table from 0-4MB
	{
		prog2_pt[i] = val|PRWS_BIT; 	// sets present and super
		val+=PAGE_SIZE;
	}

	val = _8MB + _4MB * 1; 
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | USER_BIT; 				// user
	prog2_pd[_128PDENTRY] = val; 
	prog2_pt[0] = 0;					// initial page value should be null

	val = VIDMEM;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog2_vt[0] = val;

	val = (uint32_t)prog2_vt;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog2_pd[_136PDENTRY] = val;

	val = _16MB + _16MB;
	for (i = 0; i < TABLE_SIZE; i++)
	{
		val = val & ADDR_MASK;
		val = val | PRESENT_BIT;
		val = val | RW_BIT;
		prog2_term[i] = val;
		val += PAGE_SIZE;
	}

	val = (uint32_t)prog2_term;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	prog2_pd[_144PDENTRY] = val;

	asm volatile("mov %0, %%cr3":: "b"(prog2_pd));	//moves page_directory address into the cr3 register
}

void prog3_init()
{
 	uint32_t val = (uint32_t)prog3_pt;
	val = val &	ADDR_MASK;
	val = val | PRESENT_BIT;			// present
	val = val | RW_BIT; 				// rw 
	prog3_pd[0] = val;

	val = KERNEL_BEGIN;
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | GLOBAL_BIT; 			// global
	prog3_pd[1]= val;

	val = 0;
	uint32_t i;
	for (i = 0; i < TABLE_SIZE; i++)	// setup first table from 0-4MB
	{
		prog3_pt[i] = val|PRWS_BIT; // sets present and super
		val+=PAGE_SIZE;
	}

	val = _8MB + _4MB * 2;
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | USER_BIT; 				// user
	prog3_pd[_128PDENTRY] = val; 
	prog3_pt[0] = 0;					// initial page value should be null

	val = VIDMEM;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog3_vt[0] = val;

	val = (uint32_t)prog3_vt;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog3_pd[_136PDENTRY] = val;

	val = _16MB + _16MB;
	for (i = 0; i < TABLE_SIZE; i++)
	{
		val = val & ADDR_MASK;
		val = val | PRESENT_BIT;
		val = val | RW_BIT;
		prog3_term[i] = val;
		val += PAGE_SIZE;
	}

	val = (uint32_t)prog3_term;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	prog3_pd[_144PDENTRY] = val;

	asm volatile("mov %0, %%cr3":: "b"(prog3_pd));	//moves page_directory address into the cr3 register
}

void prog4_init()
{
 	uint32_t val = (uint32_t)prog4_pt;
	val = val &	ADDR_MASK;
	val = val | PRESENT_BIT;			// present
	val = val | RW_BIT; 				// rw 
	prog4_pd[0] = val;

	val = KERNEL_BEGIN;
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | GLOBAL_BIT; 			// global
	prog4_pd[1]= val;

	val = 0;
	uint32_t i;
	for (i = 0; i < TABLE_SIZE; i++)	// setup first table from 0-4MB
	{
		prog4_pt[i] = val|PRWS_BIT; 	// sets present and super
		val+=PAGE_SIZE;
	}

	val = _8MB + _4MB * 3; 
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | USER_BIT; 				// user
	prog4_pd[_128PDENTRY] = val; 
	prog4_pt[0] = 0;					// initial page value should be null

	val = VIDMEM;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog4_vt[0] = val;

	val = (uint32_t)prog4_vt;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog4_pd[_136PDENTRY] = val;

	val = _16MB + _16MB;
	for (i = 0; i < TABLE_SIZE; i++)
	{
		val = val & ADDR_MASK;
		val = val | PRESENT_BIT;
		val = val | RW_BIT;
		prog4_term[i] = val;
		val += PAGE_SIZE;
	}

	val = (uint32_t)prog4_term;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	prog4_pd[_144PDENTRY] = val;

	asm volatile("mov %0, %%cr3":: "b"(prog4_pd));	//moves page_directory address into the cr3 register
}

void prog5_init()
{
 	uint32_t val = (uint32_t)prog5_pt;
	val = val &	ADDR_MASK;
	val = val | PRESENT_BIT;			// present
	val = val | RW_BIT; 				// rw 
	prog5_pd[0] = val;

	val = KERNEL_BEGIN;
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | GLOBAL_BIT; 			// global
	prog5_pd[1]= val;

	val = 0;
	uint32_t i;
	for (i = 0; i < TABLE_SIZE; i++)	// setup first table from 0-4MB
	{
		prog5_pt[i] = val|PRWS_BIT; 	// sets present and super
		val+=PAGE_SIZE;
	}

	val = _8MB + _4MB * 4; 
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | USER_BIT; 				// user
	prog5_pd[_128PDENTRY] = val; 
	prog5_pt[0] = 0;					// initial page value should be null

	val = VIDMEM;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog5_vt[0] = val;

	val = (uint32_t)prog5_vt;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog5_pd[_136PDENTRY] = val;

	val = _16MB + _16MB;
	for (i = 0; i < TABLE_SIZE; i++)
	{
		val = val & ADDR_MASK;
		val = val | PRESENT_BIT;
		val = val | RW_BIT;
		prog5_term[i] = val;
		val += PAGE_SIZE;
	}

	val = (uint32_t)prog5_term;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	prog5_pd[_144PDENTRY] = val;

	asm volatile("mov %0, %%cr3":: "b"(prog5_pd));	//moves page_directory address into the cr3 register
}

void prog6_init()
{
 	uint32_t val = (uint32_t)prog6_pt;
	val = val &	ADDR_MASK;

	val = val | PRESENT_BIT;			// present
	val = val | RW_BIT; 				// rw 
	prog6_pd[0] = val;

	val = KERNEL_BEGIN;
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | GLOBAL_BIT; 			// global
	prog6_pd[1]= val;

	val = 0;
	uint32_t i;
	for (i = 0; i < TABLE_SIZE; i++)	// setup first table from 0-4MB
	{
		prog6_pt[i] = val|PRWS_BIT; 	// sets present and super
		val+=PAGE_SIZE;
	}

	val = _8MB + _4MB * 5;
	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | USER_BIT; 				// user

	val = val | PRESENT_BIT; 			// present
	val = val | RW_BIT; 				// rw
	val = val | SIZE_BIT; 				// size
	val = val | USER_BIT; 				// user
	prog6_pd[_128PDENTRY] = val; 
	prog6_pt[0] = 0;					// initial page value should be null

	val = VIDMEM;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog6_vt[0] = val;

	val = (uint32_t)prog6_vt;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	val = val | USER_BIT;
	prog6_pd[_136PDENTRY] = val;

	val = _16MB + _16MB;
	for (i = 0; i < TABLE_SIZE; i++)
	{
		val = val & ADDR_MASK;
		val = val | PRESENT_BIT;
		val = val | RW_BIT;
		prog6_term[i] = val;
		val += PAGE_SIZE;
	}

	val = (uint32_t)prog6_term;
	val = val & ADDR_MASK;
	val = val | PRESENT_BIT;
	val = val | RW_BIT;
	prog6_pd[_144PDENTRY] = val;

	asm volatile("mov %0, %%cr3":: "b"(prog6_pd));	//moves page_directory address into the cr3 register
}
