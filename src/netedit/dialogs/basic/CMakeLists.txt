set(netedit_dialogs_basic_SRCS
    GNEBasicDialog.cpp
    GNEBasicDialog.h
    GNEErrorBasicDialog.cpp
    GNEErrorBasicDialog.h
    GNEHelpBasicDialog.cpp
    GNEHelpBasicDialog.h
    GNEInformationBasicDialog.cpp
    GNEInformationBasicDialog.h
    GNEOverwriteElement.cpp
    GNEOverwriteElement.h
    GNEQuestionBasicDialog.cpp
    GNEQuestionBasicDialog.h
    GNEWarningBasicDialog.cpp
    GNEWarningBasicDialog.h
)

add_library(netedit_dialogs_basic STATIC ${netedit_dialogs_basic_SRCS})
set_property(TARGET netedit_dialogs_basic PROPERTY PROJECT_LABEL "z_netedit_dialogs_basic")
add_dependencies(netedit_dialogs_basic generate-version-h)
