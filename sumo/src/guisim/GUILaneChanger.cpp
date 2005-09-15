//---------------------------------------------------------------------------//
//                        GUILaneChanger.cpp -
//  The gui-version of the lane changer
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 11:37:41  dkrajzew
// level 3 warnings removed; code style adapted
//
// Revision 1.2  2003/06/05 11:40:28  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <microsim/MSEdge.h>
#include "GUILane.h"
#include "GUILaneChanger.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG



/* =========================================================================
 * method definitions
 * ======================================================================= */
GUILaneChanger::GUILaneChanger( MSEdge::LaneCont* lanes )
    : MSLaneChanger(lanes)
{
}


GUILaneChanger::~GUILaneChanger()
{
}

void
GUILaneChanger::updateLanes()
{
    // Update the lane's vehicle-container.
    // First: it is bad style to change other classes members, but for
    // this release, other attempts were too time-consuming. In a next
    // release we will change from this lane-centered design to a vehicle-
    // centered. This will solve many problems.
    // Second: this swap would be faster if vehicle-containers would have
    // been pointers, but then I had to change too much of the MSLane code.
    for ( ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce ) {
        ce->lane->swapAfterLaneChange();
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


