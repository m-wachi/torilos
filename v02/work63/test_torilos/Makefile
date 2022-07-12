default: all

all: bin/Makefile
	cd bin; make all
	
clean:
	rm -fr bin
	
bin/Makefile:
	mkdir -p bin
	cd bin; ln -s ../src/Makefile .

test: all
	cd src; ./runtest.sh
	