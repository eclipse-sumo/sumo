/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2007-2024 German Aerospace Center (DLR) and others.
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
/// @file    TraCIServer.cpp
/// @author  Axel Wegener
/// @author  Friedemann Wesner
/// @author  Christoph Sommer
/// @author  Jakob Erdmann
/// @author  Daniel Krajzewicz
/// @author  Thimor Bohn
/// @author  Tino Morenz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @author  Mario Krumnow
/// @author  Leonhard Luecken
/// @date    2007/10/24
///
// TraCI server used to control sumo by a remote TraCI client (e.g., ns2)
/****************************************************************************/
#include <config.h>

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <string>
#include <cmath>
#include <map>
#include <iostream>
#include <algorithm>
#include <foreign/tcpip/socket.h>
#include <foreign/tcpip/storage.h>
#include <utils/common/SUMOTime.h>
#include <utils/router/DijkstraRouter.h>
#include <utils/common/NamedObjectCont.h>
#include <utils/common/RandHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/xml/XMLSubSys.h>
#include <libsumo/Helper.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/MSJunction.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <libsumo/Simulation.h>
#include <libsumo/Subscription.h>
#include <libsumo/TraCIConstants.h>
#include "TraCIServer.h"
#include "TraCIServerAPI_InductionLoop.h"
#include "TraCIServerAPI_Junction.h"
#include "TraCIServerAPI_Lane.h"
#include "TraCIServerAPI_MultiEntryExit.h"
#include "TraCIServerAPI_LaneArea.h"
#include "TraCIServerAPI_TrafficLight.h"
#include "TraCIServerAPI_Vehicle.h"
#include "TraCIServerAPI_VehicleType.h"
#include "TraCIServerAPI_Route.h"
#include "TraCIServerAPI_POI.h"
#include "TraCIServerAPI_Polygon.h"
#include "TraCIServerAPI_Edge.h"
#include "TraCIServerAPI_Simulation.h"
#include "TraCIServerAPI_Person.h"
#include "TraCIServerAPI_Calibrator.h"
#include "TraCIServerAPI_BusStop.h"
#include "TraCIServerAPI_ParkingArea.h"
#include "TraCIServerAPI_ChargingStation.h"
#include "TraCIServerAPI_RouteProbe.h"
#include "TraCIServerAPI_Rerouter.h"
#include "TraCIServerAPI_VariableSpeedSign.h"
#include "TraCIServerAPI_MeanData.h"
#include "TraCIServerAPI_OverheadWire.h"


// ===========================================================================
// debug constants
// ===========================================================================
//#define DEBUG_MULTI_CLIENTS
//#define DEBUG_SUBSCRIPTIONS
//#define DEBUG_SUBSCRIPTION_FILTERS
//#define DEBUG_RAW_INPUT


// ===========================================================================
// static member definitions
// ===========================================================================
TraCIServer* TraCIServer::myInstance = nullptr;
bool TraCIServer::myDoCloseConnection = false;


// ===========================================================================
// method definitions
// ===========================================================================
void
TraCIServer::initWrapper(const int domainID, const int variable, const std::string& objID) {
    myWrapperStorage.reset();
    myWrapperStorage.writeUnsignedByte(domainID);
    myWrapperStorage.writeUnsignedByte(variable);
    myWrapperStorage.writeString(objID);
}


bool
TraCIServer::wrapDouble(const std::string& /* objID */, const int /* variable */, const double value) {
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    myWrapperStorage.writeDouble(value);
    return true;
}


bool
TraCIServer::wrapInt(const std::string& /* objID */, const int /* variable */, const int value) {
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_INTEGER);
    myWrapperStorage.writeInt(value);
    return true;
}


bool
TraCIServer::wrapString(const std::string& /* objID */, const int /* variable */, const std::string& value) {
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_STRING);
    myWrapperStorage.writeString(value);
    return true;
}


bool
TraCIServer::wrapStringList(const std::string& /* objID */, const int /* variable */, const std::vector<std::string>& value) {
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_STRINGLIST);
    myWrapperStorage.writeStringList(value);
    return true;
}


bool
TraCIServer::wrapDoubleList(const std::string& /* objID */, const int /* variable */, const std::vector<double>& value) {
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_DOUBLELIST);
    myWrapperStorage.writeDoubleList(value);
    return true;
}


bool
TraCIServer::wrapPosition(const std::string& /* objID */, const int variable, const libsumo::TraCIPosition& value) {
    const bool includeZ = variable == libsumo::VAR_POSITION3D;
    myWrapperStorage.writeUnsignedByte(includeZ ? libsumo::POSITION_3D : libsumo::POSITION_2D);
    myWrapperStorage.writeDouble(value.x);
    myWrapperStorage.writeDouble(value.y);
    if (includeZ) {
        myWrapperStorage.writeDouble(value.z);
    }
    return true;
}


bool
TraCIServer::wrapPositionVector(const std::string& /* objID */, const int /* variable */, const libsumo::TraCIPositionVector& shape) {
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_POLYGON);
    if (shape.value.size() < 256) {
        myWrapperStorage.writeUnsignedByte((int)shape.value.size());
    } else {
        myWrapperStorage.writeUnsignedByte(0);
        myWrapperStorage.writeInt((int)shape.value.size());
    }
    for (const libsumo::TraCIPosition& pos : shape.value) {
        myWrapperStorage.writeDouble(pos.x);
        myWrapperStorage.writeDouble(pos.y);
    }
    return true;
}


bool
TraCIServer::wrapColor(const std::string& /* objID */, const int /* variable */, const libsumo::TraCIColor& value) {
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_COLOR);
    myWrapperStorage.writeUnsignedByte(value.r);
    myWrapperStorage.writeUnsignedByte(value.g);
    myWrapperStorage.writeUnsignedByte(value.b);
    myWrapperStorage.writeUnsignedByte(value.a);
    return true;
}


bool
TraCIServer::wrapStringDoublePair(const std::string& /* objID */, const int /* variable */, const std::pair<std::string, double>& value) {
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    myWrapperStorage.writeInt(2);
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_STRING);
    myWrapperStorage.writeString(value.first);
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_DOUBLE);
    myWrapperStorage.writeDouble(value.second);
    return true;
}


bool
TraCIServer::wrapStringPair(const std::string& /* objID */, const int /* variable */, const std::pair<std::string, std::string>& value) {
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_COMPOUND);
    myWrapperStorage.writeInt(2);
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_STRING);
    myWrapperStorage.writeString(value.first);
    myWrapperStorage.writeUnsignedByte(libsumo::TYPE_STRING);
    myWrapperStorage.writeString(value.second);
    return true;
}


tcpip::Storage&
TraCIServer::getWrapperStorage() {
    return myWrapperStorage;
}



