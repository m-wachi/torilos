LIB_OBJS = exit.o syscl01.o puts.o close.o open.o read.o write.o brk.o \
	dup.o getdents.o fcntl.o fork.o execve.o mkdir.o waitpid.o chdir.o \
	lseek.o fstat.o ioctl.o

default	: all  
all: libtrio01.a

%.o: %.c
	gcc -nostdinc -I../include -I../gccinclude -c -o $*.o $*.c

%.o: %.s
	as -al=$*.lst -o $*.o $*.s 

libtrio01.a : $(LIB_OBJS)
	ar -rc libtrio01.a $(LIB_OBJS)

clean:
	rm -f *.o *.lst map*.txt libtrio01.a

