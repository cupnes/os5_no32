	.text

	.global default_handler
default_handler:
	jmp	default_handler

	.global	kbc_handler
kbc_handler:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	push	%rsp
	call	do_ir_keyboard
	pop	%rsp
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global	exception_handler
exception_handler:
	jmp	exception_handler
	iretq
