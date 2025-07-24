set(netedit_dialogs_additional_SRCS
   GNEAdditionalDialog.cpp
   GNEAdditionalDialog.h
   GNECalibratorDialog.cpp
   GNECalibratorDialog.h
   GNECalibratorFlowDialog.cpp
   GNECalibratorFlowDialog.h
   GNERerouterDialog.cpp
   GNERerouterDialog.h
   GNERerouterIntervalDialog.cpp
   GNERerouterIntervalDialog.h
   GNEVariableSpeedSignDialog.cpp
   GNEVariableSpeedSignDialog.h
)

add_library(netedit_dialogs_additional STATIC ${netedit_dialogs_additional_SRCS})
set_property(TARGET netedit_dialogs_additional PROPERTY PROJECT_LABEL "z_netedit_dialogs_additional")
add_dependencies(netedit_dialogs_additional generate-version-h)
