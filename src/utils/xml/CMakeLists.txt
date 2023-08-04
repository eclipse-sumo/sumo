set(utils_xml_STAT_SRCS
   CommonXMLStructure.cpp
   CommonXMLStructure.h
   GenericHandler.cpp
   GenericHandler.h
   GenericSAXHandler.cpp
   GenericSAXHandler.h
   IStreamInputSource.h
   NamespaceIDs.cpp
   NamespaceIDs.h
   SAXWeightsHandler.cpp
   SAXWeightsHandler.h
   SUMOSAXAttributes.cpp
   SUMOSAXAttributes.h
   SUMOSAXAttributesImpl_Cached.cpp
   SUMOSAXAttributesImpl_Cached.h
   SUMOSAXAttributesImpl_Xerces.cpp
   SUMOSAXAttributesImpl_Xerces.h
   SUMOSAXHandler.cpp
   SUMOSAXHandler.h
   SUMOSAXReader.cpp
   SUMOSAXReader.h
   SUMOXMLDefinitions.cpp
   SUMOXMLDefinitions.h
   XMLSubSys.cpp
   XMLSubSys.h
)

add_library(utils_xml STATIC ${utils_xml_STAT_SRCS})
set_property(TARGET utils_xml PROPERTY PROJECT_LABEL "z_utils_xml")
