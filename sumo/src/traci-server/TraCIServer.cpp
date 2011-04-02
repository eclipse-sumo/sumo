/****************************************************************************/
/// @file    TraCIServer.cpp
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include "TraCIConstants.h"
#include "TraCIServer.h"

#ifndef NO_TRACI

#include <foreign/tcpip/socket.h>
#include <foreign/tcpip/storage.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/DijkstraRouterTT.h>
#include <utils/common/NamedObjectCont.h>
#include <utils/common/RandHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSRouteHandler.h>
#include <microsim/MSRouteLoaderControl.h>
#include <microsim/MSRouteLoader.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSJunction.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include "TraCIServerAPI_InductionLoop.h"
#include "TraCIServerAPI_Junction.h"
#include "TraCIServerAPI_Lane.h"
#include "TraCIServerAPI_MeMeDetector.h"
#include "TraCIServerAPI_TLS.h"
#include "TraCIServerAPI_Vehicle.h"
#include "TraCIServerAPI_VehicleType.h"
#include "TraCIServerAPI_Route.h"
#include "TraCIServerAPI_POI.h"
#include "TraCIServerAPI_Polygon.h"
#include "TraCIServerAPI_Edge.h"
#include "TraCIServerAPI_Simulation.h"



#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/trigger/MSCalibrator.h>
#include <microsim/MSGlobals.h>

#include <xercesc/sax2/SAX2XMLReader.hpp>

#include <string>
#include <map>
#include <iostream>
#include <utils/common/HelpersHBEFA.h>
#include <utils/common/HelpersHarmonoise.h>


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
namespace traci {

// ===========================================================================
// static member definitions
// ===========================================================================
TraCIServer* TraCIServer::myInstance = 0;
bool TraCIServer::myDoCloseConnection = false;


// ===========================================================================
// method definitions
// ===========================================================================

void
TraCIServer::openSocket(const std::map<int, CmdExecutor> &execs) {
    if (myInstance == 0) {
        if (!myDoCloseConnection && OptionsCont::getOptions().getInt("remote-port") != 0) {
            myInstance = new traci::TraCIServer();
            for (std::map<int, CmdExecutor>::const_iterator i=execs.begin(); i!=execs.end(); ++i) {
                myInstance->myExecutors[i->first] = i->second;
            }
        }
    }
}

/*****************************************************************************/

TraCIServer::TraCIServer() {
    myVehicleStateChanges[MSNet::VEHICLE_STATE_BUILT] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_DEPARTED] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_STARTING_TELEPORT] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_ENDING_TELEPORT] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_NEWROUTE] = std::vector<std::string>();
    MSNet::getInstance()->addVehicleStateListener(this);

    myExecutors[CMD_GET_INDUCTIONLOOP_VARIABLE] = &TraCIServerAPI_InductionLoop::processGet;
    myExecutors[CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE] = &TraCIServerAPI_MeMeDetector::processGet;
    myExecutors[CMD_GET_TL_VARIABLE] = &TraCIServerAPI_TLS::processGet;
    myExecutors[CMD_SET_TL_VARIABLE] = &TraCIServerAPI_TLS::processSet;
    myExecutors[CMD_GET_LANE_VARIABLE] = &TraCIServerAPI_Lane::processGet;
    myExecutors[CMD_SET_LANE_VARIABLE] = &TraCIServerAPI_Lane::processSet;
    myExecutors[CMD_GET_VEHICLE_VARIABLE] = &TraCIServerAPI_Vehicle::processGet;
    myExecutors[CMD_SET_VEHICLE_VARIABLE] = &TraCIServerAPI_Vehicle::processSet;
    myExecutors[CMD_GET_VEHICLETYPE_VARIABLE] = &TraCIServerAPI_VehicleType::processGet;
    myExecutors[CMD_GET_ROUTE_VARIABLE] = &TraCIServerAPI_Route::processGet;
    myExecutors[CMD_GET_POI_VARIABLE] = &TraCIServerAPI_POI::processGet;
    myExecutors[CMD_SET_POI_VARIABLE] = &TraCIServerAPI_POI::processSet;
    myExecutors[CMD_GET_POLYGON_VARIABLE] = &TraCIServerAPI_Polygon::processGet;
    myExecutors[CMD_SET_POLYGON_VARIABLE] = &TraCIServerAPI_Polygon::processSet;
    myExecutors[CMD_GET_JUNCTION_VARIABLE] = &TraCIServerAPI_Junction::processGet;
    myExecutors[CMD_GET_EDGE_VARIABLE] = &TraCIServerAPI_Edge::processGet;
    myExecutors[CMD_SET_EDGE_VARIABLE] = &TraCIServerAPI_Edge::processSet;
    myExecutors[CMD_GET_SIM_VARIABLE] = &TraCIServerAPI_Simulation::processGet;

    OptionsCont &oc = OptionsCont::getOptions();
    myTargetTime = 0;
    myDoCloseConnection = false;
    myDoingSimStep = false;
    myHaveWarnedDeprecation = false;

    // display warning if internal lanes are not used
    if (!MSGlobals::gUsingInternalLanes) {
        MsgHandler::getWarningInstance()->inform("Starting TraCI without using internal lanes!");
        MsgHandler::getWarningInstance()->inform("Vehicles will jump over junctions.", false);
        MsgHandler::getWarningInstance()->inform("Use without option --no-internal-links to avoid unexpected behavior", false);
    }

    try {
        int port = oc.getInt("remote-port");
        // Opens listening socket
        MsgHandler::getMessageInstance()->inform("***Starting server on port " + toString(port) + " ***");
        mySocket = new tcpip::Socket(port);
        mySocket->accept();
        // When got here, a client has connected
    } catch (tcpip::SocketException &e) {
        throw ProcessError(e.what());
    }
}

