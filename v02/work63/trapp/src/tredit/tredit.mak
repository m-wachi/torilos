LIB_OBJS = ../../syscl01.o ../../myapputil01.o ../../mylib01.o

LIB_DIR=../lib

INCLUDES=-nostdinc -I../include -I../gccinclude -I../trinclude

CRT0 = ../trcrt0.o

srcdir= ../../src/tredit

TARGET = tredit

TEST_TARGET = tredit_test

OBJS = tredit.o keyboard.o filebuffer.o menu.o

TEST_OBJS = tredit_test.o filebuffer.o

default	: all

all: $(TARGET)

%.o: $(srcdir)/%.c
	gcc $(INCLUDES) -Wall -c $<

$(TARGET): $(CRT0) $(OBJS)
	ld $(CRT0) $(OBJS) -M -Ttext 0x401000 -S -o $@ -ltrc -ltrm -Bstatic -L$(LIB_DIR) > maptredit.txt
	
$(TEST_TARGET): $(TEST_OBJS)
	gcc -o $@ $(TEST_OBJS)
	
test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) *.o *~ map*.txt

