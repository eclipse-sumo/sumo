/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2016-2017 Dresden University of Technology
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SumoVehicleSignalState.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.ws.container;

/**
 *
 * @author Mario Krumnow
 * @author Anja Liebscher
 *
 */

public enum SumoVehicleSignalState implements SumoObject {

    VEH_SIGNAL_BLINKER_RIGHT(0),
    VEH_SIGNAL_BLINKER_LEFT(1),
    VEH_SIGNAL_BLINKER_EMERGENCY(2),
    VEH_SIGNAL_BRAKELIGHT(3),
    VEH_SIGNAL_FRONTLIGHT(4),
    VEH_SIGNAL_FOGLIGHT(5),
    VEH_SIGNAL_HIGHBEAM(6),
    VEH_SIGNAL_BACKDRIVE(7),
    VEH_SIGNAL_WIPER(8),
    VEH_SIGNAL_DOOR_OPEN_LEFT(9),
    VEH_SIGNAL_DOOR_OPEN_RIGHT(10),
    VEH_SIGNAL_EMERGENCY_BLUE(11),
    VEH_SIGNAL_EMERGENCY_RED(12),
    VEH_SIGNAL_EMERGENCY_YELLOW(13);

    private final int pos;

    SumoVehicleSignalState(int pos) {
        this.pos = pos;
    }

    public int getPos() {
        return this.pos;
    }

}
