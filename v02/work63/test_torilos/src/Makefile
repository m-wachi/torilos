TGT_OBJS = trblkbuf.o mylib01.o trfilemdl.o trext2fs.o trfile.o trblkbufmdl.o \
	trext2fsmdl.o trutil.o trinode.o trext2fs_opr.o

TGT_OBJS2 =  trtty.o fifo01.o trkbd.o mylib01.o trutil.o \
	trinode.o trfile.o

BASE_TGT_OBJ = mylib01.o trutil.o

BASE_STAB_OBJ = debug_stdapp.o trlock_stdapp.o

TESTSTAB_OBJS = debug_stdapp.o trlock_stdapp.o trataiomdl_stdapp.o \
	mytask01_stdapp.o

TROSDIR = ../../torilos

TRSRCDIR = $(TROSDIR)/src

TESTSTABDIR = $(TROSDIR)/teststab

TRINC = $(TROSDIR)/include

MKDISK_APP = $(TROSDIR)/ext2fswork/mkfdimg_ext2

GETFILE_APP = $(TROSDIR)/ext2fswork2/ext2fsdisk_getfile

TGT_APP = test_trfile test_trblkbuf test_trfilemdl_read test_trfilemdl_write \
	tst_trext2fs test_trutil test_trext2fsmdl_namei \
	test_trfilemdl_getblk test_trtty test_mylib01 test_trmempage \
	test_trmempage_mdl test_trext2fs_opr_getdents test_trext2fs_opr_mkdir \
	test_trext2fs_opr_truncate test_trtask test_trtty_mdl
	
all: $(TGT_APP)

%.o: $(TRSRCDIR)/%.c
	gcc -c -g3 -Wall -nostdinc -I$(TRINC) $(TRSRCDIR)/$*.c

%.o: $(TESTSTABDIR)/%.c
	gcc -c -g3 -I$(TRINC) -I$(TESTSTABDIR)/include $(TESTSTABDIR)/$*.c

%.o: ../src/%.c
	gcc -c -g3 -Wall -I$(TRINC) -I$(TESTSTABDIR)/include ../src/$*.c


test_trfile: test_trfile.o trfile.o $(BASE_TGT_OBJ) $(BASE_STAB_OBJ)
	gcc -o $@ -g3 test_trfile.o trfile.o $(BASE_TGT_OBJ) $(BASE_STAB_OBJ)

tst_trext2fs: tst_trext2fs.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 tst_trext2fs.o $(TGT_OBJS) $(TESTSTAB_OBJS)

test_trblkbuf: test_trblkbuf.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 test_trblkbuf.o $(TGT_OBJS) $(TESTSTAB_OBJS)

test_trfilemdl_read: test_trfilemdl_read.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 test_trfilemdl_read.o $(TGT_OBJS) $(TESTSTAB_OBJS)

test_trfilemdl_write: test_trfilemdl_write.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 test_trfilemdl_write.o $(TGT_OBJS) $(TESTSTAB_OBJS)

test_trfilemdl_getblk: test_trfilemdl_getblk.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 test_trfilemdl_getblk.o $(TGT_OBJS) $(TESTSTAB_OBJS)

test_trext2fsmdl_namei: test_trext2fsmdl_namei.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 test_trext2fsmdl_namei.o $(TGT_OBJS) $(TESTSTAB_OBJS)

test_trext2fs_opr_getdents: test_trext2fs_opr_getdents.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 test_trext2fs_opr_getdents.o $(TGT_OBJS) $(TESTSTAB_OBJS)

test_trext2fs_opr_mkdir: test_trext2fs_opr_mkdir.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 test_trext2fs_opr_mkdir.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	
test_trext2fs_opr_truncate: test_trext2fs_opr_truncate.o $(TGT_OBJS) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 test_trext2fs_opr_truncate.o $(TGT_OBJS) $(TESTSTAB_OBJS)

test_trutil: test_trutil.o trutil.o mylib01.o
	gcc -o $@ -g3 test_trutil.o trutil.o mylib01.o

test_trtty: test_trtty.o $(TGT_OBJS2) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 test_trtty.o $(TGT_OBJS2) $(TESTSTAB_OBJS)

test_trtty_mdl: test_trtty_mdl.o trtty_mdl.o trtty_opr.o $(TGT_OBJS2) $(TESTSTAB_OBJS)
	gcc -o $@ -g3 test_trtty_mdl.o trtty_mdl.o trtty_opr.o $(TGT_OBJS2) $(TESTSTAB_OBJS)

