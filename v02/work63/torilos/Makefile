all: bin/Makefile
	cd tools; make
	cd fonttool; make
	cd fat12tol; make
	cd bin; make all
	cd ext2fswork2; make
	
clean:
	cd tools; make clean
	cd fonttool; make clean
	cd fat12tol; make clean
	cd bin; make clean
	cd ext2fswork2; make clean
	cd ext2fswork; make clean
	cd mkdisk; make clean
	
disk:
	cd mkdisk; make

run:
	cd bin; make run

bin/Makefile:
	mkdir -p bin
	cd bin; ln -s ../src/Makefile .

