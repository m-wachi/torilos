#ifndef MYLIB01_H
#define MYLIB01_H

char *my_strcat(char *dst, const char *src);
char *my_strncat(char *s1, const char *s2, int n);
char *my_strcpy(char *dst, const char *src);
char *my_strncpy(char *dst, const char *src, int n);
unsigned int my_strlen(char *s);
int my_strcmp(char *s1, char *s2);
int my_strncmp(char *s1, char *s2, unsigned int n);
char *my_strchr(const char *s, int c);
char *my_strrchr(const char *s, int c);
void *my_memset(void* dst, int c, int size);
void *my_memcpy(void *s1, const void *s2, int n);
int my_atoi(char *s);
#endif
