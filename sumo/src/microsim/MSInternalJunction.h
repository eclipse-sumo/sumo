/****************************************************************************/
/// @file    MSInternalJunction.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// junction.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
#ifndef MSInternalJunction_h
#define MSInternalJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/StdDefs.h>
#include "MSLogicJunction.h"
#include <bitset>
#include <vector>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSJunctionLogic;
class MSLink;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSInternalJunction
 * A class which realises junctions that do regard any kind of a right-of-way.
 * The rules for the right-of-way themselves are stored within the associated
 * "MSJunctionLogic" - structure.
 */
#ifdef HAVE_INTERNAL_LANES
class MSInternalJunction : public MSLogicJunction {
public:
    /** @brief Constructor
     * @param[in] id The id of the junction
     * @param[in] position The position of the junction
     * @param[in] shape The shape of the junction
     * @param[in] incoming The incoming lanes
     * @param[in] internal The internal lanes
     */
    MSInternalJunction(const std::string& id, SumoXMLNodeType type, const Position& position,
                       const PositionVector& shape,
                       std::vector<MSLane*> incoming, std::vector<MSLane*> internal);

    /// Destructor.
    virtual ~MSInternalJunction();


    void postloadInit();

    const std::vector<MSLink*>& getFoeLinks(const MSLink* const srcLink) const {
        UNUSED_PARAMETER(srcLink);
        return myInternalLinkFoes;
    }

    const std::vector<MSLane*>& getFoeInternalLanes(const MSLink* const srcLink) const {
        UNUSED_PARAMETER(srcLink);
        return myInternalLaneFoes;
    }

private:

    std::vector<MSLink*> myInternalLinkFoes;
    std::vector<MSLane*> myInternalLaneFoes;

    /// @brief Invalidated copy constructor.
    MSInternalJunction(const MSInternalJunction&);

    /// @brief Invalidated assignment operator.
    MSInternalJunction& operator=(const MSInternalJunction&);

};


#endif
#endif

/****************************************************************************/

