set(utils_gui_settings_STAT_SRCS
   GUICompleteSchemeStorage.cpp
   GUICompleteSchemeStorage.h
   GUIPropertyScheme.h
   GUIPropertySchemeStorage.h
   GUISettingsHandler.cpp
   GUISettingsHandler.h
   GUIVisualizationSettings.cpp
   GUIVisualizationSettings.h
)

add_library(utils_gui_settings STATIC ${utils_gui_settings_STAT_SRCS})
set_property(TARGET utils_gui_settings PROPERTY PROJECT_LABEL "z_utils_gui_settings")


########### install files ###############




#original Makefile.am contents follow:

#noinst_LIBRARIES = libguiutilssettings.a
#
#libguiutilssettings_a_SOURCES = GUICompleteSchemeStorage.cpp GUICompleteSchemeStorage.h \
#GUIPropertyScheme.h GUIPropertySchemeStorage.h \
#GUISettingsHandler.cpp GUISettingsHandler.h \
#GUIVisualizationSettings.cpp GUIVisualizationSettings.h
