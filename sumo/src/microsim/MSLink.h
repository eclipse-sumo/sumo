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
    /// Constructor
    MSLink( MSLane* succLane, bool yield );

    /// sets the request information
    void setRequestInformation(MSLogicJunction::Request *request,
        size_t requestIdx, MSLogicJunction::Respond *respond,
        size_t respondIdx/*, const std::bitset<64> &previousClear*/);

    /// sets the information about an approaching vehicle
    void setApproaching(MSVehicle *approaching);

    /// Some Junctions need to switch the Priority
    void setPriority( bool prio );

    /** @brief Returns the information whether the link may be passed
        A valid after the junctions have set their reponds */
    bool opened() const;

    /// MSLink's destination lane.
    MSLane* myLane;

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

