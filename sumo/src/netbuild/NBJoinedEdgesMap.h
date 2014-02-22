/****************************************************************************/
/// @file    NBJoinedEdgesMap.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// A structure storing information about which edges were joined
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
#ifndef NBJoinedEdgesMap_h
#define NBJoinedEdgesMap_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <vector>
#include <map>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdgeCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBJoinedEdgesMap
 * @brief A structure storing information about which edges were joined
 */
class NBJoinedEdgesMap {
public:
    /// @brief Constructor
    NBJoinedEdgesMap();

    /// @brief Destructor
    ~NBJoinedEdgesMap();

    /** @brief Initialises the map using the list of edge names
     */
    void init(NBEdgeCont& ec);

    /** @brief Informs the map that two edges have been joined
     */
    void appended(const std::string& to, const std::string& what);

    /** @brief Outputs the list of joined edges
     */
    friend std::ostream& operator<<(std::ostream& os,
                                    const NBJoinedEdgesMap& map);

private:
    /// @brief def. of a list of edges that make up an edge
    typedef std::vector<std::string> MappedEdgesVector;

    /// @brief def. of a map of edge names to lists of previous edges the current edge is made of
    typedef std::map<std::string, MappedEdgesVector> JoinedEdgesMap;

    /// @brief Map of edge names to lists of previous edges the current edge is made of
    JoinedEdgesMap myMap;

    std::map<std::string, SUMOReal> myLengths;

};




#endif

/****************************************************************************/

