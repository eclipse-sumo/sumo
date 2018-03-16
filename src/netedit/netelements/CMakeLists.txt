
set(netedit_netElements_SRCS
    GNENetElement.h
    GNENetElement.cpp
    GNEEdge.h
    GNEEdge.cpp
    GNELane.h
    GNELane.cpp
    GNEInternalLane.h
    GNEInternalLane.cpp
    GNEConnection.h
    GNEConnection.cpp
    GNECrossing.h
    GNECrossing.cpp
    GNEJunction.h
    GNEJunction.cpp
    GNEProhibition.h
    GNEProhibition.cpp
    )

add_library(netedit_netElements STATIC ${netedit_changes_STAT_SRCS})
set_property(TARGET netedit_netElements PROPERTY PROJECT_LABEL "netedit_netElements")