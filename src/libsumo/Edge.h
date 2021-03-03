/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2021 German Aerospace Center (DLR) and others.
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
/// @file    Edge.h
/// @author  Gregor Laemmel
/// @date    15.09.2017
///
// C++ TraCI client API implementation
/****************************************************************************/

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <libsumo/TraCIDefs.h>
#include <libsumo/TraCIConstants.h>


// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
class MSEdge;
class PositionVector;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Edge
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class Edge {
public:
    static double getAdaptedTraveltime(const std::string& edgeID, double time);
    static double getEffort(const std::string& edgeID, double time);
    static double getTraveltime(const std::string& edgeID);
    static double getWaitingTime(const std::string& edgeID);
    static const std::vector<std::string> getLastStepPersonIDs(const std::string& edgeID);
    static const std::vector<std::string> getLastStepVehicleIDs(const std::string& edgeID);
    static double getCO2Emission(const std::string& edgeID);
    static double getCOEmission(const std::string& edgeID);
    static double getHCEmission(const std::string& edgeID);
    static double getPMxEmission(const std::string& edgeID);
    static double getNOxEmission(const std::string& edgeID);
    static double getFuelConsumption(const std::string& edgeID);
    static double getNoiseEmission(const std::string& edgeID);
    static double getElectricityConsumption(const std::string& edgeID);
    static int getLastStepVehicleNumber(const std::string& edgeID);
    static double getLastStepMeanSpeed(const std::string& edgeID);
    static double getLastStepOccupancy(const std::string& edgeID);
    static int getLastStepHaltingNumber(const std::string& edgeID);
    static double getLastStepLength(const std::string& edgeID);
    static int getLaneNumber(const std::string& edgeID);
    static std::string getStreetName(const std::string& edgeID);
    static const std::vector<std::string> getPendingVehicles(const std::string& edgeID);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

    static void setAllowedVehicleClasses(const std::string& edgeID, std::vector<std::string> vector);
    static void setDisallowedVehicleClasses(const std::string& edgeID, std::vector<std::string> classes);
    static void adaptTraveltime(const std::string& edgeID, double time, double beginSeconds = 0., double endSeconds = std::numeric_limits<double>::max());
    static void setEffort(const std::string& edgeID, double effort, double beginSeconds = 0., double endSeconds = std::numeric_limits<double>::max());
    static void setMaxSpeed(const std::string& edgeID, double speed);

#ifndef LIBTRACI
    /** @brief Saves the shape of the requested object in the given container
    *  @param id The id of the edge to retrieve
    *  @param shape The container to fill
    */
    static void storeShape(const std::string& edgeID, PositionVector& shape);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static void setAllowedSVCPermissions(const std::string& edgeID, int permissions);

    static MSEdge* getEdge(const std::string& edgeID);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
#endif
private:
    /// @brief invalidated standard constructor
    Edge() = delete;

};

}
