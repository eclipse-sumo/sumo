/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIEvent_SimulationStep.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Event send when the a simulation step has been performed by GUIRunThread
/****************************************************************************/
#ifndef GUIEvent_SimulationStep_h
#define GUIEvent_SimulationStep_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GUIEvent.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * GUIEvent_SimulationStep
 * Throw from GUIRunThread to GUIApplicationWindow and then further to all
 * displays after a step has been performed
 */
class GUIEvent_SimulationStep : public GUIEvent {
public:
    /// constructor
    GUIEvent_SimulationStep()
        : GUIEvent(EVENT_SIMULATION_STEP) { }

    /// destructor
    ~GUIEvent_SimulationStep() { }

};


#endif

/****************************************************************************/

