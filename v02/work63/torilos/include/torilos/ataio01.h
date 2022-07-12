#ifndef ATAIO01_H
#define ATAIO01_H

#include "myutil01.h"

/************************************************
 * This code is based on ATADRVR
 * (ata-atapi.com)
 ************************************************/

//**************************************************************
//
// Global defines -- ATA register and register bits.
// command block & control block regs
//
//**************************************************************

// These are the offsets into pio_reg_addrs[]

#define CB_DATA  0   // data reg         in/out pio_base_addr1+0
#define CB_ERR   1   // error            in     pio_base_addr1+1
#define CB_FR    1   // feature reg         out pio_base_addr1+1
#define CB_SC    2   // sector count     in/out pio_base_addr1+2
#define CB_SN    3   // sector number    in/out pio_base_addr1+3
#define CB_CL    4   // cylinder low     in/out pio_base_addr1+4
#define CB_CH    5   // cylinder high    in/out pio_base_addr1+5
#define CB_DH    6   // device head      in/out pio_base_addr1+6
#define CB_STAT  7   // primary status   in     pio_base_addr1+7
#define CB_CMD   7   // command             out pio_base_addr1+7
#define CB_ASTAT 8   // alternate status in     pio_base_addr2+6
#define CB_DC    8   // device control      out pio_base_addr2+6
#define CB_DA    9   // device address   in     pio_base_addr2+7

// error reg (CB_ERR) bits

#define CB_ER_ICRC 0x80    // ATA Ultra DMA bad CRC
#define CB_ER_BBK  0x80    // ATA bad block
#define CB_ER_UNC  0x40    // ATA uncorrected error
#define CB_ER_MC   0x20    // ATA media change
#define CB_ER_IDNF 0x10    // ATA id not found
#define CB_ER_MCR  0x08    // ATA media change request
#define CB_ER_ABRT 0x04    // ATA command aborted
#define CB_ER_NTK0 0x02    // ATA track 0 not found
#define CB_ER_NDAM 0x01    // ATA address mark not found

#define CB_ER_P_SNSKEY 0xf0   // ATAPI sense key (mask)
#define CB_ER_P_MCR    0x08   // ATAPI Media Change Request
#define CB_ER_P_ABRT   0x04   // ATAPI command abort
#define CB_ER_P_EOM    0x02   // ATAPI End of Media
#define CB_ER_P_ILI    0x01   // ATAPI Illegal Length Indication

// ATAPI Interrupt Reason bits in the Sector Count reg (CB_SC)

#define CB_SC_P_TAG    0xf8   // ATAPI tag (mask)
#define CB_SC_P_REL    0x04   // ATAPI release
#define CB_SC_P_IO     0x02   // ATAPI I/O
#define CB_SC_P_CD     0x01   // ATAPI C/D

// bits 7-4 of the device/head (CB_DH) reg

#define CB_DH_LBA  0x40    // LBA bit
#define CB_DH_DEV0 0x00    // select device 0
#define CB_DH_DEV1 0x10    // select device 1
// #define CB_DH_DEV0 0xa0    // select device 0 (old definition)
// #define CB_DH_DEV1 0xb0    // select device 1 (old definition)

// status reg (CB_STAT and CB_ASTAT) bits

#define CB_STAT_BSY  0x80  // busy
#define CB_STAT_RDY  0x40  // ready
#define CB_STAT_DF   0x20  // device fault
#define CB_STAT_WFT  0x20  // write fault (old name)
#define CB_STAT_SKC  0x10  // seek complete (only SEEK command)
#define CB_STAT_SERV 0x10  // service (overlap/queued commands)
#define CB_STAT_DRQ  0x08  // data request
#define CB_STAT_CORR 0x04  // corrected (obsolete)
#define CB_STAT_IDX  0x02  // index (obsolete)
#define CB_STAT_ERR  0x01  // error (ATA)
#define CB_STAT_CHK  0x01  // check (ATAPI)

// device control reg (CB_DC) bits

#define CB_DC_HOB    0x80  // High Order Byte (48-bit LBA)
#define CB_DC_HD15   0x00  // bit 3 is reserved
// #define CB_DC_HD15   0x08  // (old definition of bit 3)
#define CB_DC_SRST   0x04  // soft reset
#define CB_DC_NIEN   0x02  // disable interrupts

// BMCR/BMIDE registers and bits

#define BM_COMMAND_REG    0            // offset to BM command reg
#define BM_CR_MASK_READ    0x00           // read from memory
#define BM_CR_MASK_WRITE   0x08           // write to memory
#define BM_CR_MASK_START   0x01           // start transfer
#define BM_CR_MASK_STOP    0x00           // stop transfer

