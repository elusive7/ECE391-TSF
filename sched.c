#include "sched.h"

/* queues for the scheduler */ 
static process_t* run_queue = NULL;
static process_t* expired_queue = NULL;
static uint32_t sched_esp; 

/*
 *	void scheduler()
 *  	Inputs: none
 *  	Return Value: none
 *		Function: Called be the pit to schedule programs (DOES NOT WORK)
 */
void scheduler(){
	/* save the scheduling esp to start next process at the correct location */
	asm volatile("				\n\
		cli						\n\
		movl %%esp, %0"  		
		: "=a"(sched_esp)
		:
		: "cc" );

	disable_irq(PIT);  //disable the pit
	send_eoi(PIT);
	/* remove first node of run_queue */ 
	process_t* cur_process = run_queue;  // save the current process
	if(cur_process != NULL){ 
		run_queue = run_queue->next;	  // set the new head
		cur_process->next = NULL;			
	
	
		/* if process is not done, add to expired queue */
		if(!cur_process->finished){
			if(expired_queue == NULL){		// if empty, set the process as the head
				expired_queue = cur_process;
			} else {						// else traverse till the end
				process_t* temp = expired_queue;
				while(temp->next != NULL){
					temp = temp->next;
				}
				temp->next = cur_process;  // put the node at the end
			}
		}	

		/* if there are no processes to schedule, return  */
		if(run_queue == NULL && expired_queue == NULL){
			sti();
			enable_irq(PIT);
			return;
		}

		/* Swap queues when rotation is complete */
		if(run_queue == NULL && expired_queue == NULL){
			process_t* temp = run_queue;
		 	run_queue = expired_queue;
		 	expired_queue = temp;
		}
	}
	tss.ss0 = KERNEL_DS;
	tss.esp0 = 0x800000 - (run_queue->curr->pid + 1) * MAX_STACK;		

	/* load the process page directory and esp and ebp */
	asm volatile("					\n\
		movl 	%0, %%ebp		\n\
		movl 	%1, %%esp		\n\
		movl	%2, %%cr3		\n\
		"
		:
		: "r"(run_queue->curr->curr_ebp), "r"(run_queue->curr->curr_esp), "r"(run_queue->curr->curr_pd)
		: "memory");
	
	return;
}

/*
 *	void add_prog_to_queue(pcb_t* pcb_loc)
 *  	Inputs: pcb_loc - pcb of the current process
 *  	Return Value: none
 *		Function: Adds the process's pcb to the run queue
 */
void add_prog_to_queue(pcb_t* pcb_loc){
	process_t* new_process;       	// create pointer to new process
	process_t* temp = run_queue;	// create temp pointer for traversing the list			
	new_process->curr = pcb_loc;
	new_process->finished = 0;
	new_process->next = NULL; 
	/* if there are no processes, make this the head */
	if(run_queue == NULL)
		run_queue = new_process;  
	else {   /* else put it at the end */
		while(temp->next != NULL){    
			temp = temp->next;
		}
		temp->next = new_process;
	}
}

/*
 *	void remove_prog_from_queue()
 *  	Inputs: none
 *  	Return Value: none
 *		Function: set the state of the current process to finished 
 */
void remove_prog_from_queue(){
	run_queue->finished = 1;  
}



