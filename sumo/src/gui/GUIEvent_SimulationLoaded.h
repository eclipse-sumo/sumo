#ifndef GUIEvent_SimulationLoaded_h
#define GUIEvent_SimulationLoaded_h
//---------------------------------------------------------------------------//
//                        GUIEvent_SimulationLoaded.h -
//  Event send when the simulation has been loaded by GUILadThread
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
// Revision 1.5  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/02/01 10:10:38  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.3  2004/11/23 10:11:33  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.2  2004/04/02 11:10:20  dkrajzew
// simulation-wide output files are now handled by MSNet directly
//
// Revision 1.1  2004/03/19 12:56:11  dkrajzew
// porting to FOX
//
// Revision 1.3  2003/02/07 10:34:15  dkrajzew
// files updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <utils/gui/events/GUIEvent.h>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUINet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * GUIEvent_SimulationLoaded
 * Throw to GUIApplicationWindow from GUILoadThread after a simulation has
 * been loaded or the loading process failed
 */
class GUIEvent_SimulationLoaded : public GUIEvent {
public:
    /// constructor
    GUIEvent_SimulationLoaded(GUINet *net,
        SUMOTime startTime, SUMOTime endTime,
        const std::string &file)
        : GUIEvent(EVENT_SIMULATION_LOADED),
        _net(net), _begin(startTime), _end(endTime),
        _file(file) { }

    /// destructor
    ~GUIEvent_SimulationLoaded() { }

public:
    /// the loaded net
    GUINet          *_net;

    /// the time the simulation shall start with
    SUMOTime     _begin;

    /// the time the simulation shall end with
    SUMOTime     _end;

    /// the name of the loaded file
    std::string     _file;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

