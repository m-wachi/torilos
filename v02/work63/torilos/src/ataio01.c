/**
 * @file ataio01.c
 * @brief ATA I/O Module
 * 
 * This code comes from ATADRVR, but not equals.
 * Adjust for 32bit-mode, change some assembler code 
 * for torilos.
 */
#include <torilos/mylib01.h>
#include <torilos/ataio01.h>
#include <torilos/pci01.h>
#include <torilos/myasmfunc01.h>
#include <torilos/debug01.h>

static void reg_wait_poll(ATA_IO* ata_io, int we, int pe, HND_TMR* tmr);
static int exec_pio_data_out_cmd(ATA_IO* ata_io, int dev,
								 unsigned int seg, unsigned int off,
								 long numSect, int multiCnt );
static int exec_pio_data_in_cmd(ATA_IO* ata_io, int dev,
								unsigned int seg, unsigned int off,
								long numSect, int multiCnt);



/*******************************************
 * Set the host adapter i/o base addresses.
 *******************************************/
void pio_set_iobase_addr(ATA_IO* ata_io) {
	/* original arguments
	  (unsigned int base1, unsigned int base2, unsigned int base3 )
	*/

	/* original code
   pio_base_addr1 = base1;
   pio_base_addr2 = base2;
   pio_bmcr_base_addr = base3;
	*/
	ata_io->pio_memory_seg = 0;
	ata_io->int_ata_status = 0;
   /*
	pio_reg_addrs[ CB_DATA ] = pio_base_addr1 + 0;  // 0
	pio_reg_addrs[ CB_FR   ] = pio_base_addr1 + 1;  // 1
	pio_reg_addrs[ CB_SC   ] = pio_base_addr1 + 2;  // 2
	pio_reg_addrs[ CB_SN   ] = pio_base_addr1 + 3;  // 3
	pio_reg_addrs[ CB_CL   ] = pio_base_addr1 + 4;  // 4
	pio_reg_addrs[ CB_CH   ] = pio_base_addr1 + 5;  // 5
	pio_reg_addrs[ CB_DH   ] = pio_base_addr1 + 6;  // 6
	pio_reg_addrs[ CB_CMD  ] = pio_base_addr1 + 7;  // 7
	pio_reg_addrs[ CB_DC   ] = pio_base_addr2 + 6;  // 8
	pio_reg_addrs[ CB_DA   ] = pio_base_addr2 + 7;  // 9
	*/

	ata_io->pio_reg_addrs[ CB_DATA ] = ata_io->cmdBase + 0;  // 0
	ata_io->pio_reg_addrs[ CB_FR   ] = ata_io->cmdBase + 1;  // 1
	ata_io->pio_reg_addrs[ CB_SC   ] = ata_io->cmdBase + 2;  // 2
	ata_io->pio_reg_addrs[ CB_SN   ] = ata_io->cmdBase + 3;  // 3
	ata_io->pio_reg_addrs[ CB_CL   ] = ata_io->cmdBase + 4;  // 4
	ata_io->pio_reg_addrs[ CB_CH   ] = ata_io->cmdBase + 5;  // 5
	ata_io->pio_reg_addrs[ CB_DH   ] = ata_io->cmdBase + 6;  // 6
	ata_io->pio_reg_addrs[ CB_CMD  ] = ata_io->cmdBase + 7;  // 7
	ata_io->pio_reg_addrs[ CB_DC   ] = ata_io->ctrlBase + 6;  // 8
	ata_io->pio_reg_addrs[ CB_DA   ] = ata_io->ctrlBase + 7;  // 9
	
}

