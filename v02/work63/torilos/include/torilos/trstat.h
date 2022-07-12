#ifndef TRSTAT_H
#define TRSTAT_H


typedef short dev_t;
typedef long time_t;

/*
 * copy of stat.h in newlib 
 */

/* It is intended that the layout of this structure not change when the
   sizes of any of the basic types change (short, int, long) [via a compile
   time option].  */

struct stat {
	dev_t st_dev;
	unsigned short st_ino;
	int st_mode;
	unsigned short st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;
	dev_t st_rdev;
	long st_size;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
};




#define	_IFMT		0170000	/* type of file */
#define		_IFDIR	0040000	/* directory */
#define		_IFCHR	0020000	/* character special */
#define		_IFBLK	0060000	/* block special */
#define		_IFREG	0100000	/* regular */
#define		_IFLNK	0120000	/* symbolic link */
#define		_IFSOCK	0140000	/* socket */
#define		_IFIFO	0010000	/* fifo */

#define	S_IFMT		_IFMT
#define	S_IFDIR		_IFDIR
#define	S_IFCHR		_IFCHR
#define	S_IFBLK		_IFBLK
#define	S_IFREG		_IFREG
#define	S_IFLNK		_IFLNK
#define	S_IFSOCK	_IFSOCK
#define	S_IFIFO		_IFIFO

#endif
