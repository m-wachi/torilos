CRT0 = ../trcrt0.o

srcdir= ../../../src/tredit

TARGET = tredit_test

OBJS = filebuffer.o

TEST_OBJS = tredit_test.o

default	: all

all: $(TARGET)

%.o: $(srcdir)/%.c
	gcc -g3 -Wall -c $<

%.o: $(srcdir)/test/%.c
	gcc -g3 -Wall -c $<

$(TARGET): $(OBJS) $(TEST_OBJS)
	gcc -o $@ $(OBJS) $(TEST_OBJS)
	
test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o *~ map*.txt

