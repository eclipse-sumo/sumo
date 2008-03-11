/****************************************************************************/
/// @file    MSBusStop.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 13.12.2005
/// @version $Id$
///
// A lane area vehicles can halt at
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

#include <cassert>
#include "MSTrigger.h"
#include "MSBusStop.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSBusStop::MSBusStop(const std::string &id,
                     const std::vector<std::string> &lines,
                     MSLane &lane,
                     SUMOReal begPos, SUMOReal endPos) throw()
        : MSTrigger(id), myLines(lines), myLane(lane),
        myBegPos(begPos), myEndPos(endPos), myLastFreePos(endPos)
{
    computeLastFreePos();
}


MSBusStop::~MSBusStop() throw()
{}


const MSLane &
MSBusStop::getLane() const throw()
{
    return myLane;
}


SUMOReal
MSBusStop::getBeginLanePosition() const throw()
{
    return myBegPos;
}


SUMOReal
MSBusStop::getEndLanePosition() const throw()
{
    return myEndPos;
}


void
MSBusStop::enter(void *what, SUMOReal beg, SUMOReal end) throw()
{
    myEndPositions[what] = std::pair<SUMOReal, SUMOReal>(beg, end);
    computeLastFreePos();
}


SUMOReal
MSBusStop::getLastFreePos() const throw()
{
    return myLastFreePos;
}


void
MSBusStop::leaveFrom(void *what) throw()
{
    assert(myEndPositions.find(what)!=myEndPositions.end());
    myEndPositions.erase(myEndPositions.find(what));
    computeLastFreePos();
}


void
MSBusStop::computeLastFreePos() throw()
{
    myLastFreePos = myEndPos;
    std::map<void*, std::pair<SUMOReal, SUMOReal> >::iterator i;
    for (i=myEndPositions.begin(); i!=myEndPositions.end(); i++) {
        if (myLastFreePos>(*i).second.second) {
            myLastFreePos = (*i).second.second;
        }
    }
}



/****************************************************************************/

