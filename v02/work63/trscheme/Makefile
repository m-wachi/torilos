default: all
all: build

bin/Makefile:
	mkdir -p bin
	cd bin; ln -s ../src/Makefile .

build: bin/Makefile
	cd bin; make all

clean:
	rm -fr bin
