set(netedit_dialogs_run_SRCS
   GNERunDialog.cpp
   GNERunDialog.h
   GNERunNetgenerateDialog.cpp
   GNERunNetgenerateDialog.h
   GNERunPythonToolDialog.cpp
   GNERunPythonToolDialog.h
   )

add_library(netedit_dialogs_run STATIC ${netedit_dialogs_run_SRCS})
set_property(TARGET netedit_dialogs_run PROPERTY PROJECT_LABEL "z_netedit_dialogs_run")
add_dependencies(netedit_dialogs_run generate-version-h)
