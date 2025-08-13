add_subdirectory(basic)
add_subdirectory(elements)
add_subdirectory(fix)
add_subdirectory(options)
add_subdirectory(run)
add_subdirectory(tools)

set(netedit_dialogs_SRCS
   GNEACChooserDialog.cpp
   GNEACChooserDialog.h
   GNEAboutDialog.cpp
   GNEAboutDialog.h
   GNEAllowVClassesDialog.cpp
   GNEAllowVClassesDialog.h
   GNEColorDialog.cpp
   GNEColorDialog.h
   GNEDialog.cpp
   GNEDialog.h
   GNEGeometryPointDialog.cpp
   GNEGeometryPointDialog.h
   GNEHelpAttributesDialog.cpp
   GNEHelpAttributesDialog.h
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
