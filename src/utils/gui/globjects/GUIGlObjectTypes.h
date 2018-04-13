/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIGlObjectTypes.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A list of object types which may be displayed within the gui
// each type has an associated string which will be prefefixed to an object id
// when constructing the full name
/****************************************************************************/
#ifndef GUIGlObjectTypes_h
#define GUIGlObjectTypes_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// definitions
// ===========================================================================
/**
 * ChooseableArtifact
 * Enumeration to differ to show the list of which artefact
 * The order is important during the selection of items for displaying their
 * tooltips; the item with the hightest type value will be chosen.
 */
enum GUIGlObjectType {
    /// @brief The network - empty
    GLO_NETWORK = 0,
    /// @brief an edge
    GLO_EDGE = 1,
    /// @brief a lane
    GLO_LANE = 2,
    /// @brief a junction
    GLO_JUNCTION = 3,
    /// @brief a tl-logic
    GLO_CROSSING = 4,
    /// @brief a connection
    GLO_CONNECTION = 5,
    /// @brief a prohibition
    GLO_PROHIBITION = 6,
    /// @brief a tl-logic
    GLO_TLLOGIC = 7,
    /// @brief a busStop
    GLO_BUS_STOP = 100,
    /// @brief a containerStop
    GLO_CONTAINER_STOP = 101,
    /// @brief a chargingStation
    GLO_CHARGING_STATION = 102,
    /// @brief a ParkingArea
    GLO_PARKING_AREA = 103,
    /// @brief a ParkingSpace
    GLO_PARKING_SPACE = 104,
    /// @brief a E1 detector
    GLO_E1DETECTOR = 105,
    /// @brief a E1 detector
    GLO_E1DETECTOR_ME = 105,
    /// @brief a E1 detector
    GLO_E1DETECTOR_INSTANT = 105,
    /// @brief a E2 detector
    GLO_E2DETECTOR = 106,
    /// @brief a E3 detector
    GLO_E3DETECTOR = 107,
    /// @brief a DetEntry detector
    GLO_DET_ENTRY = 108,
    /// @brief a DetExit detector
    GLO_DET_EXIT = 109,
    /// @brief a Rerouter
    GLO_REROUTER = 110,
    /// @brief a Rerouter
    GLO_REROUTER_EDGE = 111,
    /// @brief a Variable Speed Sign
    GLO_VSS = 112,
    /// @brief a Calibrator
    GLO_CALIBRATOR = 113,
    /// @brief a RouteProbe
    GLO_ROUTEPROBE = 114,
    /// @brief a Vaporizer
    GLO_VAPORIZER = 115,
    /// @brief a polygon
    GLO_POLYGON = 201,
    /// @brief a poi
    GLO_POI = 202,
    /// @brief a vehicles
    GLO_VEHICLE = 301,
    /// @brief a person
    GLO_PERSON = 302,
    /// @brief a container
    GLO_CONTAINER = 303,
    /// @brief empty max
    GLO_MAX = 2048
};

#endif

/****************************************************************************/
