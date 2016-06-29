
.globl sys_call_handler
	sys_call_handler:
	cmpl $0, %EAX					// check lower bound of syscall number
	jz sys_call_error				// error if 0
	cmpl $10, %EAX		// check upper bound of syscall number
	ja sys_call_error				// error if above bound

	push %EBX						// callee save registers
	push %ECX
	push %EDX
	push %ESI
	push %EDI 
	push %EBP

	pushl	%ds
	pushl	%gs
	pushl	%fs
	pushl	%es

	movl	$0x18, %EDI 	// USER_MODE
	movl	%EDI, %ds
	movl	%EDI, %gs
	movl	%EDI, %fs
	movl	%EDI, %es

	subl $1, %EAX 					// remove the offset
	pushl %EDX						//
	pushl %ECX						// push the arguments
	pushl %EBX						//	
	call *sys_call_table(, %eax, 4) // 
	addl $12, %esp   				// pop the arguments

	popl	%es
	popl	%fs
	popl	%gs
	popl	%ds

	popl %EBP						// restore registers
	popl %EDI
	popl %ESI
	popl %EDX
	popl %ECX
	popl %EBX    
	iret

sys_call_error:
	mov $-1, %eax
	iret


sys_call_table:
	.long halt 
	.long execute
	.long read
	.long write
	.long open
	.long close
	.long getargs
	.long vidmap
	.long set_handler
	.long sigreturn






