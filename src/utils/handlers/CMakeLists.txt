set(utils_handlers_STAT_SRCS
   AdditionalHandler.h
   AdditionalHandler.cpp
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

add_library(utils_handlers STATIC ${utils_handlers_STAT_SRCS})
set_property(TARGET utils_handlers PROPERTY PROJECT_LABEL "z_utils_handlers")
