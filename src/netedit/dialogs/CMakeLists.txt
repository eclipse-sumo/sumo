add_subdirectory(tools)
add_subdirectory(options)

set(netedit_dialogs_SRCS
   GNEAbout.cpp
   GNEAbout.h
   GNEAdditionalDialog.cpp
   GNEAdditionalDialog.h
   GNEAllowVClassesDialog.cpp
   GNEAllowVClassesDialog.h
   GNECalibratorDialog.cpp
   GNECalibratorDialog.h
   GNECalibratorFlowDialog.cpp
   GNECalibratorFlowDialog.h
   GNEDemandElementDialog.cpp
   GNEDemandElementDialog.h
   GNEDialogACChooser.cpp
   GNEDialogACChooser.h
   GNEFixAdditionalElements.cpp
   GNEFixAdditionalElements.h
   GNEFixDemandElements.cpp
   GNEFixDemandElements.h
   GNEFixElementsDialog.cpp
   GNEFixElementsDialog.h
   GNEFixElementsDialogRow.cpp
   GNEFixElementsDialogRow.h
   GNEFixNetworkElements.cpp
   GNEFixNetworkElements.h
   GNEGeometryPointDialog.cpp
   GNEGeometryPointDialog.h
   GNEHelpDialog.cpp
   GNEHelpDialog.h
   GNEKeepElementsDialog.cpp
   GNEKeepElementsDialog.h
   GNEMultipleParametersDialog.cpp
   GNEMultipleParametersDialog.h
   GNERerouterDialog.cpp
   GNERerouterDialog.h
   GNERerouterIntervalDialog.cpp
   GNERerouterIntervalDialog.h
   GNERouteDialog.cpp
   GNERouteDialog.h
   GNESingleParametersDialog.cpp
   GNESingleParametersDialog.h
   GNEUndoListDialog.cpp
   GNEUndoListDialog.h
   GNEVTypeDistributionsDialog.cpp
   GNEVTypeDistributionsDialog.h
   GNEVariableSpeedSignDialog.cpp
   GNEVehicleTypeDialog.cpp
   GNEVehicleTypeDialog.h
   GNEVariableSpeedSignDialog.h
   )

add_library(netedit_dialogs STATIC ${netedit_dialogs_SRCS})
set_property(TARGET netedit_dialogs PROPERTY PROJECT_LABEL "z_netedit_dialogs")
add_dependencies(netedit_dialogs generate-version-h)
