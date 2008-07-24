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
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include "TraCIConstants.h"
#include "TraCIServer.h"
#include "TraCIHandler.h"
#include "TraCIDijkstraRouter.h"

#ifndef NO_TRACI

#include "foreign/tcpip/socket.h"
#include "foreign/tcpip/storage.h"
#include "utils/common/SUMOTime.h"
#include "utils/common/SUMODijkstraRouter.h"
#include "utils/common/NamedObjectCont.h"
#include "utils/common/RandHelper.h"
#include "utils/shapes/PointOfInterest.h"
#include "utils/shapes/ShapeContainer.h"
#include "utils/shapes/Polygon2D.h"
#include "utils/xml/XMLSubSys.h"
#include "microsim/MSNet.h"
#include "microsim/MSVehicleControl.h"
#include "microsim/MSVehicle.h"
#include "microsim/MSEdge.h"
#include "microsim/MSRouteHandler.h"
#include "microsim/MSRouteLoaderControl.h"
#include "microsim/MSRouteLoader.h"
#include "microsim/MSJunctionControl.h"
#include "microsim/MSJunction.h"
#include "microsim/traffic_lights/MSTLLogicControl.h"

#include "microsim/MSEdgeControl.h"
#include "microsim/MSLane.h"
#include "microsim/trigger/MSCalibrator.h"

#include <xercesc/sax2/SAX2XMLReader.hpp>

#include <string>
#include <map>
#include <iostream>


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace tcpip;


namespace traci
{

// ===========================================================================
// static member definitions
// ===========================================================================
TraCIServer* TraCIServer::instance_ = 0;
bool TraCIServer::closeConnection_ = false;

// ===========================================================================
// method definitions
// ===========================================================================

/*****************************************************************************/

TraCIServer::TraCIServer()
{
    OptionsCont &oc = OptionsCont::getOptions();

    beginTime_ = oc.getInt("begin");
    endTime_ = oc.getInt("end");
    targetTime_ = INT_MAX;
    penetration_ = oc.getFloat("penetration");
    routeFile_ = oc.getString("route-files");
    isMapChanged_ = true;
    numEquippedVehicles_ = 0;
	totalNumVehicles_ = 0;
    closeConnection_ = false;
    netBoundary_ = NULL;

	// display warning if internal lanes are not used
	if (!oc.isSet("use-internal-links")) {
		std::stringstream msg;
		msg << "Warning: starting TraCI without using internal lanes! "
			<< "Vehicles will jump over junctions; use option --use-internal-links "
			<< "to avoid unexpected behavior" << std::endl;
		std::cout << msg.str();
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
			polygonInt2ExtId[(*it)->getName()] = polyId;
			polygonExt2IntId[polyId] = (*it)->getName();
			polyId++;
		}
	}

	// determine the maximum number of vehicles by searching route and additional input files for "vehicle" tags
	TraCIHandler xmlHandler;
	SAX2XMLReader* xmlParser = XMLSubSys::getSAXReader(xmlHandler);
	xmlParser->setContentHandler(&xmlHandler);
	xmlParser->setErrorHandler(&xmlHandler);

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
	
	delete xmlParser;

    try {
        int port = oc.getInt("remote-port");
        // Opens listening socket
        std::cout << "***Starting server on port " << port << " ***" << std::endl;
        socket_ = new Socket(port);
        socket_->accept();
        // When got here, a client has connected
    } catch (SocketException e) {
        throw ProcessError(e.what());
    }

}

/*****************************************************************************/

TraCIServer::~TraCIServer()
{
    if (socket_ != NULL) delete socket_;

    if (netBoundary_ != NULL) delete netBoundary_;
}

/*****************************************************************************/

bool
TraCIServer::processCommandsUntilSimStep(SUMOTime step)
{
    try {
        if (instance_ == 0) {
            if (OptionsCont::getOptions().getInt("remote-port") != 0) {
                instance_ = new traci::TraCIServer();
            } else {
                return false;
            }
        }
        if (instance_->targetTime_ < step) {
            return false;
        }
        // Simulation should run until
        // 1. end time reached or
        // 2. got CMD_CLOSE or
        // 3. Client closes socket connection

        while (!instance_->closeConnection_) {
            Storage storIn;
            Storage storOut;
            // Read a message
            instance_->socket_->receiveExact(storIn);

            while (storIn.valid_pos() && !instance_->closeConnection_) {
                // dispatch each command
                if (instance_->dispatchCommand(storIn, storOut) == CMD_SIMSTEP) {
                    return true;
                }
            }

            // send out all answers as one storage
            instance_->socket_->sendExact(storOut);
        }
    } catch (std::invalid_argument e) {
        throw ProcessError(e.what());
    } catch (TraCIException e) {
        throw ProcessError(e.what());
    } catch (SocketException e) {
        throw ProcessError(e.what());
    }
    if (instance_ != NULL) 
    {
        delete instance_;
        instance_ = 0;
        closeConnection_ = true;
    }
    //instance_->targetTime_ = INT_MAX;
    return false;
}

void
TraCIServer::processAfterSimStep()
{
    try {
        Storage storOut;
        // send out all answers as one storage
        instance_->postProcessSimulationStep(storOut);
        instance_->socket_->sendExact(storOut);
    } catch (TraCIException e) {
        throw ProcessError(e.what());
    } catch (SocketException e) {
        throw ProcessError(e.what());
    }
}

bool
TraCIServer::wasClosed()
{
    return closeConnection_;
}

/*****************************************************************************/

int
TraCIServer::dispatchCommand(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException, std::invalid_argument)
{
    int commandStart = requestMsg.position();
    int commandLength = requestMsg.readUnsignedByte();

    int commandId = requestMsg.readUnsignedByte();

    // dispatch commands
    switch (commandId) {
    case CMD_SETMAXSPEED:
        commandSetMaximumSpeed(requestMsg, respMsg);
        break;
    case CMD_SIMSTEP:
        commandSimulationStep(requestMsg,respMsg);
        break;
    case CMD_STOP:
        commandStopNode(requestMsg, respMsg);
        break;
    case CMD_CHANGELANE:
        commandChangeLane(requestMsg, respMsg);
        break;
    case CMD_CHANGEROUTE:
        commandChangeRoute(requestMsg, respMsg);
        break;
	case CMD_CHANGETARGET:
		commandChangeTarget(requestMsg, respMsg);
		break;
    case CMD_GETALLTLIDS:
        commandGetAllTLIds(requestMsg, respMsg);
        break;
	case CMD_GETTLSTATUS:
		commandGetTLStatus(requestMsg, respMsg);
		break;
    case CMD_CLOSE:
        commandCloseConnection(requestMsg, respMsg);
        break;
    case CMD_UPDATECALIBRATOR:
        commandUpdateCalibrator(requestMsg, respMsg);
        break;
	case CMD_POSITIONCONVERSION:
		commandPositionConversion(requestMsg, respMsg);
		break;
	case CMD_SLOWDOWN:
		commandSlowDown(requestMsg, respMsg);
		break;
	case CMD_SCENARIO:
		commandScenario(requestMsg, respMsg);
		break;
	case CMD_DISTANCEREQUEST:
		commandDistanceRequest(requestMsg, respMsg);
		break;
	case CMD_SUBSCRIBELIFECYCLES:
		commandSubscribeLifecycles(requestMsg, respMsg);
		break;
	case CMD_UNSUBSCRIBELIFECYCLES:
		commandUnsubscribeLifecycles(requestMsg, respMsg);
		break;
	case CMD_SUBSCRIBEDOMAIN:
		commandSubscribeDomain(requestMsg, respMsg);
		break;
	case CMD_UNSUBSCRIBEDOMAIN:
		commandUnsubscribeDomain(requestMsg, respMsg);
		break;
    default:
        writeStatusCmd(respMsg, commandId, RTYPE_NOTIMPLEMENTED, "Command not implemented in sumo");
        closeConnection_ = true;
    }

    if (requestMsg.position() != commandStart + commandLength) {
        ostringstream msg;
        msg << "Wrong position in requestMessage after dispatching command.";
        msg << " Expected command length was " << commandLength;
        msg << " but " << requestMsg.position() - commandStart << " Bytes were read.";
        writeStatusCmd(respMsg, commandId, RTYPE_ERR, msg.str() );
        closeConnection_ = true;
    }
    return commandId;
}

/*****************************************************************************/

void
TraCIServer::commandSetMaximumSpeed(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException, std::invalid_argument)
{
    MSVehicle* veh = getVehicleByExtId(requestMsg.readInt());   // external node id (equipped vehicle number)
    float maxspeed = requestMsg.readFloat();

    if (veh == NULL) {
        writeStatusCmd(respMsg, CMD_SETMAXSPEED, RTYPE_ERR, "Can not retrieve node with given ID");
        return;
    }

    if (maxspeed>=0.0) {
        veh->setIndividualMaxSpeed(maxspeed);
    } else {
        veh->unsetIndividualMaxSpeed();
    }

    // create a reply message
    writeStatusCmd(respMsg, CMD_SETMAXSPEED, RTYPE_OK, "");

    return;
}

