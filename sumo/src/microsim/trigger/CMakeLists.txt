message("entering trigger")

set(trigger_files
	 MSCalibrator.cpp MSCalibrator.h
	 MSLaneSpeedTrigger.cpp MSLaneSpeedTrigger.h
	 MSTrigger.h MSTrigger.cpp
	 MSChargingStation.h MSChargingStation.cpp
	 MSTriggeredRerouter.cpp MSTriggeredRerouter.h)

add_library(mstrigger STATIC ${trigger_files})
