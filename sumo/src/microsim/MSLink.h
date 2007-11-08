/****************************************************************************/
/// @file    MSLink.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The link between two lanes
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
#ifndef MSLink_h
#define MSLink_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLogicJunction.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSTrafficLightLogic;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLinks
 * Representation of the connnection between lanes.
 */
class MSLink
{
public:
    /**
     * @enum LinkState
     * This enumerations holds the possible right-of-way rules a link
     * may have.
     * Beyond the righ-of-way rules, this enumeration also holds the
     * possible traffic light states.
     */
    enum LinkState {
        LINKSTATE_TL_GREEN,
        LINKSTATE_TL_RED,
        LINKSTATE_TL_YELLOW,
        LINKSTATE_TL_OFF_BLINKING,
        LINKSTATE_TL_OFF_NOSIGNAL,
        LINKSTATE_MAJOR,
        LINKSTATE_MINOR,
        LINKSTATE_EQUAL,
        LINKSTATE_DEADEND
    };

    /**
     * @enum LinkDirection
     * The different directions a link may take.
     */
    enum LinkDirection {
        LINKDIR_STRAIGHT = 0,
        LINKDIR_TURN,
        LINKDIR_LEFT,
        LINKDIR_RIGHT,
        LINKDIR_PARTLEFT,
        LINKDIR_PARTRIGHT,
        LINKDIR_NODIR
    };
#ifndef HAVE_INTERNAL_LANES
    /// Constructor
    MSLink(MSLane* succLane,
           bool yield, LinkDirection dir, LinkState state);
#else
    /// Constructor
    MSLink(MSLane* succLane, MSLane *via,
           bool yield, LinkDirection dir, LinkState state, bool internalEnd);
#endif

    /// Destructor
    ~MSLink();

    /// sets the request information
    void setRequestInformation(
        MSLogicJunction::Request *request, size_t requestIdx,
        MSLogicJunction::Respond *respond, size_t respondIdx);

    /// sets the information about an approaching vehicle
    void setApproaching(MSVehicle *approaching);
    MSVehicle *getApproaching() const {
        return myApproaching;
    };

    /// Some Junctions need to switch the priority
    void setPriority(bool prio);

    /** @brief Returns the information whether the link may be passed
        A valid after the junctions have set their reponds */
    bool opened() const;

    /** @brief Removes the incoming vehicle's request
        Necessary to mask out vehicles having yellow or red. */
    void deleteRequest();

    /// returns the current state of the link
    LinkState getState() const;

    /// Returns the direction the vehicle passing this link take
    LinkDirection getDirection() const;

    /// Sets the current tl-state
    void setTLState(LinkState state);

    /// Returns the connected lane
    MSLane *getLane() const;

    /// Return the respond index (for visualization)
    size_t getRespondIndex() const;

#ifdef HAVE_INTERNAL_LANES
    /// Returns the innerlane following
    MSLane * const getViaLane() const;

    void resetInternalPriority();

#endif

    bool havePriority() const;
    bool isApproached() const;

private:
    /// MSLink's destination lane.
    MSLane* myLane;

#ifdef HAVE_INTERNAL_LANES
    /// The following junction-internal lane if used
    MSLane * const myJunctionInlane;
#endif

    /// MSLinks's default right of way, true for right of way MSLinks.
    bool myPrio;

    /// the approaching vehicle
    MSVehicle *myApproaching;

    /// the request to set incoming request into
    MSLogicJunction::Request *myRequest;

    /// the position of the link within this request
    size_t myRequestIdx;

    /// the respond to read whether the car may drive from
    MSLogicJunction::Respond *myRespond;

    /// the position within this respond
    size_t myRespondIdx;

    /// The basic state of the link
    LinkState myState;

    /// An abstract (hopefully human readable) definition of the link's direction
    LinkDirection myDirection;

#ifdef HAVE_INTERNAL_LANES
    bool myIsInternalEnd;
#endif

private:
    /// invalidated copy constructor
    MSLink(const MSLink &s);

    /// invalidated assignment operator
    MSLink &operator=(const MSLink &s);

};


#endif

/****************************************************************************/

