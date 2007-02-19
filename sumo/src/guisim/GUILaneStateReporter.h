/****************************************************************************/
/// @file    GUILaneStateReporter.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.04.2005
/// @version $Id$
///
//
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
#ifndef GUILaneStateReporter_h
#define GUILaneStateReporter_h
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

#include <microsim/output/e2_detectors/MSE2Collector.h>
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <microsim/MSUpdateEachTimestep.h>


// ===========================================================================
// class definitions
// ===========================================================================
class GUILaneStateReporter : public MSE2Collector,
            public MSUpdateEachTimestep<GUILaneStateReporter>
{
public:
    /// Constructor
    GUILaneStateReporter(LoggedValue_TimeFloating<SUMOReal> *densityRetriever,
                         LoggedValue_TimeFloating<SUMOReal> *speedRetriever,
                         LoggedValue_TimeFloating<SUMOReal> *haltingDurRetriever,
                         SUMOReal &floatingDensity, SUMOReal &floatingSpeed, SUMOReal &floatingHaltings,
                         const std::string &id, MSLane *lane, SUMOTime interval);
    /// Destructor
    ~GUILaneStateReporter();
    bool updateEachTimestep(void);
protected:
    LoggedValue_TimeFloating<SUMOReal> *myDensityRetriever;
    LoggedValue_TimeFloating<SUMOReal> *mySpeedRetriever;
    LoggedValue_TimeFloating<SUMOReal> *myHaltingDurRetriever;
    SUMOReal &myFloatingDensity;
    SUMOReal &myFloatingSpeed;
    SUMOReal &myFloatingHaltings;

};


#endif

/****************************************************************************/

