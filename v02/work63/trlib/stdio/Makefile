CFLAGS = -g -O2
CROSS_CFLAGS =
TARGET_CFLAGS = -O2 -DMISSING_SYSCALL_NAMES
NEWLIB_CFLAGS = -fno-builtin

#this setting by m-wachi
INCLUDES = -nostdinc -I../include -I../gccinclude -I../orginclude


OBJS=fflush.o fvwrite.o makebuf.o siprintf.o stdio.o vfprintf.o wsetup.o \
	fwalk.o findfp.o vfiprintf.o fiprintf.o iprintf.o fopen.o fgetc.o fclose.o \
	rget.o refill.o fseek.o fwrite.o flags.o sprintf.o

default: all
all: libtrstdio.a
	
%.o: %.c
#	gcc -DMISSING_SYSCALL_NAMES -nostdinc -I../include -I../gccinclude -I../orginclude -c $*.c
	gcc -c $(CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS) $(NEWLIB_CFLAGS) $(INCLUDES) $*.c

vfiprintf.o: vfprintf.c
	cp vfprintf.c vfiprintf.c
	gcc -c $(CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS) $(NEWLIB_CFLAGS) $(INCLUDES) -DINTEGER_ONLY -c vfiprintf.c


	
libtrstdio.a: $(OBJS)
	ar -rc libtrstdio.a $(OBJS)
	
clean:
	rm -f *.o libtrstdio.a vfiprintf.c

