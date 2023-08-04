/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNENamespaces.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// Class for unify namespaces in netedit
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include "GNENamespaces.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNENamespaces::GNENamespaces() :
    busStops({SUMO_TAG_BUS_STOP, SUMO_TAG_TRAIN_STOP}),
    laneAreaDetectors({SUMO_TAG_LANE_AREA_DETECTOR, GNE_TAG_MULTI_LANE_AREA_DETECTOR}),
    calibrators({SUMO_TAG_CALIBRATOR, GNE_TAG_CALIBRATOR_LANE}),
    polygons({SUMO_TAG_POLY, SUMO_TAG_TAZ, GNE_TAG_JPS_WALKABLEAREA, GNE_TAG_JPS_OBSTACLE, GNE_TAG_JPS_WAITINGAREA, GNE_TAG_JPS_SOURCE, GNE_TAG_JPS_SINK}),
    POIs({SUMO_TAG_POI, GNE_TAG_POILANE, GNE_TAG_POIGEO, GNE_TAG_JPS_WAYPOINT}),
    vehicles({}),
    persons({}),
    containers({}) {
}


GNENamespaces::~GNENamespaces() {}

/****************************************************************************/
