#include <torilos/fdc01.h>
#include <torilos/myutil01.h>
#include <torilos/debug01.h>
#include <torilos/myasmfunc01.h>

void init_dmac_fd() {
	io_out8(0x0a, 0x06);		/* mask DMA channel 2 */
	io_out8(0xd8, 0xFF);		/* reset the master flip-flop */
	io_out8(0x04, 0);			/* address to 0 (low byte) */
	io_out8(0x04, 0);			/* address to 0 (high byte) */
	io_out8(0xd8, 0xFF);		/* reset the master flip-flop (again!!!) */
	io_out8(0x05, 0xFF);		/* count to 0x23ff (low byte) */
	io_out8(0x05, 0x23);        /* count to 0x23ff (high byte) */
	io_out8(0x81, 0);	        /* external page register to 
								   0 for total address of 00 00 00 */
	io_out8(0x0a, 0x02);		/* unmask DMA channel 2 */
}

void prepare_dmac_write_fd() {
	io_out8(0x0a, 0x06);		/* mask DMA channel 2 */
	io_out8(0x0b, 0x5A);		/* 01011010
								   single transfer, address increment, 
								   autoinit, write, channel2 */
	io_out8(0x0a, 0x02);		/* unmask DMA channel 2 */
}

void prepare_dmac_read_fd() {
	io_out8(0x0a, 0x06); 	    /* mask DMA channel 2 */
	io_out8(0x0b, 0x06);		/* 01010110 
								   single transfer, address increment, 
								   autoinit, read, channel2 */
	
	io_out8(0x0a, 0x02);		/* unmask DMA channel 2 */
}

/*
 * wait until FDC is ready to OUT
 */
void wait_fdc_send(void) {

	for (;;) {
		/* 0x80=10000000 
		   7bit: ready to transfer
		   6bit: 0->data transfer from CPU to FDC
		*/
		if ((io_in8(0x03f4) & 0xc0) == 0x80) {
			break;
		}
	}
	return;
}

/*
 * wait until FDC is ready to IN
 */
void wait_fdc_recv(void) {
	for (;;) {
		/* 0xc0=11000000
		   7bit: ready to transfer
		   6bit: 1->data transfer from FDC to CPU
		*/
		if ((io_in8(0x03f4) & 0xc0) == 0xc0) {
			break;
		}
	}
	return;
}

/* 
 * wait until end seek mode and 
 * ready to send read or write command. 
 */
char wait_fdc_readwrite_ready(void) {

	for (;;) {
		/* 0x11=10001 
		   4bit: executing read or write
		   0bit: drive0 is seek mode. */
		if ((io_in8(0x03f4) & 0x11) == 0) {
			break;
		}
	}
	return 0;
}

/*
 * wait until FDC is ready 
 * to send 'sense interrupt' command
 */
int wait_fdc_ready_senseint(void) {
	char csr;
	do { 
		/* rapuOS setting
		   0x10=10000 
		   4bit: executing read or write */

		/* 1000????
		   7bit: ready to transfer
		   6bit: 0-> transfer from CPU to FDC
		   5bit: 1-> non DMA mode
		   4bit: executing read or write */

		csr = io_in8(FDC_CSR);

	} while((csr & 0xf0) != 0x80);

	return 0;
}

/*
 * send 'sense interrupt' and receive 
 * result(status register0, cylinder no)
 * 
 */
void fdc_sense_int(FDC_SENSEINT_RESULT *result) {

	wait_fdc_ready_senseint();


	wait_fdc_send();
	io_out8(FDC_DR, 0x08); /* sense interrupt */

	wait_fdc_recv();
	result->sr0 = io_in8(FDC_DR);
	
	
	wait_fdc_recv();
	result->cyl_no = io_in8(FDC_DR);
	/*	print_varval02(&hnd_sc, "fd-cyl=", data, 1, 150, 16*2);
	  fd_ctrl_flg++; */
}

/*
 * send 'seek' command
 */
void fdc_send_seek(char cyl_no) {
	wait_fdc_send(); io_out8(FDC_DR, 0x0f);
	wait_fdc_send(); io_out8(FDC_DR, 0x00);
	wait_fdc_send(); io_out8(FDC_DR, cyl_no);

}

/*
 * receive 'read data' command result 
 */
void fdc_readdata_result(FDC_READDATA_RESULT* res) {
	wait_fdc_recv();
	res->sr0 = io_in8(FDC_DR);
	wait_fdc_recv();
	res->sr1 = io_in8(FDC_DR);
	wait_fdc_recv();
	res->sr2 = io_in8(FDC_DR);
	wait_fdc_recv();
	res->cyl_no = io_in8(FDC_DR);
	wait_fdc_recv();
	res->head = io_in8(FDC_DR);
	wait_fdc_recv();
	res->sec_no = io_in8(FDC_DR);
	wait_fdc_recv();
	res->byte_per_sec = io_in8(FDC_DR);
}

/*
 * read track
 * @fdc_rd_res: READ DATA result
 * @cylinder: cylinder
 * @head_addr: head address
 */
int fdc_readtrack(FDC_READDATA_RESULT* fdc_rd_res, 
				  unsigned char cylinder, unsigned char head_addr) {

	wait_fdc_readwrite_ready();
	/* send read fd command */
	wait_fdc_send();

	debug_puts("start send data");

	io_out8(FDC_DR, 0xe6); /* read fd */
	wait_fdc_send();
	io_out8(FDC_DR, 0x00); /* head0, drive0 */

	wait_fdc_send();
	io_out8(FDC_DR, cylinder); /* cylinder0 */
	wait_fdc_send();
	io_out8(FDC_DR, head_addr); /* head address */
	wait_fdc_send();
	io_out8(FDC_DR, 1);	   /* sector number */
	wait_fdc_send();
	io_out8(FDC_DR, 0x02); /* 512byte per sector */
			
	wait_fdc_send();
	io_out8(FDC_DR, 0x12); /* EOT */
	wait_fdc_send();
	io_out8(FDC_DR, 0x1b); /* GAP3 Length */
	wait_fdc_send();
	io_out8(FDC_DR, 0xff); /* Data Length */

	fancy_wait_fdcint();

	/* receive read fd command */
	fdc_readdata_result(fdc_rd_res);

	debug_varval01("fd-sr0=", fdc_rd_res->sr0, 1);
	debug_varval01("fd-sr1=", fdc_rd_res->sr1, 1);
	debug_varval01("fd-sr2=", fdc_rd_res->sr2, 1);

	debug_varval01("fd-cyl=", fdc_rd_res->cyl_no, 1);
	debug_varval01("fd-head=", fdc_rd_res->head, 1);
	debug_varval01("fd-bps=", fdc_rd_res->byte_per_sec, 1);

	return 0;
}



/*
 * FDC motor off 
 */
void fdc_motor_off() {
	io_out8(FDC_DOR, 0x0c);
}

/*
 * FDC motor on
 */
void fdc_motor_on() {
	io_out8(FDC_DOR, 0x1c);
}
