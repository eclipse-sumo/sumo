message("entering dialogs")


set(guidialogs_files
	GUIDialog_AboutSUMO.cpp GUIDialog_AboutSUMO.h
	GUIDialog_AppSettings.cpp GUIDialog_AppSettings.h
	GUIDialog_Breakpoints.cpp GUIDialog_Breakpoints.h)

add_library(guidialogs STATIC ${guidialogs_files})
