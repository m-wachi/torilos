PROCESS0_PDT= 0x280000
#.arch i486	
.code32
.text
	
.global io_hlt, io_cli, io_sti, io_stihlt
.global io_in8, io_in16, io_in32
.global io_out8, io_out16, io_out32
.global io_load_eflags, io_store_eflags			
.global load_gdtr, load_idtr, retrieve_cr3, load_cr3, paging_on
.global asm_inthandler_unknown, 
.global asm_inthandler00, asm_inthandler01, asm_inthandler02
.global asm_inthandler03, asm_inthandler04, asm_inthandler05 
.global asm_inthandler06, asm_inthandler07, asm_inthandler08
.global asm_inthandler09, asm_inthandler0a, asm_inthandler0b
.global asm_inthandler0c, asm_inthandler0d 
.global asm_inthandler0e
.global asm_inthandler21, asm_inthandler20, asm_inthandler26, asm_inthandler80
.global load_tr, farjmp, asm_fork, loadcr3_switch_esp, asm_fork_start_child

asm_fork:	#int asm_fork(unsigned int stk_fork_bottom_addr)
	cli
	pushl	%ebp
	mov	%esp, %ebp

	# %esp = stk_fork_bottom_addr	
	movl	8(%ebp), %esp
	# *(stk_fork_bottom_addr-4) = %esp;
	movl	%ebp, %eax
	pushl	%eax

	# *(stk_fork_bottom_addr-8) = %cr3	
	movl	%cr3, %eax
	pushl	%eax

	movl	$PROCESS0_PDT, %eax
	movl	%eax, %cr3 

	call 	tsk_do_fork	#tsk_do_fork(esp, eflags)


	popl	%edx
	movl	%edx, %cr3	

asm_fork_start_child:
	leave

#loop_asm_fork:
#	hlt
#	jmp	loop_asm_fork

	sti
	ret

asm_execve:	#int asm_execve(char* path, char* argv[], char* envp[])
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	call	trelf_prep_execve
	addl	$12, %esp
	movl	%esp, %ecx
	movl	%cr3, %edx
	movl	%eax, %esp
	movl	$PROCESS0_PDT, %eax
	movl	%eax, %cr3
	pushl	%ecx
	pushl	%edx
	call	trelf_do_execve
	popl	%edx
	popl	%ecx
	movl	%ecx, %esp
	movl	%edx, %cr3
#loop_asm_execve:
#	hlt
#	jmp	loop_asm_execve
	iret

farjmp:		# void farjmp(int eip, int cs)
	ljmpl	*4(%esp)
	ret

asm_inthandler_unknown:
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler_unknown
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret

asm_inthandler00:
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler00
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret

asm_inthandler01:
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler01
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret

asm_inthandler02:
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler02
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret

asm_inthandler03:
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler03
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret

asm_inthandler04:
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler04
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret

asm_inthandler05:
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler05
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret

asm_inthandler06:	# invalid opcode
	pushw	%es
	pushw	%ds
	pusha
	
	# temporary code.
	# need restoring CR3 
	movl	$0x280000, %eax	# set kernel PDT
	movl	%eax, %cr3


	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler06
	popl	%eax
	popa
	popw	%ds
	popw	%es
	iret

asm_inthandler07:	# Device disable 
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler07
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler08:	# Double Fault
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler08
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler09:	# Co-processor Segment Overrun
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler08
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler0a:	# invalid TSS
#	movl	$4, %ecx
#	movl	4(%esp), %eax
#fin0a:
#	hlt
#	jmp fin0a
	
	pushw	%es
	pushw	%ds
	pusha
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler0a
	popl	%eax
	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler0b:	# segment not exist.
	pushw	%es
	pushw	%ds
	pusha
	
	# temporary code.
	# need restoring CR3 
	movl	$0x280000, %eax	# set kernel PDT
	movl	%eax, %cr3

	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler0b
	popl	%eax
	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler0c:	# stack fault
	pushw	%es
	pushw	%ds
	pusha
	
	# temporary code.
	# need restoring CR3 
	movl	$0x280000, %eax	# set kernel PDT
	movl	%eax, %cr3

	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler0c
	popl	%eax
	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler0d:	# general protection exception
#fin0d:
#	hlt
#	jmp fin0d

	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler0d
	popl	%eax
	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler0e:	# Page fault
