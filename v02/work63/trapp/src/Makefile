LIB_OBJS = ../syscl01.o ../myapputil01.o ../mylib01.o

LIB_DIR=lib

INCLUDES=-nostdinc -Iinclude -Igccinclude

CRT0 = ./trcrt0.o

srcdir= ../src

TARGET = myapp06.elf myapp07.elf myapp0701.elf myapp08.elf myapp0702.elf \
	 cat pwd trshell01 ls mkdir \
	 myapp05.elf myapp0502.elf myapp09.elf myapp10.elf myapp1002.elf \
	 myapp11.elf myapp12.elf myapp13.elf myapp1302.elf \
	 myapp14.elf myapp1402.elf myapp1403.elf \
	 myapp1501.elf myapp1502.elf myapp1503.elf myapp1601.elf \
	 myapp1701.elf myapp1801.elf

default	: all

all: $(TARGET)

include: ../../trlib/include
	ln -s ../../trlib/include .

gccinclude: ../../trlib/gccinclude
	ln -s ../../trlib/gccinclude

trinclude: ../../trlib/trutil/include
	ln -s ../../trlib/trutil/include trinclude

lib: ../../trlib/lib
	ln -s ../../trlib/lib .

%.o: $(srcdir)/%.c include gccinclude
	gcc $(INCLUDES) -Wall -c $<

%.o: $(srcdir)/%.s
	as -al=$*.lst -o $*.o $<

cat: cat.o $(CRT0) lib
	ld $(CRT0) cat.o -M -Ttext 0x401000 -S -o $@ -ltrc -ltrm -Bstatic -L$(LIB_DIR) > mapcat.txt

pwd: pwd.o $(CRT0) lib
	ld $(CRT0) pwd.o -M -Ttext 0x401000 -S -o $@ -ltrc -ltrm -Bstatic -L$(LIB_DIR) > mappwd.txt

ls: ls.o $(CRT0) lib
	ld $(CRT0)  ls.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapls.txt

mkdir: mkdir.o $(CRT0) lib
	ld $(CRT0)  mkdir.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmkdir.txt

#myapp0401.elf: myapp0401.o $(CRT0)
#	ld $(CRT0) myapp0401.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp0401.txt

myapp05.elf: myapp05.o $(CRT0) lib
	ld $(CRT0) myapp05.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp05.txt

myapp06.elf: myapp06.o $(CRT0) lib
	ld $(CRT0) myapp06.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp06.txt

myapp07.elf: myapp07.o $(CRT0) lib
	ld $(CRT0)  myapp07.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp07.txt

myapp0701.elf: myapp0701.o $(CRT0) lib 
	ld  $(CRT0) myapp0701.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR)  > mapmyapp0701.txt 

myapp0702.elf: myapp0702.o $(CRT0) lib
	ld  $(CRT0) myapp0702.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR)  > mapmyapp0702.txt 

myapp08.elf: myapp08.o $(CRT0) lib
	ld $(CRT0) myapp08.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp08.txt

myapp09.elf: myapp09.o $(CRT0) lib
	ld $(CRT0) myapp09.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp09.txt

myapp10.elf: myapp10.o $(CRT0) lib
	ld $(CRT0) myapp10.o -M -Ttext 0x401000 -S -o myapp10.elf -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp10.txt
	
myapp1002.elf: myapp1002.o $(CRT0) lib
	ld $(CRT0)  myapp1002.o -M -Ttext 0x401000 -S -o $@ -ltrc -ltrm -Bstatic -L$(LIB_DIR) > mapmyapp1002.txt

myapp11.elf: myapp11.o $(CRT0) lib
	ld $(CRT0)  myapp11.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp11.txt

myapp12.elf: myapp12.o $(CRT0) lib
	ld $(CRT0)  myapp12.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp12.txt

myapp13.elf: myapp13.o $(CRT0) lib
	ld $(CRT0)  myapp13.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp13.txt

myapp1302.elf: myapp1302.o $(CRT0) lib
	ld $(CRT0)  myapp1302.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp1302.txt

myapp14.elf: myapp14.o $(CRT0) lib
	ld $(CRT0)  myapp14.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp14.txt

myapp1402.elf: myapp1402.o $(CRT0) lib
	ld $(CRT0)  myapp1402.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp1402.txt

myapp1403.elf: myapp1403.o $(CRT0) lib
	ld $(CRT0)  myapp1403.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp1403.txt

myapp1501.elf: myapp1501.o $(CRT0)
	ld $(CRT0)  myapp1501.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp1501.txt

myapp1502.elf: myapp1502.o $(CRT0)
	ld $(CRT0)  myapp1502.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp1502.txt

myapp1503.elf: myapp1503.o $(CRT0)
	ld $(CRT0)  myapp1503.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp1503.txt

myapp1601.elf: myapp1601.o $(CRT0)
	ld $(CRT0)  myapp1601.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp1601.txt
	
myapp1701.elf: myapp1701.o $(CRT0)
	ld $(CRT0)  myapp1701.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > mapmyapp1701.txt
	
%.elf: %.o $(CRT0) lib
	ld $(CRT0) $*.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > map$*.txt

trshell01: trshell01.o $(CRT0) lib
	ld $(CRT0)  trshell01.o -M -Ttext 0x401000 -S -o $@ -ltrc -Bstatic -L$(LIB_DIR) > maptrshell01.txt

clean:
	rm -f $(TARGET) *.o *~ map*.txt *.lst cat include gccinclude lib

