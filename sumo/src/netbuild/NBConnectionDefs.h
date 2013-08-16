/****************************************************************************/
/// @file    NBConnectionDefs.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Some container for connections
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBConnectionDefs_h
#define NBConnectionDefs_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include "NBConnection.h"


// ===========================================================================
// definitions
// ===========================================================================
/// Definition of a connection vector
typedef std::vector<NBConnection> NBConnectionVector;


/** @brief Definition of a container for connection block dependencies
    Includes a list of all connections which prohibit the key connection */
typedef std::map<NBConnection, NBConnectionVector> NBConnectionProhibits;


#endif

/****************************************************************************/

