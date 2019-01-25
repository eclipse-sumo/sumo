/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSRightOfWayJunction.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 12 Dez 2001
/// @version $Id$
///
// A junction with right-of-way - rules
/****************************************************************************/
#ifndef MSRightOfWayJunction_h
#define MSRightOfWayJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSLogicJunction.h"
#include <bitset>
#include <vector>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSJunctionLogic;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRightOfWayJunction
 * @brief A junction with right-of-way - rules
 *
 * A class which realises junctions that do regard any kind of a right-of-way.
 * The rules for the right-of-way themselves are stored within the associated
 * "MSJunctionLogic" - structure.
 */
class MSRightOfWayJunction : public MSLogicJunction {
public:
    /** @brief Constructor
     * @param[in] id The id of the junction
     * @param[in] position The position of the junction
     * @param[in] shape The shape of the junction
     * @param[in] incoming The incoming lanes
     * @param[in] internal The internal lanes
     * @param[in] logic The logic of this junction
     */
    MSRightOfWayJunction(const std::string& id, SumoXMLNodeType type, const Position& position,
                         const PositionVector& shape,
                         std::vector<MSLane*> incoming,
                         std::vector<MSLane*> internal,
                         MSJunctionLogic* logic);

    /// Destructor.
    virtual ~MSRightOfWayJunction();

    void postloadInit();

    const std::vector<MSLink*>& getFoeLinks(const MSLink* const srcLink) const {
        return myLinkFoeLinks.find(srcLink)->second;
    }

    const std::vector<MSLane*>& getFoeInternalLanes(const MSLink* const srcLink) const {
        return myLinkFoeInternalLanes.find(srcLink)->second;
    }

    // @brief return the underlying right-of-way and conflict matrix
    const MSJunctionLogic* getLogic() const {
        return myLogic;
    }

protected:
    /** the type of the junction (its logic) */
    MSJunctionLogic* myLogic;

    // TODO: Documentation
    std::map<const MSLink*, std::vector<MSLink*> > myLinkFoeLinks;
    std::map<const MSLink*, std::vector<MSLane*> > myLinkFoeInternalLanes;


private:
    /// @brief Invalidated copy constructor.
    MSRightOfWayJunction(const MSRightOfWayJunction&);

    /// Invalidated assignment operator.
    MSRightOfWayJunction& operator=(const MSRightOfWayJunction&);

};


#endif

/****************************************************************************/