/*****************************************************************************/

TraCIServer::~TraCIServer() {
    MSNet::getInstance()->removeVehicleStateListener(this);
    if (mySocket != NULL) {
        mySocket->close();
        delete mySocket;
    }
}

/*****************************************************************************/

void
TraCIServer::close() {
    if (myInstance!=0) {
        delete myInstance;
        myInstance = 0;
        myDoCloseConnection = true;
    }
}

/*****************************************************************************/

void
TraCIServer::vehicleStateChanged(const SUMOVehicle * const vehicle, MSNet::VehicleState to) {
    if (myDoCloseConnection || OptionsCont::getOptions().getInt("remote-port") == 0) {
        return;
    }
    myVehicleStateChanges[to].push_back(vehicle->getID());
}

/*****************************************************************************/
void
TraCIServer::processCommandsUntilSimStep(SUMOTime step) {
    try {
        if (myInstance == 0) {
            if (!myDoCloseConnection && OptionsCont::getOptions().getInt("remote-port") != 0) {
                myInstance = new traci::TraCIServer();
            } else {
                return;
            }
        }
        if (step < myInstance->myTargetTime) {
            return;
        }
        // Simulation should run until
        // 1. end time reached or
        // 2. got CMD_CLOSE or
        // 3. Client closes socket connection
        if (myInstance->myDoingSimStep) {
            myInstance->postProcessSimulationStep2();
            myInstance->myDoingSimStep = false;
        }
        while (!myDoCloseConnection) {
            if (!myInstance->myInputStorage.valid_pos()) {
                if (myInstance->myOutputStorage.size() > 0) {
                    // send out all answers as one storage
                    myInstance->mySocket->sendExact(myInstance->myOutputStorage);
                }
                myInstance->myInputStorage.reset();
                myInstance->myOutputStorage.reset();
                // Read a message
                myInstance->mySocket->receiveExact(myInstance->myInputStorage);
            }
            while (myInstance->myInputStorage.valid_pos() && !myDoCloseConnection) {
                // dispatch each command
                int cmd = myInstance->dispatchCommand();
                if (cmd==CMD_SIMSTEP2) {
                    myInstance->myDoingSimStep = true;
                    for (std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i=myInstance->myVehicleStateChanges.begin(); i!=myInstance->myVehicleStateChanges.end(); ++i) {
                        (*i).second.clear();
                    }
                    return;
                }
            }
        }
        if (myDoCloseConnection && myInstance->myOutputStorage.size() > 0) {
            // send out all answers as one storage
            myInstance->mySocket->sendExact(myInstance->myOutputStorage);
        }
        for (std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i=myInstance->myVehicleStateChanges.begin(); i!=myInstance->myVehicleStateChanges.end(); ++i) {
            (*i).second.clear();
        }
    } catch (std::invalid_argument &e) {
        throw ProcessError(e.what());
    } catch (TraCIException &e) {
        throw ProcessError(e.what());
    } catch (tcpip::SocketException &e) {
        throw ProcessError(e.what());
    }
    if (myInstance != NULL) {
        delete myInstance;
        myInstance = 0;
        myDoCloseConnection = true;
    }
}