TraCIServer::TraCIServer(const SUMOTime begin, const int port, const int numClients)
    : myTargetTime(begin), myLastContextSubscription(nullptr) {
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "Creating new TraCIServer for " << numClients << " clients on port " << port << "." << std::endl;
#endif
    myVehicleStateChanges[MSNet::VehicleState::BUILT] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::DEPARTED] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::STARTING_TELEPORT] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::ENDING_TELEPORT] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::ARRIVED] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::NEWROUTE] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::STARTING_PARKING] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::MANEUVERING] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::ENDING_PARKING] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::STARTING_STOP] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::ENDING_STOP] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::COLLISION] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VehicleState::EMERGENCYSTOP] = std::vector<std::string>();

    myTransportableStateChanges[MSNet::TransportableState::PERSON_DEPARTED] = std::vector<std::string>();
    myTransportableStateChanges[MSNet::TransportableState::PERSON_ARRIVED] = std::vector<std::string>();
    myTransportableStateChanges[MSNet::TransportableState::CONTAINER_DEPARTED] = std::vector<std::string>();
    myTransportableStateChanges[MSNet::TransportableState::CONTAINER_ARRIVED] = std::vector<std::string>();

    myExecutors[libsumo::CMD_GET_INDUCTIONLOOP_VARIABLE] = &TraCIServerAPI_InductionLoop::processGet;
    myExecutors[libsumo::CMD_SET_INDUCTIONLOOP_VARIABLE] = &TraCIServerAPI_InductionLoop::processSet;
    myExecutors[libsumo::CMD_GET_LANEAREA_VARIABLE] = &TraCIServerAPI_LaneArea::processGet;
    myExecutors[libsumo::CMD_SET_LANEAREA_VARIABLE] = &TraCIServerAPI_LaneArea::processSet;
    myExecutors[libsumo::CMD_GET_MULTIENTRYEXIT_VARIABLE] = &TraCIServerAPI_MultiEntryExit::processGet;
    myExecutors[libsumo::CMD_SET_MULTIENTRYEXIT_VARIABLE] = &TraCIServerAPI_MultiEntryExit::processSet;

    myExecutors[libsumo::CMD_GET_TL_VARIABLE] = &TraCIServerAPI_TrafficLight::processGet;
    myExecutors[libsumo::CMD_SET_TL_VARIABLE] = &TraCIServerAPI_TrafficLight::processSet;
    myExecutors[libsumo::CMD_GET_LANE_VARIABLE] = &TraCIServerAPI_Lane::processGet;
    myExecutors[libsumo::CMD_SET_LANE_VARIABLE] = &TraCIServerAPI_Lane::processSet;
    myExecutors[libsumo::CMD_GET_VEHICLE_VARIABLE] = &TraCIServerAPI_Vehicle::processGet;
    myExecutors[libsumo::CMD_SET_VEHICLE_VARIABLE] = &TraCIServerAPI_Vehicle::processSet;
    myExecutors[libsumo::CMD_GET_VEHICLETYPE_VARIABLE] = &TraCIServerAPI_VehicleType::processGet;
    myExecutors[libsumo::CMD_SET_VEHICLETYPE_VARIABLE] = &TraCIServerAPI_VehicleType::processSet;
    myExecutors[libsumo::CMD_GET_ROUTE_VARIABLE] = &TraCIServerAPI_Route::processGet;
    myExecutors[libsumo::CMD_SET_ROUTE_VARIABLE] = &TraCIServerAPI_Route::processSet;
    myExecutors[libsumo::CMD_GET_POI_VARIABLE] = &TraCIServerAPI_POI::processGet;
    myExecutors[libsumo::CMD_SET_POI_VARIABLE] = &TraCIServerAPI_POI::processSet;
    myExecutors[libsumo::CMD_GET_POLYGON_VARIABLE] = &TraCIServerAPI_Polygon::processGet;
    myExecutors[libsumo::CMD_SET_POLYGON_VARIABLE] = &TraCIServerAPI_Polygon::processSet;
    myExecutors[libsumo::CMD_GET_JUNCTION_VARIABLE] = &TraCIServerAPI_Junction::processGet;
    myExecutors[libsumo::CMD_SET_JUNCTION_VARIABLE] = &TraCIServerAPI_Junction::processSet;
    myExecutors[libsumo::CMD_GET_EDGE_VARIABLE] = &TraCIServerAPI_Edge::processGet;
    myExecutors[libsumo::CMD_SET_EDGE_VARIABLE] = &TraCIServerAPI_Edge::processSet;
    myExecutors[libsumo::CMD_GET_SIM_VARIABLE] = &TraCIServerAPI_Simulation::processGet;
    myExecutors[libsumo::CMD_SET_SIM_VARIABLE] = &TraCIServerAPI_Simulation::processSet;
    myExecutors[libsumo::CMD_GET_PERSON_VARIABLE] = &TraCIServerAPI_Person::processGet;
    myExecutors[libsumo::CMD_SET_PERSON_VARIABLE] = &TraCIServerAPI_Person::processSet;
    myExecutors[libsumo::CMD_GET_CALIBRATOR_VARIABLE] = &TraCIServerAPI_Calibrator::processGet;
    myExecutors[libsumo::CMD_SET_CALIBRATOR_VARIABLE] = &TraCIServerAPI_Calibrator::processSet;
    myExecutors[libsumo::CMD_GET_BUSSTOP_VARIABLE] = &TraCIServerAPI_BusStop::processGet;
    myExecutors[libsumo::CMD_SET_BUSSTOP_VARIABLE] = &TraCIServerAPI_BusStop::processSet;
    myExecutors[libsumo::CMD_GET_PARKINGAREA_VARIABLE] = &TraCIServerAPI_ParkingArea::processGet;
    myExecutors[libsumo::CMD_SET_PARKINGAREA_VARIABLE] = &TraCIServerAPI_ParkingArea::processSet;
    myExecutors[libsumo::CMD_GET_CHARGINGSTATION_VARIABLE] = &TraCIServerAPI_ChargingStation::processGet;
    myExecutors[libsumo::CMD_SET_CHARGINGSTATION_VARIABLE] = &TraCIServerAPI_ChargingStation::processSet;
    myExecutors[libsumo::CMD_GET_ROUTEPROBE_VARIABLE] = &TraCIServerAPI_RouteProbe::processGet;
    myExecutors[libsumo::CMD_SET_ROUTEPROBE_VARIABLE] = &TraCIServerAPI_RouteProbe::processSet;
    myExecutors[libsumo::CMD_GET_REROUTER_VARIABLE] = &TraCIServerAPI_Rerouter::processGet;
    myExecutors[libsumo::CMD_SET_REROUTER_VARIABLE] = &TraCIServerAPI_Rerouter::processSet;
    myExecutors[libsumo::CMD_GET_VARIABLESPEEDSIGN_VARIABLE] = &TraCIServerAPI_VariableSpeedSign::processGet;
    myExecutors[libsumo::CMD_SET_VARIABLESPEEDSIGN_VARIABLE] = &TraCIServerAPI_VariableSpeedSign::processSet;
    myExecutors[libsumo::CMD_GET_MEANDATA_VARIABLE] = &TraCIServerAPI_MeanData::processGet;
    //myExecutors[libsumo::CMD_SET_MEANDATA_VARIABLE] = &TraCIServerAPI_MeanData::processSet;
    myExecutors[libsumo::CMD_GET_OVERHEADWIRE_VARIABLE] = &TraCIServerAPI_OverheadWire::processGet;
    myExecutors[libsumo::CMD_SET_OVERHEADWIRE_VARIABLE] = &TraCIServerAPI_OverheadWire::processSet;

    myParameterized.insert(std::make_pair(libsumo::CMD_SUBSCRIBE_VEHICLE_VARIABLE, libsumo::VAR_LEADER));
    myParameterized.insert(std::make_pair(libsumo::CMD_SUBSCRIBE_VEHICLE_VARIABLE, libsumo::VAR_FOLLOWER));
    myParameterized.insert(std::make_pair(0, libsumo::VAR_PARAMETER));
    myParameterized.insert(std::make_pair(0, libsumo::VAR_PARAMETER_WITH_KEY));

    myDoCloseConnection = false;

    // display warning if internal lanes are not used
    // TODO this may be redundant to the warning in NLBuilder::build
    if (!MSGlobals::gUsingInternalLanes && !MSGlobals::gUseMesoSim) {
        WRITE_WARNING(TL("Starting TraCI without using internal lanes!"));
        MsgHandler::getWarningInstance()->inform("Vehicles will jump over junctions.", false);
        MsgHandler::getWarningInstance()->inform("Use without option --no-internal-links to avoid unexpected behavior", false);
    }

    try {
        WRITE_MESSAGEF(TL("***Starting server on port % ***"), toString(port));
        tcpip::Socket serverSocket(port);
        if (numClients > 1) {
            WRITE_MESSAGEF(TL("  waiting for % clients..."), toString(numClients));
        }
        while ((int)mySockets.size() < numClients) {
            int index = (int)mySockets.size() + libsumo::MAX_ORDER + 1;
            mySockets[index] = new SocketInfo(serverSocket.accept(true), begin);
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::BUILT] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::DEPARTED] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::STARTING_TELEPORT] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::ENDING_TELEPORT] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::ARRIVED] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::NEWROUTE] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::STARTING_PARKING] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::MANEUVERING] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::ENDING_PARKING] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::STARTING_STOP] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::ENDING_STOP] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::COLLISION] = std::vector<std::string>();
            mySockets[index]->vehicleStateChanges[MSNet::VehicleState::EMERGENCYSTOP] = std::vector<std::string>();

            mySockets[index]->transportableStateChanges[MSNet::TransportableState::PERSON_DEPARTED] = std::vector<std::string>();
            mySockets[index]->transportableStateChanges[MSNet::TransportableState::PERSON_ARRIVED] = std::vector<std::string>();
            mySockets[index]->transportableStateChanges[MSNet::TransportableState::CONTAINER_DEPARTED] = std::vector<std::string>();
            mySockets[index]->transportableStateChanges[MSNet::TransportableState::CONTAINER_ARRIVED] = std::vector<std::string>();
            if (numClients > 1) {
                WRITE_MESSAGE(TL("  client connected"));
            }
        }
        // When got here, all clients have connected
        if (numClients > 1) {
            checkClientOrdering();
        }
        // set myCurrentSocket != mySockets.end() to indicate that this is the first step in processCommands()
        myCurrentSocket = mySockets.begin();
    } catch (tcpip::SocketException& e) {
        throw ProcessError(e.what());
    }
}


