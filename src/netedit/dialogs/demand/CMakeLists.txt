set(netedit_dialogs_demand_SRCS
   GNEDemandElementDialog.cpp
   GNEDemandElementDialog.h
   GNERouteDialog.cpp
   GNERouteDialog.h
   GNEVehicleTypeDialog.cpp
   GNEVehicleTypeDialog.h
   GNEVTypeDistributionsDialog.cpp
   GNEVTypeDistributionsDialog.h
)

add_library(netedit_dialogs_demand STATIC ${netedit_dialogs_demand_SRCS})
set_property(TARGET netedit_dialogs_demand PROPERTY PROJECT_LABEL "z_netedit_dialogs_demand")
add_dependencies(netedit_dialogs_demand generate-version-h)
