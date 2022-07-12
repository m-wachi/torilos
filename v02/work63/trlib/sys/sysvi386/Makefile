OBJS=sbrk.o opendir.o readdir.o closedir.o rewinddir.o isatty.o

TGT = libsysvi386.a

default: all
all: $(TGT)

%.o: %.c
	gcc -nostdinc -I../../include -I../../gccinclude -I../../orginclude -c $*.c

libsysvi386.a: $(OBJS)
	ar -rc $@ $(OBJS)
	
clean:
	rm -f *.o $(TGT)

