/****************************************************************************/
/// @file    TraCI_Person.h
/// @author  Leonhard Luecken
/// @date    15.09.2017
/// @version $Id: TraCI_Person.h 23527 2017-03-18 08:45:42Z behrisch $
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
#ifndef TraCI_Person_h
#define TraCI_Person_h


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
#include <traci-server/TraCIConstants.h>
#include <traci-server/TraCIServerAPI_Person.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSRoute;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCI_Route
 * @brief C++ TraCI client API implementation
 */
class TraCI_Person {
    // processSet calls private member getSingularVType(), which should not be exposed to the public API.
    friend bool TraCIServerAPI_Person::processSet(TraCIServer&, tcpip::Storage&, tcpip::Storage&);

public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static double getSpeed(const std::string& personID);
    static TraCIPosition getPosition(const std::string& personID);
    static std::string getRoadID(const std::string& personID);
    static std::string getTypeID(const std::string& personID);
    static double getWaitingTime(const std::string& personID);
    static std::string getNextEdge(const std::string& personID);
    static std::string getVehicle(const std::string& personID);
    static int getRemainingStages(const std::string& personID);
    static int getStage(const std::string& personID, int nextStageIndex = 0);
    static std::vector<std::string> getEdges(const std::string& personID, int nextStageIndex = 0);
    static std::string getParameter(const std::string& routeID, const std::string& param);
    static double getAngle(const std::string& personID);
    static double getLanePosition(const std::string& personID);
    static TraCIColor getColor(const std::string& personID);

    static void removeStages(const std::string& personID);
    static void add(const std::string& personID, const std::string& edgeID, double pos, double depart = DEPARTFLAG_NOW, const std::string typeID = "DEFAULT_PEDTYPE");
    static void appendWaitingStage(const std::string& personID, double duration, const std::string& description = "waiting", const std::string& stopID = "");
    static void appendWalkingStage(const std::string& personID, const std::vector<std::string>& edgeIDs, double arrivalPos, double duration = -1, double speed = -1, const std::string& stopID = "");
    static void appendDrivingStage(const std::string& personID, const std::string& toEdge, const std::string& lines, const std::string& stopID = "");
    static void removeStage(const std::string& personID, int nextStageIndex);
    static void rerouteTraveltime(const std::string& personID);
    static void setParameter(const std::string& personID, const std::string& key, const std::string& value);
    static void setSpeed(const std::string& personID, double speed);
    static void setType(const std::string& personID, const std::string& typeID);
    static void setLength(const std::string& personID, double length);
    static void setWidth(const std::string& personID, double width);
    static void setHeight(const std::string& personID, double height);
    static void setMinGap(const std::string& personID, double minGap);
    static void setColor(const std::string& personID, const TraCIColor& c);

private:
    /// @brief invalidated standard constructor
    TraCI_Person();

    /// @brief invalidated copy constructor
    TraCI_Person(const TraCI_Person& src);

    /// @brief invalidated assignment operator
    TraCI_Person& operator=(const TraCI_Person& src);

    static MSTransportable* getPerson(const std::string& id);

    // This does not only return the person's vType, but makes it singular.
    static std::string getSingularVType(const std::string& personID);
};


#endif

/****************************************************************************/

