set(netedit_dialogs_options_SRCS
   GNENetgenerateOptionsDialog.cpp
   GNENetgenerateOptionsDialog.h
   GNENeteditOptionsDialog.cpp
   GNENeteditOptionsDialog.h
   GNEOptionsEditor.cpp
   GNEOptionsEditor.h
   GNEOptionsEditorRow.cpp
   GNEOptionsEditorRow.h
   GNESumoOptionsDialog.cpp
   GNESumoOptionsDialog.h
   )

add_library(netedit_dialogs_options STATIC ${netedit_dialogs_options_SRCS})
set_property(TARGET netedit_dialogs_options PROPERTY PROJECT_LABEL "z_netedit_dialogs_options")
add_dependencies(netedit_dialogs_options generate-version-h)