#define BM_STATUS_REG     2            // offset to BM status reg
#define BM_SR_MASK_SIMPLEX 0x80           // simplex only
#define BM_SR_MASK_DRV1    0x40           // drive 1 can do dma
#define BM_SR_MASK_DRV0    0x20           // drive 0 can do dma
#define BM_SR_MASK_INT     0x04           // INTRQ signal asserted
#define BM_SR_MASK_ERR     0x02           // error
#define BM_SR_MASK_ACT     0x01           // active

#define BM_PRD_ADDR_LOW   4            // offset to prd addr reg low 16 bits
#define BM_PRD_ADDR_HIGH  6            // offset to prd addr reg high 16 bits

//**************************************************************
//
// Most mandtory and optional ATA commands
//
//**************************************************************

#define CMD_CFA_ERASE_SECTORS            0xC0
#define CMD_CFA_REQUEST_EXT_ERR_CODE     0x03
#define CMD_CFA_TRANSLATE_SECTOR         0x87
#define CMD_CFA_WRITE_MULTIPLE_WO_ERASE  0xCD
#define CMD_CFA_WRITE_SECTORS_WO_ERASE   0x38
#define CMD_CHECK_POWER_MODE1            0xE5
#define CMD_CHECK_POWER_MODE2            0x98
#define CMD_DEVICE_RESET                 0x08
#define CMD_EXECUTE_DEVICE_DIAGNOSTIC    0x90
#define CMD_FLUSH_CACHE                  0xE7
#define CMD_FLUSH_CACHE_EXT              0xEA
#define CMD_FORMAT_TRACK                 0x50
#define CMD_IDENTIFY_DEVICE              0xEC
#define CMD_IDENTIFY_DEVICE_PACKET       0xA1
#define CMD_IDENTIFY_PACKET_DEVICE       0xA1
#define CMD_IDLE1                        0xE3
#define CMD_IDLE2                        0x97
#define CMD_IDLE_IMMEDIATE1              0xE1
#define CMD_IDLE_IMMEDIATE2              0x95
#define CMD_INITIALIZE_DRIVE_PARAMETERS  0x91
#define CMD_INITIALIZE_DEVICE_PARAMETERS 0x91
#define CMD_NOP                          0x00
#define CMD_PACKET                       0xA0
#define CMD_READ_BUFFER                  0xE4
#define CMD_READ_DMA                     0xC8
#define CMD_READ_DMA_EXT                 0x25
#define CMD_READ_DMA_QUEUED              0xC7
#define CMD_READ_DMA_QUEUED_EXT          0x26
#define CMD_READ_MULTIPLE                0xC4
#define CMD_READ_MULTIPLE_EXT            0x29
#define CMD_READ_SECTORS                 0x20
#define CMD_READ_SECTORS_EXT             0x24
#define CMD_READ_VERIFY_SECTORS          0x40
#define CMD_READ_VERIFY_SECTORS_EXT      0x42
#define CMD_RECALIBRATE                  0x10
#define CMD_SEEK                         0x70
#define CMD_SET_FEATURES                 0xEF
#define CMD_SET_MULTIPLE_MODE            0xC6
#define CMD_SLEEP1                       0xE6
#define CMD_SLEEP2                       0x99
#define CMD_SMART                        0xB0
#define CMD_STANDBY1                     0xE2
#define CMD_STANDBY2                     0x96
#define CMD_STANDBY_IMMEDIATE1           0xE0
#define CMD_STANDBY_IMMEDIATE2           0x94
#define CMD_WRITE_BUFFER                 0xE8
#define CMD_WRITE_DMA                    0xCA
#define CMD_WRITE_DMA_EXT                0x35
#define CMD_WRITE_DMA_QUEUED             0xCC
#define CMD_WRITE_DMA_QUEUED_EXT         0x36
#define CMD_WRITE_MULTIPLE               0xC5
#define CMD_WRITE_MULTIPLE_EXT           0x39
#define CMD_WRITE_SECTORS                0x30
#define CMD_WRITE_SECTORS_EXT            0x34
#define CMD_WRITE_VERIFY                 0x3C

#define CMD_SRST                          256   // fake command code for Soft Reset

#define REG_CONFIG_TYPE_NONE  0
#define REG_CONFIG_TYPE_UNKN  1
#define REG_CONFIG_TYPE_ATA   2
#define REG_CONFIG_TYPE_ATAPI 3

// command history trace entry types used by reg_cmd_info.flg,
// trc_get_cmd_name(), etc.
#define TRC_FLAG_EMPTY 0
#define TRC_FLAG_SRST  1
#define TRC_FLAG_ATA   2
#define TRC_FLAG_ATAPI 3

