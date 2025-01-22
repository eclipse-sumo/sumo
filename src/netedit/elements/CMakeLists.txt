add_subdirectory(network)
add_subdirectory(additional)
add_subdirectory(demand)
add_subdirectory(data)

set(netedit_elements_SRCS
   GNEAttributeCarrier.cpp
   GNEAttributeCarrier.h
   GNEAttributeProperties.cpp
   GNEAttributeProperties.h
   GNECandidateElement.cpp
   GNECandidateElement.h
   GNEContour.cpp
   GNEContour.h
   GNEGeneralHandler.cpp
   GNEGeneralHandler.h
   GNEHierarchicalElement.cpp
   GNEHierarchicalElement.h
   GNEHierarchicalStructure.cpp
   GNEHierarchicalStructure.h
   GNEPathElement.cpp
   GNEPathElement.h
   GNETagProperties.cpp
   GNETagProperties.h
   )

add_library(netedit_elements STATIC ${netedit_elements_SRCS})
set_property(TARGET netedit_elements PROPERTY PROJECT_LABEL "z_netedit_elements")
