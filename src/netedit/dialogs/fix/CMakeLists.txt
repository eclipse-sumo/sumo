set(netedit_dialogs_fix_SRCS
   GNEFixAdditionalElementsDialog.cpp
   GNEFixAdditionalElementsDialog.h
   GNEFixDemandElementsDialog.cpp
   GNEFixDemandElementsDialog.h
   GNEFixElementsDialog.cpp
   GNEFixElementsDialog.h
   GNEFixElementsRowDialog.cpp
   GNEFixElementsRowDialog.h
   GNEFixNetworkElements.cpp
   GNEFixNetworkElements.h
)

add_library(netedit_dialogs_fix STATIC ${netedit_dialogs_fix_SRCS})
set_property(TARGET netedit_dialogs_fix PROPERTY PROJECT_LABEL "z_netedit_dialogs_fix")
add_dependencies(netedit_dialogs_fix generate-version-h)