#fin0e:
#	hlt
#	jmp fin0e
	
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler0e
	popl	%eax

	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler20:	
	pushw	%es
	pushw	%ds
	pusha
	
	#movl	%cr3, %eax	# load cr3 to eax
	#pushl	%eax		# save cr3
	#movl	$0x280000, %eax	# set kernel PDT
	#movl	%eax, %cr3
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler20
	
	addl	$4, %esp	# discard param(esp)
	
	#popl	%eax		# restore cr3
	#movl	%eax, %cr3

	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler21:
	pushw	%es
	pushw	%ds
	pusha
	
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler21
	
	addl	$4, %esp	# discard param(esp)
	
	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler26:
	pushw	%es
	pushw	%ds
	pusha
	movl	%esp, %eax
	pushl	%eax
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	call	inthandler26
	popl	%eax
	popa
	popw	%ds
	popw	%es
	iret
	
asm_inthandler80:
	# jump if systemcall is 'execve'
	cmpl	$11, %eax
	je		asm_execve
#fin80:	
#	hlt
#	jmp	fin80
	pushw	%es
	pushw	%ds
	pushal
	
	#pushl	%eax		# save orig eax
	#movl	%cr3, %eax	# load cr3 to eax
	#pushl	%eax		# save cr3
	#movl	$0x280000, %eax	# set kernel PDT
	#movl	%eax, %cr3
	#movl	4(%esp), %eax	# restore orig eax
	
	pushl	%ebp
	pushl	%edi
	pushl	%esi
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	pushl	%eax
	
	movw	%ss, %ax
	movw	%ax, %ds
	movw	%ax, %es
	
	sti
	call	inthandler80
	
	addl	$28, %esp	# discard inthandler params

	movl	%eax, 28(%esp)	# save return value to pushal eax
	
	popal
	popw	%ds
	popw	%es
	iret
	
retrieve_cr3:	# unsigned int retrieve_cr3(void)
	movl	%cr3, %eax 
	ret

load_cr3:	# void load_cr3(unsigned int addr)
	movl	4(%esp), %eax
	movl	%eax, %cr3
	ret
	
paging_on:	# void paging_on(void)
	movl	%cr0, %eax
	orl	$0x80000000, %eax
	movl	%eax, %cr0
	#jmp	paging_on_fin
#paging_on_fin:	
	ret

load_gdtr:	# void load_gdtr(int limit, int addr)
	movw	4(%esp), %ax
	movw	%ax, 6(%esp)
	lgdt	6(%esp)
	ret

load_idtr:	# void load_idtr(int limit, int addr)
	movw	4(%esp), %ax
	movw	%ax, 6(%esp)
	lidt	6(%esp)
	ret
	
load_tr:	# void load_tr(int tr)
	ltr	4(%esp)		# tr
	ret

io_hlt:		# void io_hlt()
	hlt
	ret
	
io_cli:		# void io_cli()
	cli
	ret
		
io_sti:		# void io_sti()
	sti
	ret
	
io_stihlt:	# void io_stihlt()
	sti
	hlt
	ret

io_in8:		# void io_in8(int port)
	movl	4(%esp), %edx	#port
	movl	$0, %eax
	inb	%dx, %al
	ret
	
io_in16:	# void io_in16(int port)
	movl	4(%esp), %edx	#port
	movl	$0, %eax
	inw	%dx, %ax
	ret

io_in32:	# void io_in32(int port)
	movl	4(%esp), %edx	#port
	movl	$0, %eax
	inl	%dx, %eax
	ret

io_out8:	# void io_out8(int port, int data)
	movl	4(%esp), %edx	#port
	#movb	8(%esp), %al	#data
	movl	8(%esp), %eax
	outb	%al, %dx
	ret

io_out16:	# void io_out16(int port, int data)
	movl	4(%esp), %edx	#port
	movl	8(%esp), %eax	#data
	outw	%ax, %dx
	ret

io_out32:	# void io_out32(int port, int data)
	movl	4(%esp), %edx	#port
	movl	8(%esp), %eax	#data
	outl	%eax, %dx
	ret

io_load_eflags:	#int io_load_eflags()
	pushf	# same as 'PUSHFD'(push %eflags)
	popl	%eax
	ret

io_store_eflags:	# void_store_eflags(int eflags)
	movl	4(%esp), %eax
	pushl	%eax
	popf	# same as 'POPFD' (push %eflags)
	ret

