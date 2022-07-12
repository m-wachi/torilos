#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFSIZE 1024

void my_itoa(char*, int);


int main(int argc, char** argv) {

  int i;
  long size;
  FILE *fpin;
  FILE *fpout;

  char buff[BUFFSIZE];
  size_t readsize, addsize, filesize;

  char* test01 = "12";
  int targetsize;

  /*
    memset(buff, 0, sizeof(buff));

    my_itoa(buff, i);

    printf("buff=%s\n", buff);
  */

  if (argc!=4) {
    puts("not enough arguments");
    return 0;
  }
  
  targetsize = atoi(argv[3]);

  printf("in=%s, out=%s, targetsize=%d\n", argv[1], argv[2], targetsize);

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

  if (filesize > targetsize) {
    puts("too much target size!!");
    exit(-1);
  }

  addsize = targetsize - filesize;

  for (i=0; i<addsize; i++) {
    fputc(0, fpout);
  }

  fclose(fpout);

  return 0;
}

void my_itoa(char *s, int n) {
  
  int a = n;
  int b, cnt, i;
  char *p;
  char buff[100];

  p = s;
  cnt = 0;
  while(a != 0) {
    b = a % 10;

    b += 0x30;

    buff[cnt++] = (char) b;

    a /= 10;
  }
    
  for(i=cnt-1; i>-1; i--) {
    *p = buff[i];
    p++;
  }
}
