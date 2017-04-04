/****************************************************************************/
/// @file    TraCI_Lane.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Leonhard Luecken
/// @date    30.05.2012
/// @version $Id: TraCI_Lane.h 23527 2017-03-18 08:45:42Z behrisch $
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCI_Lane_h
#define TraCI_Lane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <traci-server/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCI_Lane
 * @brief C++ TraCI client API implementation
 */
class TraCI_Lane {
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
    TraCI_Lane();

    /// @brief invalidated copy constructor
    TraCI_Lane(const TraCI_Lane& src);

    /// @brief invalidated assignment operator
    TraCI_Lane& operator=(const TraCI_Lane& src);
};


#endif

/****************************************************************************/

