add_subdirectory(common)
add_subdirectory(distribution)
add_subdirectory(emissions)
add_subdirectory(geom)
add_subdirectory(importio)
add_subdirectory(iodevices)
add_subdirectory(options)
add_subdirectory(shapes)
add_subdirectory(router)
add_subdirectory(traci)
add_subdirectory(vehicle)
add_subdirectory(xml)
if (FOX_FOUND)
    add_subdirectory(foxtools)
    add_subdirectory(gui)
endif ()

if (SUMO_UTILS)
    set(xml_STAT_SRCS
        xml/GenericSAXHandler.h
        xml/GenericSAXHandler.cpp
        xml/SUMOSAXAttributes.cpp
        xml/SUMOSAXAttributes.h
        xml/SUMOSAXAttributesImpl_Binary.cpp
        xml/SUMOSAXAttributesImpl_Binary.h
        xml/SUMOSAXAttributesImpl_Xerces.cpp
        xml/SUMOSAXAttributesImpl_Xerces.h
        xml/SUMOSAXAttributesImpl_Cached.cpp
        xml/SUMOSAXAttributesImpl_Cached.h
        xml/SUMOSAXHandler.cpp
        xml/SUMOSAXHandler.h
        xml/SUMOSAXReader.cpp
        xml/SUMOSAXReader.h
        xml/SUMOXMLDefinitions.cpp
        xml/SUMOXMLDefinitions.h
        xml/SAXWeightsHandler.cpp
        xml/SAXWeightsHandler.h
        xml/XMLSubSys.cpp
        xml/XMLSubSys.h
    )
    set(utillibs utils_distribution utils_shapes utils_options
        utils_geom utils_common utils_importio utils_iodevices foreign_tcpip ${PROJ_LIBRARY})
    add_library(sumoutils SHARED ${xml_STAT_SRCS})
    install(TARGETS sumoutils DESTINATION lib)
    if (MSVC OR APPLE)
        target_link_libraries(sumoutils ${utillibs})
    else ()
        target_link_libraries(sumoutils -Wl,--whole-archive ${utillibs} -Wl,--no-whole-archive)
    endif()
endif()