bool
TraCIServer::wasClosed() {
    return myDoCloseConnection;
}

/*****************************************************************************/

int
TraCIServer::dispatchCommand() {
    unsigned int commandStart = myInputStorage.position();
    unsigned int commandLength = myInputStorage.readUnsignedByte();
    if (commandLength==0) {
        commandLength = myInputStorage.readInt();
    }

    int commandId = myInputStorage.readUnsignedByte();
    bool success = false;
    // dispatch commands
    if (myExecutors.find(commandId)!=myExecutors.end()) {
        success = myExecutors[commandId](*this, myInputStorage, myOutputStorage);
    } else {
        switch (commandId) {
        case CMD_GETVERSION:
            success = commandGetVersion();
            break;
        case CMD_SIMSTEP2: {
            SUMOTime nextT = myInputStorage.readInt();
            success = true;
            if (nextT!=0) {
                myTargetTime = nextT;
            } else {
                myTargetTime += DELTA_T;
            }
            return commandId;
        }
        case CMD_CLOSE:
            success = commandCloseConnection();
            break;
        case CMD_POSITIONCONVERSION: {
            if (!myHaveWarnedDeprecation) {
                MsgHandler::getWarningInstance()->inform("Using old TraCI API, please update your client!");
                myHaveWarnedDeprecation = true;
            }
            tcpip::Storage tempMsg;
            success = TraCIServerAPI_Simulation::commandPositionConversion(*this, myInputStorage, tempMsg, CMD_POSITIONCONVERSION);
            if (success) {
                writeStatusCmd(CMD_POSITIONCONVERSION, RTYPE_OK, "");
                myOutputStorage.writeStorage(tempMsg);
            }
                                     }
            break;
        case CMD_ADDVEHICLE:
            if (!myHaveWarnedDeprecation) {
                MsgHandler::getWarningInstance()->inform("Using old TraCI API, please update your client!");
                myHaveWarnedDeprecation = true;
            }
            success = commandAddVehicle();
            break;
        case CMD_DISTANCEREQUEST: {
            if (!myHaveWarnedDeprecation) {
                MsgHandler::getWarningInstance()->inform("Using old TraCI API, please update your client!");
                myHaveWarnedDeprecation = true;
            }
            tcpip::Storage tempMsg;
            success = TraCIServerAPI_Simulation::commandDistanceRequest(*this, myInputStorage, tempMsg, CMD_DISTANCEREQUEST);
            if (success) {
                writeStatusCmd(CMD_DISTANCEREQUEST, RTYPE_OK, "");
                myOutputStorage.writeStorage(tempMsg);
            }
                                  }
            break;
        case CMD_SUBSCRIBE_INDUCTIONLOOP_VARIABLE:
        case CMD_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE:
        case CMD_SUBSCRIBE_TL_VARIABLE:
        case CMD_SUBSCRIBE_LANE_VARIABLE:
        case CMD_SUBSCRIBE_VEHICLE_VARIABLE:
        case CMD_SUBSCRIBE_VEHICLETYPE_VARIABLE:
        case CMD_SUBSCRIBE_ROUTE_VARIABLE:
        case CMD_SUBSCRIBE_POI_VARIABLE:
        case CMD_SUBSCRIBE_POLYGON_VARIABLE:
        case CMD_SUBSCRIBE_JUNCTION_VARIABLE:
        case CMD_SUBSCRIBE_EDGE_VARIABLE:
        case CMD_SUBSCRIBE_SIM_VARIABLE:
        case CMD_SUBSCRIBE_GUI_VARIABLE:
            success = addSubscription(commandId);
            break;
        default:
            writeStatusCmd(commandId, RTYPE_NOTIMPLEMENTED, "Command not implemented in sumo");
        }
    }
    if (!success) {
        while (myInputStorage.valid_pos() && myInputStorage.position() < commandStart + commandLength) {
            myInputStorage.readChar();
        }
    }
    if (myInputStorage.position() != commandStart + commandLength) {
        std::ostringstream msg;
        msg << "Wrong position in requestMessage after dispatching command.";
        msg << " Expected command length was " << commandLength;
        msg << " but " << myInputStorage.position() - commandStart << " Bytes were read.";
        writeStatusCmd(commandId, RTYPE_ERR, msg.str());
        myDoCloseConnection = true;
    }
    return commandId;
}

