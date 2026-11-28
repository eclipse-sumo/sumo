set(utils_handlers_STAT_SRCS
   AdditionalHandler.h
   AdditionalHandler.cpp
   CommonHandler.h
   CommonHandler.cpp
   ../xml/CommonXMLStructure.cpp
   ../xml/CommonXMLStructure.h
   DataHandler.h
   DataHandler.cpp
   GeneralHandler.h
   GeneralHandler.cpp
   RouteHandler.h
   RouteHandler.cpp
   MeanDataHandler.h
   MeanDataHandler.cpp
   TemplateHandler.h
   TemplateHandler.cpp
)

# Parsed route objects use vehicle parameters. Keep the dependency out of utils_xml.
add_library(utils_handlers STATIC ${utils_handlers_STAT_SRCS})
target_link_libraries(utils_handlers PRIVATE utils_vehicle)
set_property(TARGET utils_handlers PROPERTY PROJECT_LABEL "z_utils_handlers")
