#ifndef TRBLKBUFMDL_H
#define TRBLKBUFMDL_H

#include "trblkbuf.h"

int tblbm_init(unsigned char* buff, int sz_buf, int sz_block);
TBLBF_BLKCELL* tblbm_get_blkcell(unsigned long blkno);


#endif
