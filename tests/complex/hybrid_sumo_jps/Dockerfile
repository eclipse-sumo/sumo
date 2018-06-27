From debian:jessie

RUN apt-get update &&\
	apt-get install -y build-essential git subversion curl \
	unzip autoconf libtool cmake libboost-dev pkg-config \
	libxerces-c-dev python libboost-dev libboost-system1.55 \
	libboost-filesystem-dev libgdal-dev  &&\
	apt-get clean &&\
	cd &&\
	git clone https://github.com/google/protobuf.git &&\
        cd protobuf &&\
        git checkout d5fb408ddc281ffcadeb08699e65bb694656d0bd &&\
        ./autogen.sh &&\
        ./configure &&\
        make -j4 &&\
        make install &&\
	ldconfig &&\
        cd &&\
        rm -r protobuf &&\
        git clone https://github.com/grpc/grpc.git &&\
        cd grpc &&\
        git checkout 78e04bbd09f2c27c8168ce9c975c65cf1833cedb &&\
        git submodule update --init &&\
        make -j4 &&\
        make install &&\
        cd &&\
	rm -fr grpc &&\
	git clone https://github.com/OSGeo/proj.4.git &&\
	cd proj.4 &&\
	./autogen.sh &&\
	./configure &&\
	make -j4 &&\
	make install &&\
	cd &&\
	rm -fr proj.4 &&\
	svn checkout https://svn.code.sf.net/p/sumo/code/branches/gRPC &&\
	cd gRPC/sumo &&\
	make -f Makefile.cvs &&\
	./configure &&\
	make -j4 &&\
	make install &&\
	make clean &&\
	cd &&\
	git clone https://cst.version.fz-juelich.de/jupedsim/jpscore.git &&\
	cd jpscore &&\
	git checkout as-a-service &&\
	mkdir build &&\
	cd build &&\
	cmake DCMAKE_BUILD_TYPE:STRING=Release .. &&\
	make -j4 &&\
	make install &&\
	cd ../ &&\
	rm -fr build &&\
	ldconfig
