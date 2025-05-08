set(utils_gui_tests_STAT_SRCS
    GUIGlobalTestSystem.cpp
    GUIGlobalTestSystem.h
    GUITestSystem.cpp
    GUITestSystem.h
)

add_library(utils_gui_tests STATIC ${utils_gui_tests_STAT_SRCS})
set_property(TARGET utils_gui_tests PROPERTY PROJECT_LABEL "z_utils_gui_tests")
