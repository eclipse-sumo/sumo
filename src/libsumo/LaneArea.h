/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    LaneArea.h
/// @author  Michael Behrisch
/// @date    15.03.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef LaneArea_h
#define LaneArea_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSE2Collector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LaneArea
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {
class LaneArea {
public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static int getJamLengthVehicle(const std::string& detID);
    static double getJamLengthMeters(const std::string& detID);
    static double getLastStepMeanSpeed(const std::string& detID);
    static std::vector<std::string> getLastStepVehicleIDs(const std::string& detID);
    static double getLastStepOccupancy(const std::string& detID);
    static double getPosition(const std::string& detID);
    static std::string getLaneID(const std::string& detID);
    static double getLength(const std::string& detID);
    static int getLastStepVehicleNumber(const std::string& detID);
    static int getLastStepHaltingNumber(const std::string& detID);

    LIBSUMO_SUBSCRIPTION_API

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper);

private:
    static MSE2Collector* getDetector(const std::string& detID);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;

    /// @brief invalidated standard constructor
    LaneArea() = delete;

};
}


#endif

/****************************************************************************/
