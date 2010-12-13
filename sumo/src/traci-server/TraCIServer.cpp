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
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include "TraCIHandler.h"
#include "TraCIDijkstraRouter.h"

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
using namespace std;
using namespace tcpip;


namespace traci {

// ===========================================================================
// static member definitions
// ===========================================================================
TraCIServer* TraCIServer::instance_ = 0;
bool TraCIServer::closeConnection_ = false;


// ===========================================================================
// method definitions
// ===========================================================================

void
TraCIServer::jt(const std::map<int, CmdExecutor> &execs) throw() {
    if (instance_ == 0) {
        if (!closeConnection_ && OptionsCont::getOptions().getInt("remote-port") != 0) {
            instance_ = new traci::TraCIServer();
            for (std::map<int, CmdExecutor>::const_iterator i=execs.begin(); i!=execs.end(); ++i) {
                instance_->myExecutors[i->first] = i->second;
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

    myExecutors[CMD_SETMAXSPEED] = &TraCIServerAPI_Vehicle::commandSetMaximumSpeed;
    myExecutors[CMD_STOP] = &TraCIServerAPI_Vehicle::commandStopNode;
    myExecutors[CMD_CHANGELANE] = &TraCIServerAPI_Vehicle::commandChangeLane;
    myExecutors[CMD_CHANGEROUTE] = &TraCIServerAPI_Vehicle::commandChangeRoute;
    myExecutors[CMD_CHANGETARGET] = &TraCIServerAPI_Vehicle::commandChangeTarget;
    myExecutors[CMD_GETALLTLIDS] = &TraCIServerAPI_TLS::commandGetAllTLIds;
    myExecutors[CMD_GETTLSTATUS] = &TraCIServerAPI_TLS::commandGetTLStatus;
    myExecutors[CMD_SLOWDOWN] = &TraCIServerAPI_Vehicle::commandSlowDown;
    myExecutors[CMD_SUBSCRIBEDOMAIN] = &TraCIServerAPI_Vehicle::commandSubscribeDomain;
    myExecutors[CMD_UNSUBSCRIBEDOMAIN] = &TraCIServerAPI_Vehicle::commandUnsubscribeDomain;

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
    targetTime_ = 0;
    penetration_ = (float)oc.getFloat("penetration");
    routeFile_ = oc.getString("route-files");
    isMapChanged_ = true;
    numEquippedVehicles_ = 0;
    totalNumVehicles_ = 0;
    closeConnection_ = false;
    netBoundary_ = NULL;
    myDoingSimStep = false;
    myHaveWarnedDeprecation = false;

    // display warning if internal lanes are not used
    if (!MSGlobals::gUsingInternalLanes) {
        MsgHandler::getWarningInstance()->inform("Starting TraCI without using internal lanes!");
        MsgHandler::getWarningInstance()->inform("Vehicles will jump over junctions.", false);
        MsgHandler::getWarningInstance()->inform("Use without option --no-internal-links to avoid unexpected behavior", false);
    }

    // map the internal id of all traffic lights, polygons and poi to external id and vice versa
    trafficLightsInt2ExtId.clear();
    trafficLightsExt2IntId.clear();
    poiExt2IntId.clear();
    poiInt2ExtId.clear();
    polygonExt2IntId.clear();
    polygonInt2ExtId.clear();
    int extId = 0;
    std::vector<std::string> tllIds = MSNet::getInstance()->getTLSControl().getAllTLIds();
    for (std::vector<std::string>::iterator it=tllIds.begin(); it != tllIds.end(); it++) {
        trafficLightsInt2ExtId[(*it)] = extId;
        trafficLightsExt2IntId[extId] = (*it);
//		cerr << "TL int=" << *it << " --> ext=" << extId << endl;
        extId++;
    }
    int poiId = 0;
    int polyId = 0;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer(); i++) {
        std::vector<PointOfInterest*> poiList = shapeCont.getPOICont(i).getTempVector();
        for (std::vector<PointOfInterest*>::iterator it=poiList.begin(); it != poiList.end(); it++) {
            poiInt2ExtId[(*it)->getID()] = poiId;
            poiExt2IntId[poiId] = (*it)->getID();
            poiId++;
        }
        std::vector<Polygon2D*> polyList = shapeCont.getPolygonCont(i).getTempVector();
        for (std::vector<Polygon2D*>::iterator it=polyList.begin(); it != polyList.end(); it++) {
            polygonInt2ExtId[(*it)->getID()] = polyId;
            polygonExt2IntId[polyId] = (*it)->getID();
            polyId++;
        }
    }

    // determine the maximum number of vehicles by searching route and additional input files for "vehicle" tags
    TraCIHandler xmlHandler;
    SAX2XMLReader* xmlParser = XMLSubSys::getSAXReader(xmlHandler);

    if (OptionsCont::getOptions().getFloat("penetration")!=0) {
        // parse route files
        if (oc.isSet("route-files")) {
            std::vector<std::string> fileList = oc.getStringVector("route-files");
            for (std::vector<std::string>::iterator file=fileList.begin(); file != fileList.end(); file++) {
                if (oc.isUsableFileList("route-files")) {
                    xmlHandler.setFileName((*file));
                    xmlHandler.resetTotalVehicleCount();
                    xmlParser->parse(file->c_str());
                    if (!MsgHandler::getErrorInstance()->wasInformed()) {
                        totalNumVehicles_ += xmlHandler.getTotalVehicleCount();
                    }
                }
            }
        }

        // parse additional files
        if (oc.isSet("additional-files")) {
            std::vector<std::string> fileList = oc.getStringVector("additional-files");
            for (std::vector<std::string>::iterator file = fileList.begin(); file != fileList.end(); file++) {
                if (oc.isUsableFileList("additional-files")) {
                    xmlHandler.setFileName((*file));
                    xmlHandler.resetTotalVehicleCount();
                    xmlParser->parse(file->c_str());
                    if (!MsgHandler::getErrorInstance()->wasInformed()) {
                        totalNumVehicles_ += xmlHandler.getTotalVehicleCount();
                    }
                }
            }
        }
    }

    delete xmlParser;

    try {
        int port = oc.getInt("remote-port");
        // Opens listening socket
        MsgHandler::getMessageInstance()->inform("***Starting server on port " + toString(port) + " ***");
        socket_ = new Socket(port);
        socket_->accept();
        // When got here, a client has connected
    } catch (SocketException &e) {
        throw ProcessError(e.what());
    }

}

/*****************************************************************************/

TraCIServer::~TraCIServer() throw() {
    MSNet::getInstance()->removeVehicleStateListener(this);
    if (socket_ != NULL) {
        socket_->close();
        delete socket_;
    }
    if (netBoundary_ != NULL) delete netBoundary_;
}

/*****************************************************************************/

void
TraCIServer::close() {
    if (instance_!=0) {
        delete instance_;
        instance_ = 0;
        closeConnection_ = true;
    }
}

/*****************************************************************************/

void
TraCIServer::vehicleStateChanged(const SUMOVehicle * const vehicle, MSNet::VehicleState to) throw() {
    if (closeConnection_ || OptionsCont::getOptions().getInt("remote-port") == 0) {
        return;
    }
    myVehicleStateChanges[to].push_back(vehicle->getID());
}

/*****************************************************************************/
void
TraCIServer::processCommandsUntilSimStep(SUMOTime step) throw(ProcessError) {
    try {
        if (instance_ == 0) {
            if (!closeConnection_ && OptionsCont::getOptions().getInt("remote-port") != 0) {
                instance_ = new traci::TraCIServer();
            } else {
                return;
            }
        }
        if (step < instance_->targetTime_) {
            return;
        }
        // Simulation should run until
        // 1. end time reached or
        // 2. got CMD_CLOSE or
        // 3. Client closes socket connection
        if (instance_->myDoingSimStep) {
            switch (instance_->simStepCommand) {
            case CMD_SIMSTEP:
                instance_->postProcessSimulationStep();
                break;
            case CMD_SIMSTEP2:
                instance_->postProcessSimulationStep2();
                break;
            }
            instance_->myDoingSimStep = false;
        }
        while (!closeConnection_) {
            if (!instance_->myInputStorage.valid_pos()) {
                if (instance_->myOutputStorage.size() > 0) {
                    // send out all answers as one storage
                    instance_->socket_->sendExact(instance_->myOutputStorage);
                }
                instance_->myInputStorage.reset();
                instance_->myOutputStorage.reset();
                // Read a message
                instance_->socket_->receiveExact(instance_->myInputStorage);
            }
            while (instance_->myInputStorage.valid_pos() && !closeConnection_) {
                // dispatch each command
                int cmd = instance_->dispatchCommand();
                if (cmd == CMD_SIMSTEP || cmd==CMD_SIMSTEP2) {
                    instance_->simStepCommand = cmd;
                    instance_->myDoingSimStep = true;
                    for (std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i=instance_->myVehicleStateChanges.begin(); i!=instance_->myVehicleStateChanges.end(); ++i) {
                        (*i).second.clear();
                    }
                    return;
                }
            }
        }
        if (closeConnection_ && instance_->myOutputStorage.size() > 0) {
            // send out all answers as one storage
            instance_->socket_->sendExact(instance_->myOutputStorage);
        }
        for (std::map<MSNet::VehicleState, std::vector<std::string> >::iterator i=instance_->myVehicleStateChanges.begin(); i!=instance_->myVehicleStateChanges.end(); ++i) {
            (*i).second.clear();
        }
    } catch (std::invalid_argument &e) {
        throw ProcessError(e.what());
    } catch (TraCIException &e) {
        throw ProcessError(e.what());
    } catch (SocketException &e) {
        throw ProcessError(e.what());
    }
    if (instance_ != NULL) {
        delete instance_;
        instance_ = 0;
        closeConnection_ = true;
    }
}

bool
TraCIServer::wasClosed() {
    return closeConnection_;
}

/*****************************************************************************/

int
TraCIServer::dispatchCommand()
throw(TraCIException, std::invalid_argument) {
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
        case CMD_SIMSTEP:
            targetTime_ = static_cast<SUMOTime>(myInputStorage.readInt());
            success = true;
            if (!myHaveWarnedDeprecation) {
                MsgHandler::getWarningInstance()->inform("Using old TraCI API, please update your client!");
                myHaveWarnedDeprecation = true;
            }
            return commandId;
        case CMD_SIMSTEP2: {
            SUMOTime nextT = myInputStorage.readInt();
            success = true;
            if (nextT!=0) {
                targetTime_ = nextT;
            } else {
                targetTime_ += DELTA_T;
            }
            return commandId;
        }
        case CMD_CLOSE:
            success = commandCloseConnection();
            break;
        case CMD_UPDATECALIBRATOR:
            success = commandUpdateCalibrator();
            break;
        case CMD_POSITIONCONVERSION:
            success = commandPositionConversion();
            break;
        case CMD_SCENARIO:
            success = commandScenario();
            break;
        case CMD_ADDVEHICLE:
            success = commandAddVehicle();
            break;
        case CMD_DISTANCEREQUEST:
            success = commandDistanceRequest();
            break;
        case CMD_SUBSCRIBELIFECYCLES:
            success = TraCIServerAPI_Vehicle::commandSubscribeLifecycles(*this, myInputStorage, myOutputStorage);
            myCreatedVehicles.clear();
            myDestroyedVehicles.clear();
            break;
        case CMD_UNSUBSCRIBELIFECYCLES:
            success = TraCIServerAPI_Vehicle::commandUnsubscribeLifecycles(*this, myInputStorage, myOutputStorage);
            myCreatedVehicles.clear();
            myDestroyedVehicles.clear();
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
        ostringstream msg;
        msg << "Wrong position in requestMessage after dispatching command.";
        msg << " Expected command length was " << commandLength;
        msg << " but " << myInputStorage.position() - commandStart << " Bytes were read.";
        writeStatusCmd(commandId, RTYPE_ERR, msg.str());
        closeConnection_ = true;
    }
    return commandId;
}

/*****************************************************************************/

void
TraCIServer::postProcessSimulationStep() throw(TraCIException, std::invalid_argument) {
    // Position representation
    int resType = myInputStorage.readUnsignedByte();
    if (resType != POSITION_NONE && resType != POSITION_2D && resType != POSITION_ROADMAP
            && resType != POSITION_2_5D && resType != POSITION_3D) {
        writeStatusCmd(CMD_SIMSTEP, RTYPE_ERR, "Error: unsupported return format requested.");
        return;
    }
    isMapChanged_ = true;
    // Everything is fine
    writeStatusCmd(CMD_SIMSTEP, RTYPE_OK, "");

    // prepare output
    try {
        MSNet *net = MSNet::getInstance();
        // map containing all active equipped vehicles. maps external id to MSVehicle*
        map<int, const SUMOVehicle*> activeEquippedVehicles;
        // get access to all vehicles in simulation
        MSVehicleControl &vehControl = net->getVehicleControl();
        // iterate over all vehicles in simulation
        for (MSVehicleControl::constVehIt iter = vehControl.loadedVehBegin();
                iter != vehControl.loadedVehEnd(); ++iter) {
            // selected vehicle
            const std::string vehicleId   = (*iter).first;
            const SUMOVehicle *vehicle = (*iter).second;
            // insert into equippedVehicleId if not contained
            std::map<std::string, int>::const_iterator equippedVeh = equippedVehicles_.find(vehicleId);
            if (equippedVeh == equippedVehicles_.end()) {
                // determine if vehicle is equipped
                if (penetration_ >= 1. || RandHelper::rand() <= penetration_) {
                    // vehicle is equipped
                    equippedVehicles_[vehicleId] = numEquippedVehicles_;
                    // put into active list?
                    if (vehicle->isOnRoad()) {
                        if ((myLifecycleSubscriptions.count(DOM_VEHICLE) != 0) &&
                                (myLivingVehicles.count(numEquippedVehicles_) == 0)) {
                            myCreatedVehicles.insert(numEquippedVehicles_);
                            myLivingVehicles.insert(numEquippedVehicles_);
                        }
                        activeEquippedVehicles[numEquippedVehicles_] = vehicle;
                    }
                    numEquippedVehicles_++;
                } else {
                    // vehicle is not equipped
                    equippedVehicles_[vehicleId] = -1;
                }
            } else if (equippedVeh->second >= 0 && vehicle->isOnRoad()) {
                int extId = equippedVeh->second;
                if ((myLifecycleSubscriptions.count(DOM_VEHICLE) != 0) &&
                        (myLivingVehicles.count(extId) == 0)) {
                    myCreatedVehicles.insert(extId);
                    myLivingVehicles.insert(extId);
                }
                activeEquippedVehicles[extId] = vehicle;
                // vehicle is equipped
            }
        }
        if (myLifecycleSubscriptions.count(DOM_VEHICLE) != 0) {
            // iterate over all vehicles that are supposed to live
            for (std::set<int>::iterator i = myLivingVehicles.begin();
                    i != myLivingVehicles.end();) {
                int extId = *i;
                if (activeEquippedVehicles.find(extId) == activeEquippedVehicles.end()) {
                    myDestroyedVehicles.insert(extId);
                    myLivingVehicles.erase(i++);
                } else {
                    i++;
                }
            }
        }

        handleLifecycleSubscriptions();

        // for each vehicle process any active traci command
        for (std::map<std::string, int>::iterator iter = equippedVehicles_.begin();
                iter != equippedVehicles_.end(); ++iter) {
            if ((*iter).second != -1) { // Look only at equipped vehicles
                MSVehicle* veh = static_cast<MSVehicle*>(net->getVehicleControl().getVehicle((*iter).first));
                TraCIServerAPI_Vehicle::checkReroute(veh);
                if (veh != NULL) {
                    veh->processTraCICommands(targetTime_);
                }
            }
        }

        handleDomainSubscriptions(targetTime_, activeEquippedVehicles);

        //out.writeChar( static_cast<unsigned char>(rtype) );
        //out.writeInt(numEquippedVehicles_);
        // iterate over all active equipped vehicles
        // and generate a Move Node command for each vehicle
        if (resType != POSITION_NONE) {
            for (map<int, const SUMOVehicle*>::iterator iter = activeEquippedVehicles.begin();
                    iter != activeEquippedVehicles.end(); ++iter) {
                int extId = (*iter).first;
                const MSVehicle* vehicle = static_cast<const MSVehicle*>((*iter).second);
                Storage tempMsg;

                // command type
                tempMsg.writeUnsignedByte(CMD_MOVENODE);
                // node id
                tempMsg.writeInt(extId);
                // end time
                tempMsg.writeInt(targetTime_);

                if (resType == POSITION_ROADMAP) {
                    // return type
                    tempMsg.writeUnsignedByte(POSITION_ROADMAP);

                    tempMsg.writeString(vehicle->getEdge()->getID());
                    tempMsg.writeFloat((float)vehicle->getPositionOnLane());

                    // determine index of the lane the vehicle is on
                    int laneId = 0;
                    const MSLane* lane = &vehicle->getLane();
                    while ((lane = lane->getRightLane()) != NULL) {
                        laneId++;
                    }
                    tempMsg.writeUnsignedByte(laneId);
                } else if (resType == POSITION_2D || resType == POSITION_3D || resType == POSITION_2_5D) {
                    tempMsg.writeUnsignedByte(resType);
                    Position2D pos = vehicle->getLane().getShape().positionAtLengthPosition(vehicle->getPositionOnLane());
                    tempMsg.writeFloat((float)pos.x());
                    tempMsg.writeFloat((float)pos.y());
                    if (resType != POSITION_2D) {
                        // z pos: ignored
                        tempMsg.writeFloat(0);
                    }
                }

                // command length
                myOutputStorage.writeUnsignedByte((int)tempMsg.size()+1);
                // content
                myOutputStorage.writeStorage(tempMsg);
            }
        }
    } catch (...) {
        writeStatusCmd(CMD_SIMSTEP, RTYPE_ERR, "some error happen in command: simulation step. Sumo shuts down.");
        closeConnection_ = true;
    }
}


void
TraCIServer::postProcessSimulationStep2() throw(TraCIException, std::invalid_argument) {
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



/*****************************************************************************/

bool
TraCIServer::commandGetVersion() throw(TraCIException) {

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
TraCIServer::commandCloseConnection() throw(TraCIException) {
    closeConnection_ = true;
    // write answer
    writeStatusCmd(CMD_CLOSE, RTYPE_OK, "Goodbye");
    return true;
}

/*****************************************************************************/

bool
TraCIServer::commandSimulationParameter() throw(TraCIException) {
    bool setParameter = (myInputStorage.readByte() != 0);
    std::string parameter = myInputStorage.readString();

    // Prepare response
    tcpip::Storage answerTmp;

    if (parameter.compare("maxX")) {
        if (setParameter) {
            writeStatusCmd(CMD_SIMPARAMETER, RTYPE_ERR, "maxX is a read only parameter");
            return false;
        } else {
            answerTmp.writeFloat((float)(getNetBoundary().getWidth()));
        }
    } else if (parameter.compare("maxY")) {
        if (setParameter) {
            writeStatusCmd(CMD_SIMPARAMETER, RTYPE_ERR, "maxY is a read only parameter");
            return false;
        } else {
            answerTmp.writeFloat((float)(getNetBoundary().getHeight()));
        }
    } else if (parameter.compare("numberOfNodes")) {
        if (setParameter) {
            writeStatusCmd(CMD_SIMPARAMETER, RTYPE_ERR, "numberOfNodes is a read only parameter");
            return false;
        } else {
            writeStatusCmd(CMD_SIMPARAMETER, RTYPE_NOTIMPLEMENTED, "numberOfNodes not implemented yet");
            return false;
            //answerTmp.writeInt( --- Don't know where to get that information ---);
        }
    } else if (parameter.compare("airDistance")) {
        MSVehicle* veh1 = getVehicleByExtId(myInputStorage.readInt());   // external node id (equipped vehicle number)
        MSVehicle* veh2 = getVehicleByExtId(myInputStorage.readInt());   // external node id (equipped vehicle number)

        if (veh1 != NULL && veh2 != NULL) {
            if (setParameter) {
                writeStatusCmd(CMD_SIMPARAMETER, RTYPE_ERR, "airDistance is a read only parameter");
                return false;
            } else {
                float dx = (float)(veh1->getPosition().x() - veh2->getPosition().x());
                float dy = (float)(veh1->getPosition().y() - veh2->getPosition().y());
                answerTmp.writeFloat(sqrt(dx * dx + dy * dy));
            }
        } else {
            writeStatusCmd(CMD_SIMPARAMETER, RTYPE_ERR, "Can not retrieve node with given ID");
            return false;
        }
    } else if (parameter.compare("drivingDistance")) {
        MSVehicle* veh1 = getVehicleByExtId(myInputStorage.readInt());   // external node id (equipped vehicle number)
        MSVehicle* veh2 = getVehicleByExtId(myInputStorage.readInt());   // external node id (equipped vehicle number)

        if (veh1 != NULL && veh2 != NULL) {
            if (setParameter) {
                writeStatusCmd(CMD_SIMPARAMETER, RTYPE_ERR, "airDistance is a read only parameter");
                return false;
            } else {
                writeStatusCmd(CMD_SIMPARAMETER, RTYPE_NOTIMPLEMENTED, "drivingDistance not implemented yet");
                return false;
                //float dx = veh1->getPosition().x() - veh2->getPosition().x();
                //float dy = veh1->getPosition().y() - veh2->getPosition().y();
                //float distance = sqrt( dx * dx + dy * dy );
                // answerTmp.writeFloat( distance );
            }
        } else {
            writeStatusCmd(CMD_SIMPARAMETER, RTYPE_ERR, "Can not retrieve node with given ID");
            return false;
        }
    }

    // When we get here, the response is stored in answerTmp -> put into myOutputStorage
    writeStatusCmd(CMD_SIMPARAMETER, RTYPE_OK, "");

    // command length
    myOutputStorage.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(parameter.length()) + (int)answerTmp.size());
    // command type
    myOutputStorage.writeUnsignedByte(CMD_SIMPARAMETER);
    // answer only to getParameter commands as setParameter
    myOutputStorage.writeUnsignedByte(1);
    // Parameter
    myOutputStorage.writeString(parameter);
    // and the parameter dependant part
    myOutputStorage.writeStorage(answerTmp);
    return true;
}

/*****************************************************************************/

bool
TraCIServer::commandUpdateCalibrator() throw(TraCIException) {
    myOutputStorage.reset();

    int countTime = myInputStorage.readInt();
    int vehicleCount = myInputStorage.readInt();
    std::string calibratorId = myInputStorage.readString();

    MSCalibrator::updateCalibrator(calibratorId, countTime, vehicleCount);

    //@TODO write response according to result of updateCalibrator

    return true;
}

/*****************************************************************************/

bool
TraCIServer::commandPositionConversion() throw(TraCIException) {
    tcpip::Storage tmpResult;
    RoadMapPos roadPos;
    Position2D cartesianPos;
    float x = 0;
    float y = 0;
    float z = 0;
    unsigned char destPosType;

    // actual position type that will be converted
    unsigned char srcPosType = (unsigned char)myInputStorage.readUnsignedByte();

    switch (srcPosType) {
    case POSITION_2D:
    case POSITION_2_5D:
    case POSITION_3D:
        x = myInputStorage.readFloat();
        y = myInputStorage.readFloat();
        if (srcPosType != POSITION_2D) {
            z = myInputStorage.readFloat();
        }
        // destination position type
        destPosType = (unsigned char) myInputStorage.readUnsignedByte();

        switch (destPosType) {
        case POSITION_ROADMAP:
            // convert road map to 3D position
            roadPos = convertCartesianToRoadMap(Position2D(x, y));

            // write result that is added to response msg
            tmpResult.writeUnsignedByte(POSITION_ROADMAP);
            tmpResult.writeString(roadPos.roadId);
            tmpResult.writeFloat(roadPos.pos);
            tmpResult.writeUnsignedByte(roadPos.laneId);
            break;
        case POSITION_3D:
            writeStatusCmd(CMD_POSITIONCONVERSION, RTYPE_ERR,
                           "Destination position type is same as source position type");
            return false;
        default:
            writeStatusCmd(CMD_POSITIONCONVERSION, RTYPE_ERR,
                           "Destination position type not supported");
            return false;
        }
        break;
    case POSITION_ROADMAP:
        roadPos.roadId = myInputStorage.readString();
        roadPos.pos = myInputStorage.readFloat();
        roadPos.laneId = (unsigned char)myInputStorage.readUnsignedByte();

        // destination position type
        destPosType = (unsigned char)myInputStorage.readUnsignedByte();

        switch (destPosType) {
        case POSITION_2D:
        case POSITION_2_5D:
        case POSITION_3D:
            //convert 3D to road map position
            try {
                Position2D result = convertRoadMapToCartesian(roadPos);
                x = (float)result.x();
                y = (float)result.y();
            } catch (TraCIException &e) {
                writeStatusCmd(CMD_POSITIONCONVERSION, RTYPE_ERR, e.what());
                return false;
            }

            // write result that is added to response msg
            tmpResult.writeUnsignedByte(destPosType);
            tmpResult.writeFloat(x);
            tmpResult.writeFloat(y);
            if (destPosType != POSITION_2D) {
                tmpResult.writeFloat(z);
            }
            break;
        case POSITION_ROADMAP:
            writeStatusCmd(CMD_POSITIONCONVERSION, RTYPE_ERR,
                           "Destination position type is same as source position type");
            return false;
        default:
            writeStatusCmd(CMD_POSITIONCONVERSION, RTYPE_ERR,
                           "Destination position type not supported");
            return false;
        }
        break;
    default:
        writeStatusCmd(CMD_POSITIONCONVERSION, RTYPE_ERR,
                       "Source position type not supported");
        return false;
    }

    // write response message
    writeStatusCmd(CMD_POSITIONCONVERSION, RTYPE_OK, "");
    // add converted Position to response
    myOutputStorage.writeUnsignedByte(1 + 1 + (int)tmpResult.size() + 1);	// length
    myOutputStorage.writeUnsignedByte(CMD_POSITIONCONVERSION);	// command id
    myOutputStorage.writeStorage(tmpResult);	// position dependant part
    myOutputStorage.writeUnsignedByte(destPosType);	// destination type
    return true;
}

/*****************************************************************************/

bool
TraCIServer::commandScenario() throw(TraCIException) {
    Storage tmpResult;
    std::string warning = "";	// additional description for response

    // read/write flag
    bool isWriteCommand = static_cast<bool>(myInputStorage.readUnsignedByte());

    // domain
    int domain = myInputStorage.readUnsignedByte();

    try {
        switch (domain) {
        case DOM_ROADMAP:
            warning = handleRoadMapDomain(isWriteCommand, tmpResult);
            break;
        case DOM_VEHICLE:
            warning = handleVehicleDomain(isWriteCommand, tmpResult);
            break;
        case DOM_TRAFFICLIGHTS:
            warning = handleTrafficLightDomain(isWriteCommand, tmpResult);
            break;
        case DOM_POI:
            warning = handlePoiDomain(isWriteCommand, tmpResult);
            break;
        case DOM_POLYGON:
            warning = handlePolygonDomain(isWriteCommand, tmpResult);
            break;
        default:
            writeStatusCmd(CMD_SCENARIO, RTYPE_ERR, "Unknown domain specified");
            return false;
        }
    } catch (TraCIException &e) {
        writeStatusCmd(CMD_SCENARIO, RTYPE_ERR, e.what());
        return false;
    }

    // write response message
    writeStatusCmd(CMD_SCENARIO, RTYPE_OK, warning);
    // if necessary, add Scenario command containing the read value
    if (!isWriteCommand) {
        if (tmpResult.size() <= 253) {
            myOutputStorage.writeUnsignedByte(1 + 1 + (int)tmpResult.size());	// command length
            myOutputStorage.writeUnsignedByte(CMD_SCENARIO);	// command id
            myOutputStorage.writeStorage(tmpResult);	// variable dependant part
        } else {
            myOutputStorage.writeUnsignedByte(0);	// command length -> extended
            myOutputStorage.writeInt(1 + 4 + 1 + (int)tmpResult.size());
            myOutputStorage.writeUnsignedByte(CMD_SCENARIO);	// command id
            myOutputStorage.writeStorage(tmpResult);	// variable dependant part
        }
    }
    return true;
}

/*****************************************************************************/

bool
TraCIServer::commandAddVehicle() throw(TraCIException) {

    // read parameters
    std::string vehicleId = myInputStorage.readString();
    std::string vehicleTypeId = myInputStorage.readString();
    std::string routeId = myInputStorage.readString();
    std::string laneId = myInputStorage.readString();
    SUMOReal emitPosition = myInputStorage.readFloat();
    SUMOReal emitSpeed = myInputStorage.readFloat();

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
    float clippedEmitSpeed;
    if (emitSpeed<0) {
        clippedEmitSpeed = (float) MIN2(lane->getMaxSpeed(), vehicle->getMaxSpeed());
    } else {
        clippedEmitSpeed = (float) MIN3(lane->getMaxSpeed(), vehicle->getMaxSpeed(), emitSpeed);
    }

    // insert vehicle into the dictionary
    if (!MSNet::getInstance()->getVehicleControl().addVehicle(vehicle->getID(), vehicle)) {
        writeStatusCmd(CMD_ADDVEHICLE, RTYPE_ERR, "Could not add vehicle to VehicleControl");
        return false;
    }

    // try to emit
    if (!lane->isEmissionSuccess(vehicle, clippedEmitSpeed, emitPosition, true)) {
        MSNet::getInstance()->getVehicleControl().deleteVehicle(vehicle);
        writeStatusCmd(CMD_ADDVEHICLE, RTYPE_ERR, "Could not emit vehicle");
        return false;
    }

    // exec callback
    vehicle->onDepart();

    // create a reply message
    writeStatusCmd(CMD_ADDVEHICLE, RTYPE_OK, "");

    return true;
}

/*****************************************************************************/

bool
TraCIServer::commandDistanceRequest() throw(TraCIException) {
    Position2D pos1;
    Position2D pos2;
    RoadMapPos roadPos1;
    RoadMapPos roadPos2;
    const std::vector<MSLane*>* lanes;
	UNUSED_PARAMETER(lanes);

    // read position 1
    int posType = myInputStorage.readUnsignedByte();
    switch (posType) {
    case POSITION_ROADMAP:
        roadPos1.roadId = myInputStorage.readString();
        roadPos1.pos = myInputStorage.readFloat();
        roadPos1.laneId = (unsigned char)myInputStorage.readUnsignedByte();
        try {
            pos1 = convertRoadMapToCartesian(roadPos1);
        } catch (TraCIException &e) {
            writeStatusCmd(CMD_DISTANCEREQUEST, RTYPE_ERR, e.what());
            return false;
        }
        break;
    case POSITION_2D:
    case POSITION_2_5D:
    case POSITION_3D: {
        float p1x = myInputStorage.readFloat();
        float p1y = myInputStorage.readFloat();
        pos1.set(p1x, p1y);
    }
    if ((posType == POSITION_2_5D) || (posType == POSITION_3D)) {
        myInputStorage.readFloat();		// z value is ignored
    }
    roadPos1 = convertCartesianToRoadMap(pos1);
    break;
    default:
        writeStatusCmd(CMD_DISTANCEREQUEST, RTYPE_ERR, "Unknown position format used for distance request");
        return false;
    }

    // read position 2
    posType = myInputStorage.readUnsignedByte();
    switch (posType) {
    case POSITION_ROADMAP:
        roadPos2.roadId = myInputStorage.readString();
        roadPos2.pos = myInputStorage.readFloat();
        roadPos2.laneId = (unsigned char)myInputStorage.readUnsignedByte();
        try {
            pos2 = convertRoadMapToCartesian(roadPos2);
        } catch (TraCIException &e) {
            writeStatusCmd(CMD_DISTANCEREQUEST, RTYPE_ERR, e.what());
            return false;
        }
        break;
    case POSITION_2D:
    case POSITION_2_5D:
    case POSITION_3D: {
        float p2x = myInputStorage.readFloat();
        float p2y = myInputStorage.readFloat();
        pos2.set(p2x, p2y);
    }
    if ((posType == POSITION_2_5D) || (posType == POSITION_3D)) {
        myInputStorage.readFloat();		// z value is ignored
    }
    roadPos2 = convertCartesianToRoadMap(pos2);
    break;
    default:
        writeStatusCmd(CMD_DISTANCEREQUEST, RTYPE_ERR, "Unknown position format used for distance request");
        return false;
    }

    // read distance type
    int distType = myInputStorage.readUnsignedByte();

    float distance = 0.0;
    if (distType == REQUEST_DRIVINGDIST) {
        // compute driving distance
        std::vector<const MSEdge*> edges;
        TraCIDijkstraRouter<MSEdge> router(MSEdge::dictSize());

        if ((roadPos1.roadId.compare(roadPos2.roadId) == 0)
                && (roadPos1.pos <= roadPos2.pos)) {
            distance = roadPos2.pos - roadPos1.pos;
        } else {
            router.compute(MSEdge::dictionary(roadPos1.roadId), MSEdge::dictionary(roadPos2.roadId), NULL,
                           MSNet::getInstance()->getCurrentTimeStep(), edges);
            MSRoute route("", edges, false, RGBColor::DEFAULT_COLOR, std::vector<SUMOVehicleParameter::Stop>());
            distance = static_cast<float>(route.getDistanceBetween(roadPos1.pos, roadPos2.pos,
                                          MSEdge::dictionary(roadPos1.roadId), MSEdge::dictionary(roadPos2.roadId)));
        }
    } else {
        // compute air distance (default)
        // correct the distance type in case it was not valid
        distType = REQUEST_AIRDIST;
        distance = static_cast<float>(pos1.distanceTo(pos2));
    }

    // acknowledge distance request
    writeStatusCmd(CMD_DISTANCEREQUEST, RTYPE_OK, "");
    // write response command
    myOutputStorage.writeUnsignedByte(1 + 1 + 1 + 4);	// length
    myOutputStorage.writeUnsignedByte(CMD_DISTANCEREQUEST);		// command type
    myOutputStorage.writeUnsignedByte(distType);		// distance type
    myOutputStorage.writeFloat(distance);	// distance;
    return true;
}

/*****************************************************************************/


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

void
TraCIServer::convertExt2IntId(int extId, std::string& intId) {
    if (isMapChanged_) {
        isMapChanged_ = false;
        ext2intId.clear();
        for (map<std::string, int>::const_iterator iter = equippedVehicles_.begin(); iter != equippedVehicles_.end(); ++iter) {
            if (iter->second > -1) {
                ext2intId[iter->second] = iter->first;
            }
        }
    }

    // Search for external-Id-int and return internal-Id-string
    map<int, std::string>::const_iterator it = ext2intId.find(extId);
    if (it != ext2intId.end()) intId = it->second;
    else intId = "";
}

/*****************************************************************************/

MSVehicle*
TraCIServer::getVehicleByExtId(int extId) {
    if (!myHaveWarnedDeprecation) {
        MsgHandler::getWarningInstance()->inform("Using old TraCI API, please update your client!");
        myHaveWarnedDeprecation = true;
    }
    std::string intId;
    convertExt2IntId(extId, intId);
    return static_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().getVehicle(intId));
}

/*****************************************************************************/

MSTrafficLightLogic*
TraCIServer::getTLLogicByExtId(int extId) {
    if (!myHaveWarnedDeprecation) {
        MsgHandler::getWarningInstance()->inform("Using old TraCI API, please update your client!");
        myHaveWarnedDeprecation = true;
    }
    std::string intId = "";
    std::map<int, std::string>::iterator iter = trafficLightsExt2IntId.find(extId);
    if (iter != trafficLightsExt2IntId.end()) {
        intId = iter->second;
    }

    return MSNet::getInstance()->getTLSControl().getActive(intId);
}

/*****************************************************************************/

PointOfInterest*
TraCIServer::getPoiByExtId(int extId) {
    if (!myHaveWarnedDeprecation) {
        MsgHandler::getWarningInstance()->inform("Using old TraCI API, please update your client!");
        myHaveWarnedDeprecation = true;
    }
    std::string intId = "";
    std::map<int, std::string>::iterator iter = poiExt2IntId.find(extId);
    if (iter != poiExt2IntId.end()) {
        intId = iter->second;
    }

    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    PointOfInterest* poi = 0;
    for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer(); i++) {
        if (poi == 0) {
            poi = shapeCont.getPOICont(i).get(intId);
        }
    }

    return poi;
}

/*****************************************************************************/

Polygon2D*
TraCIServer::getPolygonByExtId(int extId) {
    if (!myHaveWarnedDeprecation) {
        MsgHandler::getWarningInstance()->inform("Using old TraCI API, please update your client!");
        myHaveWarnedDeprecation = true;
    }
    std::string intId = "";
    map<int, std::string>::const_iterator it = ext2intId.find(extId);
    if (it != ext2intId.end()) {
        intId = it->second;
    }

    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    Polygon2D* polygon = NULL;
    for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer(); i++) {
        if (polygon == NULL) {
            polygon = shapeCont.getPolygonCont(i).get(intId);
        }
    }

