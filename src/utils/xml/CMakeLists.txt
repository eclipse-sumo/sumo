set(utils_xml_STAT_SRCS
   GenericSAXHandler.h
   GenericSAXHandler.cpp
   SUMOSAXAttributes.cpp
   SUMOSAXAttributes.h
   SUMOSAXAttributesImpl_Binary.cpp
   SUMOSAXAttributesImpl_Binary.h
   SUMOSAXAttributesImpl_Xerces.cpp
   SUMOSAXAttributesImpl_Xerces.h
   SUMOSAXAttributesImpl_Cached.cpp
   SUMOSAXAttributesImpl_Cached.h
   SUMORouteHandler.cpp
   SUMORouteHandler.h
   SUMORouteLoader.cpp
   SUMORouteLoader.h
   SUMORouteLoaderControl.cpp
   SUMORouteLoaderControl.h
   SUMOSAXHandler.cpp
   SUMOSAXHandler.h
   SUMOSAXReader.cpp
   SUMOSAXReader.h
   SUMOVehicleParserHelper.cpp
   SUMOVehicleParserHelper.h
   SUMOXMLDefinitions.cpp
   SUMOXMLDefinitions.h
   SAXWeightsHandler.cpp
   SAXWeightsHandler.h
   XMLSubSys.cpp
   XMLSubSys.h
)

add_library(utils_xml STATIC ${utils_xml_STAT_SRCS})
set_property(TARGET utils_xml PROPERTY PROJECT_LABEL "z_utils_xml")