/*****************************************************************************/

void
TraCIServer::commandSimulationStep(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException, std::invalid_argument)
{
	MSNet *net = MSNet::getInstance();
    SUMOTime currentTime = net->getCurrentTimeStep();

    // TargetTime
    targetTime_ = static_cast<SUMOTime>(requestMsg.readDouble()) + beginTime_;
    if (targetTime_ > endTime_) {
        targetTime_ = endTime_;
    }
    resType_ = requestMsg.readUnsignedByte();
}

void
TraCIServer::postProcessSimulationStep(tcpip::Storage& respMsg) throw(TraCIException)
{
    // Position representation
    if (resType_ != POSITION_NONE && resType_ != POSITION_2D && resType_ != POSITION_ROADMAP
		&& resType_ != POSITION_2_5D && resType_ != POSITION_3D) {
        writeStatusCmd(respMsg, CMD_SIMSTEP, RTYPE_ERR, "Error: unsupported return format requested.");
        closeConnection_ = true;
        return;
    }
    isMapChanged_ = true;
    // Everything is fine
    writeStatusCmd(respMsg, CMD_SIMSTEP, RTYPE_OK, "");

    // prepare output
    try {
    	MSNet *net = MSNet::getInstance();
        // map containing all active equipped vehicles. maps external id to MSVehicle*
        map<int, const MSVehicle*> activeEquippedVehicles;
        // get access to all vehicles in simulation
        MSVehicleControl &vehControl = net->getVehicleControl();
        // iterate over all vehicles in simulation
        for (map<string, MSVehicle*>::const_iterator iter = vehControl.loadedVehBegin();
             iter != vehControl.loadedVehEnd(); ++iter) {
            // selected vehicle
            const string vehicleId   = (*iter).first;
            const MSVehicle *vehicle = (*iter).second;
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
                 i != myLivingVehicles.end(); ) {
                int extId = *i;
                if (activeEquippedVehicles.find(extId) == activeEquippedVehicles.end()) {
                    myDestroyedVehicles.insert(extId);
                    myLivingVehicles.erase(i++);
                } else {
                    i++;
                }
            }
        }

        handleLifecycleSubscriptions(respMsg);

        // for each vehicle process any active traci command
        for (std::map<std::string, int>::iterator iter = equippedVehicles_.begin();
             iter != equippedVehicles_.end(); ++iter) {
            if ((*iter).second != -1) { // Look only at equipped vehicles
                MSVehicle* veh = net->getVehicleControl().getVehicle((*iter).first);
                if (veh != NULL) {
                    veh->processTraCICommands(targetTime_);
                }
            }
        }

        handleDomainSubscriptions(respMsg, targetTime_, activeEquippedVehicles);

        //out.writeChar( static_cast<unsigned char>(rtype) );
        //out.writeInt(numEquippedVehicles_);
        // iterate over all active equipped vehicles
        // and generate a Move Node command for each vehicle
        if (resType_ != POSITION_NONE) {
            for (map<int, const MSVehicle*>::iterator iter = activeEquippedVehicles.begin();
                 iter != activeEquippedVehicles.end(); ++iter) {
                int extId = (*iter).first;
                const MSVehicle* vehicle = (*iter).second;
                Storage tempMsg;

                // command type
                tempMsg.writeUnsignedByte(CMD_MOVENODE);
                // node id
                tempMsg.writeInt(extId);
                // end time
                tempMsg.writeDouble(targetTime_);

                if (resType_ == POSITION_2D) {
                    // return type
                    tempMsg.writeUnsignedByte(POSITION_2D);

                    Position2D pos = vehicle->getPosition();
                    //xpos
                    tempMsg.writeFloat(pos.x() - getNetBoundary().xmin());
                    // y pos
                    tempMsg.writeFloat(pos.y() - getNetBoundary().ymin());
                } else if (resType_ == POSITION_ROADMAP) {
                    // return type
                    tempMsg.writeUnsignedByte(POSITION_ROADMAP);

                    tempMsg.writeString(vehicle->getEdge()->getID());
                    tempMsg.writeFloat(vehicle->getPositionOnLane());

                    // determine index of the lane the vehicle is on
                    int laneId = 0;
                    const MSLane* lane = &vehicle->getLane();
                    while ((lane = lane->getRightLane()) != NULL) {
                        laneId++;
                    }
                    tempMsg.writeUnsignedByte(laneId);
                } else if (resType_ == POSITION_3D || resType_ == POSITION_2_5D) {
                    // return type
                    tempMsg.writeUnsignedByte(resType_);

                    Position2D pos = vehicle->getPosition();
                    //xpos
                    tempMsg.writeFloat(pos.x() - getNetBoundary().xmin());
                    // y pos
                    tempMsg.writeFloat(pos.y() - getNetBoundary().ymin());
                    // z pos: ignored
                    tempMsg.writeFloat(0);
                }

                // command length
                respMsg.writeUnsignedByte(tempMsg.size()+1);
                // content
                respMsg.writeStorage(tempMsg);
            }
        }
    } catch (...) {
        writeStatusCmd(respMsg, CMD_SIMSTEP, RTYPE_ERR, "some error happen in command: simulation step. Sumo shuts down.");
    }
}

/*****************************************************************************/

void
TraCIServer::commandStopNode(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException, std::invalid_argument)
{
    //std::string roadID;
    //float lanePos;
    //unsigned char laneIndex;
	RoadMapPos roadPos;
	MSLane* actLane;

    // NodeId
	int nodeId = requestMsg.readInt();
    MSVehicle* veh = getVehicleByExtId(nodeId);   // external node id (equipped vehicle number)

	if (veh == NULL) {
        writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "Can not retrieve node with given ID");
        return;
    }

    // StopPosition
    unsigned char posType = requestMsg.readUnsignedByte();	// position type
	switch (posType) {
	case POSITION_ROADMAP:
		// read road map position
		roadPos.roadId = requestMsg.readString();
		roadPos.pos = requestMsg.readFloat();
		roadPos.laneId = requestMsg.readUnsignedByte();
		break;
	case POSITION_2D:
	case POSITION_3D:
		// convert other position type to road map position
		roadPos = convertCartesianToRoadMap(Position2D(requestMsg.readFloat(),
													requestMsg.readFloat()));
		if (posType == POSITION_3D) {
			requestMsg.readFloat();	// z value is ignored
		}
		break;
	default:
		writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "Not supported or unknown Position Format");
		return;
	}

	// Radius
    float radius = requestMsg.readFloat();
    // waitTime
    double waitTime = requestMsg.readDouble();
  
	if (roadPos.pos < 0) {
        writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "Position on lane must not be negative");
		return;
    }

	// get the actual lane that is referenced by laneIndex
	MSEdge* road = MSEdge::dictionary(roadPos.roadId);
    if (road == NULL) {
        writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "Unable to retrieve road with given id");
		return;
    }

    const MSEdge::LaneCont* const allLanes = road->getLanes();
	if (roadPos.laneId >= allLanes->size()) {
        writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "No lane existing with such id on the given road");
		return;
    }

	actLane = (*allLanes)[0];
	int index = 0;
	while (road->rightLane(actLane) != NULL) {
		actLane = road->rightLane(actLane);
		index++;
	}
	actLane = (*allLanes)[0];
	if (index < roadPos.laneId) {
		for (int i=0; i < (roadPos.laneId - index); i++) {
			actLane = road->leftLane(actLane);
		}
	} else {
		for (int i=0; i < (index - roadPos.laneId); i++) {
			actLane = road->rightLane(actLane);
		}
	}

    // Forward command to vehicle
	veh->addTraciStop(/*MSEdge::dictionary(roadPos.roadId),*/ actLane, roadPos.pos, radius, waitTime);

    // create a reply message
    writeStatusCmd(respMsg, CMD_STOP, RTYPE_OK, "");
	// add a stopnode command containging the actually used road map position to the reply
	int length = 1 + 1 + 4 + 1 + (4+roadPos.roadId.length()) + 4 + 1 + 4 + 8;
	respMsg.writeUnsignedByte(length);				// lenght
	respMsg.writeUnsignedByte(CMD_STOP);			// command id
	respMsg.writeInt(nodeId);						// node id
	respMsg.writeUnsignedByte(POSITION_ROADMAP);	// pos format
	respMsg.writeString(roadPos.roadId);					// road id
	respMsg.writeFloat(roadPos.pos);					// pos
	respMsg.writeUnsignedByte(roadPos.laneId);			// lane id
	respMsg.writeFloat(radius);						// radius
	respMsg.writeDouble(waitTime);					// wait time

    return;
}