    return polygon;
}

/*****************************************************************************/

const Boundary&
TraCIServer::getNetBoundary() {
    // If already calculated, just return the boundary
    if (netBoundary_ != NULL) return *netBoundary_;

    // Otherwise calculate it first
    netBoundary_ = new Boundary();
    /*
    {
    // use the junctions to compute the boundaries
    for (size_t index=0; index<myNet.myJunctionWrapper.size(); index++) {
    if (myNet.myJunctionWrapper[index]->getShape().size()>0) {
    ret.add(myNet.myJunctionWrapper[index]->getBoundary());
    } else {
    ret.add(myNet.myJunctionWrapper[index]->getJunction().getPosition());
    }
    }
    }
    */
    // Get all edges
    const std::vector<MSEdge*> &edges = MSNet::getInstance()->getEdgeControl().getEdges();

    // Get Boundary of Single ...
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        const std::vector<MSLane*> &lanes = (*e)->getLanes();
        for (std::vector<MSLane*>::const_iterator laneIt = lanes.begin(); laneIt != lanes.end(); ++laneIt) {
            netBoundary_->add((*laneIt)->getShape().getBoxBoundary());
        }
    }
    return *netBoundary_;
}

/*****************************************************************************/

TraCIServer::RoadMapPos
TraCIServer::convertCartesianToRoadMap(Position2D pos) {
    RoadMapPos result;
    std::vector<std::string> allEdgeIds;
    MSEdge* edge;
    Position2D lineStart;
    Position2D lineEnd;
    double minDistance = HUGE_VAL;
    SUMOReal newDistance;
    Position2D intersection;
    MSLane* tmpLane;


    allEdgeIds = MSNet::getInstance()->getEdgeControl().getEdgeNames();

    // iterate through all known edges
    for (std::vector<std::string>::iterator itId = allEdgeIds.begin(); itId != allEdgeIds.end(); itId++) {
        edge = MSEdge::dictionary((*itId));
        const std::vector<MSLane*> &allLanes = edge->getLanes();

//		cerr << "--------" << endl << "Checking edge " << edge->getID() << endl << "--------" << endl;

        // iterate through all lanes of this edge
        for (std::vector<MSLane*>::const_iterator itLane = allLanes.begin(); itLane != allLanes.end(); itLane++) {
            Position2DVector shape = (*itLane)->getShape();

//			cerr << "### Lane: " << (*itLane)->getID() << endl;

            // iterate through all segments of this lane's shape
            for (unsigned int i = 0; i < shape.size()-1; i++) {
                lineStart = shape[i];
                lineEnd = shape[i+1];

//				cerr << "-Segment " << i << "(" << shape[i] << " - " << shape[i+1] << "): ";

                // if this line is no candidate for lying closer to the cartesian position
                // than the line determined so far, skip it
                if ((lineStart.y() > (pos.y()+minDistance) && lineEnd.y() > (pos.y()+minDistance))
                        || (lineStart.y() < (pos.y()-minDistance) && lineEnd.y() < (pos.y()-minDistance))
                        || (lineStart.x() > (pos.x()+minDistance) && lineEnd.x() > (pos.x()+minDistance))
                        || (lineStart.x() < (pos.x()-minDistance) && lineEnd.x() < (pos.x()-minDistance))) {

//					cerr << "skipping (minDistance = " << minDistance << ")" << endl;

                    continue;
                } else {
                    // else compute the distance and check it
                    newDistance = GeomHelper::closestDistancePointLine(pos, lineStart, lineEnd, intersection);

//					cerr << "not skipping. ";

                    if (newDistance < minDistance && newDistance != -1.0) {
                        // new distance is shorter: save the found road map position
                        minDistance = newDistance;
                        result.roadId = (*itId);
                        result.laneId = 0;
                        tmpLane = (*itLane);
                        while ((tmpLane =tmpLane->getRightLane()) != NULL) {
                            result.laneId++;
                        }
                        result.pos = (float)lineStart.distanceTo(intersection);
                        for (unsigned int j = 0; j < i; j++) {
                            result.pos += (float)(shape[j].distanceTo(shape[j+1]));
                        }

//						cerr << "Saved new pos: " << result.pos << ", intersec at (" << intersection.x() << "," << intersection.y()
//								<< "), minDistance = " << minDistance;
                    }

//					cerr << endl;
                }
            }
        }
    }

    return result;
}

