/****************************************************************************/
/// @file    MSNet.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Clemens Honomichl
/// @author  Eric Nicolay
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Mario Krumnow
/// @author  Christoph Sommer
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// The simulated network and simulation perfomer
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include <cassert>
#include <vector>
#include <ctime>
#include <utils/common/UtilExceptions.h>
#include "MSNet.h"
#include "MSPersonControl.h"
#include "MSEdgeControl.h"
#include "MSJunctionControl.h"
#include "MSInsertionControl.h"
#include "MSEventControl.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSJunctionLogic.h"
#include "MSLane.h"
#include "MSVehicleTransfer.h"
#include "MSRoute.h"
#include <utils/xml/SUMORouteLoaderControl.h>
#include "trigger/MSTrigger.h"
#include "trigger/MSCalibrator.h"
#include "traffic_lights/MSTLLogicControl.h"
#include "MSVehicleControl.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include "traffic_lights/MSTrafficLightLogic.h"
#include <utils/shapes/Polygon.h>
#include <utils/shapes/ShapeContainer.h>

#include <utils/iodevices/OutputDevice_File.h>
#include "output/MSFCDExport.h"
#include "output/MSEmissionExport.h"
#include "output/MSFullExport.h"
#include "output/MSQueueExport.h"
#include "output/MSVTKExport.h"
#include "output/MSXMLRawOut.h"
#include "output/MSAmitranTrajectories.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/SysUtils.h>
#include <utils/common/WrappingCommand.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/PedestrianRouter.h>
#include "MSGlobals.h"
#include "MSPModel.h"
#include <utils/geom/GeoConvHelper.h>
#include "MSPerson.h"
#include "MSEdgeWeightsStorage.h"
#include "MSStateHandler.h"

#ifdef HAVE_INTERNAL
#include <mesosim/MELoop.h>
#include <utils/iodevices/BinaryInputDevice.h>
#endif

#ifndef NO_TRACI
#include <traci-server/TraCIServer.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
MSNet* MSNet::myInstance = 0;

const std::string MSNet::STAGE_EVENTS("events");
const std::string MSNet::STAGE_MOVEMENTS("move");
const std::string MSNet::STAGE_LANECHANGE("laneChange");
const std::string MSNet::STAGE_INSERTIONS("insertion");

// ===========================================================================
// member method definitions
// ===========================================================================
SUMOReal
MSNet::getEffort(const MSEdge* const e, const SUMOVehicle* const v, SUMOReal t) {
    SUMOReal value;
    const MSVehicle* const veh = dynamic_cast<const MSVehicle* const>(v);
    if (veh != 0 && veh->getWeightsStorage().retrieveExistingEffort(e, t, value)) {
        return value;
    }
    if (getInstance()->getWeightsStorage().retrieveExistingEffort(e, t, value)) {
        return value;
    }
    return 0;
}


SUMOReal
MSNet::getTravelTime(const MSEdge* const e, const SUMOVehicle* const v, SUMOReal t) {
    SUMOReal value;
    const MSVehicle* const veh = dynamic_cast<const MSVehicle* const>(v);
    if (veh != 0 && veh->getWeightsStorage().retrieveExistingTravelTime(e, t, value)) {
        return value;
    }
    if (getInstance()->getWeightsStorage().retrieveExistingTravelTime(e, t, value)) {
        return value;
    }
    return e->getMinimumTravelTime(v);
}



// ---------------------------------------------------------------------------
// MSNet - methods
// ---------------------------------------------------------------------------
MSNet*
MSNet::getInstance(void) {
    if (myInstance != 0) {
        return myInstance;
    }
    throw ProcessError("A network was not yet constructed.");
}


