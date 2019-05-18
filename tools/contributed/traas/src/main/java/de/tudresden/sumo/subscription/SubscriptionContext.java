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
/// @file    SubscriptionContext.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.sumo.subscription;

import de.tudresden.sumo.config.Constants;

public enum SubscriptionContext {

    areal_detector(Constants.CMD_SUBSCRIBE_LANEAREA_CONTEXT),
    inductive_loop(Constants.CMD_SUBSCRIBE_INDUCTIONLOOP_CONTEXT),
    me_detector(Constants.CMD_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT),
    lane(Constants.CMD_SUBSCRIBE_LANE_CONTEXT),
    vehicle(Constants.CMD_SUBSCRIBE_VEHICLE_CONTEXT),
    vehicleType(Constants.CMD_SUBSCRIBE_VEHICLETYPE_CONTEXT),
    poi(Constants.CMD_SUBSCRIBE_POI_CONTEXT),
    person(Constants.CMD_SUBSCRIBE_PERSON_CONTEXT),
    route(Constants.CMD_SUBSCRIBE_ROUTE_CONTEXT),
    simulation(Constants.CMD_SUBSCRIBE_SIM_CONTEXT),
    trafficlight(Constants.CMD_SUBSCRIBE_TL_CONTEXT),
    gui(Constants.CMD_SUBSCRIBE_GUI_CONTEXT),
    polygon(Constants.CMD_SUBSCRIBE_POLYGON_CONTEXT),
    junction(Constants.CMD_SUBSCRIBE_JUNCTION_CONTEXT),
    edge(Constants.CMD_SUBSCRIBE_EDGE_CONTEXT);

    int id;
    SubscriptionContext(int id) {
        this.id = id;
    }

    public int getID() {
        return this.id;
    }

}
