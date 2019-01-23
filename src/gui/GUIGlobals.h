/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIGlobals.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id$
///
// Some global variables (yep)
/****************************************************************************/
#ifndef GUIGlobals_h
#define GUIGlobals_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSGlobals
 * This class holds some static variables, filled mostly with values coming
 *  from the command line or the simulation configuration file.
 * They are stored herein to allow a faster access than from the options
 *  container.
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
};

#endif

/****************************************************************************/