MSNet::MSNet(MSVehicleControl* vc, MSEventControl* beginOfTimestepEvents,
             MSEventControl* endOfTimestepEvents, MSEventControl* insertionEvents,
             ShapeContainer* shapeCont):
    myVehiclesMoved(0),
    myRouterTTInitialized(false),
    myRouterTTDijkstra(0),
    myRouterTTAStar(0),
    myRouterEffort(0),
    myPedestrianRouter(0) {
    if (myInstance != 0) {
        throw ProcessError("A network was already constructed.");
    }
    OptionsCont& oc = OptionsCont::getOptions();
    myStep = string2time(oc.getString("begin"));
    myLogExecutionTime = !oc.getBool("no-duration-log");
    myLogStepNumber = !oc.getBool("no-step-log");
    myTooManyVehicles = oc.getInt("max-num-vehicles");
    myInserter = new MSInsertionControl(*vc, string2time(oc.getString("max-depart-delay")), !oc.getBool("eager-insert"));
    myVehicleControl = vc;
    myDetectorControl = new MSDetectorControl();
    myEdges = 0;
    myJunctions = 0;
    myRouteLoaders = 0;
    myLogics = 0;
    myPersonControl = 0;
    myEdgeWeights = 0;
    myShapeContainer = shapeCont == 0 ? new ShapeContainer() : shapeCont;

    myBeginOfTimestepEvents = beginOfTimestepEvents;
    myEndOfTimestepEvents = endOfTimestepEvents;
    myInsertionEvents = insertionEvents;
    myLanesRTree.first = false;

#ifdef HAVE_INTERNAL
    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gMesoNet = new MELoop(string2time(oc.getString("meso-recheck")));
    }
#endif
    myInstance = this;
}




void
MSNet::closeBuilding(MSEdgeControl* edges, MSJunctionControl* junctions,
                     SUMORouteLoaderControl* routeLoaders,
                     MSTLLogicControl* tlc,
                     std::vector<SUMOTime> stateDumpTimes,
                     std::vector<std::string> stateDumpFiles) {
    myEdges = edges;
    myJunctions = junctions;
    myRouteLoaders = routeLoaders;
    myLogics = tlc;
    // save the time the network state shall be saved at
    myStateDumpTimes = stateDumpTimes;
    myStateDumpFiles = stateDumpFiles;

    // set requests/responses
    myJunctions->postloadInitContainer();

    // initialise performance computation
    if (myLogExecutionTime) {
        mySimBeginMillis = SysUtils::getCurrentMillis();
    }
}


MSNet::~MSNet() {
    // delete events first maybe they do some cleanup
    delete myBeginOfTimestepEvents;
    delete myEndOfTimestepEvents;
    delete myInsertionEvents;
    // delete controls
    delete myJunctions;
    delete myDetectorControl;
    // delete mean data
    delete myEdges;
    delete myInserter;
    delete myLogics;
    delete myRouteLoaders;
    delete myVehicleControl;
    if (myPersonControl != 0) {
        delete myPersonControl;
    }
    delete myShapeContainer;
    delete myEdgeWeights;
    delete myRouterTTDijkstra;
    delete myRouterTTAStar;
    delete myRouterEffort;
    myLanesRTree.second.RemoveAll();
    clearAll();
#ifdef HAVE_INTERNAL
    if (MSGlobals::gUseMesoSim) {
        delete MSGlobals::gMesoNet;
    }
#endif
    myInstance = 0;
}


int
MSNet::simulate(SUMOTime start, SUMOTime stop) {
    // report the begin when wished
    WRITE_MESSAGE("Simulation started with time: " + time2string(start));
    // the simulation loop
    MSNet::SimulationState state = SIMSTATE_RUNNING;
    myStep = start;
    // preload the routes especially for TraCI
    loadRoutes();
#ifndef NO_TRACI
#ifdef HAVE_PYTHON
    if (OptionsCont::getOptions().isSet("python-script")) {
        TraCIServer::runEmbedded(OptionsCont::getOptions().getString("python-script"));
        closeSimulation(start);
        WRITE_MESSAGE("Simulation ended at time: " + time2string(getCurrentTimeStep()));
        WRITE_MESSAGE("Reason: Script ended");
        return 0;
    }
#endif
#endif
    while (state == SIMSTATE_RUNNING) {
        if (myLogStepNumber) {
            preSimStepOutput();
        }
        simulationStep();
        if (myLogStepNumber) {
            postSimStepOutput();
        }
        state = simulationState(stop);
#ifndef NO_TRACI
        if (state != SIMSTATE_RUNNING) {
            if (OptionsCont::getOptions().getInt("remote-port") != 0 && !TraCIServer::wasClosed()) {
                state = SIMSTATE_RUNNING;
            }
        }
#endif
    }
    // report the end when wished
    WRITE_MESSAGE("Simulation ended at time: " + time2string(getCurrentTimeStep()));
    WRITE_MESSAGE("Reason: " + getStateMessage(state));
    // exit simulation loop
    closeSimulation(start);
    return 0;
}

void
MSNet::loadRoutes() {
    myRouteLoaders->loadNext(myStep);
}


