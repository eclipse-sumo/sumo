/****************************************************************************/
/// @file    TraCI_Edge.cpp
/// @author  Gregor Laemmel
/// @date    15.09.2017
/// @version $Id$
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
    static MSEdge * getEdge(const std::string& id);
    static double retrieveExistingEffort(std::string& id, double time);
    static double getCurrentTravelTime(std::string& id);
    static double getWaitingSeconds(std::string& id);
    static const std::vector<std::string> getPersonIDs(std::string& id);
    static const std::vector<std::string> getVehicleIDs(std::string& id);
    static double getCO2Emissions(std::string& id);
    static double getCOEmissions(std::string& id);
    static double getHCEmissions(std::string& id);
    static double getPMxEmissions(std::string& id);
    static double getNOxEmissions(std::string& id);
    static double getFuelConsumption(std::string& id);
    static double getNoiseEmissions(std::string& id);
    static double getElectricityConsumption(std::string& id);
    static int getVehicleNumber(std::string& id);
    static double getMeanSpeed(std::string& id);
    static double getOccupancy(std::string& id);
    static int getVehicleHaltingNumber(std::string& id);
    static double getVehicleAverageLength(std::string& id);
    static const std::string& getParameter(std::string& id, std::string& paramName);
    static void setAllowdVehicleClasses(std::string& id, std::vector<std::string> vector);
    static void setDisallowedVehicleClasses(std::string& id, std::vector<std::string> classes);
    static void setAllowdSVCPermissions(std::string& id, SVCPermissions permissions);
    static void addTravelTime(std::string& id, double begTime, double endTime, double value);
    static void addEffort(std::string& id, double begTime, double endTime, double value);
    static void setMaxSpeed(std::string& id, double value);
    static void addParameter(std::string& id, std::string& name, std::string& value);
    static void getShape(const std::string& id, PositionVector& shape);
};


#endif //SUMO_TRACI_EDGE_H
