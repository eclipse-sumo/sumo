/****************************************************************************/
/// @file    MSLinkCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A vector of links
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSLinkCont_h
#define MSLinkCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include "MSLink.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLinkCont
 * A simple contanier of definitions about how a lane may be left
 */
typedef std::vector<MSLink*> MSLinkCont;


/**
 * @class MSLinkContHelper
 * Some helping functions for dealing with links.
 */
class MSLinkContHelper {
public:
#ifdef HAVE_INTERNAL_LANES
    /** @brief Returns the internal lane that must be passed in order to get to the desired edge
        Returns 0 if no such edge exists */
    static const MSEdge* getInternalFollowingEdge(MSLane* fromLane,
            const MSEdge* followerAfterInternal);
#endif

    /** @brief Returns the link connecting both lanes
        Both lanes have to be non-internal; 0 may be returned if no connection
        exists */
    static MSLink* getConnectingLink(const MSLane& from
                                     , const MSLane& to);
};


#endif

/****************************************************************************/

