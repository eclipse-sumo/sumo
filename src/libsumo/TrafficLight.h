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
/// @file    TrafficLight.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <vector>
#include <libsumo/TraCIDefs.h>
#include <libsumo/TraCIConstants.h>

// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
class MSRailSignalConstraint;
class SUMOVehicle;
#endif

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class TrafficLight
* @brief C++ TraCI client API implementation
*/
namespace LIBSUMO_NAMESPACE {
class TrafficLight {
public:

    static std::string getRedYellowGreenState(const std::string& tlsID);
    static std::vector<libsumo::TraCILogic> getAllProgramLogics(const std::string& tlsID);
    static std::vector<std::string> getControlledJunctions(const std::string& tlsID);
    static std::vector<std::string> getControlledLanes(const std::string& tlsID);
    static std::vector<std::vector<libsumo::TraCILink> > getControlledLinks(const std::string& tlsID);
    static std::string getProgram(const std::string& tlsID);
    static int getPhase(const std::string& tlsID);
    static std::string getPhaseName(const std::string& tlsID);
    static double getPhaseDuration(const std::string& tlsID);
    static double getNextSwitch(const std::string& tlsID);
    static int getServedPersonCount(const std::string& tlsID, int index);
    static std::vector<std::string> getBlockingVehicles(const std::string& tlsID, int linkIndex);
    static std::vector<std::string> getRivalVehicles(const std::string& tlsID, int linkIndex);
    static std::vector<std::string> getPriorityVehicles(const std::string& tlsID, int linkIndex);
    static std::vector<libsumo::TraCISignalConstraint> getConstraints(const std::string& tlsID, const std::string& tripId = "");
    static std::vector<libsumo::TraCISignalConstraint> getConstraintsByFoe(const std::string& foeSignal, const std::string& foeId = "");

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API


    static void setRedYellowGreenState(const std::string& tlsID, const std::string& state);
    static void setPhase(const std::string& tlsID, const int index);
    static void setPhaseName(const std::string& tlsID, const std::string& name);
    static void setProgram(const std::string& tlsID, const std::string& programID);
    static void setPhaseDuration(const std::string& tlsID, const double phaseDuration);
    static void setProgramLogic(const std::string& tlsID, const libsumo::TraCILogic& logic);

    static std::vector<libsumo::TraCISignalConstraint> swapConstraints(const std::string& tlsID, const std::string& tripId, const std::string& foeSignal, const std::string& foeId);
    static void removeConstraints(const std::string& tlsID, const std::string& tripId, const std::string& foeSignal, const std::string& foeId);

    // aliases for backward compatibility
    inline static std::vector<libsumo::TraCILogic> getCompleteRedYellowGreenDefinition(const std::string& tlsID) {
        return getAllProgramLogics(tlsID);
    }
    inline static void setCompleteRedYellowGreenDefinition(const std::string& tlsID, const libsumo::TraCILogic& logic) {
        setProgramLogic(tlsID, logic);
    }
#ifndef LIBTRACI
#ifndef SWIG
    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static libsumo::TraCISignalConstraint buildConstraint(const std::string& tlsID, const std::string& tripId, MSRailSignalConstraint* constraint, bool insertionConstraint);
    /// @brief perform swapConstraints to resolve deadlocks and return the new constraints
    static std::vector<libsumo::TraCISignalConstraint> findConstraintsDeadLocks(const std::string& foeId, const std::string& tripId, const std::string& foeSignal, const std::string& tlsID);
    static SUMOVehicle* getVehicleByTripId(const std::string tripOrVehID);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
#endif
#endif
    /// @brief invalidated standard constructor
    TrafficLight() = delete;
};


}
