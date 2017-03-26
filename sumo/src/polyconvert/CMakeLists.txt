set(polyconvert_SRCS
   polyconvert_main.cpp
   PCLoaderArcView.cpp
   PCLoaderArcView.h
   PCLoaderDlrNavteq.cpp
   PCLoaderDlrNavteq.h
   PCLoaderVisum.cpp
   PCLoaderVisum.h
   PCLoaderOSM.cpp
   PCLoaderOSM.h
   PCLoaderXML.cpp
   PCLoaderXML.h
   PCNetProjectionLoader.cpp
   PCNetProjectionLoader.h
   PCPolyContainer.cpp
   PCPolyContainer.h
   PCTypeDefHandler.cpp
   PCTypeDefHandler.h
   PCTypeMap.cpp
   PCTypeMap.h
)

add_executable(polyconvert ${polyconvert_SRCS})

target_link_libraries(polyconvert ${commonlibs} gdal)

install(TARGETS polyconvert DESTINATION bin)

#BUILT_SOURCES = pc_typemap.h
#EXTRA_DIST = pc_typemap.h
#pc_typemap.h:
#	../../tools/build/typemap.py poly
