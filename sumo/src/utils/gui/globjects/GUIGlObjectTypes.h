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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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
 * Enumeration to differ to show the list of which atrifact
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
    /// @brief empty max
    GLO_MAX = 2048

};

#endif

/****************************************************************************/
