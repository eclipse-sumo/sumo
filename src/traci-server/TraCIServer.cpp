/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
/// @version $Id$
///
// TraCI server used to control sumo by a remote TraCI client (e.g., ns2)
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

#ifdef HAVE_PYTHON
#include <Python.h>
#endif

#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <foreign/tcpip/socket.h>
#include <foreign/tcpip/storage.h>
#include <utils/common/SUMOTime.h>
#include <utils/vehicle/DijkstraRouter.h>
#include <utils/common/NamedObjectCont.h>
#include <utils/common/RandHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSTransportableControl.h>
#include <microsim/MSJunction.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <libsumo/Simulation.h>
#include "TraCIConstants.h"
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


// ===========================================================================
// debug constants
// ===========================================================================
//#define DEBUG_MULTI_CLIENTS
//#define DEBUG_SUBSCRIPTIONS


// ===========================================================================
// static member definitions
// ===========================================================================
TraCIServer* TraCIServer::myInstance = 0;
bool TraCIServer::myDoCloseConnection = false;


// ===========================================================================
// method definitions
// ===========================================================================
TraCIServer::TraCIServer(const SUMOTime begin, const int port, const int numClients)
    : myServerSocket(0),
      myTargetTime(begin),
      myAmEmbedded(port == 0) {
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "Creating new TraCIServer for " << numClients << " clients on port " << port << "." << std::endl;
#endif
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
    myVehicleStateChanges[MSNet::VEHICLE_STATE_COLLISION] = std::vector<std::string>();

    myExecutors[CMD_GET_INDUCTIONLOOP_VARIABLE] = &TraCIServerAPI_InductionLoop::processGet;
    myExecutors[CMD_GET_LANEAREA_VARIABLE] = &TraCIServerAPI_LaneArea::processGet;
    myExecutors[CMD_GET_MULTIENTRYEXIT_VARIABLE] = &TraCIServerAPI_MultiEntryExit::processGet;

    myExecutors[CMD_GET_TL_VARIABLE] = &TraCIServerAPI_TrafficLight::processGet;
    myExecutors[CMD_SET_TL_VARIABLE] = &TraCIServerAPI_TrafficLight::processSet;
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
    myExecutors[CMD_GET_PERSON_VARIABLE] = &TraCIServerAPI_Person::processGet;
    myExecutors[CMD_SET_PERSON_VARIABLE] = &TraCIServerAPI_Person::processSet;

    myParameterSizes[VAR_LEADER] = 9;

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
            myServerSocket = new tcpip::Socket(port);
            while ((int)mySockets.size() < numClients) {
                int index = (int)mySockets.size() + MAX_ORDER + 1;
                mySockets[index] = new SocketInfo(myServerSocket->accept(true), begin);
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_BUILT] = std::vector<std::string>();
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_DEPARTED] = std::vector<std::string>();
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_STARTING_TELEPORT] = std::vector<std::string>();
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_ENDING_TELEPORT] = std::vector<std::string>();
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED] = std::vector<std::string>();
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_NEWROUTE] = std::vector<std::string>();
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_STARTING_PARKING] = std::vector<std::string>();
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_ENDING_PARKING] = std::vector<std::string>();
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_STARTING_STOP] = std::vector<std::string>();
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_ENDING_STOP] = std::vector<std::string>();
                mySockets[index]->vehicleStateChanges[MSNet::VEHICLE_STATE_COLLISION] = std::vector<std::string>();
            }
            // When got here, all clients have connected
            if (numClients > 1) {
                checkClientOrdering();
            }
            // set myCurrentSocket != mySockets.end() to indicate that this is the first step in processCommandsUntilSimStep()
            myCurrentSocket = mySockets.begin();
        } catch (tcpip::SocketException& e) {
            throw ProcessError(e.what());
        }
    }
}


TraCIServer::~TraCIServer() {
    for (myCurrentSocket = mySockets.begin(); myCurrentSocket != mySockets.end(); ++myCurrentSocket) {
        delete myCurrentSocket->second;
    }
    delete myServerSocket;
    cleanup();
}


