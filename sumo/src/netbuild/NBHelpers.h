/****************************************************************************/
/// @file    NBHelpers.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Some mathematical helper methods
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBHelpers_h
#define NBHelpers_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <set>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;
class Position;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBHelpers
 * Some mathmatical methods for the computation of angles
 */
class NBHelpers {
public:
    /// @brief computes the relative angle between the two angles
    static SUMOReal relAngle(SUMOReal angle1, SUMOReal angle2);

    /// @brief ensure that reverse relAngles (>=179.999) always count as turnarounds (-180)
    static SUMOReal normRelAngle(SUMOReal angle1, SUMOReal angle2);

    /// @brief converts the numerical id to its "normal" string representation
    static std::string normalIDRepresentation(const std::string& id);

    /// @brief returns the distance between both nodes
    static SUMOReal distance(NBNode* node1, NBNode* node2);

    /// @brief Add edge ids defined in file (either ID or edge:ID per line) into the given set
    static void loadEdgesFromFile(const std::string& file, std::set<std::string>& into);

    /// @brief Add prefixed ids defined in file
    static void loadPrefixedIDsFomFile(const std::string& file, const std::string prefix,  std::set<std::string>& into);
};


#endif

/****************************************************************************/

