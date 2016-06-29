/*
 * 	COMMENTED 
 *  MAGICKED 
 */ 

#include "x86_desc.h"
#include "lib.h"
#include "handlers.h"
#include "i8259.h"
#include "keyboard.h"
#include "rtc.h"
#include "pit.h"
#include "syscall.h"

/*
 *	void divide_zero(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a divide error.
 */
void divide_zero(void)
{
	cli();
	printk("Divide Error Exception\n");
	while(1);
}

/*
 *	void debug_exception(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a debug exception.
 */
void debug_exception(void)
{
	cli();
	printk("Debug Exception\n");
	while(1);
}

/*
 *	void nm_interrupt(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a non-maskable interrupt.
 */
void nm_interrupt(void)
{
	cli();
	printk("NMI Interrupt\n");
	while(1);
}

/*
 *	void breakpoint_exception(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a breakpoint exception.
 */
void breakpoint_exception(void)
{
	cli();
	printk("Breakpoint Exception\n");
	while(1);
}

/*
 *	void id_overflow(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports an overflow exception.
 */
void id_overflow(void)
{
	cli();
	printk("Overflow Exception\n");
	while(1);
}

/*
 *	void out_of_bounds(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a BOUND-range-exceeded exception.
 */
void out_of_bounds(void)
{
	cli();
	printk("BOUND Range Exceeded Exception\n");
	while(1);
}

/*
 *	void invalid_opcode(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports an invalid opcode exception.
 */
void invalid_opcode(void)
{
	cli();
	printk("Invalid Opcode Exception\n");
	while(1);
}

/*
 *	void nm_interrupt(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a non-maskable interrupt.
 */
void no_coprocessor(void)
{
	cli();
	printk("No coprocessor exception\n");
	while(1);
}

/*
 *	void double_fault(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a double fault.
 */
void double_fault(void)
{
	cli();
	printk("Double Fault Exception\n");
	while(1);
}

/*
 *	void nm_interrupt(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a non-maskable interrupt.
 */
void coprocessor_overrun(void)
{
	cli();
	printk("Coprocessor Segment Overrun\n");
	while(1);
}

/*
 *	void bad_tss(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a invalid TSS.
 */
void bad_tss(void)
{
	cli();
	printk("Invalid TSS Exception\n");
	while(1);
}

/*
 *	void segment_not_present(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a segment not present.
 */
void segment_not_present(void)
{
	cli();
	printk("Segment Not Present\n");
	while(1);
}

/*
 *	void nm_interrupt(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a stack fault.
 */
void stack_fault(void)
{
	cli();
	printk("Stack Fault Exception\n");
	while(1);
}

/*
 *	void general_protection_fault(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a general protection fault.
 */
void general_protection_fault(void)
{
	cli();
	printk("General Protection Exception\n");
	while(1);
}

/*
 *	void page_fault(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a page fault.
 */
void page_fault(void)
{
	cli();
	uint32_t paddr=0; 
	asm volatile("			\n\
		mov %%cr2, %%eax	\n\
		mov %%eax, %0 		\n\
		"
		:"=m"(paddr)
		://no inputs
		:"eax"
		);

	printf("Page-Fault at address: 0x%x\n",paddr);
	while(1);
}

/*
 *	void nm_interrupt(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a non-maskable interrupt.
 */
void unknown_interrupt(void)
{
	cli();
	printk("Unknown interrupt exception\n");
	while(1);
}

/*
 *	void nm_interrupt(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a non-maskable interrupt.
 */
void coprocessor_fault(void)
{
	cli();
	printk("Coprocessor fault\n");
	while(1);
}

/*
 *	void alignment_check(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports an alignment check exception.
 */
void alignment_check(void)
{
	cli();
	printk("Alignment Check Exception\n");
	while(1);
}

/*
 *	void nm_interrupt(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a non-maskable interrupt.
 */
void machine_check(void)
{
	cli();
	printk("Machine-Check Exception\n");
	while(1);
}

/*
 *	void nm_interrupt(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Reports a non-maskable interrupt.
 */
void floating_point(void)
{
	cli();
	printk("SIMD Floating-Point Exception\n");
	while(1);
}

/*
 *	void pit(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Calls the pit handler.
 */
void pit(void)
{
	pit_handler();
}

/*
 *	void keyboard(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Calls the keyboard handler.
 */
void keyboard(void)
{
	keyboard_handler();
}

/*
 *	void rtc(void);
 *  	Inputs: void
 *  	Return Value: none
 *		Function: Calls the RTC handler.
 */
void rtc(void)
{
	rtc_handler();
}

