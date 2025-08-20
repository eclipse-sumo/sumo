set(netedit_dialogs_elements_lists_SRCS
   GNEAdditionalElementList.cpp
   GNEAdditionalElementList.h
   GNEDemandElementList.cpp
   GNEDemandElementList.h
   GNEElementList.cpp
   GNEElementList.h
   GNEElementTable.cpp
   GNEElementTable.h
)

add_library(netedit_dialogs_elements_lists STATIC ${netedit_dialogs_elements_lists_SRCS})
set_property(TARGET netedit_dialogs_elements_lists PROPERTY PROJECT_LABEL "z_netedit_dialogs_elements_lists")
add_dependencies(netedit_dialogs_elements_lists generate-version-h)
