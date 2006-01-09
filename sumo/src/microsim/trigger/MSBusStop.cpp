/***************************************************************************
                          MSBusStop.cpp  -
                          A point vehicles can halt at.
                             -------------------
    begin                : Mon, 13.12.2005
    copyright            : (C) 2005 by DLR/ZAIK (http://ivf.dlr.de)
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

namespace
{
    const char rcsid[] =
    "$Id$";
}

// $Log$
// Revision 1.1  2006/01/09 11:53:00  dkrajzew
// bus stops implemented
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cassert>
#include "MSTrigger.h"
#include "MSBusStop.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSBusStop::MSBusStop(const std::string &id,
                     const std::vector<std::string> &lines,
                     MSLane &lane,
                     SUMOReal begPos, SUMOReal endPos)
    : MSTrigger(id), myLines(lines), myLane(lane),
    myBegPos(begPos), myEndPos(endPos), myLastFreePos(endPos)
{
    computeLastFreePos();
}


MSBusStop::~MSBusStop()
{
}


const MSLane &
MSBusStop::getLane() const
{
    return myLane;
}


SUMOReal
MSBusStop::getBeginLanePosition() const
{
    return myBegPos;
}


SUMOReal
MSBusStop::getEndLanePosition() const
{
    return myEndPos;
}


void
MSBusStop::enter(void *what, SUMOReal beg, SUMOReal end)
{
    myEndPositions[what] = std::pair<SUMOReal, SUMOReal>(beg, end);
    computeLastFreePos();
//    myLeftPlace = myEndPos - myLastFreePos;
}


SUMOReal
MSBusStop::getLastFreePos() const
{
    return myLastFreePos;
}


void
MSBusStop::leaveFrom(void *what)
{
    assert(myEndPositions.find(what)!=myEndPositions.end());
    myEndPositions.erase(myEndPositions.find(what));
    computeLastFreePos();
}


void
MSBusStop::computeLastFreePos()
{
    myLastFreePos = myEndPos;
    std::map<void*, std::pair<SUMOReal, SUMOReal> >::iterator i;
    for(i=myEndPositions.begin(); i!=myEndPositions.end(); i++) {
        if(myLastFreePos>(*i).second.second) {
            myLastFreePos = (*i).second.second;
        }
    }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
