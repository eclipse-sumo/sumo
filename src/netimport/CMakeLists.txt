add_subdirectory(vissim)

set(netimport_STAT_SRCS
   NIFrame.cpp
   NIFrame.h
   NILoader.cpp
   NILoader.h
   NIImporter_ArcView.cpp
   NIImporter_ArcView.h
   NIImporter_DlrNavteq.cpp
   NIImporter_DlrNavteq.h
   NIImporter_ITSUMO.cpp
   NIImporter_ITSUMO.h
   NIImporter_MATSim.cpp
   NIImporter_MATSim.h
   NIImporter_OpenDrive.cpp
   NIImporter_OpenDrive.h
   NIImporter_OpenStreetMap.cpp
   NIImporter_OpenStreetMap.h
   NIImporter_RobocupRescue.cpp
   NIImporter_RobocupRescue.h
   NIImporter_SUMO.cpp
   NIImporter_SUMO.h
   NIImporter_VISUM.cpp
   NIImporter_VISUM.h
   NIVisumTL.cpp
   NIVisumTL.h
   NIXMLConnectionsHandler.cpp
   NIXMLConnectionsHandler.h
   NIXMLEdgesHandler.cpp
   NIXMLEdgesHandler.h
   NIXMLNodesHandler.cpp
   NIXMLNodesHandler.h
   NIXMLTypesHandler.cpp
   NIXMLTypesHandler.h
   NIXMLPTHandler.cpp
   NIXMLPTHandler.h
   NIXMLShapeHandler.cpp
   NIXMLShapeHandler.h
   NIXMLTrafficLightsHandler.cpp
   NIXMLTrafficLightsHandler.h
   NINavTeqHelper.cpp
   NINavTeqHelper.h
   typemap.h
)

#set_source_files_properties(${CMAKE_BINARY_DIR}/src/netimport/typemap.h PROPERTIES GENERATED TRUE)

set_source_files_properties(typemap.h PROPERTIES GENERATED TRUE)

add_library(netimport STATIC ${netimport_STAT_SRCS})
set_property(TARGET netimport PROPERTY PROJECT_LABEL "z_netimport")

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/src/netimport/typemap.h
                   COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/build/typemap.py ${CMAKE_BINARY_DIR}/src
                   )
#add_custom_target(generate-typemap-h DEPENDS ${CMAKE_BINARY_DIR}/src/netimport/typemap.h)
#set_property(TARGET generate-typemap-h PROPERTY FOLDER "CMake")
#set_source_files_properties(${CMAKE_BINARY_DIR}/src/netimport/typemap.h PROPERTIES GENERATED TRUE)
