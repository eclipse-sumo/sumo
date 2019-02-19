/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    InductionLoop.h
/// @author  Michael Behrisch
/// @date    15.03.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef InductionLoop_h
#define InductionLoop_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <libsumo/TraCIConstants.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NamedRTree;
class MSInductLoop;
class PositionVector;
namespace libsumo {
struct TraCIVehicleData;
class VariableWrapper;
}


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class InductionLoop
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {
class InductionLoop {
public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static double getPosition(const std::string& detID);
    static std::string getLaneID(const std::string& detID);
    static int getLastStepVehicleNumber(const std::string& detID);
    static double getLastStepMeanSpeed(const std::string& detID);
    static std::vector<std::string> getLastStepVehicleIDs(const std::string& detID);
    static double getLastStepOccupancy(const std::string& detID);
    static double getLastStepMeanLength(const std::string& detID);
    static double getTimeSinceDetection(const std::string& detID);
    static std::vector<libsumo::TraCIVehicleData> getVehicleData(const std::string& detID);

    LIBSUMO_SUBSCRIPTION_API

    /** @brief Returns a tree filled with inductive loop instances
     * @return The rtree of inductive loops
     */
    static NamedRTree* getTree();

    /** @brief Saves the shape of the requested object in the given container
    *  @param id The id of the loop to retrieve
    *  @param shape The container to fill
    */
    static void storeShape(const std::string& id, PositionVector& shape);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper);

private:
    static MSInductLoop* getDetector(const std::string& detID);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;

private:
    /// @brief invalidated standard constructor
    InductionLoop() = delete;

};
}


#endif

/****************************************************************************/
