add_subdirectory(additional)
add_subdirectory(demand)
add_subdirectory(fix)
add_subdirectory(tools)
add_subdirectory(options)

set(netedit_dialogs_SRCS
   GNEAboutDialog.cpp
   GNEAboutDialog.h
   GNEACChooserDialog.cpp
   GNEACChooserDialog.h
   GNEAllowVClassesDialog.cpp
   GNEAllowVClassesDialog.h
   GNEDialog.cpp
   GNEDialog.h
   GNEGeometryPointDialog.cpp
   GNEGeometryPointDialog.h
   GNEHelpAttributesDialog.cpp
   GNEHelpAttributesDialog.h
   GNEHelpDialog.cpp
   GNEHelpDialog.h
   GNEKeepElementsDialog.cpp
   GNEKeepElementsDialog.h
   GNEMultipleParametersDialog.cpp
   GNEMultipleParametersDialog.h
   GNESingleParametersDialog.cpp
   GNESingleParametersDialog.h
   GNEUndoListDialog.cpp
   GNEUndoListDialog.h
)

add_library(netedit_dialogs STATIC ${netedit_dialogs_SRCS})
set_property(TARGET netedit_dialogs PROPERTY PROJECT_LABEL "z_netedit_dialogs")
add_dependencies(netedit_dialogs generate-version-h)
