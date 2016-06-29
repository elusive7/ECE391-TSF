/*
 * 	COMMENTED 
 *  MAGICKED 
 */ 
#include "pcb.h"

/*	pcb_init
 * 	INPUTS: none
 *  OUTPUTS: returns a newly initiliazed pcb	
 *  NOTES: Initializes PCB values, set FOPS table for stdin/stdout
 *			Returns PCB to be copied into kernel.	
 */ 
pcb_t pcb_init(){
	pcb_t new_pcb;                      // create new pcb
	int i;
	for(i = 0; i < FOPS_NUM; i++){		// clear fd_entry
		new_pcb.file_desc[i].fops_ptr = (func_t*) 0; 
		new_pcb.file_desc[i].inode_ptr = 0;
		new_pcb.file_desc[i].file_pos = 0; 
		new_pcb.file_desc[i].flags = 0; 
	}
	new_pcb.file_desc[STDIN_NUM].fops_ptr = stdin_jmp_table; 
	new_pcb.file_desc[STDIN_NUM].inode_ptr = NULL;
	new_pcb.file_desc[STDIN_NUM].file_pos = 0;
	new_pcb.file_desc[STDIN_NUM].flags = 1;

	new_pcb.file_desc[STDOUT_NUM].fops_ptr = stdout_jmp_table; 
	new_pcb.file_desc[STDOUT_NUM].inode_ptr = NULL;
	new_pcb.file_desc[STDOUT_NUM].file_pos = 0;
	new_pcb.file_desc[STDOUT_NUM].flags = 1;

	for(i = 0; i < ARG_SIZE; i++){  // clear args 
		new_pcb.args[i] = 0; 
	}
	new_pcb.pid = 0;
	new_pcb.esp = 0;
	new_pcb.ebp = 0;
	new_pcb.pagedir=0;
	new_pcb.curr_esp = 0;
	new_pcb.curr_ebp = 0;
	new_pcb.curr_eip = 0;
	new_pcb.curr_pd = 0;
	new_pcb.status = 0;

	return new_pcb;
}

/*	setup_pcb
 * 	INPUTS: new_pcb : pcb pointer to source 
 *			pcb_loc : pcb pointer to dest
 *			prev_pcb: last pcb on stack
 *  OUTPUTS: returns 0 on success
 *  NOTES: Copies over new_pcb to pcb_loc (which is 
 *			current pcb) and sets the lastpcb pointer
 *			to prev_pcb
 */ 
int32_t setup_pcb(pcb_t * new_pcb, pcb_t* pcb_loc, pcb_t* prev_pcb){

	//copy new pcb from current space into kernel space.
	memcpy((void*) pcb_loc, new_pcb, PCB_SIZE);  

	// set pointer to last pcb
	pcb_loc->lastpcb_ptr = prev_pcb; 

	return 0;
}
	


	
