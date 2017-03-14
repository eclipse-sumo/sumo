message("entering netgen")

#set(netgen_files
#        )
#
#add_library(ng STATIC ${netgen_files})


set(netgenexec
        netgen_main.cpp
        NGEdge.cpp NGEdge.h
        NGFrame.cpp NGFrame.h
        NGNet.cpp NGNet.h
        NGNode.cpp NGNode.h
        NGRandomNetBuilder.cpp NGRandomNetBuilder.h)

add_executable( netgen ${netgenexec})
target_link_libraries(netgen nw nb
        utgeom utoptions utxml utcommon utimportio utiodevices foreigntcpip xerces-c gdal proj
        )



