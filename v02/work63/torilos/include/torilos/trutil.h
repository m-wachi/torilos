#ifndef TRUTIL_H
#define TRUTIL_H

char* sprint_varval(char* buff, char* varname, int value, int mode);
char* my_itoa(char *s, int n);
char *my_itoh(char *s, unsigned int n);
char* my_ultoa(char *s, unsigned long n);
char *my_ultoh(char *s, unsigned long n);
int my_stridx(char* s, char c);
unsigned int trutl_div4k(unsigned int v, int flg);
unsigned int trutl_mod4k(unsigned int v);
unsigned int trutl_align4k(unsigned int v);
#endif
