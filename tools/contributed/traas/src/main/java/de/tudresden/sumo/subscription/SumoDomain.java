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
/// @file    SumoDomain.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.sumo.subscription;

import de.tudresden.sumo.config.Constants;

public enum SumoDomain {

    inductive_loops(Constants.CMD_GET_INDUCTIONLOOP_VARIABLE),
    lanes(Constants.CMD_GET_LANE_VARIABLE),
    vehicles(Constants.CMD_GET_VEHICLE_VARIABLE),
    persons(Constants.CMD_GET_PERSON_VARIABLE),
    poi(Constants.CMD_GET_POI_VARIABLE),
    polygons(Constants.CMD_GET_POLYGON_VARIABLE),
    junction(Constants.CMD_GET_JUNCTION_VARIABLE),
    edges(Constants.CMD_GET_EDGE_VARIABLE);

    int id;

    SumoDomain(int id) {
        this.id = id;
    }

    public int getID() {
        return this.id;
    }

}
