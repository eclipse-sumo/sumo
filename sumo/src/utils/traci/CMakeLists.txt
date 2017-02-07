message("entering traci")

set(traci_files
	TraCIAPI.cpp TraCIAPI.h)

add_library(uttraci STATIC ${traci_files})
