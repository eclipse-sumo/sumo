if CHECK_MEMORY_LEAKS
MEM_LIBS  = ../foreign/nvwa/libnvwa.a
endif

if TRACI
bin_PROGRAMS = TraCITestClient

TraCITestClient_SOURCES = tracitestclient_main.cpp TraCITestClient.cpp TraCITestClient.h

TraCITestClient_LDADD = ../utils/common/libcommon.a \
../utils/traci/libtraci.a ../foreign/tcpip/libtcpip.a \
$(MEM_LIBS)
endif
