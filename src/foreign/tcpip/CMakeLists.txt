set(foreign_tcpip_STAT_SRCS
   socket.h
   socket.cpp
   storage.h
   storage.cpp
)
if (HAVE_BOOST_ASIO)
    list(APPEND foreign_tcpip_STAT_SRCS
        BoostSocket.h
        BoostSocket.cpp
    )
endif ()

add_library(foreign_tcpip STATIC ${foreign_tcpip_STAT_SRCS})
set_property(TARGET foreign_tcpip PROPERTY PROJECT_LABEL "z_foreign_tcpip")
if (HAVE_BOOST_ASIO)
    target_link_libraries(foreign_tcpip PUBLIC ${BOOST_LIBRARIES})
endif ()
if (SUMO_UTILS)
    install(TARGETS foreign_tcpip DESTINATION lib COMPONENT runtime)
endif ()
