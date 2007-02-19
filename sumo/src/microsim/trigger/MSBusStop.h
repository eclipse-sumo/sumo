/****************************************************************************/
/// @file    MSBusStop.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 13.12.2005
/// @version $Id$
///
// A point vehicles can halt at.
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
#ifndef MSBusStop_h
#define MSBusStop_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <string>
#include "MSTrigger.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSBusStop
 * @brief A point vehicles can halt at.
 */
class MSBusStop : public MSTrigger
{
public:
    /// Constructor
    MSBusStop(const std::string &id,
              const std::vector<std::string> &lines, MSLane &lane,
              SUMOReal begPos, SUMOReal endPos);

    /// Destructor
    virtual ~MSBusStop();

    /// Returns the lane this bus stop is located at
    const MSLane &getLane() const;

    /// Returns the begin position of this bus stop
    SUMOReal getBeginLanePosition() const;

    /// Returns the end position of this bus stop
    SUMOReal getEndLanePosition() const;

    /// Called if a vehicle enters this stop
    void enter(void *what, SUMOReal beg, SUMOReal end);

    /// Called if a vehicle leaves this stop
    void leaveFrom(void *what);

    /// Returns the last free position on this stop
    SUMOReal getLastFreePos() const;

protected:
    /// Computes the last free position on this stop
    void computeLastFreePos();

protected:
    /// The list of lines that are assigned to this stop
    std::vector<std::string> myLines;

    /// A map from objects (vehicles) to the areas they acquire after entering the stop
    std::map<void*, std::pair<SUMOReal, SUMOReal> > myEndPositions;

    /// The lane this bus stop is located at
    MSLane &myLane;

    /// The begin position this bus stop is located at
    SUMOReal myBegPos;

    /// The end position this bus stop is located at
    SUMOReal myEndPos;

    /// The last free position at this stop
    SUMOReal myLastFreePos;

};


#endif

/****************************************************************************/