/*****************************************************************************/

void
TraCIServer::postProcessSimulationStep2() {
    SUMOTime t = MSNet::getInstance()->getCurrentTimeStep();
    writeStatusCmd(CMD_SIMSTEP2, RTYPE_OK, "");
    int noActive = 0;
    for (std::vector<Subscription>::iterator i=mySubscriptions.begin(); i!=mySubscriptions.end();) {
        const Subscription &s = *i;
        bool isArrivedVehicle = (s.commandId == CMD_SUBSCRIBE_VEHICLE_VARIABLE) && (find(myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED].begin(), myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED].end(), s.id) != myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED].end());
        if ((s.endTime<t) || isArrivedVehicle) {
            i = mySubscriptions.erase(i);
            continue;
        }
        ++i;
        if (s.beginTime>t) {
            continue;
        }
        ++noActive;
    }
    myOutputStorage.writeInt(noActive);
    for (std::vector<Subscription>::iterator i=mySubscriptions.begin(); i!=mySubscriptions.end(); ++i) {
        const Subscription &s = *i;
        if (s.beginTime>t) {
            continue;
        }
        tcpip::Storage into;
        std::string errors;
        processSingleSubscription(s, into, errors);
        myOutputStorage.writeStorage(into);
    }
}

/*****************************************************************************/

bool
TraCIServer::commandGetVersion() {

    int apiVersion = 1;
    std::string sumoVersion = VERSION_STRING;

    // Prepare response
    tcpip::Storage answerTmp;

    answerTmp.writeInt(apiVersion);
    answerTmp.writeString(sumoVersion);

    // When we get here, the response is stored in answerTmp -> put into myOutputStorage
    writeStatusCmd(CMD_GETVERSION, RTYPE_OK, "");

    // command length
    myOutputStorage.writeUnsignedByte(1 + 1 + static_cast<int>(answerTmp.size()));
    // command type
    myOutputStorage.writeUnsignedByte(CMD_GETVERSION);
    // and the parameter dependant part
    myOutputStorage.writeStorage(answerTmp);
    return true;
}

/*****************************************************************************/

bool
TraCIServer::commandCloseConnection() {
    myDoCloseConnection = true;
    // write answer
    writeStatusCmd(CMD_CLOSE, RTYPE_OK, "Goodbye");
    return true;
}

/*****************************************************************************/

