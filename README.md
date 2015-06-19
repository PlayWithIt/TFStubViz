TFStubViz
=========

A Qt based GUI for the software emulator for Tinkerforge Hardware
(see https://github.com/PlayWithIt/TFStubserver)

In order to build this project you'll need the TFStubserver project too.

Requirements:
-------------
* Linux
* g++ 4.6.3 or higher (as on Raspberry PI, OpenSuse 13.1)
* clang++ 3.3 (llvm) or higher should work too
* Qt 5.4

Compile:
--------
* be sure that ``qmake`` is in your PATH and points to the correct Qt version
* run ``make`` in the base directory for a debug-build
* start ``build/TFStubViz``
