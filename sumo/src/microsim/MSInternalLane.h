#ifndef MSInternalLane_H
#define MSInternalLane_H
//---------------------------------------------------------------------------//
//                        MSInternalLane.h -
//  Class representing junction-internal lanes
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.3  2004/07/02 09:58:08  dkrajzew
// MeanData refactored (moved to microsim/output); numerical id for online routing added
//
// Revision 1.2  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <helpers/PreStartInitialised.h>
#include "MSLogicJunction.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSVehicle.h"
#include "MSEdgeControl.h"
#include <bitset>
#include <deque>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <iostream>
#include "MSNet.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEmitter;
class MSLink;
class MSMoveReminder;
class GUILaneWrapper;
class GUIGlObjectStorage;



/* =========================================================================
 * class definitions
 * ======================================================================= */
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
    MSInternalLane( MSNet &net, std::string id, double maxSpeed,
        double length, MSEdge *e, size_t numericalID);

    /// Destructor
    ~MSInternalLane();

    /// Sets the information where to report vehicles being on this lane to
    void setParentJunctionInformation(MSLogicJunction::InnerState *foescont,
        size_t foesIdx);

    /** @brief moves the vehicles at the end of the lane
        Overrides the method from MSLane as internal lane must inform their
         corresponding junction about the fact that a vehicle uses them */
    void moveNonCritical();

private:
    /// The container of junction-internal vehicle-occupied lanes
    MSLogicJunction::InnerState *myFoesCont;

    /// The index on where to write into this container
    size_t myFoesIndex;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
