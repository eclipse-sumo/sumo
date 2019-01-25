/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSCrossSection.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @date    Tue Nov 25 15:23:28 2003
/// @version $Id$
///
// A simple description of a position on a lane (crossing of a lane)
/****************************************************************************/
#ifndef MSCrossSection_h
#define MSCrossSection_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCrossSection
 * @brief A simple description of a position on a lane (crossing of a lane)
 */
class MSCrossSection {
public:
    /** @brief Constructor
     *
     * @param[in] lane The lane to cross
     * @param[in] pos The position at the lane
     */
    MSCrossSection(MSLane* lane, double pos) : myLane(lane), myPosition(pos) {}


public:
    /// @brief The lane to cross
    MSLane* myLane;

    /// @brief The position at the lane
    double myPosition;

};


typedef std::vector< MSCrossSection > CrossSectionVector;
typedef CrossSectionVector::iterator CrossSectionVectorIt;
typedef CrossSectionVector::const_iterator CrossSectionVectorConstIt;


#endif

/****************************************************************************/

