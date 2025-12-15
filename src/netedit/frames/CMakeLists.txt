add_subdirectory(common)
add_subdirectory(network)
add_subdirectory(demand)
add_subdirectory(data)

set(netedit_frames_SRCS
   GNEAttributesEditor.cpp
   GNEAttributesEditor.h
   GNEAttributesEditorRow.cpp
   GNEAttributesEditorRow.h
   GNEAttributesEditorType.cpp
   GNEAttributesEditorType.h
   GNEConsecutiveSelector.cpp
   GNEConsecutiveSelector.h
   GNEDemandSelector.cpp
   GNEDemandSelector.h
   GNEDrawingShape.cpp
   GNEDrawingShape.h
   GNEElementTree.cpp
   GNEElementTree.h
   GNEFrame.cpp
   GNEFrame.h
   GNEOverlappedInspection.cpp
   GNEOverlappedInspection.h
   GNEPathCreator.cpp
   GNEPathCreator.h
   GNEPathLegendModule.cpp
   GNEPathLegendModule.h
   GNEPlanCreator.cpp
   GNEPlanCreator.h
   GNEPlanCreatorLegend.cpp
   GNEPlanCreatorLegend.h
   GNEPlanSelector.cpp
   GNEPlanSelector.h
   GNESelectorParent.cpp
   GNESelectorParent.h
   GNETLSTable.cpp
   GNETLSTable.h
   GNETagSelector.cpp
   GNETagSelector.h
   GNEViewObjectSelector.cpp
   GNEViewObjectSelector.h
)

add_library(netedit_frames STATIC ${netedit_frames_SRCS})
set_property(TARGET netedit_frames PROPERTY PROJECT_LABEL "z_netedit_frames")
