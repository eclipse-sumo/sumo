/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    NamespaceIDs.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// Class for unify namespace IDs (elements that share the same namespace ID)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include "NamespaceIDs.h"


// ===========================================================================
// method definitions
// ===========================================================================


const std::vector<SumoXMLTag> NamespaceIDs::busStops = {
    SUMO_TAG_BUS_STOP,
    SUMO_TAG_TRAIN_STOP
};

const std::vector<SumoXMLTag> NamespaceIDs::laneAreaDetectors = {
    SUMO_TAG_LANE_AREA_DETECTOR,
    GNE_TAG_MULTI_LANE_AREA_DETECTOR
};

const std::vector<SumoXMLTag> NamespaceIDs::calibrators = {
    SUMO_TAG_CALIBRATOR,
    GNE_TAG_CALIBRATOR_LANE
};

const std::vector<SumoXMLTag> NamespaceIDs::polygons = {
    SUMO_TAG_POLY,
    SUMO_TAG_TAZ,
    GNE_TAG_JPS_WALKABLEAREA,
    GNE_TAG_JPS_OBSTACLE
};

const std::vector<SumoXMLTag> NamespaceIDs::POIs = {
    SUMO_TAG_POI,
    GNE_TAG_POILANE,
    GNE_TAG_POIGEO
};

const std::vector<SumoXMLTag> NamespaceIDs::types = {
    SUMO_TAG_VTYPE,
    SUMO_TAG_VTYPE_DISTRIBUTION
};

const std::vector<SumoXMLTag> NamespaceIDs::routes = {
    SUMO_TAG_ROUTE,
    SUMO_TAG_ROUTE_DISTRIBUTION
};

const std::vector<SumoXMLTag> NamespaceIDs::vehicles = {
    SUMO_TAG_VEHICLE, SUMO_TAG_TRIP,
    GNE_TAG_VEHICLE_WITHROUTE,
    GNE_TAG_TRIP_JUNCTIONS,
    GNE_TAG_TRIP_TAZS,
    GNE_TAG_FLOW_ROUTE,
    SUMO_TAG_FLOW,
    GNE_TAG_FLOW_WITHROUTE,
    GNE_TAG_FLOW_JUNCTIONS,
    GNE_TAG_FLOW_TAZS
};

const std::vector<SumoXMLTag> NamespaceIDs::persons = {
    SUMO_TAG_PERSON,
    SUMO_TAG_PERSONFLOW
};

const std::vector<SumoXMLTag> NamespaceIDs::containers = {
    SUMO_TAG_CONTAINER,
    SUMO_TAG_CONTAINERFLOW
};

const std::vector<SumoXMLTag> NamespaceIDs::stops = {
    GNE_TAG_STOP_LANE,
    GNE_TAG_STOP_BUSSTOP,
    GNE_TAG_STOP_TRAINSTOP,
    GNE_TAG_STOP_CONTAINERSTOP,
    GNE_TAG_STOP_CHARGINGSTATION,
    GNE_TAG_STOP_PARKINGAREA
};

/****************************************************************************/
