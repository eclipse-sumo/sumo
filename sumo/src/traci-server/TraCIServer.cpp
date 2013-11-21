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
/// @date    2007/10/24
/// @version $Id$
///
/// TraCI server used to control sumo by a remote TraCI client (e.g., ns2)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#ifndef NO_TRACI

#ifdef HAVE_PYTHON
#include <Python.h>
#endif

#include <string>
#include <map>
#include <iostream>
#include <foreign/tcpip/socket.h>
#include <foreign/tcpip/storage.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/DijkstraRouterTT.h>
#include <utils/common/NamedObjectCont.h>
#include <utils/common/RandHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/HelpersHBEFA.h>
#include <utils/common/HelpersHarmonoise.h>
#include <utils/common/SUMOVehicleParameter.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/shapes/Polygon.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSJunction.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include "TraCIConstants.h"
#include "TraCIServer.h"
#include "TraCIServerAPI_InductionLoop.h"
#include "TraCIServerAPI_Junction.h"
#include "TraCIServerAPI_Lane.h"
#include "TraCIServerAPI_MeMeDetector.h"
#include "TraCIServerAPI_ArealDetector.h"

#include "TraCIServerAPI_TLS.h"
#include "TraCIServerAPI_Vehicle.h"
#include "TraCIServerAPI_VehicleType.h"
#include "TraCIServerAPI_Route.h"
#include "TraCIServerAPI_POI.h"
#include "TraCIServerAPI_Polygon.h"
#include "TraCIServerAPI_Edge.h"
#include "TraCIServerAPI_Simulation.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
TraCIServer* TraCIServer::myInstance = 0;
bool TraCIServer::myDoCloseConnection = false;


// ===========================================================================
// method definitions
// ===========================================================================
TraCIServer::TraCIServer(const SUMOTime begin, const int port)
    : mySocket(0), myTargetTime(begin), myDoingSimStep(false), myAmEmbedded(port == 0), myLaneTree(0) {

    myVehicleStateChanges[MSNet::VEHICLE_STATE_BUILT] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_DEPARTED] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_STARTING_TELEPORT] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_ENDING_TELEPORT] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_NEWROUTE] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_STARTING_PARKING] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_ENDING_PARKING] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_STARTING_STOP] = std::vector<std::string>();
    myVehicleStateChanges[MSNet::VEHICLE_STATE_ENDING_STOP] = std::vector<std::string>();
    MSNet::getInstance()->addVehicleStateListener(this);

    myExecutors[CMD_GET_INDUCTIONLOOP_VARIABLE] = &TraCIServerAPI_InductionLoop::processGet;
	myExecutors[CMD_GET_AREAL_DETECTOR_VARIABLE] = &TraCIServerAPI_ArealDetector::processGet;
	myExecutors[CMD_GET_MULTI_ENTRY_EXIT_DETECTOR_VARIABLE] = &TraCIServerAPI_MeMeDetector::processGet;

    myExecutors[CMD_GET_TL_VARIABLE] = &TraCIServerAPI_TLS::processGet;
    myExecutors[CMD_SET_TL_VARIABLE] = &TraCIServerAPI_TLS::processSet;
    myExecutors[CMD_GET_LANE_VARIABLE] = &TraCIServerAPI_Lane::processGet;
    myExecutors[CMD_SET_LANE_VARIABLE] = &TraCIServerAPI_Lane::processSet;
    myExecutors[CMD_GET_VEHICLE_VARIABLE] = &TraCIServerAPI_Vehicle::processGet;
    myExecutors[CMD_SET_VEHICLE_VARIABLE] = &TraCIServerAPI_Vehicle::processSet;
    myExecutors[CMD_GET_VEHICLETYPE_VARIABLE] = &TraCIServerAPI_VehicleType::processGet;
    myExecutors[CMD_SET_VEHICLETYPE_VARIABLE] = &TraCIServerAPI_VehicleType::processSet;
    myExecutors[CMD_GET_ROUTE_VARIABLE] = &TraCIServerAPI_Route::processGet;
    myExecutors[CMD_SET_ROUTE_VARIABLE] = &TraCIServerAPI_Route::processSet;
    myExecutors[CMD_GET_POI_VARIABLE] = &TraCIServerAPI_POI::processGet;
    myExecutors[CMD_SET_POI_VARIABLE] = &TraCIServerAPI_POI::processSet;
    myExecutors[CMD_GET_POLYGON_VARIABLE] = &TraCIServerAPI_Polygon::processGet;
    myExecutors[CMD_SET_POLYGON_VARIABLE] = &TraCIServerAPI_Polygon::processSet;
    myExecutors[CMD_GET_JUNCTION_VARIABLE] = &TraCIServerAPI_Junction::processGet;
    myExecutors[CMD_GET_EDGE_VARIABLE] = &TraCIServerAPI_Edge::processGet;
    myExecutors[CMD_SET_EDGE_VARIABLE] = &TraCIServerAPI_Edge::processSet;
    myExecutors[CMD_GET_SIM_VARIABLE] = &TraCIServerAPI_Simulation::processGet;
    myExecutors[CMD_SET_SIM_VARIABLE] = &TraCIServerAPI_Simulation::processSet;

    myDoCloseConnection = false;

    // display warning if internal lanes are not used
    if (!MSGlobals::gUsingInternalLanes) {
        WRITE_WARNING("Starting TraCI without using internal lanes!");
        MsgHandler::getWarningInstance()->inform("Vehicles will jump over junctions.", false);
        MsgHandler::getWarningInstance()->inform("Use without option --no-internal-links to avoid unexpected behavior", false);
    }

    if (!myAmEmbedded) {
        try {
            WRITE_MESSAGE("***Starting server on port " + toString(port) + " ***");
            mySocket = new tcpip::Socket(port);
            mySocket->accept();
            // When got here, a client has connected
        } catch (tcpip::SocketException& e) {
            throw ProcessError(e.what());
        }
    }
}


