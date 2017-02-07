message("entering settings")

set(utguisettings_files
	 GUICompleteSchemeStorage.cpp GUICompleteSchemeStorage.h
	 GUIPropertyScheme.h GUIPropertySchemeStorage.h
	 GUISettingsHandler.cpp GUISettingsHandler.h
	 GUIVisualizationSettings.cpp GUIVisualizationSettings.h)

add_library(utguisettings STATIC ${utguisettings_files})

