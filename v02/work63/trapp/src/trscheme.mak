#
# $Author: m-wachi $
# $Rev: $
#
OBJS = trsheme.o objects.o tokenizer.o primitive_procedures.o \
		eval.o analyze.o env.o debug.o io.o \
		test.o test_prim_proc.o test_io.o test_tokenizer.o

CFLAGS = -Wall

TARGET = trscheme

srcdir= ../../../trscheme/src

LIB_DIR=../lib

INCLUDES=-I../include -I../gccinclude

CRT0 = ../trcrt0.o

#comment out if you compile for torilos
OPT = -DTORILOS -nostdinc $(INCLUDES)

default: all
all: $(TARGET)

%.o: $(srcdir)/%.c 
	gcc $(OPT)  -c $(CFLAGS) $<

trscheme: $(OBJS)
	#gcc -o $@ $(OBJS)
	ld $(CRT0)  $(OBJS) -M -Ttext 0x401000 -S -o $@ -ltrc -ltrm -Bstatic -L$(LIB_DIR) > maptrscheme.txt

clean:
	rm -f *.o $(TARGET)
