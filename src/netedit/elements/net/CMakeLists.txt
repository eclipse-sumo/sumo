set(netedit_elements_net_SRCS
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

add_library(netedit_elements_net STATIC ${netedit_elements_net_SRCS})
set_property(TARGET netedit_elements_net PROPERTY PROJECT_LABEL "z_netedit_elements_net")