#ifndef MSEdgeControl_H
#define MSEdgeControl_H
/***************************************************************************
                          MSEdgeControl.h  -  Coordinates Edge
                          operations.
                             -------------------
    begin                : Mon, 09 Apr 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.7  2005/02/01 10:10:40  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.6  2004/08/02 12:08:39  dkrajzew
// raw-output extracted; output device handling rechecked
//
// Revision 1.5  2004/07/02 09:55:13  dkrajzew
// MeanData refactored (moved to microsim/output)
//
// Revision 1.4  2003/07/16 15:28:00  dkrajzew
// MSEmitControl now only simulates lanes which do have vehicles; the edges do not go through the lanes, the EdgeControl does
//
// Revision 1.3  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:40:35  dkrajzew
// usage of MSPerson removed; will be reimplemented later
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.3  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.2  2002/04/24 13:06:47  croessel
// Changed signature of void detectCollisions() to void detectCollisions(
// MSNet::Time )
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.2  2002/03/20 11:11:09  croessel
// Splitted friend from class-declaration.
//
// Revision 2.1  2002/03/13 16:56:34  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.0  2002/02/14 14:43:14  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.12  2002/02/05 13:51:51  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.11  2002/01/30 15:44:36  croessel
// Again: added "std::" in operator<<
//
// Revision 1.10  2002/01/30 11:28:27  croessel
// Added "std::" in operator<<
//
// Revision 1.9  2001/12/19 16:57:11  croessel
// using namespace std replaced by std::
//
// Revision 1.8  2001/11/15 17:12:13  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.7  2001/11/14 11:45:54  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.6  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.5  2001/10/23 09:29:49  traffic
// parser bugs removed
//
// Revision 1.3  2001/09/06 15:31:25  croessel
// Added operator<< to class MSEdgeControl for simple text output.
//
// Revision 1.2  2001/07/16 12:55:46  croessel
// Changed id type from unsigned int to string. Added string-pointer dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "MSEdge.h"
#include <microsim/output/meandata/MSMeanData_Net.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OutputDevice;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 */
class MSEdgeControl
{
public:
    /// Container for edges.
    typedef std::vector< MSEdge* > EdgeCont;

    /// Use thic constructor only.
    MSEdgeControl( std::string id, EdgeCont *singleLane, EdgeCont *multiLane);

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
    void detectCollisions( SUMOTime timestep );

    /** Inserts edgecontrol into the static dictionary and returns true
        if the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool dictionary( std::string id, MSEdgeControl* edgeControl );

    /** Returns the MSEdgeControl associated to the key id if exists,
        otherwise returns 0. */
    static MSEdgeControl* dictionary( std::string id );

    /** Clears the dictionary */
    static void clear();

    /// simple output operator
    friend std::ostream& operator<<( std::ostream& os,
                                     const MSEdgeControl& ec );

    void addToLanes(MSMeanData_Net *newMeanData);

    const EdgeCont &getSingleLaneEdges() const;

    const EdgeCont &getMultiLaneEdges() const;

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
        MSEdge::LaneCont::iterator firstNeigh;
        MSEdge::LaneCont::iterator lastNeigh;
    };

private:
    /// Unique ID.
    std::string myID;

    /// Single lane edges.
    EdgeCont* mySingleLaneEdges;

    /// Multi lane edges.
    EdgeCont* myMultiLaneEdges;

    /// definitions of the static dictionary type
    typedef std::map< std::string, MSEdgeControl* > DictType;
    /// Static dictionary to associate string-ids with objects.
    static DictType myDict;

    /// Default constructor.
    MSEdgeControl();

    /// Copy constructor.
    MSEdgeControl( const MSEdgeControl& );

    /// Assignment operator.
    MSEdgeControl& operator=( const MSEdgeControl& );

private:
    /// Definition of a container about a lane's number of vehicles and neighbors
    typedef std::vector<LaneUsage> LaneUsageVector;

    /// Information about lanes' number of vehicles and neighbors
    LaneUsageVector myLanes;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/


#endif

// Local Variables:
// mode:C++
// End:
