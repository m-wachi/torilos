#include <stdio.h>
#include <string.h>

#define BUFFSIZE 512
#define TARGETSIZE 430

long get_file_size(char*);

int main(int argc, char** argv) {

  char s[] = "Hello\n";
  char fname[] = "iplbody01.img";
  char s2[7];
  int i;
  long size;
  FILE *fpin;
  FILE *fpout;

  char buff[BUFFSIZE];
  size_t readsize, addsize;

  if (argc!=3) {
    puts("not enough arguments");
    return 0;
  }
  
  printf("in=%s, out=%s\n", argv[1], argv[2]);

  if (NULL == (fpin = fopen(argv[1], "rb"))) {
    printf("Couldn't open file: %s.n", argv[1]);
    return -1;
  }
  
  if (NULL == (fpout = fopen(argv[2], "wb"))) {
    printf("Couldn't open file: %s.n", argv[2]);
    return -1;
  }
  
  memset(buff, 0, sizeof(buff));
  
  readsize = fread(buff, 1, sizeof(buff), fpin);
  printf("size=%d\n", readsize);

  fclose(fpin);

  addsize = TARGETSIZE - readsize;

  for (i=0; i<readsize; i++) {
    fputc(buff[i], fpout);
  }

  for (i=0; i<addsize; i++) {
    fputc(0, fpout);
  }

  fputc(0x55, fpout);
  fputc(0xaa, fpout);

  fclose(fpout);

  return 0;
}
long get_file_size(char* filename) {
  FILE *fp;

  if (NULL == (fp = fopen(filename, "rb")))
    return -1;

  fseek(fp, 0, SEEK_END);

  return ftell(fp);
}
