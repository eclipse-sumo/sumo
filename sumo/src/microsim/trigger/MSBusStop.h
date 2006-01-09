#ifndef MSBusStop_h
#define MSBusStop_h
/***************************************************************************
                          MSBusStop.h  -
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
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <map>
#include <string>
#include "MSTrigger.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSBusStop
 * @brief A point vehicles can halt at.
 */
class MSBusStop : public MSTrigger {
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


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
