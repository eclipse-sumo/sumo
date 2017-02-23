message("entering tcpip")

set(tcpip_files
	socket.h socket.cpp storage.h storage.cpp)

add_library(foreigntcpip STATIC ${tcpip_files})
