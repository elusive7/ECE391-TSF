#ifndef SCHED_H
#define SCHED_H

#include "types.h"
#include "syscall.h"
#include "lib.h"
#include "pit.h"
#include "pcb.h"

#define MAX_PROGS 3
#define PROCESS_1 0
#define PROCESS_2 1
#define PROCESS_3 2

void add_prog_to_queue();

void remove_prog_from_queue();

void scheduler();

/* Contains the pcb for current process, flag for finished, and ptr to next process */
typedef struct process {
	struct pcb* curr;
	int32_t finished;
	struct process* next;
} process_t;





//void init_shells();

//extern int32_t restore_flag;


#endif


