OBJS= locale.o

default: all 
all: libtrlocale.a
	
%.o: %.c
	gcc -DMISSING_SYSCALL_NAMES -nostdinc -I../include -I../gccinclude -I../orginclude -c $*.c

libtrlocale.a: $(OBJS)
	ar -rc libtrlocale.a $(OBJS)
	
clean:
	rm -f *.o libtrlocale.a

