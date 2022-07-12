#
# $Author: m-wachi $
# $Rev: 206 $
#
OBJS = trsheme.o objects.o tokenizer.o primitive_procedures.o \
		eval.o analyze.o env.o debug.o io.o \
		test.o test_prim_proc.o test_io.o test_tokenizer.o

CFLAGS = -g3 -Wall

TARGET = trscheme

srcdir= ../src

default: all
all: $(TARGET)

%.o: $(srcdir)/%.c 
	gcc $(OPT) -c $(CFLAGS) $<

trscheme: $(OBJS)
	gcc -o $@ $(OBJS)

clean:
	rm -f *.o autotest.scm $(TARGET)
