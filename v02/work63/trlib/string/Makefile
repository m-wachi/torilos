OBJS=memcpy.o memset.o strcpy.o strcat.o memchr.o memmove.o strlen.o \
	strcmp.o strncpy.o

default: all
all: libtrstr.a

%.o: %.c
	gcc -I../include -c -o $*.o $*.c

libtrstr.a: $(OBJS)
	ar -r libtrstr.a $(OBJS)
	
clean:
	rm -f *.o libtrstr.a
	
