/***************************************************************************
                          MSRightOfWayJunction.h  -  Usual right-of-way
                          junction.
                             -------------------
    begin                : Wed, 12 Dez 2001
    copyright            : (C) 2001 by Christian Roessel
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
// Revision 1.2  2002/10/16 16:42:29  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include; junction extended by position information (should be revalidated later)
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.1  2002/02/21 18:49:46  croessel
// Deadlock-killer implemented.
//
// Revision 2.0  2002/02/14 14:43:19  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.6  2002/02/13 10:12:26  croessel
// Removed "class findCompetitor" from inside "class
// MSRightOfWayJunction".
//
// Revision 1.5  2002/02/01 11:52:28  croessel
// Removed function-adaptor findCompetitor from inside the class to the
// outside to please MSVC++.
//
// Revision 1.4  2002/02/01 11:40:34  croessel
// Changed return-type of some void methods used in for_each-loops to
// bool in order to please MSVC++.
//
// Revision 1.3  2001/12/20 14:39:15  croessel
// using namespace std replaced by std::
//
// Revision 1.2  2001/12/13 15:53:12  croessel
// In class InLane: changed Junction to RightOfWayJunction.
//
// Revision 1.1  2001/12/12 17:46:02  croessel
// Initial commit. Part of a new junction hierarchy.
//

#ifndef MSRightOfWayJunction_H
#define MSRightOfWayJunction_H

#include "MSLogicJunction.h"
#include <vector>
#include <string>

class MSLane;
class MSJunctionLogic;
class findCompetitor;

/**
 */
class MSRightOfWayJunction : public MSLogicJunction
{
public:
    /** Holds the information the incoming lanes. Drive- and
        Brakerequest will be used to distinguish between main- and
        sideroad-links if necessary (e.g. if a prioritized vehicle
        has no drive request but it's brake distance is past the
        junction it may block sideroad vehicles. */
    class InLane
    {
    public:
        friend class MSRightOfWayJunction;
        friend class MSTrafficLightJunction;
        friend class findCompetitor;

        InLane( MSLane* inLane );

    private:
        MSLane* myLane;

        // Used for brake-request-conflicts
        bool myDriveRequest;
        bool myBrakeRequest;

        InLane();
    };

    /// Destructor.
    virtual ~MSRightOfWayJunction();

    /** Container for incoming lanes. */
    typedef std::vector< InLane* > InLaneCont;

    /** Use this constructor only. */
    MSRightOfWayJunction( std::string id,
                          double x, double y,
                          InLaneCont* in,
                          MSJunctionLogic* logic );

    /** Clears junction's and lane's requests to prepare for the next
        iteration. */
    bool clearRequests();

    /** Call all lanes in myLanes and let them set their first vehicle's
        requests. Before, empty old requests/responds. */
    bool setFirstVehiclesRequests();

    /** Collect the first car's requests, calculate the respond
        according to the right of way rules and move the vehicles on
        their lane resp. set them in the succeeding lane's buffer. */
    virtual bool moveFirstVehicles();

    /** Integrate the moved vehicles into their target-lane. This is
        neccessary if you use not thread-safe containers. */
    bool vehicles2targetLane();

protected:
    /** Collects the previously set requests and stores
        them in myCurrRequest and in myInLanes. */
    virtual void collectRequests();

    /** Junction's in-lanes. */
    InLaneCont* myInLanes;

    /** Current request. */
    Request myRequest;

     /** Current respond. */
    Respond myRespond;

    /** the type of the junction (its logic) */
    MSJunctionLogic* myLogic;

    /// Search for deadlock-situations and eleminate them.
    virtual void deadlockKiller();

    /** Tells myInLanes to move the first vehicles
        according to the calculated myCurrRespond. */
    void moveVehicles();

private:

    /** Main road vehicles may have brake requests without drive
        requests. This can affect the right of way of side road
        vehicles. This method will modify myRespond if necessary. */
    void brakeReqConfl();

    /** Checks, if a conflicting side road vehicle is allowed to
        drive even if a main road vehicle has a brake request (but no
        drive request. */

    bool drivePermit( const MSLane* prio, const MSLane* out, const
                      MSLane* compete );

    /// Default constructor.
    MSRightOfWayJunction();

    /// Copy constructor.
    MSRightOfWayJunction( const MSRightOfWayJunction& );

    /// Assignment operator.
    MSRightOfWayJunction& operator=( const MSRightOfWayJunction& );
};


/** Function Object for use with Function Adater on vehicle
    containers. */
class findCompetitor
{
public:
    typedef const MSRightOfWayJunction::InLane* first_argument_type;
    typedef const
    std::pair< const MSLane*, const MSLane* > second_argument_type;
    typedef bool result_type;

    /** Returns true if there is a yield road's first vehicle
        that has the same succ-lane. There may be more than
        one. Check their drive-permission outside. */
    result_type operator() ( first_argument_type competeLane,
                             second_argument_type inOut ) const;
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "MSRightOfWayJunction.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:










