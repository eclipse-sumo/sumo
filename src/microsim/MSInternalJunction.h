/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#ifndef MSInternalJunction_h
#define MSInternalJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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

    bool indirectBicycleTurn(const MSLane* specialLane, const MSLink* thisLink, const MSLane* foeFirstPart, const MSLink* foeLink) const;

    std::vector<MSLink*> myInternalLinkFoes;
    std::vector<MSLane*> myInternalLaneFoes;

    /// @brief Invalidated copy constructor.
    MSInternalJunction(const MSInternalJunction&);

    /// @brief Invalidated assignment operator.
    MSInternalJunction& operator=(const MSInternalJunction&);

};


#endif

/****************************************************************************/

