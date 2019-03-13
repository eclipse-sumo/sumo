set(netedit_dialogs_SRCS
   GNEVariableSpeedSignDialog.h
   GNEVariableSpeedSignDialog.cpp
   GNEDialog_About.h
   GNEDialog_About.cpp
   GNEDialog_AllowDisallow.h
   GNEDialog_AllowDisallow.cpp
   GNEGenericParameterDialog.h
   GNEGenericParameterDialog.cpp
   GNEDialog_FixAdditionalElements.h
   GNEDialog_FixAdditionalElements.cpp
   GNEDialog_FixDemandElements.h
   GNEDialog_FixDemandElements.cpp
   GNEAdditionalDialog.h
   GNEAdditionalDialog.cpp
   GNEDemandElementDialog.h
   GNEDemandElementDialog.cpp
   GNERerouterDialog.h
   GNERerouterDialog.cpp
   GNERerouterIntervalDialog.h
   GNERerouterIntervalDialog.cpp
   GNECalibratorDialog.h
   GNECalibratorDialog.cpp
   GNECalibratorFlowDialog.h
   GNECalibratorFlowDialog.cpp
   GNERouteDialog.h
   GNERouteDialog.cpp
   GNEVehicleTypeDialog.h
   GNEVehicleTypeDialog.cpp
   GNEDialogACChooser.h
   GNEDialogACChooser.cpp
   )

add_library(netedit_dialogs STATIC ${netedit_dialogs_SRCS})
set_property(TARGET netedit_dialogs PROPERTY PROJECT_LABEL "z_netedit_dialogs")
add_dependencies(netedit_dialogs generate-version-h)
