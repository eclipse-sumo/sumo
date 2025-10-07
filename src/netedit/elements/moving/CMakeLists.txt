set(netedit_elements_moving_SRCS
   GNEMoveElement.cpp
   GNEMoveElement.h
   GNEMoveElementConnection.cpp
   GNEMoveElementConnection.h
   GNEMoveElementCrossing.cpp
   GNEMoveElementCrossing.h
   GNEMoveElementEdge.cpp
   GNEMoveElementEdge.h
   GNEMoveElementEdgeDouble.cpp
   GNEMoveElementEdgeDouble.h
   GNEMoveElementJunction.cpp
   GNEMoveElementJunction.h
   GNEMoveElementLane.cpp
   GNEMoveElementLane.h
   GNEMoveElementLaneDouble.cpp
   GNEMoveElementLaneDouble.h
   GNEMoveElementLaneSingle.cpp
   GNEMoveElementLaneSingle.h
   GNEMoveElementShape.cpp
   GNEMoveElementShape.h
   GNEMoveElementView.cpp
   GNEMoveElementView.h
   GNEMoveOffset.cpp
   GNEMoveOffset.h
   GNEMoveOperation.cpp
   GNEMoveOperation.h
   GNEMoveResult.cpp
   GNEMoveResult.h
)

add_library(netedit_elements_moving STATIC ${netedit_elements_moving_SRCS})
set_property(TARGET netedit_elements_moving PROPERTY PROJECT_LABEL "z_netedit_elements_moving")
