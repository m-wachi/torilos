/**
 * @file trataiomdl.c
 * @brief ATA I/O module (middle layer)
 */
#include <torilos/mylib01.h>
#include <torilos/trataiomdl.h>
#include <torilos/ataio01.h>
#include <torilos/debug01.h>
#include <torilos/myasmfunc01.h>
#include <torilos/pci01.h>
#include <torilos/mymm01.h>
#include <torilos/trfile.h>
#include <torilos/trblkbufmdl.h>

static ATA_IO ml_ata_io;

static unsigned char* ml_blkbuf;


void tatam_init() {

	int priSec, numDev, rc, data;
	int i, dev = 0;
	unsigned short vendor_id, device_id, uss_data;
	unsigned short pcicfg_bar0, pcicfg_bar1, pcicfg_bar4;
	char buff[512];
	char * devTypeStr[]
		= { "NO DEVICE", "UNKNOWN TYPE", "ATA", "ATAPI" };

	unsigned short *p_sh_addr0;
	unsigned short sh_idev_buf[256];

	unsigned short nof_logical_cylinders = 0;
	unsigned short nof_logical_heads = 0;
	unsigned short nof_logical_sectors = 0;
	unsigned short capabilities = 0;

	ata_ata_io_init(&ml_ata_io);

	/* priSec is always 0 at the moment */
	priSec = 0;

	/*
	 * Access PCI Configuration space 
	 */
	vendor_id = pciConfigReadWord(0, 1, 1, 0);
	debug_varval01("vendor_id=", vendor_id, 1);
			
	device_id = pciConfigReadWord(0, 1, 1, 2);
	debug_varval01("device_id=", device_id, 1);

	uss_data = pciConfigReadWord(0, 1, 1, 4);
	debug_varval01("command=", uss_data, 1);

	uss_data = pciConfigReadWord(0, 1, 1, 6);
	debug_varval01("status=", uss_data, 1);

	pcicfg_bar0 = pciConfigReadWord(0, 1, 1, 0x10);
	debug_varval01("bar0l=", pcicfg_bar0, 1);

	pcicfg_bar1 = pciConfigReadWord(0, 1, 1, 0x14);
	debug_varval01("bar1l=", pcicfg_bar1, 1);

	pcicfg_bar4 = pciConfigReadWord(0, 1, 1, 0x20);
	debug_varval01("bar4l=", pcicfg_bar4, 1);

	uss_data = pciConfigReadWord(0, 1, 1, 0x22);
	debug_varval01("bar4h=", uss_data, 1);

	rc = ata_config_ioaddr_irq(&ml_ata_io, pcicfg_bar0, pcicfg_bar1, 
							   pcicfg_bar4, priSec);
	if(rc)
		debug_puts("PCI information invalid !");

	debug_varval01("ATA CMD base ", ml_ata_io.cmdBase, 1);
	debug_varval01("ATA CTRL base ", ml_ata_io.ctrlBase, 1);
	debug_varval01("ATA BMCR base ", ml_ata_io.bmcrBase, 1);
	debug_varval01("ATA IRQ ", ml_ata_io.irq, 0);

	/* not use at the moment. 
	if ( ! irqNum ) {
		irqNum = GetPciWord( pciBus, pciDev, pciFun, 0x3c );
		irqNum &= 0x00ff;
		printf( "IRQ defaulted to %d\n", irqNum );
	}
	*/

	/*---------- ATADRVR initialization
	 *
	 * 1) must tell the driver what the I/O port addresses.
	 *    note that the driver also supports, buff2[512] all the PCMCIA
	 *    PC Card modes (I/O and memory).
	 */
	pio_set_iobase_addr(&ml_ata_io);


	/*
	 * 2) find out what devices are present -- this is the step
	 * many driver writers ignore.  You really can't just do
	 * resets and commands without first knowing what is out there.
	 * Even if you don't care the driver does care.
	 */
	numDev = reg_config(&ml_ata_io);

	debug_varval01("Found devices=", numDev, 0);
	my_strcpy(buff, "dev 0 is ");
	my_strcat(buff, (char*)devTypeStr[ ml_ata_io.reg_config_info[0] ]);
	my_strcat(buff, ", dev 1 is ");
	my_strcat(buff, (char*)devTypeStr[ ml_ata_io.reg_config_info[1] ]);
	my_strcat(buff, ".");

	debug_puts(buff);



	//---------- Try some commands in "polling" mode
	
	debug_puts("Polling mode...");

	// do an ATA soft reset (SRST) and return the command block
	// regs for device 0 in struct reg_cmd_info
	/* ClearTrace(); */
	debug_puts("Soft Reset...");
	rc = reg_reset(&ml_ata_io, 0, dev );
	if ( rc )
		debug_puts("Error!!");
	debug_puts("Soft Reset... OK.");

	data = io_in8(0x01f7);
	debug_varval01("IO-0x01f7: ", data, 1);

	data = pio_inbyte(&ml_ata_io, CB_STAT);
	debug_varval01("CB_STAT: ", data, 1);

	//ata_io.int_ata_status = 0;

	debug_varval01("before reg_pio_data_in_lba28: int_ata_status=",
				  ml_ata_io.int_ata_status, 0);


	debug_varval01("call_back=", (int)ml_ata_io.reg_drq_block_call_back, 1);


	debug_puts("IDENTIFY DEVICE...");
	rc = reg_pio_data_in_lba28(&ml_ata_io, dev, CMD_IDENTIFY_DEVICE,
							   0, 0,
							   0L,
							   //FP_SEG( bufferPtr ), FP_OFF( bufferPtr ),
							   0, 0,
							   1L, 0);
	if ( rc )
	   //ShowAll();
		debug_puts("Error!!");
	debug_puts("IDENTIFY DEVICE...OK.");


	data = pio_inbyte(&ml_ata_io, CB_STAT);
	debug_varval01("CB_STAT: ", data, 1);

	
	p_sh_addr0 = (unsigned short*)0;

	for(i=0; i<sizeof(sh_idev_buf); i++) {
		sh_idev_buf[i] = *p_sh_addr0++;
	}

	
	nof_logical_cylinders = sh_idev_buf[1];

	nof_logical_heads = sh_idev_buf[3];

	nof_logical_sectors = sh_idev_buf[6];

	capabilities = sh_idev_buf[49];

	debug_varval01("ATA # logical cylinders=", 
				  nof_logical_cylinders, 0);

	debug_varval01("ATA # logical heads=", 
				  nof_logical_heads, 0);
	
	debug_varval01("ATA # logical sectors=", 
				  nof_logical_sectors, 0);
	
	debug_varval01("ATA # capabilities=", 
				  capabilities, 1);

	debug_varval01("totalBytesXfer=", 
				  ml_ata_io.reg_cmd_info.totalBytesXfer, 0);

	//fancy_wait(300);

	tatam_waitready(1);

	//TODO These block-buffer initialization procedure should be moved other module.
	ml_blkbuf = (unsigned char*) mm_malloc_pg_knl(4096 * 16); //64 blocks
	//rc = tblbf_init_old(ml_blkbuf, 16);
	//rc = tblbf_init(ml_blkbuf, 16, 1024);
	rc = tblbm_init(ml_blkbuf, 16, 1024);
	
	if (rc) {
		debug_puts("Block-Buffer initializing error.");
	}
	
	debug_puts("ATA-Device has been initialized.");

}

