default: all
all: float.h limits.h stdarg.h stddef.h syslimits.h

GCC_INCLUDE_DIR = /usr/lib/gcc/i486-linux-gnu/4.1.2/include

%.h:
	cp -p $(GCC_INCLUDE_DIR)/$*.h .
	