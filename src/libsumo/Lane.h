/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Lane.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Leonhard Luecken
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef Lane_h
#define Lane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Lane
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {
class Lane {
public:

    // Getter
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static int getLinkNumber(std::string laneID);
    static std::string getEdgeID(std::string laneID);
    static double getLength(std::string laneID);
    static double getMaxSpeed(std::string laneID);
    static std::vector<std::string> getAllowed(std::string laneID);
    static std::vector<std::string> getDisallowed(std::string laneID);
    static std::vector<TraCIConnection> getLinks(std::string laneID);
    static TraCIPositionVector getShape(std::string laneID);
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

    // Setter
    static void setAllowed(std::string laneID, std::vector<std::string> allowedClasses);
    static void setDisallowed(std::string laneID, std::vector<std::string> disallowedClasses);
    static void setMaxSpeed(std::string laneID, double speed);
    static void setLength(std::string laneID, double length);

    // Generic parameter get/set
    static std::string getParameter(const std::string& laneID, const std::string& param);
    static void setParameter(const std::string& routeID, const std::string& key, const std::string& value); // not needed so far

    // Subscriptions (TODO?)
    static void subscribe(const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars);
    static void subscribeContext(const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<int>& vars);

    static const MSLane* getLane(const std::string& id);

private:
    /// @brief invalidated standard constructor
    Lane();

    /// @brief invalidated copy constructor
    Lane(const Lane& src);

    /// @brief invalidated assignment operator
    Lane& operator=(const Lane& src);
};
}


#endif

/****************************************************************************/
