/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
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
/// @file    MultiEntryExit.h
/// @author  Michael Behrisch
/// @date    15.03.2017
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
class MSE3Collector;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MultiEntryExit
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class MultiEntryExit {
public:
    static std::vector<std::string> getEntryLanes(const std::string& detID);
    static std::vector<std::string> getExitLanes(const std::string& detID);
    static std::vector<double> getEntryPositions(const std::string& detID);
    static std::vector<double> getExitPositions(const std::string& detID);

    static int getLastStepVehicleNumber(const std::string& detID);
    static double getLastStepMeanSpeed(const std::string& detID);
    static std::vector<std::string> getLastStepVehicleIDs(const std::string& detID);
    static int getLastStepHaltingNumber(const std::string& detID);

    static double getLastIntervalMeanTravelTime(const std::string& detID);
    static double getLastIntervalMeanHaltsPerVehicle(const std::string& detID);
    static double getLastIntervalMeanTimeLoss(const std::string& detID);
    static int getLastIntervalVehicleSum(const std::string& detID);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

#ifndef LIBTRACI
#ifndef SWIG
    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

    static MSE3Collector* getDetector(const std::string& detID);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
#endif
#endif

private:
    /// @brief invalidated standard constructor
    MultiEntryExit() = delete;

};


}