//*************************************************************
//
// reg_config() - Check the host adapter and determine the
//                number and type of drives attached.
//
// This process is not documented by any of the ATA standards.
//
// Infomation is returned by this function is in
// reg_config_info[] -- see ATAIO.H.
//
//*************************************************************
int reg_config(ATA_IO* ata_io) {
   int numDev = 0;
   unsigned char sc;
   unsigned char sn;
   unsigned char cl;
   unsigned char ch;
   unsigned char st;
   unsigned char devCtrl;

   // setup register values

   devCtrl = CB_DC_HD15 | ( ata_io->int_use_intr_flag ? 0 : CB_DC_NIEN );

   // mark start of config in low level trace

   /*trc_llt( 0, 0, TRC_LLT_S_CFG ); */

   // reset Bus Master Error bit

   sub_writeBusMstrStatus(ata_io, BM_SR_MASK_ERR );

   // assume there are no devices

   ata_io->reg_config_info[0] = REG_CONFIG_TYPE_NONE;
   ata_io->reg_config_info[1] = REG_CONFIG_TYPE_NONE;

   // set up Device Control register
   /*
   pio_outbyte( CB_DC, devCtrl );
   */
   pio_outbyte(ata_io, CB_DC, devCtrl );

   // lets see if there is a device 0
   /* pio_outbyte( CB_DH, CB_DH_DEV0 ); */
   pio_outbyte(ata_io, CB_DH, CB_DH_DEV0 );


   /* DELAY400NS; */
   ata_delay400ns(ata_io);

   //pio_outbyte( CB_SC, 0x55 );
   pio_outbyte(ata_io, CB_SC, 0x55 );
   //pio_outbyte( CB_SN, 0xaa );
   pio_outbyte(ata_io, CB_SN, 0xaa );
   //pio_outbyte( CB_SC, 0xaa );
   pio_outbyte(ata_io, CB_SC, 0xaa );
   //pio_outbyte( CB_SN, 0x55 );
   pio_outbyte(ata_io, CB_SN, 0x55 );
   //pio_outbyte( CB_SC, 0x55 );
   pio_outbyte(ata_io, CB_SC, 0x55 );
   //pio_outbyte( CB_SN, 0xaa );
   pio_outbyte(ata_io, CB_SN, 0xaa );
   //sc = pio_inbyte( CB_SC );
   sc = pio_inbyte(ata_io, CB_SC);
   //sn = pio_inbyte( CB_SN );
   sn = pio_inbyte(ata_io, CB_SN );

   if ( ( sc == 0x55 ) && ( sn == 0xaa ) )
      ata_io->reg_config_info[0] = REG_CONFIG_TYPE_UNKN;

   // lets see if there is a device 1

   pio_outbyte(ata_io, CB_DH, CB_DH_DEV1 );

   /* ignore at the moment
	  DELAY400NS; */

   pio_outbyte(ata_io, CB_SC, 0x55 );
   pio_outbyte(ata_io, CB_SN, 0xaa );
   pio_outbyte(ata_io, CB_SC, 0xaa );
   pio_outbyte(ata_io, CB_SN, 0x55 );
   pio_outbyte(ata_io, CB_SC, 0x55 );
   pio_outbyte(ata_io, CB_SN, 0xaa );
   sc = pio_inbyte(ata_io, CB_SC );
   sn = pio_inbyte(ata_io, CB_SN );
   if ( ( sc == 0x55 ) && ( sn == 0xaa ) )
      ata_io->reg_config_info[1] = REG_CONFIG_TYPE_UNKN;

   // now we think we know which devices, if any are there,
   // so lets try a soft reset (ignoring any errors).

   pio_outbyte(ata_io, CB_DH, CB_DH_DEV0 );
   //DELAY400NS;

   reg_reset(ata_io, 0, 0 );

   // lets check device 0 again, is device 0 really there?
   // is it ATA or ATAPI?

   pio_outbyte(ata_io, CB_DH, CB_DH_DEV0 );
   //DELAY400NS;
   sc = pio_inbyte(ata_io, CB_SC );
   sn = pio_inbyte(ata_io, CB_SN );
   if ( ( sc == 0x01 ) && ( sn == 0x01 ) ) {
      ata_io->reg_config_info[0] = REG_CONFIG_TYPE_UNKN;
      st = pio_inbyte(ata_io, CB_STAT );
      cl = pio_inbyte(ata_io, CB_CL );
      ch = pio_inbyte(ata_io, CB_CH );
      if ( ( ( cl == 0x14 ) && ( ch == 0xeb ) )       // PATAPI
           ||
           ( ( cl == 0x69 ) && ( ch == 0x96 ) )       // SATAPI
         ) {
		  ata_io->reg_config_info[0] = REG_CONFIG_TYPE_ATAPI;
      } else if ( ( st != 0 ) &&
				  ( ( ( cl == 0x00 ) && ( ch == 0x00 ) )     // PATA
					||
					( ( cl == 0x3c ) && ( ch == 0xc3 ) ) )   // SATA
				)  {
		  ata_io->reg_config_info[0] = REG_CONFIG_TYPE_ATA;
      }
   }

   // lets check device 1 again, is device 1 really there?
   // is it ATA or ATAPI?

   pio_outbyte(ata_io, CB_DH, CB_DH_DEV1 );
   //DELAY400NS;
   sc = pio_inbyte(ata_io, CB_SC );
   sn = pio_inbyte(ata_io, CB_SN );
   if ( ( sc == 0x01 ) && ( sn == 0x01 ) ) {
	   ata_io->reg_config_info[1] = REG_CONFIG_TYPE_UNKN;
	   st = pio_inbyte(ata_io, CB_STAT );
	   cl = pio_inbyte(ata_io, CB_CL );
	   ch = pio_inbyte(ata_io, CB_CH );
	   if ( ( ( cl == 0x14 ) && ( ch == 0xeb ) )       // PATAPI
			||
			( ( cl == 0x69 ) && ( ch == 0x96 ) )       // SATAPI
		  ) {
		   ata_io->reg_config_info[1] = REG_CONFIG_TYPE_ATAPI;
	   }
	   else if ( ( st != 0 )
				 &&
				 ( ( ( cl == 0x00 ) && ( ch == 0x00 ) )     // PATA
				   ||
				   ( ( cl == 0x3c ) && ( ch == 0xc3 ) ) )   // SATA
			   ) {
		   ata_io->reg_config_info[1] = REG_CONFIG_TYPE_ATA;
	   }
   }

   // If possible, select a device that exists, try device 0 first.
   if ( ata_io->reg_config_info[1] != REG_CONFIG_TYPE_NONE ) {
	   pio_outbyte(ata_io, CB_DH, CB_DH_DEV1 );
	   //DELAY400NS;
	   numDev ++ ;
   }
   if ( ata_io->reg_config_info[0] != REG_CONFIG_TYPE_NONE ) {
	   pio_outbyte(ata_io, CB_DH, CB_DH_DEV0 );
	   //DELAY400NS;
	   numDev ++ ;
   }

   // BMCR/BMIDE Error=1?

   if ( sub_readBusMstrStatus(ata_io) & BM_SR_MASK_ERR ) {
      ata_io->reg_cmd_info.ec = 78;                  // yes
      /* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
   }

   // mark end of config in low level trace

   //trc_llt( 0, 0, TRC_LLT_E_CFG );

   // return the number of devices found

   return numDev;
}

//*************************************************************
//
// reg_reset() - Execute a Software Reset.
//
// See ATA-2 Section 9.2, ATA-3 Section 9.2, ATA-4 Section 8.3.
//
//*************************************************************
int reg_reset(ATA_IO* ata_io, int skipFlag, int devRtrn ) {
	unsigned char sc;
	unsigned char sn;
	unsigned char status;
	unsigned char devCtrl;
	HND_TMR tmr;

	// setup register values
	devCtrl = CB_DC_HD15 | ( ata_io->int_use_intr_flag ? 0 : CB_DC_NIEN );

	// mark start of reset in low level trace
	/* trc_llt( 0, 0, TRC_LLT_S_RST ); */

   // reset Bus Master Error bit
   sub_writeBusMstrStatus(ata_io, BM_SR_MASK_ERR );

   // Reset error return data.
   sub_zero_return_data(ata_io);
   ata_io->reg_cmd_info.flg = TRC_FLAG_SRST;
   ata_io->reg_cmd_info.ct  = TRC_TYPE_ASR;

   // initialize the command timeout counter
   /* tmr_set_timeout(); */
   /******************************
	* I don't know how long wait
	* so set timeout 5sec
	******************************/
   my_set_timeout(&tmr, 500);

   // Set and then reset the soft reset bit in the Device Control
   // register.  This causes device 0 be selected.

   if ( ! skipFlag ) {
	   pio_outbyte(ata_io, CB_DC, devCtrl | CB_DC_SRST );
	   /* DELAY400NS; */
	   ata_delay400ns(ata_io);
	   pio_outbyte(ata_io, CB_DC, devCtrl );
	   /* DELAY400NS; */
	   ata_delay400ns(ata_io);
   }

   // If there is a device 0, wait for device 0 to set BSY=0.
   if ( ata_io->reg_config_info[0] != REG_CONFIG_TYPE_NONE ) {
	   sub_atapi_delay(ata_io, 0);
	   /* trc_llt( 0, 0, TRC_LLT_PNBSY ); */
	   while ( 1 ) {
		   status = pio_inbyte(ata_io, CB_STAT);
		   if ( ( status & CB_STAT_BSY ) == 0 )
			   break;
		   
	
		   /* if ( tmr_chk_timeout() ) { */
		   if (my_is_timeout(&tmr)) {
			   /* trc_llt( 0, 0, TRC_LLT_TOUT ); */
			   ata_io->reg_cmd_info.to = 1;
			   ata_io->reg_cmd_info.ec = 1;
			   /* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
			   break;
		   }
	   }
   }

   // If there is a device 1, wait until device 1 allows
   // register access.

   if (ata_io->reg_config_info[1] != REG_CONFIG_TYPE_NONE ) {
	   sub_atapi_delay(ata_io, 1 );
	   /* trc_llt( 0, 0, TRC_LLT_PNBSY ); */
	   while ( 1 ) {
		   pio_outbyte(ata_io, CB_DH, CB_DH_DEV1 );
		   /* ignore at the moment
			  DELAY400NS;*/
		   sc = pio_inbyte(ata_io, CB_SC );
		   sn = pio_inbyte(ata_io, CB_SN );
		   
		   if ( ( sc == 0x01 ) && ( sn == 0x01 ) )
			   break;

		   /* if ( tmr_chk_timeout() ) { */
		   if (my_is_timeout(&tmr)) {
			   /* trc_llt( 0, 0, TRC_LLT_TOUT ); */
			   ata_io->reg_cmd_info.to = 1;
			   ata_io->reg_cmd_info.ec = 2;
			   /* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
			   break;
		   }
      }

      // Now check if drive 1 set BSY=0.

      if ( ata_io->reg_cmd_info.ec == 0 ) {
		  if ( pio_inbyte(ata_io, CB_STAT ) & CB_STAT_BSY ) {
            ata_io->reg_cmd_info.ec = 3;
            /* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
		  }
      }
   }

   // RESET_DONE:

   // We are done but now we must select the device the caller
   // requested before we trace the command.  This will cause
   // the correct data to be returned in reg_cmd_info.

   pio_outbyte(ata_io, CB_DH, devRtrn ? CB_DH_DEV1 : CB_DH_DEV0 );
   /* ignore at the moment
	  DELAY400NS;*/

   /* ignore at the moment
	  sub_trace_command();*/

   // If possible, select a device that exists,
   // try device 0 first.
   
   if ( ata_io->reg_config_info[1] != REG_CONFIG_TYPE_NONE ) {
	   pio_outbyte(ata_io, CB_DH, CB_DH_DEV1 );
	   /* ignore at the moment
		  DELAY400NS; */
   }
   if ( ata_io->reg_config_info[0] != REG_CONFIG_TYPE_NONE )
   {
	   pio_outbyte(ata_io, CB_DH, CB_DH_DEV0 );
	  /* ignore at the moment
		 DELAY400NS; */
   }

   // BMCR/BMIDE Error=1?

   if ( sub_readBusMstrStatus(ata_io) & BM_SR_MASK_ERR ) {
	   ata_io->reg_cmd_info.ec = 78;                  // yes
	   /* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
   }

   // mark end of reset in low level trace

   /* trc_llt( 0, 0, TRC_LLT_E_RST ); */

   // All done.  The return values of this function are described in
   // ATAIO.H.

   if (ata_io->reg_cmd_info.ec )
      return 1;
   return 0;
}

//*************************************************************
//
// reg_pio_data_in_lba28() - Easy way to execute a PIO Data In command
//                           using an LBA sector address.
//
//*************************************************************
int reg_pio_data_in_lba28(ATA_IO* ata_io, int dev, int cmd,
                           unsigned int fr, unsigned int sc,
                           unsigned long lba,
                           unsigned int seg, unsigned int off,
						  long numSect, int multiCnt) {
	                      //HND_SC* hnd_sc,  MY_CONSOLE* cnsl) {

	// Reset error return data.

	sub_zero_return_data(ata_io);
	ata_io->reg_cmd_info.flg = TRC_FLAG_ATA;
	ata_io->reg_cmd_info.ct  = TRC_TYPE_APDI;
	ata_io->reg_cmd_info.cmd = cmd;
	ata_io->reg_cmd_info.fr1 = fr;
	ata_io->reg_cmd_info.sc1 = sc;
	ata_io->reg_cmd_info.dh1 = CB_DH_LBA | (dev ? CB_DH_DEV1 : CB_DH_DEV0 );
	ata_io->reg_cmd_info.dc1 = ata_io->int_use_intr_flag ? 0 : CB_DC_NIEN;
	ata_io->reg_cmd_info.lbaSize = LBA28;
	ata_io->reg_cmd_info.lbaHigh1 = 0L;
	ata_io->reg_cmd_info.lbaLow1 = lba;

	// these commands transfer only 1 sector
	if (( cmd == CMD_IDENTIFY_DEVICE )
		|| ( cmd == CMD_IDENTIFY_DEVICE_PACKET ))
		numSect = 1;

	// adjust multiple count
	if ( multiCnt & 0x0800 ) {
		// assume caller knows what they are doing
		multiCnt &= 0x00ff;
	} else {
      // only Read Multiple uses multiCnt
		if ( cmd != CMD_READ_MULTIPLE )
			multiCnt = 1;
	}

	ata_io->reg_cmd_info.ns  = numSect;
	ata_io->reg_cmd_info.mc  = multiCnt;

	return exec_pio_data_in_cmd(ata_io, dev, seg, off, numSect, multiCnt);
								//hnd_sc, cnsl);
}

//*************************************************************
//
// reg_pio_data_out_lba28() - Easy way to execute a PIO Data In command
//                            using an LBA sector address.
//
//*************************************************************

int reg_pio_data_out_lba28(ATA_IO* ata_io, int dev, int cmd,
                            unsigned int fr, unsigned int sc,
                            unsigned long lba,
                            unsigned int seg, unsigned int off,
                            long numSect, int multiCnt )

{

	// Reset error return data.

	sub_zero_return_data(ata_io);
	ata_io->reg_cmd_info.flg = TRC_FLAG_ATA;
	ata_io->reg_cmd_info.ct  = TRC_TYPE_APDO;
	ata_io->reg_cmd_info.cmd = cmd;
	ata_io->reg_cmd_info.fr1 = fr;
	ata_io->reg_cmd_info.sc1 = sc;
	ata_io->reg_cmd_info.dh1 = CB_DH_LBA | (dev ? CB_DH_DEV1 : CB_DH_DEV0 );
	ata_io->reg_cmd_info.dc1 = ata_io->int_use_intr_flag ? 0 : CB_DC_NIEN;
	ata_io->reg_cmd_info.lbaSize = LBA28;
	ata_io->reg_cmd_info.lbaHigh1 = 0;
	ata_io->reg_cmd_info.lbaLow1 = lba;

	// adjust multiple count
	if ( multiCnt & 0x0800 )
	{
		// assume caller knows what they are doing
		multiCnt &= 0x00ff;
	}
	else
	{
		// only Write Multiple and CFA Write Multiple W/O Erase uses multiCnt
		if (    ( cmd != CMD_WRITE_MULTIPLE )
				&& ( cmd != CMD_CFA_WRITE_MULTIPLE_WO_ERASE )
		)
			multiCnt = 1;
	}

	ata_io->reg_cmd_info.ns  = numSect;
	ata_io->reg_cmd_info.mc  = multiCnt;

	return exec_pio_data_out_cmd(ata_io, dev, seg, off, numSect, multiCnt );
}


/*************************************************************
 * reg_wait_poll() - wait for interrupt or poll for BSY=0
 *
 *************************************************************/
static void reg_wait_poll(ATA_IO* ata_io, int we, int pe, HND_TMR* tmr ) {
	unsigned char status;

	// Wait for interrupt -or- wait for not BUSY -or- wait for time out.

	if ( we && ata_io->int_use_intr_flag ) {
		/* trc_llt( 0, 0, TRC_LLT_WINT ); */
		while ( 1 ) {
			if ( ata_io->int_intr_flag ) {                // interrupt ?
				/* trc_llt( 0, 0, TRC_LLT_INTRQ ); */ // yes
				/*if ( ata_io->int_bmcr_addr )
					 trc_llt( 0, ata_io->int_bm_status, TRC_LLT_R_BM_SR );*/
				status = ata_io->int_ata_status;         // get status
				/* trc_llt( CB_STAT, status, TRC_LLT_INB ); */
				/* if ( ata_io->int_bmcr_addr )
					trc_llt( 0, 0x04, TRC_LLT_W_BM_SR ); */
				break;
			}
			/* if ( tmr_chk_timeout() ) { */          // time out ?
			if (my_is_timeout(tmr)) {
				/* trc_llt( 0, 0, TRC_LLT_TOUT ); */  // yes
				ata_io->reg_cmd_info.to = 1;
				ata_io->reg_cmd_info.ec = we;
				/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
				break;
			}
		}
	}
	else {
		/* trc_llt( 0, 0, TRC_LLT_PNBSY ); */
		while ( 1 ) {
			status = pio_inbyte(ata_io, CB_ASTAT );       // poll for not busy
			if ( ( status & CB_STAT_BSY ) == 0 )
				break;
			/* if ( tmr_chk_timeout() ) { */              // time out yet ?
			if (my_is_timeout(tmr)) {
				/* trc_llt( 0, 0, TRC_LLT_TOUT ); */
				ata_io->reg_cmd_info.to = 1;
				ata_io->reg_cmd_info.ec = pe;
				/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
				break;
			}
		}
	}

#if DEBUG_REG & 0x01
	/* trc_llt( 0, int_intr_cntr, TRC_LLT_DEBUG ); */  // for debugging
#endif

	// Reset the interrupt flag.

	if ( ata_io->int_intr_flag > 1 )      // extra interrupt(s) ?
		ata_io->reg_cmd_info.failbits |= FAILBIT15;
	ata_io->int_intr_flag = 0;
}

//*************************************************************
//
// exec_pio_data_out_cmd() - Execute a PIO Data Out command.
//
// See ATA-2 Section 9.4, ATA-3 Section 9.4,
// ATA-4 Section 8.7 Figure 11.
//
//*************************************************************
static int exec_pio_data_out_cmd(ATA_IO* ata_io, int dev,
								 unsigned int seg, unsigned int off,
								 long numSect, int multiCnt ) {
	unsigned char status;
	int loopFlag = 1;
	long wordCnt;
	unsigned int saveSeg = seg;
	unsigned int saveOff = off;

	HND_TMR tmr;


	// mark start of PDO cmd in low level trace
	/* trc_llt( 0, 0, TRC_LLT_S_PDO ); */

	// reset Bus Master Error bit
	sub_writeBusMstrStatus(ata_io, BM_SR_MASK_ERR );

	// Set command time out.
	/* tmr_set_timeout(); */
	/******************************
	* I don't know how long wait
	* so set timeout 5sec
	******************************/
	my_set_timeout(&tmr, 500);


	// Select the drive - call the sub_select function.
	// Quit now if this fails.
	if ( sub_select(ata_io, dev, &tmr) ) {
		/* sub_trace_command(); */
		/* trc_llt( 0, 0, TRC_LLT_E_PDO ); */
		ata_io->reg_drq_block_call_back = (void *) 0;
		return 1;
	}

	// Set up all the registers except the command register.
	sub_setup_command(ata_io);

	// For interrupt mode, instal interrupt handler.
	/* ignore at the moment
	   int_save_int_vect();*/

	// Start the command by setting the Command register.  The drive
	// should immediately set BUSY status.
	pio_outbyte(ata_io, CB_CMD, ata_io->reg_cmd_info.cmd );

	// Waste some time by reading the alternate status a few times.
	// This gives the drive time to set BUSY in the status register on
	// really fast systems.  If we don't do this, a slow drive on a fast
	// system may not set BUSY fast enough and we would think it had
	// completed the command when it really had not even started the
	// command yet.

	/* DELAY400NS; */
	ata_delay400ns(ata_io);

	// Wait for not BUSY or time out.
	// Note: No interrupt is generated for the
	// first sector of a write command.  Well...
	// that's not really true we are working with
	// a PCMCIA PC Card ATA device.

	sub_atapi_delay(ata_io, dev);
	/* trc_llt( 0, 0, TRC_LLT_PNBSY ); */
	while ( 1 )	{
		status = pio_inbyte(ata_io, CB_ASTAT );
		if ( ( status & CB_STAT_BSY ) == 0 )
			break;
		/* if ( tmr_chk_timeout() ) { */
		if (my_is_timeout(&tmr)) {
			/* trc_llt( 0, 0, TRC_LLT_TOUT ); */
			ata_io->reg_cmd_info.to = 1;
			ata_io->reg_cmd_info.ec = 47;
			/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
			loopFlag = 0;
			break;
		}
	}

	// If we are using interrupts and we just got an interrupt, this is
	// wrong.  The drive must not generate an interrupt at this time.

	if ( loopFlag && ata_io->int_use_intr_flag && ata_io->int_intr_flag ) {
		ata_io->reg_cmd_info.ec = 46;
		/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
		loopFlag = 0;
	}

	// This loop writes each sector.

	while ( loopFlag ) {
		// WRITE_LOOP:
		//
		// NOTE NOTE NOTE ...  The primary status register (1f7) MUST NOT be
		// read more than ONCE for each sector transferred!  When the
		// primary status register is read, the drive resets IRQ 14.  The
		// alternate status register (3f6) can be read any number of times.
		// For correct results, transfer the 256 words (REP OUTSW), wait for
		// interrupt and then read the primary status register.  AS
		// SOON as BOTH the primary status register has been read AND the
		// last of the 256 words has been written, the drive is allowed to
		// generate the next IRQ 14 (newer and faster drives could generate
		// the next IRQ 14 in 50 microseconds or less).  If the primary
		// status register is read more than once, there is the possibility
		// of a race between the drive and the software and the next IRQ 14
		// could be reset before the system interrupt controller sees it.

		// If BSY=0 and DRQ=1, transfer the data,
		// even if we find out there is an error later.

		if ( ( status & ( CB_STAT_BSY | CB_STAT_DRQ ) ) == CB_STAT_DRQ ) {
			// do the slow data transfer thing
			if ( ata_io->reg_slow_xfer_flag ) {
				if ( numSect <= ata_io->reg_slow_xfer_flag ) {
					sub_xfer_delay();
					ata_io->reg_slow_xfer_flag = 0;
				}
			}

			// increment number of DRQ packets
			ata_io->reg_cmd_info.drqPackets ++ ;

			// determine the number of sectors to transfer
			wordCnt = multiCnt ? multiCnt : 1;
			if ( wordCnt > numSect )
				wordCnt = numSect;
			wordCnt = wordCnt * 256;

			// Quit if buffer overrun.
			// If DRQ call back in use:
			// a) Call DRQ block call back function.
			// b) Adjust buffer address.

			if ( ata_io->reg_drq_block_call_back ) {
				if ( ( wordCnt << 1 ) > ata_io->reg_buffer_size ) {
					ata_io->reg_cmd_info.ec = 61;
					/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
					break;   // go to READ_DONE
				}
				ata_io->reg_cmd_info.drqPacketSize = ( wordCnt << 1 );
				(* ata_io->reg_drq_block_call_back) ( & ata_io->reg_cmd_info );
				seg = saveSeg;
				off = saveOff;
			}
			else {
				if ( ( ata_io->reg_cmd_info.totalBytesXfer + ( wordCnt << 1 ) )
					 > ata_io->reg_buffer_size ) {
					ata_io->reg_cmd_info.ec = 61;
					/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
					break;   // go to READ_DONE
				}
			}

			// Do the REP OUTSW to write the data for one DRQ block.

			ata_io->reg_cmd_info.totalBytesXfer += ( wordCnt << 1 );
			pio_drq_block_out(ata_io, CB_DATA, seg, off, wordCnt );
			
			/* DELAY400NS; */    // delay so device can get the status updated
			ata_delay400ns(ata_io);
			// Note: The drive should have dropped DATA REQUEST and
			// raised BUSY by now.
			
			// Decrement the count of sectors to be transferred
			// and increment buffer address.

			numSect = numSect - ( multiCnt ? multiCnt : 1 );
			seg = seg + ( 32 * ( multiCnt ? multiCnt : 1 ) );
		}

		// So was there any error condition?

		if ( status & ( CB_STAT_BSY | CB_STAT_DF | CB_STAT_ERR ) ) {
			ata_io->reg_cmd_info.ec = 41;
			/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
			break;   // go to WRITE_DONE
		}

		// DRQ should have been set -- was it?

		if ( ( status & CB_STAT_DRQ ) == 0 ) {
			ata_io->reg_cmd_info.ec = 42;
			/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
			break;   // go to WRITE_DONE
		}

		// Wait for interrupt -or- wait for not BUSY -or- wait for time out.

		sub_atapi_delay(ata_io, dev );
		reg_wait_poll(ata_io, 44, 45, &tmr);

		// If polling or error read the status, otherwise
		// get the status that was read by the interrupt handler.

		if ( ( ! ata_io->int_use_intr_flag ) || ( ata_io->reg_cmd_info.ec ) )
			status = pio_inbyte(ata_io, CB_STAT );
		else
			status = ata_io->int_ata_status;

		// If there was a time out error, go to WRITE_DONE.
		if ( ata_io->reg_cmd_info.ec )
			break;   // go to WRITE_DONE

		// If all of the requested sectors have been transferred, make a
		// few more checks before we exit.
		if ( numSect < 1 ) {
			// Since the drive has transferred all of the sectors without
			// error, the drive MUST not have BUSY, DEVICE FAULT, DATA REQUEST
			// or ERROR status at this time.

			if ( status & ( CB_STAT_BSY | CB_STAT_DF | 
							CB_STAT_DRQ | CB_STAT_ERR ) ) {
				ata_io->reg_cmd_info.ec = 43;
				/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
				break;   // go to WRITE_DONE
			}

			// All sectors have been written without error, go to WRITE_DONE.

			break;   // go to WRITE_DONE

		}

		//
		// This is the end of the write loop.  If we get here, the loop
		// is repeated to write the next sector.  Go back to WRITE_LOOP.

	}

	// read the output registers and trace the command.

	/* sub_trace_command(); */

	// BMCR/BMIDE Error=1?

	if ( sub_readBusMstrStatus(ata_io) & BM_SR_MASK_ERR )	{
		ata_io->reg_cmd_info.ec = 78;                  // yes
		/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
	}

	// WRITE_DONE:

	// For interrupt mode, remove interrupt handler.
	/* ignore at the moment
	   int_restore_int_vect(); */

	// mark end of PDO cmd in low level trace

	/* trc_llt( 0, 0, TRC_LLT_E_PDO ); */

	// reset reg_drq_block_call_back to NULL (0)

	ata_io->reg_drq_block_call_back = (void *) 0;

	// All done.  The return values of this function are described in
	// ATAIO.H.

	if ( ata_io->reg_cmd_info.ec )
		return 1;
	return 0;
}


/*************************************************************
 * exec_pio_data_in_cmd() - Execute a PIO Data In command.
 *
 * See ATA-2 Section 9.3, ATA-3 Section 9.3,
 * ATA-4 Section 8.6 Figure 10.
 *
 *************************************************************/
static int exec_pio_data_in_cmd(ATA_IO* ata_io, int dev,
								unsigned int seg, unsigned int off,
								long numSect, int multiCnt) {
	                            //HND_SC* hnd_sc, MY_CONSOLE* cnsl) {
	unsigned char status;
	long wordCnt;
	unsigned int saveSeg = seg;
	unsigned int saveOff = off;

	HND_TMR tmr;


	debug_varval01("exec_pio_data_in_cmd: numSect=", numSect, 0);

	// mark start of PDI cmd in low level trace
	/* trc_llt( 0, 0, TRC_LLT_S_PDI ); */

	// reset Bus Master Error bit
	sub_writeBusMstrStatus(ata_io, BM_SR_MASK_ERR );

	// Set command time out.
	/* tmr_set_timeout(); */
	my_set_timeout(&tmr, 500);

	// Select the drive - call the sub_select function.
	// Quit now if this fails.


	if ( sub_select(ata_io, dev, &tmr) ) {
		/* sub_trace_command(); */
		/* trc_llt( 0, 0, TRC_LLT_E_PDI ); */
		ata_io->reg_drq_block_call_back = (void *) 0;
		return 1;
	}

	/***************************
	 * something wrong!!
	 ***************************/

	// Set up all the registers except the command register.
	sub_setup_command(ata_io);

	// For interrupt mode, install interrupt handler.
	/* int_save_int_vect(); */

	status = pio_inbyte(ata_io, CB_ASTAT );
	//debug_varval01("before command send:astat=", status, 1);



	// Start the command by setting the Command register.  The drive
	// should immediately set BUSY status.
	
	pio_outbyte(ata_io, CB_CMD, ata_io->reg_cmd_info.cmd );


	// Waste some time by reading the alternate status a few times.
	// This gives the drive time to set BUSY in the status register on
	// really fast systems.  If we don't do this, a slow drive on a fast
	// system may not set BUSY fast enough and we would think it had
	// completed the command when it really had not even started the
	// command yet.


	status = pio_inbyte(ata_io, CB_ASTAT );
	//debug_varval01("after command sent:astat=", status, 1);


	/* DELAY400NS; */
	ata_delay400ns(ata_io);

	// Loop to read each sector.

	while ( 1 )  {
		// READ_LOOP:
		//
		// NOTE NOTE NOTE ...  The primary status register (1f7) MUST NOT be
		// read more than ONCE for each sector transferred!  When the
		// primary status register is read, the drive resets IRQ 14.  The
		// alternate status register (3f6) can be read any number of times.
		// After interrupt read the the primary status register ONCE
		// and transfer the 256 words (REP INSW).  AS SOON as BOTH the
		// primary status register has been read AND the last of the 256
		// words has been read, the drive is allowed to generate the next
		// IRQ 14 (newer and faster drives could generate the next IRQ 14 in
		// 50 microseconds or less).  If the primary status register is read
		// more than once, there is the possibility of a race between the
		// drive and the software and the next IRQ 14 could be reset before
		// the system interrupt controller sees it.

		// Wait for interrupt -or- wait for not BUSY -or- wait for time out.

		//debug_varval01("before sub_atapi_delay: int_ata_status=",
		//			  ata_io->int_ata_status, 0);

		sub_atapi_delay(ata_io, dev );

		reg_wait_poll(ata_io, 34, 35, &tmr );

		//debug_varval01("after reg_wait_poll: ec=",
		//			  ata_io->reg_cmd_info.ec, 0);
		//debug_varval01("after reg_wait_poll: int_ata_status=",
		//			  ata_io->int_ata_status, 0);

		// If polling or error read the status, otherwise
		// get the status that was read by the interrupt handler.

		if ( ( ! ata_io->int_use_intr_flag ) || (ata_io->reg_cmd_info.ec ) )
			status = pio_inbyte(ata_io, CB_STAT );
		else
			status = ata_io->int_ata_status;

		//debug_varval01("after reg_wait_poll2: status=",
		//			  status, 0);

		// If there was a time out error, go to READ_DONE.

		if (ata_io->reg_cmd_info.ec )
			break;   // go to READ_DONE

		// If BSY=0 and DRQ=1, transfer the data,
		// even if we find out there is an error later.

		if ( ( status & ( CB_STAT_BSY | CB_STAT_DRQ ) ) == CB_STAT_DRQ )  {
			// do the slow data transfer thing

			//debug_puts("slow....");

			if ( ata_io->reg_slow_xfer_flag ) {
				if ( numSect <= ata_io->reg_slow_xfer_flag ) {
					sub_xfer_delay();
					ata_io->reg_slow_xfer_flag = 0;
				}
			}

			// increment number of DRQ packets
			ata_io->reg_cmd_info.drqPackets ++ ;

			// determine the number of sectors to transfer

			wordCnt = multiCnt ? multiCnt : 1;
			if ( wordCnt > numSect )
				wordCnt = numSect;
			wordCnt = wordCnt * 256;

			// Quit if buffer overrun.
			// Adjust buffer address when DRQ block call back in use.

			if ( ata_io->reg_drq_block_call_back ) {
				if ( ( wordCnt << 1 ) > ata_io->reg_buffer_size ) {
					ata_io->reg_cmd_info.ec = 61;
					/* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
					break;   // go to READ_DONE
				}
				seg = saveSeg;
				off = saveOff;
			}
			else {
				if ((ata_io->reg_cmd_info.totalBytesXfer + ( wordCnt << 1 ))
					> ata_io->reg_buffer_size ) {
					ata_io->reg_cmd_info.ec = 61;
					/* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
					break;   // go to READ_DONE
				}
			}

			// Do the REP INSW to read the data for one DRQ block.

			ata_io->reg_cmd_info.totalBytesXfer += ( wordCnt << 1 );

			debug_varval01("pio_drq_block_in: seg", seg, 1);

			pio_drq_block_in(ata_io, CB_DATA, seg, off, wordCnt );
			

			/* DELAY400NS; */   // delay so device can get the status updated
			ata_delay400ns(ata_io);

			// Note: The drive should have dropped DATA REQUEST by now.  If there
			// are more sectors to transfer, BUSY should be active now (unless
			// there is an error).

			// Call DRQ block call back function.
			if ( ata_io->reg_drq_block_call_back ) {
				ata_io->reg_cmd_info.drqPacketSize = ( wordCnt << 1 );
				(* ata_io->reg_drq_block_call_back) ( & ata_io->reg_cmd_info );
			}

			
			  // Decrement the count of sectors to be transferred
			  // and increment buffer address.

			  numSect = numSect - ( multiCnt ? multiCnt : 1 );
			  seg = seg + ( 32 * ( multiCnt ? multiCnt : 1 ) );
		}


		// So was there any error condition?

		if ( status & ( CB_STAT_BSY | CB_STAT_DF | CB_STAT_ERR ) ) {
			ata_io->reg_cmd_info.ec = 31;
			/* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
			break;   // go to READ_DONE
		}

		// DRQ should have been set -- was it?
		if ( ( status & CB_STAT_DRQ ) == 0 ) {
			ata_io->reg_cmd_info.ec = 32;
			/* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
			break;   // go to READ_DONE
		}

		// If all of the requested sectors have been transferred, make a
		// few more checks before we exit.

		if ( numSect < 1 ) {
			// Since the drive has transferred all of the requested sectors
			// without error, the drive should not have BUSY, DEVICE FAULT,
			// DATA REQUEST or ERROR active now.

			sub_atapi_delay(ata_io, dev );
			status = pio_inbyte(ata_io, CB_STAT );

			//debug_varval01("stats=", status, 1);

			if ( status & ( CB_STAT_BSY | CB_STAT_DF | 
							CB_STAT_DRQ | CB_STAT_ERR ) ) {
				ata_io->reg_cmd_info.ec = 33;
				/* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
				break;   // go to READ_DONE
			}

			// All sectors have been read without error, go to READ_DONE.

			break;   // go to READ_DONE

		}

		// This is the end of the read loop.  If we get here, the loop is
		// repeated to read the next sector.  Go back to READ_LOOP.

	}


	  // read the output registers and trace the command.

	  /* sub_trace_command(); */

	  // BMCR/BMIDE Error=1?

	  if ( sub_readBusMstrStatus(ata_io) & BM_SR_MASK_ERR ) {
		  ata_io->reg_cmd_info.ec = 78;                  // yes
		  /* trc_llt( 0, reg_cmd_info.ec, TRC_LLT_ERROR ); */
	  }

	  // READ_DONE:

	  // For interrupt mode, remove interrupt handler.

	  /* int_restore_int_vect(); */

	  // mark end of PDI cmd in low level trace

	  /* trc_llt( 0, 0, TRC_LLT_E_PDI ); */

	  // reset reg_drq_block_call_back to NULL (0)

	  ata_io->reg_drq_block_call_back = (void *) 0;

	  // All done.  The return values of this function are described in
	  // ATAIO.H.

	  debug_varval01("reg_cmd_info.ec=", 
					 ata_io->reg_cmd_info.ec, 0);
	  if ( ata_io->reg_cmd_info.ec )
		  return 1;

	  return 0;
  }



unsigned char sub_readBusMstrStatus(ATA_IO* ata_io) {
	unsigned char x;

	/* if ( pio_bmcr_base_addr < 0x0100 ) */
	if (ata_io->bmcrBase < 0x0100)
		return 0;
   
	/* x = inportb( pio_bmcr_base_addr + BM_STATUS_REG ); */
	x = io_in8(ata_io->bmcrBase + BM_STATUS_REG);
	/* trc_llt( 0, x, TRC_LLT_R_BM_SR ); */
	return x;
}

void sub_writeBusMstrStatus(ATA_IO* ata_io, unsigned char x) {

	/* if ( pio_bmcr_base_addr < 0x0100 ) */
	if (ata_io->bmcrBase < 0x0100) 
		return;
	/* trc_llt( 0, x, TRC_LLT_W_BM_SR ); */
	/* outportb( pio_bmcr_base_addr + BM_STATUS_REG, x ); */
	io_out8(ata_io->bmcrBase + BM_STATUS_REG, x);
}

/*************************************************************
 *
 * sub_zero_return_data() -- zero the return data areas.
 *
 *************************************************************/
void sub_zero_return_data(ATA_IO* ata_io ) {
	//int ndx;

	/* couldn't understand
	   for ( ndx = 0; ndx < sizeof(reg_cmd_info ); ndx ++ ) {
	   ( (unsigned char *) & reg_cmd_info )[ndx] = 0; 

	   }
	*/

	/* might be like this. */
	my_memset(&ata_io->reg_cmd_info, 0, sizeof(ata_io->reg_cmd_info));

}


/*************************************************************
 * sub_atapi_delay() - delay for at least two ticks of the bios
 *                     timer (or at least 110ms).
 *
 * changed - waiting time = 200msec
 *************************************************************/
void sub_atapi_delay(ATA_IO* ata_io, int dev ) {

	//int ndx;
	//long lw;

	if (ata_io->reg_config_info[dev] != REG_CONFIG_TYPE_ATAPI )
		return;
	if ( ! ata_io->reg_atapi_delay_flag )
		return;

	/* original code 
	   trc_llt( 0, 0, TRC_LLT_DELAY1 );
	   for ( ndx = 0; ndx < 3; ndx ++ )
	   {
	   lw = tmr_read_bios_timer();
	   while ( lw == tmr_read_bios_timer() )
	   // do nothing ;
	   }
	*/
	fancy_wait(20);
}


/*************************************************************
 * sub_select() - function used to select a drive.
 *
 * Function to select a drive making sure that BSY=0 DRQ=0.
 *
 *************************************************************/
int sub_select(ATA_IO* ata_io, int dev, HND_TMR* tmr) {
	unsigned char status;
	int data;
	// PAY ATTENTION HERE
	// The caller may want to issue a command to a device that doesn't
	// exist (for example, Exec Dev Diag), so if we see this,
	// just select that device, skip all status checking and return.
	// We assume the caller knows what they are doing!
	if ( ata_io->reg_config_info[ dev ] < REG_CONFIG_TYPE_ATA )	{
		// select the device and return

		pio_outbyte(ata_io, CB_DH, dev ? CB_DH_DEV1 : CB_DH_DEV0 );
		/* DELAY400NS; */
		ata_delay400ns(ata_io);
		return 0;
	}

	//debug_puts("sub_select start!");
	
	// The rest of this is the normal ATA stuff for device selection
	// and we don't expect the caller to be selecting a device that
	// does not exist.
	// We don't know which drive is currently selected but we should
	// wait BSY=0 and DRQ=0. Normally both BSY=0 and DRQ=0
	// unless something is very wrong!

	/* trc_llt( 0, 0, TRC_LLT_PNBSY ); */
	while ( 1 )	{
		status = pio_inbyte(ata_io, CB_STAT );
		if ( ( status & ( CB_STAT_BSY | CB_STAT_DRQ ) ) == 0 )
			break;
		/* if ( tmr_chk_timeout() ) { */
		if (my_is_timeout(tmr)) {
			/* trc_llt( 0, 0, TRC_LLT_TOUT ); */
			ata_io->reg_cmd_info.to = 1;
			ata_io->reg_cmd_info.ec = 11;
			/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
			ata_io->reg_cmd_info.st2 = status;
			ata_io->reg_cmd_info.as2 = pio_inbyte(ata_io, CB_ASTAT );
			ata_io->reg_cmd_info.er2 = pio_inbyte(ata_io, CB_ERR );
			ata_io->reg_cmd_info.sc2 = pio_inbyte(ata_io, CB_SC );
			ata_io->reg_cmd_info.sn2 = pio_inbyte(ata_io, CB_SN );
			ata_io->reg_cmd_info.cl2 = pio_inbyte(ata_io, CB_CL );
			ata_io->reg_cmd_info.ch2 = pio_inbyte(ata_io, CB_CH );
			ata_io->reg_cmd_info.dh2 = pio_inbyte(ata_io, CB_DH );
			return 1;
		}
	}

	// Here we select the drive we really want to work with by
	// setting the DEV bit in the Drive/Head register.

	pio_outbyte(ata_io, CB_DH, dev ? CB_DH_DEV1 : CB_DH_DEV0 );

	io_out8(0x01f6, 0x00);
	data = io_in8(0x03f6);
	//debug_varval01("alstat=", data, 1);

	/* DELAY400NS; */
	ata_delay400ns(ata_io);
	/* DELAY400NS; */
	ata_delay400ns(ata_io);
	/* DELAY400NS; */
	ata_delay400ns(ata_io);
	/* DELAY400NS; */
	ata_delay400ns(ata_io);
	/* DELAY400NS; */
	ata_delay400ns(ata_io);


	data = io_in8(0x03f6);
	//debug_varval01("alstat2=", data, 1);

	/* DELAY400NS; */
	ata_delay400ns(ata_io);

	// Wait for the selected device to have BSY=0 and DRQ=0.
	// Normally the drive should be in this state unless
	// something is very wrong (or initial power up is still in
	// progress).

	/* trc_llt( 0, 0, TRC_LLT_PNBSY ); */
	while ( 1 )	{
		status = pio_inbyte(ata_io, CB_STAT );
		if ( ( status & ( CB_STAT_BSY | CB_STAT_DRQ ) ) == 0 )
			break;
		/* if ( tmr_chk_timeout() ) { */
		if (my_is_timeout(tmr)) {
			/* trc_llt( 0, 0, TRC_LLT_TOUT ); */
			ata_io->reg_cmd_info.to = 1;
			ata_io->reg_cmd_info.ec = 12;
			/* trc_llt( 0, ata_io->reg_cmd_info.ec, TRC_LLT_ERROR ); */
			ata_io->reg_cmd_info.st2 = status;
			ata_io->reg_cmd_info.as2 = pio_inbyte(ata_io, CB_ASTAT );
			ata_io->reg_cmd_info.er2 = pio_inbyte(ata_io, CB_ERR );
			ata_io->reg_cmd_info.sc2 = pio_inbyte(ata_io, CB_SC );
			ata_io->reg_cmd_info.sn2 = pio_inbyte(ata_io, CB_SN );
			ata_io->reg_cmd_info.cl2 = pio_inbyte(ata_io, CB_CL );
			ata_io->reg_cmd_info.ch2 = pio_inbyte(ata_io, CB_CH );
			ata_io->reg_cmd_info.dh2 = pio_inbyte(ata_io, CB_DH );
			return 1;
		}
	}

	// All done.  The return values of this function are described in
	// ATAIO.H.

	if ( ata_io->reg_cmd_info.ec )
		return 1;

	//debug_puts("sub_select normally end.");
	return 0;
}

//*************************************************************
//
// sub_setup_command() -- setup the command parameters
//                        in FR, SC, SN, CL, CH and DH.
//
//*************************************************************
void sub_setup_command(ATA_IO* ata_io) {
	unsigned char fr48[2];
	unsigned char sc48[2];
	unsigned char lba48[8];
	unsigned char c;
	unsigned int i;
	unsigned char dh_h; /* device head uppper 4bit */
	unsigned char dh_l; /* device head lower 4bit */

	// WARNING: THIS CODE IS DESIGNED FOR A STUPID PROCESSOR
	// LIKE INTEL X86 THAT IS Little-Endian, THAT IS, A
	// PROCESSOR THAT STORES DATA IN MEMORY IN THE WRONG
	// BYTE ORDER !!!

	//* (unsigned int *) fr48 = ata_io->reg_cmd_info.fr1;
	//* (unsigned int *) sc48 = ata_io->reg_cmd_info.sc1;
	//* (unsigned long *) ( lba48 + 4 ) = ata_io->reg_cmd_info.lbaHigh1;
	//* (unsigned long *) ( lba48 + 0 ) = ata_io->reg_cmd_info.lbaLow1;

	pio_outbyte(ata_io, CB_DC, ata_io->reg_cmd_info.dc1 );

	if ( ata_io->reg_cmd_info.lbaSize == LBA28 ) {

		// in ATA LBA28 mode
		//ata_io->reg_cmd_info.fr1 = fr48[0];
		//pio_outbyte(ata_io, CB_FR, fr48[0] );
		c = ata_io->reg_cmd_info.fr1 & 0x00ff;
		debug_varval01("fr1=", c, 1);
		pio_outbyte(ata_io, CB_FR, c);
		
		//ata_io->reg_cmd_info.sc1 = sc48[0];
		//pio_outbyte(ata_io, CB_SC, sc48[0] );
		c = ata_io->reg_cmd_info.sc1 & 0x00ff;
		debug_varval01("sc1=", c, 1);
		pio_outbyte(ata_io, CB_SC, c);

		//ata_io->reg_cmd_info.sn1 = lba48[0];
		//pio_outbyte(ata_io, CB_SN, lba48[0] );
		c = ata_io->reg_cmd_info.lbaLow1 & 0x000000ff;
		ata_io->reg_cmd_info.sn1 = c;
		debug_varval01("sn1=", c, 1);
		pio_outbyte(ata_io, CB_SN, c);

		//ata_io->reg_cmd_info.cl1 = lba48[1];
		//pio_outbyte(ata_io, CB_CL, lba48[1] );
		i = ata_io->reg_cmd_info.lbaLow1 & 0x0000ff00;
		c = i >> 8;
		ata_io->reg_cmd_info.cl1 = c;
		debug_varval01("cl1=", c, 1);
		pio_outbyte(ata_io, CB_CL, c);

		//ata_io->reg_cmd_info.ch1 = lba48[2];
		//pio_outbyte(ata_io, CB_CH, lba48[2] );
		i = ata_io->reg_cmd_info.lbaLow1 & 0x00ff0000;
		c = i >> 16;
		ata_io->reg_cmd_info.ch1 = c;
		debug_varval01("ch1=", c, 1);
		pio_outbyte(ata_io, CB_CH, c);

		/* ata_io->reg_cmd_info.dh1 = ( ata_io->reg_cmd_info.dh1 & 0xf0 ) | 
		   ( lba48[3] & 0x0f );
		   pio_outbyte(ata_io, CB_DH, ata_io->reg_cmd_info.dh1 ); */

		/* device head = 0100???? */
		dh_h = ata_io->reg_cmd_info.dh1 & 0xf0;
		i = ata_io->reg_cmd_info.lbaLow1 & 0x0f000000;
		dh_l = i >> 24;
		c = dh_h | dh_l;
		ata_io->reg_cmd_info.dh1 = c;
		debug_varval01("dh1=", c, 1);
		pio_outbyte(ata_io, CB_DH, c);
	}
	else if ( ata_io->reg_cmd_info.lbaSize == LBA48 ) {

		/**************************
		 * not modified yet!!!
		 **************************/

		// in ATA LBA48 mode
		pio_outbyte(ata_io, CB_FR, fr48[1] );
		pio_outbyte(ata_io, CB_SC, sc48[1] );
		pio_outbyte(ata_io, CB_SN, lba48[3] );
		pio_outbyte(ata_io, CB_CL, lba48[4] );
		pio_outbyte(ata_io, CB_CH, lba48[5] );
		ata_io->reg_cmd_info.fr1 = fr48[0];
		pio_outbyte(ata_io, CB_FR, fr48[0] );
		ata_io->reg_cmd_info.sc1 = sc48[0];
		pio_outbyte(ata_io, CB_SC, sc48[0] );
		ata_io->reg_cmd_info.sn1 = lba48[0];
		pio_outbyte(ata_io, CB_SN, lba48[0] );
		ata_io->reg_cmd_info.cl1 = lba48[1];
		pio_outbyte(ata_io, CB_CL, lba48[1] );
		ata_io->reg_cmd_info.ch1 = lba48[2];
		pio_outbyte(ata_io, CB_CH, lba48[2] );
		pio_outbyte(ata_io, CB_DH, ata_io->reg_cmd_info.dh1  );
	} else {

		/**************************
		 * not modified yet!!!
		 **************************/

		// in ATA CHS or ATAPI LBA32 mode
		pio_outbyte(ata_io, CB_FR, ata_io->reg_cmd_info.fr1  );
		pio_outbyte(ata_io, CB_SC, ata_io->reg_cmd_info.sc1  );
		pio_outbyte(ata_io, CB_SN, ata_io->reg_cmd_info.sn1  );
		pio_outbyte(ata_io, CB_CL, ata_io->reg_cmd_info.cl1  );
		pio_outbyte(ata_io, CB_CH, ata_io->reg_cmd_info.ch1  );
		pio_outbyte(ata_io, CB_DH, ata_io->reg_cmd_info.dh1  );
	}
}

/*************************************************************
 * sub_xfer_delay() - delay until the bios timer ticks
 *                    (from 0 to 55ms).
 * changed wait 60msec;
 *************************************************************/
void sub_xfer_delay( void ) {
   //long lw;

   /* original code
   trc_llt( 0, 0, TRC_LLT_DELAY2 );
   lw = tmr_read_bios_timer();
   while ( lw == tmr_read_bios_timer() )
   // do nothing 
   ;
   */
   fancy_wait(6);
}

/*************************************************************
 * sub_trace_command() -- trace the end of a command.
 *
 *************************************************************/
void sub_trace_command(ATA_IO* ata_io) {
	unsigned long lba;
	unsigned char sc48[2];
	unsigned char lba48[8];

	ata_io->reg_cmd_info.st2 = pio_inbyte(ata_io, CB_STAT );
	ata_io->reg_cmd_info.as2 = pio_inbyte(ata_io, CB_ASTAT );
	ata_io->reg_cmd_info.er2 = pio_inbyte(ata_io, CB_ERR );
	if ( ata_io->reg_cmd_info.lbaSize == LBA48 ) {
		// read back ATA LBA48...
		sc48[0]  = pio_inbyte(ata_io, CB_SC );
		lba48[0] = pio_inbyte(ata_io, CB_SN );
		lba48[1] = pio_inbyte(ata_io, CB_CL );
		lba48[2] = pio_inbyte(ata_io, CB_CH );
		pio_outbyte(ata_io, CB_DC, CB_DC_HOB );
		sc48[1]  = pio_inbyte(ata_io, CB_SC );
		lba48[3] = pio_inbyte(ata_io, CB_SN );
		ata_io->reg_cmd_info.sn2 = lba48[3];
		lba48[4] = pio_inbyte(ata_io, CB_CL );
		ata_io->reg_cmd_info.cl2 = lba48[4];
		lba48[5] = pio_inbyte(ata_io, CB_CH );
		ata_io->reg_cmd_info.ch2 = lba48[5];
		lba48[6] = 0;
		lba48[7] = 0;
		ata_io->reg_cmd_info.sc2 = * (unsigned int *) sc48;
		ata_io->reg_cmd_info.lbaHigh2 = * (unsigned long *) ( lba48 + 4 );
		ata_io->reg_cmd_info.lbaLow2  = * (unsigned long *) ( lba48 + 0 );
		ata_io->reg_cmd_info.dh2 = pio_inbyte(ata_io, CB_DH );
	}
	else {
		// read back ATA CHS, ATA LBA28 or ATAPI LBA32
		ata_io->reg_cmd_info.sc2 = pio_inbyte(ata_io, CB_SC );
		ata_io->reg_cmd_info.sn2 = pio_inbyte(ata_io, CB_SN );
		ata_io->reg_cmd_info.cl2 = pio_inbyte(ata_io, CB_CL );
		ata_io->reg_cmd_info.ch2 = pio_inbyte(ata_io, CB_CH );
		ata_io->reg_cmd_info.dh2 = pio_inbyte(ata_io, CB_DH );
		ata_io->reg_cmd_info.lbaHigh2 = 0;
		ata_io->reg_cmd_info.lbaLow2 = 0;
		if ( ata_io->reg_cmd_info.lbaSize == LBA28 ) {
			lba = ata_io->reg_cmd_info.dh2 & 0x0f;
			lba = lba << 8;
			lba = lba | ata_io->reg_cmd_info.ch2;
			lba = lba << 8;
			lba = lba | ata_io->reg_cmd_info.cl2;
			lba = lba << 8;
			lba = lba | ata_io->reg_cmd_info.sn2;
			ata_io->reg_cmd_info.lbaLow2 = lba;
		}
	}
	/* trc_cht(); */
}



void pio_outbyte(ATA_IO* ata_io, int addr, unsigned char data ) {

   int regAddr;

   regAddr = ata_io->pio_reg_addrs[ addr ];

   io_out8(regAddr, data);
}

unsigned char pio_inbyte(ATA_IO* ata_io, int addr ) {
   unsigned int regAddr;
   int ret;
   unsigned char uc;

   regAddr = ata_io->pio_reg_addrs[ addr ];
   ret = io_in8(regAddr);
   uc = ret;
   return uc;

}

/* now editing...  */
void pio_drq_block_out(ATA_IO* ata_io,  unsigned int addrDataReg,
		unsigned int bufSeg, unsigned int bufOff,
		long wordCnt ){

	long bCnt;
	int memDtOpt;
	unsigned int randVal;
	unsigned int dataRegAddr;
	/* DOS ....
	   unsigned int far * uip1;
	   unsigned int far * uip2;
	   unsigned char far * ucp1;
	   unsigned char far * ucp2;
	 */
	unsigned int *uip1;
	unsigned int *uip2;
	unsigned char *ucp1;
	unsigned char *ucp2;

	unsigned long bufAddr;

	// NOTE: wordCnt is the size of a DRQ data block/packet
	// in words. The maximum value of wordCnt is normally:
	// a) For ATA, 16384 words or 32768 bytes (64 sectors,
	//    only with READ/WRITE MULTIPLE commands),
	// b) For ATAPI, 32768 words or 65536 bytes
	//    (actually 65535 bytes plus a pad byte).

	// normalize bufSeg:bufOff

	bufAddr = bufSeg;
	bufAddr = bufAddr << 4;
	bufAddr = bufAddr + bufOff;

	if ( ata_io->pio_memory_seg )
	{

		// PCMCIA Memory mode data transfer.

		// set Data reg address per pio_memory_dt_opt
		dataRegAddr = 0x0000;
		memDtOpt = ata_io->pio_memory_dt_opt;
		if ( ata_io->pio_memory_dt_opt == PIO_MEMORY_DT_OPTR )
		{
			/* ignore at the moment
			randVal = * (unsigned int *) MK_FP( 0x40, 0x6c ); */
			memDtOpt = randVal % 3;
		}
		if ( memDtOpt == PIO_MEMORY_DT_OPT8 )
			dataRegAddr = 0x0008;
		if ( memDtOpt == PIO_MEMORY_DT_OPTB )
		{
			dataRegAddr = 0x0400;
			if ( ata_io->pio_memory_dt_opt == PIO_MEMORY_DT_OPTR )
				dataRegAddr = dataRegAddr | ( randVal & 0x03fe );
		}

		if ( ata_io->pio_xfer_width == 8 )
		{
			// PCMCIA Memory mode 8-bit
			bCnt = wordCnt * 2L;
			/* ignore at the moment
	         ucp2 = (unsigned char far *) MK_FP( pio_memory_seg, dataRegAddr );
			 */
			for ( ; bCnt > 0; bCnt -- )
			{
				bufSeg = (unsigned int) ( bufAddr >> 4 );
				bufOff = (unsigned int) ( bufAddr & 0x0000000fL );
				/* ignore at the moment
				ucp1 = (unsigned char far *) MK_FP( bufSeg, bufOff ); */
				* ucp2 = * ucp1;
				bufAddr += 1;
				if ( memDtOpt == PIO_MEMORY_DT_OPTB )
				{
					dataRegAddr += 1;
					dataRegAddr = ( dataRegAddr & 0x03ff ) | 0x0400;
					/* ignore at the moment
					ucp2 = (unsigned char far *) MK_FP( pio_memory_seg, dataRegAddr );
					 */
				}
			}
			//trc_llt( addrDataReg, 0, TRC_LLT_OUTSB );
		}
		else
		{
			// PCMCIA Memory mode 16-bit
			/* ignore at the moment
			uip2 = (unsigned int far *) MK_FP( pio_memory_seg, dataRegAddr ); */
			for ( ; wordCnt > 0; wordCnt -- )
			{
				bufSeg = (unsigned int) ( bufAddr >> 4 );
				bufOff = (unsigned int) ( bufAddr & 0x0000000fL );
				/* ignore at the moment
				uip1 = (unsigned int far *) MK_FP( bufSeg, bufOff ); */
				* uip2 = * uip1;
				bufAddr += 2;
				if ( memDtOpt == PIO_MEMORY_DT_OPTB )
				{
					dataRegAddr = dataRegAddr + 2;
					dataRegAddr = ( dataRegAddr & 0x03fe ) | 0x0400;
					/* ignore at the moment
					uip2 = (unsigned int far *) MK_FP( pio_memory_seg, dataRegAddr );
					*/
				}
			}
			//trc_llt( addrDataReg, 0, TRC_LLT_OUTSW );
		}
	}
	   else
	   {
	      int pxw;
	      long wc;

	      // adjust pio_xfer_width - don't use DWORD if wordCnt is odd.

	      pxw = ata_io->pio_xfer_width;
	      if ( ( pxw == 32 ) && ( wordCnt & 0x00000001L ) )
	         pxw = 16;

	      // Data transfer using OUTS instruction.
	      // Break the transfer into chunks of 32768 or fewer bytes.

	      while ( wordCnt > 0 )
	      {
	         bufOff = (unsigned int) ( bufAddr & 0x0000000fL );
	         bufSeg = (unsigned int) ( bufAddr >> 4 );
	         if ( wordCnt > 16384L )
	            wc = 16384;
	         else
	            wc = wordCnt;
	         if ( pxw == 8 )
	         {
	            // do REP OUTS
	            pio_rep_outbyte(ata_io, addrDataReg, bufSeg, bufOff, wc * 2L );
	         }
	         else
	         if ( pxw == 32 )
	         {
	            // do REP OUTSD
	            pio_rep_outdword(ata_io, addrDataReg, bufSeg, bufOff, wc / 2L );
	         }
	         else
	         {
	            // do REP OUTSW
	            pio_rep_outword(ata_io, addrDataReg, bufSeg, bufOff, wc );
	         }
	         bufAddr = bufAddr + ( wc * 2 );
	         wordCnt = wordCnt - wc;
	      }
	   }

	   return;


}


// Note: pio_drq_block_in() is the primary way perform PIO
// Data In transfers. It will handle 8-bit, 16-bit and 32-bit
// I/O based data transfers and 8-bit and 16-bit PCMCIA Memory
// mode transfers.
void pio_drq_block_in(ATA_IO* ata_io, unsigned int addrDataReg,
                       unsigned int bufSeg, unsigned int bufOff,
                       long wordCnt ) {
	long bCnt;
	int memDtOpt;
	unsigned int randVal;
	unsigned int dataRegAddr;
	/* DOS ....
	unsigned int far * uip1;
	unsigned int far * uip2;
	unsigned char far * ucp1;
	unsigned char far * ucp2;
	*/
	unsigned int *uip1;
	unsigned int *uip2;
	unsigned char *ucp1;
	unsigned char *ucp2;

	unsigned long bufAddr;

	// NOTE: wordCnt is the size of a DRQ data block/packet
	// in words. The maximum value of wordCnt is normally:
	// a) For ATA, 16384 words or 32768 bytes (64 sectors,
	//    only with READ/WRITE MULTIPLE commands),
	// b) For ATAPI, 32768 words or 65536 bytes
	//    (actually 65535 bytes plus a pad byte).

	// normalize bufSeg:bufOff

	bufAddr = bufSeg;
	bufAddr = bufAddr << 4;
	bufAddr = bufAddr + bufOff;

	/*
	debug_puts("pio_drq_block_in start");
	debug_varval01("bufAddr=", bufAddr, 1);
	debug_varval01("wordCnt=", wordCnt, 0);
    */
	
	if ( ata_io->pio_memory_seg ) {

		// PCMCIA Memory mode data transfer.

		// set Data reg address per pio_memory_dt_opt
		dataRegAddr = 0x0000;
		memDtOpt = ata_io->pio_memory_dt_opt;
		if ( ata_io->pio_memory_dt_opt == PIO_MEMORY_DT_OPTR ) {
			/* ignore at the moment
			   randVal = * (unsigned int *) MK_FP( 0x40, 0x6c ); */
			memDtOpt = randVal % 3;
		}
		if ( memDtOpt == PIO_MEMORY_DT_OPT8 )
			dataRegAddr = 0x0008;
		if ( memDtOpt == PIO_MEMORY_DT_OPTB )  {
			dataRegAddr = 0x0400;
			if ( ata_io->pio_memory_dt_opt == PIO_MEMORY_DT_OPTR )
				dataRegAddr = dataRegAddr | ( randVal & 0x03fe );
		}

		if ( ata_io->pio_xfer_width == 8 ) {
			// PCMCIA Memory mode 8-bit
			bCnt = wordCnt * 2L;
			/* ignore at the moment
			ucp1 = (unsigned char far *) MK_FP( pio_memory_seg, dataRegAddr );
			*/
			for ( ; bCnt > 0; bCnt -- ) {

				bufSeg = (unsigned int) ( bufAddr >> 4 );
				bufOff = (unsigned int) ( bufAddr & 0x0000000fL );
				/* ignore at the moment
				   ucp2 = (unsigned char far *) MK_FP( bufSeg, bufOff ); */
				* ucp2 = * ucp1;
				bufAddr += 1;
				if ( memDtOpt == PIO_MEMORY_DT_OPTB ) {
					dataRegAddr += 1;
					dataRegAddr = ( dataRegAddr & 0x03ff ) | 0x0400;
					/* ignore at the moment
					ucp1 = (unsigned char far *) MK_FP( pio_memory_seg, dataRegAddr );
					*/
				}
			}
			/* trc_llt( addrDataReg, 0, TRC_LLT_INSB ); */
		}
		else {
			// PCMCIA Memory mode 16-bit
			/* ignore at the moment
			uip1 = (unsigned int far *) MK_FP( pio_memory_seg, dataRegAddr );
			*/
			for ( ; wordCnt > 0; wordCnt -- ) {
				bufSeg = (unsigned int) ( bufAddr >> 4 );
				bufOff = (unsigned int) ( bufAddr & 0x0000000fL );
				/* ignore at the moment
				uip2 = (unsigned int far *) MK_FP( bufSeg, bufOff );
				*/
				* uip2 = * uip1;
				bufAddr += 2;
				if ( memDtOpt == PIO_MEMORY_DT_OPTB ) {
					dataRegAddr += 2;
					dataRegAddr = ( dataRegAddr & 0x03fe ) | 0x0400;
					/* ignore at the moment
					uip1 = (unsigned int far *) MK_FP( pio_memory_seg, dataRegAddr );
					*/
				}
			}
			/* trc_llt( addrDataReg, 0, TRC_LLT_INSW ); */
		}
	}
	else {
		int pxw;
		long wc;

		// adjust pio_xfer_width - don't use DWORD if wordCnt is odd.

		pxw = ata_io->pio_xfer_width;
		if ( ( pxw == 32 ) && ( wordCnt & 0x00000001L ) )
			pxw = 16;

		// Data transfer using INS instruction.
		// Break the transfer into chunks of 32768 or fewer bytes.

		while ( wordCnt > 0 ) {
			bufSeg = (unsigned int) ( bufAddr >> 4 );
			bufOff = (unsigned int) ( bufAddr & 0x0000000fL );
			if ( wordCnt > 16384L )
				wc = 16384;
			else
				wc = wordCnt;

			if ( pxw == 8 ) {
				// do REP INS
				pio_rep_inbyte(ata_io, addrDataReg, bufSeg, bufOff, wc * 2L );
			}
			else if ( pxw == 32 ) {
				// do REP INSD
				pio_rep_indword(ata_io, addrDataReg, bufSeg, bufOff, wc / 2L );
			}
			else {
				// do REP INSW
				pio_rep_inword(ata_io, addrDataReg, bufSeg, bufOff, wc );
			}



			bufAddr = bufAddr + ( wc * 2 );
			wordCnt = wordCnt - wc;
		}
	}

	return;
}


/*
 * big change!!
 * @bufOff is ignored at the moment.
 * always load at bufSeg << 4
 */
void pio_rep_inbyte(ATA_IO* ata_io, unsigned int addrDataReg,
                     unsigned int bufSeg, unsigned int bufOff,
                     long byteCnt ) {
   int dataRegAddr = ata_io->pio_reg_addrs[ addrDataReg ];
   //unsigned int bCnt = (unsigned int) byteCnt;
   int i;
   char* buff;

   //buff = 0;
   buff = (char*) (bufSeg << 4);
   for (i=0; i<byteCnt; i++) {
	   *buff = io_in8(dataRegAddr);
	   buff++;
   }

}	

/*
 * big change!!
 * @bufOff is ignored at the moment.
 * always load at bufSeg << 4
 */
void pio_rep_inword(ATA_IO* ata_io, unsigned int addrDataReg,
                     unsigned int bufSeg, unsigned int bufOff,
                     long wordCnt ) {
   int dataRegAddr = ata_io->pio_reg_addrs[ addrDataReg ];
   //unsigned int bCnt = (unsigned int) byteCnt;
   int i;
   unsigned short* buff;

   //buff = 0;
   buff = (unsigned short*) (bufSeg << 4);
   for (i=0; i<wordCnt; i++) {
	   *buff = io_in16(dataRegAddr);
	   buff++;
   }

}


/*
 * Not implemented yet!
 */
void pio_rep_indword(ATA_IO* ata_io, unsigned int addrDataReg,
					 unsigned int bufSeg, unsigned int bufOff,
					 long dwordCnt ) {

   //int dataRegAddr = ata_io->pio_reg_addrs[ addrDataReg ];
   //unsigned int bCnt = (unsigned int) byteCnt;
   //int i;
   //char* buff;

   //buff = 0;
   /*
   buff = (char*) bufSeg << 4;
   for (i=0; i<dwordCnt*4; i++) {
	   *buff = io_in8(dataRegAddr);
	   buff++;
   }
    */
   debug_puts("error - pio_rep_indword isn't implemented yet!");
}	


/*
 * big change!!
 * @bufOff is ignored at the moment.
 * always load at bufSeg << 4
 */
void pio_rep_outbyte(ATA_IO* ata_io, unsigned int addrDataReg,
                      unsigned int bufSeg, unsigned int bufOff,
                      long byteCnt )

{
   unsigned int dataRegAddr = ata_io->pio_reg_addrs[ addrDataReg ];
   //unsigned int bCnt = (unsigned int) byteCnt;

   int i;
   char* buff;

   buff = (char*) (bufSeg << 4);
   for (i=0; i<byteCnt; i++) {
	   io_out8(dataRegAddr, *buff);
	   buff++;
   }
   
}

/*
 * big change!!
 * @bufOff is ignored at the moment.
 * always load at bufSeg << 4
 */
void pio_rep_outword(ATA_IO* ata_io, unsigned int addrDataReg,
                      unsigned int bufSeg, unsigned int bufOff,
                      long wordCnt )

{
   unsigned int dataRegAddr = ata_io->pio_reg_addrs[ addrDataReg ];
   //unsigned int wCnt = (unsigned int) wordCnt;

   int i;
   unsigned short* buff;

   //buff = 0;
   buff = (unsigned short*) (bufSeg << 4);
   for (i=0; i<wordCnt; i++) {
	   io_out16(dataRegAddr, *buff);
	   buff++;
   }

}

/*
 * Not implemented yet!
 */
void pio_rep_outdword(ATA_IO* ata_io, unsigned int addrDataReg,
                       unsigned int bufSeg, unsigned int bufOff,
                       long dwordCnt )

{
   //unsigned int dataRegAddr = ata_io->pio_reg_addrs[ addrDataReg ];
   //unsigned int dwCnt = (unsigned int) dwordCnt;

   debug_puts("error - pio_rep_indword isn't implemented yet!");
}

void ata_delay400ns(ATA_IO* ata_io) {
	pio_inbyte(ata_io, CB_ASTAT ); 
	pio_inbyte(ata_io, CB_ASTAT );
	pio_inbyte(ata_io, CB_ASTAT ); 
	pio_inbyte(ata_io, CB_ASTAT ); 
}


/****************************
 * wachi original code 
 ****************************/


/*
 * initialize ATA_IO
 */
void ata_ata_io_init(ATA_IO* ata_io) {
	ata_io->int_use_intr_flag = 0; /* default value is 0 */
	ata_io->pio_memory_dt_opt = PIO_MEMORY_DT_OPT0;
	ata_io->pio_memory_seg = 0;
	ata_io->pio_xfer_width = 16;
	ata_io->reg_drq_block_call_back = 0;	/* add wachi originally */
	ata_io->reg_buffer_size = 4096;	
}

/*
 * config ATA IO address and IRQ 
 *
 * @priSec: 0: primary,  1: secondary
 */
int ata_config_ioaddr_irq(ATA_IO* ata_io, unsigned short pcicfg_bar0, 
						  unsigned short pcicfg_bar1, 
						  unsigned short pcicfg_bar4, int priSec) {

	ata_io->cmdBase = pcicfg_bar0;
	ata_io->ctrlBase = pcicfg_bar1;
	ata_io->bmcrBase = pcicfg_bar4;
			
	if ((ata_io->cmdBase == 0xffff )
		|| (ata_io->ctrlBase == 0xffff )
		|| (ata_io->bmcrBase == 0xffff )) {
				
		return -1;
	}

	ata_io->cmdBase &= 0xfffe;
	ata_io->ctrlBase &= 0xfffe;
	ata_io->bmcrBase &= 0xfffe;
	if ( ata_io->cmdBase == 0 ) {
		ata_io->cmdBase  = priSec ? 0x170 : 0x1f0;
		ata_io->ctrlBase = priSec ? 0x370 : 0x3f0;
		ata_io->irq = priSec ? 15 : 14;
	} else {
		ata_io->ctrlBase -= 4;
	}
	if ( priSec )
		ata_io->bmcrBase += 8;
	
	return 0;
}
