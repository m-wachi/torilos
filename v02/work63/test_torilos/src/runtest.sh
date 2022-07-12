#!/bin/bash
cd ../bin

ERRCNT=0
ERRPRG=""

make test_file
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1` 
	ERRPRG="$ERRPRG test_file" 
fi

make test_blkbuf
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1` 
	ERRPRG="$ERRPRG test_blkbuf" 
fi

make test_read
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_read" 
fi

make test_getblk
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_getblk" 
fi

make test_ext2fs
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_ext2fs" 
fi

make test_util
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_util" 
fi

make test_namei
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_namei" 
fi

make test_getdents
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_getdents" 
fi

make test_mkdir
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_mkdir" 
fi

make test_truncate
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_truncate" 
fi

make test_tty
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_tty" 
fi

make test_tty_mdl
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_tty_mdl" 
fi

make test_mempage
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_mempage" 
fi

make test_mempage_mdl
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_mempage_mdl" 
fi

make test_task
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_trtask" 
fi

make test_write
if [ 0 -ne $? ];
then
	ERRCNT=`expr $ERRCNT + 1`
	ERRPRG="$ERRPRG test_write" 
fi


if [ 0 -eq $ERRCNT ];
then
	echo
	echo "=== All Test passed. ==="
	echo
else
	echo "!!! $ERRCNT Tests Error!!!"
	echo "Error tests are $ERRPRG ."
fi
	