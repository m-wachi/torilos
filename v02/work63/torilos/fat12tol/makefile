OBJS = analyze01.o fat12.o 

default	: 
	make bin

%.o: %.c
	gcc -c $*.c

%.o: %.s
	as -al=$*.lst -o $*.o $*.s 

analyze01: $(OBJS)
	gcc -o analyze01 $(OBJS)

mkfdimg01: fat12.o mkfdimg01.o
	gcc -o mkfdimg01 fat12.o mkfdimg01.o

run: analyze01
	./analyze01 work.img

run2: mkfdimg01
	./mkfdimg01 output.bin bootsect.bin analyze01.o torilos.sys fat12.o

bin :
	make -r analyze01
	make -r mkfdimg01

clean:
	rm -f *.o *~


