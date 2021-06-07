/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GUIGlObjectTypes.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A list of object types which may be displayed within the gui
// each type has an associated string which will be prefefixed to an object id
// when constructing the full name
/****************************************************************************/
#pragma once
#include <config.h>


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

    /// @name nettork elements
    /// @{

    /// @brief reserved GLO type to pack all network elements
    GLO_NETWORKELEMENT = 1,
    /// @brief an edge
    GLO_EDGE = 2,
    /// @brief a lane
    GLO_LANE = 3,
    /// @brief a junction
    GLO_JUNCTION = 4,
    /// @brief a connection
    GLO_CONNECTION = 5,
    /// @brief a tl-logic
    GLO_CROSSING = 6,
    /// @brief a tl-logic
    GLO_TLLOGIC = 7,
    /// @brief an edge_type
    GLO_TYPE = 8,

    /// @}

    /// @name data elements
    /// @{

    /// @brief edge data
    GLO_EDGEDATA = 50,
    /// @brief edge relation data
    GLO_EDGERELDATA = 51,
    /// @brief TAZ relation data
    GLO_TAZRELDATA = 52,

    /// @}

    /// @name additional elements
    /// @{

    /// @brief reserved GLO type to pack all additionals elements
    GLO_ADDITIONALELEMENT = 100,
    /// @brief a busStop
    GLO_BUS_STOP = 101,
    /// @brief a trainStop
    GLO_TRAIN_STOP = 102,
    /// @brief a containerStop
    GLO_CONTAINER_STOP = 103,
    /// @brief a chargingStation
    GLO_CHARGING_STATION = 104,
    /// @brief a ParkingArea
    GLO_PARKING_AREA = 105,
    /// @brief a ParkingSpace
    GLO_PARKING_SPACE = 106,
    /// @brief a E1 detector
    GLO_E1DETECTOR = 107,
    /// @brief a E1 detector
    GLO_E1DETECTOR_ME = 108,
    /// @brief a E1 detector
    GLO_E1DETECTOR_INSTANT = 109,
    /// @brief a E2 detector
    GLO_E2DETECTOR = 110,
    /// @brief a E3 detector
    GLO_E3DETECTOR = 111,
    /// @brief a DetEntry detector
    GLO_DET_ENTRY = 112,
    /// @brief a DetExit detector
    GLO_DET_EXIT = 113,
    /// @brief a Rerouter
    GLO_REROUTER = 114,
    /// @brief a Rerouter
    GLO_REROUTER_EDGE = 115,
    /// @brief a Variable Speed Sign
    GLO_VSS = 116,
    /// @brief a Calibrator
    GLO_CALIBRATOR = 117,
    /// @brief a RouteProbe
    GLO_ROUTEPROBE = 118,
    /// @brief a Vaporizer
    GLO_VAPORIZER = 119,
    /// @brief a Acces
    GLO_ACCESS = 120,
    /// @brief a segment of an overhead line
    GLO_OVERHEAD_WIRE_SEGMENT = 121,

    /// @}

    /// @name shape elements
    /// @{

    /// @brief reserved GLO type to pack shapes
    GLO_SHAPE = 200,
    /// @brief a polygon
    GLO_POLYGON = 201,
    /// @brief a poi
    GLO_POI = 202,

    /// @}

    /// @name demand elements
    /// @{

    /// @brief reserved GLO type to pack all RouteElements (note: In this case the sorting of GLO_<element> is important!)
    GLO_ROUTEELEMENT = 300,
    /// @bief vTypes
    GLO_VTYPE = 301,

    /// @}

    /// @name routes
    /// @{

    /// @brief a route
    GLO_ROUTE = 310,

    /// @}

    /// @name Person plans
    /// @{

    /// @brief a ride
    GLO_RIDE = 320,
    /// @brief a walk
    GLO_WALK = 321,
    /// @brief a person trip
    GLO_PERSONTRIP = 322,

    /// @}

    /// @name stops
    /// @{

    /// @brief a stop
    GLO_STOP = 330,
    /// @brief a person stop
    GLO_PERSONSTOP = 331,

    /// @}

    /// @name vehicles
    /// @{

    /// @brief a vehicle
    GLO_VEHICLE = 340,
    /// @brief a trip
    GLO_TRIP = 341,
    /// @brief a flow
    GLO_FLOW = 342,
    /// @brief a routeFlow
    GLO_ROUTEFLOW = 343,

    /// @}

    /// @name containers (carried by vehicles)
    /// @{

    /// @brief a container
    GLO_CONTAINER = 350,

    /// @}

    /// @name persons
    /// @{

    /// @brief a person
    GLO_PERSON = 360,
    /// @brief a person flow
    GLO_PERSONFLOW = 361,

    /// @}

    /// @brief Traffic Assignment Zones (TAZs)
    GLO_TAZ = 400,

    /// @name other
    /// @{

    /// @brief text element (used in NETEDIT)
    GLO_TEXTNAME = 1000,

    /// @brief dotted contour front element (used in NETEDIT)
    GLO_DOTTEDCONTOUR_FRONT = 1010,

    /// @brief dotted contour inspected element (used in NETEDIT)
    GLO_DOTTEDCONTOUR_INSPECTED = 1020,

    /// @brief temporal shape (used in NETEDIT)
    GLO_TEMPORALSHAPE = 1030,

    /// @brief rectangle selection shape (used in NETEDIT)
    GLO_RECTANGLESELECTION = 1040,

    /// @brief test element (used in NETEDIT)
    GLO_TESTELEMENT = 1050,

    /// @}

    /// @brief empty max
    GLO_MAX = 2048
};
