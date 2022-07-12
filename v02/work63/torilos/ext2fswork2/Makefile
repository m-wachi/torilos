TGT_OBJS = ../bin/trblkbuf.o ../bin/mylib01.o ../bin/trfilemdl.o \
	../bin/trext2fs.o ../bin/trfile.o ../bin/trblkbufmdl.o \
	../bin/trext2fsmdl.o ../bin/trutil.o ../bin/trinode.o \
	../bin/trext2fs_opr.o

BASE_TGT_OBJ = mylib01.o trutil.o

BASE_STAB_OBJ = debug_stdapp.o trlock_stdapp.o

TESTSTAB_OBJS = debug_stdapp.o trlock_stdapp.o trataiomdl_stdapp.o \
	mytask01_stdapp.o

TROSDIR = ../../torilos02

TESTSTABDIR = ../teststab

TRINC = ../include

TGT_APP = ext2fsdisk_getfile ext2fsdisk_analyzefile	ext2fsdisk_writefile 
	
all: $(TGT_APP)

%.o: $(TESTSTABDIR)/%.c
	gcc -c -g3 -I$(TRINC) -I$(TESTSTABDIR)/include $(TESTSTABDIR)/$*.c

%.o: %.c
	gcc -c -g3 -Wall -I$(TRINC) -I$(TESTSTABDIR)/include $*.c

ext2fsdisk_getfile: ext2fsdisk_getfile.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ ext2fsdisk_getfile.o $(TGT_OBJS) $(TESTSTAB_OBJS) 

ext2fsdisk_analyzefile: ext2fsdisk_analyzefile.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ ext2fsdisk_analyzefile.o $(TGT_OBJS) $(TESTSTAB_OBJS) 

ext2fsdisk_writefile: ext2fsdisk_writefile.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ ext2fsdisk_writefile.o $(TGT_OBJS) $(TESTSTAB_OBJS) 

clean:
	rm -f *.o $(TGT_APP)