bool
TraCIServer::commandAddVehicle() {

    // read parameters
    std::string vehicleId = myInputStorage.readString();
    std::string vehicleTypeId = myInputStorage.readString();
    std::string routeId = myInputStorage.readString();
    std::string laneId = myInputStorage.readString();
    SUMOReal insertionPosition = myInputStorage.readFloat();
    SUMOReal insertionSpeed = myInputStorage.readFloat();

    // find vehicleType
    MSVehicleType *vehicleType = MSNet::getInstance()->getVehicleControl().getVType(vehicleTypeId);
    if (!vehicleType) {
        writeStatusCmd(CMD_ADDVEHICLE, RTYPE_ERR, "Invalid vehicleTypeId: '"+vehicleTypeId+"'");
        return false;
    }

    // find route
    const MSRoute *route = MSRoute::dictionary(routeId);
    if (!route) {
        writeStatusCmd(CMD_ADDVEHICLE, RTYPE_ERR, "Invalid routeId: '"+routeId+"'");
        return false;
    }

    // find lane
    MSLane *lane;
    if (laneId != "") {
        lane = MSLane::dictionary(laneId);
        if (!lane) {
            writeStatusCmd(CMD_ADDVEHICLE, RTYPE_ERR, "Invalid laneId: '"+laneId+"'");
            return false;
        }
    } else {
        lane = route->getEdges()[0]->getLanes()[0];
        if (!lane) {
            writeStatusCmd(CMD_STOP, RTYPE_ERR, "Could not find first lane of first edge in routeId '"+routeId+"'");
            return false;
        }
    }

    if (&lane->getEdge()!=*route->begin()) {
        writeStatusCmd(CMD_STOP, RTYPE_ERR, "The route must start at the edge the lane starts at.");
        return false;
    }

    // build vehicle
    SUMOVehicleParameter* vehicleParams = new SUMOVehicleParameter();
    vehicleParams->id = vehicleId;
    vehicleParams->depart = MSNet::getInstance()->getCurrentTimeStep()+1;
    MSVehicle *vehicle = static_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().buildVehicle(vehicleParams, route, vehicleType));
    if (vehicle == NULL) {
        writeStatusCmd(CMD_STOP, RTYPE_ERR, "Could not build vehicle");
        return false;
    }

    // calculate speed
    float clippedInsertionSpeed;
    if (insertionSpeed<0) {
        clippedInsertionSpeed = (float) MIN2(lane->getMaxSpeed(), vehicle->getMaxSpeed());
    } else {
        clippedInsertionSpeed = (float) MIN3(lane->getMaxSpeed(), vehicle->getMaxSpeed(), insertionSpeed);
    }

    // insert vehicle into the dictionary
    if (!MSNet::getInstance()->getVehicleControl().addVehicle(vehicle->getID(), vehicle)) {
        writeStatusCmd(CMD_ADDVEHICLE, RTYPE_ERR, "Could not add vehicle to VehicleControl");
        return false;
    }

    // try to emit
    if (!lane->isInsertionSuccess(vehicle, clippedInsertionSpeed, insertionPosition, true)) {
        MSNet::getInstance()->getVehicleControl().deleteVehicle(vehicle);
        writeStatusCmd(CMD_ADDVEHICLE, RTYPE_ERR, "Could not insert vehicle");
        return false;
    }

    // exec callback
    vehicle->onDepart();

    // create a reply message
    writeStatusCmd(CMD_ADDVEHICLE, RTYPE_OK, "");

    return true;
}


/*****************************************************************************/

void
TraCIServer::writeStatusCmd(int commandId, int status, const std::string &description) {
    writeStatusCmd(commandId, status, description, myOutputStorage);
}


void
TraCIServer::writeStatusCmd(int commandId, int status, const std::string &description, tcpip::Storage &outputStorage) {
    if (status == RTYPE_ERR) {
        MsgHandler::getErrorInstance()->inform("Answered with error to command " + toString(commandId) + ": " + description);
    } else if (status == RTYPE_NOTIMPLEMENTED) {
        MsgHandler::getErrorInstance()->inform("Requested command not implemented (" + toString(commandId) + "): " + description);
    }
    outputStorage.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(description.length())); // command length
    outputStorage.writeUnsignedByte(commandId); // command type
    outputStorage.writeUnsignedByte(status); // status
    outputStorage.writeString(description); // description
}