/*****************************************************************************/

Position2D
TraCIServer::convertRoadMapToCartesian(traci::TraCIServer::RoadMapPos roadPos)
throw(TraCIException) {
    if (roadPos.pos < 0) {
        throw TraCIException("Position on lane must not be negative");
    }

    // get the edge and lane of this road map position
    MSEdge* road = MSEdge::dictionary(roadPos.roadId);
    if (road == NULL) {
        throw TraCIException("Unable to retrieve road with given id");
    }

    const std::vector<MSLane*> &allLanes = road->getLanes();
    if ((roadPos.laneId >= allLanes.size()) || (allLanes.size() == 0)) {
        throw TraCIException("No lane existing with such id on the given road");
    }

    // get corresponding x and y coordinates
    Position2DVector shape = allLanes[roadPos.laneId]->getShape();
    return shape.positionAtLengthPosition(roadPos.pos);
}

/*****************************************************************************/

std::string
TraCIServer::handleRoadMapDomain(bool isWriteCommand, tcpip::Storage& response)
throw(TraCIException) {
    std::string name = "";
    std::string warning = "";	// additional description for response
    DataTypeContainer dataCont;

    // domain object
    int objectId = myInputStorage.readInt();
    // check for valid object id
    if (objectId < 0 || (unsigned int) objectId >= MSEdge::dictSize()) {
        throw TraCIException("Invalid object id specified");
    }
    MSEdge* edge = MSEdge::dictionary(objectId);

    // variable id
    int variableId = myInputStorage.readUnsignedByte();

    // value data type
    int dataType = myInputStorage.readUnsignedByte();

    // if end of message is not yet reached, the value parameter has to be read
    if (myInputStorage.valid_pos()) {
        dataCont.readValue((unsigned int) dataType, myInputStorage);
    }

    if (isWriteCommand) {
        throw TraCIException("Road map domain does not contain writable variables");
    }

    // write beginning of the answer message
    response.writeUnsignedByte((isWriteCommand ? 0x01 : 0x00));	// get/set flag
    response.writeUnsignedByte(DOM_ROADMAP);	// domain
    response.writeInt(objectId);	// domain object id
    response.writeUnsignedByte(variableId);		// variable

    switch (variableId) {
        // name std::string of the object
    case DOMVAR_NAME:
        if (edge != NULL) {
            name = edge->getID();
            response.writeUnsignedByte(TYPE_STRING);
            response.writeString(name);
            if (dataType != TYPE_STRING) {
                warning = "Warning: requested data type could not be used; using string instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve road with given id");
        }
        break;

        // numerical id of an edge
    case DOMVAR_EXTID:
        //if (dataType != TYPE_STRING) {
        if (dataCont.getLastValueRead() != TYPE_STRING) {
            throw TraCIException("Internal id must be given as string value");
        }
        //name = myInputStorage.readString();
        name = dataCont.getString();
        edge = MSEdge::dictionary(name);
        if (edge != NULL) {
            response.writeUnsignedByte(TYPE_INTEGER);
            response.writeInt(edge->getNumericalID());
        } else {
            std::stringstream msg;
            msg << "Edge with internal id " << name << " not existing";
            throw TraCIException(msg.str());
        }
        break;

        // net boundaries
    case DOMVAR_BOUNDINGBOX:
        response.writeUnsignedByte(TYPE_BOUNDINGBOX);
        response.writeFloat((float)getNetBoundary().xmin());
        response.writeFloat((float)getNetBoundary().ymin());
        response.writeFloat((float)getNetBoundary().xmax());
        response.writeFloat((float)getNetBoundary().ymax());
        // add a warning to the response if the requested data type was not correct
        if (dataType != TYPE_BOUNDINGBOX) {
            warning = "Warning: requested data type could not be used; using boundary box type instead!";
        }
        break;

        // number of roads
    case DOMVAR_COUNT:
        response.writeUnsignedByte(TYPE_INTEGER);
        response.writeInt((int)(MSNet::getInstance()->getEdgeControl().getEdgeNames().size()));
        if (dataType != TYPE_INTEGER) {
            warning = "Warning: requested data type could not be used; using integer instead!";
        }
        break;

        // shape of a road
    case DOMVAR_SHAPE:
        if (edge != NULL) {
            const std::vector<MSLane*> &lanes = edge->getLanes();
            const Position2DVector shape = lanes[lanes.size()/2]->getShape();
            response.writeUnsignedByte(TYPE_POLYGON);
            response.writeUnsignedByte((int)MIN2(static_cast<size_t>(255),shape.size()));
            for (unsigned int iPoint=0; iPoint < MIN2(static_cast<size_t>(255),shape.size()); iPoint++) {
                response.writeFloat((float)shape[iPoint].x());
                response.writeFloat((float)shape[iPoint].y());
            }
            if (dataType != TYPE_POLYGON) {
                warning = "Warning: requested data type could not be used; using polygon type instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve road with given id");
        }
        break;

        // unknown variable
    default:
        throw TraCIException("Unknown domain variable specified");
    }

    return warning;
}

