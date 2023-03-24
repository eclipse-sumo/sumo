set(netedit_dialogs_tools_SRCS
   GNEPythonTool.h
   GNEPythonTool.cpp
   GNEPythonToolDialog.h
   GNEPythonToolDialog.cpp
   GNEPythonToolDialogElements.h
   GNEPythonToolDialogElements.cpp
   GNENetdiffToolDialog.h
   GNENetdiffToolDialog.cpp
   GNERunPythonTool.h
   GNERunPythonTool.cpp
   GNERunPythonToolDialog.h
   GNERunPythonToolDialog.cpp
   GNERunNetgenerate.h
   GNERunNetgenerate.cpp
   GNENetgenerateDialog.h
   GNENetgenerateDialog.cpp
   GNERunNetgenerateDialog.h
   GNERunNetgenerateDialog.cpp
   )

add_library(netedit_dialogs_tools STATIC ${netedit_dialogs_tools_SRCS})
set_property(TARGET netedit_dialogs_tools PROPERTY PROJECT_LABEL "z_netedit_tools")
add_dependencies(netedit_dialogs_tools generate-version-h)
