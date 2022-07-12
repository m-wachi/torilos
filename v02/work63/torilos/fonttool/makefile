
default	: font.o

font.o : font.s
	as -o font.o font.s

font.s : makefont_gas hankaku.txt
	./makefont_gas hankaku.txt font.s

makefont_gas : makefont_gas.c
	gcc -o makefont_gas makefont_gas.c

clean:
	rm -f *.o *~ font.s makefont_gas

 