CFLAGS = -g -O2
CROSS_CFLAGS =
TARGET_CFLAGS = -O2 -DMISSING_SYSCALL_NAMES
NEWLIB_CFLAGS = -fno-builtin

#this setting by m-wachi
INCLUDES = -nostdinc -I../include -I../gccinclude -I../orginclude

OBJS= ctype_.o

default: all
all: libtrctype.a
	
.c.o: 
	gcc -c $(CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS) $(NEWLIB_CFLAGS) $(INCLUDES) $*.c

libtrctype.a: $(OBJS)
	ar -rc libtrctype.a $(OBJS)
	
clean:
	rm -f *.o libtrctype.a

