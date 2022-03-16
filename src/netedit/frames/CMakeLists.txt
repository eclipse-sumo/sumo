add_subdirectory(common)
add_subdirectory(network)
add_subdirectory(demand)
add_subdirectory(data)

set(netedit_frames_SRCS
   GNEFrame.h
   GNEFrame.cpp
   GNEFrameModules.h
   GNEFrameModules.cpp
   GNEFrameAttributeModules.h
   GNEFrameAttributeModules.cpp
   GNEM_ConsecutiveLaneSelector.cpp
   GNEM_ConsecutiveLaneSelector.h
   GNEM_DemandElementSelector.cpp
   GNEM_DemandElementSelector.h
   GNEM_DrawingShape.cpp
   GNEM_DrawingShape.h
   GNEM_HierarchicalElementTree.cpp
   GNEM_HierarchicalElementTree.h
   GNEM_NetworkElementSelector.cpp
   GNEM_NetworkElementSelector.h
   GNEM_OverlappedInspection.cpp
   GNEM_OverlappedInspection.h
   GNEM_PathCreator.cpp
   GNEM_PathCreator.h
   GNEM_PathLegend.cpp
   GNEM_PathLegend.h
   GNEM_SelectorParent.cpp
   GNEM_SelectorParent.h
   GNEM_TagSelector.cpp
   GNEM_TagSelector.h
   )

add_library(netedit_frames STATIC ${netedit_frames_SRCS})
set_property(TARGET netedit_frames PROPERTY PROJECT_LABEL "z_netedit_frames")