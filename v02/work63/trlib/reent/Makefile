OBJS= openr.o reent.o readr.o closer.o writer.o impure.o fstatr.o lseekr.o \
	sbrkr.o
	
default: all
all: libtrreent.a
	
%.o: %.c
	gcc -DMISSING_SYSCALL_NAMES -nostdinc -I../include -I../gccinclude -I../orginclude -c $*.c

libtrreent.a: $(OBJS)
	ar -rc libtrreent.a $(OBJS)
	
clean:
	rm -f *.o libtrreent.a

