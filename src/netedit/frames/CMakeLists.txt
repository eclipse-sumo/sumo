add_subdirectory(common)
add_subdirectory(network)
add_subdirectory(demand)
add_subdirectory(data)

set(netedit_frames_SRCS
   GNEFrame.h
   GNEFrame.cpp
   GNEConsecutiveSelector.cpp
   GNEConsecutiveSelector.h
   GNEDemandSelector.cpp
   GNEDemandSelector.h
   GNEDrawingShape.cpp
   GNEDrawingShape.h
   GNEElementTree.cpp
   GNEElementTree.h
   GNENetworkSelector.cpp
   GNENetworkSelector.h
   GNEOverlappedInspection.cpp
   GNEOverlappedInspection.h
   GNEPathCreator.cpp
   GNEPathCreator.h
   GNEPathLegendModule.cpp
   GNEPathLegendModule.h
   GNESelectorParent.cpp
   GNESelectorParent.h
   GNETagSelector.cpp
   GNETagSelector.h
   GNEFlowEditor.h
   GNEFlowEditor.cpp
   GNETLSTable.h
   GNETLSTable.cpp
   GNEPlanSelector.h
   GNEPlanSelector.cpp
   GNEPlanCreator.h
   GNEPlanCreator.cpp
   GNEPlanCreatorLegend.h
   GNEPlanCreatorLegend.cpp
   GNEAttributesEditorRow.h
   GNEAttributesEditorRow.cpp
   GNEAttributesEditor.h
   GNEAttributesEditor.cpp
   )

add_library(netedit_frames STATIC ${netedit_frames_SRCS})
set_property(TARGET netedit_frames PROPERTY PROJECT_LABEL "z_netedit_frames")
