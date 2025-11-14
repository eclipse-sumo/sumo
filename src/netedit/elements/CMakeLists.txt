add_subdirectory(additional)
add_subdirectory(data)
add_subdirectory(demand)
add_subdirectory(moving)
add_subdirectory(network)

set(netedit_elements_SRCS
   GNEAttributeCarrier.cpp
   GNEAttributeCarrier.h
   GNECandidateElement.cpp
   GNECandidateElement.h
   GNEContour.cpp
   GNEContour.h
   GNEGeneralHandler.cpp
   GNEGeneralHandler.h
   GNEHierarchicalElement.cpp
   GNEHierarchicalElement.h
   GNEHierarchicalStructureChildren.cpp
   GNEHierarchicalStructureChildren.h
   GNEHierarchicalStructureParents.cpp
   GNEHierarchicalStructureParents.h
   GNEPathElement.cpp
   GNEPathElement.h
)

add_library(netedit_elements STATIC ${netedit_elements_SRCS})
set_property(TARGET netedit_elements PROPERTY PROJECT_LABEL "z_netedit_elements")
