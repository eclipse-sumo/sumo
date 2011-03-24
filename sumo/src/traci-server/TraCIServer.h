/****************************************************************************/
/// @file    TraCIServer.h
/// @author  Axel Wegener <wegener@itm.uni-luebeck.de>
/// @author  Friedemann Wesner <wesner@itm.uni-luebeck.de>
/// @author  Christoph Sommer <christoph.sommer@informatik.uni-erlangen.de>
/// @date    2007/10/24
/// @version $Id$
///
/// TraCI server used to control sumo by a remote TraCI client (e.g., ns2)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TRACISERVER_H
#define TRACISERVER_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include "TraCIConstants.h"

#define BUILD_TCPIP
#include <foreign/tcpip/socket.h>
#include <foreign/tcpip/storage.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>

#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/GeomHelper.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include "TraCIException.h"

#include <map>
#include <string>
#include <set>


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class TraCIServer
*/
namespace traci {
// TraCIServer
// Allows communication of sumo with external program. The external
// program will control sumo.
class TraCIServer : public MSNet::VehicleStateListener {
public:
    /// @brief Definition of a method to be called for serving an associated commandID
    typedef bool(*CmdExecutor)(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);


    /** @brief Initialises the server
     * @param[in] execs The (additional) command executors to use
     */
    static void openSocket(const std::map<int, CmdExecutor> &execs);



    struct RoadMapPos {
        std::string roadId;
        float pos;
        unsigned char laneId;

        RoadMapPos(): roadId(""), pos(0), laneId(0) {};
    };

    // process all commands until a simulation step is wanted
    static void processCommandsUntilSimStep(SUMOTime step);

    // check whether close was requested
    static bool wasClosed();

    // check whether close was requested
    static void close();

    void vehicleStateChanged(const SUMOVehicle * const vehicle, MSNet::VehicleState to);

    void writeStatusCmd(int commandId, int status, const std::string &description);
    void writeStatusCmd(int commandId, int status, const std::string &description, tcpip::Storage &outputStorage);
    /**
     * Converts a cartesian position to the closest road map position
     *
     * @param pos	cartesian position that is to be converted
     * @return the closest road map position to the cartesian position
     */
    TraCIServer::RoadMapPos convertCartesianToRoadMap(Position2D pos);

    const std::map<MSNet::VehicleState, std::vector<std::string> > &getVehicleStateChanges() const {
        return myVehicleStateChanges;
    }

    /** @brief Returns the list of (unified) command executors
     *
     * Not all command executor methods apply to the wanted footprint; other
     *  comments may be served internally (aka there are more commandIds recognized
     *  than within this container
     * @return Mapped command executors
     */
    std::map<int, CmdExecutor> &getExecutors() {
        return myExecutors;
    }




private:

    // Constructor
    // Reads the needed parameters out of static OptionsCont
    TraCIServer();

    // Destructor
    // final cleanup
    virtual ~TraCIServer(void);

    int dispatchCommand();

    void postProcessSimulationStep2();

    bool commandGetVersion();

    bool commandCloseConnection();

    bool commandPositionConversion();

    bool commandAddVehicle();

    bool commandDistanceRequest();

    bool addSubscription(int commandId);

    /**
     * Converts a road map position to a cartesian position
     *
     * @param pos road map position that is to be convertes
     * @return closest 2D position
     */
    Position2D convertRoadMapToCartesian(TraCIServer::RoadMapPos pos);

    // singleton instance of the server
    static TraCIServer* myInstance;
    static bool myDoCloseConnection;

    // socket on which server is listening on
    tcpip::Socket* mySocket;

    // simulation begin and end time
    SUMOTime myTargetTime;


    tcpip::Storage myInputStorage;
    tcpip::Storage myOutputStorage;
    bool myDoingSimStep;

    /// @brief Map of commandIds -> their executors; applicable if the executor applies to the method footprint
    std::map<int, CmdExecutor> myExecutors;


    class Subscription {
    public:
        Subscription(int commandIdArg, const std::string &idArg, const std::vector<int> &variablesArg,
                     SUMOTime beginTimeArg, SUMOTime endTimeArg)
                : commandId(commandIdArg), id(idArg), variables(variablesArg), beginTime(beginTimeArg), endTime(endTimeArg) {}
        int commandId;
        std::string id;
        std::vector<int> variables;
        SUMOTime beginTime;
        SUMOTime endTime;

    };

    std::vector<Subscription> mySubscriptions;

    bool processSingleSubscription(const TraCIServer::Subscription &s, tcpip::Storage &writeInto,
                                   std::string &errors);

    std::map<MSNet::VehicleState, std::vector<std::string> > myVehicleStateChanges;

};

}


#endif

#endif