TraCIServer::~TraCIServer() {
    for (const auto& socket : mySockets) {
        delete socket.second;
    }
    // there is no point in calling cleanup() here, it does not free any pointers and will only modify members which get deleted anyway
}


// ---------- Initialisation and Shutdown
void
TraCIServer::openSocket(const std::map<int, CmdExecutor>& execs) {
    if (myInstance == nullptr && !myDoCloseConnection && (OptionsCont::getOptions().getInt("remote-port") != 0)) {
        myInstance = new TraCIServer(string2time(OptionsCont::getOptions().getString("begin")),
                                     OptionsCont::getOptions().getInt("remote-port"),
                                     OptionsCont::getOptions().getInt("num-clients"));
        for (std::map<int, CmdExecutor>::const_iterator i = execs.begin(); i != execs.end(); ++i) {
            myInstance->myExecutors[i->first] = i->second;
        }
    }
    if (myInstance != nullptr) {
        // maybe net was deleted and built again
        MSNet::getInstance()->addVehicleStateListener(myInstance);
        MSNet::getInstance()->addTransportableStateListener(myInstance);
        myInstance->mySubscriptionCache.writeInt(0);
    }
}


void
TraCIServer::close() {
    if (myInstance == nullptr) {
        return;
    }
    delete myInstance;
    myInstance = nullptr;
    myDoCloseConnection = true;
}


bool
TraCIServer::wasClosed() {
    return myDoCloseConnection;
}


// ---------- Initialisation and Shutdown


void
TraCIServer::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to, const std::string& /*info*/) {
    if (!myDoCloseConnection) {
        myVehicleStateChanges[to].push_back(vehicle->getID());
        for (std::map<int, SocketInfo*>::iterator i = mySockets.begin(); i != mySockets.end(); ++i) {
            i->second->vehicleStateChanges[to].push_back(vehicle->getID());
        }
    }
}


void
TraCIServer::transportableStateChanged(const MSTransportable* const transportable, MSNet::TransportableState to, const std::string& /*info*/) {
    if (!myDoCloseConnection) {
        myTransportableStateChanges[to].push_back(transportable->getID());
        for (std::map<int, SocketInfo*>::iterator i = mySockets.begin(); i != mySockets.end(); ++i) {
            i->second->transportableStateChanges[to].push_back(transportable->getID());
        }
    }
}


void
TraCIServer::checkClientOrdering() {
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "Checking client order requests." << std::endl;
#endif
    // check for SET_ORDER commands queued by connected clients
    // In multiclient cas it is mandatory that SET_ORDER  is sent as the first command (or directly after GET_VERSION)
    myCurrentSocket = mySockets.begin();
    while (myCurrentSocket != mySockets.end()) {
#ifdef DEBUG_MULTI_CLIENTS
        std::cout << "  Socket " << myCurrentSocket->second->socket << ":" << std::endl;
#endif
//        bool clientUnordered = true;
        while (true) {
            myInputStorage.reset();
            myCurrentSocket->second->socket->receiveExact(myInputStorage);
            int commandStart, commandLength;
            int commandId = readCommandID(commandStart, commandLength);
#ifdef DEBUG_MULTI_CLIENTS
            std::cout << "    received command " << commandId << std::endl;
#endif
            // Whether the received command is a permitted command for the initialization phase.
            // Currently, getVersion and setOrder are permitted.
            bool initCommand = commandId == libsumo::CMD_SETORDER || commandId == libsumo::CMD_GETVERSION;
            if (initCommand) {
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "    Init command. Sending response." << std::endl;
#endif
                // reset input storage to initial state before reading the commandId
                // (ugly, but we can't just reset the store's iter_ from here)
                // Giving the commandId to dispatch command didn't work either
                tcpip::Storage tmp;
                tmp.writeStorage(myInputStorage);
                myInputStorage.reset();
                // we don't know whether the command was set with extended
                // length syntax or not so we hardcode the length here (#5037)
                myInputStorage.writeUnsignedByte(commandId == libsumo::CMD_SETORDER ? 6 : 2);
                myInputStorage.writeUnsignedByte(commandId);
                myInputStorage.writeStorage(tmp);

                // Handle initialization command completely
                dispatchCommand();
                myCurrentSocket->second->socket->sendExact(myOutputStorage);
                myOutputStorage.reset();
            } else {
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "    Client " << myCurrentSocket->second->socket << " did not set order initially." << std::endl;
#endif
                throw ProcessError(TL("Execution order (libsumo::CMD_SETORDER) was not set for all TraCI clients in pre-execution phase."));
            }
            if (commandId == libsumo::CMD_SETORDER) {
                // This is what we have waited for.
                break;
            }
        }
        ++myCurrentSocket;
    }
}


void
TraCIServer::processReorderingRequests() {
    // Process reordering requests
    if (mySocketReorderRequests.size() > 0) {
        // process reordering requests
        std::map<int, SocketInfo*>::const_iterator i = mySocketReorderRequests.begin();
        std::map<int, SocketInfo*>::iterator j;
#ifdef DEBUG_MULTI_CLIENTS
        std::cout << SIMTIME << " Current socket ordering:\n";
        for (j = mySockets.begin(); j != mySockets.end(); ++j) {
            std::cout << "      " << j->first << ": " << j->second->socket << "\n";
        }
        std::cout << "Reordering requests:\n";
        for (i = mySocketReorderRequests.begin(); i != mySocketReorderRequests.end(); ++i) {
            std::cout << "      Socket " << i->second->socket << " -> " << i->first << "\n";
        }
        i = mySocketReorderRequests.begin();
#endif
        while (i != mySocketReorderRequests.end()) {
            j = mySockets.begin();
            while (j != mySockets.end()) {
                if (j->second->socket == i->second->socket) {
                    break;
                } else {
                    j++;
                }
            }
            assert(j != mySockets.end());
            mySockets.erase(j);
            mySockets[i->first] = i->second;
            ++i;
        }
        mySocketReorderRequests.clear();
#ifdef DEBUG_MULTI_CLIENTS
        std::cout << "New socket ordering:\n";
        for (j = mySockets.begin(); j != mySockets.end(); ++j) {
            std::cout << "      " << j->first << ": " << j->second->socket << "\n";
        }
        std::cout << std::endl;
#endif
    }
}


SUMOTime
TraCIServer::nextTargetTime() const {
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "\n    Determining new target time..." << std::endl;
    if (mySockets.size() == 0) {
        std::cout << "    All clients have disconnected." << std::endl;
    }
#endif
    std::map<int, SocketInfo*>::const_iterator i;
    SUMOTime targetTime = std::numeric_limits<SUMOTime>::max();
    for (i = mySockets.begin(); i != mySockets.end(); ++i) {
#ifdef DEBUG_MULTI_CLIENTS
        std::cout << "    target time for client " << i->second->socket << ": " << i->second->targetTime << "\n";
#endif
        targetTime = MIN2(targetTime, i->second->targetTime);
    }
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << std::endl;
#endif
    return targetTime;
}


// send out subscription results to clients which will act in this step (i.e. with client target time <= myTargetTime)
void
TraCIServer::sendOutputToAll() const {
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "\n    Sending subscription results to clients:\n";
#endif
    std::map<int, SocketInfo*>::const_iterator i = mySockets.begin();
    while (i != mySockets.end()) {
        if (i->second->targetTime <= MSNet::getInstance()->getCurrentTimeStep()) {
            // this client will become active before the next SUMO step. Provide subscription results.
            i->second->socket->sendExact(myOutputStorage);
#ifdef DEBUG_MULTI_CLIENTS
            std::cout << i->second->socket << "\n";
#endif
        }
        ++i;
    }
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << std::endl;
#endif
}


