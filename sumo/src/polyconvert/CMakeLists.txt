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
   pc_typemap.h
)

add_executable(polyconvert ${polyconvert_SRCS})
set_target_properties(polyconvert PROPERTIES OUTPUT_NAME_DEBUG polyconvertD)
target_link_libraries(polyconvert ${commonlibs} ${GDAL_LIBRARY})

add_custom_command(OUTPUT pc_typemap.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/build/typemap.py poly
                   )
