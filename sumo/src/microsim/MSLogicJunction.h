/****************************************************************************/
/// @file    MSLogicJunction.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// with one ore more logics.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSLogicJunction_h
#define MSLogicJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSJunction.h"
#include <utils/common/SUMOTime.h>
#include <utils/common/StdDefs.h>
#include <bitset>
#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLogicJunction
 * A junction which may not let all vehicles through, but must perform any
 * kind of an operation to determine which cars are allowed to drive in this
 * step.
 */
class MSLogicJunction : public MSJunction {
public:


    /// Destructor.
    virtual ~MSLogicJunction();

    /** @brief Container for link response and foes */
    typedef std::bitset<SUMO_MAX_CONNECTIONS> LinkBits;

    /// initialises the junction after the whole net has been loaded
    virtual void postloadInit();

protected:
    /** @brief Constructor
     * @param[in] id The id of the junction
     * @param[in] id The type of the junction
     * @param[in] position The position of the junction
     * @param[in] shape The shape of the junction
     * @param[in] incoming The incoming lanes
     * @param[in] internal The internal lanes
     */
    MSLogicJunction(const std::string& id,
                    SumoXMLNodeType type,
                    const Position& position,
                    const PositionVector& shape,
                    std::vector<MSLane*> incoming
#ifdef HAVE_INTERNAL_LANES
                    , std::vector<MSLane*> internal
#endif
                   );

protected:
    /// list of incoming lanes
    std::vector<MSLane*> myIncomingLanes;

#ifdef HAVE_INTERNAL_LANES
    /// list of incoming lanes
    std::vector<MSLane*> myInternalLanes;
#endif

private:
    /// @brief Invalidated copy constructor.
    MSLogicJunction(const MSLogicJunction&);

    /// @brief Invalidated assignment operator.
    MSLogicJunction& operator=(const MSLogicJunction&);

};


#endif

/****************************************************************************/

