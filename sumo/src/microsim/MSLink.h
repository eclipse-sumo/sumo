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
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSLogicJunction.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;

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
    enum LinkState {
        LINKSTATE_ABSTRACT_TL,
        LINKSTATE_TL_GREEN,
        LINKSTATE_TL_RED,
        LINKSTATE_TL_YELLOW,
        LINKSTATE_TL_OFF_BLINKING,
        LINKSTATE_TL_OFF_NOSIGNAL,
        LINKSTATE_MAJOR,
        LINKSTATE_MINOR,
        LINKSTATE_EQUAL
    };

    enum LinkDirection {
        LINKDIR_STRAIGHT = 0,
        LINKDIR_TURN,
        LINKDIR_LEFT,
        LINKDIR_RIGHT,
        LINKDIR_PARTLEFT,
        LINKDIR_PARTRIGHT
    };

    /// Constructor
    MSLink( MSLane* succLane, MSLane *via,
        bool yield, LinkDirection dir, LinkState state, bool internalEnd );

    /// Destructor
    ~MSLink();

    /// sets the request information
    void setRequestInformation(MSLogicJunction::Request *request,
        size_t requestIdx, MSLogicJunction::Respond *respond,
        size_t respondIdx/*, const std::bitset<64> &previousClear*/);

    /// sets the information about an approaching vehicle
    void setApproaching(MSVehicle *approaching);

    /// Some Junctions need to switch the Priority
    void setPriority( bool prio, bool yellow );

    /** @brief Returns the information whether the link may be passed
        A valid after the junctions have set their reponds */
    bool opened() const;


    void deleteRequest();

    LinkState getState() const;

    LinkDirection getDirection() const;

    void setTLState(LinkState state);

    /// MSLink's destination lane.
    MSLane* myLane;

    MSLane *myJunctionInlane;

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

	/// Information whether the tl (if this link belongs to one) shows yellow
	bool myAmYellow; // !!! deprecated

    /// Am abstract (hopefully human readable) definition of the link's direction
    LinkDirection myDirection;


private:
    bool myIsInternalEnd;
private:
    /// default constructor
    MSLink();

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSLink.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

