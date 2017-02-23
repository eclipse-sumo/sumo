message("entering actions")

set(actions_files
	Command_SaveTLSState.cpp Command_SaveTLSState.h
	Command_SaveTLSSwitches.cpp Command_SaveTLSSwitches.h
	Command_SaveTLSSwitchStates.cpp Command_SaveTLSSwitchStates.h
	Command_SaveTLCoupledLaneDet.cpp Command_SaveTLCoupledLaneDet.h
	Command_SaveTLCoupledDet.cpp Command_SaveTLCoupledDet.h)

add_library(msactions STATIC ${actions_files})
