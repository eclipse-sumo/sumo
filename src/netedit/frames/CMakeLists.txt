set(netedit_frames_SRCS
   GNEFrame.h
   GNEFrame.cpp
   GNEInspectorFrame.h
   GNEInspectorFrame.cpp
   GNESelectorFrame.h
   GNESelectorFrame.cpp
   GNEConnectorFrame.h
   GNEConnectorFrame.cpp
   GNETLSEditorFrame.h
   GNETLSEditorFrame.cpp
   GNEAdditionalFrame.h
   GNEAdditionalFrame.cpp
   GNECrossingFrame.h
   GNECrossingFrame.cpp
   GNEDeleteFrame.h
   GNEDeleteFrame.cpp
   GNEPolygonFrame.h
   GNEPolygonFrame.cpp
   GNEProhibitionFrame.cpp
   )

add_library(netedit_frames STATIC ${netedit_frames_SRCS})
set_property(TARGET netedit_frames PROPERTY PROJECT_LABEL "z_netedit_frames")