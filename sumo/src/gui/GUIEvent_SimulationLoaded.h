/****************************************************************************/
/// @file    GUIEvent_SimulationLoaded.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Event send when the simulation has been loaded by GUILadThread
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIEvent_SimulationLoaded_h
#define GUIEvent_SimulationLoaded_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
 * @class  GUIEvent_SimulationLoaded
 *
 * Throw to GUIApplicationWindow from GUILoadThread after a simulation has
 * been loaded or the loading process failed
 */
class GUIEvent_SimulationLoaded : public GUIEvent {
public:
    /// constructor
    GUIEvent_SimulationLoaded(GUINet* net,
                              SUMOTime startTime, SUMOTime endTime,
                              const std::string& file,
                              const std::vector<std::string>& settingsFiles,
                              const bool osgView)
        : GUIEvent(EVENT_SIMULATION_LOADED),
          myNet(net), myBegin(startTime), myEnd(endTime),
          myFile(file), mySettingsFiles(settingsFiles),
          myOsgView(osgView)
    { }

    /// destructor
    ~GUIEvent_SimulationLoaded() { }

public:
    /// the loaded net
    GUINet*  myNet;

    /// the time the simulation shall start with
    const SUMOTime myBegin;

    /// the time the simulation shall end with
    const SUMOTime myEnd;

    /// the name of the loaded file
    const std::string myFile;

    /// the name of the settings file to load
    const std::vector<std::string> mySettingsFiles;

    /// whether to load the OpenSceneGraph view
    const bool myOsgView;

private:
    /// @brief Invalidated assignment operator
    GUIEvent_SimulationLoaded& operator=(const GUIEvent_SimulationLoaded& s);
};


#endif

/****************************************************************************/

