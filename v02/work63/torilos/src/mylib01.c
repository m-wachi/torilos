#include <torilos/mylib01.h>


char *my_strcat(char *dst, const char *src) {
    char  *p = dst;

    while (*dst) dst++;           
    while ((*dst++ = *src++)) ;
    return (p);
}

char *my_strncat(char *s1, const char *s2, int n) {
    char  *p = s1;

    while (*s1) s1++;
    while (n--) {
        if (!(*s1++ = *s2++)) break;
    }
    *s1 = '\0';
    return (p);
}

char *my_strcpy(char *dst, const char *src) {
    char *p = dst;

    while ((*dst++ = *src++))
        ;
    return (p);
}

char *my_strncpy(char *dst, const char *src, int n) {
    char  *p = dst;
    while (n) {
        n--;
        if (!(*dst++ = *src++)) break;    
    }
    while (n--)
        *dst++ = '\0';
    return (p);
}

unsigned int my_strlen(char *s) {
    unsigned int len = 0;

    while (*s++)
        len++;
    return (len);
}

int my_strcmp(char *s1, char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2)         /* not equal */
            return ((unsigned char)*s1 - (unsigned char)*s2);
        s1++;
        s2++;
    }
    if (*s1) return 1;
	if (*s2) return -1;
    return 0;
}

int my_strncmp(char *s1, char *s2, unsigned int n) {
	if (n == 0)
		return 0;

	while (n-- != 0 && *s1 == *s2)	{
		if (n == 0 || *s1 == '\0' || *s2 == '\0')
			break;
		s1++;
		s2++;
	}

	return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}

char *my_strchr(const char *s, int c) {
    c = (char)c;
    while (*s != c) {
        if (*s == '\0')             /* not found */
            return (0);
        s++;
    }
    return ((char *)s);
}

char *my_strrchr(const char *s, int c)
{
    char* p;
    
    p = 0;

    c = (char)c;
    while (1) {
        if (*s == c)
            p = (char*)s;
        if (*s == '\0')
            break;
        s++;
    }
    return (p);
}

void *my_memset(void* dst, int c, int size) {

    const unsigned char uc = c;
    unsigned char *p = (unsigned char *)dst;

    while (size-- > 0)
        *p++ = uc;

    return (dst);
}

void *my_memcpy(void *s1, const void *s2, int n) {
    char        *p1 = (char *)s1;
    const char  *p2 = (const char *)s2;

    while (n-- > 0) {
        *p1 = *p2;
        p1++;
        p2++;
    }
    return (s1);
}

int my_atoi(char *s) {
	int i, l, val;
	l = my_strlen(s);
	val = 0;
	for (i=0; i<l; i++) {
		val = val * 10 + s[i] - 48; 
	}
	return val;
}