void setup_exceptions(void)
{
	int32_t i;
	//go through all exceptions, set reserved bits and size = 1. Individual functions
	for(i = 0 ; i < NUM_EXCEPTIONS; i++)
	{
		idt[i].present = 1;
		idt[i].dpl = 0;
		idt[i].reserved0 = 0;
		idt[i].size = 1; 
		idt[i].reserved1 = 1; 
		idt[i].reserved2 = 1;
		idt[i].reserved3 = 0;
		idt[i].seg_selector = KERNEL_CS;
	}
	
	//use provided macro to setup function offset.
	SET_IDT_ENTRY(idt[0], (uint32_t)&divide_zero1);
	SET_IDT_ENTRY(idt[1], (uint32_t)&debug_exception1);
	SET_IDT_ENTRY(idt[2], (uint32_t)&nm_interrupt1);
	SET_IDT_ENTRY(idt[3], (uint32_t)&breakpoint_exception1);
	SET_IDT_ENTRY(idt[4], (uint32_t)&id_overflow1);
	SET_IDT_ENTRY(idt[5], (uint32_t)&out_of_bounds1);
	SET_IDT_ENTRY(idt[6], (uint32_t)&invalid_opcode1);
	SET_IDT_ENTRY(idt[7], (uint32_t)&no_coprocessor1);
	SET_IDT_ENTRY(idt[8], (uint32_t)&double_fault1);
	SET_IDT_ENTRY(idt[9], (uint32_t)&coprocessor_overrun1);
	SET_IDT_ENTRY(idt[10], (uint32_t)&bad_tss1);
	SET_IDT_ENTRY(idt[11], (uint32_t)&segment_not_present1);
	SET_IDT_ENTRY(idt[12], (uint32_t)&stack_fault1);
	SET_IDT_ENTRY(idt[13], (uint32_t)&general_protection_fault1);
	SET_IDT_ENTRY(idt[14], (uint32_t)&page_fault1);
	SET_IDT_ENTRY(idt[15], (uint32_t)&unknown_interrupt1);
	SET_IDT_ENTRY(idt[16], (uint32_t)&coprocessor_fault1);
	SET_IDT_ENTRY(idt[17], (uint32_t)&alignment_check1);
	SET_IDT_ENTRY(idt[18], (uint32_t)&machine_check1);
	SET_IDT_ENTRY(idt[19], (uint32_t)&floating_point1);

	//setup PIT
	idt[PIT_IDT].present = 1;
	idt[PIT_IDT].dpl = 0;
	idt[PIT_IDT].reserved0 = 0;
	idt[PIT_IDT].size = 1; 
	idt[PIT_IDT].reserved1 = 1;
	idt[PIT_IDT].reserved2 = 1;
	idt[PIT_IDT].reserved3 = 1;
	idt[PIT_IDT].seg_selector = KERNEL_CS;
	SET_IDT_ENTRY(idt[PIT_IDT], (uint32_t)&pit1);	

	//setup keyboard
	idt[KEYBOARD_IDT].present = 1;
	idt[KEYBOARD_IDT].dpl = 0;
	idt[KEYBOARD_IDT].reserved0 = 0;
	idt[KEYBOARD_IDT].size = 1; 
	idt[KEYBOARD_IDT].reserved1 = 1;
	idt[KEYBOARD_IDT].reserved2 = 1;
	idt[KEYBOARD_IDT].reserved3 = 1;
	idt[KEYBOARD_IDT].seg_selector = KERNEL_CS;
	SET_IDT_ENTRY(idt[KEYBOARD_IDT], (uint32_t)&keyboard1);	
	
	//setup RTC
	idt[RTC_IDT].present = 1;
	idt[RTC_IDT].dpl = 0;
	idt[RTC_IDT].reserved0 = 0;
	idt[RTC_IDT].size = 1;
	idt[RTC_IDT].reserved1 = 1;
	idt[RTC_IDT].reserved2 = 1;
	idt[RTC_IDT].reserved3 = 1;
	idt[RTC_IDT].seg_selector = KERNEL_CS;
	SET_IDT_ENTRY(idt[RTC_IDT], (uint32_t)&rtc1);

	//setup syscalls
	idt[SYSCALLS].present = 1;
	idt[SYSCALLS].dpl = USERPRV; 			// user privilege
	idt[SYSCALLS].reserved0 = 0;
	idt[SYSCALLS].size = 1;
	idt[SYSCALLS].reserved1 = 1;
	idt[SYSCALLS].reserved2 = 1;
	idt[SYSCALLS].reserved3 = 0;
	idt[SYSCALLS].seg_selector = KERNEL_CS;
	SET_IDT_ENTRY(idt[SYSCALLS], (uint32_t)&sys_call_handler);
}