test_mylib01: mylib01.o test_mylib01.o
	gcc -o $@ -g3 test_mylib01.o mylib01.o

test_trmempage: test_trmempage.o trmempage.o $(BASE_TGT_OBJ) $(BASE_STAB_OBJ)
	gcc -o $@ -g3 test_trmempage.o trmempage.o $(BASE_TGT_OBJ) $(BASE_STAB_OBJ)

test_trmempage_mdl: test_trmempage_mdl.o trmempage_mdl.o trmempage.o mymm01_stdapp.o trpmemmgr_mdl_stdapp.o $(BASE_TGT_OBJ) $(BASE_STAB_OBJ)
	gcc -o $@ -g3 test_trmempage_mdl.o trmempage_mdl.o trmempage.o mymm01_stdapp.o trpmemmgr_mdl_stdapp.o $(BASE_TGT_OBJ) $(BASE_STAB_OBJ)

test_trtask: test_trtask.o trtask.o $(BASE_TGT_OBJ) $(BASE_STAB_OBJ)
	gcc -o $@ -g3 test_trtask.o trtask.o $(BASE_TGT_OBJ) $(BASE_STAB_OBJ)
	

$(MKDISK_APP):
	cd $(TROSDIR)/ext2fswork; make
	
$(GETFILE_APP):
	cd $(TROSDIR)/ext2fswork2; make
	
testdata_read.data: ../testdata/testdata_read.data
	cp -p $< .

test_namei.img.org: ../testdata/test_namei.img.org
	cp -p $< .
	
testdata_write.data.org: ../testdata/testdata_write.data.org	
	cp -p $< .

#prepare test_read
test.img: testdata_read.data $(MKDISK_APP)
	$(MKDISK_APP) test.img $<

#test trfile
test_file: test_trfile
	./test_trfile

#test trblkbuf
test_blkbuf: test_trblkbuf
	./test_trblkbuf 

#test tflm_read
test_read: test_trfilemdl_read test.img
	cp test.img testwk.img
	./test_trfilemdl_read testwk.img testdata_read.data 1500

#test tflm_write
test_write: test_trfilemdl_write test.img testdata_write.data.org $(GETFILE_APP)
	cp test.img testwk.img
	cp testdata_write.data.org testdata_write.data
	./test_trfilemdl_write testwk.img testdata_write.data 1500
	rm testdata_write.data
	$(GETFILE_APP) testwk.img testdata_write.data
	diff testdata_write.data.org testdata_write.data
	
#test tflm_getblk_by_inode_index
test_getblk: test_trfilemdl_getblk test.img
	./test_trfilemdl_getblk test.img

#test trext2fs_opr_getdents
test_getdents: test_trext2fs_opr_getdents test_namei.img
	./test_trext2fs_opr_getdents

#test test_trext2fs_opr_mkdir
test_mkdir: test_trext2fs_opr_mkdir test_namei.img
	cp test_namei.img test_namei_wk.img
	./test_trext2fs_opr_mkdir test_namei_wk.img dirbytest
	
#test test_trext2fs_opr_truncate
test_truncate: test_trext2fs_opr_truncate test.img testdata_write.data.org $(GETFILE_APP)
	cp test.img testwk.img
	cp testdata_write.data.org testdata_write.data
	./test_trext2fs_opr_truncate testwk.img testdata_write.data 1024
	#rm testdata_write.data
	#$(GETFILE_APP) testwk.img testdata_write.data

#test trext2fs
test_ext2fs: tst_trext2fs
	./tst_trext2fs

#test trext2fs
test_util: test_trutil
	./test_trutil

#namei test data preparation
test_namei.img: test_namei.img.org
	cp test_namei.img.org test_namei.img
	
#test namei
test_namei: test_trext2fsmdl_namei test_namei.img
	./test_trext2fsmdl_namei

#test trtty
test_tty: test_trtty
	./test_trtty

#test trtty_mdl
test_tty_mdl: test_trtty_mdl
	./test_trtty_mdl

#test mempage
test_mempage: test_trmempage
	./test_trmempage

#test mempage_mdl
test_mempage_mdl: test_trmempage_mdl
	./test_trmempage_mdl

#test mempage_mdl
test_task: test_trtask
	./test_trtask

clean:
	rm -f *.o test.img testwk.img test_namei.img $(TGT_APP)
