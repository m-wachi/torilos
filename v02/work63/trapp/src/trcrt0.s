.code32
.text
	
.global _start

_start:
	movl	8(%esp), %eax
	movl	%eax, environ
	#pushl $0x47
	#pushl $0x46
	#pushl $0x45
	#pushl $0x44
	#pushl $0x43
	#pushl $msg
	#pushl $0x04
	#call syscall

	#pushl $0x48fff0
	#pushl $3
	call main
	movl	%eax, %ebx
	movl	$1, %eax
	int	$0x80	#exit system call
	
.data
.global environ, errno

environ:
	.int	0x00

errno:		#fancy errno
	.int	0x00