/*
 * wait until BSY:OFF, DRDY:ON
 * 
 * @mode 1: read CB_STAT, 0: read CB_ASTAT
 */
void tatam_waitready(int mode) {
	int reg;
	unsigned char data;
	
	reg = mode ? CB_STAT : CB_ASTAT;

	if (reg == CB_STAT)
		debug_puts("wait by CB_STAT");
	else
		debug_puts("wait by CB_ASTAT");
		
	while(1) {
		data = pio_inbyte(&ml_ata_io, reg);
		if (tatam_isbsy(data))
			fancy_wait(10);
		else if (tatam_isdrdy(data))
			break;
	}

}

int tatam_isbsy(unsigned char stat) {
	if (0x80 & stat)
		return 1;
	return 0;
}

int tatam_isdrdy(unsigned char stat) {
	if (0x40 & stat)
		return 1;
	return 0;
}

int tatam_readblock(unsigned char* dst, unsigned long lba) {
	int rc;
	int dev = 0;

	rc = reg_pio_data_in_lba28(&ml_ata_io, dev, CMD_READ_SECTORS,
							   0, 
							   2,   //sector count
							   lba, //LBA
							   0, 0,
							   2, //numSect
							   0);

	my_memcpy(dst, (char*)0, 1024);

	return rc;
}
/**
 * @brief write block-data to disk
 * 
 * @param src data to write
 * @param lba lba (not block-no, block-no * 2)
 * @return 0: success, !=0: error
 */
int tatam_writeblock(unsigned char* src, unsigned long lba) {
	int rc;
	int dev = 0;

	my_memcpy((char*)0, src, 1024);
	
	rc = reg_pio_data_out_lba28(&ml_ata_io, dev, CMD_WRITE_SECTORS,
							   0, 
							   2,   //sector count
							   lba, //LBA
							   0, 0,
							   2, //numSect
							   0);

	return rc;
}

