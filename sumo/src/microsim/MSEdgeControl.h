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
    MSEdgeControl(/*std::string id, */EdgeCont *singleLane, EdgeCont *multiLane);

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

    /** Inserts edgecontrol into the static dictionary and returns true
        if the key id isn't already in the dictionary. Otherwise returns
        false. */
    //static bool dictionary( std::string id, MSEdgeControl* edgeControl );

    /** Returns the MSEdgeControl associated to the key id if exists,
        otherwise returns 0. */
    //static MSEdgeControl* dictionary( std::string id );

    /** Clears the dictionary */
    //static void clear();
    /*
        // simple output operator
        friend std::ostream& operator<<( std::ostream& os,
                                         const MSEdgeControl& ec );
    */
    void insertMeanData(unsigned int number);

    const EdgeCont &getSingleLaneEdges() const;

    const EdgeCont &getMultiLaneEdges() const;

    void saveState(std::ostream &os, long what);
    void loadState(BinaryInputDevice &bis, long what);

    std::vector<std::string> getEdgeNames() const;

public:
    /**
     * @struct LaneUsage
     * To fasten up speed, this structure holds the number of vehicles using
     *  a lane and the lane's neighbours. Only lanes that are occupied are
     *  forced to compute the vehicles longitunidal movement.
     * The information about a lane's neighbours speed up the computation
     *  of the lane changing.
     */
    struct LaneUsage
    {
        MSLane *lane;
        size_t noVehicles;
        SUMOReal vehLenSum;
        MSEdge::LaneCont::const_iterator firstNeigh;
        MSEdge::LaneCont::const_iterator lastNeigh;
    };

private:
    /// Unique ID.
    //std::string myID;

    /// Single lane edges.
    EdgeCont* mySingleLaneEdges;

    /// Multi lane edges.
    EdgeCont* myMultiLaneEdges;

    /*
    /// definitions of the static dictionary type
    typedef std::map< std::string, MSEdgeControl* > DictType;
    /// Static dictionary to associate string-ids with objects.
    static DictType myDict;
    */

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

};


#endif

/****************************************************************************/

