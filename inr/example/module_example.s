.section .text
.globl main
.type main, @function
.p2align 4
main:
	movl %edi, %eax
	addl $20, %eax
	ret 
