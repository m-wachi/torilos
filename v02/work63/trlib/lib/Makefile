SUBLIBS = ../string/libtrstr.a ../trio/libtrio01.a ../reent/libtrreent.a \
	../sys/sysvi386/libsysvi386.a ../stdlib/libtrstdlib.a ../trutil/libtrutil.a \
	../stdio/libtrstdio.a ../locale/libtrlocale.a ../ctype/libtrctype.a
	
default: all
all: libtrc.a libtrm.a
	
libtrc.a: $(SUBLIBS)
	rm -f $@
	for i in $(SUBLIBS) ; do \
		ar x $$i ; \
	done ;
	ar rc $@ *.o
	rm -f *.o
	
clean:
	rm -f libtrc.a libtrm.a

libtrm.a: ../libm/math/libtrm.a
	cp ../libm/math/libtrm.a .
