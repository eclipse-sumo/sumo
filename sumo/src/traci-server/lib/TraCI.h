/****************************************************************************/
/// @file    TraCI.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
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
#ifndef TraCI_h
#define TraCI_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <traci-server/TraCIDefs.h>

class Position;
class PositionVector;
class RGBColor;
class MSEdge;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCI
 * @brief C++ TraCI client API implementation
 */
class TraCI {
public:
    /// @name Connection handling
    /// @{

    /** @brief Connects to the specified SUMO server
     * @param[in] host The name of the host to connect to
     * @param[in] port The port to connect to
     * @exception tcpip::SocketException if the connection fails
     */
    //void connect(const std::string& host, int port);


    /// @brief ends the simulation and closes the connection
    void close();
    /// @}

    /// @brief load a simulation with the given arguments
    static void load(const std::vector<std::string>& args);

    /// @brief Advances by one step (or up to the given time)
    void simulationStep(SUMOTime time = 0);

    /// @brief {object->{variable->value}}
    typedef std::map<int, TraCIValue> TraCIValues;
    typedef std::map<std::string, TraCIValues> SubscribedValues;
    typedef std::map<std::string, SubscribedValues> SubscribedContextValues;

    //void subscribe(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars) const;
    //void subscribeContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<int>& vars) const;

    const SubscribedValues& getSubscriptionResults() const;
    const TraCIValues& getSubscriptionResults(const std::string& objID) const;

    const SubscribedContextValues& getContextSubscriptionResults() const;
    const SubscribedValues& getContextSubscriptionResults(const std::string& objID) const;

    /// @brief helper functions
    static TraCIPositionVector makeTraCIPositionVector(const PositionVector& positionVector);
    static TraCIPosition makeTraCIPosition(const Position& position);
    static Position makePosition(const TraCIPosition& position);

    static PositionVector makePositionVector(const TraCIPositionVector& vector);
    static TraCIColor makeTraCIColor(const RGBColor& color);
    static RGBColor makeRGBColor(const TraCIColor& color);

    static MSEdge* getEdge(const std::string& edgeID);
    static const MSLane* getLaneChecking(const std::string& edgeID, int laneIndex, double pos);
    static std::pair<MSLane*, double> convertCartesianToRoadMap(Position pos);

    static std::vector<std::string>& getLoadArgs() {
        return myLoadArgs;
    }

private:
    /// @brief invalidated copy constructor
    TraCI(const TraCI& src);

    /// @brief invalidated assignment operator
    TraCI& operator=(const TraCI& src);

    SubscribedValues mySubscribedValues;
    SubscribedContextValues mySubscribedContextValues;

    static std::vector<std::string> myLoadArgs;
};


#endif

/****************************************************************************/

