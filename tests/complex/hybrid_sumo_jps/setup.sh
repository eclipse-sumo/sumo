#!/bin/bash
apt-get install -y cmake
apt-get install -y libboost-dev
apt-get install -y libboost-system1.55
apt-get install -y libboost-filesystem-dev




cd
git clone https://cst.version.fz-juelich.de/jupedsim/jpscore.git
cd jpscore
git checkout as-a-service
mkdir build
cd build
cmake DCMAKE_BUILD_TYPE:STRING=Release ..
make -j4
make install
cd ../
rm -fr build

