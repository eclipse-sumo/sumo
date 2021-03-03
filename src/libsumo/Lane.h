/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2021 German Aerospace Center (DLR) and others.
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
    static int getLinkNumber(std::string laneID);
    static std::string getEdgeID(std::string laneID);
    static double getLength(std::string laneID);
    static double getMaxSpeed(std::string laneID);
    static std::vector<std::string> getAllowed(std::string laneID);
    static std::vector<std::string> getDisallowed(std::string laneID);
    static std::vector<libsumo::TraCIConnection> getLinks(std::string laneID);
    static libsumo::TraCIPositionVector getShape(std::string laneID);
    static double getWidth(std::string laneID);
    static double getCO2Emission(std::string laneID);
    static double getCOEmission(std::string laneID);
    static double getHCEmission(std::string laneID);
    static double getPMxEmission(std::string laneID);
    static double getNOxEmission(std::string laneID);
    static double getFuelConsumption(std::string laneID);
    static double getNoiseEmission(std::string laneID);
    static double getElectricityConsumption(std::string laneID);
    static double getLastStepMeanSpeed(std::string laneID);
    static double getLastStepOccupancy(std::string laneID);
    static double getLastStepLength(std::string laneID);
    static double getWaitingTime(std::string laneID);
    static double getTraveltime(std::string laneID);
    static int getLastStepVehicleNumber(std::string laneID);
    static int getLastStepHaltingNumber(std::string laneID);
    static std::vector<std::string> getLastStepVehicleIDs(std::string laneID);
    static std::vector<std::string> getFoes(const std::string& laneID, const std::string& toLaneID);
    static std::vector<std::string> getInternalFoes(const std::string& laneID);
    static const std::vector<std::string> getPendingVehicles(const std::string& laneID);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

    // Setter
    static void setAllowed(std::string laneID, std::string allowedClass);
    static void setAllowed(std::string laneID, std::vector<std::string> allowedClasses);
    static void setDisallowed(std::string laneID, std::vector<std::string> disallowedClasses);
    static void setMaxSpeed(std::string laneID, double speed);
    static void setLength(std::string laneID, double length);

#ifndef LIBTRACI
    /** @brief Saves the shape of the requested object in the given container
    *  @param id The id of the lane to retrieve
    *  @param shape The container to fill
    */
    static void storeShape(const std::string& id, PositionVector& shape);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static const MSLane* getLane(const std::string& id);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
#endif
private:
    /// @brief invalidated standard constructor
    Lane() = delete;
};


}
