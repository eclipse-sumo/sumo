set(microsim_trigger_STAT_SRCS
   MSCalibrator.cpp
   MSCalibrator.h
   MSLaneSpeedTrigger.cpp
   MSLaneSpeedTrigger.h
   MSTrigger.h
   MSTrigger.cpp
   MSChargingStation.h
   MSChargingStation.cpp
   MSTriggeredRerouter.cpp
   MSTriggeredRerouter.h
)

add_library(microsim_trigger STATIC ${microsim_trigger_STAT_SRCS})