// command types (protocol types) used by reg_cmd_info.ct,
// trc_cht_types(), etc.  this is a bit shift value.
// NOTE: also see trc_get_type_name() in ATAIOTRC.C !
#define TRC_TYPE_ALL    16    // trace all cmd types
#define TRC_TYPE_NONE    0    // don't trace any cmd types
#define TRC_TYPE_ADMAI   1    // ATA DMA In
#define TRC_TYPE_ADMAO   2    // ATA DMA Out
#define TRC_TYPE_AND     3    // ATA PIO Non-Data
#define TRC_TYPE_APDI    4    // ATA PIO Data In
#define TRC_TYPE_APDO    5    // ATA PIO Data Out
#define TRC_TYPE_ASR     6    // ATA Soft Reset
#define TRC_TYPE_PDMAI   7    // ATAPI DMA In
#define TRC_TYPE_PDMAO   8    // ATAPI DMA Out
#define TRC_TYPE_PND     9    // ATAPI PIO Non-Data
#define TRC_TYPE_PPDI   10    // ATAPI PIO Data In
#define TRC_TYPE_PPDO   11    // ATAPI PIO Data Out

#define PIO_MEMORY_DT_OPT0 0  // use Data reg at offset 0H
#define PIO_MEMORY_DT_OPT8 1  // use Data reg at offset 8H
#define PIO_MEMORY_DT_OPTB 2  // use Data reg at offsets 400-7ffH
#define PIO_MEMORY_DT_OPTR 3  // randomly select these options


struct REG_CMD_INFO {
	// entry type, flag and command code
	unsigned char flg;         // see TRC_FLAG_xxx in ataio.h
	unsigned char ct;          // see TRC_TYPE_xxx in ataio.h
	unsigned char cmd;         // command code
	// before regs
	//unsigned int  fr1;         
	unsigned short fr1;        // feature (8 or 16 bits)
	//unsigned int  sc1;       
	unsigned short sc1;        // sec cnt (8 or 16 bits)
   unsigned char sn1;         // sec num
   unsigned char cl1;         // cyl low
   unsigned char ch1;         // cyl high
   unsigned char dh1;         // device head
   unsigned char dc1;         // device control
   // after regs
   unsigned char st2;         // status reg
   unsigned char as2;         // alt status reg
   unsigned char er2;         // error reg
   unsigned int  sc2;         // sec cnt (8 or 16 bits)
   unsigned char sn2;         // sec num
   unsigned char cl2;         // cyl low
   unsigned char ch2;         // cyl high
   unsigned char dh2;         // device head
   // driver error codes
   unsigned char ec;          // detailed error code
   unsigned char to;          // not zero if time out error
   // additional result info
   long totalBytesXfer;       // total bytes transfered
   long drqPackets;           // number of PIO DRQ packets
   long drqPacketSize;        // number of bytes in current DRQ block
   unsigned int failbits;     // failure bits (protocol errors)
      #define FAILBIT15 0x8000   // extra interrupts detected
      #define FAILBIT14 0x4000
      #define FAILBIT13 0x2000
      #define FAILBIT12 0x1000
      #define FAILBIT11 0x0800
      #define FAILBIT10 0x0400
      #define FAILBIT9  0x0200
      #define FAILBIT8  0x0100   // SC( CD/IO bits) wrong at end of cmd
      #define FAILBIT7  0x0080   // byte count odd at data packet xfer time
      #define FAILBIT6  0x0040   // byte count wrong at data packet xfer time
      #define FAILBIT5  0x0020   // SC (IO bit) wrong at data packet xfer time
      #define FAILBIT4  0x0010   // SC (CD bit) wrong at data packet xfer time
      #define FAILBIT3  0x0008   // byte count wrong at cmd packet xfer time
      #define FAILBIT2  0x0004   // SC wrong at cmd packet xfer time
      #define FAILBIT1  0x0002   // got interrupt before cmd packet xfer
      #define FAILBIT0  0x0001   // slow setting BSY=1 or DRQ=1 after AO cmd
   // sector count, multiple count, and CHS/LBA info
   long ns;                   // number of sectors (sector count)
   int mc;                    // multiple count
   unsigned char lbaSize;     // size of LBA used
      #define LBACHS 0           // last command used CHS
      #define LBA28  28          // last command used 28-bit LBA
      #define LBA32  32          // last command used 32-bit LBA (Packet)
      #define LBA48  48          // last command used 48-bit LBA
      #define LBA64  64          // future use?
	//unsigned long lbaLow1;     // lower 32-bits of LBA before
	unsigned int lbaLow1;     // lower 32-bits of LBA before
	//unsigned long lbaHigh1;    // upper 32-bits of LBA before
	unsigned int lbaHigh1;    // upper 32-bits of LBA before
	//unsigned long lbaLow2;     // lower 32-bits of LBA after
	unsigned int lbaLow2;     // lower 32-bits of LBA after
	//unsigned long lbaHigh2;    // upper 32-bits of LBA after
	unsigned int lbaHigh2;    // upper 32-bits of LBA after
} ;



