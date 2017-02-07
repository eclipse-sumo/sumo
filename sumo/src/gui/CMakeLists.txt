message("entering gui")

set(gui_files
	GUIApplicationWindow.cpp GUIApplicationWindow.h
	GUIEvent_SimulationEnded.h GUIEvent_SimulationLoaded.h GUIEvent_Screenshot.h
	GUIGlobals.cpp GUIGlobals.h
	GUIManipulator.h GUIManipulator.cpp
	GUILoadThread.cpp GUILoadThread.h
	GUIRunThread.cpp GUIRunThread.h
	GUISUMOViewParent.cpp GUISUMOViewParent.h
	GUITLLogicPhasesTrackerWindow.cpp GUITLLogicPhasesTrackerWindow.h
	GUIViewTraffic.cpp GUIViewTraffic.h
	TraCIServerAPI_GUI.cpp TraCIServerAPI_GUI.h)

add_library(gui STATIC ${gui_files})


add_subdirectory(dialogs)
