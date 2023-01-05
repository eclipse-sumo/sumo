/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2016-2017 Dresden University of Technology
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
/// @file    SumoTrafficLightType.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
///
//
/****************************************************************************/
package de.tudresden.ws.container;

/**
 * 
 * @deprecated use package {@link de.tudresden.sumo.objects} instead.
 */

public enum SumoTrafficLightType {

	    STATIC(0),
	    RAIL_SIGNAL(1),
	    RAIL_CROSSING(2),
	    ACTUATED(3),
	    DELAYBASED(4),
	    SOTL_PHASE(5),
	    SOTL_PLATOON(6),
	    SOTL_REQUEST(7),
	    SOTL_WAVE(8),
	    SOTL_MARCHING(9),
	    SWARM_BASED(10),
	    HILVL_DETERMINISTIC(11),
	    OFF(12),
	    INVALID(13);
	
	int index;
	
	SumoTrafficLightType(int index){this.index = index;}
	
	public int getIndex() {return this.index;}
	
}
