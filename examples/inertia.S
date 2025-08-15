	.file "inertia.inr"
.section .text
	.globl add
	.balign 16
	.type add, @function
add:
	                                        
	movq %rdi, %rax
	addq %rsi, %rax
	ret
.Ladd_end:
	.size add, .Ladd_end-add
	.globl ret_gp
	.balign 16
	.type ret_gp, @function
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
.Lret_gp_end:
	.size ret_gp, .Lret_gp_end-ret_gp
	.globl retnum
	.balign 16
	.type retnum, @function
retnum:
	movq (%rsp), %rax
	movq 0(%rsp), %rax
	movq $0xFFFFFFFFFFFFFFFF, %rax                     
	movl $11, %ecx
	addl $1, %ecx
	movq %rcx, %rax
	addl $10, %eax
	ret
.Lretnum_end:
	.size retnum, .Lretnum_end-retnum
	.globl main
	.balign 16
	.type main, @function
main:
	                                        
	xorl %eax, %eax
	ret
.Lmain_end:
	.size main, .Lmain_end-main