/*****************************************************************************/
void
TraCIServer::commandChangeLane(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException, std::invalid_argument)
{
    // NodeId
    MSVehicle* veh = getVehicleByExtId(requestMsg.readInt());   // external node id (equipped vehicle number)
    // Lane ID
    char laneIndex = requestMsg.readByte();
    // stickyTime
    float stickyTime = requestMsg.readFloat();

    if (veh == NULL) {
        writeStatusCmd(respMsg, CMD_CHANGELANE, RTYPE_ERR, "Can not retrieve node with given ID");
        return;
    }

	/*const MSEdge* const road = veh->getEdge();
    const MSEdge::LaneCont* const allLanes = road->getLanes();*/
	if ((laneIndex < 0) || (laneIndex >= veh->getEdge()->getLanes()->size())) {
        writeStatusCmd(respMsg, CMD_CHANGELANE, RTYPE_ERR, "No lane existing with given id on the current road");
    }
	
	// Forward command to vehicle
	veh->startLaneChange(static_cast<int>(laneIndex), static_cast<SUMOTime>(stickyTime));

    // create a reply message
    writeStatusCmd(respMsg, CMD_CHANGELANE, RTYPE_OK, "");

    return;
}

/*****************************************************************************/
void
TraCIServer::commandChangeRoute(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException, std::invalid_argument)
{
    // NodeId
    int vehId = requestMsg.readInt();
    MSVehicle* veh = getVehicleByExtId(vehId);   // external node id (equipped vehicle number)
    // edgeID
    std::string edgeID = requestMsg.readString();
    // travelTime
    double travelTime = requestMsg.readDouble();

    if (veh == NULL) {
        std::ostringstream os;
        os << "Can not retrieve node with ID " << vehId;
        writeStatusCmd(respMsg, CMD_CHANGEROUTE, RTYPE_ERR, os.str());
        return;
    }

    if (travelTime < 0) {
        //restore last travel time
        veh->restoreEdgeWeightLocally(edgeID, MSNet::getInstance()->getCurrentTimeStep());
    } else {
        // change edge weight for the vehicle
        bool result = veh->changeEdgeWeightLocally(edgeID, travelTime,
                      MSNet::getInstance()->getCurrentTimeStep());
        if (!result) {
            writeStatusCmd(respMsg, CMD_CHANGEROUTE, RTYPE_ERR, "Could not set new travel time properly");
            return;
        }
    }

    // create a reply message
    writeStatusCmd(respMsg, CMD_CHANGEROUTE, RTYPE_OK, "");

    return;
}

/*****************************************************************************/
void
TraCIServer::commandChangeTarget(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException, std::invalid_argument)
{
    // NodeId
    MSVehicle* veh = getVehicleByExtId(requestMsg.readInt());   // external node id (equipped vehicle number)
    // EdgeId
    std::string edgeID = requestMsg.readString();

    // destination edge
    const MSEdge* destEdge = MSEdge::dictionary(edgeID);

    if (veh == NULL) {
        writeStatusCmd(respMsg, CMD_CHANGETARGET, RTYPE_ERR, "Can not retrieve node with given ID");
        return;
    }

    if (destEdge == NULL) {
        writeStatusCmd(respMsg, CMD_CHANGETARGET, RTYPE_ERR, "Can not retrieve road with ID " + edgeID);
        return;
    }

    // build a new route between the vehicle's current edge and destination edge
    //std::vector<const MSEdge*> newRoute;
    MSEdgeVector newRoute;
    const MSEdge* currentEdge = veh->getEdge();
    SUMODijkstraRouter_Direct<MSEdge, MSVehicle, prohibited_withRestrictions<MSEdge, MSVehicle> > router(MSEdge::dictSize(), true, &MSEdge::getVehicleEffort);
    router.compute(currentEdge, destEdge, (const MSVehicle* const) veh,
                   MSNet::getInstance()->getCurrentTimeStep(), newRoute);

    // replace the vehicle's route by the new one
    veh->replaceRoute(newRoute, MSNet::getInstance()->getCurrentTimeStep());

    // create a reply message
    writeStatusCmd(respMsg, CMD_CHANGETARGET, RTYPE_OK, "");

    return;
}

/*****************************************************************************/
void
TraCIServer::commandGetAllTLIds(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
    tcpip::Storage tempMsg;

    // get the TLLogicControl
    MSTLLogicControl &tlsControl = MSNet::getInstance()->getTLSControl();
    // get the ids
    std::vector<std::string> idList = tlsControl.getAllTLIds();

    if (idList.size() == 0) {
        // create negative response message
        writeStatusCmd(respMsg, CMD_GETALLTLIDS, RTYPE_ERR, "Could not retrieve any traffic light id");
        return;
    }

    // create positive response message
    writeStatusCmd(respMsg, CMD_GETALLTLIDS, RTYPE_OK, "");

    // create a response command for each string id
    for (std::vector<std::string>::iterator iter = idList.begin(); iter != idList.end(); iter++) {
        // command length
        respMsg.writeByte(2 + (4 + (*iter).size()));
        // command type
        respMsg.writeByte(CMD_TLIDLIST);
        // id string
        respMsg.writeString((*iter));
    }
}

/*****************************************************************************/
void
TraCIServer::commandGetTLStatus(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
    SUMOTime lookback = 60; // Time to look in history for recognizing yellowTimes

    tcpip::Storage tempMsg;

    // trafic light id
    int extId = requestMsg.readInt();
    // start of time interval
    double timeFrom = requestMsg.readDouble();
    // end of time interval
    double timeTo = requestMsg.readDouble();

    // get the running programm of the traffic light
    MSTrafficLightLogic* const tlLogic = getTLLogicByExtId(extId);
    
    // error checking
    if (tlLogic == NULL) {
        writeStatusCmd(respMsg, CMD_GETTLSTATUS, RTYPE_ERR, "Could not retrieve traffic light with given id");
        return;
    }
    if ((timeTo < timeFrom) || (timeTo < 0) || (timeFrom < 0)) {
        writeStatusCmd(respMsg, CMD_GETTLSTATUS, RTYPE_ERR, "The given time interval is not valid");
        return;
    }

    // acknowledge the request
    writeStatusCmd(respMsg, CMD_GETTLSTATUS, RTYPE_OK, "");
    std::vector<MSLink::LinkState> linkStates;
    std::vector<double> yellowTimes;
    size_t lastStep = tlLogic->getStepNo();
    MSPhaseDefinition phase = tlLogic->getCurrentPhaseDef();
    MSTrafficLightLogic::LinkVectorVector affectedLinks = tlLogic->getLinks();

    // save the current link states
    for (int i = 0; i < affectedLinks.size(); i++) {
        linkStates.push_back(phase.getLinkState(i));
        yellowTimes.push_back(-1);
    }

    // check every second of the given time interval for a switch in the traffic light's phases
    for (SUMOTime time = static_cast<SUMOTime>(timeFrom) - lookback; time <= static_cast<SUMOTime>(timeTo); time++) {
        if (time < 0) time = 0;
        size_t position = tlLogic->getPosition(time);
        size_t currentStep = tlLogic->getStepFromPos(position);

        if (currentStep != lastStep) {
            lastStep = currentStep;
            phase = tlLogic->getPhaseFromStep(currentStep);

            // for every link of the tl's junction, compare the actual and the last red/green state
			// for each link with new red/green status, write a TLSWITCH command
			std::map<const MSEdge*, pair<const MSEdge*, int> > writtenEdgePairs;
            for (int i = 0; i < linkStates.size(); i++) {
                MSLink::LinkState nextLinkState = phase.getLinkState(i);

                if (nextLinkState == MSLink::LINKSTATE_TL_YELLOW) {
                    if (yellowTimes[i] < 0) yellowTimes[i] = time;
                } else {
                    if (nextLinkState != linkStates[i] && time >= timeFrom) {
                        linkStates[i] = nextLinkState;

                        // get the group of links that is affected by the changed light status
                        MSTrafficLightLogic::LinkVector linkGroup = affectedLinks[i];
                        // get the group of preceding lanes of the link group
                        MSTrafficLightLogic::LaneVector laneGroup = tlLogic->getLanesAt(i);

                        for (int j = 0; j < linkGroup.size(); j++) {
							const MSEdge* const precEdge = laneGroup[j]->getEdge();
							const MSEdge* const succEdge = linkGroup[j]->getLane()->getEdge();

							// for each pair of edges and every different tl state, write only one tl switch command
							std::map<const MSEdge*, pair<const MSEdge*, int> >::iterator itPair = writtenEdgePairs.find(precEdge);
							if (itPair != writtenEdgePairs.end()) {
								if (itPair->second.first == succEdge && itPair->second.second == nextLinkState) {
									continue;
								}
							}
							// remember the current edge pair and tl status
							writtenEdgePairs[precEdge] = make_pair(succEdge, nextLinkState);

                            // time of the switch
                            tempMsg.writeDouble(time);
                            // preceeding edge id
                            tempMsg.writeString(precEdge->getID());
                            // traffic light's position on preceeding edge
                            tempMsg.writeFloat(laneGroup[j]->getShape().length());
                            // succeeding edge id
                            tempMsg.writeString(succEdge->getID());
                            // new status
							switch (nextLinkState) {
							case MSLink::LINKSTATE_TL_GREEN:
								tempMsg.writeUnsignedByte(TLPHASE_GREEN);
								break;
							case MSLink::LINKSTATE_TL_RED:
								tempMsg.writeUnsignedByte(TLPHASE_RED);
								break;
							case MSLink::LINKSTATE_TL_OFF_BLINKING:
								tempMsg.writeUnsignedByte(TLPHASE_BLINKING);
								break;
							case MSLink::LINKSTATE_TL_OFF_NOSIGNAL:
								tempMsg.writeUnsignedByte(TLPHASE_NOSIGNAL);
								break;
							default:
								tempMsg.writeUnsignedByte(TLPHASE_NOSIGNAL);
							}
                            //yellow time
                            tempMsg.writeDouble(yellowTimes[i]<0 ? 0 : time - yellowTimes[i]);

							if (tempMsg.size() <= 253) {
								// command length
								respMsg.writeUnsignedByte(1 + 1 + tempMsg.size());
							} else {
								// command length extended
								respMsg.writeUnsignedByte(0);
								respMsg.writeInt(1 + 4 + 1 + tempMsg.size());
							}
							// command type
							respMsg.writeUnsignedByte(CMD_TLSWITCH);
							// command content
							respMsg.writeStorage(tempMsg);
                            tempMsg.reset();
                        }
                    }
                    yellowTimes[i] = -1;
                }
            }
        }
    }
}