int
TraCIServer::processCommands(const SUMOTime step, const bool afterMove) {
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << SIMTIME << " processCommands(step = " << step << "):\n" << std::endl;
#endif
    try {
        int finalCmd = 0;
        const bool firstStep = myCurrentSocket != mySockets.end();
        // update client order if requested
        processReorderingRequests();
        if (!firstStep && !afterMove) {
            // This is the entry point after performing a SUMO step (block is skipped before first SUMO step since then no simulation results have to be sent)
            // update subscription results
            postProcessSimulationStep();
            // Send out subscription results to clients which will act in this SUMO step (i.e. with client target time <= current sumo timestep end)
            sendOutputToAll();
            myOutputStorage.reset();
        }

        // determine minimal next target time among clients
        myTargetTime = nextTargetTime();

        if (step < myTargetTime) {
#ifdef DEBUG_MULTI_CLIENTS
            std::cout << "    next target time is larger than next SUMO simstep (" << step << "). Returning from processCommands()." << std::endl;
#endif
            return finalCmd;
        }

        // Simulation should run until
        // 1. end time reached or
        // 2. got libsumo::CMD_CLOSE or
        // 3. got libsumo::CMD_LOAD or
        // 4. Client closes socket connection
        while (!myDoCloseConnection && myTargetTime <= (MSNet::getInstance()->getCurrentTimeStep()) && finalCmd != libsumo::CMD_EXECUTEMOVE) {
#ifdef DEBUG_MULTI_CLIENTS
            std::cout << "  Next target time: " << myTargetTime << std::endl;
#endif
            // Iterate over clients and process communication for the ones with target time == myTargetTime
            myCurrentSocket = mySockets.begin();
            while (myCurrentSocket != mySockets.end()) {
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "  current socket: " << myCurrentSocket->second->socket
                          << " with target time=" << myCurrentSocket->second->targetTime
                          << std::endl;
#endif

                if (myCurrentSocket->second->targetTime > myTargetTime || (afterMove && !myCurrentSocket->second->executeMove)) {
                    // this client must wait
#ifdef DEBUG_MULTI_CLIENTS
                    std::cout <<  "       skipping client " << myCurrentSocket->second->socket
                              << " with target time=" << myCurrentSocket->second->targetTime << std::endl;
#endif
                    myCurrentSocket++;
                    continue;
                }
                finalCmd = 0;
                while (finalCmd == 0) {
                    if (!myInputStorage.valid_pos()) {
                        // have read request completely, send response if adequate
                        if (myOutputStorage.size() > 0) {
                            // send response to previous query
                            myCurrentSocket->second->socket->sendExact(myOutputStorage);
                            myOutputStorage.reset();
                        }
#ifdef DEBUG_MULTI_CLIENTS
                        std::cout << "    resetting input storage and reading next command..." << std::endl;
#endif
                        // Read next request
                        myInputStorage.reset();
                        myCurrentSocket->second->socket->receiveExact(myInputStorage);
                    }

                    while (myInputStorage.valid_pos() && !myDoCloseConnection) {
                        const int cmd = dispatchCommand();
                        if (cmd == libsumo::CMD_SIMSTEP || cmd == libsumo::CMD_LOAD || cmd == libsumo::CMD_EXECUTEMOVE || cmd == libsumo::CMD_CLOSE) {
                            finalCmd = cmd;
                        }
                    }
                }
            }
            if (!myLoadArgs.empty()) {
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "  Breaking loop to load new simulation." << std::endl;
#endif
                break;
            } else if (myDoCloseConnection) {
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "  Breaking loop because last client closed connection." << std::endl;
#endif
                break;
            }
            SUMOTime nextT = nextTargetTime();
            // minimal target time among clients should have been increased during the last loop through mySockets
            // XXX: The assert below is disabled since many tests do sth. like simulationStep(step). Such that for a first call step=0,
            //      leading to targetTime==1000 (increased by DELTA_T in dispatchCommand()),
            //      the next call is then usually simulationStep(step=1000) leading to no further increase
            //      and thus a failing assertion here.
            //assert(myTargetTime < nextT || myDoCloseConnection);
            myTargetTime = nextT;
        }
        // All clients are done with the current time step
        // Reset myVehicleStateChanges and myTransportableStateChanges
        for (auto& item : myVehicleStateChanges) {
            item.second.clear();
        }
        for (auto& item : myTransportableStateChanges) {
            item.second.clear();
        }
        return finalCmd;
    } catch (std::invalid_argument& e) {
        throw ProcessError(e.what());
    } catch (libsumo::TraCIException& e) {
        throw ProcessError(e.what());
    } catch (tcpip::SocketException& e) {
        throw ProcessError(e.what());
    }
}


void
TraCIServer::cleanup() {
    mySubscriptions.clear();
    myTargetTime = string2time(OptionsCont::getOptions().getString("begin"));
    for (myCurrentSocket = mySockets.begin(); myCurrentSocket != mySockets.end(); ++myCurrentSocket) {
        myCurrentSocket->second->targetTime = myTargetTime;
        myCurrentSocket->second->executeMove = false;
    }
    myOutputStorage.reset();
    myInputStorage.reset();
    mySubscriptionCache.reset();
    for (auto& i : myVehicleStateChanges) {
        i.second.clear();
    }
    for (auto& i : myTransportableStateChanges) {
        i.second.clear();
    }
    myCurrentSocket = mySockets.begin();
}


std::map<int, TraCIServer::SocketInfo*>::iterator
TraCIServer::removeCurrentSocket() {
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "       Removing socket " << myCurrentSocket->second->socket
              << " (order " << myCurrentSocket->first << ")" << std::endl;
#endif
    delete myCurrentSocket->second;
    myCurrentSocket = mySockets.erase(myCurrentSocket);
    return myCurrentSocket;
}


int
TraCIServer::readCommandID(int& commandStart, int& commandLength) {
    commandStart = myInputStorage.position();
    commandLength = myInputStorage.readUnsignedByte();
    if (commandLength == 0) {
        commandLength = myInputStorage.readInt();
    }
#ifdef DEBUG_RAW_INPUT
    std::cout << " commandStart=" << commandStart << " commandLength=" << commandLength << " pos=" << myInputStorage.position() << " raw=";
    for (auto it = myInputStorage.begin(); it != myInputStorage.end(); ++it) {
        std::cout << (int)*it << " ";
    }
    std::cout << "\n";
#endif
    return myInputStorage.readUnsignedByte();
}


