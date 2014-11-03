/****************************************************************************/
/// @file    RODFRouteDesc.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A route within the DFROUTER
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
#ifndef RODFRouteDesc_h
#define RODFRouteDesc_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class RODFDetector;


// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @struct RODFRouteDesc
 * @brief A route within the DFROUTER
 */
struct RODFRouteDesc {
    /// @brief The edges the route is made of
    std::vector<ROEdge*> edges2Pass;
    /// @brief The name of the route
    std::string routename;
    SUMOReal duration_2;
    SUMOReal distance;
    int passedNo;
    const ROEdge* endDetectorEdge;
    const ROEdge* lastDetectorEdge;
    SUMOReal distance2Last;
    SUMOTime duration2Last;

    SUMOReal overallProb;
    SUMOReal factor;

};


#endif

/****************************************************************************/

