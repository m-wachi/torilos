#include <torilos/mylib01.h>
#include <torilos/myasmfunc01.h>
#include <torilos/graphic01.h>
#include <torilos/dsctbl01.h>
#include <torilos/intr01.h>
#include <torilos/fifo01.h>
#include <torilos/fdc01.h>
#include <torilos/trconsole01.h>
#include <torilos/trconsolemdl.h>
#include <torilos/myutil01.h>
#include <torilos/ataio01.h>
#include <torilos/pci01.h>
#include <torilos/trtermmgr.h>
#include <torilos/debug01.h>
#include <torilos/timer01.h>
#include <torilos/fat12.h>
#include <torilos/mymm01.h>
#include <torilos/mypage01.h>
#include <torilos/fat12f01.h>
#include <torilos/mytask01.h>
#include <torilos/syshndlr01.h>
#include <torilos/debug01.h>
#include <torilos/elf.h>
#include <torilos/trataiomdl.h>
#include <torilos/ext2fs.h>
#include <torilos/trblkbuf.h>
#include <torilos/trext2fs.h>
#include <torilos/trfile.h>
#include <torilos/trfilemdl.h>
#include <torilos/trkbd.h>
#include <torilos/troscmd.h>
#include <torilos/trblkbufmdl.h>
#include <torilos/trext2fsmdl.h>
#include <torilos/trinode.h>
#include <torilos/trtty.h>
#include <torilos/trtty_mdl.h>
#include <torilos/trtty_opr.h>
#include <torilos/trmempage.h>
#include <torilos/trutil.h>
#include <torilos/trext2fs_opr.h>
#include <torilos/trlock.h>
#include <torilos/trpmemmgr_mdl.h>
int fdc_test01(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl);
int tty_switch(HND_SC* hnd_sc, int tty_no);
void test_locking();
void knlthr01();
void knlthr02();
void test_console(HND_SC* hnd_sc);

extern FIFO8 g_keyque;
extern FIFO8 g_floppyque;
extern unsigned long g_time_counter;

extern unsigned long g_ts_timeout;

static TCNS_CONSOLE* my_cnsl;

static int ml_tty0_switch;

static int ml_test_lock_flg;

