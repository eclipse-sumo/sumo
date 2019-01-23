set(utils_vehicle_STAT_SRCS
   SUMOVehicle.h
   SUMOVehicleParameter.cpp
   SUMOVehicleParameter.h
   SUMOVTypeParameter.cpp
   SUMOVTypeParameter.h)

add_library(utils_vehicle STATIC ${utils_vehicle_STAT_SRCS})
set_property(TARGET utils_vehicle PROPERTY PROJECT_LABEL "z_utils_vehicle")
