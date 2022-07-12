.code16
.text

.global _start

_start:
	.byte	0xeb, 0x4e
	.byte   0x90
	.ascii	"TORILIPL"
	.word	512
	.byte	1	
	.word	1
	.byte	2
	.word	224
	.word	2880
	.byte	0xf0
	.word	9
	.word	18
	.word	2
	.long	0
	.long	2880
	.byte	0, 0, 0x29
	.long	0xffffffff
	.ascii	"TORILOS    "
	.ascii	"FAT12   "
	.byte	0,0,0,0,0,0,0,0
	.byte	0,0,0,0,0,0,0,0
	.byte	0,0