void
MSNet::closeSimulation(SUMOTime start) {
    if (myLogExecutionTime) {
        long duration = SysUtils::getCurrentMillis() - mySimBeginMillis;
        std::ostringstream msg;
        // print performance notice
        msg << "Performance: " << "\n" << " Duration: " << duration << " ms" << "\n";
        if (duration != 0) {
            msg << " Real time factor: " << (STEPS2TIME(myStep - start) * 1000. / (SUMOReal)duration) << "\n";
            msg.setf(std::ios::fixed , std::ios::floatfield);    // use decimal format
            msg.setf(std::ios::showpoint);    // print decimal point
            msg << " UPS: " << ((SUMOReal)myVehiclesMoved / ((SUMOReal)duration / 1000)) << "\n";
        }
        // print vehicle statistics
        const std::string discardNotice = ((myVehicleControl->getLoadedVehicleNo() != myVehicleControl->getDepartedVehicleNo()) ?
                                           " (Loaded: " + toString(myVehicleControl->getLoadedVehicleNo()) + ")" : "");
        msg << "Vehicles: " << "\n"
            << " Inserted: " << myVehicleControl->getDepartedVehicleNo() << discardNotice << "\n"
            << " Running: " << myVehicleControl->getRunningVehicleNo() << "\n"
            << " Waiting: " << myInserter->getWaitingVehicleNo() << "\n";

        if (myVehicleControl->getTeleportCount() > 0) {
            // print optional teleport statistics
            std::vector<std::string> reasons;
            if (myVehicleControl->getCollisionCount() > 0) {
                reasons.push_back("Collisions: " + toString(myVehicleControl->getCollisionCount()));
            }
            if (myVehicleControl->getTeleportsJam() > 0) {
                reasons.push_back("Jam: " + toString(myVehicleControl->getTeleportsJam()));
            }
            if (myVehicleControl->getTeleportsYield() > 0) {
                reasons.push_back("Yield: " + toString(myVehicleControl->getTeleportsYield()));
            }
            if (myVehicleControl->getTeleportsWrongLane() > 0) {
                reasons.push_back("Wrong Lane: " + toString(myVehicleControl->getTeleportsWrongLane()));
            }
            msg << "Teleports: " << myVehicleControl->getTeleportCount() << " (" << joinToString(reasons, ", ") << ")\n";
        }
        if (myVehicleControl->getEmergencyStops() > 0) {
            msg << "Emergency Stops: " << myVehicleControl->getEmergencyStops() << "\n";
        }
        WRITE_MESSAGE(msg.str());
    }
    myDetectorControl->close(myStep);
    if (OptionsCont::getOptions().getBool("vehroute-output.write-unfinished")) {
        MSDevice_Vehroutes::generateOutputForUnfinished();
    }
#ifndef NO_TRACI
    TraCIServer::close();
#endif
}


void
MSNet::simulationStep() {
#ifndef NO_TRACI
    TraCIServer::processCommandsUntilSimStep(myStep);
    TraCIServer* t = TraCIServer::getInstance();
    if (t != 0 && t->getTargetTime() != 0 && t->getTargetTime() < myStep) {
        return;
    }
#endif
    // execute beginOfTimestepEvents
    if (myLogExecutionTime) {
        mySimStepBegin = SysUtils::getCurrentMillis();
    }
    // simulation state output
    std::vector<SUMOTime>::iterator timeIt = find(myStateDumpTimes.begin(), myStateDumpTimes.end(), myStep);
    if (timeIt != myStateDumpTimes.end()) {
        const int dist = (int)distance(myStateDumpTimes.begin(), timeIt);
        MSStateHandler::saveState(myStateDumpFiles[dist], myStep);
    }
    myBeginOfTimestepEvents->execute(myStep);
    if (MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions(myStep, STAGE_EVENTS);
    }
    // check whether the tls programs need to be switched
    myLogics->check2Switch(myStep);

#ifdef HAVE_INTERNAL
    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gMesoNet->simulate(myStep);
    } else {
#endif

        // assure all lanes with vehicles are 'active'
        myEdges->patchActiveLanes();

        // compute safe velocities for all vehicles for the next few lanes
        // also register ApproachingVehicleInformation for all links
        myEdges->planMovements(myStep);

        // decide right-of-way and execute movements
        myEdges->executeMovements(myStep);
        if (MSGlobals::gCheck4Accidents) {
            myEdges->detectCollisions(myStep, STAGE_MOVEMENTS);
        }

        // Vehicles change Lanes (maybe)
        myEdges->changeLanes(myStep);

        if (MSGlobals::gCheck4Accidents) {
            myEdges->detectCollisions(myStep, STAGE_LANECHANGE);
        }
#ifdef HAVE_INTERNAL
    }
