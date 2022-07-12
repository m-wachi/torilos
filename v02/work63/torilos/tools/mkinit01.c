#include <stdio.h>
#include <string.h>

#define BUFFSIZE 1024
#define TARGETSIZE 2048

int main(int argc, char** argv) {

  int i;
  long size;
  FILE *fpin;
  FILE *fpout;

  char buff[BUFFSIZE];
  size_t readsize, addsize, filesize;

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

  filesize = 0;

  while (0==feof(fpin)) {

    memset(buff, 0, sizeof(buff));
  
    readsize = fread(buff, 1, sizeof(buff), fpin);

    for (i=0; i<readsize; i++) {
      fputc(buff[i], fpout);
    }

    filesize += readsize;

  }

  fclose(fpin);

  printf("size=%d\n", filesize);

  addsize = TARGETSIZE - filesize;

  for (i=0; i<addsize; i++) {
    fputc(0, fpout);
  }

  fclose(fpout);

  return 0;
}