void mymain() {

	unsigned char kbdchar;

	int data, keyque_out, i;
	int ctrl_status=0;
	int redisp_pos = 0;
	char buff[512], buff2[512], cmdbuf[512];
	unsigned int app05_load_addr, app06_load_addr;


	FDC_SENSEINT_RESULT fdc_sint_res;
	FDC_READDATA_RESULT fdc_rd_res;
	HND_SC hnd_sc;

	unsigned char fat12[9*512];
	FAT12_DIRENT fat12_dirent[16];
	unsigned short fat12_ext[6*512];

	CYL_HD_SEC chs;

	HND_LIPT hnd_lipt;

	HND_FAT12F_FILE hnd_f;

	TRTSK_QUEENTRY* tsk_ent0;
	/*
	TSK_TASK* task_b;
	TSK_TASK* task_app;
	TSK_TASK* task_app06;
	TSK_TASK* task_app05_1;
	*/
	
	unsigned int prev_sec = 0, cur_sec=0;

	/*
	TRMPG_VM_INF app05_vminf, app06_vminf;
	TRMPG_VM_INF* p_app05_1_vminf;
	*/
	
	Elf32_Ehdr* elfhdr;
	Elf32_Phdr* phdr;

	FAT12_DIRENT* app_a_dirent;
	FAT12_DIRENT* app_b_dirent;
	FAT12_DIRENT* app_c_dirent;

	//struct ext2_super_block ext2_sb;	// super block

	//TE2FS_HND_FS hnd_fs;		// FileSystem Handle (old)
	TE2FM_HND_FSMDL hnd_fsmdl;	// FileSystem Handle (wrapper version)
	
	TFL_HND_FILE* user_fdtbl[16];  // user fd table
	//unsigned char* rootdir_ent;		//root-directory-entry
	struct ext2_dir_entry_2* cwd_dirent;
	
	init_gdtidt();

	/* phisical memory management setup */
	//mm_setup(&pg_mngr);
	mm_setup();
	tpmmm_init();
	
	/* init paging module */
	trmpg_init();

	/* paging setup */
	pag_setup_paging();

	init_pic();

	io_sti();

	init_pit();

	init_screen(&hnd_sc);

	//vram = (char *)VRAM_ADDRESS;

	tsk_init();
	
	my_cnsl = trtrm_init(&hnd_sc);
	
	//tcns_init_console(&my_cnsl);
	tcnm_init(&hnd_sc, my_cnsl);
	
	lipt_init(&hnd_lipt);
	
	debug_init(&hnd_sc, my_cnsl);

	
	/* draw mouse-cursor */
	//draw_init_mcursor(mcursor, &hnd_sc);


	sysh_init_sysfifo();

	/* allow interrupt by IRQ0, 1, 6 */
	io_out8(PIC0_IMR, 0xb8);
	//io_out8(PIC0_IMR, 0xf8);
	//io_out8(PIC0_IMR, 0xfb);
	//io_out8(PIC0_IMR, 0xf9);
	io_out8(PIC1_IMR, 0xff);

	/* FDC motor on */
	fdc_motor_on();

	/* DMAC init */
	init_dmac_fd();

	//mm_print_pg_mngr(&pg_mngr);
	mm_print_pg_mngr();
	
	/* setup main task */
	tsk_ent0 = tsk_new_task(2*8, 1*8, 0, 0x280000, 0);
	tsk_add_to_running(tsk_ent0);
	
	puts_varval01(&hnd_sc, my_cnsl, "task0 selector", tsk_ent0->task.sel, 1);

	load_tr(tsk_ent0->task.sel);

	tsk_init_cur_task();

	tfl_init_userfd(user_fdtbl);

	trind_init();
	
	trtym_init();
	
	ml_tty0_switch = 0;
	//setup_app_pdt();

	//new_app_pdt();

	for (;;) {
		io_cli();

		cur_sec = g_time_counter / 100;

		//if (0 == g_time_counter % 100) {
		if (cur_sec != prev_sec) {
			
			boxfill8((unsigned char*)hnd_sc.vram_base, hnd_sc.width, COL8_DARK_SKYBLUE, 
					 450, 16*27, 450+8*15-1, 16*27+15);
			//print_varval12(&hnd_sc, "time(sec)=", g_time_counter/100, 
			//			   0, 450, 16*27);
			print_varval12(&hnd_sc, "time(sec)=", cur_sec, 
						   0, 450, 16*27);
			prev_sec = cur_sec;
			
			//tcnm_refresh(&hnd_sc, &my_cnsl);
			trtrm_refresh();
		}

		if (0 < sysh_quecount()) {
			char buff[80];
			char buff2[80];
			SYSH_REGS* p_regs;

			debug_puts("syscall handling..");
			p_regs = sysh_getregs();
			sprint_varval(buff, "eax", p_regs->eax, 1);
			debug_puts("eax set.");
			sprint_varval(buff2, ", ebx", p_regs->ebx, 1);
			my_strcat(buff, buff2);
			sprint_varval(buff2, ", ecx", p_regs->ecx, 1);
			my_strcat(buff, buff2);
			sprint_varval(buff2, ", edx", p_regs->edx, 1);
			my_strcat(buff, buff2);
			debug_puts(buff); 
			sprint_varval(buff, "esi", p_regs->esi, 1);
			sprint_varval(buff2, ", edi", p_regs->edi, 1);
			my_strcat(buff, buff2);
			sprint_varval(buff2, ", ebp", p_regs->ebp, 1);
			my_strcat(buff, buff2);
			debug_puts(buff); 

		}

		if (0 < fifo8_count(&g_keyque)) {

			keyque_out = g_keyque.out;
			data = fifo8_get(&g_keyque);
			
			if (ml_tty0_switch) {
				trtty_fifoput(0, data);
				tsk_wakeup_waitkbd();
			}
			
			io_sti();
			
			sprint_varval(buff2, "keydata=", data, 1);
			my_strcpy(buff, buff2);
			sprint_varval(buff2, ", keyque_out=", keyque_out, 1);
			my_strcat(buff, buff2);

			boxfill8((unsigned char*)hnd_sc.vram_base, hnd_sc.width, COL8_DARK_SKYBLUE, 
					 10, 16*27, 10+50*8-1, 16*27+15);

			puts_pos(&hnd_sc, buff, 10, 16*27);

			kbdchar = tkbd_kbdkey2char(data);

			hnd_lipt.y = 26;

			if (data == 0x48) { // up arrow
				if (!ml_tty0_switch) {
					redisp_pos--;
					redisp_console(&hnd_sc, my_cnsl, redisp_pos);
				}
			} 
			else if (data == 0x50) { // down arrow
				if (!ml_tty0_switch) {
					redisp_pos++;
					redisp_console(&hnd_sc, my_cnsl, redisp_pos);
				}
			} 
			else if (data == 0x3b) { // F1
				tty_switch(&hnd_sc, 0);
			}
			else if (kbdchar > 0 && !ml_tty0_switch) {
				redisp_pos=0;

				if (kbdchar == 0x0a) {
					my_strcpy(cmdbuf, (char*)hnd_lipt.line);
				} else {
					cmdbuf[0] = '\0';
				}
				
				lipt_putc(&hnd_sc, my_cnsl, &hnd_lipt, kbdchar);
				
				//troc_runoscmd(cmdbuf, rootdir_ent, user_fdtbl);
				//troc_runoscmd(cmdbuf, hnd_fsmdl.rootdir_blkcell->blk, user_fdtbl);
				troc_runoscmd(cmdbuf, user_fdtbl);
			} 

			
		} 
		else if (g_time_counter > 250 && ctrl_status == 0) {
			// Initialize ATA I/O module.
			tatam_init();
			ctrl_status++;
			
		}
		/*
		else if (g_time_counter > 250 && ctrl_status == 0) {

			// FD READ TEST 
			//fdc_test01(&hnd_sc, &my_cnsl);
			
			ctrl_status++;
			
		}
		*/
		else if (g_time_counter > 400 && ctrl_status==1) {
			
			io_sti();

			tflm_prepare_filesystem(&hnd_fsmdl);
			te2fo_init(&hnd_fsmdl);
			troc_init(hnd_fsmdl.rootdir_blkcell->blk);
			
			tsk_set_switch_interval(20);
			
			ctrl_status++;
		}
		else if (g_time_counter > 600 && ctrl_status==2) {
			
			//troc_runoscmd("myapp07.elf", hnd_fsmdl.rootdir_blkcell->blk, user_fdtbl);
			//troc_runoscmd("myapp12.elf", hnd_fsmdl.rootdir_blkcell->blk, user_fdtbl);
			//troc_runoscmd("cat hello2.txt", user_fdtbl);
			//troc_runoscmd("myapp08.elf", user_fdtbl);
			test_console(&hnd_sc);
			io_sti();
			debug_puts("return swapper process.");
			
			ctrl_status++;
			//ctrl_status = 100;
		}
		else if (g_time_counter > 800 && ctrl_status==3) {
			tsk_set_switch_interval(5);
			//troc_runoscmd("myapp0502.elf test.out first_writing.", user_fdtbl);
			troc_runoscmd("tredit", user_fdtbl);
			//troc_runoscmd("trshell01", user_fdtbl);
			//test_locking();
			io_sti();
			debug_puts("return swapper process2.");
			
			//ctrl_status++;
			ctrl_status=100;
		}
		/*
		else if (g_time_counter > 800 && ctrl_status==3) {
			int fd;
			unsigned sz_read;
			unsigned char buff[1024];
			unsigned char* p_buff;
			
			io_sti();

			//fd = tflm_open2("hello2.txt", rootdir_ent, user_fdtbl);
			fd = tflm_open3("hello2.txt", user_fdtbl);
			
			puts_varval01(&hnd_sc, &my_cnsl, 
					"fd", fd, 0);

			puts_varval01(&hnd_sc, &my_cnsl, "file size", 
					user_fdtbl[fd]->ext2_inode->i_size, 1);
			
			my_memset(buff, 0, sizeof(buff));

			p_buff = buff;
			
			sz_read = tflm_read(fd, p_buff, 3, user_fdtbl);
			p_buff += 3;
			
			puts_varval01(&hnd_sc, &my_cnsl, 
					"sz_read", sz_read, 0);
			
			my_puts(&hnd_sc, &my_cnsl, "print hello2.txt. (2)");
			
			put_multi(&hnd_sc, &my_cnsl, buff);

			sz_read = tflm_read(fd, p_buff, 4, user_fdtbl);
			p_buff += 4;

			puts_varval01(&hnd_sc, &my_cnsl, "sz_read", sz_read, 0);
			
			my_puts(&hnd_sc, &my_cnsl, "print hello2.txt. (3)");
			put_multi(&hnd_sc, &my_cnsl, buff);

			sz_read = tflm_read(fd, p_buff, 100, user_fdtbl);
			p_buff += sz_read;

			puts_varval01(&hnd_sc, &my_cnsl, "sz_read", sz_read, 0);
			
			my_puts(&hnd_sc, &my_cnsl, "print hello2.txt. (4)");
			put_multi(&hnd_sc, &my_cnsl, buff);

			sz_read = tflm_read(fd, p_buff, 100, user_fdtbl);
			p_buff += sz_read;

			puts_varval01(&hnd_sc, &my_cnsl, "sz_read", sz_read, 0);
			
			my_puts(&hnd_sc, &my_cnsl, "print hello2.txt. (5)");
			put_multi(&hnd_sc, &my_cnsl, buff);

			
			tflm_close(fd, user_fdtbl);			

			//ctrl_status++;
			ctrl_status = 6;
			//ctrl_status = 100;
		}
		*/
		else if (g_time_counter > 1000 && ctrl_status==4) {
			//troc_runoscmd("trshell01", user_fdtbl);
			troc_runoscmd("ls", user_fdtbl);
			io_sti();
			
			ctrl_status = 100;
		}		
		/*
		else if (g_time_counter > 1300 && ctrl_status==4) {
			int fd;
			unsigned sz_read;
			unsigned char buff[1024];
			unsigned char* p_buff;
			
			io_sti();

			my_memset(buff, 0, sizeof(buff));

			p_buff = buff;

			fd = tflm_open2("mymain01.c", rootdir_ent, user_fdtbl);

			puts_varval01(&hnd_sc, &my_cnsl, 
					"fd", fd, 0);

			sz_read = tflm_read(fd, p_buff, 1008, user_fdtbl);
			puts_varval01(&hnd_sc, &my_cnsl, "sz_read", sz_read, 1);

			my_memset(buff, 0, sizeof(buff));
			p_buff = buff;
			
			sz_read = tflm_read(fd, p_buff, 8, user_fdtbl);
			p_buff += 8;
			
			my_puts(&hnd_sc, &my_cnsl, "print mymain01.c (1008 - 1016)");
			put_multi(&hnd_sc, &my_cnsl, buff);

			sz_read = tflm_read(fd, p_buff, 32, user_fdtbl);
			p_buff += 32;
			
			my_puts(&hnd_sc, &my_cnsl, "print mymain01.c (1008 - 1048)");
			put_multi(&hnd_sc, &my_cnsl, buff);

			my_memset(buff, 0, sizeof(buff));
			p_buff = buff;

			sz_read = tflm_read(fd, p_buff, 968, user_fdtbl);
			puts_varval01(&hnd_sc, &my_cnsl, "sz_read", sz_read, 1);

			my_memset(buff, 0, sizeof(buff));
			p_buff = buff;
			
			sz_read = tflm_read(fd, p_buff, 8, user_fdtbl);
			puts_varval01(&hnd_sc, &my_cnsl, "sz_read", sz_read, 1);
			p_buff += 8;

			my_puts(&hnd_sc, &my_cnsl, "print mymain01.c (2016 - 2024)");
			put_multi(&hnd_sc, &my_cnsl, buff);

			sz_read = tflm_read(fd, p_buff, 24, user_fdtbl);
			puts_varval01(&hnd_sc, &my_cnsl, "sz_read", sz_read, 1);
			p_buff += 24;

			my_puts(&hnd_sc, &my_cnsl, "print mymain01.c (2016 - 2048)");
			put_multi(&hnd_sc, &my_cnsl, buff);
			
			sz_read = tflm_read(fd, p_buff, 16, user_fdtbl);
			puts_varval01(&hnd_sc, &my_cnsl, "sz_read", sz_read, 1);
			p_buff += 16;

			my_puts(&hnd_sc, &my_cnsl, "print mymain01.c (2016 - 2064)");
			put_multi(&hnd_sc, &my_cnsl, buff);
			
			tfl_close(fd, user_fdtbl);			

			ctrl_status=6;
		}
		*/
		/*
		else if (g_time_counter > 1600 && ctrl_status==5) {
			int rc;
			unsigned long lba;
			unsigned char buff[1024];
			
			io_sti();
			my_memset(buff, 0x88, sizeof(buff));
			
			my_strcpy(buff, "HeyHeyHey HoHoHo");
			
			//write 0x5000 -> lba 0x28?
			lba = 0x28;
			
			rc = tatam_writeblock(buff, lba);

			puts_varval01(&hnd_sc, &my_cnsl, 
						  "tatam_writeblock(\"HeyHeyHey HoHoHo\", 0x28); rc", rc, 0);

			ctrl_status++;
			
		}
		*/
		else if (g_time_counter > 1400 && ctrl_status==6) {
			io_sti();

			//fancy_ls(rootdir_ent);
			fancy_ll(hnd_fsmdl.rootdir_blkcell->blk);
			
			ctrl_status++;
			//ctrl_status = 100;
		}
		else if (g_time_counter > 1600 && ctrl_status==7) {
			int fd;
			char buff[1024];

			io_sti();

			my_puts(&hnd_sc, my_cnsl, "create touched1.txt");
				
			//fd = tflm_create("touched1.txt", rootdir_ent, user_fdtbl);
			//fd = tflm_create2("touched1.txt", user_fdtbl);
			fd = te2fo_create("/touched1.txt", user_fdtbl);

			puts_varval01(&hnd_sc, my_cnsl, "fd", fd, 0);

			if (fd > 0) {
				my_strcpy(buff, "This text has been written by the program.\nHey0\n");

				tflm_write(fd, buff, 1024, user_fdtbl);
				
				tflm_close(fd, user_fdtbl);			
				
			} else {
				my_puts(&hnd_sc, my_cnsl, "Error - create touched.txt");
			}

			//fancy_ls(rootdir_ent);
			fancy_ll(hnd_fsmdl.rootdir_blkcell->blk);
			
			ctrl_status++;
			//ctrl_status=9;
		}
		else if (g_time_counter > 1800 && ctrl_status==8) {
			int fd;
			char buff[1024];

			io_sti();

			my_puts(&hnd_sc, my_cnsl, "create touched2.txt");
				
			//fd = tflm_create("touched2.txt", rootdir_ent, user_fdtbl);
			//fd = tflm_create2("touched2.txt", user_fdtbl);
			fd = te2fo_create("/touched2.txt", user_fdtbl);
			
			puts_varval01(&hnd_sc, my_cnsl, "fd", fd, 0);

			if (fd > 0) {
				my_strcpy(buff, "This is touched2.txt.\n");

				tflm_write(fd, buff, 1024, user_fdtbl);
				
				tflm_close(fd, user_fdtbl);			
				
			} else {
				my_puts(&hnd_sc, my_cnsl, "Error - create touched2.txt");
			}

			//fancy_ls(rootdir_ent);
			fancy_ls(hnd_fsmdl.rootdir_blkcell->blk);
			
			ctrl_status++;
		}
		else if (g_time_counter > 2000 && ctrl_status==9) {
			int fd;
			fd = trtyo_open("/dev/tty0", 0, user_fdtbl);
			if (fd >= 0) {
				my_strcpy(buff, "open /dev/tty0.\n");

				trtty_fifoput(0, 0x1e);	// press 'a'
				trtty_fifoput(0, 0x9e);	// release 'a'
				trtty_fifoput(0, 0x30);	// press 'b'

				my_memset(buff, 0, sizeof(buff));
				
				trtyo_read(fd, buff, 1, user_fdtbl);

				puts_varval01(&hnd_sc, my_cnsl, "buff[0]", buff[0], 1);
				
				trtyo_close(fd, user_fdtbl);			
				
			} else {
				my_puts(&hnd_sc, my_cnsl, "Error - open /dev/tty0");
			}

			ctrl_status=100;
		}		
//		else if (g_time_counter > 1800 && ctrl_status==8) {
//			int rc;
//			
//			rc = tflm_unlink("touched1.txt", rootdir_ent);
//
//			puts_varval01(&hnd_sc, &my_cnsl, "unlink touched1.txt, rc", rc, 0);
//			
//			ctrl_status++;
//		}
//		else if (g_time_counter > 2000 && ctrl_status==9) {
//			
//			io_sti();
//			
//			my_puts(&hnd_sc, &my_cnsl, "sync..");
//			tflm_sync();
//			
//			ctrl_status++;
//			//ctrl_status=100;
//		}
		else if (g_time_counter > 2200 && ctrl_status==10) {
			int rc;
			
			io_sti();

			tsk_set_switch_interval(20);
			
			//rc = run_app("myapp06.elf", rootdir_ent, user_fdtbl,
			//		0, 0);
			
			//rc = troc_parse_run("myapp06.elf aaa.txt bbb.txt", 
			//		rootdir_ent, user_fdtbl);
			rc = troc_parse_run("myapp06.elf aaa.txt bbb.txt", 
					hnd_fsmdl.rootdir_blkcell->blk, user_fdtbl, "/");

			puts_varval01(&hnd_sc, my_cnsl, "rc", rc, 0);
			
			ctrl_status++;
		}
		else if (g_time_counter > 2400 && ctrl_status==11) {
			int rc;
			
			io_sti();

			//rc = troc_parse_run("cat hello2.txt", 
			//		rootdir_ent, user_fdtbl);
			rc = troc_parse_run("cat hello2.txt", 
					hnd_fsmdl.rootdir_blkcell->blk, user_fdtbl, "/");
			
			puts_varval01(&hnd_sc, my_cnsl, "rc", rc, 0);
			
			//ctrl_status++;
			ctrl_status=100;

		}
		else if (g_time_counter > 2000 && ctrl_status==21) {
			unsigned char *p_fat_entry;
			unsigned char *p_char;

			my_puts(&hnd_sc, my_cnsl, "start FAT reading");

			/* FDC motor on again */
			fdc_motor_on();
			
			/* seek cylinder 0 */
			fdc_send_seek(0);
			fancy_wait_fdcint();

			/* sense interrupt */
			fdc_sense_int(&fdc_sint_res);

			puts_varval01(&hnd_sc, my_cnsl, "fd-sr0=", fdc_sint_res.sr0, 1);

			puts_varval01(&hnd_sc, my_cnsl, "fd-cyl=", fdc_sint_res.cyl_no, 1);

			/* prepare DMAC for read */
			prepare_dmac_read_fd();

			my_puts(&hnd_sc, my_cnsl, "start READ DATA");


			/* send READ DATA and receive result */
			fdc_readtrack(&fdc_rd_res, 0, 0);

			my_puts(&hnd_sc, my_cnsl, "end FAT reading");
			
			p_fat_entry = (unsigned char*) 512;

			p_char = p_fat_entry;
			
			/* copy fat12 (1st) */
			for (i=0; i<9*512; i++) {
				fat12[i] = *p_char;
				p_char++;
			}

			fat12_extr_fat12(fat12_ext, fat12);

			ctrl_status++;
		}
		else if (g_time_counter > 2200 && ctrl_status==22) {

			unsigned char *p_char;

			my_puts(&hnd_sc, my_cnsl, "start FAT12 directory entry reading");

			my_puts(&hnd_sc, my_cnsl, "start READ DATA");

			/* send READ DATA and receive result */
			fdc_readtrack(&fdc_rd_res, 0, 1);

			my_puts(&hnd_sc, my_cnsl, "end reading");
			
			p_char = (unsigned char*) 512;
			
			my_memcpy(&fat12_dirent, p_char, 512);

			for (i=0; i<16; i++) {
				if (0x20 == fat12_dirent[i].type || 
					0x10 == fat12_dirent[i].type) {
					
					my_itoa(buff, i);
					my_strcat(buff, ": entry=");
					fat12_getfilename(&fat12_dirent[i], buff2);
					my_strcat(buff, buff2);
					sprint_varval(buff2, ", type=", fat12_dirent[i].type, 1);
					my_strcat(buff, buff2);
					sprint_varval(buff2, ", size=", fat12_dirent[i].size, 0);
					my_strcat(buff, buff2);
					sprint_varval(buff2, ", clstr_no=", 
								  fat12_dirent[i].clstr_no, 0);
					my_strcat(buff, buff2);
					
					my_puts(&hnd_sc, my_cnsl, buff);
				}
			}

			clstr2chs(&chs, fat12_dirent[2].clstr_no);

			sprint_varval(buff, "cyl=", chs.cyl, 0);
			sprint_varval(buff2, ", hd=", chs.hd, 0);
			my_strcat(buff, buff2);
			sprint_varval(buff2, ", sec=", chs.sec, 0);
			my_strcat(buff, buff2);
			my_puts(&hnd_sc, my_cnsl, buff);

			ctrl_status++;
		}
		else if (g_time_counter > 2400 && ctrl_status==23) {

			unsigned char *p_char;

			my_puts(&hnd_sc, my_cnsl, "start file reading.. entry 2.");

			/* seek cylinder */
			fdc_send_seek(chs.cyl);
			fancy_wait_fdcint();

			/* sense interrupt */
			fdc_sense_int(&fdc_sint_res);

			puts_varval01(&hnd_sc, my_cnsl, "fd-sr0=", fdc_sint_res.sr0, 1);

			puts_varval01(&hnd_sc, my_cnsl, "fd-cyl=", fdc_sint_res.cyl_no, 1);
			my_puts(&hnd_sc, my_cnsl, "start READ DATA");

			/* send READ DATA and receive result */
			fdc_readtrack(&fdc_rd_res, chs.cyl, chs.hd);
			//fdc_readtrack2(&fdc_rd_res, chs.cyl, chs.hd, 10);

			my_puts(&hnd_sc, my_cnsl, "end reading");
			
			/* FDC motor off */
			fdc_motor_off();
			ctrl_status++;

		}
		else if (g_time_counter > 2600 && ctrl_status==24) {
			unsigned int dataaddr, srcaddr = 0;
			unsigned char *databuf;
			int datacnt, size;
			
			size = fat12_dirent[2].size;

			//dataaddr = mm_malloc_pg(size+1);
			dataaddr = mm_malloc_pg_knl(size+1);

			databuf = (void*) dataaddr;
			srcaddr += (chs.sec-1) * 512;

			my_memcpy(databuf, (void*)srcaddr, size);
			databuf[size] = '\0';
			
			puts_varval01(&hnd_sc, my_cnsl, "addr", dataaddr, 1);

			mm_print_pg_mngr();

			put_multi(&hnd_sc, my_cnsl, (char*)databuf);

			//mm_free(&pg_mngr, dataaddr);
			mm_free(dataaddr);

			//ctrl_status++;
			ctrl_status=31;

		}
		/*
		else if (g_time_counter > 3000 && ctrl_status==30) {

			tsk_debug_running();


			mm_print_pg_mngr();

			task_b = tsk_new_task_knl_default((int)&task_b_main, 0x280000);

			my_puts(&hnd_sc, &my_cnsl, "after new task..");
			tsk_debug_running();
			
			puts_varval01(&hnd_sc, &my_cnsl, "task_b selector", 
						  task_b->sel, 1);

			puts_varval01(&hnd_sc, &my_cnsl, "tss_b.eip", task_b->tss.eip, 1);
			puts_varval01(&hnd_sc, &my_cnsl, "tss_b.esp", task_b->tss.esp, 1);
			puts_varval01(&hnd_sc, &my_cnsl, "tss_b.ss", task_b->tss.ss, 1);


			debug_puts("about to switch task c.");

			tsk_set_cur_task(task_b);
			puts_varval01(&hnd_sc, &my_cnsl, "task_b->sel", task_b->sel, 1);
			farjmp(0, task_b->sel);
			
			my_puts(&hnd_sc, &my_cnsl, "after switch back..");
			tsk_debug_running();

			//ctrl_status++;
			ctrl_status=31;
		}
		*/
		else if (g_time_counter > 3800 && ctrl_status==31) {

			// FDC motor on again 
			fdc_motor_on();

			app_a_dirent = &fat12_dirent[10];

			fat12f_open_file(&hnd_f, app_a_dirent, fat12_ext);

			print_hnd_fat12f_file(&hnd_f);

			// FDC motor off
			fdc_motor_off();

			io_sti();

			//put_multi(&hnd_sc, &my_cnsl, hnd_f.buff);

			puts_varval01(&hnd_sc, my_cnsl, "load addr", (int)hnd_f.buff, 1);
			ctrl_status++;
		}
		else if (g_time_counter > 4000 && ctrl_status==32) {
			unsigned int hd_addr, addr;
			int pg_size;
			TRMPG_VM_INF* vminf;


			//pg_size = fat12_dirent[10].size;
			pg_size = app_a_dirent->size;

			app05_load_addr = hd_addr = mm_malloc_pg_app(pg_size);

			my_memcpy((unsigned char*)hd_addr,
					  hnd_f.buff, fat12_dirent[10].size);

			//hd_addr = (unsigned int)hnd_f.buff;
			
			puts_varval01(&hnd_sc, my_cnsl, "myapp08 load addr", 
						  app05_load_addr, 1);

			fat12f_freefile(&hnd_f);

			elfhdr = (Elf32_Ehdr*)hd_addr;

			sprint_varval(buff, "entry", elfhdr->e_entry, 1);
			sprint_varval(buff2, ", phoff", elfhdr->e_phoff, 1);
			my_strcat(buff, buff2);
			my_puts(&hnd_sc, my_cnsl, buff);

			addr = hd_addr + elfhdr->e_phoff;
			for (i=0; i<elfhdr->e_phnum; i++) {
				phdr = (Elf32_Phdr*) addr;
				sprint_varval(buff, "p_offset", phdr->p_offset, 1);
				sprint_varval(buff2, ", vaddr", phdr->p_vaddr, 1);
				my_strcat(buff, buff2);
				sprint_varval(buff2, ", filesz", phdr->p_filesz, 1);
				my_strcat(buff, buff2);
				my_puts(&hnd_sc, my_cnsl, buff);
				addr += sizeof(Elf32_Phdr);

			}
			
			
			//tsk_debug_running();

			ctrl_status++;
		}
		else if (g_time_counter > 4200 && ctrl_status==33) {

			//unsigned char *appbuf;

			//
			// read file 
			//

			// FDC motor on again 
			fdc_motor_on();

			app_b_dirent = &fat12_dirent[11];

			fat12f_open_file(&hnd_f, app_b_dirent, fat12_ext);

			print_hnd_fat12f_file(&hnd_f);

			// FDC motor off
			fdc_motor_off();

			puts_varval01(&hnd_sc, my_cnsl, "load addr", (int)hnd_f.buff, 1);

			//1
			// copy file data to application area
			//
			app06_load_addr = mm_malloc_pg_app(app_b_dirent->size);

			my_memcpy((void*)app06_load_addr, hnd_f.buff, app_b_dirent->size);
					  
			puts_varval01(&hnd_sc, my_cnsl, "myapp09 load addr", 
						  app06_load_addr, 1);

			fat12f_freefile(&hnd_f);

			ctrl_status++;
		}
		/*
		else if (g_time_counter > 4400 && ctrl_status==34) {

			//
			// create new task for myapp08
			//
			task_app = tsk_new_task_appelf(app05_load_addr,
										   app_a_dirent->size, 
										   16, 2);

			puts_varval01(&hnd_sc, &my_cnsl, "task_app selector", 
						  task_app->sel, 1);
			puts_varval01(&hnd_sc, &my_cnsl, "tss_app01.esp", 
						  task_app->tss.esp, 1);
			puts_varval01(&hnd_sc, &my_cnsl, "tss_app01.eip", 
						  task_app->tss.eip, 1);
			puts_varval01(&hnd_sc, &my_cnsl, "app08 pdt addr", 
						  task_app->vminf->pdt_addr, 1);


			//
			// create new task for myapp09
			//
			task_app06 = tsk_new_task_appelf(app06_load_addr,
											 app_b_dirent->size, 
											 16, 2);

			puts_varval01(&hnd_sc, &my_cnsl, "task_app06 selector", 
						  task_app06->sel, 1);
			puts_varval01(&hnd_sc, &my_cnsl, "tss_app06.esp", 
						  task_app06->tss.esp, 1);
			puts_varval01(&hnd_sc, &my_cnsl, "tss_app06.eip", 
						  task_app06->tss.eip, 1);
			puts_varval01(&hnd_sc, &my_cnsl, "app09 pdt addr", 
						  task_app06->vminf->pdt_addr, 1);



			tsk_set_switch_interval(20);

			tsk_debug_running();

			ctrl_status++;
		}*/
		else if (g_time_counter > 5600 && ctrl_status==35) {
			//tsk_debugout_tss(&task_app->tss);

			mm_print_pg_mngr();

			tsk_debug_running();

			//ctrl_status++;
			ctrl_status = 100;
		}
		else if (g_time_counter > 5400 && ctrl_status==23) {
			int rc;
			
			io_sti();
			//ata_init01(&hnd_sc, &my_cnsl);
			rc = tatam_readblock((unsigned char*)0x500000, 0x48);
			puts_varval01(&hnd_sc, my_cnsl, 
						  "tatam_readblock(0x500000, 0x48); rc", rc, 0);

			ctrl_status++;

		}
		else {
			io_stihlt();
		}
	}

}

