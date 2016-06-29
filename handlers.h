#ifndef _IDT_H
#define _IDT_H

/* IDT Constants */
#define NUM_EXCEPTIONS 	0x20
#define SLAVE 			0x02

/* Indices for Interrupts */
#define PIT_IDT			32
#define KEYBOARD_IDT 	33
#define RTC_IDT 		40

/* Function Declarations */
extern void setup_exceptions(void);
extern void test_exception(void);

/* Exception Wrapper Declarations */
extern void divide_zero1(void);
extern void debug_exception1(void);
extern void nm_interrupt1(void);
extern void breakpoint_exception1(void);
extern void id_overflow1(void);
extern void out_of_bounds1(void);
extern void invalid_opcode1(void);
extern void no_coprocessor1(void);
extern void double_fault1(void);
extern void coprocessor_overrun1(void);
extern void bad_tss1(void);
extern void segment_not_present1(void);
extern void stack_fault1(void);
extern void general_protection_fault1(void);
extern void page_fault1(void);
extern void unknown_interrupt1(void);
extern void coprocessor_fault1(void);
extern void alignment_check1(void);
extern void machine_check1(void);
extern void floating_point1(void);

/* Interrupt Wrapper Declarations */
extern void pit1(void);
extern void keyboard1(void);
extern void rtc1(void);

#endif
