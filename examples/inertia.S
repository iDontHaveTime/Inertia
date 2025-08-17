	/* THIS IS JUST A REFERENCE FOR FUTURE OUTPUT */
	.text
	.extern printf
	.globl main
	.p2align 4
	.type main,@function
main:
	.cfi_startproc
	subq $8, %rsp
	xorl %eax, %eax
	# sysv vararg


	leaq .str0(%rip), %rdi
	movq printf@GOTPCREL(%rip), %r10 
	call *%r10
	
	addq $8, %rsp
	xorl %eax, %eax
	ret
.Lfunc_main_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc

.section .rodata
	.str0: .asciz "Hello, World!\n"
