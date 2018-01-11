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
    GLO_JUNCTION = 4,
    /// @brief a tl-logic
    GLO_CROSSING = 5,
    /// @brief a connection
    GLO_CONNECTION = 6,
    /// @brief a prohibition
    GLO_PROHIBITION = 7,
    /// @brief a tl-logic
    GLO_TLLOGIC = 8,
    /// @brief a detector
    GLO_DETECTOR = 16,
    /// @brief a lane speed trigger,
    GLO_TRIGGER = 64,
    /// @brief compound additional
    GLO_ADDITIONAL = GLO_DETECTOR | GLO_TRIGGER,
    /// @brief a polygon
    GLO_POLYGON = 128,
    /// @brief a poi
    GLO_POI = 256,
    /// @brief a vehicles
    GLO_VEHICLE = 512,
    /// @brief a person
    GLO_PERSON = 1024,
    /// @brief a container
    GLO_CONTAINER = 2048,
    /// @brief empty max
    GLO_MAX = 4096

};

#endif

/****************************************************************************/
