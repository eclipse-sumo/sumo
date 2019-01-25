/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#ifndef MSLogicJunction_h
#define MSLogicJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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

    /** @brief Returns all internal lanes on the junction
     */
    const std::vector<MSLane*> getInternalLanes() const;

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
                    std::vector<MSLane*> incoming,
                    std::vector<MSLane*> internal
                   );

protected:
    /// list of incoming lanes
    std::vector<MSLane*> myIncomingLanes;

    /// list of internal lanes
    std::vector<MSLane*> myInternalLanes;

private:
    /// @brief Invalidated copy constructor.
    MSLogicJunction(const MSLogicJunction&);

    /// @brief Invalidated assignment operator.
    MSLogicJunction& operator=(const MSLogicJunction&);

};


#endif

/****************************************************************************/

