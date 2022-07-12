#ifndef FIFO_H
#define FIFO_H

#define FLAGS_OVERRUN     0x0001;

typedef struct fifo8 {
	unsigned char *buf;
	int out, in, size, free, flags;
} FIFO8;

void fifo8_init(FIFO8* fifo, int size, unsigned char *buf);
int fifo8_put(FIFO8* fifo, unsigned char data);
int fifo8_get(FIFO8* fifo);
int fifo8_count(FIFO8* fifo);


#endif
