set(netedit_dialogs_tools_SRCS
   GNEPythonToolDialog.h
   GNEPythonToolDialog.cpp
   GNEPythonToolDialogElements.h
   GNEPythonToolDialogElements.cpp
   GNERunPythonToolDialog.h
   GNERunPythonToolDialog.cpp
   GNENetgenerateDialog.h
   GNENetgenerateDialog.cpp
   GNERunNetgenerateDialog.h
   GNERunNetgenerateDialog.cpp
   )

add_library(netedit_dialogs_tools STATIC ${netedit_dialogs_tools_SRCS})
set_property(TARGET netedit_dialogs_tools PROPERTY PROJECT_LABEL "z_netedit_dialogs_tools")
add_dependencies(netedit_dialogs_tools generate-version-h)
