.code32
.text

.global _start

_start:		# This program is loaded 0xc600

# 0-8MPage Table setup
# setup 2 page table (0-4M, 4-8M)
# linear address = phisical address
	cld
	movl	$0x37000, %edi
	movl	$0x07, %eax
pt_setup_lp:		
	stosl
	addl	$0x1000, %eax	
	cmpl	$0x39000, %edi
	jne	pt_setup_lp
	
# VESA VRAM Page Table setup (addr=0xe0000000)
# setup 1 page table. enabled only first 75 entries.
# linear address = phisical address
	movl	$0x39000, %edi
	movl	$0xe0000007, %eax
	movl	$75, %ecx
pt_vesa_lp:
	stosl
	addl	$0x1000, %eax
	decl	%ecx
	jne	pt_vesa_lp
	
	movl	$949, %ecx	# 1024 - 75
	xorl	%eax, %eax
	rep
	stosl	
	
# Page Directory Table setup
# make first 2 entries enable	
	movl	$0x36000, %edi
	movl	$0x37007, (%edi)
	movl	$0x36004, %edi
	movl	$0x38007, (%edi)
	
	movl	$0x0, %eax
	movl	$1022, %ecx
	movl	$0x36008, %edi
	rep
	stosl	
	
# Enable VESA PDE(0xe0000000)
	movl	$0x36e00, %edi
	movl	$0x39007, (%edi)

	
# setup cr3 then paging on.
	movl	$0x36000, %eax
	movl	%eax, %cr3
	
	movl	%cr0, %eax
	orl	$0x80000000, %eax
	movl	%eax, %cr0
	jmp	reset_addr
reset_addr:
	movw	$0x08, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss			

	movl	$0x00100000, %ebx
	movl	$0x00260000, %esp
	ljmpl	$16, $0x00100000 #OK! it works!

fin:	
	hlt
	jmp	fin
	