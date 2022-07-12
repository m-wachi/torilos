#include <torilos/fifo01.h>

void fifo8_init(FIFO8* fifo, int size, unsigned char *buf) {
	fifo->size = size;
	fifo->buf = buf;
	fifo->out = fifo->in = 0;
	fifo->flags = 0;
	fifo->free = size;
}


int fifo8_put(FIFO8* fifo, unsigned char data) {

	if (0 == fifo->free) {
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}

	fifo->buf[fifo->in++] = data;

	if (fifo->in >= fifo->size) {
		fifo->in = 0;
	}
	fifo->free--;
	return 0;
}

int fifo8_get(FIFO8* fifo) {

	int data;

	/* return -1 if empty */
	if (fifo->size <= fifo->free) {
		return -1; 
	}

	data = fifo->buf[fifo->out++];

	if (fifo->out >= fifo->size) {
		fifo->out = 0;
	}
	fifo->free++;
	return data;
}

int fifo8_count(FIFO8* fifo) {
	return fifo->size - fifo->free;
}
