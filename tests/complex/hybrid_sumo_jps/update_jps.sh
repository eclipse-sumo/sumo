#!/bin/bash
#echo "jpscore test"
cd
#git clone https://cst.version.fz-juelich.de/jupedsim/jpscore.git
cd jpscore
git pull
mkdir build
cd build
cmake DCMAKE_BUILD_TYPE:STRING=Release ..
make -j4
make install
cd ../
rm -fr build 
cd
