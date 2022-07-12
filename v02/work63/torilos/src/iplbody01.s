CYLS = 10	

.code16
.text

.global _start

_start:
#	org 0x7c50

	movw	$0x0, %ax
	movw	%ax, %ss
	movw	$0x7c00, %sp
	movw	%ax, %dx
	movw	%ax, %ds

	movw	$msg, %si
	call	printstr
	movw	$greetmsg, %si
	call	printstr
#read fd
	movw	$0x820, %ax
	movw	%ax, %es
	movb	$0, %ch		# cylinder 0
	movb	$0, %dh		# head 0
	movb	$2, %cl
read_fd_loop:	
	movw	$0, %si		# retry counter
retry:
	movb	$0x02, %ah	# AH=0x02 - read disk
	movb	$1, %al		# 1 sector
	movw	$0, %bx
	movb	$0x00, %dl	# Drive A
	int	$0x13
	jnc	read_next
	addw	$1, %si
	cmpw	$5, %si
	jae	error
	movw	$0x00, %ax
	movb	$0x00, %dl	# Drive A
	int	$0x13		# reset drive
	jmp	retry
read_next:
	movw	%es, %ax
	addw	$0x0020, %ax	# load address +0x200(0x20=512/16)
	movw	%ax, %es
	addb	$1, %cl		# ++CL
	cmpb	$18, %cl
	jbe	read_fd_loop	# jump read_fd_loop if CL <= 18
	movb	$1, %cl
	addb	$1, %dh
	cmpb	$2, %dh
	jb	read_fd_loop	# jump read_fd_loop if DH < 2
	movb	$0, %dh
	addb	$1, %ch
	cmpb	$CYLS, %ch
	jb	read_fd_loop	# jump read_fd_loop if CH < CYLS
	
	movw	$readfd_success_msg, %si
	call	printstr
	jmp	0x0c200
error:	
	movw	$errormsg, %si
	call	printstr
fin:
	hlt
	jmp	fin
printstr:	
	nop	
putloop:
	movb	(%si), %al
	addw	$1, %si
	cmpb	$0, %al
	je	putloop_end
	movb	$0x0e, %ah
	movw	$0x15, %bx
	int	$0x10
	jmp	putloop
putloop_end:
	ret
msg:
	.byte	0x0a, 0x0a
	.ascii	"hello, world"
	.byte	0x0d, 0x0a
	.ascii	"you can use LABEL in GAS now!!"
	.byte	0x0d, 0x0a, 0x0
greetmsg:	
	.ascii	"you can use GAS procedure now!"
	.byte	0x0d, 0x0a, 0x0
readfd_success_msg:
	.ascii	"10 cylinders data has been loaded at 0x08200-0x34fff"
	.byte	0x0d, 0x0a, 0x0
errormsg:	
	.ascii	"error on reading floppy disk."
	.byte	0x0d, 0x0a, 0x0

