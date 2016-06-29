#ifndef SYSCALL_H
#define SYSCALL_H 

#include "types.h"
#include "pcb.h"
#include "x86_desc.h"
#include "filesys.h"
#include "lib.h"
#include "paging.h"
#include "keyboard.h"
#include "rtc.h"

/* Macros for syscalls */
#define SYS_HALT    1
#define SYS_EXECUTE 2
#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_CLOSE   6
#define SYS_GETARGS 7
#define SYS_VIDMAP  8
#define SYS_SET_HANDLER  9
#define SYS_SIGRETURN  10
#define SYSCALLS 0x80
#define MAX_PROCESSES 6
#define PROGADDR 	0x08048000
#define _128MB		0x08000000

#define ELF_SIZE 4
//elf magic 
#define MAGIC0 0x7F
#define MAGIC1 0x45
#define MAGIC2 0x4C
#define MAGIC3 0x46

#define BUFFASIZE 128
#define BIT_A 24
#define BIT_B 25
#define BIT_C 26
#define BIT_D 27

#define SHIFT 8

#define STACKBOT 0x08400000

#define TEXTSCREENVIDMEM 0x8800000
#define STDIN_NUM 0
#define STDOUT_NUM 1
#define MIN_FDENTRY 0 
#define MAX_FDENTRY 7
#define FIRST_FDENTRY 2
#define JMPTABLE_SIZE 4

#define NUM_TERM 3

int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read(int32_t fd, void* buf, int32_t nbytes);
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t* buf, int32_t nbytes);
int32_t vidmap(uint8_t** screen_start);
int32_t set_handler(int32_t signum, void* handler);
int32_t sigreturn(void);

/* Helper Functions */
//int32_t terminal_init();

//EXTERNED SHELL 
extern int32_t shell_count;

extern void sys_call_handler();
extern int32_t (*fs_jmp_table[4])();
extern int32_t (*stdin_jmp_table[4])();
extern int32_t (*stdout_jmp_table[4])();
extern int32_t (*dir_jmp_table[4])();
extern int32_t (*rtc_jmp_table[4])(); 

extern int32_t prog_count[NUM_TERM];
#endif





