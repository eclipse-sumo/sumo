#ifndef NBConnectionDefs_h
#define NBConnectionDefs_h

#include <vector>
#include <map>
#include "NBConnection.h"

/// Definition of a connection vector
typedef std::vector<NBConnection> NBConnectionVector;

/** @brief Definition of a container for connection block dependencies
    Includes a list of all connections which prohibit the key connection */
typedef std::map<NBConnection, NBConnectionVector> NBConnectionProhibits;


#endif

