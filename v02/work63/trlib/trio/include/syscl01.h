#ifndef SYSCL01_H
#define SYSCL01_H

int syscall(unsigned int eax, unsigned int ebx, 
			unsigned int ecx, unsigned int edx,
			unsigned int esi, unsigned int edi,
			unsigned int ebp);

int systest(unsigned int eax, unsigned int ebx, 
		unsigned int ecx, unsigned int edx,
		unsigned int esi, unsigned int edi,
		unsigned int ebp);

#endif
