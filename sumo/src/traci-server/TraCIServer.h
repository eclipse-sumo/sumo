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
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

    struct RoadMapPos {
        std::string roadId;
        float pos;
        unsigned char laneId;

        RoadMapPos(): roadId(""), pos(0), laneId(0) {};
    };

    // process all commands until a simulation step is wanted
    static void processCommandsUntilSimStep(SUMOTime step) throw(ProcessError);

    // check whether close was requested
    static bool wasClosed();

    void vehicleStateChanged(const MSVehicle * const vehicle, MSNet::VehicleState to) throw();

private:

    // Constructor
    // Reads the needed parameters out of static OptionsCont
    TraCIServer();

    // Destructor
    // final cleanup
    virtual ~TraCIServer(void) throw();

    int dispatchCommand() throw(TraCIException, std::invalid_argument);

    // process command setMaximumSpeed
    // This command causes the node given by nodeId to limit its speed to a maximum speed (float).
    // If maximum speed is set to a negative value, the individual speed limit for that node gets annihilated.
    // @param in contains unparsed parameters targetTime, ResultType
    // @param out contains node positions ready for output
    // @param length message length
    bool commandSetMaximumSpeed() throw(TraCIException, std::invalid_argument);

    // process command simStep
    // This is the basic comman that encourage the mobility generator to simulate up to the given TargetTime.
    // Normaly, the network simulator sends this command every time unit to gain actual node positions.
    // Probably, the node positions can be desribed in a x- and y-position in a simualated 2D-world.
    // But node positions can be represented in many more ways, e.g. 2.5D, 3D or as points on a road network.
    // The desired representation of positions is given by the entry ResultType.
    // If more than one representation is needed, the command simulation step can be sent several times with the same Target Time and different ResultTypes.
    void postProcessSimulationStep() throw(TraCIException, std::invalid_argument);
    void postProcessSimulationStep2() throw(TraCIException, std::invalid_argument);

    bool commandStopNode() throw(TraCIException, std::invalid_argument);

    bool commandChangeLane() throw(TraCIException, std::invalid_argument);

    bool commandChangeRoute() throw(TraCIException, std::invalid_argument);

    bool commandChangeTarget() throw(TraCIException, std::invalid_argument);

    bool commandCloseConnection() throw(TraCIException);

    bool commandSimulationParameter() throw(TraCIException);

    // process command getTLStatus
    // The traffic light with the given id is asked for all state transitions, that will occur  within
    // a given time interval. Each status change is returned by a TLSwitch command.
    bool commandGetTLStatus() throw(TraCIException);

    // process command slowDown
    // Tell the node given by nodeID to slow down to the given speed (float) within the time intervall
    // given by duration. This simulates different methods of slowing down, like instant braking, coasting...
    // It's assumed that the speed reduction is linear
    // @param in contains nodeID(integer), speed (float), duration(double)
    bool commandSlowDown() throw(TraCIException);

    // command getAllTLIds
    // Returns a list of strings representing the ids of all traffic lights in the simulation
    bool commandGetAllTLIds() throw(TraCIException);

    bool commandUpdateCalibrator() throw(TraCIException);

    bool commandPositionConversion() throw(TraCIException);

    bool commandScenario() throw(TraCIException);

    bool commandDistanceRequest() throw(TraCIException);

    bool commandSubscribeLifecycles() throw(TraCIException);

    bool commandUnsubscribeLifecycles() throw(TraCIException);

    bool commandSubscribeDomain() throw(TraCIException);

    bool commandUnsubscribeDomain() throw(TraCIException);


    void writeStatusCmd(int commandId, int status, std::string description);

    /**
     * Handles the request of a Scenario Command for obtaining information on
     * the road map domain.
     *
     * @param requestMsg original Scenario command message, the fields flag and
     *						domain have already been read
     * @param response storage object that will contain the variable dependant part of the
     *					response on this request
     * @param isWriteCommand true, if the command wants to write a value
     * @return string containing an optional warning (to be added to the response command) if
     *			the requested variable type could not be used
     */
    std::string handleRoadMapDomain(bool isWriteCommand, tcpip::Storage& response) throw(TraCIException);

    /**
     * Handles the request of a Scenario Command for obtaining information on
     * the vehicle domain.
     */
    std::string handleVehicleDomain(bool isWriteCommand, tcpip::Storage& response) throw(TraCIException);

    /**
     * Handles  the request of a Scenario Command for obtaining information on
     * the traffic light domain.
     */
    std::string handleTrafficLightDomain(bool isWriteCommand, tcpip::Storage& response) throw(TraCIException);

    /**
     * Handles  the request of a Scenario Command for obtaining information on
     * the point of interest domain.
     */
    std::string handlePoiDomain(bool isWriteCommand, tcpip::Storage& response) throw(TraCIException);

    /**
     * Handles  the request of a Scenario Command for obtaining information on
     * the polygon domain.
     */
    std::string handlePolygonDomain(bool isWriteCommand, tcpip::Storage& response) throw(TraCIException);

    /**
     * Notifies client of all lifecycle events it is subscribed to
     */
    void handleLifecycleSubscriptions() throw(TraCIException);

    /**
     * Notifies client of all domain object update events it is subscribed to
     */
    void handleDomainSubscriptions(const SUMOTime& currentTime, const std::map<int, const MSVehicle*>& activeEquippedVehicles) throw(TraCIException);


    bool addSubscription(int commandId) throw(TraCIException);

    /**
     * Converts a cartesian position to the closest road map position
     *
     * @param pos	cartesian position that is to be converted
     * @return the closest road map position to the cartesian position
     */
    TraCIServer::RoadMapPos convertCartesianToRoadMap(Position2D pos);

    /**
     * Converts a road map position to a cartesian position
     *
     * @param pos road map position that is to be convertes
     * @return closest 2D position
     */
    Position2D convertRoadMapToCartesian(TraCIServer::RoadMapPos pos) throw(TraCIException);

    // singleton instance of the server
    static TraCIServer* instance_;

    // socket on which server is listening on
    tcpip::Socket* socket_;

    // simulation begin and end time
    SUMOTime targetTime_;

    // penetration rate, measurement of equipped vehicles in simulation
    float penetration_;

    // routeFile name of file which contains vehicle routes
    std::string routeFile_;

    // maps all internal vehicle ids to external id if equipped else to -1
    // Set isMapChanged_ to true, if altering this map
    std::map<std::string, int> equippedVehicles_;

    // maps all external vehicle ids to internal id
    // use method convertExt2IntId instead of accessing the map directly!!!
    std::map<int, std::string> ext2intId;
    bool isMapChanged_;
    void convertExt2IntId(int extId, std::string& intId);

    // maps all internal traffic light ids to external ids
    std::map<int, std::string> trafficLightsExt2IntId;
    // maps all external traffic light ids to internal ids
    std::map<std::string, int> trafficLightsInt2ExtId;

    // maps all internal point of interest ids to external ids
    std::map<int, std::string> poiExt2IntId;
    // maps all external point of interest ids to internal ids
    std::map<std::string, int> poiInt2ExtId;

    // maps all internal polygon ids to external ids
    std::map<int, std::string> polygonExt2IntId;
    // maps all external polygon ids to internal ids
    std::map<std::string, int> polygonInt2ExtId;

    // return vehicle that is referenced by the given external id
    MSVehicle* getVehicleByExtId(int extId);

    // return traffic light logic that is referenced by the given external id
    MSTrafficLightLogic* getTLLogicByExtId(int extId);

    // return point of interest that is referenced by the given external id
    PointOfInterest* getPoiByExtId(int extId);

    // return polygon that is referenced by the given external id
    Polygon2D* getPolygonByExtId(int extId);

    // hold number of all equipped vehicles
    int numEquippedVehicles_;

    // maximum number of vehicles within the simulation
    int totalNumVehicles_;

    // holds all Domain Ids to whose objects' lifecycle the client subscribed
    std::set<int> myLifecycleSubscriptions;

    // holds all Domain Ids to whose objects the client subscribed, along with the variable/type pairs the client is subscribed to
    std::map<int, std::list<std::pair<int, int> > > myDomainSubscriptions;

    // external ids of all vehicles that are currently "living", i.e. have been created, but not yet destroyed
    std::set<int> myLivingVehicles;

    // external ids of all vehicles that have entered the simulation get inserted here
    std::set<int> myCreatedVehicles;

    // external ids of all vehicles that have quit the simulation get inserted here
    std::set<int> myDestroyedVehicles;

    static bool closeConnection_;

    Boundary* netBoundary_;
    const Boundary& getNetBoundary();

    tcpip::Storage myInputStorage;
    tcpip::Storage myOutputStorage;
    bool myDoingSimStep;
    int simStepCommand;

    std::set<MSVehicle*> myVehiclesToReroute;

    class Subscription {
    public:
        Subscription(int commandIdArg, const std::string &idArg, const std::vector<int> &variablesArg,
                     double beginTimeArg, double endTimeArg)
                : commandId(commandIdArg), id(idArg), variables(variablesArg), beginTime(beginTimeArg), endTime(endTimeArg) {}
        int commandId;
        std::string id;
        std::vector<int> variables;
        double beginTime;
        double endTime;

    };

    std::vector<Subscription> mySubscriptions;

    bool processSingleSubscription(const TraCIServer::Subscription &s, tcpip::Storage &writeInto,
                                   std::string &errors) throw(TraCIException);

    std::map<MSNet::VehicleState, std::vector<std::string> > myVehicleStateChanges;

};

