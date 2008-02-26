/****************************************************************************/
/// @file    TraCIServer.cpp
/// @author  Axel Wegener <wegener@itm.uni-luebeck.de>
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
// compiler pragmas
// ===========================================================================


// ===========================================================================
// included modules
// ===========================================================================
#include "TraCIConstants.h"
#include "TraCIServer.h"

#ifdef TRACI

#include "foreign/tcpip/socket.h"
#include "foreign/tcpip/storage.h"
#include "utils/common/SUMOTime.h"
#include "utils/common/SUMODijkstraRouter.h"
#include "microsim/MSNet.h"
#include "microsim/MSVehicleControl.h"
#include "microsim/MSVehicle.h"
#include "microsim/MSEdge.h"
#include "microsim/MSRouteHandler.h"
#include "microsim/MSRouteLoaderControl.h"
#include "microsim/MSRouteLoader.h"
#include "microsim/traffic_lights/MSTLLogicControl.h"

#include "microsim/MSEdgeControl.h"
#include "microsim/MSLane.h"
#include "microsim/trigger/MSCalibrator.h"

#include <string>
#include <map>
#include <iostream>
#include <cstdlib>
#include <cfloat>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace tcpip;


// ===========================================================================
// method definitions
// ===========================================================================
namespace traci
{
/*****************************************************************************/

TraCIServer::TraCIServer()
{
    OptionsCont &oc = OptionsCont::getOptions();

    port_ = oc.getInt("remote-port");
    beginTime_ = oc.getInt("begin");
    endTime_ = oc.getInt("end");
    penetration_ = oc.getFloat("penetration");
    routeFile_ = oc.getString("route-files");
    isMapChanged_ = true;
    numEquippedVehicles_ = 0;
    closeConnection_ = false;
    netBoundary_ = NULL;
}

/*****************************************************************************/

TraCIServer::~TraCIServer()
{
    if (netBoundary_ != NULL) delete netBoundary_;
}

/*****************************************************************************/

void
TraCIServer::run()
{
    // Prepare simulation
    MSNet::getInstance()->initialiseSimulation();

    try {
        // Opens listening socket
        std::cout << "***Starting server on port " << port_  << " ***" << std::endl;
        Socket socket(port_);
        socket.accept();

        // When got here, a client has connected
        // Simulation should run until
        // 1. end time reached or
        // 2. got CMD_CLOSE or
        // 3. Client closes socket connection

        while (!closeConnection_) {
            Storage storIn;
            Storage storOut;
            // Read a message
            socket.receiveExact(storIn);

            while (storIn.valid_pos() && !closeConnection_) {
                // dispatch each command
                if (! dispatchCommand(storIn, storOut)) closeConnection_ = true;
            }

            // send out all answers as one storage
            socket.sendExact(storOut);
        }
    } catch (TraCIException e) {
        cerr << e.what() << endl;
    } catch (SocketException e) {
        cerr << e.what() << endl;
    }

    MSNet::getInstance()->closeSimulation(beginTime_, endTime_);

}

/*****************************************************************************/

bool
TraCIServer::dispatchCommand(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
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
    case CMD_CLOSE:
        commandCloseConnection(requestMsg, respMsg);
        break;
    case CMD_UPDATECALIBRATOR:
        commandUpdateCalibrator(requestMsg, respMsg);
        break;
	case CMD_POSITIONCONVERSION:
		commandPositionConversion(requestMsg, respMsg);
		break;
    default:
        writeStatusCmd(respMsg, commandId, RTYPE_NOTIMPLEMENTED, "Command not implemented in sumo");
        return false;
    }

    if (requestMsg.position() != commandStart + commandLength) {
        writeStatusCmd(respMsg, commandId, RTYPE_ERR, "Wrong position in requestMessage after dispatching command");
        return false;
    }
    return true;
}

/*****************************************************************************/

