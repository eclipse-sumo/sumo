/****************************************************************************/
/// @file    MSEdgeControl.h
/// @author  Christian Roessel
/// @date    Mon, 09 Apr 2001
/// @version $Id$
///
// operations.
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
#ifndef MSEdgeControl_h
#define MSEdgeControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <list>
#include <set>
#include "MSEdge.h"
#include <microsim/output/meandata/MSMeanData_Net.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class BinaryInputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSEdgeControl
 */
class MSEdgeControl
{
public:
    /// Container for edges.
    typedef std::vector< MSEdge* > EdgeCont;

    /// Use thic constructor only.
    MSEdgeControl(EdgeCont *singleLane, EdgeCont *multiLane);

    /// Destructor.
    ~MSEdgeControl();

    /** Moves (i.e. makes v- and x-updates) all vehicles currently on
        the net, except the first ones on each lane. They will by
        moved by the junctions. */
    void moveNonCritical();
    void moveCritical();
    void moveFirst();

    /// Try to change lanes in multilane edges.
    void changeLanes();

    /** Detect collisions. Shouldn't be necessary if
        model-implementation is correct. */
    void detectCollisions(SUMOTime timestep);

    void insertMeanData(unsigned int number);

    const EdgeCont &getSingleLaneEdges() const;

    const EdgeCont &getMultiLaneEdges() const;

    std::vector<std::string> getEdgeNames() const;

    void gotActive(MSLane *l);

public:
    /**
     * @struct LaneUsage
     * To fasten up speed, this structure holds the number of vehicles using
     *  a lane and the lane's neighbours. Only lanes that are occupied are
     *  forced to compute the vehicles longitunidal movement.
     * The information about a lane's neighbours speed up the computation
     *  of the lane changing.
     */
    struct LaneUsage {
        MSLane *lane;
        size_t noVehicles;
        SUMOReal vehLenSum;
        MSEdge::LaneCont::const_iterator firstNeigh;
        MSEdge::LaneCont::const_iterator lastNeigh;
        int index;
        bool amActive;
    };

private:
    /// Single lane edges.
    EdgeCont* mySingleLaneEdges;

    /// Multi lane edges.
    EdgeCont* myMultiLaneEdges;

    /// Default constructor.
    MSEdgeControl();

    /// Copy constructor.
    MSEdgeControl(const MSEdgeControl&);

    /// Assignment operator.
    MSEdgeControl& operator=(const MSEdgeControl&);

private:
    /// Definition of a container about a lane's number of vehicles and neighbors
    typedef std::vector<LaneUsage> LaneUsageVector;

    /// Information about lanes' number of vehicles and neighbors
    LaneUsageVector myLanes;

    std::list<MSLane*> myActiveLanes;

    std::vector<MSLane*> myWithVehicles2Integrate;

    std::set<MSLane*> myChangedStateLanes;

};


#endif

/****************************************************************************/

