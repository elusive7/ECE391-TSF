/* Exception Wrappers */
.globl divide_zero1
.globl debug_exception1
.globl nm_interrupt1
.globl breakpoint_exception1
.globl id_overflow1
.globl out_of_bounds1
.globl invalid_opcode1
.globl no_coprocessor1
.globl double_fault1
.globl coprocessor_overrun1
.globl bad_tss1
.globl segment_not_present1
.globl stack_fault1
.globl general_protection_fault1
.globl page_fault1
.globl unknown_interrupt1
.globl coprocessor_fault1
.globl alignment_check1
.globl machine_check1
.globl floating_point1

divide_zero1:
	pushal
	call divide_zero
	popal
	iret

debug_exception1:	
	pushal
	call debug_exception
	popal
	iret

nm_interrupt1:	
	pushal
	call nm_interrupt
	popal
	iret

breakpoint_exception1:	
	pushal
	call breakpoint_exception
	popal
	iret

id_overflow1:	
	pushal
	call id_overflow
	popal
	iret

out_of_bounds1:	
	pushal
	call out_of_bounds
	popal
	iret

invalid_opcode1:
	pushal 
	call invalid_opcode
	popal
	iret

no_coprocessor1:	
	pushal
	call no_coprocessor
	popal
	iret

double_fault1:	
	pushal
	call double_fault
	popal
	iret

coprocessor_overrun1:	
	pushal
	call coprocessor_overrun
	popal
	iret

bad_tss1:
	pushal
	call bad_tss
	popal
	iret

segment_not_present1:
	pushal
	call segment_not_present
	popal
	iret

stack_fault1:	
	pushal
	call stack_fault
	popal
	iret

general_protection_fault1:
	pushal
	call general_protection_fault
	popal
	iret

page_fault1:	
	pushal
	call page_fault
	popal
	iret

unknown_interrupt1:	
	pushal
	call unknown_interrupt
	popal
	iret

coprocessor_fault1:	
	pushal
	call coprocessor_fault
	popal
	iret

alignment_check1:	
	pushal
	call alignment_check
	popal
	iret

machine_check1:	
	pushal
	call machine_check
	popal
	iret

floating_point1:	
	pushal
	call floating_point
	popal
	iret

/* Interrupt Wrappers */
.globl pit1
.globl keyboard1
.globl rtc1

pit1:
	pushal
	call pit
	popal
	iret

keyboard1:
	pushal
	call keyboard
	popal
	iret

rtc1:
	pushal
	call rtc
	popal
	iret
