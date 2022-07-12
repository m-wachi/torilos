#ifndef FDC01_H
#define FDC01_H

#define FDC_DOR         0x03f2	/* Data Output Regiser */
#define FDC_CSR         0x03f4	/* Controller Status Register */
/* CSR content
   Bit
   Function
   7 Request for Master
   6 Data Input/Output    1:FDC->CPU  0:CPU->FDC
   5 Non-DMA Mode
   4 Diskette Controller Busy
   3 Reserved
   2 Reserved
   1 Drive 1 Busy
   0 Drive 0 Busy(Seek mode)
*/

#define FDC_DR          0x03f5	/* Data Register */


/* Status Register 0
   7, 6 Interrupt Code (IC)
     00 = Normal Termination of Command (NT)
     01 = Abrupt Termination of Command (AT) 
	      - command executed but end abnormally.
     10 = Invalid Command Issue (IC) - command wasn't started.
     11 = Reserved (Abnormal Termination)
   5 Seek End (SE) - ON if 'seek' command completed.
   4 Equipment Check (EC) 
     - ON if FDC didn't detect 'TRACK0-SIGNAL' 
	   after 'Recalibrate' command.
   3 Reserved
   2 Head Address (HD)
   1, 0 Unit select(US) -  Drive NO which interrupted

   US definition
   00 Drive0
   01 Drive1
   10, 11 reserved.

*/

/* Status Register 1
  7 End of Cylinder (EN) - ON if Controller will access the end of 
                         sector in cylinder.
  6 Reserved - always 0
  5 Data Error (DE) - ON if controller detect CRC-Error 
                    on ID field or data field.
  4 Overrun (OR) - ON if MAIN-SYSTEM didn't give the service to controller.
  3 Reserved - always 0
  2 No Data (ND) - ON if controller couldn't find the sector.
  1 Not Writable (NW) 
  0 Missing Address Mark (MA) 
*/

typedef struct fdc_senseint_result {
	char sr0;     /* status register0 */
	char cyl_no;  /* cylinder no */
} FDC_SENSEINT_RESULT;

typedef struct fdc_readdata_result {
	char sr0;     /* status register0 */
	char sr1;     /* status register1 */
	char sr2;     /* status register2 */
	char cyl_no;  /* cylinder no */
	char head;	  /* head address */
	char sec_no;  /* sector no */
	char byte_per_sec; /* number of data byte in sector */
} FDC_READDATA_RESULT;

void init_dmac_fd();
void prepare_dmac_read_fd();
void prepare_dmac_write_fd();


void wait_fdc_send(void);
void wait_fdc_recv(void);
char wait_fdc_readwrite_ready(void);
int wait_fdc_ready_senseint(void);
void fdc_sense_int(FDC_SENSEINT_RESULT *result);
void fdc_send_seek(char);
void fdc_readdata_result(FDC_READDATA_RESULT* res);
int fdc_readtrack(FDC_READDATA_RESULT* fdc_rd_res, 
				  unsigned char cylinder, unsigned char head_addr);

void fdc_motor_off();
void fdc_motor_on();


#endif
