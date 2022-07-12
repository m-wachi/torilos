TGT_OBJS = trext2fs.o trblkbuf.o mylib01.o

TESTSTAB_OBJS = debug_stdapp.o trlock_stdapp.o sprint_varval_stdapp.o trfile_old.o 

TRSRCDIR = ../src

TESTSTABDIR = ../teststab

default	:  analyze_ext2fs analyze_ext2fs2 mkfdimg_ext2

%.o: $(TRSRCDIR)/%.c
	gcc -c -g3 -I../include -o $*.o $(TRSRCDIR)/$*.c

%.o: $(TESTSTABDIR)/%.c
	gcc -c -g3 -I../include -o $*.o $(TESTSTABDIR)/$*.c

%.o: %.c
	gcc -c -g3 -I../include -o $*.o $*.c

%.o: %.s
	as -al=$*.lst -o $*.o $*.s 

analyze_ext2fs: analyze_ext2fs.o 
	gcc -o analyze_ext2fs analyze_ext2fs.o

analyze_ext2fs2: analyze_ext2fs2.o 
	gcc -o analyze_ext2fs2 analyze_ext2fs2.o

mkfdimg_ext2: mkfdimg_ext2.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o mkfdimg_ext2 mkfdimg_ext2.o $(TGT_OBJS) $(TESTSTAB_OBJS)

run: analyze_ext2fs
	./analyze_ext2fs floppy_ext2.img

#runmk: mkfdimg_ext2
#	./mkfdimg_ext2 test.img hello.txt hello2.txt mymain01.c myapp08.elf myapp07.elf
#	../zerofill2 test.img testfd.img 1474560

clean:
	rm -f *.o *~ analyze_ext2fs analyze_ext2fs2 mkfdimg_ext2 tst_trext2fs

