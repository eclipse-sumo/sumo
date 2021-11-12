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
/// @file    InductionLoop.h
/// @author  Michael Behrisch
/// @date    15.03.2017
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
class NamedRTree;
class MSInductLoop;
class PositionVector;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class InductionLoop
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class InductionLoop {
public:
    static double getPosition(const std::string& detID);
    static std::string getLaneID(const std::string& detID);
    static int getLastStepVehicleNumber(const std::string& detID);
    static double getLastStepMeanSpeed(const std::string& detID);
    static std::vector<std::string> getLastStepVehicleIDs(const std::string& detID);
    static double getLastStepOccupancy(const std::string& detID);
    static double getLastStepMeanLength(const std::string& detID);
    static double getTimeSinceDetection(const std::string& detID);
    static std::vector<libsumo::TraCIVehicleData> getVehicleData(const std::string& detID);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

#ifndef LIBTRACI
#ifndef SWIG
    /** @brief Returns a tree filled with inductive loop instances
     * @return The rtree of inductive loops
     */
    static NamedRTree* getTree();
    static void cleanup();

    /** @brief Saves the shape of the requested object in the given container
    *  @param id The id of the loop to retrieve
    *  @param shape The container to fill
    */
    static void storeShape(const std::string& id, PositionVector& shape);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static MSInductLoop* getDetector(const std::string& detID);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
    static NamedRTree* myTree;
#endif
#endif

private:
    /// @brief invalidated standard constructor
    InductionLoop() = delete;

};
}