/*****************************************************************************/

void
TraCIServer::commandSlowDown(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
    // NodeId
    MSVehicle* veh = getVehicleByExtId(requestMsg.readInt());   // external node id (equipped vehicle number)
    // speed
    float newSpeed = MAX2(requestMsg.readFloat(), 0.0f);
    // time interval
    double duration = requestMsg.readDouble();

    if (veh == NULL) {
        writeStatusCmd(respMsg, CMD_SLOWDOWN, RTYPE_ERR, "Can not retrieve node with given ID");
        return;
    }
    /*if (newSpeed < 0) {
        writeStatusCmd(respMsg, CMD_SLOWDOWN, RTYPE_ERR, "Negative speed value");
        return;
    }*/
    if (duration <= 0) {
        writeStatusCmd(respMsg, CMD_SLOWDOWN, RTYPE_ERR, "Invalid time interval");
        return;
    }

    if (!veh->startSpeedAdaption(newSpeed, static_cast<SUMOTime>(duration), MSNet::getInstance()->getCurrentTimeStep())) {
        writeStatusCmd(respMsg, CMD_SLOWDOWN, RTYPE_ERR, "Could not slow down");
        return;
    }

    // create positive response message
    writeStatusCmd(respMsg, CMD_SLOWDOWN, RTYPE_OK, "");
}

/*****************************************************************************/

void
TraCIServer::commandCloseConnection(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
    closeConnection_ = true;
    // write answer
    writeStatusCmd(respMsg, CMD_CLOSE, RTYPE_OK, "Goodbye");
}

/*****************************************************************************/

void
TraCIServer::commandSimulationParameter(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
    bool setParameter = (requestMsg.readByte() != 0);
    string parameter = requestMsg.readString();

    // Prepare response
    tcpip::Storage answerTmp;

    if (parameter.compare("maxX")) {
        if (setParameter) {
            writeStatusCmd(respMsg, CMD_SIMPARAMETER, RTYPE_ERR, "maxX is a read only parameter");
            return;
        } else {
            answerTmp.writeFloat(getNetBoundary().getWidth());
        }
    } else if (parameter.compare("maxY")) {
        if (setParameter) {
            writeStatusCmd(respMsg, CMD_SIMPARAMETER, RTYPE_ERR, "maxY is a read only parameter");
            return;
        } else {
            answerTmp.writeFloat(getNetBoundary().getHeight());
        }
    } else if (parameter.compare("numberOfNodes")) {
        if (setParameter) {
            writeStatusCmd(respMsg, CMD_SIMPARAMETER, RTYPE_ERR, "numberOfNodes is a read only parameter");
        } else {
            writeStatusCmd(respMsg, CMD_SIMPARAMETER, RTYPE_NOTIMPLEMENTED, "numberOfNodes not implemented yet");
            return;
            //answerTmp.writeInt( --- Don't know where to get that information ---);
        }
    } else if (parameter.compare("airDistance")) {
        MSVehicle* veh1 = getVehicleByExtId(requestMsg.readInt());   // external node id (equipped vehicle number)
        MSVehicle* veh2 = getVehicleByExtId(requestMsg.readInt());   // external node id (equipped vehicle number)

        if (veh1 != NULL && veh2 != NULL) {
            if (setParameter) {
                writeStatusCmd(respMsg, CMD_SIMPARAMETER, RTYPE_ERR, "airDistance is a read only parameter");
                return;
            } else {
                float dx = veh1->getPosition().x() - veh2->getPosition().x();
                float dy = veh1->getPosition().y() - veh2->getPosition().y();
                answerTmp.writeFloat(sqrt(dx * dx + dy * dy));
            }
        } else {
            writeStatusCmd(respMsg, CMD_SIMPARAMETER, RTYPE_ERR, "Can not retrieve node with given ID");
            return;
        }
    } else if (parameter.compare("drivingDistance")) {
        MSVehicle* veh1 = getVehicleByExtId(requestMsg.readInt());   // external node id (equipped vehicle number)
        MSVehicle* veh2 = getVehicleByExtId(requestMsg.readInt());   // external node id (equipped vehicle number)

        if (veh1 != NULL && veh2 != NULL) {
            if (setParameter) {
                writeStatusCmd(respMsg, CMD_SIMPARAMETER, RTYPE_ERR, "airDistance is a read only parameter");
                return;
            } else {
                writeStatusCmd(respMsg, CMD_SIMPARAMETER, RTYPE_NOTIMPLEMENTED, "drivingDistance not implemented yet");
                return;
                //float dx = veh1->getPosition().x() - veh2->getPosition().x();
                //float dy = veh1->getPosition().y() - veh2->getPosition().y();
                //float distance = sqrt( dx * dx + dy * dy );
                // answerTmp.writeFloat( distance );
            }
        } else {
            writeStatusCmd(respMsg, CMD_SIMPARAMETER, RTYPE_ERR, "Can not retrieve node with given ID");
            return;
        }
    }

    // When we get here, the response is stored in answerTmp -> put into respMsg
    writeStatusCmd(respMsg, CMD_SIMPARAMETER, RTYPE_OK, "");

    // command length
    respMsg.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(parameter.length()) + answerTmp.size());
    // command type
    respMsg.writeUnsignedByte(CMD_SIMPARAMETER);
    // answer only to getParameter commands as setParameter
    respMsg.writeUnsignedByte(1);
    // Parameter
    respMsg.writeString(parameter);
    // and the parameter dependant part
    respMsg.writeStorage(answerTmp);
}

/*****************************************************************************/

void
TraCIServer::commandUpdateCalibrator(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
    respMsg.reset();

    int countTime = requestMsg.readInt();
    int vehicleCount = requestMsg.readInt();
    std::string calibratorId = requestMsg.readString();

    MSCalibrator::updateCalibrator(calibratorId, countTime, vehicleCount);

    //@TODO write response according to result of updateCalibrator

    return;
}

/*****************************************************************************/

