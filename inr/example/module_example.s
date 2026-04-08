main:
	movl %edi, %vreg0
	movl %vreg0, %vreg1
	addl $20, %vreg1
	movl %vreg1, %eax
	ret 
