#ifndef PCI01_H
#define PCI01_H
unsigned short pciConfigReadWord (unsigned short bus, unsigned short slot,
                                  unsigned short func, unsigned short offset);

#endif
