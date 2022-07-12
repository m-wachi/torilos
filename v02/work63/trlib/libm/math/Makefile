OBJS= s_infconst.o s_isinf.o s_isnan.o

TGT = libtrm.a

CFLAGS = -g -O2
CROSS_CFLAGS =
TARGET_CFLAGS = -O2 -DMISSING_SYSCALL_NAMES
NEWLIB_CFLAGS = -fno-builtin

#this setting by m-wachi
INCLUDES = -nostdinc -I../../include -I../../gccinclude -I../../orginclude

default: all
all: $(TGT)

.c.o:
	gcc -c $(CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS) $(NEWLIB_CFLAGS) $(INCLUDES) $*.c

libtrm.a: $(OBJS)
	ar -rc libtrm.a $(OBJS)
	
clean:
	rm -f *.o $(TGT)

