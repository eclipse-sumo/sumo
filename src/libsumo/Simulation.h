/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Simulation.h
/// @author  Robert Hilbrich
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef Simulation_h
#define Simulation_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <libsumo/TraCIDefs.h>
#include <traci-server/TraCIConstants.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Simulation
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {
class Simulation {
public:
    /// @brief load a simulation with the given arguments
    static void load(const std::vector<std::string>& args);

    /// @brief return whether a simulation (network) is present
    static bool isLoaded();

    /// @brief close simulation
    static void close();

    /// @brief Advances by one step (or up to the given time)
    static void step(const SUMOTime time = 0);

    static void subscribe(const std::string& objID, const std::vector<int>& vars, SUMOTime beginTime, SUMOTime endTime);
    static void subscribeContext(const std::string& objID, int domain, double range, const std::vector<int>& vars, SUMOTime beginTime, SUMOTime endTime);
    static const SubscriptionResults getSubscriptionResults();
    static const TraCIResults getSubscriptionResults(const std::string& objID);
    static const ContextSubscriptionResults getContextSubscriptionResults();
    static const SubscriptionResults getContextSubscriptionResults(const std::string& objID);

    static SUMOTime getCurrentTime();

    static SUMOTime getDeltaT();

    static TraCIBoundary getNetBoundary();

    static TraCIPosition convert3D(const std::string& edgeID, double pos, int laneIndex=0, bool toGeo=false);

    static int getMinExpectedNumber();

    static TraCIStage findRoute(const std::string& from, const std::string& to, const std::string& typeID, const SUMOTime depart, const int routingMode);

    /* @note: default arrivalPos is not -1 because this would lead to very short walks when moving against the edge direction, 
     * instead the middle of the edge is used. DepartPos is treated differently so that 1-edge walks do not have length 0.
     */
    static std::vector<TraCIStage> findIntermodalRoute(const std::string& from, const std::string& to, const std::string& modes="",
            SUMOTime depart=-1, const int routingMode=0, double speed = -1., double walkFactor = -1.,
            double departPos = 0, double arrivalPos = INVALID_DOUBLE_VALUE, const double departPosLat = 0,
            const std::string& pType = "", const std::string& vType = "", const std::string& destStop = "");

    static std::string getParameter(const std::string& objectID, const std::string& key);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;

    /// @brief invalidated standard constructor
    Simulation();

    /// @brief invalidated copy constructor
    Simulation(const Simulation& src);

    /// @brief invalidated assignment operator
    Simulation& operator=(const Simulation& src);
};
}


#endif

/****************************************************************************/
