message("entering mesogui")

set(mesogui_files
	GUIMEInductLoop.cpp GUIMEInductLoop.h
	GUIMEVehicle.cpp GUIMEVehicle.h
	GUIMEVehicleControl.cpp GUIMEVehicleControl.h)
add_library(mesogui STATIC ${mesogui_files})
