#ifndef _PAGING_H
#define _PAGING_H 

#include "lib.h"
#include "types.h"
#include "keyboard.h"

/* Paging Constants */
#define KERNEL_BEGIN    0x00400000	// 4MB bound
#define DIRECTORY_SIZE  1024
#define TABLE_SIZE      1024
#define PAGE_SIZE       4096
#define _4KB			4096
#define MAX_STACK		0x2000		// 8KB per spec.
#define _4MB			0x00400000
#define _8MB			0x00800000
#define _12MB 			0x00C00000
#define _16MB 			0x01000000
#define ADDR_MASK		0xFFFFF000	// address mask for top 20 bits
#define PRESENT_BIT		0x00000001 	// present bit mask
#define	RW_BIT			0x00000002 	// read/write bit set; 0 for read-only
#define SIZE_BIT		0x00000080 	// bitset to toggle 4MB pages, 0 for 4kB
#define GLOBAL_BIT		0x00000100 	// bitset for global page, only use for kernel 
#define PRWS_BIT		0x00000003 	// present, rw, super all in one
#define USER_BIT 		0x00000004
#define CACHE_BIT		0x00000010
#define IGNORE_BIT		0x00000080
#define cr4_BITSET      0x00000010 	// bit set 4 of cr4  	   
#define cr0_ENABLE 		0x80000001 	// enable paging  
#define _128PDENTRY		32
#define _136PDENTRY		34
#define _144PDENTRY 	36

/* Virtual Buffers */
#define VIRTUAL1		35
#define VIRTUAL2		36
#define VIRTUAL3		37
#define MB136			0x08800000 
#define VIDMEM			0x000B8000	// start of video memory

/* Initial Page Directory */
uint32_t page_directory[DIRECTORY_SIZE] __attribute__((aligned(_4KB)));
/* Initial Page Table */
uint32_t page_table[TABLE_SIZE] __attribute__((aligned(_4KB)));
/* Video Memory Table */
uint32_t vid_table[TABLE_SIZE] __attribute__((aligned(_4KB)));

/* Page Directories for Programs 1-6 */
uint32_t prog1_pd[DIRECTORY_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog2_pd[DIRECTORY_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog3_pd[DIRECTORY_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog4_pd[DIRECTORY_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog5_pd[DIRECTORY_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog6_pd[DIRECTORY_SIZE] __attribute__((aligned(_4KB)));

/* Page Tables for Programs 1-6 */
uint32_t prog1_pt[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog2_pt[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog3_pt[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog4_pt[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog5_pt[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog6_pt[TABLE_SIZE] __attribute__((aligned(_4KB)));

/* Video Tables for Programs 1-6 */
uint32_t prog1_vt[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog2_vt[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog3_vt[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog4_vt[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog5_vt[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog6_vt[TABLE_SIZE] __attribute__((aligned(_4KB)));

/* Terminal Buffer Pages for Programs 1-6 */
uint32_t prog1_term[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog2_term[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog3_term[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog4_term[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog5_term[TABLE_SIZE] __attribute__((aligned(_4KB)));
uint32_t prog6_term[TABLE_SIZE] __attribute__((aligned(_4KB)));

/* Paging Initialization */
void paging_init();

/* Initialization for Programs 1-6 */
void prog1_init();
void prog2_init();
void prog3_init();
void prog4_init();
void prog5_init();
void prog6_init();

#endif
