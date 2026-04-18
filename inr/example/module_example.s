.section .text
.globl main
.type main, @function
.p2align 4
main:
	subq $4, %rsp
	movl %edi, %eax
	addl $20, %eax
	movl %eax, (%rsp)
	addq $4, %rsp
	ret 
