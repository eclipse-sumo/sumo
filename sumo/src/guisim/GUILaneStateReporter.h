#ifndef GUILaneStateReporter_h
#define GUILaneStateReporter_h
//---------------------------------------------------------------------------//
//                        GUILaneStateReporter.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.12  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.11  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <microsim/output/e2_detectors/MSE2Collector.h>
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <microsim/MSUpdateEachTimestep.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUILaneStateReporter : public MSE2Collector,
    public MSUpdateEachTimestep<GUILaneStateReporter>
{
public:
	/// Constructor
    GUILaneStateReporter(LoggedValue_TimeFloating<SUMOReal> *densityRetriever,
        LoggedValue_TimeFloating<SUMOReal> *speedRetriever,
        LoggedValue_TimeFloating<SUMOReal> *haltingDurRetriever,
        SUMOReal &SUMORealingDensity, SUMOReal &SUMORealingSpeed, SUMOReal &SUMORealingHaltings,
        const std::string &id, MSLane *lane, SUMOTime interval);
	/// Destructor
    ~GUILaneStateReporter();
    bool updateEachTimestep( void );
protected:
    LoggedValue_TimeFloating<SUMOReal> *myDensityRetriever;
    LoggedValue_TimeFloating<SUMOReal> *mySpeedRetriever;
    LoggedValue_TimeFloating<SUMOReal> *myHaltingDurRetriever;
    SUMOReal &myFloatingDensity;
    SUMOReal &myFloatingSpeed;
    SUMOReal &myFloatingHaltings;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

