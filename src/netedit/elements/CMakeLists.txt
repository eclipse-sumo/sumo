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
   GNEGeneralHandler.cpp
   GNEGeneralHandler.h
   GNEHierarchicalContainer.cpp
   GNEHierarchicalContainer.h
   GNEHierarchicalElement.cpp
   GNEHierarchicalElement.h
   GNEContour.cpp
   GNEContour.h
   GNETagProperties.cpp
   GNETagProperties.h
   )

add_library(netedit_elements STATIC ${netedit_elements_SRCS})
set_property(TARGET netedit_elements PROPERTY PROJECT_LABEL "z_netedit_elements")
