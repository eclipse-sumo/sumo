set(utils_additionals_STAT_SRCS
   AdditionalHandler.h
   AdditionalHandler.cpp
)

add_library(utils_additionals STATIC ${utils_additionals_STAT_SRCS})
set_property(TARGET utils_additionals PROPERTY PROJECT_LABEL "z_utils_additionals")