int
TraCIServer::dispatchCommand() {
    int commandStart, commandLength;
    int commandId = readCommandID(commandStart, commandLength);
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "       dispatchCommand() called for client " << myCurrentSocket->second->socket
              << ", commandId = " << commandId << std::endl;
#endif
    bool success = false;
    // dispatch commands
    if (myExecutors.find(commandId) != myExecutors.end()) {
        success = myExecutors[commandId](*this, myInputStorage, myOutputStorage);
    } else {
        switch (commandId) {
            case libsumo::CMD_GETVERSION:
                success = commandGetVersion();
                break;
            case libsumo::CMD_LOAD: {
                std::vector<std::string> args;
                if (!readTypeCheckingStringList(myInputStorage, args)) {
                    return writeErrorStatusCmd(libsumo::CMD_LOAD, "A load command needs a list of string arguments.", myOutputStorage);
                }
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "       commandId == libsumo::CMD_LOAD"
                          << ", args = " << toString(args) << std::endl;
#endif
                try {
                    myLoadArgs = args;
                    success = true;
                    writeStatusCmd(libsumo::CMD_LOAD, libsumo::RTYPE_OK, "");
                    // XXX: This only cares for the client that issued the load command.
                    // Multiclient-load functionality is still to be implemented. Refs #3146.
                    myCurrentSocket->second->socket->sendExact(myOutputStorage);
                    myCurrentSocket = mySockets.end();
                    myOutputStorage.reset();
                } catch (libsumo::TraCIException& e) {
                    return writeErrorStatusCmd(libsumo::CMD_LOAD, e.what(), myOutputStorage);
                }
                break;
            }
            case libsumo::CMD_EXECUTEMOVE:
                myCurrentSocket->second->executeMove = true;
                myCurrentSocket++;
                success = true;
                writeStatusCmd(libsumo::CMD_EXECUTEMOVE, libsumo::RTYPE_OK, "");
                break;
            case libsumo::CMD_SIMSTEP: {
                const double nextT = myInputStorage.readDouble();
                if (nextT == 0.) {
                    myCurrentSocket->second->targetTime += DELTA_T;
                } else {
                    myCurrentSocket->second->targetTime = TIME2STEPS(nextT);
                }
                myCurrentSocket->second->executeMove = false;
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "       commandId == libsumo::CMD_SIMSTEP"
                          << ", next target time for client is " << myCurrentSocket->second->targetTime << std::endl;
#endif
                if (myCurrentSocket->second->targetTime <= MSNet::getInstance()->getCurrentTimeStep()) {
                    // This is not the last TraCI simstep in the current SUMO simstep -> send single simstep response.
                    // @note: In the other case the simstep results are sent to all after the SUMO step was performed, see entry point for processCommands()
                    sendSingleSimStepResponse();
                }
                // Clear vehicleStateChanges and transportableStateChanges for this client
                // -> For subsequent TraCI stepping
                // that is performed within this SUMO step, no updates on vehicle states
                // belonging to the last SUMO simulation step will be received by this client.
                for (auto& item : myCurrentSocket->second->vehicleStateChanges) {
                    item.second.clear();
                }
                for (auto& item : myCurrentSocket->second->transportableStateChanges) {
                    item.second.clear();
                }
                myCurrentSocket++;
                return commandId;
            }
            case libsumo::CMD_CLOSE:
                writeStatusCmd(libsumo::CMD_CLOSE, libsumo::RTYPE_OK, "");
                myCurrentSocket->second->socket->sendExact(myOutputStorage);
                myOutputStorage.reset();
                if (mySockets.size() == 1) {
                    // Last client has closed connection
                    myDoCloseConnection = true;
                }
                // remove current socket and increment to next socket in ordering
                myCurrentSocket = removeCurrentSocket();
                success = true;
                break;
            case libsumo::CMD_SETORDER: {
                const int order = myInputStorage.readInt();
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "       commandId == libsumo::CMD_SETORDER"
                          << ", order index is " << order << std::endl;
#endif
                if (order > libsumo::MAX_ORDER) {
                    return writeErrorStatusCmd(libsumo::CMD_SETORDER, "A set order command needs an int argument below " + toString(libsumo::MAX_ORDER) + ".", myOutputStorage);
                }
                if (mySockets.count(order) > 0 || mySocketReorderRequests.count(order) > 0) {
                    return writeErrorStatusCmd(libsumo::CMD_SETORDER, "Order '" + toString(order) + "' is already taken.", myOutputStorage);
                }
                // memorize reorder request (will only take effect in the next step)
                mySocketReorderRequests[order] = myCurrentSocket->second;
                success = true;
                writeStatusCmd(libsumo::CMD_SETORDER, libsumo::RTYPE_OK, "");
                break;
            }
            case libsumo::CMD_SUBSCRIBE_BUSSTOP_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_CALIBRATOR_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_CHARGINGSTATION_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_EDGE_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_GUI_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_INDUCTIONLOOP_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_JUNCTION_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_LANE_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_LANEAREA_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_MEANDATA_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_OVERHEADWIRE_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_PARKINGAREA_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_PERSON_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_POI_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_POLYGON_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_REROUTER_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_ROUTE_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_ROUTEPROBE_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_SIM_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_TL_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_VARIABLESPEEDSIGN_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_VEHICLE_VARIABLE:
            case libsumo::CMD_SUBSCRIBE_VEHICLETYPE_VARIABLE:
                success = addObjectVariableSubscription(commandId, false);
                break;
            case libsumo::CMD_SUBSCRIBE_BUSSTOP_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_CALIBRATOR_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_CHARGINGSTATION_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_EDGE_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_GUI_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_INDUCTIONLOOP_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_JUNCTION_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_LANE_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_LANEAREA_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_MEANDATA_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_OVERHEADWIRE_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_PARKINGAREA_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_PERSON_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_POI_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_POLYGON_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_REROUTER_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_ROUTE_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_ROUTEPROBE_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_SIM_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_TL_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_VARIABLESPEEDSIGN_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_VEHICLE_CONTEXT:
            case libsumo::CMD_SUBSCRIBE_VEHICLETYPE_CONTEXT:
                success = addObjectVariableSubscription(commandId, true);
                break;
            case libsumo::CMD_ADD_SUBSCRIPTION_FILTER:
                success = addSubscriptionFilter();
                break;
            default:
                if (commandId == libsumo::CMD_GET_GUI_VARIABLE || commandId == libsumo::CMD_SET_GUI_VARIABLE) {
                    writeStatusCmd(commandId, libsumo::RTYPE_NOTIMPLEMENTED, "GUI is not running, command not implemented in command line sumo");
                } else {
                    writeStatusCmd(commandId, libsumo::RTYPE_NOTIMPLEMENTED, "Command not implemented in sumo");
                }
        }
    }
    if (!success) {
        while (myInputStorage.valid_pos() && (int)myInputStorage.position() < commandStart + commandLength) {
            myInputStorage.readChar();
        }
    }
    if ((int)myInputStorage.position() != commandStart + commandLength) {
        std::ostringstream msg;
        msg << "Wrong position in requestMessage after dispatching command " << commandId << ".";
        msg << " Expected command length was " << commandLength;
        msg << " but " << myInputStorage.position() - commandStart << " Bytes were read.";
        writeStatusCmd(commandId, libsumo::RTYPE_ERR, msg.str());
        myDoCloseConnection = true;
    }
    return commandId;
}


// ---------- Server-internal command handling
bool
TraCIServer::commandGetVersion() {
    // Prepare response
    tcpip::Storage answerTmp;
    answerTmp.writeInt(libsumo::TRACI_VERSION);
    answerTmp.writeString("SUMO " VERSION_STRING);
    // When we get here, the response is stored in answerTmp -> put into myOutputStorage
    writeStatusCmd(libsumo::CMD_GETVERSION, libsumo::RTYPE_OK, "");
    // command length
    myOutputStorage.writeUnsignedByte(1 + 1 + static_cast<int>(answerTmp.size()));
    // command type
    myOutputStorage.writeUnsignedByte(libsumo::CMD_GETVERSION);
    // and the parameter dependant part
    myOutputStorage.writeStorage(answerTmp);
    return true;
}


void
TraCIServer::postProcessSimulationStep() {
    SUMOTime t = MSNet::getInstance()->getCurrentTimeStep();
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "   postProcessSimulationStep() at time=" << t << std::endl;
#endif
    writeStatusCmd(libsumo::CMD_SIMSTEP, libsumo::RTYPE_OK, "");
    int noActive = 0;
    for (std::vector<libsumo::Subscription>::iterator i = mySubscriptions.begin(); i != mySubscriptions.end();) {
        const libsumo::Subscription& s = *i;
        bool isArrivedVehicle = (s.commandId == libsumo::CMD_SUBSCRIBE_VEHICLE_VARIABLE || s.commandId == libsumo::CMD_SUBSCRIBE_VEHICLE_CONTEXT)
                                && (find(myVehicleStateChanges[MSNet::VehicleState::ARRIVED].begin(), myVehicleStateChanges[MSNet::VehicleState::ARRIVED].end(), s.id) != myVehicleStateChanges[MSNet::VehicleState::ARRIVED].end());

        bool isArrivedPerson = (s.commandId == libsumo::CMD_SUBSCRIBE_PERSON_VARIABLE || s.commandId == libsumo::CMD_SUBSCRIBE_PERSON_CONTEXT) && MSNet::getInstance()->getPersonControl().get(s.id) == nullptr;
        if ((s.endTime < t) || isArrivedVehicle || isArrivedPerson) {
            i = mySubscriptions.erase(i);
            continue;
        }
        ++i;
        if (s.beginTime > t) {
            continue;
        }
        ++noActive;
    }
    mySubscriptionCache.reset();
#ifdef DEBUG_SUBSCRIPTIONS
    std::cout << "   Initial size of mySubscriptionCache is " << mySubscriptionCache.size()
              << "\n   Nr. of active subscriptions = " << noActive << std::endl;
#endif
    mySubscriptionCache.writeInt(noActive);
#ifdef DEBUG_SUBSCRIPTIONS
    std::cout << "   Size after writing an int is " << mySubscriptionCache.size() << std::endl;
#endif
    for (std::vector<libsumo::Subscription>::iterator i = mySubscriptions.begin(); i != mySubscriptions.end();) {
        const libsumo::Subscription& s = *i;
        if (s.beginTime > t) {
            ++i;
            continue;
        }
        tcpip::Storage into;
        std::string errors;
        bool ok = processSingleSubscription(s, into, errors);
#ifdef DEBUG_SUBSCRIPTIONS
        std::cout << "   Size of into-store for subscription " << s.id
                  << ": " << into.size() << std::endl;
#endif
        mySubscriptionCache.writeStorage(into);
        if (ok) {
            ++i;
        } else {
            i = mySubscriptions.erase(i);
        }
    }
    myOutputStorage.writeStorage(mySubscriptionCache);
#ifdef DEBUG_SUBSCRIPTIONS
    std::cout << "   Size after writing subscriptions is " << mySubscriptionCache.size() << std::endl;
#endif
}


