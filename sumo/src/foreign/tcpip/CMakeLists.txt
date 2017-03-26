set(foreign_tcpip_STAT_SRCS
   socket.h
   socket.cpp
   storage.h
   storage.cpp
)

add_library(foreign_tcpip STATIC ${foreign_tcpip_STAT_SRCS})
