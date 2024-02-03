set(netedit_frames_network_SRCS
   GNEAdditionalFrame.cpp
   GNEAdditionalFrame.h
   GNEConnectorFrame.cpp
   GNEConnectorFrame.h
   GNECreateEdgeFrame.cpp
   GNECreateEdgeFrame.h
   GNECrossingFrame.cpp
   GNECrossingFrame.h
   GNEDecalFrame.cpp
   GNEDecalFrame.h
   GNEProhibitionFrame.cpp
   GNEProhibitionFrame.h
   GNEShapeFrame.cpp
   GNEShapeFrame.h
   GNETAZFrame.cpp
   GNETAZFrame.h
   GNETLSEditorFrame.cpp
   GNETLSEditorFrame.h
   GNEWireFrame.cpp
   GNEWireFrame.h
   )

add_library(netedit_frames_network STATIC ${netedit_frames_network_SRCS})
set_property(TARGET netedit_frames_network PROPERTY PROJECT_LABEL "z_netedit_frames_network")
