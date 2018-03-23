set(netedit_netelements_SRCS
   GNENetElement.h
   GNENetElement.cpp
   GNEEdge.h
   GNEEdge.cpp
   GNELane.h
   GNELane.cpp
   GNEInternalLane.h
   GNEInternalLane.cpp
   GNEConnection.h
   GNEConnection.cpp
   GNECrossing.h
   GNECrossing.cpp
   GNEJunction.h
   GNEJunction.cpp
   GNEProhibition.h
   GNEProhibition.cpp
   )

add_library(netedit_netelements STATIC ${netedit_netelements_SRCS})
set_property(TARGET netedit_netelements PROPERTY PROJECT_LABEL "z_netedit_netelements")