/*****************************************************************************/

bool
TraCIServer::addSubscription(int commandId) {
    SUMOTime beginTime = myInputStorage.readInt();
    SUMOTime endTime = myInputStorage.readInt();
    std::string id = myInputStorage.readString();
    int no = myInputStorage.readUnsignedByte();
    std::vector<int> variables;
    for (int i=0; i<no; ++i) {
        variables.push_back(myInputStorage.readUnsignedByte());
    }
    // check subscribe/unsubscribe
    bool ok = true;
    if (variables.size()==0) {
        // try unsubscribe
        bool found = false;
        for (std::vector<Subscription>::iterator j=mySubscriptions.begin(); j!=mySubscriptions.end();) {
            if ((*j).id==id&&(*j).commandId==commandId) {
                j = mySubscriptions.erase(j);
                found = true;
                continue;
            }
            ++j;
        }
        if (found) {
            writeStatusCmd(commandId, RTYPE_OK, "");
        } else {
            writeStatusCmd(commandId, RTYPE_OK, "The subscription to remove was not found.");
        }
    } else {
        // process subscription
        Subscription s(commandId, id, variables, beginTime, endTime);
        tcpip::Storage writeInto;
        std::string errors;
        if (s.endTime<MSNet::getInstance()->getCurrentTimeStep()) {
            processSingleSubscription(s, writeInto, errors);
            writeStatusCmd(s.commandId, RTYPE_ERR, "Subscription has ended.");
        } else {
            if (processSingleSubscription(s, writeInto, errors)) {
                mySubscriptions.push_back(s);
                writeStatusCmd(s.commandId, RTYPE_OK, "");
            } else {
                writeStatusCmd(s.commandId, RTYPE_ERR, "Could not add subscription (" + errors + ").");
            }
        }
        myOutputStorage.writeStorage(writeInto);
    }
    return ok;
}


bool
TraCIServer::processSingleSubscription(const Subscription &s, tcpip::Storage &writeInto,
                                       std::string &errors) {
    bool ok = true;
    tcpip::Storage outputStorage;
    for (std::vector<int>::const_iterator i=s.variables.begin(); i!=s.variables.end(); ++i) {
        tcpip::Storage message;
        message.writeUnsignedByte(*i);
        message.writeString(s.id);
        tcpip::Storage tmpOutput;
        int getId = s.commandId - 0x30;
        if (myExecutors.find(getId)!=myExecutors.end()) {
            ok &= myExecutors[getId](*this, message, tmpOutput);
        } else {
            writeStatusCmd(s.commandId, RTYPE_NOTIMPLEMENTED, "Unsupported command specified", tmpOutput);
            ok = false;
        }
        // copy response part
        if (ok) {
            int length = tmpOutput.readUnsignedByte();
            while (--length>0) tmpOutput.readUnsignedByte();
            length = tmpOutput.readUnsignedByte();
            length = tmpOutput.readInt();
            //read responseType 
			tmpOutput.readUnsignedByte();
            int variable = tmpOutput.readUnsignedByte();
            std::string id = tmpOutput.readString();
            outputStorage.writeUnsignedByte(variable);
            outputStorage.writeUnsignedByte(RTYPE_OK);
            length -= (1+4+1+4+(int)id.length());
            while (--length>0) {
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
            outputStorage.writeUnsignedByte(RTYPE_ERR);
            outputStorage.writeUnsignedByte(TYPE_STRING);
            outputStorage.writeString(msg);
            errors = errors + msg;
        }
    }
    writeInto.writeUnsignedByte(0); // command length -> extended
    writeInto.writeInt((1+4) + 1 + (4 + (int)(s.id.length())) + 1 + (int)outputStorage.size());
    writeInto.writeUnsignedByte(s.commandId + 0x10);
    writeInto.writeString(s.id);
    writeInto.writeUnsignedByte((int)(s.variables.size()));
    writeInto.writeStorage(outputStorage);
    return ok;
}

}

#endif