void
TraCIServer::sendSingleSimStepResponse() {
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "       Sending cached simstep response to current client " << myCurrentSocket->second->socket
              << " (-> intermediate TraCI step)."
              << "\n       Size of mySubscriptionCache is " << mySubscriptionCache.size()
              << std::endl;
#endif
    writeStatusCmd(libsumo::CMD_SIMSTEP, libsumo::RTYPE_OK, "");

// NOTE: the commented code would send an empty response
//    myOutputStorage.writeInt(0);
//    myCurrentSocket->second->socket->sendExact(myOutputStorage);
//    myOutputStorage.reset();
    myOutputStorage.writeStorage(mySubscriptionCache);
    // send results to active client
    myCurrentSocket->second->socket->sendExact(myOutputStorage);
    myOutputStorage.reset();
}


void
TraCIServer::writeStatusCmd(int commandId, int status, const std::string& description) {
    writeStatusCmd(commandId, status, description, myOutputStorage);
}


void
TraCIServer::writeStatusCmd(int commandId, int status, const std::string& description, tcpip::Storage& outputStorage) {
    if (status == libsumo::RTYPE_ERR) {
        WRITE_ERROR("Answered with error to command " + toHex(commandId, 2) + ": " + description);
    } else if (status == libsumo::RTYPE_NOTIMPLEMENTED) {
        WRITE_ERROR("Requested command not implemented (" + toHex(commandId, 2) + "): " + description);
    }
    outputStorage.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(description.length())); // command length
    outputStorage.writeUnsignedByte(commandId); // command type
    outputStorage.writeUnsignedByte(status); // status
    outputStorage.writeString(description); // description
}


bool
TraCIServer::writeErrorStatusCmd(int commandId, const std::string& description, tcpip::Storage& outputStorage) {
    writeStatusCmd(commandId, libsumo::RTYPE_ERR, description, outputStorage);
    return false;
}


void
TraCIServer::initialiseSubscription(libsumo::Subscription& s) {
    tcpip::Storage writeInto;
    std::string errors;
    libsumo::Subscription* modifiedSubscription = nullptr;
    try {
        if (processSingleSubscription(s, writeInto, errors)) {
            if (s.endTime < MSNet::getInstance()->getCurrentTimeStep()) {
                writeStatusCmd(s.commandId, libsumo::RTYPE_ERR, "Subscription has ended.");
            } else {
                if (libsumo::Helper::needNewSubscription(s, mySubscriptions, modifiedSubscription)) {
                    // Add new subscription to subscription cache (note: seems a bit inefficient)
                    if (s.beginTime < MSNet::getInstance()->getCurrentTimeStep()) {
                        // copy new subscription into cache
                        int noActive = 1 + (mySubscriptionCache.size() > 0 ? mySubscriptionCache.readInt() : 0);
                        tcpip::Storage tmp;
                        tmp.writeInt(noActive);
                        while (mySubscriptionCache.valid_pos()) {
                            tmp.writeByte(mySubscriptionCache.readByte());
                        }
                        tmp.writeStorage(writeInto);
                        mySubscriptionCache.reset();
                        mySubscriptionCache.writeStorage(tmp);
                    }
                }
                writeStatusCmd(s.commandId, libsumo::RTYPE_OK, "");
            }
            if (modifiedSubscription != nullptr && (
                        modifiedSubscription->isVehicleToVehicleContextSubscription()
                        || modifiedSubscription->isVehicleToPersonContextSubscription())) {
                // Set last modified vehicle context subscription active for filter modifications
                myLastContextSubscription = modifiedSubscription;
            } else {
                // adding other subscriptions deactivates the activation for filter addition
                myLastContextSubscription = nullptr;
            }
        } else {
            writeStatusCmd(s.commandId, libsumo::RTYPE_ERR, "Could not add subscription. " + errors);
        }
    } catch (libsumo::TraCIException& e) {
        writeStatusCmd(s.commandId, libsumo::RTYPE_ERR, e.what());
    }
    myOutputStorage.writeStorage(writeInto);
}


void
TraCIServer::removeSubscription(int commandId, const std::string& id, int domain) {
    bool found = false;
    std::vector<libsumo::Subscription>::iterator j;
    for (j = mySubscriptions.begin(); j != mySubscriptions.end();) {
        if (j->id == id && j->commandId == commandId && j->contextDomain == domain) {
            j = mySubscriptions.erase(j);
            if (j != mySubscriptions.end() && myLastContextSubscription == &(*j)) {
                // Remove also reference for filter additions
                myLastContextSubscription = nullptr;
            }
            found = true;
            continue;
        }
        ++j;
    }
    // try unsubscribe
    if (found) {
        writeStatusCmd(commandId, libsumo::RTYPE_OK, "");
    } else {
        writeStatusCmd(commandId, libsumo::RTYPE_ERR, "The subscription to remove was not found.");
    }
}


bool
TraCIServer::processSingleSubscription(const libsumo::Subscription& s, tcpip::Storage& writeInto,
                                       std::string& errors) {
    bool ok = true;
    tcpip::Storage outputStorage;
    const int getCommandId = s.contextDomain > 0 ? s.contextDomain : s.commandId - 0x30;
    std::set<std::string> objIDs;
    if (s.contextDomain > 0) {
        if ((s.activeFilters & libsumo::SUBS_FILTER_NO_RTREE) == 0) {
            PositionVector shape;
            libsumo::Helper::findObjectShape(s.commandId, s.id, shape);
            libsumo::Helper::collectObjectIDsInRange(s.contextDomain, shape, s.range, objIDs);
        }
        libsumo::Helper::applySubscriptionFilters(s, objIDs);
    } else {
        objIDs.insert(s.id);
    }
    const int numVars = s.contextDomain > 0 && s.variables.size() == 1 && s.variables[0] == libsumo::TRACI_ID_LIST ? 0 : (int)s.variables.size();
    int skipped = 0;
    for (std::set<std::string>::iterator j = objIDs.begin(); j != objIDs.end(); ++j) {
        if (s.contextDomain > 0) {
            //if (centralObject(s, *j)) {
            //    skipped++;
            //    continue;
            //}
            outputStorage.writeString(*j);
        }
        if (numVars > 0) {
            std::vector<std::shared_ptr<tcpip::Storage> >::const_iterator k = s.parameters.begin();
            for (std::vector<int>::const_iterator i = s.variables.begin(); i != s.variables.end(); ++i, ++k) {
                tcpip::Storage message;
                message.writeUnsignedByte(*i);
                message.writeString(*j);
                // TODO check why writeStorage fails here (probably some kind of invalid iterator)
                for (const auto& v :** k) {
                    message.writeChar(v);
                }
                tcpip::Storage tmpOutput;
                if (myExecutors.find(getCommandId) != myExecutors.end()) {
                    ok &= myExecutors[getCommandId](*this, message, tmpOutput);
                } else {
                    writeStatusCmd(s.commandId, libsumo::RTYPE_NOTIMPLEMENTED, "Unsupported command specified", tmpOutput);
                    ok = false;
                }
                // copy response part
                if (ok) {
                    int length = tmpOutput.readUnsignedByte();
                    while (--length > 0) {
                        tmpOutput.readUnsignedByte();
                    }
                    int lengthLength = 1;
                    length = tmpOutput.readUnsignedByte();
                    if (length == 0) {
                        lengthLength = 5;
                        length = tmpOutput.readInt();
                    }
                    //read responseType
                    tmpOutput.readUnsignedByte();
                    int variable = tmpOutput.readUnsignedByte();
                    std::string id = tmpOutput.readString();
                    outputStorage.writeUnsignedByte(variable);
                    outputStorage.writeUnsignedByte(libsumo::RTYPE_OK);
                    length -= (lengthLength + 1 + 4 + (int)id.length());
                    while (--length > 0) {
                        outputStorage.writeUnsignedByte(tmpOutput.readUnsignedByte());
                    }
                } else {
                    //read length
                    tmpOutput.readUnsignedByte();
                    //read cmd
                    tmpOutput.readUnsignedByte();
                    //read status
                    tmpOutput.readUnsignedByte();
                    std::string msg = tmpOutput.readString();
                    outputStorage.writeUnsignedByte(*i);
                    outputStorage.writeUnsignedByte(libsumo::RTYPE_ERR);
                    outputStorage.writeUnsignedByte(libsumo::TYPE_STRING);
                    outputStorage.writeString(msg);
                    errors = errors + msg;
                }
            }
        }
    }
    int length = (1 + 4) + 1 + (4 + (int)s.id.length()) + 1 + (int)outputStorage.size();
    if (s.contextDomain > 0) {
        length += 1 + 4;  // context domain and number of objects
    }
    // we always write extended command length here for backward compatibility
    writeInto.writeUnsignedByte(0); // command length -> extended
    writeInto.writeInt(length);
    writeInto.writeUnsignedByte(s.commandId + 0x10);
    writeInto.writeString(s.id);
    if (s.contextDomain > 0) {
        writeInto.writeUnsignedByte(s.contextDomain);
    }
    writeInto.writeUnsignedByte(numVars);
    if (s.contextDomain > 0) {
        writeInto.writeInt((int)objIDs.size() - skipped);
    }
    if (s.contextDomain == 0 || objIDs.size() != 0) {
        writeInto.writeStorage(outputStorage);
    }
    return ok;
}


