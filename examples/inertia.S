	.text
	.extern printf
	.globl main
	.p2align 4
	.type main,@function
main:
	subq $8, %rsp
	xorl %eax, %eax
	# sysv vararg


	leaq .str0(%rip), %rdi
	movq printf@GOTPCREL(%rip), %r10 
	call *%r10
	
	addq $8, %rsp
	xorl %eax, %eax
	ret

.section .rodata
	.str0: .asciz "Hello, World!\n"
