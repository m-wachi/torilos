
TARGET = zerofill2 mkiplbody01

default: all
all: $(TARGET) 

%.o: %.c
	gcc -c -o $*.o $*.c

zerofill2: zerofill2.o
	gcc -o zerofill2 zerofill2.o

mkiplbody01: mkiplbody01.o
	gcc -o mkiplbody01 mkiplbody01.o

clean:
	rm -fr $(TARGET) *.o
	