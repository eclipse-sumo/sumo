message("entering tracker")

set(utguitracker_files
	GUIParameterTracker.cpp GUIParameterTracker.h
	TrackerValueDesc.cpp TrackerValueDesc.h)

add_library(utguitracker STATIC ${utguitracker_files})
