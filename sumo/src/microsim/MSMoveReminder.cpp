/****************************************************************************/
/// @file    MSMoveReminder.cpp
/// @author  Daniel Krajzewicz
/// @date    2008-10-27
/// @version $Id: MSMoveReminder.h 5048 2008-02-07 12:15:22Z behrisch $
///
// Something on a lane to be noticed about vehicle movement
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <cassert>
#include "MSLane.h"
#include "MSMoveReminder.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSMoveReminder::MSMoveReminder(MSLane * const lane) throw()
        : myLane(lane)
{
    if (myLane!=0) {
        // add reminder to lane
        myLane->addMoveReminder(this);
    }
}


/****************************************************************************/

