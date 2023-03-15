set(netedit_dialogs_tools_SRCS
   GNETool.h
   GNETool.cpp
   GNEToolDialog.h
   GNEToolDialog.cpp
   GNEToolDialogElements.h
   GNEToolDialogElements.cpp
   GNERunTool.h
   GNERunTool.cpp
   GNERunToolDialog.h
   GNERunToolDialog.cpp
   )

add_library(netedit_dialogs_tools STATIC ${netedit_dialogs_tools_SRCS})
set_property(TARGET netedit_dialogs_tools PROPERTY PROJECT_LABEL "z_netedit_tools")
add_dependencies(netedit_dialogs_tools generate-version-h)
