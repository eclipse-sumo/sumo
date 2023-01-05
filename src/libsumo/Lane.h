/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
/// @file    Lane.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Leonhard Luecken
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <vector>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
class MSLane;
class PositionVector;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Lane
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class Lane {
public:
    // Getter
    static int getLinkNumber(const std::string& laneID);
    static std::string getEdgeID(const std::string& laneID);
    static double getLength(const std::string& laneID);
    static double getMaxSpeed(const std::string& laneID);
    static double getFriction(const std::string& laneID);
    static std::vector<std::string> getAllowed(const std::string& laneID);
    static std::vector<std::string> getDisallowed(const std::string& laneID);
    static std::vector<libsumo::TraCIConnection> getLinks(const std::string& laneID);
    static libsumo::TraCIPositionVector getShape(const std::string& laneID);
    static double getWidth(const std::string& laneID);
    static double getCO2Emission(const std::string& laneID);
    static double getCOEmission(const std::string& laneID);
    static double getHCEmission(const std::string& laneID);
    static double getPMxEmission(const std::string& laneID);
    static double getNOxEmission(const std::string& laneID);
    static double getFuelConsumption(const std::string& laneID);
    static double getNoiseEmission(const std::string& laneID);
    static double getElectricityConsumption(const std::string& laneID);
    static double getLastStepMeanSpeed(const std::string& laneID);
    static double getLastStepOccupancy(const std::string& laneID);
    static double getLastStepLength(const std::string& laneID);
    static double getWaitingTime(const std::string& laneID);
    static double getTraveltime(const std::string& laneID);
    static int getLastStepVehicleNumber(const std::string& laneID);
    static int getLastStepHaltingNumber(const std::string& laneID);
    static std::vector<std::string> getLastStepVehicleIDs(const std::string& laneID);
    static std::vector<std::string> getFoes(const std::string& laneID, const std::string& toLaneID);
    static std::vector<std::string> getInternalFoes(const std::string& laneID);
    static const std::vector<std::string> getPendingVehicles(const std::string& laneID);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

    // Setter
    static void setAllowed(const std::string& laneID, std::string allowedClasses);
    static void setAllowed(const std::string& laneID, std::vector<std::string> allowedClasses);
    static void setDisallowed(const std::string& laneID, std::string disallowedClasses);
    static void setDisallowed(const std::string& laneID, std::vector<std::string> disallowedClasses);
    static void setMaxSpeed(const std::string& laneID, double speed);
    static void setLength(const std::string& laneID, double length);
    static void setFriction(const std::string& laneID, double friction);

    // Generic parameter get/set
    //static std::string getParameter(const std::string& laneID, const std::string& param);
    //static void setParameter(const std::string& routeID, const std::string& key, const std::string& value); // not needed so far

#ifndef LIBTRACI
#ifndef SWIG
    /** @brief Saves the shape of the requested object in the given container
    *  @param id The id of the lane to retrieve
    *  @param shape The container to fill
    */
    static void storeShape(const std::string& id, PositionVector& shape);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static MSLane* getLane(const std::string& id);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
#endif
#endif
private:
    /// @brief invalidated standard constructor
    Lane() = delete;
};


}
