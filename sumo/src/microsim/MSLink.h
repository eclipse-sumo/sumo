#ifndef MSLink_h
#define MSLink_h
//---------------------------------------------------------------------------//
//                        MSLink.h -
//  The link between two lanes
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.16  2006/01/11 11:54:35  dkrajzew
// reworked possible link states; new link coloring
//
// Revision 1.15  2005/11/09 06:39:38  dkrajzew
// usage of internal lanes is now optional at building
//
// Revision 1.14  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.13  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.12  2004/02/16 14:23:02  dkrajzew
// copy constructor and assignment operator invalidated
//
// Revision 1.11  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.10  2003/11/12 13:50:30  dkrajzew
// MSLink-members are now secured from the outer world
//
// Revision 1.9  2003/09/05 15:11:43  dkrajzew
// first steps for reading of internal lanes
//
// Revision 1.8  2003/08/21 12:52:15  dkrajzew
// lane2lane connection display added
//
// Revision 1.7  2003/08/04 11:42:35  dkrajzew
// missing deletion of traffic light logics on closing a network added
//
// Revision 1.6  2003/07/30 09:09:55  dkrajzew
// added end-of-link definition (direction, type) for visualisation
//
// Revision 1.5  2003/06/05 16:05:41  dkrajzew
// removal of links request added; needed by new traffic lights
//
// Revision 1.4  2003/05/21 15:15:42  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.3  2003/04/14 08:33:00  dkrajzew
// some further bugs removed
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSLogicJunction.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class MSTrafficLightLogic;


/* =========================================================================
 * class definitions
 * ======================================================================= */
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
    MSLink( MSLane* succLane,
        bool yield, LinkDirection dir, LinkState state, bool internalEnd );
#else
    /// Constructor
    MSLink( MSLane* succLane, MSLane *via,
        bool yield, LinkDirection dir, LinkState state, bool internalEnd );
#endif

    /// Destructor
    ~MSLink();

    /// sets the request information
    void setRequestInformation(
        MSLogicJunction::Request *request, size_t requestIdx,
        MSLogicJunction::Respond *respond, size_t respondIdx);

    /// sets the information about an approaching vehicle
    void setApproaching(MSVehicle *approaching);

    /// Some Junctions need to switch the priority
    void setPriority( bool prio, bool yellow );

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

    /// Returns whether the tl (if this link belongs to one) shows yellow
    bool amYellow() const;

    /// Returns the connected lane
    MSLane *getLane() const;

#ifdef HAVE_INTERNAL_LANES
    /// Returns the innerlane following
    MSLane * const getViaLane() const;
#endif


    bool havePriority() const;

private:
    bool myIsInternalEnd;

    /// the request to set incoming request into
    MSLogicJunction::Request *myRequest;

    /// the position of the link within this request
    size_t myRequestIdx;

    /// the respond to read whether the car may drive from
    MSLogicJunction::Respond *myRespond;

    /// the position within this respond
    size_t myRespondIdx;

    /// An abstract (hopefully human readable) definition of the link's direction
    LinkDirection myDirection;

	/// Information whether the tl (if this link belongs to one) shows yellow
	bool myAmYellow; // !!! deprecated

    /// The basic state of the link
    LinkState myState;

    /// the approaching vehicle
    MSVehicle *myApproaching;

    /// MSLink's destination lane.
    MSLane* myLane;

#ifdef HAVE_INTERNAL_LANES
    /// The following junction-internal lane if used
    MSLane * const myJunctionInlane;
#endif

    /// MSLinks's default right of way, true for right of way MSLinks.
    bool myPrio;

private:
    /// invalidated copy constructor
    MSLink(const MSLink &s);

    /// invalidated assignment operator
    MSLink &operator=(const MSLink &s);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

