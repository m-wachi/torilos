OBJS= malloc.o \
	mallocr.o mbtowc_r.o mlock.o freer.o dtoa.o mprec.o callocr.o atoi.o \
	strtol.o atof.o strtod.o

TGT = libtrstdlib.a

MALLOC_CFLAGS = -DINTERNAL_NEWLIB -DMISSING_SYSCALL_NAMES \
	-nostdinc -I../include -I../gccinclude -I../orginclude 

default: all
all: $(TGT)

mallocr.o: mallocr.c
	gcc -DDEFINE_MALLOC $(MALLOC_CFLAGS) -c mallocr.c -o $@

freer.o: mallocr.c
	gcc -DDEFINE_FREE $(MALLOC_CFLAGS) -c mallocr.c -o $@
	
callocr.o: mallocr.c
	$(CC) -DDEFINE_CALLOC $(MALLOC_CFLAGS) -c mallocr.c -o $@

%.o: %.c
	gcc -DMISSING_SYSCALL_NAMES -nostdinc -I../include -I../gccinclude -I../orginclude -c $*.c

libtrstdlib.a: $(OBJS)
	ar -rc libtrstdlib.a $(OBJS)
	
clean:
	rm -f *.o $(TGT)

