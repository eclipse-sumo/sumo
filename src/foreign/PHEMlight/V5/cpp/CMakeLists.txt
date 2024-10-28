set(foreign_phemlight_V5_STAT_SRCS
   CEP.cpp
   CEP.h
   CEPHandler.cpp
   CEPHandler.h
   Constants.cpp
   Constants.h
   Correction.cpp
   Correction.h
   Helpers.cpp
   Helpers.h
)

add_library(foreign_phemlight_V5 STATIC ${foreign_phemlight_V5_STAT_SRCS})
set_property(TARGET foreign_phemlight_V5 PROPERTY PROJECT_LABEL "z_foreign_phemlight_V5")