#endif
    loadRoutes();

    // persons
    if (myPersonControl != 0) {
        myPersonControl->checkWaitingPersons(this, myStep);
    }
    // insert Vehicles
    myInsertionEvents->execute(myStep);
    myInserter->emitVehicles(myStep);
    if (MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions(myStep, STAGE_INSERTIONS);
    }
    MSVehicleTransfer::getInstance()->checkInsertions(myStep);

    // execute endOfTimestepEvents
    myEndOfTimestepEvents->execute(myStep);

#ifndef NO_TRACI
    if (TraCIServer::getInstance() != 0) {
        TraCIServer::getInstance()->postProcessVTD();
    }
#endif
    // update and write (if needed) detector values
    writeOutput();

    if (myLogExecutionTime) {
        mySimStepEnd = SysUtils::getCurrentMillis();
        mySimStepDuration = mySimStepEnd - mySimStepBegin;
        myVehiclesMoved += myVehicleControl->getRunningVehicleNo();
    }
    myStep += DELTA_T;
}


MSNet::SimulationState
MSNet::simulationState(SUMOTime stopTime) const {
    if (myTooManyVehicles > 0 && (int) myVehicleControl->getRunningVehicleNo() > myTooManyVehicles) {
        return SIMSTATE_TOO_MANY_VEHICLES;
    }
#ifndef NO_TRACI
    if (TraCIServer::wasClosed()) {
        return SIMSTATE_CONNECTION_CLOSED;
    }
    if (stopTime < 0 && OptionsCont::getOptions().getInt("remote-port") == 0) {
#else
    if (stopTime < 0) {
#endif
        if (myInsertionEvents->isEmpty()
                && (myVehicleControl->getActiveVehicleCount() == 0)
                && (myInserter->getPendingFlowCount() == 0)
                && (myPersonControl == 0 || !myPersonControl->hasNonWaiting())) {
            if (myPersonControl) {
                myPersonControl->abortWaiting();
            }
            myVehicleControl->abortWaiting();
            return SIMSTATE_NO_FURTHER_VEHICLES;
        }
    }
    if (stopTime >= 0 && myStep >= stopTime) {
        return SIMSTATE_END_STEP_REACHED;
    }
    return SIMSTATE_RUNNING;
}


std::string
MSNet::getStateMessage(MSNet::SimulationState state) {
    switch (state) {
        case MSNet::SIMSTATE_RUNNING:
            return "";
        case MSNet::SIMSTATE_END_STEP_REACHED:
            return "The final simulation step has been reached.";
        case MSNet::SIMSTATE_NO_FURTHER_VEHICLES:
            return "All vehicles have left the simulation.";
        case MSNet::SIMSTATE_CONNECTION_CLOSED:
            return "TraCI requested termination.";
        case MSNet::SIMSTATE_ERROR_IN_SIM:
            return "An error occured (see log).";
        case MSNet::SIMSTATE_TOO_MANY_VEHICLES:
            return "Too many vehicles.";
        default:
            return "Unknown reason.";
    }
}


void
MSNet::clearAll() {
    // clear container
    MSEdge::clear();
    MSLane::clear();
    MSRoute::clear();
    delete MSVehicleTransfer::getInstance();
    MSDevice_Routing::cleanup();
    MSTrigger::cleanup();
    MSCalibrator::cleanup();
    MSPModel::cleanup();
    PedestrianEdge<MSEdge, MSLane, MSJunction>::cleanup();
}


void
MSNet::writeOutput() {
    // update detector values
    myDetectorControl->updateDetectors(myStep);

    // check state dumps
    if (OptionsCont::getOptions().isSet("netstate-dump")) {
        MSXMLRawOut::write(OutputDevice::getDeviceByOption("netstate-dump"), *myEdges, myStep);
    }

    // check fcd dumps
    if (OptionsCont::getOptions().isSet("fcd-output")) {
        MSFCDExport::write(OutputDevice::getDeviceByOption("fcd-output"), myStep);
    }

    // check emission dumps
    if (OptionsCont::getOptions().isSet("emission-output")) {
        MSEmissionExport::write(OutputDevice::getDeviceByOption("emission-output"), myStep);
    }

    // check full dumps
    if (OptionsCont::getOptions().isSet("full-output")) {
        MSFullExport::write(OutputDevice::getDeviceByOption("full-output"), myStep);
    }

    // check queue dumps
    if (OptionsCont::getOptions().isSet("queue-output")) {
        MSQueueExport::write(OutputDevice::getDeviceByOption("queue-output"), myStep);
    }

    // check amitran dumps
    if (OptionsCont::getOptions().isSet("amitran-output")) {
        MSAmitranTrajectories::write(OutputDevice::getDeviceByOption("amitran-output"), myStep);
    }

    // check vtk dumps
    if (OptionsCont::getOptions().isSet("vtk-output")) {

        if (MSNet::getInstance()->getVehicleControl().getRunningVehicleNo() > 0) {
            std::string timestep = time2string(myStep);
            timestep = timestep.substr(0, timestep.length() - 3);
            std::string output = OptionsCont::getOptions().getString("vtk-output");
            std::string filename = output + "_" + timestep + ".vtp";

            OutputDevice_File dev = OutputDevice_File(filename, false);

            //build a huge mass of xml files
            MSVTKExport::write(dev, myStep);

        }

    }

    // summary output
    if (OptionsCont::getOptions().isSet("summary-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("summary-output");
        unsigned int departedVehiclesNumber = myVehicleControl->getDepartedVehicleNo();
        const SUMOReal meanWaitingTime = departedVehiclesNumber != 0 ? myVehicleControl->getTotalDepartureDelay() / (SUMOReal) departedVehiclesNumber : -1.;
        unsigned int endedVehicleNumber = myVehicleControl->getEndedVehicleNo();
        const SUMOReal meanTravelTime = endedVehicleNumber != 0 ? myVehicleControl->getTotalTravelTime() / (SUMOReal) endedVehicleNumber : -1.;
        od.openTag("step").writeAttr("time", time2string(myStep)).writeAttr("loaded", myVehicleControl->getLoadedVehicleNo())
        .writeAttr("inserted", myVehicleControl->getDepartedVehicleNo()).writeAttr("running", myVehicleControl->getRunningVehicleNo())
        .writeAttr("waiting", myInserter->getWaitingVehicleNo()).writeAttr("ended", myVehicleControl->getEndedVehicleNo())
        .writeAttr("meanWaitingTime", meanWaitingTime).writeAttr("meanTravelTime", meanTravelTime);
        if (myLogExecutionTime) {
            od.writeAttr("duration", mySimStepDuration);
        }
        od.closeTag();
    }

    // write detector values
    myDetectorControl->writeOutput(myStep + DELTA_T, false);

    // write link states
    if (OptionsCont::getOptions().isSet("link-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("link-output");
        od.openTag("timestep");
        od.writeAttr(SUMO_ATTR_ID, STEPS2TIME(myStep));
        const std::vector<MSEdge*>& edges = myEdges->getEdges();
        for (std::vector<MSEdge*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            const std::vector<MSLane*>& lanes = (*i)->getLanes();
            for (std::vector<MSLane*>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
                const std::vector<MSLink*>& links = (*j)->getLinkCont();
                for (std::vector<MSLink*>::const_iterator k = links.begin(); k != links.end(); ++k) {
                    (*k)->writeApproaching(od, (*j)->getID());
                }
            }
        }
        od.closeTag();
    }
}


bool
MSNet::logSimulationDuration() const {
    return myLogExecutionTime;
}


MSPersonControl&
MSNet::getPersonControl() {
    if (myPersonControl == 0) {
        myPersonControl = new MSPersonControl();
    }
    return *myPersonControl;
}


MSEdgeWeightsStorage&
MSNet::getWeightsStorage() {
    if (myEdgeWeights == 0) {
        myEdgeWeights = new MSEdgeWeightsStorage();
    }
    return *myEdgeWeights;
}


void
MSNet::preSimStepOutput() const {
    std::cout << "Step #" << time2string(myStep);
}


void
MSNet::postSimStepOutput() const {
    if (myLogExecutionTime) {
        std::ostringstream oss;
        oss.setf(std::ios::fixed , std::ios::floatfield);    // use decimal format
        oss.setf(std::ios::showpoint);    // print decimal point
        oss << std::setprecision(OUTPUT_ACCURACY);
        if (mySimStepDuration != 0) {
            oss << " (" << mySimStepDuration << "ms ~= "
                << (1000. / (SUMOReal) mySimStepDuration) << "*RT, ~"
                << ((SUMOReal) myVehicleControl->getRunningVehicleNo() / (SUMOReal) mySimStepDuration * 1000.);
        } else {
            oss << " (0ms ?*RT. ?";
        }
        oss << "UPS, vehicles"
            << " TOT " << myVehicleControl->getDepartedVehicleNo()
            << " ACT " << myVehicleControl->getRunningVehicleNo()
            << ")                                              ";
        std::string prev = "Step #" + time2string(myStep - DELTA_T);
        std::cout << oss.str().substr(0, 78 - prev.length());
    }
    std::cout << '\r';
}


void
MSNet::addVehicleStateListener(VehicleStateListener* listener) {
    if (find(myVehicleStateListeners.begin(), myVehicleStateListeners.end(), listener) == myVehicleStateListeners.end()) {
        myVehicleStateListeners.push_back(listener);
    }
}


void
MSNet::removeVehicleStateListener(VehicleStateListener* listener) {
    std::vector<VehicleStateListener*>::iterator i = find(myVehicleStateListeners.begin(), myVehicleStateListeners.end(), listener);
    if (i != myVehicleStateListeners.end()) {
        myVehicleStateListeners.erase(i);
    }
}


void
MSNet::informVehicleStateListener(const SUMOVehicle* const vehicle, VehicleState to) {
    for (std::vector<VehicleStateListener*>::iterator i = myVehicleStateListeners.begin(); i != myVehicleStateListeners.end(); ++i) {
        (*i)->vehicleStateChanged(vehicle, to);
    }
}



// ------ Insertion and retrieval of bus stops ------
bool
MSNet::addBusStop(MSBusStop* busStop) {
    return myBusStopDict.add(busStop->getID(), busStop);
}


MSBusStop*
MSNet::getBusStop(const std::string& id) const {
    return myBusStopDict.get(id);
}


std::string
MSNet::getBusStopID(const MSLane* lane, const SUMOReal pos) const {
    const std::map<std::string, MSBusStop*>& vals = myBusStopDict.getMyMap();
    for (std::map<std::string, MSBusStop*>::const_iterator it = vals.begin(); it != vals.end(); ++it) {
        MSBusStop* stop = it->second;
        if (&stop->getLane() == lane && fabs(stop->getEndLanePosition() - pos) < POSITION_EPS) {
            return stop->getID();
        }
    }
    return "";
}


SUMOAbstractRouter<MSEdge, SUMOVehicle>&
MSNet::getRouterTT(const std::vector<MSEdge*>& prohibited) const {
    if (!myRouterTTInitialized) {
        myRouterTTInitialized = true;
        const std::string routingAlgorithm = OptionsCont::getOptions().getString("routing-algorithm");
        if (routingAlgorithm == "dijkstra") {
            myRouterTTDijkstra = new DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> >(
                MSEdge::numericalDictSize(), true, &MSNet::getTravelTime);
        } else {
            if (routingAlgorithm != "astar") {
                WRITE_WARNING("TraCI and Triggers cannot use routing algorithm '" + routingAlgorithm + "'. using 'astar' instead.");
            }
            myRouterTTAStar = new AStarRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> >(
                MSEdge::numericalDictSize(), true, &MSNet::getTravelTime);
        }
    }
    if (myRouterTTDijkstra != 0) {
        myRouterTTDijkstra->prohibit(prohibited);
        return *myRouterTTDijkstra;
    } else {
        assert(myRouterTTAStar != 0);
        myRouterTTAStar->prohibit(prohibited);
        return *myRouterTTAStar;
    }
}


SUMOAbstractRouter<MSEdge, SUMOVehicle>&
MSNet::getRouterEffort(const std::vector<MSEdge*>& prohibited) const {
    if (myRouterEffort == 0) {
        myRouterEffort = new DijkstraRouterEffort_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> >(
            MSEdge::numericalDictSize(), true, &MSNet::getEffort, &MSNet::getTravelTime);
    }
    myRouterEffort->prohibit(prohibited);
    return *myRouterEffort;
}


MSNet::MSPedestrianRouterDijkstra&
MSNet::getPedestrianRouter(const std::vector<MSEdge*>& prohibited) const {
    if (myPedestrianRouter == 0) {
        myPedestrianRouter = new MSPedestrianRouterDijkstra();
    }
    myPedestrianRouter->prohibit(prohibited);
    return *myPedestrianRouter;
}


const NamedRTree&
MSNet::getLanesRTree() const {
    if (!myLanesRTree.first) {
        MSLane::fill(myLanesRTree.second);
        myLanesRTree.first = true;
    }
    return myLanesRTree.second;
}


/****************************************************************************/
