/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    RODFRouteDesc.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A route within the DFROUTER
/****************************************************************************/
#ifndef RODFRouteDesc_h
#define RODFRouteDesc_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <string>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class RODFDetector;

typedef std::vector<ROEdge*> ROEdgeVector;

// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @struct RODFRouteDesc
 * @brief A route within the DFROUTER
 */
struct RODFRouteDesc {
    /// @brief The edges the route is made of
    ROEdgeVector edges2Pass;
    /// @brief The name of the route
    std::string routename;
    double duration_2;
    double distance;
    int passedNo;
    const ROEdge* endDetectorEdge;
    const ROEdge* lastDetectorEdge;
    double distance2Last;
    SUMOTime duration2Last;

    double overallProb;
    double factor;

};


#endif

/****************************************************************************/

