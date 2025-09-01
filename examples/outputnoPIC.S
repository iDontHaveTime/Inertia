	.text
	.globl main
	.p2align 4
	.type main,@function
main:
	.cfi_startproc
	xorl %eax, %eax
	retq
.Lfunc_main_end:
	.size main, .Lfunc_main_end-main
	.cfi_endproc

	.globl aligned
	.p2align 5
	.type aligned,@function
aligned:
	.cfi_startproc
	retq
.Lfunc_aligned_end:
	.size aligned, .Lfunc_aligned_end-aligned
	.cfi_endproc

	.p2align 4
	.type local,@function
local:
	.cfi_startproc
	movq $4294967295, %rax
	retq
.Lfunc_local_end:
	.size local, .Lfunc_local_end-local
	.cfi_endproc