// ---------- Initialisation and Shutdown
void
TraCIServer::openSocket(const std::map<int, CmdExecutor>& execs) {
    if (myInstance == 0 && !myDoCloseConnection && (OptionsCont::getOptions().getInt("remote-port") != 0
#ifdef HAVE_PYTHON
            || OptionsCont::getOptions().isSet("python-script")
#endif
                                                   )) {
        myInstance = new TraCIServer(string2time(OptionsCont::getOptions().getString("begin")),
                                     OptionsCont::getOptions().getInt("remote-port"),
                                     OptionsCont::getOptions().getInt("num-clients"));
        for (std::map<int, CmdExecutor>::const_iterator i = execs.begin(); i != execs.end(); ++i) {
            myInstance->myExecutors[i->first] = i->second;
        }
    }
    if (myInstance != 0) {
        // maybe net was deleted and built again
        MSNet::getInstance()->addVehicleStateListener(myInstance);
    }
}


void
TraCIServer::close() {
    if (myInstance == 0) {
        return;
    }
    delete myInstance;
    myInstance = 0;
    myDoCloseConnection = true;
}


bool
TraCIServer::wasClosed() {
    return myDoCloseConnection;
}


// ---------- Initialisation and Shutdown


void
TraCIServer::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to) {
    if (!myDoCloseConnection) {
        myVehicleStateChanges[to].push_back(vehicle->getID());
        if (!myAmEmbedded) {
            for (std::map<int, SocketInfo*>::iterator i = mySockets.begin(); i != mySockets.end(); ++i) {
                i->second->vehicleStateChanges[to].push_back(vehicle->getID());
            }
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
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4127) // do not warn about constant conditional expression
#endif
        while (true) {
#ifdef _MSC_VER
#pragma warning(pop)
#endif
            myInputStorage.reset();
            myCurrentSocket->second->socket->receiveExact(myInputStorage);
            int commandStart, commandLength;
            int commandId = readCommandID(commandStart, commandLength);
#ifdef DEBUG_MULTI_CLIENTS
            std::cout << "    received command " << commandId << std::endl;
#endif
            // Whether the received command is a permitted command for the initialization phase.
            // Currently, getVersion and setOrder are permitted.
            bool initCommand = commandId == CMD_SETORDER || commandId == CMD_GETVERSION;
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
                myInputStorage.writeUnsignedByte(commandLength);
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
                throw ProcessError("Execution order (CMD_SETORDER) was not set for all TraCI clients in pre-execution phase.");
            }
            if (commandId == CMD_SETORDER) {
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


void
TraCIServer::processCommandsUntilSimStep(SUMOTime step) {
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << SIMTIME << " processCommandsUntilSimStep(step = " << step << "):\n" << std::endl;
#endif
    try {
        const bool firstStep = myCurrentSocket != mySockets.end();
        // update client order if requested
        processReorderingRequests();
        if (!firstStep) {
            // This is the entry point after performing a SUMO step (block is skipped before first SUMO step since then no simulation results have to be sent)
            // update subscription results
            postProcessSimulationStep();
            // Send out subscription results to clients which will act in this SUMO step (i.e. with client target time <= current sumo timestep end)
            sendOutputToAll();
            myOutputStorage.reset();
        }

        // determine minimal next target time among clients
        myTargetTime = nextTargetTime();

        if (myAmEmbedded || step < myTargetTime) {
#ifdef DEBUG_MULTI_CLIENTS
            if (step < myTargetTime) {
                std::cout << "    next target time is larger than next SUMO simstep (" << step << "). Returning from processCommandsUntilSimStep()." << std::endl;
            }
#endif
            return;
        }

        // Simulation should run until
        // 1. end time reached or
        // 2. got CMD_CLOSE or
        // 3. got CMD_LOAD or
        // 4. Client closes socket connection
        while (!myDoCloseConnection && myTargetTime <= (MSNet::getInstance()->getCurrentTimeStep())) {
#ifdef DEBUG_MULTI_CLIENTS
            std::cout << "  Next target time: " << myTargetTime << std::endl;
#endif
            // Iterate over clients and process communication for the ones with target time == myTargetTime
            myCurrentSocket = mySockets.begin();
            while (myCurrentSocket != mySockets.end()) {
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "  current socket: " << myCurrentSocket->second->socket
                          << " with target time " << myCurrentSocket->second->targetTime
                          << std::endl;
#endif

                if (myCurrentSocket->second->targetTime > myTargetTime) {
                    // this client must wait
#ifdef DEBUG_MULTI_CLIENTS
                    std::cout <<  "       skipping client " << myCurrentSocket->second->socket
                              << " with target time " << myCurrentSocket->second->targetTime << std::endl;
#endif
                    myCurrentSocket++;
                    continue;
                }
                bool done = false;
                bool closed = false;
                bool load = false;
                while (!done && !closed && !load) {
                    if (!myInputStorage.valid_pos()) {
                        // have read request completely, send response if adequate
                        if (myOutputStorage.size() > 0) {
#ifdef DEBUG_MULTI_CLIENTS
                            std::cout << "    sending response..." << std::endl;
#endif
                            // send response to previous query
                            myCurrentSocket->second->socket->sendExact(myOutputStorage);
                            myOutputStorage.reset();
                        } else {
#ifdef DEBUG_MULTI_CLIENTS
                            std::cout << "    No input and no output stored (This is the next client)." << std::endl;
#endif
                        }
#ifdef DEBUG_MULTI_CLIENTS
                        std::cout << "    resetting input storage and reading next command..." << std::endl;
#endif
                        // Read next request
                        myInputStorage.reset();
                        myCurrentSocket->second->socket->receiveExact(myInputStorage);
                    }

                    while (myInputStorage.valid_pos() && !myDoCloseConnection) {
                        // dispatch command
                        const int cmd = dispatchCommand();
#ifdef DEBUG_MULTI_CLIENTS
                        std::cout << "    Received command " << cmd << std::endl;
#endif
                        if (cmd == CMD_SIMSTEP) {
#ifdef DEBUG_MULTI_CLIENTS
                            std::cout << "    Received command SIM_STEP, end turn for client " << myCurrentSocket->second->socket << std::endl;
#endif
                            done = true;
                        } else if (cmd == CMD_LOAD) {
#ifdef DEBUG_MULTI_CLIENTS
                            std::cout << "    Received command LOAD." << std::endl;
#endif
                            load = true;
                        } else if (cmd == CMD_CLOSE) {
#ifdef DEBUG_MULTI_CLIENTS
                            std::cout << "    Received command CLOSE." << std::endl;
#endif
                            closed = true;
                        }
                    }
                }
                if (done) {
                    // Clear vehicleStateChanges for this client -> For subsequent TraCI stepping
                    // that is performed within this SUMO step, no updates on vehicle states
                    // belonging to the last SUMO simulation step will be received by this client.
                    for (std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i = myCurrentSocket->second->vehicleStateChanges.begin(); i != myCurrentSocket->second->vehicleStateChanges.end(); ++i) {
                        (*i).second.clear();
                    }
                    myCurrentSocket++;
                } else if (load) {
                    myCurrentSocket = mySockets.end();
                } else {
                    assert(closed);
                    // remove current socket and increment to next socket in ordering
                    myCurrentSocket = removeCurrentSocket();
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
        // Reset myVehicleStateChanges
        for (std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i = myVehicleStateChanges.begin(); i != myVehicleStateChanges.end(); ++i) {
            (*i).second.clear();
        }

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
    }
    myOutputStorage.reset();
    myInputStorage.reset();
    mySubscriptionCache.reset();
    std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i;
    for (i = myVehicleStateChanges.begin(); i != myVehicleStateChanges.end(); i++) {
        i->second.clear();
    }
    myCurrentSocket = mySockets.begin();
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
        assert(myInstance != 0);
        myInstance->myInputStorage.reset();
        myInstance->myOutputStorage.reset();
        for (std::string::iterator i = cmd.begin(); i != cmd.end(); ++i) {
            myInstance->myInputStorage.writeChar(*i);
        }
        myInstance->dispatchCommand();
        return std::string(myInstance->myOutputStorage.begin(), myInstance->myOutputStorage.end());
    } catch (std::invalid_argument& e) {
        throw ProcessError(e.what());
    } catch (libsumo::TraCIException& e) {
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
        PyObject* sys_path, *path;
        char pathstr[] = "path";
        sys_path = PySys_GetObject(pathstr);
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


std::map<int, TraCIServer::SocketInfo*>::iterator
TraCIServer::removeCurrentSocket() {
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "       Removing socket " << myCurrentSocket->second->socket
              << " (order " << myCurrentSocket->first << ")" << std::endl;
#endif

    if (mySockets.size() == 1) {
        // Last client has disconnected
        delete myCurrentSocket->second->socket;
        mySockets.clear();
        myCurrentSocket = mySockets.end();
        return myCurrentSocket;
    }

    const int currOrder = myCurrentSocket->first;
    delete myCurrentSocket->second->socket;
    myCurrentSocket++;
    if (myCurrentSocket != mySockets.end()) {
        const int nextOrder = myCurrentSocket->first;
        mySockets.erase(currOrder);
        myCurrentSocket = mySockets.find(nextOrder);
    } else {
        mySockets.erase(currOrder);
        myCurrentSocket = mySockets.end();
    }
    return myCurrentSocket;
}


int
TraCIServer::readCommandID(int& commandStart, int& commandLength) {
    commandStart = myInputStorage.position();
    commandLength = myInputStorage.readUnsignedByte();
    if (commandLength == 0) {
        commandLength = myInputStorage.readInt();
    }
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
            case CMD_GETVERSION:
                success = commandGetVersion();
                break;
            case CMD_LOAD: {
                std::vector<std::string> args;
                if (!readTypeCheckingStringList(myInputStorage, args)) {
                    return writeErrorStatusCmd(CMD_LOAD, "A load command needs a list of string arguments.", myOutputStorage);
                }
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "       commandId == CMD_LOAD"
                          << ", args = " << toString(args) << std::endl;
#endif
                try {
                    myLoadArgs = args;
                    success = true;
                    writeStatusCmd(CMD_LOAD, RTYPE_OK, "");
                    // XXX: This only cares for the client that issued the load command.
                    // Multiclient-load functionality is still to be implemented. Refs #3146.
                    myCurrentSocket->second->socket->sendExact(myOutputStorage);
                    myOutputStorage.reset();
                } catch (libsumo::TraCIException& e) {
                    return writeErrorStatusCmd(CMD_LOAD, e.what(), myOutputStorage);
                }
                break;
            }
            case CMD_SIMSTEP: {
                SUMOTime nextT = myInputStorage.readInt();
                if (myAmEmbedded) {
                    if (nextT == 0) {
                        myTargetTime += DELTA_T;
                    } else {
                        myTargetTime = nextT;
                    }
                    for (std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i = myVehicleStateChanges.begin(); i != myVehicleStateChanges.end(); ++i) {
                        (*i).second.clear();
                    }
                    while (MSNet::getInstance()->getCurrentTimeStep() < myTargetTime) {
                        MSNet::getInstance()->simulationStep();
                    }
                    postProcessSimulationStep();
                } else {
                    if (nextT == 0) {
                        myCurrentSocket->second->targetTime += DELTA_T;
                    } else {
                        myCurrentSocket->second->targetTime = nextT;
                    }
#ifdef DEBUG_MULTI_CLIENTS
                    std::cout << "       commandId == CMD_SIMSTEP"
                              << ", next target time for client is " << myCurrentSocket->second->targetTime << std::endl;
#endif
                    if (myCurrentSocket->second->targetTime <= MSNet::getInstance()->getCurrentTimeStep()) {
                        // This is not the last TraCI simstep in the current SUMO simstep -> send single simstep response.
                        // @note: In the other case the simstep results are sent to all after the SUMO step was performed, see entry point for processCommandsUntilSimStep()
                        sendSingleSimStepResponse();
                    }
                }
                return commandId;
            }
            case CMD_CLOSE:
                writeStatusCmd(CMD_CLOSE, RTYPE_OK, "");
                myCurrentSocket->second->socket->sendExact(myOutputStorage);
                myOutputStorage.reset();
                if (mySockets.size() == 1) {
                    // Last client has closed connection
                    myDoCloseConnection = true;
                }
                success = true;
                break;
            case CMD_SETORDER: {
                const int order = myInputStorage.readInt();
#ifdef DEBUG_MULTI_CLIENTS
                std::cout << "       commandId == CMD_SETORDER"
                          << ", order index is " << order << std::endl;
#endif
                if (order > MAX_ORDER) {
                    return writeErrorStatusCmd(CMD_SETORDER, "A set order command needs an int argument below " + toString(MAX_ORDER) + ".", myOutputStorage);
                }
                if (mySockets.count(order) > 0 || mySocketReorderRequests.count(order) > 0) {
                    return writeErrorStatusCmd(CMD_SETORDER, "Order '" + toString(order) + "' is already taken.", myOutputStorage);
                }
                // memorize reorder request (will only take effect in the next step)
                mySocketReorderRequests[order] = myCurrentSocket->second;
                success = true;
                writeStatusCmd(CMD_SETORDER, RTYPE_OK, "");
                break;
            }
            case CMD_SUBSCRIBE_INDUCTIONLOOP_VARIABLE:
            case CMD_SUBSCRIBE_LANEAREA_VARIABLE:
            case CMD_SUBSCRIBE_MULTIENTRYEXIT_VARIABLE:
            case CMD_SUBSCRIBE_TL_VARIABLE:
            case CMD_SUBSCRIBE_LANE_VARIABLE:
            case CMD_SUBSCRIBE_VEHICLE_VARIABLE:
            case CMD_SUBSCRIBE_PERSON_VARIABLE:
            case CMD_SUBSCRIBE_VEHICLETYPE_VARIABLE:
            case CMD_SUBSCRIBE_ROUTE_VARIABLE:
            case CMD_SUBSCRIBE_POI_VARIABLE:
            case CMD_SUBSCRIBE_POLYGON_VARIABLE:
            case CMD_SUBSCRIBE_JUNCTION_VARIABLE:
            case CMD_SUBSCRIBE_EDGE_VARIABLE:
            case CMD_SUBSCRIBE_SIM_VARIABLE:
            case CMD_SUBSCRIBE_GUI_VARIABLE:
                success = addObjectVariableSubscription(commandId, false);
                break;
            case CMD_SUBSCRIBE_INDUCTIONLOOP_CONTEXT:
            case CMD_SUBSCRIBE_LANEAREA_CONTEXT:
            case CMD_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT:
            case CMD_SUBSCRIBE_TL_CONTEXT:
            case CMD_SUBSCRIBE_LANE_CONTEXT:
            case CMD_SUBSCRIBE_VEHICLE_CONTEXT:
            case CMD_SUBSCRIBE_PERSON_CONTEXT:
            case CMD_SUBSCRIBE_VEHICLETYPE_CONTEXT:
            case CMD_SUBSCRIBE_ROUTE_CONTEXT:
            case CMD_SUBSCRIBE_POI_CONTEXT:
            case CMD_SUBSCRIBE_POLYGON_CONTEXT:
            case CMD_SUBSCRIBE_JUNCTION_CONTEXT:
            case CMD_SUBSCRIBE_EDGE_CONTEXT:
            case CMD_SUBSCRIBE_SIM_CONTEXT:
            case CMD_SUBSCRIBE_GUI_CONTEXT:
                success = addObjectVariableSubscription(commandId, true);
                break;
            default:
                writeStatusCmd(commandId, RTYPE_NOTIMPLEMENTED, "Command not implemented in sumo");
        }
    }
    if (!success) {
        while (myInputStorage.valid_pos() && (int)myInputStorage.position() < commandStart + commandLength) {
            myInputStorage.readChar();
        }
    }
    if ((int)myInputStorage.position() != commandStart + commandLength) {
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


void
TraCIServer::postProcessSimulationStep() {
    SUMOTime t = MSNet::getInstance()->getCurrentTimeStep();
#ifdef DEBUG_MULTI_CLIENTS
    std::cout << "   postProcessSimulationStep() at time " << t << std::endl;
#endif
    writeStatusCmd(CMD_SIMSTEP, RTYPE_OK, "");
    int noActive = 0;
    for (std::vector<Subscription>::iterator i = mySubscriptions.begin(); i != mySubscriptions.end();) {
        const Subscription& s = *i;
        bool isArrivedVehicle = (s.commandId == CMD_SUBSCRIBE_VEHICLE_VARIABLE || s.commandId == CMD_SUBSCRIBE_VEHICLE_CONTEXT)
                                && (find(myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED].begin(), myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED].end(), s.id) != myVehicleStateChanges[MSNet::VEHICLE_STATE_ARRIVED].end());
        bool isArrivedPerson = (s.commandId == CMD_SUBSCRIBE_PERSON_VARIABLE || s.commandId == CMD_SUBSCRIBE_PERSON_CONTEXT) && MSNet::getInstance()->getPersonControl().get(s.id) == 0;
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
    for (std::vector<Subscription>::iterator i = mySubscriptions.begin(); i != mySubscriptions.end();) {
        const Subscription& s = *i;
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
    writeStatusCmd(CMD_SIMSTEP, RTYPE_OK, "");

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
    if (status == RTYPE_ERR) {
        WRITE_ERROR("Answered with error to command " + toHex(commandId, 2) + ": " + description);
    } else if (status == RTYPE_NOTIMPLEMENTED) {
        WRITE_ERROR("Requested command not implemented (" + toHex(commandId, 2) + "): " + description);
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
            bool needNewSubscription = true;
            for (std::vector<Subscription>::iterator i = mySubscriptions.begin(); i != mySubscriptions.end(); ++i) {
                if (s.commandId == i->commandId && s.id == i->id &&
                        s.beginTime == i->beginTime && s.endTime == i->endTime &&
                        s.contextVars == i->contextVars && s.contextDomain == i->contextDomain && s.range == i->range) {
                    std::vector<std::vector<unsigned char> >::const_iterator k = s.parameters.begin();
                    for (std::vector<int>::const_iterator j = s.variables.begin(); j != s.variables.end(); ++j, ++k) {
                        const int offset = (int)(std::find(i->variables.begin(), i->variables.end(), *j) - i->variables.begin());
                        if (offset == (int)i->variables.size() || i->parameters[offset] != *k) {
                            i->variables.push_back(*j);
                            i->parameters.push_back(*k);
                        }
                    }
                    needNewSubscription = false;
                    break;
                }
            }
            if (needNewSubscription) {
                mySubscriptions.push_back(s);
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
        case CMD_SUBSCRIBE_MULTIENTRYEXIT_CONTEXT:
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
        case CMD_SUBSCRIBE_PERSON_CONTEXT:
            if (TraCIServerAPI_Person::getPosition(id, p)) {
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
        libsumo::Helper::collectObjectsInRange(s.contextDomain, shape, s.range, objIDs);
    } else {
        objIDs.insert(s.id);
    }
    const int numVars = s.contextVars && s.variables.size() == 1 && s.variables[0] == ID_LIST ? 0 : (int)s.variables.size();
    for (std::set<std::string>::iterator j = objIDs.begin(); j != objIDs.end(); ++j) {
        if (s.contextVars) {
            outputStorage.writeString(*j);
        }
        if (numVars > 0) {
            std::vector<std::vector<unsigned char> >::const_iterator k = s.parameters.begin();
            for (std::vector<int>::const_iterator i = s.variables.begin(); i != s.variables.end(); ++i, ++k) {
                tcpip::Storage message;
                message.writeUnsignedByte(*i);
                message.writeString(*j);
                message.writePacket(*k);
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
    int length = (1 + 4) + 1 + (4 + (int)(s.id.length())) + 1 + (int)outputStorage.size();
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
TraCIServer::addObjectVariableSubscription(const int commandId, const bool hasContext) {
    const SUMOTime beginTime = myInputStorage.readInt();
    const SUMOTime endTime = myInputStorage.readInt();
    const std::string id = myInputStorage.readString();
    const int domain = hasContext ? myInputStorage.readUnsignedByte() : 0;
    const double range = hasContext ? myInputStorage.readDouble() : 0.;
    const int num = myInputStorage.readUnsignedByte();
    std::vector<int> variables;
    std::vector<std::vector<unsigned char> > parameters;
    for (int i = 0; i < num; ++i) {
        const int varID = myInputStorage.readUnsignedByte();
        variables.push_back(varID);
        parameters.push_back(std::vector<unsigned char>());
        for (int j = 0; j < myParameterSizes[varID]; j++) {
            parameters.back().push_back(myInputStorage.readChar());
        }
    }
    // check subscribe/unsubscribe
    if (variables.size() == 0) {
        removeSubscription(commandId, id, -1);
        return true;
    }
    // process subscription
    Subscription s(commandId, id, variables, parameters, beginTime, endTime, hasContext, domain, range);
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
TraCIServer::readTypeCheckingColor(tcpip::Storage& inputStorage, libsumo::TraCIColor& into) {
    if (inputStorage.readUnsignedByte() != TYPE_COLOR) {
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
    if (inputStorage.readUnsignedByte() != POSITION_2D) {
        return false;
    }
    into.x = inputStorage.readDouble();
    into.y = inputStorage.readDouble();
    into.z = 0;
    return true;
}


bool
TraCIServer::readTypeCheckingBoundary(tcpip::Storage& inputStorage, Boundary& into) {
    if (inputStorage.readUnsignedByte() != TYPE_BOUNDINGBOX) {
        return false;
    }
    const double xmin = inputStorage.readDouble();
    const double ymin = inputStorage.readDouble();
    const double xmax = inputStorage.readDouble();
    const double ymax = inputStorage.readDouble();
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
    int noEntries = inputStorage.readUnsignedByte();
    PositionVector shape;
    for (int i = 0; i < noEntries; ++i) {
        double x = inputStorage.readDouble();
        double y = inputStorage.readDouble();
        into.push_back(Position(x, y));
    }
    return true;
}


void
TraCIServer::setTargetTime(SUMOTime targetTime) {
    myTargetTime = targetTime;
    for (auto& s : mySockets) {
        s.second->targetTime = targetTime;
    }
}


/****************************************************************************/
