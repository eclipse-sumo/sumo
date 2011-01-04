/****************************************************************************/
/// @file    MSLogicJunction.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// with one ore more logics.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

    /** @brief Container for link foes */
    typedef std::bitset<64> LinkFoes;

    /** @brief Container for junction-internal lane occupation
        Each element of this container represents one particular
        junction-internal lane */
    typedef std::bitset<64> InnerState;

    /// initialises the junction after the whole net has been loaded
    virtual void postloadInit() throw(ProcessError);

protected:
    /** @brief Constructor
     * @param[in] id The id of the junction
     * @param[in] position The position of the junction
     * @param[in] shape The shape of the junction
     * @param[in] incoming The incoming lanes
     * @param[in] internal The internal lanes
     */
    MSLogicJunction(const std::string &id, const Position2D &position,
                    const Position2DVector &shape,
                    std::vector<MSLane*> incoming
#ifdef HAVE_INTERNAL_LANES
                    , std::vector<MSLane*> internal
#endif
                   ) throw();

protected:
    /// list of incoming lanes
    std::vector<MSLane*> myIncomingLanes;

#ifdef HAVE_INTERNAL_LANES
    /// list of incoming lanes
    std::vector<MSLane*> myInternalLanes;
#endif

    /** Current inner state */
    InnerState  myInnerState;


private:
    /// @brief Invalidated copy constructor.
    MSLogicJunction(const MSLogicJunction&);

    /// @brief Invalidated assignment operator.
    MSLogicJunction& operator=(const MSLogicJunction&);

};


#endif

/****************************************************************************/

