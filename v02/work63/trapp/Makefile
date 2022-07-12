default: all
all: build

bin/Makefile:
	mkdir -p bin
	cd bin; ln -s ../src/Makefile .

bin/trscheme/Makefile:
	mkdir -p bin/trscheme
	cd bin/trscheme; ln -s ../../src/trscheme.mak Makefile
	
bin/tredit/Makefile:
	mkdir -p bin/tredit
	cd bin/tredit; ln -s ../../src/tredit/tredit.mak Makefile

bin/tredit/test/Makefile:
	mkdir -p bin/tredit/test
	cd bin/tredit/test; ln -s ../../../src/tredit/test/tredit_test.mak Makefile

build: bin/Makefile bin/trscheme/Makefile bin/tredit/Makefile
	cd bin; make all
	cd bin/trscheme; make all
	cd bin/tredit; make all
	
test: bin/tredit/test/Makefile
	cd bin/tredit/test; make test	
	
clean:
	rm -fr bin