void
TraCIServer::commandSetMaximumSpeed(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
    MSVehicle* veh = getVehicleByExtId(requestMsg.readInt());   // external node id (equipped vehicle number)
    float maxspeed = requestMsg.readFloat();

    if (veh == NULL) {
        writeStatusCmd(respMsg, CMD_SETMAXSPEED, RTYPE_ERR, "Can not retrieve node with given ID");
        return;
    }

    if (maxspeed>=0) {
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
throw(TraCIException)
{
	MSNet *net = MSNet::getInstance();
    SUMOTime currentTime = net->getCurrentTimeStep();

    // TargetTime
    SUMOTime targetTime = static_cast<SUMOTime>(requestMsg.readDouble()) + beginTime_;
    if (targetTime > endTime_) {
        targetTime = endTime_;
    }

    // Position representation
    int resType = requestMsg.readUnsignedByte();
    if (resType != POSITION_2D && resType != POSITION_ROADMAP) {
        writeStatusCmd(respMsg, CMD_SIMSTEP, RTYPE_ERR, "Error: unsupported return format requested.");
        return;
    }


    // do simulation step
    while (targetTime > currentTime) {
        net->simulationStep(currentTime, currentTime + DELTA_T);
        currentTime += DELTA_T;
        isMapChanged_ = true;
        cout << "Step #" << currentTime << (char) 13;
    }

	// for each vehicle process any active traci command
	for (std::map<std::string, int>::iterator iter = equippedVehicles_.begin();
            iter != equippedVehicles_.end(); ++iter) {
        if ((*iter).second != -1) { // Look only at equipped vehicles
            MSVehicle* veh = net->getVehicleControl().getVehicle((*iter).first);
            if (veh != NULL) {
				veh->processTraCICommands(currentTime);
            }
        }
    }

    // prepare output
    try {
        // map containing all active equipped vehicles. maps external id to MSVehicle*
        map<int, const MSVehicle*> activeEquippedVehicles;
        // get access to all vehicles in simulation
        MSVehicleControl &vehControl = net->getVehicleControl();
        // iterate over all vehicles in simulation
        for (map<string, MSVehicle*>::const_iterator iter = vehControl.loadedVehBegin(); iter != vehControl.loadedVehEnd(); ++iter) {
            // selected vehicle
            const string vehicleId   = (*iter).first;
            const MSVehicle *vehicle = (*iter).second;
            // insert into equippedVehicleId if not contained
            std::map<std::string, int>::const_iterator equippedVeh = equippedVehicles_.find(vehicleId);
            if (equippedVeh == equippedVehicles_.end()) {
                // determine if vehicle is equipped
                double rnd = double(rand())/RAND_MAX;
                if (rnd <= penetration_) {
                    // vehicle is equipped
                    equippedVehicles_[vehicleId] = numEquippedVehicles_;
                    // put into active list?
                    if (vehicle->getInTransit()) {
                        activeEquippedVehicles[numEquippedVehicles_] = vehicle;
                    }
                    numEquippedVehicles_++;
                } else {
                    // vehicle is not equipped
                    equippedVehicles_[vehicleId] = -1;
                }
            } else if (equippedVeh->second >= 0 && vehicle->getInTransit()) {
                int extId = equippedVeh->second;
                activeEquippedVehicles[extId] = vehicle;
                // vehicle is equipped
            }
        }

        // Everything is fine
        writeStatusCmd(respMsg, CMD_SIMSTEP, RTYPE_OK, "");

        //out.writeChar( static_cast<unsigned char>(rtype) );
        //out.writeInt(numEquippedVehicles_);
        // iterate over all active equipped vehicles
        // and generate a Move Node command for each vehicle
        for (map<int, const MSVehicle*>::iterator iter = activeEquippedVehicles.begin(); iter != activeEquippedVehicles.end(); ++iter) {
            int extId = (*iter).first;
            const MSVehicle* vehicle = (*iter).second;

            // command length
            respMsg.writeUnsignedByte(23);
            // command type
            respMsg.writeUnsignedByte(CMD_MOVENODE);
            // node id
            respMsg.writeInt(extId);
            // end time
            respMsg.writeDouble(currentTime);

            if (resType == POSITION_2D) {
                // return type
                respMsg.writeUnsignedByte(POSITION_2D);

                Position2D pos = vehicle->getPosition();
                //xpos
                respMsg.writeFloat(pos.x() - getNetBoundary().xmin());
                // y pos
                respMsg.writeFloat(pos.y() - getNetBoundary().ymin());
            } else if (resType == POSITION_ROADMAP) {
                // return type
                respMsg.writeUnsignedByte(POSITION_ROADMAP);

                respMsg.writeString(vehicle->getEdge()->getID());
                respMsg.writeFloat(vehicle->getPositionOnLane());
            }

        }

    } catch (...) {
        writeStatusCmd(respMsg, CMD_SIMSTEP, RTYPE_ERR, "some error happen in command: simulation step. Sumo shuts down.");
        return;
    }

    return;
}

/*****************************************************************************/

void
TraCIServer::commandStopNode(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
{
    std::string roadID;
    float lanePos;
    unsigned char laneIndex;
	MSLane* actLane;

    // NodeId
	int nodeId = requestMsg.readInt();
    MSVehicle* veh = getVehicleByExtId(nodeId);   // external node id (equipped vehicle number)

    // StopPosition
    unsigned char posType = requestMsg.readUnsignedByte();	// position type
    if (posType == POSITION_ROADMAP) {
        // road-id
        roadID = requestMsg.readString();
        // position on lane
        lanePos = requestMsg.readFloat();
        // lane-id
        laneIndex = requestMsg.readUnsignedByte();

        if (lanePos < 0) {
            writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "Position on lane must not be negative");
        }

		// get the actual lane that is referenced by laneIndex
		MSEdge* road = MSEdge::dictionary(roadID);
        if (road == NULL) {
            writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "Unable to retrieve road with given id");
        }

        const MSEdge::LaneCont* const allLanes = road->getLanes();
		if (laneIndex >= allLanes->size()) {
            writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "No lane existing with such id on the given road");
        }

		actLane = (*allLanes)[0];
		int index = 0;
		while (road->rightLane(actLane) != NULL) {
			actLane = road->rightLane(actLane);
			index++;
		}
		actLane = (*allLanes)[0];
		if (index < laneIndex) {
			for (int i=0; i < (laneIndex - index); i++) {
				actLane = road->leftLane(actLane);
			}
		} else {
			for (int i=0; i < (index - laneIndex); i++) {
				actLane = road->rightLane(actLane);
			}
		}
		
    } else {
        writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "Currently not supported or unknown Position Format");
    }

    // Radius
    float radius = requestMsg.readFloat();
    // waitTime
    double waitTime = requestMsg.readDouble();

    if (veh == NULL) {
        writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "Can not retrieve node with given ID");
        return;
    }

    // Forward command to vehicle
    veh->addTraciStop(actLane, lanePos, radius, waitTime);

    // create a reply message
    writeStatusCmd(respMsg, CMD_STOP, RTYPE_OK, "");
	// add a stopnode command containging the actually used road map position to the reply
	int length = 1 + 1 + 4 + 1 + roadID.length() + 4 + 1 + 4 + 8;
	respMsg.writeByte(length);				// lenght
	respMsg.writeByte(CMD_STOP);			// command id
	respMsg.writeInt(nodeId);				// node id
	respMsg.writeByte(POSITION_ROADMAP);	// pos format
	respMsg.writeString(roadID);			// road id
	respMsg.writeFloat(lanePos);			// pos
	respMsg.writeByte(laneIndex);			// lane id
	respMsg.writeFloat(radius);				// radius
	respMsg.writeDouble(waitTime);			// wait time


    return;
}

