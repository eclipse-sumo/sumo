set(netedit_tools_SRCS
   GNENetDiffTool.cpp
   GNENetDiffTool.h
   GNEPythonTool.cpp
   GNEPythonTool.h
   )

add_library(netedit_tools STATIC ${netedit_tools_SRCS})
set_property(TARGET netedit_tools PROPERTY PROJECT_LABEL "z_netedit_tools")
add_dependencies(netedit_tools generate-version-h)