void
TraCIServer::commandPositionConversion(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) 
throw(TraCIException)
{
	tcpip::Storage tmpResult;
	RoadMapPos roadPos;
	Position2D cartesianPos;
	float x = 0;
	float y = 0;
	float z = 0;
	unsigned char destPosType;

	static std::stringstream out;
	std::ofstream file;

	// actual position type that will be converted
	unsigned char srcPosType = requestMsg.readUnsignedByte();

	switch (srcPosType) {
	case POSITION_2D:
	case POSITION_3D:
		x = requestMsg.readFloat();
		y = requestMsg.readFloat();
		if (srcPosType == POSITION_3D) {
			z = requestMsg.readFloat();
		}
		// destination position type
		destPosType = requestMsg.readUnsignedByte();

		switch (destPosType) {
		case POSITION_ROADMAP:
			// convert road map to 3D position
			roadPos = convertCartesianToRoadMap(Position2D(x, y));

			/*out << "TraCI: position conversion from: x=" << x << " y=" << y 
				<< " to: roadId=" << roadPos.roadId << " pos=" << roadPos.pos 
				<< " laneId =" << (int)roadPos.laneId << std::endl;
			std::cerr << out.str();
			file.open("pos_convert_log.txt");
			file.write(out.str().c_str(), out.str().size());
			file.close();*/

			// write result that is added to response msg
			tmpResult.writeUnsignedByte(POSITION_ROADMAP);	
			tmpResult.writeString(roadPos.roadId);	
			tmpResult.writeFloat(roadPos.pos);
			tmpResult.writeUnsignedByte(roadPos.laneId);
			break;	
		case POSITION_3D:
			writeStatusCmd(respMsg, CMD_POSITIONCONVERSION, RTYPE_ERR, 
						"Destination position type is same as source position type");
			return;
		default:
			writeStatusCmd(respMsg, CMD_POSITIONCONVERSION, RTYPE_ERR, 
							"Destination position type not supported");
			return;
		}
		break;
	case POSITION_ROADMAP:	
		roadPos.roadId = requestMsg.readString();
		roadPos.pos = requestMsg.readFloat();
		roadPos.laneId = requestMsg.readUnsignedByte();

		// destination position type
		destPosType = requestMsg.readUnsignedByte();

		switch (destPosType) {
		case POSITION_3D:
			//convert 3D to road map position
			try {
				Position2D result = convertRoadMapToCartesian(roadPos);
				x = result.x();
				y = result.y();

				/*out << "TraCI: position conversion from: roadId=" << roadPos.roadId << " pos=" 
					<< roadPos.pos << " laneId= " << (int)roadPos.laneId 
					<< " to: x=" << x << " y=" << y << std::endl;
				std::cerr << out.str();
				file.open("pos_convert_log.txt");
				file.write(out.str().c_str(), out.str().size());
				file.close();*/
			} catch (TraCIException e) {
				writeStatusCmd(respMsg, CMD_POSITIONCONVERSION, RTYPE_ERR, e.what());
			}
			
			// write result that is added to response msg
			tmpResult.writeUnsignedByte(POSITION_3D);	
			tmpResult.writeFloat(x);	
			tmpResult.writeFloat(y);
			tmpResult.writeFloat(z);
			break;
		case POSITION_ROADMAP:
			writeStatusCmd(respMsg, CMD_POSITIONCONVERSION, RTYPE_ERR, 
						"Destination position type is same as source position type");
			return;
		default:
			writeStatusCmd(respMsg, CMD_POSITIONCONVERSION, RTYPE_ERR, 
						"Destination position type not supported");
			return;
		}
		break;
	default:
		writeStatusCmd(respMsg, CMD_POSITIONCONVERSION, RTYPE_ERR, 
					"Source position type not supported");
		return;
	}

	// write response message
	writeStatusCmd(respMsg, CMD_POSITIONCONVERSION, RTYPE_OK, "");	
	// add converted Position to response
	respMsg.writeUnsignedByte(1 + 1 + tmpResult.size() + 1);	// length
	respMsg.writeUnsignedByte(CMD_POSITIONCONVERSION);	// command id
	respMsg.writeStorage(tmpResult);	// position dependant part
	respMsg.writeUnsignedByte(destPosType);	// destination type
}

/*****************************************************************************/

void
TraCIServer::commandScenario(tcpip::Storage &requestMsg, tcpip::Storage &respMsg) 
throw(TraCIException)
{
	Storage tmpResult;
	string warning = "";	// additional description for response

	// read/write flag
	bool isWriteCommand = requestMsg.readUnsignedByte();

	// domain
	int domain = requestMsg.readUnsignedByte();

	switch (domain) {
	// road map domain
	case DOM_ROADMAP:
		try {
			warning = handleRoadMapDomain(isWriteCommand, requestMsg, tmpResult);
		} catch (TraCIException e) {
			writeStatusCmd(respMsg, CMD_SCENARIO, RTYPE_ERR, e.what());
			return;
		}
		break;

	// vehicle domain
	case DOM_VEHICLE:
		try {
			warning = handleVehicleDomain(isWriteCommand, requestMsg, tmpResult);
		} catch (TraCIException e) {
			writeStatusCmd(respMsg, CMD_SCENARIO, RTYPE_ERR, e.what());
			return;
		}
		break;

	// traffic light domain
	case DOM_TRAFFICLIGHTS:
		try {
			warning = handleTrafficLightDomain(isWriteCommand, requestMsg, tmpResult);
		} catch (TraCIException e) {
			writeStatusCmd(respMsg, CMD_SCENARIO, RTYPE_ERR, e.what());
			return;
		}
		break;

	// point of interest domain
	case DOM_POI:
		try {
			warning = handlePoiDomain(isWriteCommand, requestMsg, tmpResult);
		} catch (TraCIException e) {
			writeStatusCmd(respMsg, CMD_SCENARIO, RTYPE_ERR, e.what());
			return;
		}
		break;

	// polygon domain
	case DOM_POLYGON:
		try {
			warning = handlePolygonDomain(isWriteCommand, requestMsg, tmpResult);
		} catch (TraCIException e) {
			writeStatusCmd(respMsg, CMD_SCENARIO, RTYPE_ERR, e.what());
			return;
		}
		break;

	// unknown domain
	default:
		writeStatusCmd(respMsg, CMD_SCENARIO, RTYPE_ERR, "Unknown domain specified");
		return;
	}

	// write response message
	writeStatusCmd(respMsg, CMD_SCENARIO, RTYPE_OK, warning);
	// if necessary, add Scenario command containing the read value
	if (!isWriteCommand) {
        if (tmpResult.size() <= 253 ) 
        {
            respMsg.writeUnsignedByte(1 + 1 + tmpResult.size());	// command length
		    respMsg.writeUnsignedByte(CMD_SCENARIO);	// command id
		    respMsg.writeStorage(tmpResult);	// variable dependant part
        } else {
            respMsg.writeUnsignedByte(0);	// command length -> extended
            respMsg.writeInt(1 + 4 + 1 + tmpResult.size());
            respMsg.writeUnsignedByte(CMD_SCENARIO);	// command id
		    respMsg.writeStorage(tmpResult);	// variable dependant part
        }
	}
}

/*****************************************************************************/

void 
TraCIServer::commandDistanceRequest(tcpip::Storage& requestMsg, tcpip::Storage& respMsg) 
throw(TraCIException)
{
	Position2D pos1;
	Position2D pos2;
	RoadMapPos roadPos1;
	RoadMapPos roadPos2;
	const MSEdge::LaneCont* lanes;

	// read position 1
	int posType = requestMsg.readUnsignedByte();
	switch (posType) {
	case POSITION_ROADMAP:
		roadPos1.roadId = requestMsg.readString();
		roadPos1.pos = requestMsg.readFloat();
		roadPos1.laneId = requestMsg.readUnsignedByte();
		try {
			pos1 = convertRoadMapToCartesian(roadPos1);
		} catch(TraCIException e) {
			writeStatusCmd(respMsg, CMD_DISTANCEREQUEST, RTYPE_ERR, e.what());
			return;
		}
		break;
	case POSITION_2D:
	case POSITION_2_5D:
	case POSITION_3D:
		pos1.set(requestMsg.readFloat(), requestMsg.readFloat());
		if ((posType == POSITION_2_5D) || (posType == POSITION_3D)) {
			requestMsg.readFloat();		// z value is ignored
		}
		roadPos1 = convertCartesianToRoadMap(pos1);
		break;
	default:
		writeStatusCmd(respMsg, CMD_DISTANCEREQUEST, RTYPE_ERR, "Unknown position format used for distance request");
		return;
	}

	// read position 2
	posType = requestMsg.readUnsignedByte();
	switch (posType) {
	case POSITION_ROADMAP:
		roadPos2.roadId = requestMsg.readString();
		roadPos2.pos = requestMsg.readFloat();
		roadPos2.laneId = requestMsg.readUnsignedByte();
		try {
			pos2 = convertRoadMapToCartesian(roadPos2);
		} catch(TraCIException e) {
			writeStatusCmd(respMsg, CMD_DISTANCEREQUEST, RTYPE_ERR, e.what());
			return;
		}
		break;
	case POSITION_2D:
	case POSITION_2_5D:
	case POSITION_3D:
		pos2.set(requestMsg.readFloat(), requestMsg.readFloat());
		if ((posType == POSITION_2_5D) || (posType == POSITION_3D)) {
			requestMsg.readFloat();		// z value is ignored
		}
		roadPos2 = convertCartesianToRoadMap(pos2);
		break;
	default:
		writeStatusCmd(respMsg, CMD_DISTANCEREQUEST, RTYPE_ERR, "Unknown position format used for distance request");
		return;
	}

	// read distance type
	int distType = requestMsg.readUnsignedByte();
	
	float distance = 0.0;
	if (distType == REQUEST_DRIVINGDIST) {
		// compute driving distance
		std::vector<const MSEdge*> edges;
		TraCIDijkstraRouter<MSEdge> router(MSEdge::dictSize());

		if ( (roadPos1.roadId.compare(roadPos2.roadId) == 0) 
			&& (roadPos1.pos <= roadPos2.pos) ) {
			distance = roadPos2.pos - roadPos1.pos;
		} else {
			router.compute(MSEdge::dictionary(roadPos1.roadId), MSEdge::dictionary(roadPos2.roadId), NULL,
				MSNet::getInstance()->getCurrentTimeStep(), edges);
//			cerr << "route: ";
			for (MSRouteIterator it=edges.begin(); it != edges.end(); it++) {
//				cerr << " " << (*it)->getID();
			}
			cerr << endl;
			MSRoute route("", edges, false);
			distance = static_cast<float>(route.getDistanceBetween(roadPos1.pos, roadPos2.pos,
				MSEdge::dictionary(roadPos1.roadId), MSEdge::dictionary(roadPos2.roadId)));
			/*if (edges.size() != 0) {
				distance = -roadPos1.pos;
				for (std::vector<const MSEdge*>::iterator edge=edges.begin(); edge!=edges.end(); edge++) {
					lanes = (*edge)->getLanes();
					distance += (*lanes)[0]->getShape().length();
				}
				lanes = MSEdge::dictionary(roadPos2.roadId)->getLanes();
				distance -= ((*lanes)[0]->getShape().length() - roadPos2.pos);
			}*/
		}
	} else {
		// compute air distance (default)
		// correct the distance type in case it was not valid
		distType = REQUEST_AIRDIST;
		distance = static_cast<float>(GeomHelper::distance(pos1, pos2));
	}

	// acknowledge distance request
	writeStatusCmd(respMsg, CMD_DISTANCEREQUEST, RTYPE_OK, "");
	// write response command
	respMsg.writeUnsignedByte(1 + 1 + 1 + 4);	// length
	respMsg.writeUnsignedByte(CMD_DISTANCEREQUEST);		// command type
	respMsg.writeUnsignedByte(distType);		// distance type
	respMsg.writeFloat(distance);	// distance;
}

