/****************************************************************************/
/// @file    GUIAddWeightsStorage.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 16 Jun 2004
/// @version $Id$
///
// Definition of a storage for additional net weights
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIAddWeightsStorage_h
#define GUIAddWeightsStorage_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <vector>
#include <string>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// structure definitions
// ===========================================================================
/**
 * @struct GUIAddWeight
 * @brief A single additional edge weight definition
 */
struct GUIAddWeight {
    /// The edge id of the edge which shall be additionally weighted
    std::string edgeID;
    /// The absolute edge weight
    SUMOReal absolute;
    /// The summand to add to the edge's original weight
    SUMOReal summand;
    /// The factor to multiply to the edge's original weight with
    SUMOReal factor;
    /// Time step the additional weight starts to be valid at
    SUMOTime timeBeg;
    /// Time step until which the additional is valid
    SUMOTime timeEnd;
};


/// Definition of a storage for additional net weights
typedef std::vector<GUIAddWeight> GUIAddWeightsStorage;


#endif

/****************************************************************************/

