#!/bin/csh

setenv CPUPROFILE "performance_test.prof"

setenv g++ /opt/homebrew/Cellar/gcc/14.1.0/bin/g++-14 
setenv gcc /opt/homebrew/Cellar/gcc/14.1.0/bin/gcc-14 

setenv LD_LIBRARY_PATH ""
setenv LD_LIBRARY_PATH "/opt/homebrew/opt/gperftools/lib:$LD_LIBRARY_PATH"
setenv LD_LIBRARY_PATH "/opt/homebrew/opt/gperftools/lib:$LD_LIBRARY_PATH"