/*****************************************************************************/

void
TraCIServer::commandSubscribeLifecycles(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
	// domain
	int domain = requestMsg.readUnsignedByte();

	if (domain != DOM_VEHICLE) {
		// send negative command response
		writeStatusCmd(respMsg, CMD_SUBSCRIBELIFECYCLES, RTYPE_NOTIMPLEMENTED, "Can only subscribe to lifecycle of DOM_VEHICLE at this time");
		return;
	}

	myLifecycleSubscriptions.insert(domain);

	if (domain == DOM_VEHICLE) {
		myCreatedVehicles.clear();
		myDestroyedVehicles.clear();
	}

	// send positive command response
	writeStatusCmd(respMsg, CMD_SUBSCRIBELIFECYCLES, RTYPE_OK, "");
}

/*****************************************************************************/

void
TraCIServer::commandUnsubscribeLifecycles(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
	// domain
	int domain = requestMsg.readUnsignedByte();

	if (domain != DOM_VEHICLE) {
		// send negative command response
		writeStatusCmd(respMsg, CMD_UNSUBSCRIBELIFECYCLES, RTYPE_NOTIMPLEMENTED, "Can only subscribe to lifecycle of DOM_VEHICLE at this time");
		return;
	}

	myLifecycleSubscriptions.erase(domain);

	if (domain == DOM_VEHICLE) {
		myCreatedVehicles.clear();
		myDestroyedVehicles.clear();
	}

	// send positive command response
	writeStatusCmd(respMsg, CMD_UNSUBSCRIBELIFECYCLES, RTYPE_OK, "");
}

/*****************************************************************************/

void
TraCIServer::commandSubscribeDomain(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
	// domain
	int domainId = requestMsg.readUnsignedByte();

	if (domainId != DOM_VEHICLE) {
		// send negative command response
		writeStatusCmd(respMsg, CMD_SUBSCRIBEDOMAIN, RTYPE_NOTIMPLEMENTED, "Can only subscribe to DOM_VEHICLE at this time");
		return;
	}

    // variable count
    int variableCount = requestMsg.readUnsignedByte();

    // list of variable ids and types
    std::list<std::pair<int, int> > subscribedVariables;
    for (int i = 0; i < variableCount; i++) {
		// variable id
		int variableId = requestMsg.readUnsignedByte();

		// value data type
		int dataType = requestMsg.readUnsignedByte();

		if (!( 
				((domainId == DOM_VEHICLE) && (variableId == DOMVAR_SIMTIME) && (dataType == TYPE_DOUBLE))
				||
				((domainId == DOM_VEHICLE) && (variableId == DOMVAR_SPEED) && (dataType == TYPE_FLOAT))
				||
				((domainId == DOM_VEHICLE) && (variableId == DOMVAR_POSITION) && (dataType == POSITION_2D))
				||
				((domainId == DOM_VEHICLE) && (variableId == DOMVAR_POSITION) && (dataType == POSITION_ROADMAP))
				||
				((domainId == DOM_VEHICLE) && (variableId == DOMVAR_ANGLE) && (dataType == TYPE_FLOAT))
				)) {
			// send negative command response
			writeStatusCmd(respMsg, CMD_SUBSCRIBEDOMAIN, RTYPE_NOTIMPLEMENTED, "Can not subscribe to this domain/variable/type combination");
			return;
		}

		subscribedVariables.push_back(std::pair<int, int>(variableId, dataType));
    }

	// store subscription
	myDomainSubscriptions[domainId] = subscribedVariables;

	// send positive command response
	writeStatusCmd(respMsg, CMD_SUBSCRIBEDOMAIN, RTYPE_OK, "");
}

/*****************************************************************************/

void
TraCIServer::commandUnsubscribeDomain(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
	// domain
	int domain = requestMsg.readUnsignedByte();

	if (domain != DOM_VEHICLE) {
		// send negative command response
		writeStatusCmd(respMsg, CMD_UNSUBSCRIBEDOMAIN, RTYPE_NOTIMPLEMENTED, "Can only subscribe to lifecycle of DOM_VEHICLE at this time");
		return;
	}

	myDomainSubscriptions.erase(domain);

	// send positive command response
	writeStatusCmd(respMsg, CMD_UNSUBSCRIBEDOMAIN, RTYPE_OK, "");
}

/*****************************************************************************/

void
TraCIServer::writeStatusCmd(tcpip::Storage& respMsg, int commandId, int status, std::string description)
{
    if (status == RTYPE_ERR) {
        closeConnection_ = true;
        cerr << "Answered with error to command " << (int)commandId
        << ": " << description << endl;
    } else if (status == RTYPE_NOTIMPLEMENTED) {
        cerr << "Requested command not implemented (" << (int)commandId
        << "): " << description << endl;
    }

    // command length
    respMsg.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(description.length()));
    // command type
    respMsg.writeUnsignedByte(commandId);
    // status
    respMsg.writeUnsignedByte(status);
    // description
    respMsg.writeString(description);

    return;
}

/*****************************************************************************/