/*****************************************************************************/

std::string
TraCIServer::handleVehicleDomain(bool isWriteCommand, tcpip::Storage& response)
throw(TraCIException) {
    std::string name;
    int count = 0;
    MSVehicleControl* vehControl = NULL;
    std::string warning = "";	// additional description for response
    DataTypeContainer dataCont;

    // domain object
    int objectId = myInputStorage.readInt();
    MSVehicle* veh = getVehicleByExtId(objectId);	//get node by id

    // variable id
    int variableId = myInputStorage.readUnsignedByte();

    // value data type
    int dataType = myInputStorage.readUnsignedByte();

    // if end of message is not yet reached, the value parameter has to be read
    if (myInputStorage.valid_pos()) {
        dataCont.readValue(dataType, myInputStorage);
    }

    if (isWriteCommand) {
        throw TraCIException("Vehicle domain does not contain writable variables");
    }

    // write beginning of the answer message
    response.writeUnsignedByte((isWriteCommand ? 0x01 : 0x00));	// get/set flag
    response.writeUnsignedByte(DOM_VEHICLE);	// domain
    response.writeInt(objectId);	// domain object id
    response.writeUnsignedByte(variableId);		// variable

    switch (variableId) {
        // name std::string of the object
    case DOMVAR_NAME:
        if (veh != NULL) {
            name = veh->getID();
            response.writeUnsignedByte(TYPE_STRING);
            response.writeString(name);
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_STRING) {
                warning = "Warning: requested data type could not be used; using string instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;

        // external id of the object
    case DOMVAR_EXTID:
//		if (dataType != TYPE_STRING) {
        if (dataCont.getLastValueRead() != TYPE_STRING) {
            throw TraCIException("Internal id must be given as string value");
        }
//		name = myInputStorage.readString();
        name = dataCont.getString();
        if (equippedVehicles_.find(name) != equippedVehicles_.end()) {
            response.writeUnsignedByte(TYPE_INTEGER);
            response.writeInt(equippedVehicles_[name]);
        } else {
            std::stringstream msg;
            msg << "Vehicle with internal id " << name << " not existing or not accessible via TraCI";
            throw TraCIException(msg.str());
        }
        break;

        // number of active nodes
    case DOMVAR_COUNT:
        vehControl = &MSNet::getInstance()->getVehicleControl();
        for (MSVehicleControl::constVehIt vehIt = vehControl->loadedVehBegin(); vehIt != vehControl->loadedVehEnd(); vehIt++) {
            if (vehIt->second->isOnRoad()) {
                count++;
            }
        }
        response.writeUnsignedByte(TYPE_INTEGER);
        response.writeInt(count);
        if (dataType != TYPE_INTEGER) {
            warning = "Warning: requested data type could not be used; using integer instead!";
        }
        break;

        // upper bound for number of nodes
    case DOMVAR_MAXCOUNT:
        response.writeUnsignedByte(TYPE_INTEGER);
        response.writeInt(totalNumVehicles_);
        if (dataType != TYPE_INTEGER) {
            warning = "Warning: requested data type could not be used; using integer instead!";
        }
        break;

        // number of active nodes accesible via traci
    case DOMVAR_EQUIPPEDCOUNT:
        for (std::map<std::string, int>::iterator it=equippedVehicles_.begin(); it != equippedVehicles_.end(); it++) {
            MSVehicle* veh = static_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().getVehicle(it->first));
            if (veh->isOnRoad()) {
                count++;
            }
        }
        response.writeUnsignedByte(TYPE_INTEGER);
        response.writeInt(count);
        if (dataType != TYPE_INTEGER) {
            warning = "Warning: requested data type could not be used; using integer instead!";
        }
        break;

        // upper bound for number of nodes accesible via traci
    case DOMVAR_EQUIPPEDCOUNTMAX:
        response.writeUnsignedByte(TYPE_INTEGER);
        response.writeInt(static_cast<int>(0.5 + totalNumVehicles_ * penetration_));
        if (dataType != TYPE_INTEGER) {
            warning = "Warning: requested data type could not be used; using integer instead!";
        }
        break;

        // node position
    case DOMVAR_POSITION:
        if (veh != NULL) {
            switch (dataType) {
            case POSITION_3D:
                response.writeUnsignedByte(POSITION_3D);
                response.writeFloat((float)(veh->getPosition().x()));
                response.writeFloat((float)(veh->getPosition().y()));
                response.writeFloat(0);
                break;
            default:
                response.writeByte(POSITION_ROADMAP);
                response.writeString(veh->getEdge()->getID());
                response.writeFloat((float)(veh->getPositionOnLane()));
                int laneId = 0;
                MSLane* lane = veh->getLane().getRightLane();
                while (lane != NULL) {
                    laneId++;
                    lane =lane->getRightLane();
                }
                response.writeUnsignedByte(laneId);
                // add a warning to the response if the requested data type was not correct
                if (dataType != POSITION_ROADMAP) {
                    warning = "Warning: requested data type could not be used; using road map position instead!";
                }
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;

        // node speed
    case DOMVAR_SPEED:
        if (veh != NULL) {
            response.writeUnsignedByte(TYPE_FLOAT);
            response.writeFloat((float)(veh->getSpeed()));
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_FLOAT) {
                warning = "Warning: requested data type could not be used; using float instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;

        // node maximum allowed speed
    case DOMVAR_ALLOWED_SPEED:
        if (veh != NULL) {
            response.writeUnsignedByte(TYPE_FLOAT);
            response.writeFloat((float)(veh->getLane().getMaxSpeed()));
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_FLOAT) {
                warning = "Warning: requested data type could not be used; using float instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;

        // node route
    case DOMVAR_ROUTE:
        if (veh != NULL) {
            response.writeUnsignedByte(TYPE_STRING);
            MSRoute r = static_cast<SUMOVehicle*>(veh)->getRoute();
            std::string strRoute = "";
            for (MSRouteIterator it = r.begin(); it != r.end(); ++it) {
                if (strRoute.length()) strRoute.append(" ");
                strRoute.append((*it)->getID());
            }
            response.writeString(strRoute);
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_STRING) {
                warning = "Warning: requested data type could not be used; using string instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;

        // air distance from vehicle to a point
    case DOMVAR_AIRDISTANCE:
        // driving distance from vehicle to a point
    case DOMVAR_DRIVINGDISTANCE:
        if (veh != NULL) {
            Position2D destPos;
            RoadMapPos destRoadPos;
            float distance = 0.0;

            // read destinatin position
            float x,y;
			UNUSED_PARAMETER(x);
			UNUSED_PARAMETER(y);
            switch (dataCont.getLastValueRead()) {
            case POSITION_ROADMAP:
                destRoadPos = dataCont.getRoadMapPosition();
                destPos = convertRoadMapToCartesian(destRoadPos);
                if (MSEdge::dictionary(destRoadPos.roadId) == NULL) throw TraCIException("Unable to retrieve edge with given id");
                break;
            case POSITION_3D:
            case POSITION_2_5D:
            case POSITION_2D:
                destPos = dataCont.getAnyPosition();
                destRoadPos = convertCartesianToRoadMap(destPos);
                break;
            default:
                throw TraCIException("Distance request to unknown destination");
            }

//            switch (dataType)
//            {
//            case POSITION_ROADMAP:
//                {
//                    destEdge = myInputStorage.readString();
//                    destPos = myInputStorage.readFloat();
//                    destLane = myInputStorage.readUnsignedByte();
////                    cerr << " destEdge=" << destEdge << " destPos=" << destPos << " destLane=" << destLane << endl;
//                    edge = MSEdge::dictionary( destEdge );
//                    if (edge == NULL ) throw TraCIException("Unable to retrieve edge with given id");
//
//					const std::vector<MSLane*>& lanes = edge->getLanes();
//					if (destLane > lanes.size()-1) throw TraCIException("No lane existing with specified id on this edge");
//                    pos = lanes[destLane]->getShape().positionAtLengthPosition(destPos);
//                }
//                break;
//            case POSITION_2D:
//            case POSITION_2_5D:
//            case POSITION_3D:
//                {
//                    float destX = myInputStorage.readFloat();
//                    float destY = myInputStorage.readFloat();
//					if ((dataType == POSITION_3D) || (dataType == POSITION_2_5D)) {
//						myInputStorage.readFloat();	// z value is ignored
//					}
//					pos.set(destX, destY);
//
//					RoadMapPos roadPos = convertCartesianToRoadMap(pos);
//					destEdge = roadPos.roadId;
//					destPos = roadPos.pos;
//					destLane = roadPos.laneId;
//					edge = MSEdge::dictionary( destEdge );
//                }
//                break;
//            default:
//                throw TraCIException("Distance request to unknown destination");
//            }

            // compute the distance to destination position
            if (variableId == DOMVAR_AIRDISTANCE) {
                distance = static_cast<float>(veh->getPosition().distanceTo(destPos));
            } else {
                distance = static_cast<float>(veh->getDistanceToPosition(destRoadPos.pos, MSEdge::dictionary(destRoadPos.roadId)));
            }
            // write response
            response.writeUnsignedByte(TYPE_FLOAT);
            response.writeFloat(distance);
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;

        // CO2 emission
    case DOMVAR_CO2EMISSION:
        if (veh != NULL) {
            response.writeUnsignedByte(TYPE_FLOAT);
            response.writeFloat((float)(HelpersHBEFA::computeCO2(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getPreDawdleAcceleration())));
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_FLOAT) {
                warning = "Warning: requested data type could not be used; using float instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;
        // CO emission
    case DOMVAR_COEMISSION:
        if (veh != NULL) {
            response.writeUnsignedByte(TYPE_FLOAT);
            response.writeFloat((float)(HelpersHBEFA::computeCO(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getPreDawdleAcceleration())));
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_FLOAT) {
                warning = "Warning: requested data type could not be used; using float instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;
        // HC emission
    case DOMVAR_HCEMISSION:
        if (veh != NULL) {
            response.writeUnsignedByte(TYPE_FLOAT);
            response.writeFloat((float)(HelpersHBEFA::computeHC(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getPreDawdleAcceleration())));
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_FLOAT) {
                warning = "Warning: requested data type could not be used; using float instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;
        // PMx emission
    case DOMVAR_PMXEMISSION:
        if (veh != NULL) {
            response.writeUnsignedByte(TYPE_FLOAT);
            response.writeFloat((float)(HelpersHBEFA::computePMx(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getPreDawdleAcceleration())));
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_FLOAT) {
                warning = "Warning: requested data type could not be used; using float instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;
        // NOx emission
    case DOMVAR_NOXEMISSION:
        if (veh != NULL) {
            response.writeUnsignedByte(TYPE_FLOAT);
            response.writeFloat((float)(HelpersHBEFA::computeNOx(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getPreDawdleAcceleration())));
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_FLOAT) {
                warning = "Warning: requested data type could not be used; using float instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;
        // CO2 emission
    case DOMVAR_FUELCONSUMPTION:
        if (veh != NULL) {
            response.writeUnsignedByte(TYPE_FLOAT);
            response.writeFloat((float)(HelpersHBEFA::computeFuel(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getPreDawdleAcceleration())));
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_FLOAT) {
                warning = "Warning: requested data type could not be used; using float instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;
        // noise emission
    case DOMVAR_NOISEEMISSION:
        if (veh != NULL) {
            response.writeUnsignedByte(TYPE_FLOAT);
            response.writeFloat((float)(HelpersHarmonoise::computeNoise(veh->getVehicleType().getEmissionClass(), veh->getSpeed(), veh->getPreDawdleAcceleration())));
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_FLOAT) {
                warning = "Warning: requested data type could not be used; using float instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve node with given ID");
        }
        break;

        // unknown variable
    default:
        throw TraCIException("Unknown domain variable specified");
    }

    return warning;
}

/*****************************************************************************/

std::string
TraCIServer::handleTrafficLightDomain(bool isWriteCommand, tcpip::Storage& response)
throw(TraCIException) {
    int count = 0;
    std::string name;
    std::string warning = "";	// additional description for response
    DataTypeContainer dataCont;

    // domain object
    int objectId = myInputStorage.readInt();
    MSTrafficLightLogic* tlLogic = getTLLogicByExtId(objectId);

    // variable id
    int variableId = myInputStorage.readUnsignedByte();

    // value data type
    int dataType = myInputStorage.readUnsignedByte();

    // if end of message is not yet reached, the value parameter has to be read
    if (myInputStorage.valid_pos()) {
        dataCont.readValue((unsigned char)dataType, myInputStorage);
    }

    if (isWriteCommand) {
        throw TraCIException("Traffic Light domain does not contain writable variables");
    }

    // write beginning of the answer message
    response.writeUnsignedByte((isWriteCommand ? 0x01 : 0x00));	// get/set flag
    response.writeUnsignedByte(DOM_TRAFFICLIGHTS);	// domain
    response.writeInt(objectId);	// domain object id
    response.writeUnsignedByte(variableId);		// variable

    switch (variableId) {
        // name std::string of the object
    case DOMVAR_NAME:
        if (tlLogic != NULL) {
            name = tlLogic->getID();
            response.writeUnsignedByte(TYPE_STRING);
            response.writeString(name);
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_STRING) {
                warning = "Warning: requested data type could not be used; using string instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve traffic light with given id");
        }
        break;

        // external id of the object
    case DOMVAR_EXTID:
//		if (dataType != TYPE_STRING) {
        if (dataCont.getLastValueRead() != TYPE_STRING) {
            throw TraCIException("Internal id must be given as string value");
        }
//		name = myInputStorage.readString();
        name = dataCont.getString();
        if (trafficLightsInt2ExtId.find(name) != trafficLightsInt2ExtId.end()) {
            response.writeUnsignedByte(TYPE_INTEGER);
            response.writeInt(trafficLightsInt2ExtId[name]);
        } else {
            std::stringstream msg;
            msg << "Traffic Light with internal id " << name << " not existing";
            throw TraCIException(msg.str());
        }
        break;

        // count of traffic lights
    case DOMVAR_COUNT:
        count = (int)(MSNet::getInstance()->getTLSControl().getAllTLIds().size());
        response.writeUnsignedByte(TYPE_INTEGER);
        response.writeInt(count);
        // add a warning to the response if the requested data type was not correct
        if (dataType != TYPE_INTEGER) {
            warning = "Warning: requested data type could not be used; using integer instead!";
        }
        break;

        // position of a traffic light
    case DOMVAR_POSITION:
        if (tlLogic != NULL) {
            MSJunction* junc = MSNet::getInstance()->getJunctionControl().get(tlLogic->getID());
            response.writeUnsignedByte(POSITION_3D);
            response.writeFloat((float)(junc->getPosition().x()));
            response.writeFloat((float)(junc->getPosition().y()));
            response.writeFloat(0);
            // add a warning to the response if the requested data type was not correct
            if (dataType != POSITION_3D) {
                warning = "Warning: requested data type could not be used; using position 3d instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve traffic light with given id");
        }
        break;

        // current or next traffic light phase
    case DOMVAR_CURTLPHASE:
    case DOMVAR_NEXTTLPHASE:
        if (tlLogic != NULL) {
            // get the required phase of the tl logic
            size_t step = tlLogic->getCurrentPhaseIndex();
            if (variableId == DOMVAR_NEXTTLPHASE) {
                size_t curStep = tlLogic->getCurrentPhaseIndex();
                SUMOTime pos = tlLogic->getPhaseIndexAtTime(MSNet::getInstance()->getCurrentTimeStep());
                do {
                    pos++;
                } while ((step=tlLogic->getIndexFromOffset(pos)) == curStep);
            }
            MSPhaseDefinition phase = tlLogic->getPhase((unsigned int)step);

            // get the list of link vectors affected by that tl logic
            MSTrafficLightLogic::LinkVectorVector affectedLinks = tlLogic->getLinks();

            // for each affected link of that tl logic, write the  phase state
            // to the answer message along with preceding and succeeding edge
            Storage phaseList;
            int listLength = 0;
//			std::map<MSLane*, std::set<const MSEdge*> > connectLane2Edge;
            std::map<const MSEdge*, pair<const MSEdge*, int> > writtenEdgePairs;
            for (unsigned int i=0; i<affectedLinks.size(); i++) {
                // get the list of links controlled by that light
                MSTrafficLightLogic::LinkVector linkGroup = affectedLinks[i];
                // get the list of preceding lanes to that links
                MSTrafficLightLogic::LaneVector laneGroup = tlLogic->getLanesAt(i);
                // get status of the traffic light
                MSLink::LinkState tlState = phase.getSignalState(i);

//				const MSEdge* precEdge = NULL;
//				const MSEdge* succEdge = NULL;
                for (unsigned int linkNo=0; linkNo<linkGroup.size(); linkNo++) {
                    // if multiple lanes of different edges lead to the same lane on another edge,
                    // only write such pair of edges once
                    /*					if ((precEdge == laneGroup[linkNo]->getEdge())
                    						&& (succEdge == linkGroup[linkNo]->getLane()->getEdge())) {
                    						continue;
                    					}
                    					// remember preceding and succeeding edge
                    					precEdge = laneGroup[linkNo]->getEdge();
                    					succEdge = linkGroup[linkNo]->getLane()->getEdge();

                    					// if the current ingoing lane was part of a connection before...
                    					std::map<MSLane*, std::set<const MSEdge*> >::iterator itMap = connectLane2Edge.find(laneGroup[linkNo]);
                    					if (itMap != connectLane2Edge.end()) {
                    						// ...and the succeding edge of this connection is the same as before...
                    						std::set<const MSEdge*>::iterator itEdge = itMap->second.find(succEdge);
                    						if (itEdge != itMap->second.end()) {
                    							// ...then the connection's phase doesn't need to be reported again
                    							continue;
                    						}
                    					}
                    					// remember the edge that this lane leads to
                    					connectLane2Edge[laneGroup[linkNo]].insert(succEdge);
                    */
                    MSEdge &precEdge = laneGroup[linkNo]->getEdge();
                    MSEdge &succEdge = linkGroup[linkNo]->getLane()->getEdge();

                    // for each pair of edges and every different tl state, write only one tl switch command
                    std::map<const MSEdge*, pair<const MSEdge*, int> >::iterator itPair = writtenEdgePairs.find(&precEdge);
                    if (itPair != writtenEdgePairs.end()) {
                        if (itPair->second.first == &succEdge && itPair->second.second == tlState) {
                            continue;
                        }
                    }
                    // remember the current edge pair and tl status
                    writtenEdgePairs[&precEdge] = std::make_pair(&succEdge, tlState);

                    // write preceding edge
                    phaseList.writeString(precEdge.getID());
                    // write succeeding edge
                    phaseList.writeString(succEdge.getID());
                    // write status of the traffic light
                    switch (tlState) {
                    case MSLink::LINKSTATE_TL_GREEN_MAJOR:
                    case MSLink::LINKSTATE_TL_GREEN_MINOR:
                        phaseList.writeUnsignedByte(TLPHASE_GREEN);
                        break;
                    case MSLink::LINKSTATE_TL_YELLOW_MAJOR:
                    case MSLink::LINKSTATE_TL_YELLOW_MINOR:
                        phaseList.writeUnsignedByte(TLPHASE_YELLOW);
                        break;
                    case MSLink::LINKSTATE_TL_RED:
                        phaseList.writeUnsignedByte(TLPHASE_RED);
                        break;
                    case MSLink::LINKSTATE_TL_OFF_BLINKING:
                        phaseList.writeUnsignedByte(TLPHASE_BLINKING);
                        break;
                    case MSLink::LINKSTATE_TL_OFF_NOSIGNAL:
                        phaseList.writeUnsignedByte(TLPHASE_NOSIGNAL);
                        break;
                    default:
                        phaseList.writeUnsignedByte(TLPHASE_NOSIGNAL);
                    }
                    // increase length of the phase list
                    listLength++;
                }
            }
            //write data type to answer message
            response.writeUnsignedByte(TYPE_TLPHASELIST);
            // write length of the phase list to answer message
            response.writeUnsignedByte(listLength);
            // write list of phases to answer message
            response.writeStorage(phaseList);

            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_TLPHASELIST) {
                warning = "Warning: requested data type could not be used; using type traffic light phase list instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve traffic light with given id");
        }
        break;

        // unknown variable
    default:
        throw TraCIException("Unknown domain variable specified");
    }

    return warning;
}

/*****************************************************************************/

std::string
TraCIServer::handlePoiDomain(bool isWriteCommand, tcpip::Storage& response)
throw(TraCIException) {
    std::string name;
    std::string warning = "";	// additional description for response
    DataTypeContainer dataCont;

    // domain object
    int objectId = myInputStorage.readInt();
    PointOfInterest* poi = getPoiByExtId(objectId);

    // variable id
    int variableId = myInputStorage.readUnsignedByte();

    // value data type
    int dataType = myInputStorage.readUnsignedByte();

    // if end of message is not yet reached, the value parameter has to be read
    if (myInputStorage.valid_pos()) {
        dataCont.readValue((unsigned char)dataType, myInputStorage);
    }

    if (isWriteCommand) {
        throw TraCIException("Point of interest domain does not contain writable variables");
    }

    // write beginning of the answer message
    response.writeUnsignedByte((isWriteCommand ? 0x01 : 0x00));	// get/set flag
    response.writeUnsignedByte(DOM_POI);	// domain
    response.writeInt(objectId);	// domain object id
    response.writeUnsignedByte(variableId);		// variable

    // get list of shapes and determine total number of poi
    int count = 0;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer(); i++) {
        count += shapeCont.getPOICont(i).size();
    }

    switch (variableId) {
        // name std::string of the object
    case DOMVAR_NAME:
        if (poi != NULL) {
            name = poi->getID();
            response.writeUnsignedByte(TYPE_STRING);
            response.writeString(name);
            // add a warning to the response if the requested data type was not correct
            if (dataType != TYPE_STRING) {
                warning = "Warning: requested data type could not be used; using string instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve point of interest with given id");
        }
        break;

        // external id of the object
    case DOMVAR_EXTID:
//		if (dataType != TYPE_STRING) {
        if (dataCont.getLastValueRead() != TYPE_STRING) {
            throw TraCIException("Internal id must be given as string value");
        }
//		name = myInputStorage.readString();
        name = dataCont.getString();
        if (poiInt2ExtId.find(name) != poiInt2ExtId.end()) {
            response.writeUnsignedByte(TYPE_INTEGER);
            response.writeInt(poiInt2ExtId[name]);
        } else {
            std::stringstream msg;
            msg << "Point of interest with internal id " << name << " not existing";
            throw TraCIException(msg.str());
        }
        break;

        // number of poi
    case DOMVAR_COUNT:
        response.writeUnsignedByte(TYPE_INTEGER);
        response.writeInt(count);
        // add a warning to the response if the requested data type was not correct
        if (dataType != TYPE_INTEGER) {
            warning = "Warning: requested data type could not be used; using integer instead!";
        }
        break;

        // position of a poi
    case DOMVAR_POSITION:
        if (poi == NULL) {
            throw TraCIException("Unable to retrieve point of interest with given id");
        } else {
            response.writeUnsignedByte(POSITION_3D);
            response.writeFloat((float)(poi->x()));
            response.writeFloat((float)(poi->y()));
            response.writeFloat(0);
        }
        // add a warning to the response if the requested data type was not correct
        if (dataType != POSITION_3D) {
            warning = "Warning: requested data type could not be used; using 3D position instead!";
        }
        break;

        // type of a poi
    case DOMVAR_TYPE:
        if (poi == NULL) {
            throw TraCIException("Unable to retrieve point of interest with given id");
        } else {
            response.writeUnsignedByte(TYPE_STRING);
            response.writeString(poi->getType());
        }
        // add a warning to the response if the requested data type was not correct
        if (dataType != TYPE_STRING) {
            warning = "Warning: requested data type could not be used; using string instead!";
        }
        break;

        // layer of a poi
    case DOMVAR_LAYER:
        // unknown variable
    default:
        throw TraCIException("Unknown domain variable specified");
    }

    return warning;
}

/*****************************************************************************/

std::string
TraCIServer::handlePolygonDomain(bool isWriteCommand, tcpip::Storage& response)
throw(TraCIException) {
    std::string name;
    std::string warning = "";	// additional description for response
    DataTypeContainer dataCont;

    // domain object
    int objectId = myInputStorage.readInt();
    Polygon2D* poly = getPolygonByExtId(objectId);

    // variable id
    int variableId = myInputStorage.readUnsignedByte();

    // value data type
    int dataType = myInputStorage.readUnsignedByte();

    // if end of message is not yet reached, the value parameter has to be read
    if (myInputStorage.valid_pos()) {
        dataCont.readValue((unsigned char)dataType, myInputStorage);
    }

    if (isWriteCommand) {
        throw TraCIException("Polygon domain does not contain writable variables");
    }

    // write beginning of the answer message
    response.writeUnsignedByte((isWriteCommand ? 0x01 : 0x00));	// get/set flag
    response.writeUnsignedByte(DOM_POLYGON);	// domain
    response.writeInt(objectId);	// domain object id
    response.writeUnsignedByte(variableId);		// variable

    // get list of shapes and determine total number of polygons
    int count = 0;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    for (int i = shapeCont.getMinLayer(); i <= shapeCont.getMaxLayer(); i++) {
        count += shapeCont.getPolygonCont(i).size();
    }

    switch (variableId) {
        // name std::string of the object
    case DOMVAR_NAME:
        if (poly != NULL) {
            name = poly->getID();
            response.writeUnsignedByte(TYPE_STRING);
            response.writeString(name);
            if (dataType != TYPE_STRING) {
                warning = "Warning: requested data type could not be used; using string instead!";
            }
        } else {
            throw TraCIException("Unable to retrieve polygon with given id");
        }
        break;

        // external id of the object
    case DOMVAR_EXTID:
//		if (dataType != TYPE_STRING) {
        if (dataCont.getLastValueRead() != TYPE_STRING) {
            throw TraCIException("Internal id must be given as string value");
        }
//		name = myInputStorage.readString();
        name = dataCont.getString();
        if (polygonInt2ExtId.find(name) != polygonInt2ExtId.end()) {
            response.writeUnsignedByte(TYPE_INTEGER);
            response.writeInt(polygonInt2ExtId[name]);
        } else {
            std::stringstream msg;
            msg << "Polygon with internal id " << name << " not existing";
            throw TraCIException(msg.str());
        }
        break;

        // number of polygons
    case DOMVAR_COUNT:
        response.writeUnsignedByte(TYPE_INTEGER);
        response.writeInt(count);
        // add a warning to the response if the requested data type was not correct
        if (dataType != TYPE_INTEGER) {
            warning = "Warning: requested data type could not be used; using integer instead!";
        }
        break;

        // position of a polygon
    case DOMVAR_POSITION:
        if (poly == NULL) {
            throw TraCIException("Unable to retrieve polygon with given id");
        } else {
            response.writeUnsignedByte(POSITION_3D);
            response.writeFloat((float)(poly->getShape().getPolygonCenter().x()));
            response.writeFloat((float)(poly->getShape().getPolygonCenter().y()));
            response.writeFloat(0);
        }
        // add a warning to the response if the requested data type was not correct
        if (dataType != POSITION_3D) {
            warning = "Warning: requested data type could not be used; using 3D position instead!";
        }
        break;

        // type of a polygon
    case DOMVAR_TYPE:
        if (poly == NULL) {
            throw TraCIException("Unable to retrieve polygon with given id");
        } else {
            response.writeUnsignedByte(TYPE_STRING);
            response.writeString(poly->getType());
        }
        // add a warning to the response if the requested data type was not correct
        if (dataType != TYPE_STRING) {
            warning = "Warning: requested data type could not be used; using string instead!";
        }
        break;

        // shape of a polygon
    case DOMVAR_SHAPE:
        if (poly == NULL) {
            throw TraCIException("Unable to retrieve polygon with given id");
        } else {
            response.writeUnsignedByte(TYPE_POLYGON);
            response.writeUnsignedByte((int)MIN2(static_cast<size_t>(255),poly->getShape().size()));
            for (unsigned int i=0; i < MIN2(static_cast<size_t>(255),poly->getShape().size()); i++) {
                response.writeFloat((float)(poly->getShape()[i].x()));
                response.writeFloat((float)(poly->getShape()[i].y()));
            }
        }
        // add a warning to the response if the requested data type was not correct
        if (dataType != TYPE_POLYGON) {
            warning = "Warning: requested data type could not be used; using polygon type instead!";
        }
        break;

        // layer of a polygon
    case DOMVAR_LAYER:
        // unknown variable
    default:
        throw TraCIException("Unknown domain variable specified");
    }

    return warning;
}

/*****************************************************************************/

void
TraCIServer::handleLifecycleSubscriptions()
throw(TraCIException) {

    if (myLifecycleSubscriptions.count(DOM_VEHICLE) != 0) {

        for (std::set<int>::const_iterator i = myDestroyedVehicles.begin(); i != myDestroyedVehicles.end(); i++) {
            int extId = *i;

            myOutputStorage.writeUnsignedByte(1+1+1+4);
            myOutputStorage.writeUnsignedByte(CMD_OBJECTDESTRUCTION);
            myOutputStorage.writeUnsignedByte(DOM_VEHICLE);
            myOutputStorage.writeInt(extId);
        }
        myDestroyedVehicles.clear();

        for (std::set<int>::const_iterator i = myCreatedVehicles.begin(); i != myCreatedVehicles.end(); i++) {
            int extId = *i;

            myOutputStorage.writeUnsignedByte(1+1+1+4);
            myOutputStorage.writeUnsignedByte(CMD_OBJECTCREATION);
            myOutputStorage.writeUnsignedByte(DOM_VEHICLE);
            myOutputStorage.writeInt(extId);
        }
        myCreatedVehicles.clear();

    }
}

/*****************************************************************************/

void
TraCIServer::handleDomainSubscriptions(const SUMOTime& currentTime, const map<int, const SUMOVehicle*>& activeEquippedVehicles)
throw(TraCIException) {

    if (myDomainSubscriptions.count(DOM_VEHICLE) != 0) {

        std::list<std::pair<int, int> > subscribedVariables = myDomainSubscriptions[DOM_VEHICLE];

        // iterate over all objects
        for (map<int, const SUMOVehicle*>::const_iterator iter = activeEquippedVehicles.begin(); iter != activeEquippedVehicles.end(); ++iter) {
            int extId = (*iter).first;
            const MSVehicle* vehicle = static_cast<const MSVehicle*>((*iter).second);
            Storage tempMsg;

            // buffer send of command
            tempMsg.writeUnsignedByte(CMD_UPDATEOBJECT);

            // buffer send of domain
            tempMsg.writeByte(DOM_VEHICLE);

            // buffer send of object id
            tempMsg.writeInt(extId);

            // buffer send of subscribed variables
            for (std::list<std::pair<int, int> >::const_iterator i = subscribedVariables.begin(); i != subscribedVariables.end(); i++) {
                int variableId = i->first;
                int dataType = i->second;

                if ((variableId == DOMVAR_SIMTIME) && (dataType == TYPE_DOUBLE)) {
                    tempMsg.writeDouble(currentTime);
                }
                if ((variableId == DOMVAR_SPEED) && (dataType == TYPE_FLOAT)) {
                    tempMsg.writeFloat((float)(vehicle->getSpeed()));
                }
                if ((variableId == DOMVAR_ALLOWED_SPEED) && (dataType == TYPE_FLOAT)) {
                    tempMsg.writeFloat((float)(vehicle->getLane().getMaxSpeed()));
                }
                if ((variableId == DOMVAR_POSITION) && (dataType == POSITION_2D)) {
                    Position2D pos = vehicle->getLane().getShape().positionAtLengthPosition(vehicle->getPositionOnLane());
                    tempMsg.writeFloat((float)(pos.x()));
                    tempMsg.writeFloat((float)(pos.y()));
                }
                if ((variableId == DOMVAR_POSITION) && (dataType == POSITION_ROADMAP)) {
                    tempMsg.writeString(vehicle->getEdge()->getID());
                }
                if ((variableId == DOMVAR_ANGLE) && (dataType == TYPE_FLOAT)) {
                    tempMsg.writeFloat((float)(vehicle->getLane().getShape().rotationDegreeAtLengthPosition(vehicle->getPositionOnLane())));
                }
            }
            // send command length
            myOutputStorage.writeUnsignedByte((int)tempMsg.size()+1);
            // send command
            myOutputStorage.writeStorage(tempMsg);
        }
    }
}


bool
TraCIServer::addSubscription(int commandId) throw(TraCIException) {
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
                                       std::string &errors) throw(TraCIException) {
    bool ok = true;
    tcpip::Storage outputStorage;
    size_t wholeSize = 0;
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
            int responseType = tmpOutput.readUnsignedByte();
            int variable = tmpOutput.readUnsignedByte();
            std::string id = tmpOutput.readString();
            outputStorage.writeUnsignedByte(variable);
            outputStorage.writeUnsignedByte(RTYPE_OK);
            length -= (1+4+1+4+(int)id.length());
            while (--length>0) {
                outputStorage.writeUnsignedByte(tmpOutput.readUnsignedByte());
            }
        } else {
            int length = tmpOutput.readUnsignedByte();
            int cmd = tmpOutput.readUnsignedByte();
            int status = tmpOutput.readUnsignedByte();
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