void test_locking() {
	ml_test_lock_flg = 0;
	tsk_set_switch_interval(5);
	debug_varval01("flg addr", (int)&ml_test_lock_flg, 1);
	tsk_new_task_knl_default((unsigned int)knlthr01, PROCESS0_PDT);
	tsk_new_task_knl_default((unsigned int)knlthr02, PROCESS0_PDT);

}


void knlthr01() {
	unsigned long next_time_counter;
	int counter = 0;
	int flg_got_lock = 0;
	
	debug_puts("knlthr01 start.");
	next_time_counter = g_time_counter + 200;
	
	while(1) {
		if (next_time_counter < g_time_counter) {
			debug_varval01("knlthr01: count", counter++, 0);
			if (flg_got_lock) {
				flg_got_lock = 0;
				ml_test_lock_flg = 0;
				debug_varval01("knlthr01 released a lock. g_time", 
						g_time_counter, 0);
				tsk_wakeup_locked_tasks(TLCK_LOCK_MEM);
				next_time_counter = g_time_counter + 200;
			} else {
				if (ml_test_lock_flg == 0) {
					ml_test_lock_flg = 1;
					flg_got_lock = 1;
					debug_puts("knlthr01 got a lock.");
					next_time_counter = g_time_counter + 1000;
				} else if (ml_test_lock_flg == 1){
					debug_puts("knlthr01 couldn't get a lock.");
					next_time_counter = g_time_counter + 200;
				}
			}
		}
		io_hlt();
	}
	
}

