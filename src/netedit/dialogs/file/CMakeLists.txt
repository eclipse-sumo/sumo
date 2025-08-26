set(netedit_dialogs_file_SRCS
    GNEFileDialog.cpp
    GNEFileDialog.h
    GNEFileSelector.cpp
    GNEFileSelector.h
)

add_library(netedit_dialogs_file STATIC ${netedit_dialogs_file_SRCS})
set_property(TARGET netedit_dialogs_file PROPERTY PROJECT_LABEL "z_netedit_dialogs_file")
add_dependencies(netedit_dialogs_file generate-version-h)
