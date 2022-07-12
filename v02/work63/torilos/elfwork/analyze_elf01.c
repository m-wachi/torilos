#include <stdio.h>
#include <stdlib.h>
#include <string.h>

	
#define EI_NIDENT	16


typedef unsigned int	Elf32_Addr;
typedef unsigned short	Elf32_Half;
typedef unsigned int 	Elf32_Off;
typedef int             Elf32_Sword;
typedef unsigned int	Elf32_Word;


typedef struct elf32_hdr{
  unsigned char	e_ident[EI_NIDENT];
  Elf32_Half	e_type;
  Elf32_Half	e_machine;
  Elf32_Word	e_version;
  Elf32_Addr	e_entry;  /* Entry point */
  Elf32_Off	e_phoff;
  Elf32_Off	e_shoff;
  Elf32_Word	e_flags;
  Elf32_Half	e_ehsize;
  Elf32_Half	e_phentsize;
  Elf32_Half	e_phnum;
  Elf32_Half	e_shentsize;
  Elf32_Half	e_shnum;
  Elf32_Half	e_shstrndx;
} Elf32_Ehdr;


typedef struct elf32_phdr{
  Elf32_Word	p_type;
  Elf32_Off	p_offset;
  Elf32_Addr	p_vaddr;
  Elf32_Addr	p_paddr;
  Elf32_Word	p_filesz;
  Elf32_Word	p_memsz;
  Elf32_Word	p_flags;
  Elf32_Word	p_align;
} Elf32_Phdr;

int main(int argc, char** argv) {
	FILE *fpin;
	char buff[512];
	size_t readsize;
	static unsigned char elfmagic[5] = " ELF";
	int i;
	Elf32_Ehdr elfhdr;
	Elf32_Phdr phdr;

	elfmagic[0] = 0x7f;

	if (argc != 2) {
		puts("error: not enough argument.");
		exit(1);
	}
	puts("Hello");

	if (NULL == (fpin = fopen(argv[1], "rb"))) {
		printf("Couldn't open file: %s.n", argv[1]);
		return -1;
	}

	readsize = fread(&elfhdr, 1, sizeof(elfhdr), fpin);

	if(strncmp(elfhdr.e_ident, elfmagic, 4)) {
		printf("error: %s is not elf binary\n", argv[1]);
		return -2;
	}


	printf("e_ident[0]=0x%x\n", elfhdr.e_ident[0]);

	printf("entry=0x%x\n", elfhdr.e_entry);

	printf("program header entry size=0x%x, count=%d, offset=%d\n", 
		   elfhdr.e_phentsize, elfhdr.e_phnum, elfhdr.e_phoff);
	
	for(i=0; i<elfhdr.e_phnum; i++) {
		readsize = fread(&phdr, 1, sizeof(phdr), fpin);
		printf("phdr %d: type=0x%x, offset=0x%x, viraddr=0x%x, flags=%d, filesz=%#x, memsz=%#x\n",
			   i, phdr.p_type, phdr.p_offset, phdr.p_vaddr, phdr.p_flags,
			   phdr.p_filesz, phdr.p_memsz);
	}
	fclose(fpin);
}
