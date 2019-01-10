#!/bin/bash
cd
cd gRPC/sumo 
svn up
make -f Makefile.cvs
./configure
make -j4
make install
make clean 
cd

