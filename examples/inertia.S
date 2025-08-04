.section .text
	.globl add
	.balign 16
add:
	movl $1, %eax
	ret
	.globl ret_gp
	.balign 16
ret_gp:
	movq %rdi, %rax
	ret
	movq %rsi, %rax
	ret
	movq %rdx, %rax
	ret
	movq %rcx, %rax
	ret
	movq %r8, %rax
	ret
	movq %r9, %rax
	ret
	movq 8(%rsp), %rax
	ret
	movq 16(%rsp), %rax
	ret
	.globl main
	.balign 16
main:
	xorl %eax, %eax
	ret
