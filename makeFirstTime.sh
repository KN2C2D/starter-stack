#!/bin/bash
set -e
cd Lib
autoreconf -i
automake --add-missing
./configure CXXFLAGS='-std=c++03' --prefix=`pwd|sed 's/...$//'`/Agent/Lib
make -j8
make install 
cd ../Agent
autoreconf -i
automake --add-missing
./configure CXXFLAGS='-std=c++03' --with-librcsc=`pwd`/Lib/
make -j8