/*****************************************************************************/
void
TraCIServer::commandChangeLane(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
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

	// Forward command to vehicle
	const MSEdge* const road = veh->getEdge();
    const MSEdge::LaneCont* const allLanes = road->getLanes();
	if (laneIndex >= allLanes->size()) {
        writeStatusCmd(respMsg, CMD_STOP, RTYPE_ERR, "No lane existing with such id on the current road");
    }

	int index = 0;
	MSLane* actLane = (*allLanes)[0];
	while (road->rightLane(actLane) != NULL) {
		actLane = road->rightLane(actLane);
		index++;
	}
	
	if (index < laneIndex) {
		veh->forceLaneChangeLeft(laneIndex - index, stickyTime);
	}
	if (index > laneIndex) {
		veh->forceLaneChangeRight(index - laneIndex, stickyTime);
	}

    // create a reply message
    writeStatusCmd(respMsg, CMD_CHANGELANE, RTYPE_OK, "");

    return;
}

/*****************************************************************************/
void
TraCIServer::commandChangeRoute(tcpip::Storage& requestMsg, tcpip::Storage& respMsg)
throw(TraCIException)
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
throw(TraCIException)
{
    // NodeId
    MSVehicle* veh = getVehicleByExtId(requestMsg.readInt());   // external node id (equipped vehicle number)
    // EdgeId
    std::string edgeID = requestMsg.readString();

    // destination edge
    const MSEdge* destEdge = MSEdge::dictionary(edgeID);

    if (veh == NULL) {
        writeStatusCmd(respMsg, CMD_CHANGEROUTE, RTYPE_ERR, "Can not retrieve node with given ID");
        return;
    }

    if (destEdge == NULL) {
        writeStatusCmd(respMsg, CMD_CHANGEROUTE, RTYPE_ERR, "Can not retrieve road with given ID");
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
    tcpip::Storage tempMsg;

    // trafic light id
    std::string id = requestMsg.readString();
    // start of time interval
    double timeFrom = requestMsg.readDouble();
    // end of time interval
    double timeTo = requestMsg.readDouble();

    // get the running programm of the traffic light
    MSTLLogicControl &tlsControl = MSNet::getInstance()->getTLSControl();
    MSTrafficLightLogic* const tlLogic = tlsControl.get(id).getActive();

    // error checking
    if (tlLogic == NULL) {
        writeStatusCmd(respMsg, CMD_GETTLSTATUS, RTYPE_ERR, "Could not retrieve traffic light with given id");
        return;
    }
    if ((timeTo < timeFrom) || (timeTo < 0) || (timeFrom < 0)) {
        writeStatusCmd(respMsg, CMD_GETALLTLIDS, RTYPE_ERR, "The given time interval is not valid");
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
        yellowTimes.push_back(0);
    }

    // check every second of the given time interval for a switch in the traffic light's phases
    for (int time = timeFrom; time <= timeTo; time++) {
        size_t position = tlLogic->getPosition(time);
        size_t currentStep = tlLogic->getStepFromPos(position);

        if (currentStep != lastStep) {
            lastStep = currentStep;
            phase = tlLogic->getPhaseFromStep(currentStep);

            // for every link of the tl's junction, compare the actual and the last red/green state
            for (int i = 0; i < linkStates.size(); i++) {
                MSLink::LinkState nextLinkState = phase.getLinkState(i);

                if (nextLinkState != linkStates[i]) {
                    linkStates[i] = nextLinkState;

                    // get the group of links that is affected by the changed light status
                    MSTrafficLightLogic::LinkVector linkGroup = affectedLinks[i];
                    // get the group of preceding lanes of the link group
                    MSTrafficLightLogic::LaneVector laneGroup = tlLogic->getLanesAt(i);

                    if (nextLinkState == MSLink::LINKSTATE_TL_YELLOW) {
                        yellowTimes[i]++;
                    } else {

                        // for each link with new red/green status, write a TLSWITCH command
                        for (int j = 0; j < linkGroup.size(); j++) {

                            // time of the switch
                            tempMsg.writeDouble(time);
                            // preceding edeg id
                            tempMsg.writeString(laneGroup[j]->getEdge()->getID());
                            // succeeding edge id
                            tempMsg.writeString(linkGroup[j]->getLane()->getEdge()->getID());
                            // new status
                            if (nextLinkState == MSLink::LINKSTATE_TL_RED) {
                                tempMsg.writeString("red");
                            } else {
                                tempMsg.writeString("green");
                            }
                            //yellow time
                            tempMsg.writeDouble(yellowTimes[i]);

                            // command length
                            respMsg.writeByte(2 + tempMsg.size());
                            // command type
                            respMsg.writeByte(CMD_TLSWITCH);
                            // command content
                            respMsg.writeStorage(tempMsg);
                        }

                        yellowTimes[i] = 0;
                    }
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
    float newSpeed = requestMsg.readFloat();
    // time interval
    double duration = requestMsg.readDouble();

    if (veh == NULL) {
        writeStatusCmd(respMsg, CMD_SLOWDOWN, RTYPE_ERR, "Can not retrieve node with given ID");
        return;
    }
    if (newSpeed < 0) {
        writeStatusCmd(respMsg, CMD_SLOWDOWN, RTYPE_ERR, "Negative speed value");
        return;
    }
    if (duration < 0) {
        writeStatusCmd(respMsg, CMD_SLOWDOWN, RTYPE_ERR, "Invalid time interval");
        return;
    }

    if (!veh->startSpeedAdaption(newSpeed, duration, MSNet::getInstance()->getCurrentTimeStep())) {
        writeStatusCmd(respMsg, CMD_SLOWDOWN, RTYPE_ERR, "Not slowing down");
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
    // Close simulation
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

	unsigned char posType = requestMsg.readUnsignedByte();
	float x = requestMsg.readFloat();
	float y = requestMsg.readFloat();
	float z = requestMsg.readFloat();

	RoadMapPos roadPos = convertCartesianToRoadMap(Position2D(x, y));
	std::cerr << "TraCI: position conversion from: x=" << x << " y=" << y << " to: roadId=" << roadPos.roadId 
		<< " pos=" << roadPos.pos << " laneId =" << roadPos.laneId;

	writeStatusCmd(respMsg, CMD_POSITIONCONVERSION, RTYPE_OK, "");	
}
/*****************************************************************************/

void
TraCIServer::writeStatusCmd(tcpip::Storage& respMsg, int commandId, int status, std::string description)
{
    if (status == RTYPE_ERR) {
        closeConnection_ = true;
        cerr << "Answered with error to command " << commandId
        << ": " << description << endl;
    } else if (status == RTYPE_NOTIMPLEMENTED) {
        cerr << "Requested command not implemented (" << commandId
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
	double minDistance = DBL_MAX;
	SUMOReal newDistance;
	Position2D intersection;
	MSLane* tmpLane;

	
	allEdgeIds = MSNet::getInstance()->getEdgeControl().getEdgeNames();

	// iterate through all known edges
	for (std::vector<std::string>::iterator itId = allEdgeIds.begin(); itId != allEdgeIds.end(); itId++) {
		edge = MSEdge::dictionary((*itId));
		const MSEdge::LaneCont * const allLanes = edge->getLanes();

		// iterate through all lanes of this edge
		for (MSEdge::LaneCont::const_iterator itLane = allLanes->begin(); itLane != allLanes->end(); itLane++) {
			Position2DVector shape = (*itLane)->getShape();
			
			// iterate through all segments of this lane's shape
			for (int i = 0; i < shape.size()-1; i++) {
				lineStart = shape[i];
				lineEnd = shape[i+1];

				// if this line is no candidate for lying nearer to the cartesian position 
				// than the line determined so far, skip it
				if ( (lineStart.y() > (pos.y()+minDistance) && lineEnd.y() > (pos.y()+minDistance))
					|| (lineStart.y() < (pos.y()-minDistance) && lineEnd.y() < (pos.y()-minDistance))
					|| (lineStart.x() > (pos.x()+minDistance) && lineEnd.x() > (pos.x()+minDistance))
					|| (lineStart.x() < (pos.x()-minDistance) && lineEnd.x() < (pos.x()-minDistance)) ) {
					continue;
				} else {
					// else compute the distance and check it
					newDistance = GeomHelper::closestDistancePointLine(pos, lineStart, lineEnd, intersection);
					if (newDistance < minDistance && newDistance != -1.0) {
						// new distance is shorter
						minDistance = newDistance;

						// save the found road map position
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
					}
				}
			}
		}
	}

	return result;
}


/*****************************************************************************/
}

#endif
