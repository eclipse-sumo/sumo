add_subdirectory(lists)

set(netedit_dialogs_elements_SRCS
   GNECalibratorDialog.cpp
   GNECalibratorDialog.h
   GNECalibratorFlowDialog.cpp
   GNECalibratorFlowDialog.h
   GNEElementDialog.h
   GNERerouterDialog.cpp
   GNERerouterDialog.h
   GNERerouterIntervalDialog.cpp
   GNERerouterIntervalDialog.h
   GNERouteDialog.cpp
   GNERouteDialog.h
   GNEVTypeDistributionsDialog.cpp
   GNEVTypeDistributionsDialog.h
   GNEVariableSpeedSignDialog.cpp
   GNEVariableSpeedSignDialog.h
   GNEVehicleTypeDialog.cpp
   GNEVehicleTypeDialog.h
)

add_library(netedit_dialogs_elements STATIC ${netedit_dialogs_elements_SRCS})
set_property(TARGET netedit_dialogs_elements PROPERTY PROJECT_LABEL "z_netedit_dialogs_elements")
add_dependencies(netedit_dialogs_elements generate-version-h)
