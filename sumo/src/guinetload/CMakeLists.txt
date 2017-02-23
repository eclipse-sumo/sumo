message("entering guinetload")

set(guinetload_files
	GUIDetectorBuilder.cpp GUIDetectorBuilder.h
	GUIEdgeControlBuilder.cpp GUIEdgeControlBuilder.h
	GUITriggerBuilder.cpp GUITriggerBuilder.h)

add_library(guinetload STATIC ${guinetload_files})

