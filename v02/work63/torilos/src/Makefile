MYMAIN_OBJS = mymain01.o graphic01.o myasmfunc01.o dsctbl01.o intr01.o \
	timer01.o fifo01.o fdc01.o pci01.o ataio01.o mymm01.o fat12.o \
	syshndlr01.o mytask01.o mypage01.o trtermmgr.o trutil.o trtty.o \
	fat12f01.o font.o myutil01.o mylib01.o trconsole01.o trconsolemdl.o \
	trataiomdl.o trblkbuf.o trblkbufmdl.o trfile.o trext2fs.o trfilemdl.o \
	trscreen.o trkbd.o troscmd.o trlock.o trext2fsmdl.o trtty_opr.o \
	debug01.o trinode.o trfile_opr.o trmempage.o trmempage_mdl.o \
	trelf.o trext2fs_opr.o trtask.o trtty_mdl.o trlock_asm.o \
	trpmemmgr_mdl.o trfpuctl_asm.o
 
ZEROFILL=../tools/zerofill2

SRC=../src

default	: all

%.o: $(SRC)/%.c
	gcc -c -Wall -nostdinc -I../include $(SRC)/$*.c

%.o: $(SRC)/%.s
	as -al=$*.lst -o $*.o $(SRC)/$*.s 

font.o: 
	cp ../fonttool/font.o .

header.bin: header01.o
	ld -Ttext 0x0 -s --oformat binary -o header.bin header01.o

iplbody01.bin: iplbody01.o
	ld -Ttext 0x7c50 -s --oformat binary -o iplbody01.bin iplbody01.o

iplbody.bin: iplbody01.bin
	../tools/mkiplbody01 iplbody01.bin iplbody.bin

bootsect.bin: header.bin iplbody.bin
	cat header.bin iplbody.bin > bootsect.bin

middle.bin: middle01.o
	ld -Ttext 0x00 -s --oformat binary -o middle.bin middle01.o

init01.bin: init01.o
	ld -Ttext 0xc200 -s --oformat binary -o init01.bin init01.o

init.bin: init01.bin
	$(ZEROFILL) init01.bin init.bin 1024

setup3201.bin: setup3201.o
	ld -Ttext 0xc600 -s --oformat binary -o setup3201.bin setup3201.o

setup32.bin: setup3201.bin
	$(ZEROFILL) setup3201.bin setup32.bin 1024

#mymain01.o: mymain01.c myasmfunc01.h graphic01.h
	#gcc -c -static -Wa,-R -o mymain01.o mymain01.c 
#	gcc -c -o mymain01.o mymain01.c

mymain01.bin: $(MYMAIN_OBJS)
	ld -M -Ttext 0x0100000 -s --oformat binary -e mymain -o mymain01.bin $(MYMAIN_OBJS) > map.txt

mymain.bin: mymain01.bin
	$(ZEROFILL) mymain01.bin mymain.bin 131072 #128*1024

torilos.sys: mymain.bin init.bin setup32.bin
	cat init.bin setup32.bin mymain.bin > torilos.sys

pre_myos.bin: bootsect.bin torilos.sys
	../fat12tol/mkfdimg01 pre_myos.bin bootsect.bin torilos.sys

myos.bin: pre_myos.bin
	$(ZEROFILL) pre_myos.bin myos.bin 1474560 


testmylib01: testmylib01.c mylib01.o mylib01.h
	gcc -o testmylib01 testmylib01.c mylib01.o

testconsole01: testconsole01.c trconsole01.o trconsole01.h
	gcc -o testconsole01 trconsole01.o mylib01.o testconsole01.c


testfd.img:
	cd ../mkdisk; make mkdisk

run: myos.bin testfd.img
	qemu -m 8 -fda myos.bin -hda testfd.img -boot a

all : myos.bin
	#make -r -k -C appwork
	#make -r myos.bin

clean:
	rm -f *.bin *.o *.elf *~ *.lst torilos.sys map.txt testfd.img

test: testmylib01
	./testmylib01

testcons: testconsole01
	./testconsole01
