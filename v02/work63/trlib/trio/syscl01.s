.code32
.text
	
.global syscall, systest

syscall:	# int syscall(eax, ebx, ecx, edx, esi, edi, ebp)
	pusha
	
	movl	60(%esp), %ebp
	movl	56(%esp), %edi
	movl	52(%esp), %esi
	movl	48(%esp), %edx
	movl	44(%esp), %ecx
	movl	40(%esp), %ebx
	movl	36(%esp), %eax

	int	$0x80
	
	movl	%eax, 28(%esp)	# save %eax value in the stack.
		
	popa
#	movl	$9, %eax
	ret	
	
systest:	# int systest(eax, ebx, ecx, edx, esi, edi, ebp)
	pusha
	
	movl	60(%esp), %ebp
	movl	56(%esp), %edi
	movl	52(%esp), %esi
	movl	48(%esp), %edx
	movl	44(%esp), %ecx
	movl	40(%esp), %ebx
	movl	36(%esp), %eax

	int	$0x80
	
	movl	%eax, 28(%esp)	
#	movl	$9, 28(%esp)	
#systest_lp:
#	hlt
#	jmp systest_lp
	popa
#	movl	$9, %eax
	ret	
	
	


	