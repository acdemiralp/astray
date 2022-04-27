#!/bin/bash

if [ ! -d "build" ] ; then mkdir build ; fi
cd build
if [ ! -d "vcpkg" ] ; then git clone https://github.com/Microsoft/vcpkg.git ; fi
cd vcpkg
if [ ! -f "vcpkg" ] ; then ./bootstrap-vcpkg.sh ; fi

VCPKG_DEFAULT_TRIPLET=x64-linux
./vcpkg install --recurse --overlay-ports=../../vcpkg/overlay_ports doctest eigen3 tbb thrust
cd ..

cmake -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --target all --config Release --parallel 8
cd ..
