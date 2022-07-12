.code32
.text

.global tlck_swap

tlck_swap:	#tlck_swap(int *p_lock)
	movl	$1, %eax
	movl	4(%esp), %ecx
	xchg	(%ecx), %eax
	ret