bool
TraCIServer::addObjectVariableSubscription(const int commandId, const bool hasContext) {
    const double beginTime = myInputStorage.readDouble();
    const double endTime = myInputStorage.readDouble();
    const SUMOTime begin = beginTime == libsumo::INVALID_DOUBLE_VALUE ? 0 : TIME2STEPS(beginTime);
    const SUMOTime end = endTime == libsumo::INVALID_DOUBLE_VALUE || endTime > STEPS2TIME(SUMOTime_MAX) ? SUMOTime_MAX : TIME2STEPS(endTime);
    const std::string id = myInputStorage.readString();
    const int domain = hasContext ? myInputStorage.readUnsignedByte() : 0;
    double range = hasContext ? myInputStorage.readDouble() : 0.;
    if (commandId == libsumo::CMD_SUBSCRIBE_SIM_CONTEXT) {
        range = std::numeric_limits<double>::max();
    }
    const int num = myInputStorage.readUnsignedByte();
    std::vector<int> variables;
    std::vector<std::shared_ptr<tcpip::Storage> > parameters;
    for (int i = 0; i < num; ++i) {
        const int varID = myInputStorage.readUnsignedByte();
        variables.push_back(varID);
        parameters.push_back(std::make_shared<tcpip::Storage>());
        if ((myParameterized.count(std::make_pair(0, varID)) > 0) || (myParameterized.count(std::make_pair(commandId, varID)) > 0)) {
            const int parType = myInputStorage.readUnsignedByte();
            parameters.back()->writeUnsignedByte(parType);
            if (parType == libsumo::TYPE_DOUBLE) {
                parameters.back()->writeDouble(myInputStorage.readDouble());
            } else if (parType == libsumo::TYPE_STRING) {
                parameters.back()->writeString(myInputStorage.readString());
            } else {
                // Error!
            }
        }
    }
    // check subscribe/unsubscribe
    if (variables.empty()) {
        removeSubscription(commandId, id, domain);
        return true;
    }
    // process subscription
    libsumo::Subscription s(commandId, id, variables, parameters, begin, end, domain, range);
    initialiseSubscription(s);
    return true;
}



bool
TraCIServer::addSubscriptionFilter() {
    bool success  = true;
    // Read filter type
    int filterType = myInputStorage.readUnsignedByte();

    if (myLastContextSubscription == nullptr) {
        writeStatusCmd(libsumo::CMD_ADD_SUBSCRIPTION_FILTER, libsumo::RTYPE_ERR,
                       "No previous vehicle context subscription exists to apply filter type " + toHex(filterType, 2));
        return false;
    }

    // dispatch according to filter type
    switch (filterType) {
        case libsumo::FILTER_TYPE_NONE:
            // Remove all filters
            removeFilters();
            break;
        case libsumo::FILTER_TYPE_LANES: {
            // Read relative lanes to consider for context filter
            int nrLanes = (int)myInputStorage.readByte();
            std::vector<int> lanes;
            for (int i = 0; i < nrLanes; ++i) {
                lanes.push_back((int) myInputStorage.readByte());
            }
            addSubscriptionFilterLanes(lanes);
        }
        break;
        case libsumo::FILTER_TYPE_NOOPPOSITE:
            // Add no-opposite filter
            addSubscriptionFilterNoOpposite();
            break;
        case libsumo::FILTER_TYPE_DOWNSTREAM_DIST: {
            myInputStorage.readByte(); // read type double
            double dist = myInputStorage.readDouble();
            addSubscriptionFilterDownstreamDistance(dist);
        }
        break;
        case libsumo::FILTER_TYPE_UPSTREAM_DIST: {
            myInputStorage.readByte(); // read type double
            double dist = myInputStorage.readDouble();
            addSubscriptionFilterUpstreamDistance(dist);
        }
        break;
        case libsumo::FILTER_TYPE_LEAD_FOLLOW: {
            // Read relative lanes to consider for context filter
            addSubscriptionFilterLeadFollow();
        }
        break;
        case libsumo::FILTER_TYPE_TURN: {
            myInputStorage.readByte(); // read type double
            double dist = myInputStorage.readDouble();
            addSubscriptionFilterTurn(dist);
        }
        break;
        case libsumo::FILTER_TYPE_VCLASS: {
            myInputStorage.readByte(); // read type stringlist
            SVCPermissions vClasses = parseVehicleClasses(myInputStorage.readStringList());
            addSubscriptionFilterVClass(vClasses);
        }
        break;
        case libsumo::FILTER_TYPE_VTYPE: {
            myInputStorage.readByte(); // read type stringlist
            std::vector<std::string> vTypesVector = myInputStorage.readStringList();
            std::set<std::string> vTypesSet;
            vTypesSet.insert(vTypesVector.begin(), vTypesVector.end());
            addSubscriptionFilterVType(vTypesSet);
        }
        break;
        case libsumo::FILTER_TYPE_FIELD_OF_VISION: {
            myInputStorage.readByte();  // read type double
            double angle = myInputStorage.readDouble();
            addSubscriptionFilterFieldOfVision(angle);
        }
        break;
        case libsumo::FILTER_TYPE_LATERAL_DIST: {
            myInputStorage.readByte();  // read type double
            double dist = myInputStorage.readDouble();
            addSubscriptionFilterLateralDistance(dist);
        }
        break;
        default:
            writeStatusCmd(libsumo::CMD_ADD_SUBSCRIPTION_FILTER, libsumo::RTYPE_NOTIMPLEMENTED,
                           "'" + toString(filterType) + "' is no valid filter type code.");
            success  = false;
    }

    if (success) {
        // acknowledge filter addition
        writeStatusCmd(libsumo::CMD_ADD_SUBSCRIPTION_FILTER, libsumo::RTYPE_OK, "");
    }

    return success;
}


void
TraCIServer::removeFilters() {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Removing filters" << std::endl;
#endif
    myLastContextSubscription->activeFilters = libsumo::SUBS_FILTER_NONE;
}

void
TraCIServer::addSubscriptionFilterLanes(std::vector<int> lanes) {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Adding lane filter (lanes=" << toString(lanes) << ")" << std::endl;
#endif
    myLastContextSubscription->activeFilters = myLastContextSubscription->activeFilters | libsumo::SUBS_FILTER_LANES;
    myLastContextSubscription->filterLanes = lanes;
}

void
TraCIServer::addSubscriptionFilterNoOpposite() {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Adding no opposite filter" << std::endl;
#endif
    myLastContextSubscription->activeFilters = myLastContextSubscription->activeFilters | libsumo::SUBS_FILTER_NOOPPOSITE;
}