void knlthr02() {
	unsigned long next_time_counter;
	unsigned long inner_next_counter;
	int counter = 0;
	int flg_got_lock = 0;
	
	debug_puts("        knlthr02 start.");
	next_time_counter = g_time_counter + 300;
	
	while(1) {
		if (next_time_counter < g_time_counter) {
			debug_varval01("        knlthr02: count", counter++, 0);
			
			debug_varval01("        knlthr02 try lock... g_time", 
					g_time_counter, 0);
			tlck_lock(&ml_test_lock_flg, TLCK_LOCK_MEM);
			flg_got_lock = 1;
			debug_varval01("        knlthr02 got a lock. g_time", 
					g_time_counter, 0);
			
			inner_next_counter = g_time_counter + 400;
			while (inner_next_counter > g_time_counter)
				io_stihlt();
			
			tlck_release(&ml_test_lock_flg, TLCK_LOCK_MEM);
			debug_puts("        knlthr02 released a lock.");
			
			next_time_counter = g_time_counter + 300;
		}
		if (flg_got_lock)
			io_hlt();
	}
	
}



int tty_switch(HND_SC* hnd_sc, int tty_no) {
	char s_mode[20];
	int rc = 0;
	
	if (tty_no == 0) {
		if (ml_tty0_switch) {
			ml_tty0_switch = 0;
			my_strcpy(s_mode, "tty0:OFF");
		}
		else {
			ml_tty0_switch = 1;
			my_strcpy(s_mode, "tty0:ON");
		}
		rc = ml_tty0_switch;
		boxfill8((unsigned char*)hnd_sc->vram_base, hnd_sc->width, 
				COL8_DARK_SKYBLUE, 8, 16*29, 8+8*8-1, 16*29+15);
		puts_pos(hnd_sc, s_mode, 8, 16*29);
		
	}
	return rc;
}


