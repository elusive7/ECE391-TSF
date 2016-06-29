#ifndef PCB_H
#define PCB_H

#include "types.h"
#include "filesys.h"
#include "lib.h"
#include "syscall.h"

#define FOPS_NUM 8					// Number of max files 
#define ARG_SIZE 128				// Length of argument
#define PCB0_LOC 0x7FE000 			// location of first PCB
#define PCB1_LOC 0x7FC000			// location of shell 1 PCB
#define PCB2_LOC 0x7FA000			// location of shell 2 PCB
#define PCB3_LOC 0x7F8000			// location of shell 3 PCB
#define EIGHT_KB 0x2000 
#define FD_SIZE 16 					// size of an fd_entry	

// 40 is from 10 uint32_t PCB values, 2 is from uint8_t
#define PCB_SIZE FOPS_NUM*FD_SIZE + ARG_SIZE + 40 + 2
#define CALL_OPEN 0
#define CALL_READ 1
#define CALL_WRITE 2
#define CALL_CLOSE 3
#define NUM_TERM 3

typedef int32_t (*func_t)(); 

// File descriptor entry
typedef struct fd_entry{
	func_t* fops_ptr;
	uint32_t inode_ptr;
	uint32_t file_pos;
	uint32_t flags;	
} fd_entry_t;

/*
	PCB needs pid to jump back to current stack location after 
	completion of program
	Both parent and current esp,ebp,page_directorys are needed
	to correctly return after halt
*/
typedef struct pcb {
	
	uint32_t pid;					// process id	
	uint32_t esp;					// esp,ebp, pagedir for shell
	uint32_t ebp;
	uint32_t pagedir;				
	uint32_t curr_esp;				// esp,ebp,eip,page directory for user program
	uint32_t curr_ebp;
	uint32_t curr_eip;
	uint32_t curr_pd;
	uint32_t term_num;				// Terminal number on which program displays
	int8_t status;					// Status code for return values
	struct pcb * lastpcb_ptr; 		// Pointer to parent pcb
	uint8_t args[ARG_SIZE];			// space for process' arguments
	fd_entry_t file_desc[FOPS_NUM]; // process' file descriptor array

} pcb_t;

/* Array of PCB for three terminals */
extern pcb_t* pcb_loc[NUM_TERM];

/* Creates a new pcb */
pcb_t pcb_init();

/* maps the new pcb over to the proper memory location */
int32_t setup_pcb(pcb_t* new_pcb, pcb_t* pcb_loc, pcb_t* prev_pcb);

#endif 







