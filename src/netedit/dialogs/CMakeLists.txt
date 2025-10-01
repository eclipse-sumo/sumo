add_subdirectory(basic)
add_subdirectory(elements)
add_subdirectory(file)
add_subdirectory(fix)
add_subdirectory(options)
add_subdirectory(run)
add_subdirectory(tools)

set(netedit_dialogs_SRCS
   GNEACChooserDialog.cpp
   GNEACChooserDialog.h
   GNEAboutDialog.cpp
   GNEAboutDialog.h
   GNEVClassesDialog.cpp
   GNEVClassesDialog.h
   GNEColorDialog.cpp
   GNEColorDialog.h
   GNECrashDialog.cpp
   GNECrashDialog.h
   GNEDialog.cpp
   GNEDialog.h
   GNEDialogEnum.h
   GNEGeometryPointDialog.cpp
   GNEGeometryPointDialog.h
   GNEHelpAttributesDialog.cpp
   GNEHelpAttributesDialog.h
   GNEParametersDialog.cpp
   GNEParametersDialog.h
   GNESaveDialog.cpp
   GNESaveDialog.h
   GNEUndoListDialog.cpp
   GNEUndoListDialog.h
)

add_library(netedit_dialogs STATIC ${netedit_dialogs_SRCS})
set_property(TARGET netedit_dialogs PROPERTY PROJECT_LABEL "z_netedit_dialogs")
add_dependencies(netedit_dialogs generate-version-h)
