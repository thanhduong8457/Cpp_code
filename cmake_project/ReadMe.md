mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_STANDARD=14 -DCMAKE_CXX_COMPILER=...
cmake --build .
