SMAP = 0x534d4150
E820MAP_BASE = 0x4000
E820MAP_OFFSET = 0x0010	
SP_START = 0xca00
.code16
.text

.global _start

_start:
#	org 0xc200	This program is loaded 0xc200
	
# initialize registers
	movw	$0, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	
# e820 memory check
	movw	$E820MAP_OFFSET, %di	#offset 0
	xorl	%ebx, %ebx
	pushw	$0x0000		#counter
memchk_next:		
	movw	$E820MAP_BASE, %ax	#base 0x40000
	movw	%ax, %es
	movl	$SMAP, %edx
	movl	$20, %ecx
	movl	$0x0000e820, %eax
	int	$0x15
	jc	memchk_ng
	cmpl	$SMAP, %eax
	jne	memchk_ng
	
	popw	%ax	#inc counter
	incw	%ax
	pushw	%ax
	movw	%di, %ax	#forward %di(+20)
	addw	$20, %ax
	movw	%ax, %di	
	cmpl	$0, %ebx	#other entry exist?
	jne	memchk_next
memchk_ok:
	movw	$0, %ax
	movw	%ax, %di
	popw	%ax
	movw	%ax, %es:(%di)	#memo the number of e820map entry.
	movw	$memchk_good_msg, %si
	call	printstr
	jmp	start_graphic
memchk_ng:
	movw	$memchk_ng_msg, %si
	call	printstr
	jmp	error_fin

	
start_graphic:	

	#jmp	vc_setup2

# set VIDEO
	movw	$0x9000, %ax	#check VESA exists
	movw	%ax, %es
	movw	$0, %di
	movw	$0x4f00, %ax
	int	$0x10
	cmpw	$0x004f, %ax
	jne	vc_setup_err
	
	movw	%es:(%di), %ax	#check VESA version
	cmpw	$0x0200, %ax	# required 2.0 or later
	jb	vc_setup_err
	
	movw	$0x4101, %bx	#VESA 640x480x8bit-color
	movw	$0x4f02, %ax
	int	$0x10
	jmp	st32bit
vc_setup_err:		
	movw	$vc_setup_err_msg, %si
	call	printstr
	jmp	error_fin
	
vc_setup2:	
	movb	$0x13, %al	#VGA Graphics 300x200x8bit-color
	movb	$0x00, %ah
	int	$0x10
	jmp	st32bit

	
#########################################	
# start the process to shift 32bit mode
#########################################
		
# mask PIC interuption
# right initialization do later
st32bit:	
	movb	$0xff, %al
	outb	%al, $0x21
	nop			# some of machines doesn't work if no 'nop'
	outb	%al, $0xa1
		
	cli			# mask CPU interuption.
	
# A20Gate ON
	call	waitkbdout
	movb	$0xd1, %al
	outb	%al, $0x64
	call	waitkbdout
	movb	$0x60, %al	# enable A20
	call	waitkbdout
	
#mixcode

	lgdt	GDTR0
	movl	%cr0, %eax
	andl	$0x7fffffff, %eax
	orl	$0x00000001, %eax
	movl	%eax, %cr0	# set CR0
	jmp	pipelineflush

pipelineflush:	
	movw	$0x08, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss			
	
	# copy mymain(128kb) from $0xc600 to 0x100000
	movl	$0x0ca00, %esi
	movl	$0x00100000, %edi
	movl	$0x8000, %ecx	# 0x8000 = 128*1024/4
	call	memcpy
	
	ljmpl	$16, $0x0000c600 #jump to setup32
	
error_fin:	#stop loop for error
	hlt
	jmp	error_fin
	
waitkbdout:	
	inb	$0x64, %al
	andb	$0x02, %al
	inb	$0x60, %al	# read to make buffer empty
	jnz	waitkbdout	# jump 'waitkdbout' if result of AND is NOT 0
	ret
	

memcpy:	#memory copy
	# %esi: source address
	# %edi: distination address
	# %ecx:	the number of copying byte / 4
	movl	(%esi), %eax
	addl	$4, %esi
	movl	%eax, (%edi)
	addl	$4, %edi
	subl	$1, %ecx
	jnz	memcpy
	ret
	
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
	
	
memchk_good_msg:
	.ascii	"e820 memory check OK."
	.byte	0x0d, 0x0a, 0x00
memchk_ng_msg:
	.ascii	"e820 memory check NG."
	.byte	0x0d, 0x0a, 0x00

vc_setup_err_msg:
	.ascii	"Sorry.. Your VIDEO-CARD is not VESA 2.0."
	.byte	0x0d, 0x0a, 0x00

	.align	16
GDT0:
	.skip	8		# null selector
	.word	0xffff, 0x0000, 0x9200, 0x00cf	# RW segment 32bit
	.word	0xffff, 0x0000, 0x9a00, 0x00cf	# Exec segment 32bit
	.word	0
	
GDTR0:
	.word	0x0017	# 8*3-1
	.long	GDT0

	.align	16