// Helper class for reading different data type values out of a storage message
class DataTypeContainer {
private:
    int intValue;
    double realValue;
    std::string stringValue;
    TraCIServer::RoadMapPos roadPosValue;
    float posXValue;
    float posYValue;
    float posZValue;

    int lastValueRead;

public:
    DataTypeContainer() :lastValueRead(-1) {};

    void readValue(unsigned char dataType, tcpip::Storage& msg) throw(TraCIException) {
        switch (dataType) {
        case TYPE_UBYTE:
            intValue = msg.readUnsignedByte();
            break;
        case TYPE_BYTE:
            intValue = msg.readByte();
            break;
        case TYPE_INTEGER:
            intValue = msg.readInt();
            break;
        case TYPE_FLOAT:
            realValue = msg.readFloat();
            break;
        case TYPE_DOUBLE:
            realValue = msg.readDouble();
            break;
        case POSITION_ROADMAP:
            roadPosValue.roadId = msg.readString();
            roadPosValue.pos = msg.readFloat();
            roadPosValue.laneId = msg.readUnsignedByte();
            break;
        case POSITION_2D:
        case POSITION_2_5D:
        case POSITION_3D:
            posXValue = msg.readFloat();
            posYValue = msg.readFloat();
            if (dataType != POSITION_2D) {
                posZValue = msg.readFloat();
            }
            break;
        case TYPE_STRING:
            stringValue = msg.readString();
            break;
        default:
            std::stringstream error;
            error << "Can't read value from request message: the data type " << (int)dataType << " is not known";
            throw TraCIException(error.str());
        }
        lastValueRead = dataType;
    };