TraCIServer::~TraCIServer() {
    MSNet::getInstance()->removeVehicleStateListener(this);
    if (mySocket != NULL) {
        mySocket->close();
        delete mySocket;
    }
    for (std::map<int, NamedRTree*>::const_iterator i = myObjects.begin(); i != myObjects.end(); ++i) {
        delete(*i).second;
    }
    delete myLaneTree;
}


// ---------- Initialisation and Shutdown
void
TraCIServer::openSocket(const std::map<int, CmdExecutor>& execs) {
    if (myInstance == 0) {
        if (!myDoCloseConnection && OptionsCont::getOptions().getInt("remote-port") != 0) {
            myInstance = new TraCIServer(string2time(OptionsCont::getOptions().getString("begin")),
                                         OptionsCont::getOptions().getInt("remote-port"));
            for (std::map<int, CmdExecutor>::const_iterator i = execs.begin(); i != execs.end(); ++i) {
                myInstance->myExecutors[i->first] = i->second;
            }
        }
    }
}


void
TraCIServer::close() {
    if (myInstance != 0) {
        delete myInstance;
        myInstance = 0;
        myDoCloseConnection = true;
    }
}


bool
TraCIServer::wasClosed() {
    return myDoCloseConnection;
}


void
TraCIServer::setVTDControlled(MSVehicle* v, MSLane* l, SUMOReal pos, int edgeOffset, MSEdgeVector route) {
    myVTDControlledVehicles[v->getID()] = v;
    v->getInfluencer().setVTDControlled(true, l, pos, edgeOffset, route);
}

void
TraCIServer::postProcessVTD() {
    for (std::map<std::string, MSVehicle*>::const_iterator i = myVTDControlledVehicles.begin(); i != myVTDControlledVehicles.end(); ++i) {
        if (MSNet::getInstance()->getVehicleControl().getVehicle((*i).first) != 0) {
            (*i).second->getInfluencer().postProcessVTD((*i).second);
        } else {
            WRITE_WARNING("Vehicle '" + (*i).first + "' was removed though being controlled by VTD");
        }
    }
    myVTDControlledVehicles.clear();
}


