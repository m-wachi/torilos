APP_DIR = ../../trapp/bin
TOOL_DIR = ../../test_torilos/bin

WRITE_TOOL = ../ext2fswork2/ext2fsdisk_writefile

default: mkdisk

APPS = pwd cat trshell01 ls mkdir trscheme tredit \
	myapp05.elf myapp0502.elf

#testfd.img:
#	cp $(APP_DIR)/*.elf $(APP_DIR)/cat $(APP_DIR)/pwd .
#	./mkfdimg_ext2 test.img hello2.txt cat myapp07.elf pwd
#	../tools/zerofill2 test.img testfd.img 1474560
#	cp testfd.img ../bin

#mkdisk:
#	rm -f testfd.img
#	make testfd.img

$(WRITE_TOOL):
	make -C ../ext2fswork2

mkdisk: $(APPS) $(WRITE_TOOL)
	cp test2fd.img.org test.img
	$(WRITE_TOOL) test.img pwd
	$(WRITE_TOOL) test.img cat
	$(WRITE_TOOL) test.img trshell01
	$(WRITE_TOOL) test.img ls
	$(WRITE_TOOL) test.img mkdir
	$(WRITE_TOOL) test.img trscheme
	$(WRITE_TOOL) test.img tredit
	$(WRITE_TOOL) test.img myapp05.elf
	$(WRITE_TOOL) test.img myapp0502.elf
	mv test.img testfd.img
	cp testfd.img ../bin
	 

pwd: $(APP_DIR)/pwd
	cp -p $< .

cat: $(APP_DIR)/cat
	cp -p $< .
	
trshell01: $(APP_DIR)/trshell01
	cp -p $< .

ls: $(APP_DIR)/ls
	cp -p $< .
	
mkdir: $(APP_DIR)/mkdir
	cp -p $< .
	
trscheme: $(APP_DIR)/trscheme/trscheme
	cp -p $< .
	
tredit: $(APP_DIR)/tredit/tredit
	cp -p $< .
	
myapp05.elf: $(APP_DIR)/myapp05.elf
	cp -p $(APP_DIR)/myapp05.elf .
	
myapp0502.elf: $(APP_DIR)/myapp0502.elf
	cp -p $(APP_DIR)/myapp0502.elf .
	
mkdisk2:
	cp $(APP_DIR)/*.elf $(APP_DIR)/cat $(APP_DIR)/pwd $(APP_DIR)/trshell01 \
		$(APP_DIR)/ls $(APP_DIR)/mkdir \
		$(APP_DIR)/trscheme/bin/trscheme $(APP_DIR)/trscheme/bin/withdraw2.scm \
		.
	cp test2fd.img.org test.img
	$(TOOL_DIR)/ext2fsdisk_writefile test.img pwd
	$(TOOL_DIR)/ext2fsdisk_writefile test.img cat
	$(TOOL_DIR)/ext2fsdisk_writefile test.img trshell01
	$(TOOL_DIR)/ext2fsdisk_writefile test.img ls
	$(TOOL_DIR)/ext2fsdisk_writefile test.img mkdir
	$(TOOL_DIR)/ext2fsdisk_writefile test.img trscheme
	$(TOOL_DIR)/ext2fsdisk_writefile test.img withdraw2.scm
	#$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp0401.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp05.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp11.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp08.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp09.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp10.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp1002.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp12.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp07.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp06.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp13.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp1302.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp14.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp1402.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp1403.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp1501.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp1502.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp1503.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp1601.elf
	$(TOOL_DIR)/ext2fsdisk_writefile test.img myapp1701.elf
	mv test.img testfd.img
	cp testfd.img ../bin
	 
	
clean:
	rm -f *.elf *.o *~ map*.txt testfd.img $(APPS)

