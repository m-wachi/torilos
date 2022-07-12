LIB_OBJS = ../syscl01.o ../myapputil01.o ../mylib01.o

LIB_DIR = /home/m-wachi/src/workspace/torilos02-work20/appwork

default: all
all: libtrutil.a 

%.o: %.c
	gcc -I../include -c -o $*.o $*.c

%.o: %.s
	as -al=$*.lst -o $*.o $*.s 

libtrutil.a : myapputil01.o
	ar -r libtrutil.a myapputil01.o

clean:
	rm -f *.o *.lst map*.txt libtrutil.a

