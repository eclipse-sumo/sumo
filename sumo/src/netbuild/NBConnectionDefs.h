#ifndef NBConnectionDefs_h
#define NBConnectionDefs_h

#include "NBEdge.h"
#include <vector>
#include <map>


/// Definition of a connection
typedef std::pair<NBEdge*, NBEdge*> Connection;

/// Definition of a connection vector
typedef std::vector<Connection> ConnectionVector;

/** @brief Definition of a container for connection block dependencies
    Includes a list of all connections which prohibit the key connection */
typedef std::map<Connection, ConnectionVector> ConnectionProhibits;


#endif

