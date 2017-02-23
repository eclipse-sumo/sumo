message("entering xml")

set(xml_files
        GenericSAXHandler.h GenericSAXHandler.cpp
        SUMOSAXAttributes.cpp SUMOSAXAttributes.h
        SUMOSAXAttributesImpl_Binary.cpp SUMOSAXAttributesImpl_Binary.h
        SUMOSAXAttributesImpl_Xerces.cpp SUMOSAXAttributesImpl_Xerces.h
        SUMORouteHandler.cpp SUMORouteHandler.h
        SUMORouteLoader.cpp SUMORouteLoader.h
        SUMORouteLoaderControl.cpp SUMORouteLoaderControl.h
        SUMOSAXHandler.cpp SUMOSAXHandler.h
        SUMOSAXReader.cpp SUMOSAXReader.h
        SUMOVehicleParserHelper.cpp SUMOVehicleParserHelper.h
        SUMOXMLDefinitions.cpp SUMOXMLDefinitions.h
        SAXWeightsHandler.cpp SAXWeightsHandler.h
        XMLSubSys.cpp XMLSubSys.h
        SUMOSAXAttributesImpl_Cached.cpp SUMOSAXAttributesImpl_Cached.h)

add_library(utxml STATIC ${xml_files})
