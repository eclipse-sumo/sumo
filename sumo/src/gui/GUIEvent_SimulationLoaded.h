/****************************************************************************/
/// @file    GUIEvent_SimulationLoaded.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// Event send when the simulation has been loaded by GUILadThread
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
#ifndef GUIEvent_SimulationLoaded_h
#define GUIEvent_SimulationLoaded_h
// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/gui/events/GUIEvent.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * GUIEvent_SimulationLoaded
 * Throw to GUIApplicationWindow from GUILoadThread after a simulation has
 * been loaded or the loading process failed
 */
class GUIEvent_SimulationLoaded : public GUIEvent
{
public:
    /// constructor
    GUIEvent_SimulationLoaded(GUINet *net,
                              SUMOTime startTime, SUMOTime endTime,
                              const std::string &file)
            : GUIEvent(EVENT_SIMULATION_LOADED),
            _net(net), _begin(startTime), _end(endTime),
            _file(file)
    { }

    /// destructor
    ~GUIEvent_SimulationLoaded()
    { }

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


#endif

/****************************************************************************/

