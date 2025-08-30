	.globl main
	.p2align 4
	.type main,@function
main:
	.cfi_startproc
	xorl %eax, %eax
	ret
.L_func_main_end:
	.size main, .L_func_main_end-main
	.cfi_endproc

	.globl aligned
	.p2align 5
	.type aligned,@function
aligned:
	.cfi_startproc
	ret
.L_func_aligned_end:
	.size aligned, .L_func_aligned_end-aligned
	.cfi_endproc

	.p2align 4
	.type local,@function
local:
	.cfi_startproc
	ret
.L_func_local_end:
	.size local, .L_func_local_end-local
	.cfi_endproc

