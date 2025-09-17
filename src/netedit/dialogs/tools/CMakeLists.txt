set(netedit_dialogs_tools_SRCS
   GNENetgenerateDialog.cpp
   GNENetgenerateDialog.h
   GNEPythonToolDialog.cpp
   GNEPythonToolDialog.h
   GNEPythonToolDialogElements.cpp
   GNEPythonToolDialogElements.h
   )

add_library(netedit_dialogs_tools STATIC ${netedit_dialogs_tools_SRCS})
set_property(TARGET netedit_dialogs_tools PROPERTY PROJECT_LABEL "z_netedit_dialogs_tools")
add_dependencies(netedit_dialogs_tools generate-version-h)