    int getLastValueRead() {
        return lastValueRead;
    }

    unsigned char getUByte() throw(TraCIException) {
        if (lastValueRead == TYPE_UBYTE) {
            return static_cast<unsigned char>(intValue);
        } else {
            throw TraCIException("An unsigned byte value has not been read");
        }
    };

    char getByte() throw(TraCIException) {
        if (lastValueRead == TYPE_BYTE) {
            return static_cast<char>(intValue);
        } else {
            throw TraCIException("A byte value has not been read");
        }
    };

    int getInteger() throw(TraCIException) {
        if (lastValueRead == TYPE_INTEGER) {
            return intValue;
        } else {
            throw TraCIException("An integer value has not been read");
        }
    };

    float getFloat() throw(TraCIException) {
        if (lastValueRead == TYPE_FLOAT) {
            return static_cast<float>(realValue);
        } else {
            throw TraCIException("A float value has not been read");
        }
    };

    double getDouble() throw(TraCIException) {
        if (lastValueRead == TYPE_DOUBLE) {
            return intValue;
        } else {
            throw TraCIException("A double value has not been read");
        }
    };

    TraCIServer::RoadMapPos getRoadMapPosition() throw(TraCIException) {
        if (lastValueRead == POSITION_ROADMAP) {
            return roadPosValue;
        } else {
            throw TraCIException("A road map position has not been read");
        }
    };

    void get3DPosition(float& inX, float& inY, float& inZ) throw(TraCIException) {
        if (lastValueRead == POSITION_3D || lastValueRead == POSITION_2_5D) {
            inX = posXValue;
            inY = posYValue;
            inZ = posZValue;
        } else {
            throw TraCIException("A 3d position has not been read");
        }
    };

    void get2DPosition(float& inX, float& inY) throw(TraCIException) {
        if (lastValueRead == POSITION_2D) {
            inX = posXValue;
            inY = posYValue;
        } else {
            throw TraCIException("A 2d position has not been read");
        }
    };

    Position2D getAnyPosition() throw(TraCIException) {
        if (lastValueRead == POSITION_2D
                || lastValueRead == POSITION_3D
                || lastValueRead == POSITION_2_5D) {
            Position2D pos(static_cast<SUMOReal>(posXValue),
                           static_cast<SUMOReal>(posYValue));
            return pos;
        } else {
            throw TraCIException("No position has been read");
        }
    };

    std::string getString() throw(TraCIException) {
        if (lastValueRead == TYPE_STRING) {
            return stringValue;
        } else {
            throw TraCIException("A string value has not been read");
        }
    };
};

}


#endif

#endif
