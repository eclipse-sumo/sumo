message("entering mesosim")

set(mesosim_files
	MEInductLoop.cpp MEInductLoop.h
	MELoop.cpp MELoop.h
	MESegment.cpp MESegment.h
	METriggeredCalibrator.cpp METriggeredCalibrator.h
	MEVehicle.cpp MEVehicle.h
	MEVehicleControl.cpp MEVehicleControl.h)

add_library(mesosim STATIC ${mesosim_files})