void
TraCIServer::convertExt2IntId(int extId, std::string& intId)
{
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
TraCIServer::getVehicleByExtId(int extId)
{
    std::string intId;
    convertExt2IntId(extId, intId);
    return MSNet::getInstance()->getVehicleControl().getVehicle(intId);
}

/*****************************************************************************/

MSTrafficLightLogic*
TraCIServer::getTLLogicByExtId(int extId) 
{
	std::string intId = "";
	std::map<int, std::string>::iterator iter = trafficLightsExt2IntId.find(extId);
	if (iter != trafficLightsExt2IntId.end()) {
		intId = iter->second;
	}

	return MSNet::getInstance()->getTLSControl().getActive(intId);
}

/*****************************************************************************/

PointOfInterest*
TraCIServer::getPoiByExtId(int extId)
{
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
TraCIServer::getPolygonByExtId(int extId)
{
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
TraCIServer::getNetBoundary()
{
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
    MSEdgeControl& edges = MSNet::getInstance()->getEdgeControl();

    // Get Boundary of Single ...
    for (MSEdgeControl::EdgeCont::const_iterator edgeIt = edges.getSingleLaneEdges().begin();
            edgeIt != edges.getSingleLaneEdges().end(); ++edgeIt) {
        for (MSEdge::LaneCont::const_iterator laneIt = (*edgeIt)->getLanes()->begin();
                laneIt != (*edgeIt)->getLanes()->end(); ++laneIt) {
            netBoundary_->add((*laneIt)->getShape().getBoxBoundary());
        }
    }

    // ... and MultiLaneEdges
    for (MSEdgeControl::EdgeCont::const_iterator edgeIt = edges.getMultiLaneEdges().begin();
            edgeIt != edges.getMultiLaneEdges().end(); ++edgeIt) {
        for (MSEdge::LaneCont::const_iterator laneIt = (*edgeIt)->getLanes()->begin();
                laneIt != (*edgeIt)->getLanes()->end(); ++laneIt) {
            netBoundary_->add((*laneIt)->getShape().getBoxBoundary());
        }
    }

    // make the boundary slightly larger
    netBoundary_->grow(0.1);

    return *netBoundary_;
}

/*****************************************************************************/

TraCIServer::RoadMapPos
TraCIServer::convertCartesianToRoadMap(Position2D pos) 
{
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
		const MSEdge::LaneCont * const allLanes = edge->getLanes();

//		cerr << "--------" << endl << "Checking edge " << edge->getID() << endl << "--------" << endl;

		// iterate through all lanes of this edge
		for (MSEdge::LaneCont::const_iterator itLane = allLanes->begin(); itLane != allLanes->end(); itLane++) {
			Position2DVector shape = (*itLane)->getShape();

//			cerr << "### Lane: " << (*itLane)->getID() << endl;
			
			// iterate through all segments of this lane's shape
			for (int i = 0; i < shape.size()-1; i++) {
				lineStart = shape[i];
				lineEnd = shape[i+1];

//				cerr << "-Segment " << i << "(" << shape[i] << " - " << shape[i+1] << "): ";

				// if this line is no candidate for lying closer to the cartesian position 
				// than the line determined so far, skip it
				if ( (lineStart.y() > (pos.y()+minDistance) && lineEnd.y() > (pos.y()+minDistance))
					|| (lineStart.y() < (pos.y()-minDistance) && lineEnd.y() < (pos.y()-minDistance))
					|| (lineStart.x() > (pos.x()+minDistance) && lineEnd.x() > (pos.x()+minDistance))
					|| (lineStart.x() < (pos.x()-minDistance) && lineEnd.x() < (pos.x()-minDistance)) ) {

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
						while ( (tmpLane =tmpLane->getRightLane()) != NULL) {
							result.laneId++;
						}
						result.pos = GeomHelper::distance(lineStart, intersection);
						for (int j = 0; j < i; j++) {
							result.pos += GeomHelper::distance(shape[j], shape[j+1]);
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
throw(TraCIException)
{
	if (roadPos.pos < 0) {
		throw TraCIException("Position on lane must not be negative");
	}

	// get the edge and lane of this road map position
	MSEdge* road = MSEdge::dictionary(roadPos.roadId);
    if (road == NULL) {
		throw TraCIException("Unable to retrieve road with given id");
    }

    const MSEdge::LaneCont* const allLanes = road->getLanes();
	if ((roadPos.laneId >= allLanes->size()) || (allLanes->size() == 0)) {
		throw TraCIException("No lane existing with such id on the given road");
    }

	// get corresponding x and y coordinates
	Position2DVector shape = (*allLanes)[roadPos.laneId]->getShape();
	return shape.positionAtLengthPosition(roadPos.pos);
}

/*****************************************************************************/

std::string
TraCIServer::handleRoadMapDomain(bool isWriteCommand, tcpip::Storage& requestMsg, tcpip::Storage& response) 
throw(TraCIException)
{
	string name = "";
	string warning = "";	// additional description for response
	DataTypeContainer dataCont;

	// domain object
	int objectId = requestMsg.readInt();
	// check for valid object id
	if (objectId < 0 || objectId >= MSEdge::dictSize()) {
		throw TraCIException("Invalid object id specified");
	}
	MSEdge* edge = MSEdge::dictionary(objectId);

	// variable id
	int variableId = requestMsg.readUnsignedByte();

	// value data type
	int dataType = requestMsg.readUnsignedByte();

	// if end of message is not yet reached, the value parameter has to be read
	if (requestMsg.valid_pos()) {
		dataCont.readValue(dataType, requestMsg);
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
	// name string of the object
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
		//name = requestMsg.readString();
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
		response.writeFloat(0.0);
		response.writeFloat(0.0);
		response.writeFloat(getNetBoundary().getWidth());
		response.writeFloat(getNetBoundary().getHeight());
		// add a warning to the response if the requested data type was not correct
		if (dataType != TYPE_BOUNDINGBOX) {
			warning = "Warning: requested data type could not be used; using boundary box type instead!";
		}
		break;

	// number of roads
	case DOMVAR_COUNT:
		response.writeUnsignedByte(TYPE_INTEGER);
		response.writeInt(MSNet::getInstance()->getEdgeControl().getEdgeNames().size());
		if (dataType != TYPE_INTEGER) {
			warning = "Warning: requested data type could not be used; using integer instead!";
		}
		break;

	// shape of a road
	case DOMVAR_SHAPE:
		if (edge != NULL) {			
			const MSEdge::LaneCont* lanes = edge->getLanes();
			const Position2DVector shape = (*lanes)[(*lanes).size()/2]->getShape();
			response.writeUnsignedByte(TYPE_POLYGON);
			response.writeUnsignedByte(MIN2(static_cast<size_t>(255),shape.size()));
			for (int iPoint=0; iPoint < MIN2(static_cast<size_t>(255),shape.size()); iPoint++) {
				response.writeFloat(shape[iPoint].x());
				response.writeFloat(shape[iPoint].y());
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
TraCIServer::handleVehicleDomain(bool isWriteCommand, tcpip::Storage& requestMsg, tcpip::Storage& response) 
throw(TraCIException)
{
	std::string name;
	int count = 0;
	MSVehicleControl* vehControl = NULL;
	std::string warning = "";	// additional description for response
	DataTypeContainer dataCont;

	// domain object
	int objectId = requestMsg.readInt();
	MSVehicle* veh = getVehicleByExtId(objectId);	//get node by id

	// variable id
	int variableId = requestMsg.readUnsignedByte();

	// value data type
	int dataType = requestMsg.readUnsignedByte();

	// if end of message is not yet reached, the value parameter has to be read
	if (requestMsg.valid_pos()) {
		dataCont.readValue(dataType, requestMsg);
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
	// name string of the object
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
//		name = requestMsg.readString();
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
			MSVehicle* veh = MSNet::getInstance()->getVehicleControl().getVehicle(it->first);
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
				response.writeFloat(veh->getPosition().x());
				response.writeFloat(veh->getPosition().y());
				response.writeFloat(0);
				break;
			default:
				response.writeByte(POSITION_ROADMAP);
				response.writeString(veh->getEdge()->getID());
				response.writeFloat(veh->getPositionOnLane());
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
			response.writeFloat(veh->getSpeed());
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
        if (veh != NULL)
        {
			response.writeUnsignedByte(TYPE_STRING);
			MSRoute r = veh->getRoute();
            string strRoute = "";
            for (MSRouteIterator it = r.begin(); it != r.end(); ++it)
            {
                if (strRoute.length()) strRoute.append(" ");
                strRoute.append( (*it)->getID() );
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
        if (veh != NULL)
        {
			Position2D destPos;
			RoadMapPos destRoadPos;
			float distance = 0.0;

			// read destinatin position
			float x,y;
			switch(dataCont.getLastValueRead()) {
			case POSITION_ROADMAP:
				destRoadPos = dataCont.getRoadMapPosition();
				destPos = convertRoadMapToCartesian(destRoadPos);
				if (MSEdge::dictionary(destRoadPos.roadId) == NULL ) throw TraCIException("Unable to retrieve edge with given id");
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
//                    destEdge = requestMsg.readString();
//                    destPos = requestMsg.readFloat();
//                    destLane = requestMsg.readUnsignedByte();
////                    cerr << " destEdge=" << destEdge << " destPos=" << destPos << " destLane=" << destLane << endl;
//                    edge = MSEdge::dictionary( destEdge );
//                    if (edge == NULL ) throw TraCIException("Unable to retrieve edge with given id");
//
//					const MSEdge::LaneCont& lanes = *(edge->getLanes());
//					if (destLane > lanes.size()-1) throw TraCIException("No lane existing with specified id on this edge");
//                    pos = lanes[destLane]->getShape().positionAtLengthPosition(destPos);         
//                }
//                break;
//            case POSITION_2D:
//            case POSITION_2_5D:
//            case POSITION_3D:
//                {
//                    float destX = requestMsg.readFloat();
//                    float destY = requestMsg.readFloat();
//					if ((dataType == POSITION_3D) || (dataType == POSITION_2_5D)) {
//						requestMsg.readFloat();	// z value is ignored
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
				distance = static_cast<float>(GeomHelper::distance(veh->getPosition(), destPos));
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

	// unknown variable
	default:
		throw TraCIException("Unknown domain variable specified");
	}

	return warning;
}

/*****************************************************************************/

std::string
TraCIServer::handleTrafficLightDomain(bool isWriteCommand, tcpip::Storage& requestMsg, tcpip::Storage& response)  
throw(TraCIException)
{
	int count = 0;
	std::string name;
	std::string warning = "";	// additional description for response
	DataTypeContainer dataCont;

	// domain object
	int objectId = requestMsg.readInt();
	MSTrafficLightLogic* tlLogic = getTLLogicByExtId(objectId);

	// variable id
	int variableId = requestMsg.readUnsignedByte();

	// value data type
	int dataType = requestMsg.readUnsignedByte();

	// if end of message is not yet reached, the value parameter has to be read
	if (requestMsg.valid_pos()) {
		dataCont.readValue(dataType, requestMsg);
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
	// name string of the object
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
//		name = requestMsg.readString();
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
		count = MSNet::getInstance()->getTLSControl().getAllTLIds().size();
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
			response.writeFloat(junc->getPosition().x());
			response.writeFloat(junc->getPosition().y());
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
			size_t step = tlLogic->getStepNo();
			if (variableId == DOMVAR_NEXTTLPHASE) {
				size_t curStep = tlLogic->getStepNo();
				size_t pos = tlLogic->getPosition(MSNet::getInstance()->getCurrentTimeStep());
				do {
					pos++;
				} while ( (step=tlLogic->getStepFromPos(pos)) == curStep);
			}
			MSPhaseDefinition phase = tlLogic->getPhaseFromStep(step);

			// get the list of link vectors affected by that tl logic
			MSTrafficLightLogic::LinkVectorVector affectedLinks = tlLogic->getLinks();

			// for each affected link of that tl logic, write the  phase state
			// to the answer message along with preceding and succeeding edge
			Storage phaseList;
			int listLength = 0;
//			std::map<MSLane*, std::set<const MSEdge*> > connectLane2Edge;
			std::map<const MSEdge*, pair<const MSEdge*, int> > writtenEdgePairs;
			for (int i=0; i<affectedLinks.size(); i++) {
				// get the list of links controlled by that light
				MSTrafficLightLogic::LinkVector linkGroup = affectedLinks[i];
				// get the list of preceding lanes to that links
				MSTrafficLightLogic::LaneVector laneGroup = tlLogic->getLanesAt(i);
				// get status of the traffic light
				MSLink::LinkState tlState = phase.getLinkState(i);

//				const MSEdge* precEdge = NULL;
//				const MSEdge* succEdge = NULL;
				for (int linkNo=0; linkNo<linkGroup.size(); linkNo++) {
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
					const MSEdge* const precEdge = laneGroup[linkNo]->getEdge();
					const MSEdge* const succEdge = linkGroup[linkNo]->getLane()->getEdge();

					// for each pair of edges and every different tl state, write only one tl switch command
					std::map<const MSEdge*, pair<const MSEdge*, int> >::iterator itPair = writtenEdgePairs.find(precEdge);
					if (itPair != writtenEdgePairs.end()) {
						if (itPair->second.first == succEdge && itPair->second.second == tlState) {
							continue;
						}
					}
					// remember the current edge pair and tl status
					writtenEdgePairs[precEdge] = make_pair(succEdge, tlState);

					// write preceding edge
					phaseList.writeString(precEdge->getID());
					// write succeeding edge
					phaseList.writeString(succEdge->getID());
					// write status of the traffic light
					switch (tlState) {
					case MSLink::LINKSTATE_TL_GREEN:
						phaseList.writeUnsignedByte(TLPHASE_GREEN);
						break;
					case MSLink::LINKSTATE_TL_YELLOW:
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
TraCIServer::handlePoiDomain(bool isWriteCommand, tcpip::Storage& requestMsg, tcpip::Storage& response) 
throw(TraCIException)
{
	std::string name;
	std::string warning = "";	// additional description for response
	DataTypeContainer dataCont;

	// domain object
	int objectId = requestMsg.readInt();
	PointOfInterest* poi = getPoiByExtId(objectId);

	// variable id
	int variableId = requestMsg.readUnsignedByte();

	// value data type
	int dataType = requestMsg.readUnsignedByte();

	// if end of message is not yet reached, the value parameter has to be read
	if (requestMsg.valid_pos()) {
		dataCont.readValue(dataType, requestMsg);
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
	// name string of the object
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
//		name = requestMsg.readString();
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
			response.writeFloat(poi->x());
			response.writeFloat(poi->y());
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
TraCIServer::handlePolygonDomain(bool isWriteCommand, tcpip::Storage& requestMsg, tcpip::Storage& response) 
throw(TraCIException)
{
	std::string name;
	std::string warning = "";	// additional description for response
	DataTypeContainer dataCont;

	// domain object
	int objectId = requestMsg.readInt();
	Polygon2D* poly = getPolygonByExtId(objectId);

	// variable id
	int variableId = requestMsg.readUnsignedByte();

	// value data type
	int dataType = requestMsg.readUnsignedByte();

	// if end of message is not yet reached, the value parameter has to be read
	if (requestMsg.valid_pos()) {
		dataCont.readValue(dataType, requestMsg);
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
	// name string of the object
	case DOMVAR_NAME:
		if (poly != NULL) {
			name = poly->getName();
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
//		name = requestMsg.readString();
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
			response.writeFloat(poly->getPosition2DVector().center().x());
			response.writeFloat(poly->getPosition2DVector().center().y());
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
			response.writeUnsignedByte(MIN2(static_cast<size_t>(255),poly->getPosition2DVector().size()));
			for (int i=0; i < MIN2(static_cast<size_t>(255),poly->getPosition2DVector().size()); i++) {
				response.writeFloat(poly->getPosition2DVector()[i].x());
				response.writeFloat(poly->getPosition2DVector()[i].y());
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
TraCIServer::handleLifecycleSubscriptions(tcpip::Storage& respMsg)
throw(TraCIException)
{

	if (myLifecycleSubscriptions.count(DOM_VEHICLE) != 0) {

		for (std::set<int>::const_iterator i = myDestroyedVehicles.begin(); i != myDestroyedVehicles.end(); i++) {
			int extId = *i;

			respMsg.writeUnsignedByte(1+1+1+4);
			respMsg.writeUnsignedByte(CMD_OBJECTDESTRUCTION);
			respMsg.writeUnsignedByte(DOM_VEHICLE);
			respMsg.writeInt(extId);
		}
		myDestroyedVehicles.clear();

		for (std::set<int>::const_iterator i = myCreatedVehicles.begin(); i != myCreatedVehicles.end(); i++) {
			int extId = *i;

			respMsg.writeUnsignedByte(1+1+1+4);
			respMsg.writeUnsignedByte(CMD_OBJECTCREATION);
			respMsg.writeUnsignedByte(DOM_VEHICLE);
			respMsg.writeInt(extId);
		}
		myCreatedVehicles.clear();

	}
}

/*****************************************************************************/

void
TraCIServer::handleDomainSubscriptions(tcpip::Storage& respMsg, const SUMOTime& currentTime, const map<int, const MSVehicle*>& activeEquippedVehicles)
throw(TraCIException)
{

	if (myDomainSubscriptions.count(DOM_VEHICLE) != 0) {

		std::list<std::pair<int, int> > subscribedVariables = myDomainSubscriptions[DOM_VEHICLE];

		// iterate over all objects
		for (map<int, const MSVehicle*>::const_iterator iter = activeEquippedVehicles.begin(); iter != activeEquippedVehicles.end(); ++iter) {
			int extId = (*iter).first;
			const MSVehicle* vehicle = (*iter).second;
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
					tempMsg.writeFloat(vehicle->getSpeed());
				}
				if ((variableId == DOMVAR_POSITION) && (dataType == POSITION_2D)) {
					Position2D pos = vehicle->getPosition();
					tempMsg.writeFloat(pos.x() - getNetBoundary().xmin());
					tempMsg.writeFloat(pos.y() - getNetBoundary().ymin());
				}
				if ((variableId == DOMVAR_POSITION) && (dataType == POSITION_ROADMAP)) {
					tempMsg.writeString(vehicle->getEdge()->getID());
				}
				if ((variableId == DOMVAR_ANGLE) && (dataType == TYPE_FLOAT)) {
					tempMsg.writeFloat(vehicle->getLane().getShape().rotationDegreeAtLengthPosition(vehicle->getPositionOnLane()));
				}
			}

			// send command length
			respMsg.writeUnsignedByte(tempMsg.size()+1);

			// send command
			respMsg.writeStorage(tempMsg);
		}


	}

}
    
/*****************************************************************************/
}

#endif
