#ifndef TRATAIOMDL_H
#define TRATAIOMDL_H

void tatam_init();
void tatam_waitready(int mode);
int tatam_isbsy(unsigned char stat);
int tatam_isdrdy(unsigned char stat);
int tatam_readblock(unsigned char* dst, unsigned long lba);
int tatam_writeblock(unsigned char* src, unsigned long lba);

#endif