/*
 * wachi original
 */
typedef struct ata_io {
	unsigned short cmdBase;
	unsigned short ctrlBase;
	unsigned short bmcrBase;
	int irq;
	int reg_config_info[2];
	struct REG_CMD_INFO reg_cmd_info;
	int reg_atapi_delay_flag;
	long reg_slow_xfer_flag;
	long reg_buffer_size;

	int int_use_intr_flag;    // use INT mode if != 0
	int int_intr_flag;   // interrupt flag, incremented
	                     // for each device interrupt
	unsigned char int_ata_status;    // ATA status
	unsigned int int_bmcr_addr;   // BMCR/BMIDE status reg i/o address
	unsigned char int_bm_status;     // BMCR/BMIDE status
	void ( * reg_drq_block_call_back ) ( struct REG_CMD_INFO * );

	unsigned short pio_reg_addrs[10];
	int pio_memory_dt_opt;
	unsigned int pio_memory_seg;
	int pio_xfer_width;
} ATA_IO;


unsigned char sub_readBusMstrStatus(ATA_IO* ata_io);
void sub_writeBusMstrStatus(ATA_IO* ata_io, unsigned char x);
void sub_zero_return_data(ATA_IO* ata_io );
void sub_atapi_delay(ATA_IO* ata_io, int dev );
int sub_select(ATA_IO* ata_io, int dev, HND_TMR* tmr);
void sub_setup_command(ATA_IO* ata_io);
void sub_xfer_delay( void );
void sub_trace_command(ATA_IO* ata_io);

void pio_set_iobase_addr(ATA_IO* ata_io);
void pio_outbyte(ATA_IO* ata_io, int addr, unsigned char data);
unsigned char pio_inbyte(ATA_IO* ata_io, int addr );
void pio_rep_inbyte(ATA_IO* ata_io, unsigned int addrDataReg,
					unsigned int bufSeg, unsigned int bufOff,
					long byteCnt );
void pio_rep_inword(ATA_IO* ata_io, unsigned int addrDataReg,
                     unsigned int bufSeg, unsigned int bufOff,
                     long wordCnt );
void pio_rep_indword(ATA_IO* ata_io, unsigned int addrDataReg,
					 unsigned int bufSeg, unsigned int bufOff,
					 long dwordCnt );

void pio_rep_outbyte(ATA_IO* ata_io,  unsigned int addrDataReg,
                      unsigned int bufSeg, unsigned int bufOff,
                      long byteCnt );

void pio_rep_outword(ATA_IO* ata_io, unsigned int addrDataReg,
                      unsigned int bufSeg, unsigned int bufOff,
                      long wordCnt );

void pio_rep_outdword(ATA_IO* ata_io, unsigned int addrDataReg,
                       unsigned int bufSeg, unsigned int bufOff,
                       long dwordCnt );

/* dummy function (not implemented yet.) */
void pio_drq_block_out(ATA_IO* ata_io, unsigned int addrDataReg,
                        unsigned int bufSeg, unsigned int bufOff,
                        long wordCnt );


void pio_drq_block_in(ATA_IO* ata_io, unsigned int addrDataReg,
                       unsigned int bufSeg, unsigned int bufOff,
                       long wordCnt );

int reg_config(ATA_IO* ata_io);
int reg_reset(ATA_IO* ata_io, int skipFlag, int devRtrn );
int reg_pio_data_in_lba28(ATA_IO* ata_io, int dev, int cmd,
						  unsigned int fr, unsigned int sc,
						  unsigned long lba,
						  unsigned int seg, unsigned int off,
						  long numSect, int multiCnt);

int reg_pio_data_out_lba28(ATA_IO* ata_io, int dev, int cmd,
                            unsigned int fr, unsigned int sc,
                            unsigned long lba,
                            unsigned int seg, unsigned int off,
                            long numSect, int multiCnt );


void ata_delay400ns(ATA_IO* ata_io);
void ata_ata_io_init(ATA_IO* ata_io);
int ata_config_ioaddr_irq(ATA_IO* ata_io, unsigned short pcicfg_bar0, 
						  unsigned short pcicfg_bar1, 
						  unsigned short pcicfg_bar4, int priSec);


#endif
