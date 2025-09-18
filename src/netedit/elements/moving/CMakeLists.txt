set(netedit_elements_moving_SRCS
   GNEMoveElement.cpp
   GNEMoveElement.h
)

add_library(netedit_elements_moving STATIC ${netedit_elements_moving_SRCS})
set_property(TARGET netedit_elements_moving PROPERTY PROJECT_LABEL "z_netedit_elements_moving")
