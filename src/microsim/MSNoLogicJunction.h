/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSNoLogicJunction.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// logic, e.g. for exits.
/****************************************************************************/
#ifndef MSNoLogicJunction_h
#define MSNoLogicJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <vector>
#include <bitset>
#include "MSJunction.h"

// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSNoLogicJunction
 * This junctions let all vehicles past through so they only should be used on
 * junctions where incoming vehicles are no foes to each other (may drive
 * simultaneously).
 */
class MSNoLogicJunction  : public MSJunction {
public:
    /// Destructor.
    virtual ~MSNoLogicJunction();

    /** @brief Constructor
     * @param[in] id The id of the junction
     * @param[in] position The position of the junction
     * @param[in] shape The shape of the junction
     * @param[in] incoming The incoming lanes
     * @param[in] internal The internal lanes
     */
    MSNoLogicJunction(const std::string& id, SumoXMLNodeType type, const Position& position,
                      const PositionVector& shape,
                      std::vector<MSLane*> incoming,
                      std::vector<MSLane*> internal);

    /** Initialises the junction after the net was completely loaded */
    void postloadInit();

    /** @brief Returns all internal lanes on the junction
     */
    virtual const std::vector<MSLane*> getInternalLanes() const;

private:
    /** Lanes incoming to the junction */
    std::vector<MSLane*> myIncomingLanes;

    /** The junctions internal lanes */
    std::vector<MSLane*> myInternalLanes;

private:
    /// @brief Invalidated copy constructor.
    MSNoLogicJunction(const MSNoLogicJunction&);

    /// @brief Invalidated assignment operator.
    MSNoLogicJunction& operator=(const MSNoLogicJunction&);

};


#endif

/****************************************************************************/

