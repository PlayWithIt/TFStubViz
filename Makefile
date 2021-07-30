#############################################################################
# Linux Makefile for building: TFStubViz
#############################################################################

ifeq "$(CONFIG)" "Debug"
  CONFIG=debug
endif
ifeq "$(CONFIG)" "Release"
  CONFIG=release
endif
ifeq "$(CONFIG)" ""
  CONFIG=debug
endif

all:	build
	cd build && qmake -spec linux-g++ CONFIG+=$(CONFIG) -o Makefile ../TFStubViz.pro
	cd build && make -j4

build:
	mkdir build

clean:
	rm -rf build *.o

new:	clean all
