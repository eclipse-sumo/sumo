/****************************************************************************/
/// @file    MSInternalLane.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2003
/// @version $Id$
///
// Class representing junction-internal lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSInternalLane_h
#define MSInternalLane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLogicJunction.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSEdgeControl.h"
#include <bitset>
#include <deque>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <iostream>
#include "MSNet.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLink;
class MSMoveReminder;
class GUILaneWrapper;
class GUIGlObjectStorage;



// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSInternalLane
 * Class which represents a single lane that lies within a junction.
 *  The simulation shall be able to use these lanes only when needed and work
 *  as previously thought without considering any junction-internal things.
 * Junction-internal conflicts are managed by this class by setting the
 *  information about a vehicle being on an internal lane to the junction
 *  logic. This logic than decides whether further vehicles may approach the
 *  junction
 */
class MSInternalLane
            : public MSLane
{
public:
    /// Constructor
    MSInternalLane(/*MSNet &net, */std::string id, SUMOReal maxSpeed,
                                   SUMOReal length, MSEdge *e, size_t numericalID,
                                   const Position2DVector &shape,
                                   const std::vector<SUMOVehicleClass> &allowed,
                                   const std::vector<SUMOVehicleClass> &disallowed);

    /// Destructor
    virtual ~MSInternalLane();

    /// Sets the information where to report vehicles being on this lane to
    void setParentJunctionInformation(MSLogicJunction::InnerState *foescont,
                                      size_t foesIdx);

    /** @brief moves the vehicles at the end of the lane
        Overrides the method from MSLane as internal lane must inform their
         corresponding junction about the fact that a vehicle uses them */
    virtual bool moveNonCritical();

    virtual bool moveCritical();


    void setPassPosition(SUMOReal passPos);

private:
    /// The container of junction-internal vehicle-occupied lanes
    MSLogicJunction::InnerState *myFoesCont;

    /// The index on where to write into this container
    int myFoesIndex;

    SUMOReal myPassPosition;

};


#endif

/****************************************************************************/

