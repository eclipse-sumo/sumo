/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIGlobals.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    2004
///
// Some global variables for the GUI
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIGlobals
 * This class holds some static variables, filled mostly with values coming
 *  from the command line, application settings or the registry.
 */
class GUIGlobals {
public:
    /// the simulation shall start direct after loading
    static bool gRunAfterLoad;

    /// the window shall be closed when the simulation has ended
    static bool gQuitOnEnd;

    /// the simulation shall reload when it has ended (demo)
    static bool gDemoAutoReload;

    /// the aggregation period for tracker windows in seconds
    static double gTrackerInterval;

    /// whether secondary shapes are currently being drawn
    static bool gSecondaryShape;
};
