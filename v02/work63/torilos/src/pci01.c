#include <torilos/pci01.h>
#include <torilos/myasmfunc01.h>

/*
 * Access PCI Configuration space
 * (see OSDevWiki - http://www.osdev.org/wiki/PCI)
 */
unsigned short pciConfigReadWord (unsigned short bus, unsigned short slot,
                                  unsigned short func, unsigned short offset) {
	unsigned long address;
	unsigned long lbus = (unsigned long)bus;
	unsigned long lslot = (unsigned long)slot;
	unsigned long lfunc = (unsigned long)func;
	unsigned short tmp = 0;
 
	/* create configuration address as per Figure 1 */
	address = (unsigned long)((lbus << 16) | (lslot << 11) |
							  (lfunc << 8) | (offset & 0xfc) | 
							  ((int)0x80000000));
 
	/* write out the address */
	io_out32(0xCF8, address);
	/* read in the data */
	tmp = (unsigned short)((io_in32(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	return (tmp);
}
