set(netedit_dialogs_options_SRCS
   GNEOptionsDialog.h
   GNEOptionsDialog.cpp
   GNEOptionsDialogElements.h
   GNEOptionsDialogElements.cpp
   )

add_library(netedit_dialogs_options STATIC ${netedit_dialogs_options_SRCS})
set_property(TARGET netedit_dialogs_options PROPERTY PROJECT_LABEL "z_netedit_dialogs_options")
add_dependencies(netedit_dialogs_options generate-version-h)
