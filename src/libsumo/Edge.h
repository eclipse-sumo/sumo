/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Edge.h
/// @author  Gregor Laemmel
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/

#ifndef SUMO_Edge_H
#define SUMO_Edge_H

#include <vector>
#include <string>

namespace libsumo {
class Edge {

public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static double retrieveExistingTravelTime(const std::string& id, double time);
    static MSEdge* getEdge(const std::string& id);
    static double retrieveExistingEffort(const std::string& id, double time);
    static double getCurrentTravelTime(const std::string& id);
    static double getWaitingSeconds(const std::string& id);
    static const std::vector<std::string> getPersonIDs(const std::string& id);
    static const std::vector<std::string> getVehicleIDs(const std::string& id);
    static double getCO2Emissions(const std::string& id);
    static double getCOEmissions(const std::string& id);
    static double getHCEmissions(const std::string& id);
    static double getPMxEmissions(const std::string& id);
    static double getNOxEmissions(const std::string& id);
    static double getFuelConsumption(const std::string& id);
    static double getNoiseEmissions(const std::string& id);
    static double getElectricityConsumption(const std::string& id);
    static int getVehicleNumber(const std::string& id);
    static double getMeanSpeed(const std::string& id);
    static double getOccupancy(const std::string& id);
    static int getVehicleHaltingNumber(const std::string& id);
    static double getVehicleAverageLength(const std::string& id);
    static int getLaneNumber(const std::string& id);
    static std::string getParameter(const std::string& id, const std::string& paramName);
    static void setAllowedVehicleClasses(const std::string& id, std::vector<std::string> vector);
    static void setDisallowedVehicleClasses(const std::string& id, std::vector<std::string> classes);
    static void setAllowedSVCPermissions(const std::string& id, SVCPermissions permissions);
    static void addTravelTime(const std::string& id, double begTime, double endTime, double value);
    static void addEffort(const std::string& id, double begTime, double endTime, double value);
    static void setMaxSpeed(const std::string& id, double value);
    static void setParameter(const std::string& id, const std::string& name, const std::string& value);
    static void getShape(const std::string& id, PositionVector& shape);
};
}


#endif //SUMO_Edge_H

/****************************************************************************/
