#!/bin/csh

if (-d build) then
    rm -rf build
endif

mkdir build 
pushd build
cmake ..
make

source ../setup.csh

./performance_test

popd
