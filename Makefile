#############################################################################
# Linux Makefile for building: TFStubViz
#############################################################################

all:	build
	cd build && qmake -spec linux-g++ CONFIG+=debug -o Makefile ../TFStubViz.pro
	cd build && make -j4

build:
	mkdir build

clean:
	rm -rf build *.o

new:	clean all
