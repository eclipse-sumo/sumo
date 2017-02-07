message("entering netload")

set(netload_files
	NLDetectorBuilder.h NLDetectorBuilder.cpp
	NLDiscreteEventBuilder.h NLDiscreteEventBuilder.cpp
	NLEdgeControlBuilder.h NLEdgeControlBuilder.cpp
	NLJunctionControlBuilder.h NLJunctionControlBuilder.cpp
	NLBuilder.h NLBuilder.cpp
	NLHandler.h NLHandler.cpp
	NLTriggerBuilder.h NLTriggerBuilder.cpp)
add_library(netload STATIC ${netload_files})