void
TraCIServer::addSubscriptionFilterDownstreamDistance(double dist) {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Adding downstream dist filter (dist=" << toString(dist) << ")" << std::endl;
#endif
    myLastContextSubscription->activeFilters = myLastContextSubscription->activeFilters | libsumo::SUBS_FILTER_DOWNSTREAM_DIST;
    myLastContextSubscription->filterDownstreamDist = dist;
}

void
TraCIServer::addSubscriptionFilterUpstreamDistance(double dist) {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Adding upstream dist filter (dist=" << toString(dist) << ")" << std::endl;
#endif
    myLastContextSubscription->activeFilters = myLastContextSubscription->activeFilters | libsumo::SUBS_FILTER_UPSTREAM_DIST;
    myLastContextSubscription->filterUpstreamDist = dist;
}

void
TraCIServer::addSubscriptionFilterLeadFollow() {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Adding Lead/Follow-maneuver filter" << std::endl;
#endif
    myLastContextSubscription->activeFilters = myLastContextSubscription->activeFilters | libsumo::SUBS_FILTER_LEAD_FOLLOW;
}

void
TraCIServer::addSubscriptionFilterTurn(double dist) {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Adding turn-maneuver filter" << std::endl;
#endif
    myLastContextSubscription->activeFilters = myLastContextSubscription->activeFilters | libsumo::SUBS_FILTER_TURN;
    myLastContextSubscription->filterFoeDistToJunction = dist;
}

void
TraCIServer::addSubscriptionFilterVClass(SVCPermissions vClasses) {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Adding vClass filter (vClasses=" << toString(vClasses) << ")" << std::endl;
#endif
    myLastContextSubscription->activeFilters = myLastContextSubscription->activeFilters | libsumo::SUBS_FILTER_VCLASS;
    myLastContextSubscription->filterVClasses = vClasses;
}

void
TraCIServer::addSubscriptionFilterVType(std::set<std::string> vTypes) {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Adding vType filter (vTypes=" << toString(vTypes) << ")" << std::endl;
#endif
    myLastContextSubscription->activeFilters = myLastContextSubscription->activeFilters | libsumo::SUBS_FILTER_VTYPE;
    myLastContextSubscription->filterVTypes = vTypes;
}

void
TraCIServer::addSubscriptionFilterFieldOfVision(double openingAngle) {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Adding FieldOfVision filter (openingAngle=" << toString(openingAngle) << ")" << std::endl;
#endif
    myLastContextSubscription->activeFilters = myLastContextSubscription->activeFilters | libsumo::SUBS_FILTER_FIELD_OF_VISION;
    myLastContextSubscription->filterFieldOfVisionOpeningAngle = openingAngle;
}

void
TraCIServer::addSubscriptionFilterLateralDistance(double dist) {
#ifdef DEBUG_SUBSCRIPTION_FILTERS
    std::cout << "Adding lateral dist filter (dist=" << toString(dist) << ")" << std::endl;
#endif
    myLastContextSubscription->activeFilters = myLastContextSubscription->activeFilters | libsumo::SUBS_FILTER_LATERAL_DIST;
    myLastContextSubscription->filterLateralDist = dist;
}

void
TraCIServer::writeResponseWithLength(tcpip::Storage& outputStorage, tcpip::Storage& tempMsg) {
    if (tempMsg.size() < 254) {
        outputStorage.writeUnsignedByte(1 + (int)tempMsg.size()); // command length -> short
    } else {
        outputStorage.writeUnsignedByte(0); // command length -> extended
        outputStorage.writeInt(1 + 4 + (int)tempMsg.size());
    }
    outputStorage.writeStorage(tempMsg);
}


void
TraCIServer::writePositionVector(tcpip::Storage& outputStorage, const libsumo::TraCIPositionVector& shape) {
    outputStorage.writeUnsignedByte(libsumo::TYPE_POLYGON);
    if (shape.value.size() < 256) {
        outputStorage.writeUnsignedByte((int)shape.value.size());
    } else {
        outputStorage.writeUnsignedByte(0);
        outputStorage.writeInt((int)shape.value.size());
    }
    for (const libsumo::TraCIPosition& pos : shape.value) {
        outputStorage.writeDouble(pos.x);
        outputStorage.writeDouble(pos.y);
    }
}


bool
TraCIServer::readTypeCheckingInt(tcpip::Storage& inputStorage, int& into) {
    if (inputStorage.readUnsignedByte() != libsumo::TYPE_INTEGER) {
        return false;
    }
    into = inputStorage.readInt();
    return true;
}


bool
TraCIServer::readTypeCheckingDouble(tcpip::Storage& inputStorage, double& into) {
    if (inputStorage.readUnsignedByte() != libsumo::TYPE_DOUBLE) {
        return false;
    }
    into = inputStorage.readDouble();
    return true;
}


bool
TraCIServer::readTypeCheckingString(tcpip::Storage& inputStorage, std::string& into) {
    if (inputStorage.readUnsignedByte() != libsumo::TYPE_STRING) {
        return false;
    }
    into = inputStorage.readString();
    return true;
}


bool
TraCIServer::readTypeCheckingStringList(tcpip::Storage& inputStorage, std::vector<std::string>& into) {
    if (inputStorage.readUnsignedByte() != libsumo::TYPE_STRINGLIST) {
        return false;
    }
    into = inputStorage.readStringList();
    return true;
}


bool
TraCIServer::readTypeCheckingDoubleList(tcpip::Storage& inputStorage, std::vector<double>& into) {
    if (inputStorage.readUnsignedByte() != libsumo::TYPE_DOUBLELIST) {
        return false;
    }
    into = inputStorage.readDoubleList();
    return true;
}


bool
TraCIServer::readTypeCheckingColor(tcpip::Storage& inputStorage, libsumo::TraCIColor& into) {
    if (inputStorage.readUnsignedByte() != libsumo::TYPE_COLOR) {
        return false;
    }
    into.r = static_cast<unsigned char>(inputStorage.readUnsignedByte());
    into.g = static_cast<unsigned char>(inputStorage.readUnsignedByte());
    into.b = static_cast<unsigned char>(inputStorage.readUnsignedByte());
    into.a = static_cast<unsigned char>(inputStorage.readUnsignedByte());
    return true;
}


bool
TraCIServer::readTypeCheckingPosition2D(tcpip::Storage& inputStorage, libsumo::TraCIPosition& into) {
    if (inputStorage.readUnsignedByte() != libsumo::POSITION_2D) {
        return false;
    }
    into.x = inputStorage.readDouble();
    into.y = inputStorage.readDouble();
    into.z = 0;
    return true;
}


bool
TraCIServer::readTypeCheckingByte(tcpip::Storage& inputStorage, int& into) {
    if (inputStorage.readByte() != libsumo::TYPE_BYTE) {
        return false;
    }
    into = inputStorage.readByte();
    return true;
}


bool
TraCIServer::readTypeCheckingUnsignedByte(tcpip::Storage& inputStorage, int& into) {
    if (inputStorage.readUnsignedByte() != libsumo::TYPE_UBYTE) {
        return false;
    }
    into = inputStorage.readUnsignedByte();
    return true;
}


bool
TraCIServer::readTypeCheckingPolygon(tcpip::Storage& inputStorage, PositionVector& into) {
    if (inputStorage.readUnsignedByte() != libsumo::TYPE_POLYGON) {
        return false;
    }
    into.clear();
    int size = inputStorage.readUnsignedByte();
    if (size == 0) {
        size = inputStorage.readInt();
    }
    PositionVector shape;
    for (int i = 0; i < size; ++i) {
        double x = inputStorage.readDouble();
        double y = inputStorage.readDouble();
        if (std::isnan(x) || std::isnan(y)) {
            throw libsumo::TraCIException("NaN-Value in shape.");
        }
        into.push_back(Position(x, y));
    }
    return true;
}


void
TraCIServer::stateLoaded(SUMOTime targetTime) {
    myTargetTime = targetTime;
    for (auto& s : mySockets) {
        s.second->targetTime = targetTime;
        s.second->executeMove = false;
        for (auto& stateChange : s.second->vehicleStateChanges) {
            stateChange.second.clear();
        }
        for (auto& stateChange : s.second->transportableStateChanges) {
            stateChange.second.clear();
        }
    }
    mySubscriptions.clear();
    mySubscriptionCache.reset();
}


bool
TraCIServer::centralObject(const libsumo::Subscription& s, const std::string& objID) {
    return (s.id == objID && s.commandId + 32 == s.contextDomain);
}


/****************************************************************************/