bool
TraCIServer::vtdDebug() const {
    return true;
}


// ---------- Initialisation and Shutdown


void
TraCIServer::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to) {
    if (!myDoCloseConnection) {
        myVehicleStateChanges[to].push_back(vehicle->getID());
    }
}


void
TraCIServer::processCommandsUntilSimStep(SUMOTime step) {
    try {
        if (myInstance == 0) {
            if (!myDoCloseConnection && OptionsCont::getOptions().getInt("remote-port") != 0) {
                myInstance = new TraCIServer(string2time(OptionsCont::getOptions().getString("begin")),
                                             OptionsCont::getOptions().getInt("remote-port"));
            } else {
                return;
            }
        }
        if (myInstance->myAmEmbedded || step < myInstance->myTargetTime) {
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
                if (cmd == CMD_SIMSTEP2) {
                    myInstance->myDoingSimStep = true;
                    for (std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i = myInstance->myVehicleStateChanges.begin(); i != myInstance->myVehicleStateChanges.end(); ++i) {
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
        for (std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i = myInstance->myVehicleStateChanges.begin(); i != myInstance->myVehicleStateChanges.end(); ++i) {
            (*i).second.clear();
        }
    } catch (std::invalid_argument& e) {
        throw ProcessError(e.what());
    } catch (TraCIException& e) {
        throw ProcessError(e.what());
    } catch (tcpip::SocketException& e) {
        throw ProcessError(e.what());
    }
    if (myInstance != NULL) {
        delete myInstance;
        myInstance = 0;
        myDoCloseConnection = true;
    }
}




#ifdef HAVE_PYTHON
// ===========================================================================
// python functions (traciemb module)
// ===========================================================================
static PyObject*
traciemb_execute(PyObject* /* self */, PyObject* args) {
    const char* msg;
    int size;
    if (!PyArg_ParseTuple(args, "s#", &msg, &size)) {
        return NULL;
    }
    std::string result = TraCIServer::execute(std::string(msg, size));
    return Py_BuildValue("s#", result.c_str(), result.size());
}

static PyMethodDef EmbMethods[] = {
    {
        "execute", traciemb_execute, METH_VARARGS,
        "Execute the given TraCI command and return the result."
    },
    {NULL, NULL, 0, NULL}
};


std::string
TraCIServer::execute(std::string cmd) {
    try {
        if (myInstance == 0) {
            if (!myDoCloseConnection) {
                myInstance = new TraCIServer(string2time(OptionsCont::getOptions().getString("begin")));
            } else {
                return "";
            }
        }
        myInstance->myInputStorage.reset();
        myInstance->myOutputStorage.reset();
        for (std::string::iterator i = cmd.begin(); i != cmd.end(); ++i) {
            myInstance->myInputStorage.writeChar(*i);
        }
        myInstance->dispatchCommand();
        return std::string(myInstance->myOutputStorage.begin(), myInstance->myOutputStorage.end());
    } catch (std::invalid_argument& e) {
        throw ProcessError(e.what());
    } catch (TraCIException& e) {
        throw ProcessError(e.what());
    } catch (tcpip::SocketException& e) {
        throw ProcessError(e.what());
    }
}


void
TraCIServer::runEmbedded(std::string pyFile) {
    PyObject* pName, *pModule;
    Py_Initialize();
    Py_InitModule("traciemb", EmbMethods);
    if (pyFile.length() > 3 && !pyFile.compare(pyFile.length() - 3, 3, ".py")) {
        PyObject *sys_path, *path;
        sys_path = PySys_GetObject("path");
        if (sys_path == NULL || !PyList_Check(sys_path)) {   
            throw ProcessError("Could not access python sys.path!");
        }
        path = PyString_FromString(FileHelpers::getFilePath(pyFile).c_str());
        PyList_Insert(sys_path, 0, path);
        Py_DECREF(path);
        FILE* pFile = fopen(pyFile.c_str(), "r");
        if (pFile == NULL) {
            throw ProcessError("Failed to load \"" + pyFile + "\"!");
        }
        PyRun_SimpleFile(pFile, pyFile.c_str());
        fclose(pFile);
    } else {
        pName = PyString_FromString(pyFile.c_str());
        /* Error checking of pName left out */
        pModule = PyImport_Import(pName);
        Py_DECREF(pName);
        if (pModule == NULL) {
            PyErr_Print();
            throw ProcessError("Failed to load \"" + pyFile + "\"!");
        }
    }
    Py_Finalize();
}
#endif


int
TraCIServer::dispatchCommand() {
    unsigned int commandStart = myInputStorage.position();
    unsigned int commandLength = myInputStorage.readUnsignedByte();
    if (commandLength == 0) {
        commandLength = myInputStorage.readInt();
    }

    int commandId = myInputStorage.readUnsignedByte();
    bool success = false;
    // dispatch commands
    if (myExecutors.find(commandId) != myExecutors.end()) {
        success = myExecutors[commandId](*this, myInputStorage, myOutputStorage);
    } else {
        switch (commandId) {
            case CMD_GETVERSION:
                success = commandGetVersion();
                break;
            case CMD_SIMSTEP2: {
                SUMOTime nextT = myInputStorage.readInt();
                success = true;
                if (nextT != 0) {
                    myTargetTime = nextT;
                } else {
                    myTargetTime += DELTA_T;
                }
                if (myAmEmbedded) {
                    MSNet::getInstance()->simulationStep();
                    postProcessSimulationStep2();
                    for (std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i = myInstance->myVehicleStateChanges.begin(); i != myInstance->myVehicleStateChanges.end(); ++i) {
                        (*i).second.clear();
                    }
                }
                return commandId;
            }
            case CMD_CLOSE:
                success = commandCloseConnection();
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
                success = addObjectVariableSubscription(commandId);
                break;
            case CMD_SUBSCRIBE_INDUCTIONLOOP_CONTEXT:
            case CMD_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_CONTEXT:
            case CMD_SUBSCRIBE_TL_CONTEXT:
            case CMD_SUBSCRIBE_LANE_CONTEXT:
            case CMD_SUBSCRIBE_VEHICLE_CONTEXT:
            case CMD_SUBSCRIBE_VEHICLETYPE_CONTEXT:
            case CMD_SUBSCRIBE_ROUTE_CONTEXT:
            case CMD_SUBSCRIBE_POI_CONTEXT:
            case CMD_SUBSCRIBE_POLYGON_CONTEXT:
            case CMD_SUBSCRIBE_JUNCTION_CONTEXT:
            case CMD_SUBSCRIBE_EDGE_CONTEXT:
            case CMD_SUBSCRIBE_SIM_CONTEXT:
            case CMD_SUBSCRIBE_GUI_CONTEXT:
                success = addObjectContextSubscription(commandId);
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


// ---------- Server-internal command handling
bool
TraCIServer::commandGetVersion() {
    std::string sumoVersion = VERSION_STRING;
    // Prepare response
    tcpip::Storage answerTmp;
    answerTmp.writeInt(TRACI_VERSION);
    answerTmp.writeString(std::string("SUMO ") + sumoVersion);
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


bool
TraCIServer::commandCloseConnection() {
    myDoCloseConnection = true;
    // write answer
    writeStatusCmd(CMD_CLOSE, RTYPE_OK, "");
    return true;
}


void
TraCIServer::postProcessSimulationStep2() {
    SUMOTime t = MSNet::getInstance()->getCurrentTimeStep();
    writeStatusCmd(CMD_SIMSTEP2, RTYPE_OK, "");
    int noActive = 0;
    for (std::vector<Subscription>::iterator i = mySubscriptions.begin(); i != mySubscriptions.end();) {
        const Subscription& s = *i;
        bool isArrivedVehicle = (s.commandId == CMD_SUBSCRIBE_VEHICLE_VARIABLE || s.commandId == CMD_SUBSCRIBE_VEHICLE_CONTEXT) 
            && (find(myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED].begin(), myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED].end(), s.id) != myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED].end());
        if ((s.endTime < t) || isArrivedVehicle) {
            i = mySubscriptions.erase(i);
            continue;
        }
        ++i;
        if (s.beginTime > t) {
            continue;
        }
        ++noActive;
    }
    myOutputStorage.writeInt(noActive);
    for (std::vector<Subscription>::iterator i = mySubscriptions.begin(); i != mySubscriptions.end(); ) {
        const Subscription& s = *i;
        if (s.beginTime > t) {
            ++i;
            continue;
        }
        tcpip::Storage into;
        std::string errors;
        bool ok = processSingleSubscription(s, into, errors);
        myOutputStorage.writeStorage(into);
        if(ok) {
            ++i;
        } else {
            i = mySubscriptions.erase(i);
        }
    }
}


void
TraCIServer::writeStatusCmd(int commandId, int status, const std::string& description) {
    writeStatusCmd(commandId, status, description, myOutputStorage);
}


void
TraCIServer::writeStatusCmd(int commandId, int status, const std::string& description, tcpip::Storage& outputStorage) {
    if (status == RTYPE_ERR) {
        WRITE_ERROR("Answered with error to command " + toString(commandId) + ": " + description);
    } else if (status == RTYPE_NOTIMPLEMENTED) {
        WRITE_ERROR("Requested command not implemented (" + toString(commandId) + "): " + description);
    }
    outputStorage.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(description.length())); // command length
    outputStorage.writeUnsignedByte(commandId); // command type
    outputStorage.writeUnsignedByte(status); // status
    outputStorage.writeString(description); // description
}


bool
TraCIServer::writeErrorStatusCmd(int commandId, const std::string& description, tcpip::Storage& outputStorage) {
    writeStatusCmd(commandId, RTYPE_ERR, description, outputStorage);
    return false;
}


void
TraCIServer::initialiseSubscription(const TraCIServer::Subscription& s) {
    tcpip::Storage writeInto;
    std::string errors;
    if (processSingleSubscription(s, writeInto, errors)) {
        if (s.endTime < MSNet::getInstance()->getCurrentTimeStep()) {
            writeStatusCmd(s.commandId, RTYPE_ERR, "Subscription has ended.");
        } else {
            mySubscriptions.push_back(s);
            writeStatusCmd(s.commandId, RTYPE_OK, "");
        }
    } else {
        writeStatusCmd(s.commandId, RTYPE_ERR, "Could not add subscription (" + errors + ").");
    }
    myOutputStorage.writeStorage(writeInto);
}


void
TraCIServer::removeSubscription(int commandId, const std::string& id, int domain) {
    bool found = false;
    for (std::vector<Subscription>::iterator j = mySubscriptions.begin(); j != mySubscriptions.end();) {
        if ((*j).id == id && (*j).commandId == commandId && (domain < 0 || (*j).contextDomain == domain)) {
            j = mySubscriptions.erase(j);
            found = true;
            continue;
        }
        ++j;
    }
    // try unsubscribe
    if (found) {
        writeStatusCmd(commandId, RTYPE_OK, "");
    } else {
        writeStatusCmd(commandId, RTYPE_OK, "The subscription to remove was not found.");
    }
}


bool
TraCIServer::findObjectShape(int domain, const std::string& id, PositionVector& shape) {
    Position p;
    switch (domain) {
        case CMD_SUBSCRIBE_INDUCTIONLOOP_CONTEXT:
            if (TraCIServerAPI_InductionLoop::getPosition(id, p)) {
                shape.push_back(p);
                return true;
            }
            break;
        case CMD_SUBSCRIBE_MULTI_ENTRY_EXIT_DETECTOR_CONTEXT:
            break;
        case CMD_SUBSCRIBE_TL_CONTEXT:
            break;
        case CMD_SUBSCRIBE_LANE_CONTEXT:
            if (TraCIServerAPI_Lane::getShape(id, shape)) {
                return true;
            }
            break;
        case CMD_SUBSCRIBE_VEHICLE_CONTEXT:
            if (TraCIServerAPI_Vehicle::getPosition(id, p)) {
                shape.push_back(p);
                return true;
            }
            break;
        case CMD_SUBSCRIBE_VEHICLETYPE_CONTEXT:
            break;
        case CMD_SUBSCRIBE_ROUTE_CONTEXT:
            break;
        case CMD_SUBSCRIBE_POI_CONTEXT:
            if (TraCIServerAPI_POI::getPosition(id, p)) {
                shape.push_back(p);
                return true;
            }
            return false;
        case CMD_SUBSCRIBE_POLYGON_CONTEXT:
            if (TraCIServerAPI_Polygon::getShape(id, shape)) {
                return true;
            }
            break;
        case CMD_SUBSCRIBE_JUNCTION_CONTEXT:
            if (TraCIServerAPI_Junction::getPosition(id, p)) {
                shape.push_back(p);
                return true;
            }
            break;
        case CMD_SUBSCRIBE_EDGE_CONTEXT:
            if (TraCIServerAPI_Edge::getShape(id, shape)) {
               return true;
            }
            break;
        case CMD_SUBSCRIBE_SIM_CONTEXT:
            return false;
        case CMD_SUBSCRIBE_GUI_CONTEXT:
            break;
        default:
            break;
    }
    return false;
}

void
TraCIServer::collectObjectsInRange(int domain, const PositionVector& shape, SUMOReal range, std::set<std::string>& into) {
    // build the look-up tree if not yet existing
    if (myObjects.find(domain) == myObjects.end()) {
        switch (domain) {
            case CMD_GET_INDUCTIONLOOP_VARIABLE:
                myObjects[CMD_GET_INDUCTIONLOOP_VARIABLE] = TraCIServerAPI_InductionLoop::getTree();
                break;
            case CMD_GET_EDGE_VARIABLE:
            case CMD_GET_LANE_VARIABLE:
            case CMD_GET_VEHICLE_VARIABLE:
                myObjects[CMD_GET_EDGE_VARIABLE] = 0;
                myObjects[CMD_GET_LANE_VARIABLE] = 0;
                myObjects[CMD_GET_VEHICLE_VARIABLE] = 0;
                myLaneTree = new RTree<MSLane*, MSLane, float, 2, TraCIServerAPI_Lane::StoringVisitor>(&MSLane::visit);
                MSLane::fill(*myLaneTree);
                break;
            case CMD_GET_POI_VARIABLE:
                myObjects[CMD_GET_POI_VARIABLE] = TraCIServerAPI_POI::getTree();
                break;
            case CMD_GET_POLYGON_VARIABLE:
                myObjects[CMD_GET_POLYGON_VARIABLE] = TraCIServerAPI_Polygon::getTree();
                break;
            case CMD_GET_JUNCTION_VARIABLE:
                myObjects[CMD_GET_JUNCTION_VARIABLE] = TraCIServerAPI_Junction::getTree();
                break;
            default:
                break;
        }
    }
    const Boundary b = shape.getBoxBoundary().grow(range);
    const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
    const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
    switch (domain) {
        case CMD_GET_INDUCTIONLOOP_VARIABLE:
        case CMD_GET_POI_VARIABLE:
        case CMD_GET_POLYGON_VARIABLE:
        case CMD_GET_JUNCTION_VARIABLE: {
            Named::StoringVisitor sv(into);
            myObjects[domain]->Search(cmin, cmax, sv);
        }
        break;
        case CMD_GET_EDGE_VARIABLE:
        case CMD_GET_LANE_VARIABLE:
        case CMD_GET_VEHICLE_VARIABLE: {
            TraCIServerAPI_Lane::StoringVisitor sv(into, shape, range, domain);
            myLaneTree->Search(cmin, cmax, sv);
        }
        break;
        default:
            break;
    }
}


bool
TraCIServer::processSingleSubscription(const Subscription& s, tcpip::Storage& writeInto,
                                       std::string& errors) {
    bool ok = true;
    tcpip::Storage outputStorage;
    const int getCommandId = s.contextVars ? s.contextDomain : s.commandId - 0x30;
    std::set<std::string> objIDs;
    if (s.contextVars) {
        PositionVector shape;
        if (!findObjectShape(s.commandId, s.id, shape)) {
            return false;
        }
        collectObjectsInRange(s.contextDomain, shape, s.range, objIDs);
    } else {
        objIDs.insert(s.id);
    }
    const int numVars = s.contextVars && s.variables.size() == 1 && s.variables[0] == ID_LIST ? 0 : (int)s.variables.size();
    for (std::set<std::string>::iterator j = objIDs.begin(); j != objIDs.end(); ++j) {
        if (s.contextVars) {
            outputStorage.writeString(*j);
        }
        if (numVars > 0) {
            for (std::vector<int>::const_iterator i = s.variables.begin(); i != s.variables.end(); ++i) {
                tcpip::Storage message;
                message.writeUnsignedByte(*i);
                message.writeString(*j);
                tcpip::Storage tmpOutput;
                if (myExecutors.find(getCommandId) != myExecutors.end()) {
                    ok &= myExecutors[getCommandId](*this, message, tmpOutput);
                } else {
                    writeStatusCmd(s.commandId, RTYPE_NOTIMPLEMENTED, "Unsupported command specified", tmpOutput);
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
                    outputStorage.writeUnsignedByte(RTYPE_OK);
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
                    outputStorage.writeUnsignedByte(RTYPE_ERR);
                    outputStorage.writeUnsignedByte(TYPE_STRING);
                    outputStorage.writeString(msg);
                    errors = errors + msg;
                }
            }
        }
    }
    unsigned int length = (1 + 4) + 1 + (4 + (int)(s.id.length())) + 1 + (int)outputStorage.size();
    if (s.contextVars) {
        length += 4;
    }
    writeInto.writeUnsignedByte(0); // command length -> extended
    writeInto.writeInt(length);
    writeInto.writeUnsignedByte(s.commandId + 0x10);
    writeInto.writeString(s.id);
    if (s.contextVars) {
        writeInto.writeUnsignedByte(s.contextDomain);
    }
    writeInto.writeUnsignedByte(numVars);
    if (s.contextVars) {
        writeInto.writeInt((int)objIDs.size());
    }
    if (!s.contextVars || objIDs.size() != 0) {
        writeInto.writeStorage(outputStorage);
    }
    return ok;
}


bool
TraCIServer::addObjectVariableSubscription(int commandId) {
    SUMOTime beginTime = myInputStorage.readInt();
    SUMOTime endTime = myInputStorage.readInt();
    std::string id = myInputStorage.readString();
    int no = myInputStorage.readUnsignedByte();
    std::vector<int> variables;
    for (int i = 0; i < no; ++i) {
        variables.push_back(myInputStorage.readUnsignedByte());
    }
    // check subscribe/unsubscribe
    if (variables.size() == 0) {
        removeSubscription(commandId, id, -1);
        return true;
    }
    // process subscription
    Subscription s(commandId, id, variables, beginTime, endTime, false, 0, 0);
    initialiseSubscription(s);
    return true;
}


bool
TraCIServer::addObjectContextSubscription(int commandId) {
    SUMOTime beginTime = myInputStorage.readInt();
    SUMOTime endTime = myInputStorage.readInt();
    std::string id = myInputStorage.readString();
    int domain = myInputStorage.readUnsignedByte();
    SUMOReal range = myInputStorage.readDouble();
    int no = myInputStorage.readUnsignedByte();
    std::vector<int> variables;
    for (int i = 0; i < no; ++i) {
        variables.push_back(myInputStorage.readUnsignedByte());
    }
    // check subscribe/unsubscribe
    if (variables.size() == 0) {
        removeSubscription(commandId, id, -1);
        return true;
    }
    Subscription s(commandId, id, variables, beginTime, endTime, true, domain, range);
    initialiseSubscription(s);
    return true;
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


bool
TraCIServer::readTypeCheckingInt(tcpip::Storage& inputStorage, int& into) {
    if (inputStorage.readUnsignedByte() != TYPE_INTEGER) {
        return false;
    }
    into = inputStorage.readInt();
    return true;
}


bool
TraCIServer::readTypeCheckingDouble(tcpip::Storage& inputStorage, double& into) {
    if (inputStorage.readUnsignedByte() != TYPE_DOUBLE) {
        return false;
    }
    into = inputStorage.readDouble();
    return true;
}


bool
TraCIServer::readTypeCheckingString(tcpip::Storage& inputStorage, std::string& into) {
    if (inputStorage.readUnsignedByte() != TYPE_STRING) {
        return false;
    }
    into = inputStorage.readString();
    return true;
}


bool
TraCIServer::readTypeCheckingStringList(tcpip::Storage& inputStorage, std::vector<std::string>& into) {
    if (inputStorage.readUnsignedByte() != TYPE_STRINGLIST) {
        return false;
    }
    into = inputStorage.readStringList();
    return true;
}


bool
TraCIServer::readTypeCheckingColor(tcpip::Storage& inputStorage, RGBColor& into) {
    if (inputStorage.readUnsignedByte() != TYPE_COLOR) {
        return false;
    }
    unsigned char r = static_cast<unsigned char>(inputStorage.readUnsignedByte());
    unsigned char g = static_cast<unsigned char>(inputStorage.readUnsignedByte());
    unsigned char b = static_cast<unsigned char>(inputStorage.readUnsignedByte());
    unsigned char a = static_cast<unsigned char>(inputStorage.readUnsignedByte());
    into.set(r, g, b, a);
    return true;
}


bool
TraCIServer::readTypeCheckingPosition2D(tcpip::Storage& inputStorage, Position& into) {
    if (inputStorage.readUnsignedByte() != POSITION_2D) {
        return false;
    }
    SUMOReal x = inputStorage.readDouble();
    SUMOReal y = inputStorage.readDouble();
    into.set(x, y, 0);
    return true;
}


bool
TraCIServer::readTypeCheckingBoundary(tcpip::Storage& inputStorage, Boundary& into) {
    if (inputStorage.readUnsignedByte() != TYPE_BOUNDINGBOX) {
        return false;
    }
    const SUMOReal xmin = inputStorage.readDouble();
    const SUMOReal ymin = inputStorage.readDouble();
    const SUMOReal xmax = inputStorage.readDouble();
    const SUMOReal ymax = inputStorage.readDouble();
    into.set(xmin, ymin, xmax, ymax);
    return true;
}


bool
TraCIServer::readTypeCheckingByte(tcpip::Storage& inputStorage, int& into) {
    if (inputStorage.readUnsignedByte() != TYPE_BYTE) {
        return false;
    }
    into = inputStorage.readByte();
    return true;
}


bool
TraCIServer::readTypeCheckingUnsignedByte(tcpip::Storage& inputStorage, int& into) {
    if (inputStorage.readUnsignedByte() != TYPE_UBYTE) {
        return false;
    }
    into = inputStorage.readUnsignedByte();
    return true;
}


bool
TraCIServer::readTypeCheckingPolygon(tcpip::Storage& inputStorage, PositionVector& into) {
    if (inputStorage.readUnsignedByte() != TYPE_POLYGON) {
        return false;
    }
    into.clear();
    unsigned int noEntries = inputStorage.readUnsignedByte();
    PositionVector shape;
    for (unsigned int i = 0; i < noEntries; ++i) {
        SUMOReal x = inputStorage.readDouble();
        SUMOReal y = inputStorage.readDouble();
        into.push_back(Position(x, y));
    }
    return true;
}

#endif
