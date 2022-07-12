#ifndef MYLIB01_H
#define MYLIB01_H

char* my_itoa(char *s, int n);
char* my_itoh(char *s, unsigned int n);
char* my_ultoa(char *s, unsigned long n);
char *my_ultoh(char *s, unsigned long n);
char *my_strcat(char *dst, const char *src);
char *my_strcpy(char *dst, const char *src);
char *my_strncpy(char *dst, const char *src, int n);
unsigned int my_strlen(char *s);
int my_strcmp(char *s1, char *s2);
void *my_memset(void* dst, int c, int size);
void *my_memcpy(void *s1, const void *s2, int n);

#endif
