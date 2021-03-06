/* This code came from linux linux/dirent.h */
#ifndef _LINUX_DIRENT_H
#define _LINUX_DIRENT_H

typedef long __kernel_off_t;	//add m-wachi(not original) 

struct dirent {
	long		d_ino;
	__kernel_off_t	d_off;
	unsigned short	d_reclen;
	char		d_name[256]; /* We must not include limits.h! */
};

/* dirent64 is never refered (m-wachi)
struct dirent64 {
	__u64		d_ino;
	__s64		d_off;
	unsigned short	d_reclen;
	unsigned char	d_type;
	char		d_name[256];
};
*/

#ifdef __KERNEL__

struct linux_dirent64 {
	u64		d_ino;
	s64		d_off;
	unsigned short	d_reclen;
	unsigned char	d_type;
	char		d_name[0];
};

#endif	/* __KERNEL__ */


#endif
