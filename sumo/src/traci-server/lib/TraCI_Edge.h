/****************************************************************************/
/// @file    TraCI_Edge.cpp
/// @author  Gregor Laemmel
/// @date    15.09.2017
/// @version $Id: TraCI_Edge.cpp 25296 2017-07-22 18:29:42Z behrisch $
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2017-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef SUMO_TRACI_EDGE_H
#define SUMO_TRACI_EDGE_H

#include <vector>
#include <string>

class TraCI_Edge {

public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static double retrieveExistingTravelTime(std::string& id, double time);
    static MSEdge * getEdge(std::string& id);
    static double retrieveExistingEffort(std::string& id, double time);
    static double getCurrentTravelTime(std::string& id);
    static double getWaitingSeconds(std::string& id);
    static const std::vector<std::string> getPersonIDs(std::string& id);
};


#endif //SUMO_TRACI_EDGE_H