/*
 * FD read test.
 */
int fdc_test01(HND_SC *hnd_sc, TCNS_CONSOLE* cnsl) {

	FDC_SENSEINT_RESULT fdc_sint_res;
	FDC_READDATA_RESULT fdc_rd_res;

	int data;
	char *memaddr;
	char buff[512], buff2[512];

	/* seek cylinder 3 */
	fdc_send_seek(3);
	fancy_wait_fdcint();

	/* sense interrupt */
	fdc_sense_int(&fdc_sint_res);

	data = fdc_sint_res.sr0;
	puts_varval01(hnd_sc, cnsl, "fd-sr0=", data, 1);

	data = fdc_sint_res.cyl_no;
	puts_varval01(hnd_sc, cnsl, "fd-cyl=", data, 1);

	/* seek cylinder 0 */
	fdc_send_seek(0);
	fancy_wait_fdcint();

	/* sense interrupt */
	fdc_sense_int(&fdc_sint_res);

	data = fdc_sint_res.sr0;
	puts_varval01(hnd_sc, cnsl, "fd-sr0=", data, 1);

	data = fdc_sint_res.cyl_no;
	puts_varval01(hnd_sc, cnsl, "fd-cyl=", data, 1);
			
	/* prepare DMAC for read */
	prepare_dmac_read_fd();

	/* send READ DATA and receive result */
	fdc_readtrack(&fdc_rd_res, 0, 0);

	/* FDC motor off */
	my_puts(hnd_sc, cnsl, "FDC motor off");
	fdc_motor_off();

	return 0;
}

void test_console(HND_SC* hnd_sc) {
	int x, y;
	
	y = 5;
	for (x=5; x<15; x++) {
		tcnm_boxfill(hnd_sc, x, y, COL8_YELLOW);
	}
	for (y=5; y<10; y++) {
		tcnm_boxfill(hnd_sc, x, y, COL8_RED);
	}
}
