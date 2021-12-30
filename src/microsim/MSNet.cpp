/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
///
// The simulated network and simulation perfomer
/****************************************************************************/
#include <config.h>

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

#include "trigger/MSTrigger.h"
#include "trigger/MSCalibrator.h"
#include "traffic_lights/MSTLLogicControl.h"
#include "MSVehicleControl.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/SysUtils.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/WrappingCommand.h>
#include <utils/common/SystemFrame.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/iodevices/OutputDevice_File.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/router/DijkstraRouter.h>
#include <utils/router/AStarRouter.h>
#include <utils/router/IntermodalRouter.h>
#include <utils/router/PedestrianRouter.h>
#include <utils/vehicle/SUMORouteLoaderControl.h>
#include <utils/xml/XMLSubSys.h>
#include <traci-server/TraCIServer.h>
#include <libsumo/Simulation.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/devices/MSRoutingEngine.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_Tripinfo.h>
#include <microsim/devices/MSDevice_BTsender.h>
#include <microsim/devices/MSDevice_SSM.h>
#include <microsim/devices/MSDevice_ElecHybrid.h>
#include <microsim/devices/MSDevice_ToC.h>
#include <microsim/devices/MSDevice_Taxi.h>
#include <microsim/output/MSBatteryExport.h>
#include <microsim/output/MSElecHybridExport.h>
#include <microsim/output/MSEmissionExport.h>
#include <microsim/output/MSFCDExport.h>
#include <microsim/output/MSFullExport.h>
#include <microsim/output/MSQueueExport.h>
#include <microsim/output/MSVTKExport.h>
#include <microsim/output/MSXMLRawOut.h>
#include <microsim/output/MSAmitranTrajectories.h>
#include <microsim/output/MSStopOut.h>
#include <microsim/transportables/MSPModel.h>
#include <microsim/transportables/MSPerson.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSRailSignal.h>
#include <microsim/traffic_lights/MSRailSignalConstraint.h>
#include <microsim/traffic_lights/MSRailSignalControl.h>
#include <microsim/trigger/MSChargingStation.h>
#include <microsim/trigger/MSOverheadWire.h>
#include <utils/router/FareModul.h>

#include <microsim/transportables/MSTransportableControl.h>
#include "MSEdgeControl.h"
#include "MSJunctionControl.h"
#include "MSInsertionControl.h"
#include "MSDynamicShapeUpdater.h"
#include "MSEventControl.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSJunctionLogic.h"
#include "MSLane.h"
#include "MSVehicleTransfer.h"
#include "MSRoute.h"
#include "MSGlobals.h"
#include "MSEdgeWeightsStorage.h"
#include "MSStateHandler.h"
#include "MSFrame.h"
#include "MSParkingArea.h"
#include "MSStoppingPlace.h"
#include "MSNet.h"


// ===========================================================================
// debug constants
// ===========================================================================
//#define DEBUG_SIMSTEP


// ===========================================================================
// static member definitions
// ===========================================================================
MSNet* MSNet::myInstance = nullptr;

const std::string MSNet::STAGE_EVENTS("events");
const std::string MSNet::STAGE_MOVEMENTS("move");
const std::string MSNet::STAGE_LANECHANGE("laneChange");
const std::string MSNet::STAGE_INSERTIONS("insertion");

// ===========================================================================
// static member method definitions
// ===========================================================================
double
MSNet::getEffort(const MSEdge* const e, const SUMOVehicle* const v, double t) {
    double value;
    const MSVehicle* const veh = dynamic_cast<const MSVehicle* const>(v);
    if (veh != nullptr && veh->getWeightsStorage().retrieveExistingEffort(e, t, value)) {
        return value;
    }
    if (getInstance()->getWeightsStorage().retrieveExistingEffort(e, t, value)) {
        return value;
    }
    return 0;
}


double
MSNet::getTravelTime(const MSEdge* const e, const SUMOVehicle* const v, double t) {
    double value;
    const MSVehicle* const veh = dynamic_cast<const MSVehicle* const>(v);
    if (veh != nullptr && veh->getWeightsStorage().retrieveExistingTravelTime(e, t, value)) {
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
    if (myInstance != nullptr) {
        return myInstance;
    }
    throw ProcessError("A network was not yet constructed.");
}

void
MSNet::initStatic() {
    if (!MSGlobals::gUseMesoSim) {
        MSVehicle::Influencer::init();
    }
}

void
MSNet::cleanupStatic() {
    if (!MSGlobals::gUseMesoSim) {
        MSVehicle::Influencer::cleanup();
    }
}


MSNet::MSNet(MSVehicleControl* vc, MSEventControl* beginOfTimestepEvents,
             MSEventControl* endOfTimestepEvents,
             MSEventControl* insertionEvents,
             ShapeContainer* shapeCont):
    myAmInterrupted(false),
    myVehiclesMoved(0),
    myPersonsMoved(0),
    myHavePermissions(false),
    myHasInternalLinks(false),
    myJunctionHigherSpeeds(false),
    myHasElevation(false),
    myHasPedestrianNetwork(false),
    myHasBidiEdges(false),
    myEdgeDataEndTime(-1),
    myDynamicShapeUpdater(nullptr) {
    if (myInstance != nullptr) {
        throw ProcessError("A network was already constructed.");
    }
    OptionsCont& oc = OptionsCont::getOptions();
    myStep = string2time(oc.getString("begin"));
    myMaxTeleports = oc.getInt("max-num-teleports");
    myLogExecutionTime = !oc.getBool("no-duration-log");
    myLogStepNumber = !oc.getBool("no-step-log");
    myLogStepPeriod = oc.getInt("step-log.period");
    myInserter = new MSInsertionControl(*vc, string2time(oc.getString("max-depart-delay")), oc.getBool("eager-insert"), oc.getInt("max-num-vehicles"),
                                        string2time(oc.getString("random-depart-offset")));
    myVehicleControl = vc;
    myDetectorControl = new MSDetectorControl();
    myEdges = nullptr;
    myJunctions = nullptr;
    myRouteLoaders = nullptr;
    myLogics = nullptr;
    myPersonControl = nullptr;
    myContainerControl = nullptr;
    myEdgeWeights = nullptr;
    myShapeContainer = shapeCont == nullptr ? new ShapeContainer() : shapeCont;

    myBeginOfTimestepEvents = beginOfTimestepEvents;
    myEndOfTimestepEvents = endOfTimestepEvents;
    myInsertionEvents = insertionEvents;
    myLanesRTree.first = false;

    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gMesoNet = new MELoop(string2time(oc.getString("meso-recheck")));
    }
    myInstance = this;
    initStatic();
}


void
MSNet::closeBuilding(const OptionsCont& oc, MSEdgeControl* edges, MSJunctionControl* junctions,
                     SUMORouteLoaderControl* routeLoaders,
                     MSTLLogicControl* tlc,
                     std::vector<SUMOTime> stateDumpTimes,
                     std::vector<std::string> stateDumpFiles,
                     bool hasInternalLinks,
                     bool junctionHigherSpeeds,
                     double version) {
    myEdges = edges;
    myJunctions = junctions;
    myRouteLoaders = routeLoaders;
    myLogics = tlc;
    // save the time the network state shall be saved at
    myStateDumpTimes = stateDumpTimes;
    myStateDumpFiles = stateDumpFiles;
    myStateDumpPeriod = string2time(oc.getString("save-state.period"));
    myStateDumpPrefix = oc.getString("save-state.prefix");
    myStateDumpSuffix = oc.getString("save-state.suffix");

    // initialise performance computation
    mySimBeginMillis = SysUtils::getCurrentMillis();
    myTraCIMillis = 0;
    myHasInternalLinks = hasInternalLinks;
    myJunctionHigherSpeeds = junctionHigherSpeeds;
    myHasElevation = checkElevation();
    myHasPedestrianNetwork = checkWalkingarea();
    myHasBidiEdges = checkBidiEdges();
    myVersion = version;
    if ((!MSGlobals::gUsingInternalLanes || !myHasInternalLinks)
            && MSGlobals::gWeightsSeparateTurns > 0) {
        throw ProcessError("Option weights.separate-turns is only supported when simulating with internal lanes");
    }
}


MSNet::~MSNet() {
    cleanupStatic();
    // delete controls
    delete myJunctions;
    delete myDetectorControl;
    // delete mean data
    delete myEdges;
    delete myInserter;
    delete myLogics;
    delete myRouteLoaders;
    if (myPersonControl != nullptr) {
        delete myPersonControl;
    }
    if (myContainerControl != nullptr) {
        delete myContainerControl;
    }
    delete myVehicleControl; // must happen after deleting transportables
    // delete events late so that vehicles can get rid of references first
    delete myBeginOfTimestepEvents;
    myBeginOfTimestepEvents = nullptr;
    delete myEndOfTimestepEvents;
    myEndOfTimestepEvents = nullptr;
    delete myInsertionEvents;
    myInsertionEvents = nullptr;
    delete myShapeContainer;
    delete myEdgeWeights;
    for (auto& router : myRouterTT) {
        delete router.second;
    }
    myRouterTT.clear();
    for (auto& router : myRouterEffort) {
        delete router.second;
    }
    myRouterEffort.clear();
    for (auto& router : myPedestrianRouter) {
        delete router.second;
    }
    myPedestrianRouter.clear();
    for (auto& router : myIntermodalRouter) {
        delete router.second;
    }
    myIntermodalRouter.clear();
    myLanesRTree.second.RemoveAll();
    clearAll();
    if (MSGlobals::gUseMesoSim) {
        delete MSGlobals::gMesoNet;
    }
    myInstance = nullptr;
}


void
MSNet::addRestriction(const std::string& id, const SUMOVehicleClass svc, const double speed) {
    myRestrictions[id][svc] = speed;
}


const std::map<SUMOVehicleClass, double>*
MSNet::getRestrictions(const std::string& id) const {
    std::map<std::string, std::map<SUMOVehicleClass, double> >::const_iterator i = myRestrictions.find(id);
    if (i == myRestrictions.end()) {
        return nullptr;
    }
    return &i->second;
}

void
MSNet::addMesoType(const std::string& typeID, const MesoEdgeType& edgeType) {
    myMesoEdgeTypes[typeID] = edgeType;
}

const MSNet::MesoEdgeType&
MSNet::getMesoType(const std::string& typeID) {
    if (myMesoEdgeTypes.count(typeID) == 0) {
        // init defaults
        const OptionsCont& oc = OptionsCont::getOptions();
        MesoEdgeType edgeType;
        edgeType.tauff = string2time(oc.getString("meso-tauff"));
        edgeType.taufj = string2time(oc.getString("meso-taufj"));
        edgeType.taujf = string2time(oc.getString("meso-taujf"));
        edgeType.taujj = string2time(oc.getString("meso-taujj"));
        edgeType.jamThreshold = oc.getFloat("meso-jam-threshold");
        edgeType.junctionControl = oc.getBool("meso-junction-control");
        edgeType.tlsPenalty = oc.getFloat("meso-tls-penalty");
        edgeType.tlsFlowPenalty = oc.getFloat("meso-tls-flow-penalty");
        edgeType.minorPenalty = string2time(oc.getString("meso-minor-penalty"));
        edgeType.overtaking = oc.getBool("meso-overtaking");
        myMesoEdgeTypes[typeID] = edgeType;
    }
    return myMesoEdgeTypes[typeID];
}

MSNet::SimulationState
MSNet::simulate(SUMOTime start, SUMOTime stop) {
    // report the begin when wished
    WRITE_MESSAGE("Simulation version " + std::string(VERSION_STRING) + " started with time: " + time2string(start));
    // the simulation loop
    SimulationState state = SIMSTATE_RUNNING;
    // state loading may have changed the start time so we need to reinit it
    myStep = start;
    int numSteps = 0;
    bool doStepLog = false;
    while (state == SIMSTATE_RUNNING) {
        doStepLog = myLogStepNumber && (numSteps % myLogStepPeriod == 0);
        if (doStepLog) {
            preSimStepOutput();
        }
        simulationStep();
        if (doStepLog) {
            postSimStepOutput();
        }
        state = adaptToState(simulationState(stop));
#ifdef DEBUG_SIMSTEP
        std::cout << SIMTIME << " MSNet::simulate(" << start << ", " << stop << ")"
                  << "\n simulation state: " << getStateMessage(state)
                  << std::endl;
#endif
        numSteps++;
    }
    if (myLogStepNumber && !doStepLog) {
        // ensure some output on the last step
        preSimStepOutput();
        postSimStepOutput();
    }
    // exit simulation loop
    if (myLogStepNumber) {
        // start new line for final verbose output
        std::cout << "\n";
    }
    closeSimulation(start, getStateMessage(state));
    return state;
}


void
MSNet::loadRoutes() {
    myRouteLoaders->loadNext(myStep);
}


const std::string
MSNet::generateStatistics(SUMOTime start) {
    std::ostringstream msg;
    if (myLogExecutionTime) {
        long duration = SysUtils::getCurrentMillis() - mySimBeginMillis;
        // print performance notice
        msg << "Performance: " << "\n" << " Duration: " << elapsedMs2string(duration) << "\n";
        if (duration != 0) {
            if (TraCIServer::getInstance() != nullptr) {
                msg << " TraCI-Duration: " << elapsedMs2string(myTraCIMillis) << "\n";
            }
            msg << " Real time factor: " << (STEPS2TIME(myStep - start) * 1000. / (double)duration) << "\n";
            msg.setf(std::ios::fixed, std::ios::floatfield);     // use decimal format
            msg.setf(std::ios::showpoint);    // print decimal point
            msg << " UPS: " << ((double)myVehiclesMoved / ((double)duration / 1000)) << "\n";
            if (myPersonsMoved > 0) {
                msg << " UPS-Persons: " << ((double)myPersonsMoved / ((double)duration / 1000)) << "\n";
            }
        }
        // print vehicle statistics
        const std::string discardNotice = ((myVehicleControl->getLoadedVehicleNo() != myVehicleControl->getDepartedVehicleNo()) ?
                                           " (Loaded: " + toString(myVehicleControl->getLoadedVehicleNo()) + ")" : "");
        msg << "Vehicles: " << "\n"
            << " Inserted: " << myVehicleControl->getDepartedVehicleNo() << discardNotice << "\n"
            << " Running: " << myVehicleControl->getRunningVehicleNo() << "\n"
            << " Waiting: " << myInserter->getWaitingVehicleNo() << "\n";

        if (myVehicleControl->getTeleportCount() > 0 || myVehicleControl->getCollisionCount() > 0) {
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
        if (myPersonControl != nullptr && myPersonControl->getLoadedNumber() > 0) {
            msg << "Persons: " << "\n"
                << " Inserted: " << myPersonControl->getLoadedNumber() << "\n"
                << " Running: " << myPersonControl->getRunningNumber() << "\n";
            if (myPersonControl->getJammedNumber() > 0) {
                msg << " Jammed: " << myPersonControl->getJammedNumber() << "\n";
            }
        }
        if (myContainerControl != nullptr && myContainerControl->getLoadedNumber() > 0) {
            msg << "Containers: " << "\n"
                << " Inserted: " << myContainerControl->getLoadedNumber() << "\n"
                << " Running: " << myContainerControl->getRunningNumber() << "\n";
            if (myContainerControl->getJammedNumber() > 0) {
                msg << " Jammed: " << myContainerControl->getJammedNumber() << "\n";
            }
        }
    }
    if (OptionsCont::getOptions().getBool("duration-log.statistics")) {
        msg << MSDevice_Tripinfo::printStatistics();
    }
    return msg.str();
}

void
MSNet::writeCollisions() const {
    OutputDevice& od = OutputDevice::getDeviceByOption("collision-output");
    for (const auto& item : myCollisions) {
        for (const auto& c : item.second) {
            od.openTag("collision");
            od.writeAttr("time", time2string(getCurrentTimeStep()));
            od.writeAttr("type", c.type);
            od.writeAttr("lane", c.lane->getID());
            od.writeAttr("pos", c.pos);
            od.writeAttr("collider", item.first);
            od.writeAttr("victim", c.victim);
            od.writeAttr("colliderType", c.colliderType);
            od.writeAttr("victimType", c.victimType);
            od.writeAttr("colliderSpeed", c.colliderSpeed);
            od.writeAttr("victimSpeed", c.victimSpeed);
            od.closeTag();
        }
    }

}

void
MSNet::writeStatistics() const {
    OutputDevice& od = OutputDevice::getDeviceByOption("statistic-output");
    od.openTag("vehicles");
    od.writeAttr("loaded", myVehicleControl->getLoadedVehicleNo());
    od.writeAttr("inserted", myVehicleControl->getDepartedVehicleNo());
    od.writeAttr("running", myVehicleControl->getRunningVehicleNo());
    od.writeAttr("waiting", myInserter->getWaitingVehicleNo());
    od.closeTag();
    od.openTag("teleports");
    od.writeAttr("total", myVehicleControl->getTeleportCount());
    od.writeAttr("jam", myVehicleControl->getTeleportsJam());
    od.writeAttr("yield", myVehicleControl->getTeleportsYield());
    od.writeAttr("wrongLane", myVehicleControl->getTeleportsWrongLane());
    od.closeTag();
    od.openTag("safety");
    od.writeAttr("collisions", myVehicleControl->getCollisionCount());
    od.writeAttr("emergencyStops", myVehicleControl->getEmergencyStops());
    od.closeTag();
    od.openTag("persons");
    od.writeAttr("loaded", myPersonControl != nullptr ? myPersonControl->getLoadedNumber() : 0);
    od.writeAttr("running", myPersonControl != nullptr ? myPersonControl->getRunningNumber() : 0);
    od.writeAttr("jammed", myPersonControl != nullptr ? myPersonControl->getJammedNumber() : 0);
    od.closeTag();
    if (OptionsCont::getOptions().isSet("tripinfo-output") || OptionsCont::getOptions().getBool("duration-log.statistics")) {
        MSDevice_Tripinfo::writeStatistics(od);
    }

}

void
MSNet::closeSimulation(SUMOTime start, const std::string& reason) {
    // report the end when wished
    WRITE_MESSAGE("Simulation ended at time: " + time2string(getCurrentTimeStep()));
    if (reason != "") {
        WRITE_MESSAGE("Reason: " + reason);
    }
    myDetectorControl->close(myStep);
    if (MSStopOut::active() && OptionsCont::getOptions().getBool("stop-output.write-unfinished")) {
        MSStopOut::getInstance()->generateOutputForUnfinished();
    }
    if (OptionsCont::getOptions().getBool("vehroute-output.write-unfinished")) {
        MSDevice_Vehroutes::generateOutputForUnfinished();
    }
    if (OptionsCont::getOptions().getBool("tripinfo-output.write-unfinished")) {
        MSDevice_Tripinfo::generateOutputForUnfinished();
    }
    if (OptionsCont::getOptions().isSet("chargingstations-output")) {
        writeChargingStationOutput();
    }
    if (OptionsCont::getOptions().isSet("overheadwiresegments-output")) {
        writeOverheadWireSegmentOutput();
    }
    if (OptionsCont::getOptions().isSet("substations-output")) {
        writeSubstationOutput();
    }
    if (OptionsCont::getOptions().isSet("railsignal-block-output")) {
        writeRailSignalBlocks();
    }
    if (myLogExecutionTime || OptionsCont::getOptions().getBool("duration-log.statistics")) {
        WRITE_MESSAGE(generateStatistics(start));
    }
    if (OptionsCont::getOptions().isSet("statistic-output")) {
        writeStatistics();
    }
}


void
MSNet::simulationStep() {
#ifdef DEBUG_SIMSTEP
    std::cout << SIMTIME << ": MSNet::simulationStep() called"
              << ", myStep = " << myStep
              << std::endl;
#endif
    TraCIServer* t = TraCIServer::getInstance();
    if (t != nullptr) {
        if (myLogExecutionTime) {
            myTraCIStepDuration = SysUtils::getCurrentMillis();
        }
        t->processCommandsUntilSimStep(myStep);
#ifdef DEBUG_SIMSTEP
        bool loadRequested = !TraCI::getLoadArgs().empty();
        assert(t->getTargetTime() >= myStep || loadRequested || TraCIServer::wasClosed());
#endif
        if (myLogExecutionTime) {
            myTraCIStepDuration = SysUtils::getCurrentMillis() - myTraCIStepDuration;
        }
        if (TraCIServer::wasClosed()) {
            return;
        }
    }
#ifdef DEBUG_SIMSTEP
    std::cout << SIMTIME << ": TraCI target time: " << t->getTargetTime() << std::endl;
#endif
    // execute beginOfTimestepEvents
    if (myLogExecutionTime) {
        mySimStepDuration = SysUtils::getCurrentMillis();
    }
    // simulation state output
    std::vector<SUMOTime>::iterator timeIt = std::find(myStateDumpTimes.begin(), myStateDumpTimes.end(), myStep);
    if (timeIt != myStateDumpTimes.end()) {
        const int dist = (int)distance(myStateDumpTimes.begin(), timeIt);
        MSStateHandler::saveState(myStateDumpFiles[dist], myStep);
    }
    if (myStateDumpPeriod > 0 && myStep % myStateDumpPeriod == 0) {
        std::string timeStamp = time2string(myStep);
        std::replace(timeStamp.begin(), timeStamp.end(), ':', '-');
        const std::string filename = myStateDumpPrefix + "_" + timeStamp + myStateDumpSuffix;
        MSStateHandler::saveState(filename, myStep);
        myPeriodicStateFiles.push_back(filename);
        int keep = OptionsCont::getOptions().getInt("save-state.period.keep");
        if (keep > 0 && (int)myPeriodicStateFiles.size() > keep) {
            std::remove(myPeriodicStateFiles.front().c_str());
            myPeriodicStateFiles.erase(myPeriodicStateFiles.begin());
        }
    }
    myBeginOfTimestepEvents->execute(myStep);
    MSRailSignal::recheckGreen();
#ifdef HAVE_FOX
    MSRoutingEngine::waitForAll();
#endif
    if (MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions(myStep, STAGE_EVENTS);
    }
    // check whether the tls programs need to be switched
    myLogics->check2Switch(myStep);

    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gMesoNet->simulate(myStep);
        myVehicleControl->removePending();
    } else {
        // assure all lanes with vehicles are 'active'
        myEdges->patchActiveLanes();

        // compute safe velocities for all vehicles for the next few lanes
        // also register ApproachingVehicleInformation for all links
        myEdges->planMovements(myStep);

        // register junction approaches based on planned velocities as basis for right-of-way decision
        myEdges->setJunctionApproaches(myStep);

        // decide right-of-way and execute movements
        myEdges->executeMovements(myStep);
        if (MSGlobals::gCheck4Accidents) {
            myEdges->detectCollisions(myStep, STAGE_MOVEMENTS);
        }

        // vehicles may change lanes
        myEdges->changeLanes(myStep);

        if (MSGlobals::gCheck4Accidents) {
            myEdges->detectCollisions(myStep, STAGE_LANECHANGE);
        }
    }
    loadRoutes();

    // persons
    if (myPersonControl != nullptr && myPersonControl->hasTransportables()) {
        myPersonControl->checkWaiting(this, myStep);
    }
    // containers
    if (myContainerControl != nullptr && myContainerControl->hasTransportables()) {
        myContainerControl->checkWaiting(this, myStep);
    }
    // insert vehicles
    myInserter->determineCandidates(myStep);
    myInsertionEvents->execute(myStep);
#ifdef HAVE_FOX
    MSRoutingEngine::waitForAll();
#endif
    myInserter->emitVehicles(myStep);
    if (MSGlobals::gCheck4Accidents) {
        //myEdges->patchActiveLanes(); // @note required to detect collisions on lanes that were empty before insertion. wasteful?
        myEdges->detectCollisions(myStep, STAGE_INSERTIONS);
    }
    MSVehicleTransfer::getInstance()->checkInsertions(myStep);

    // execute endOfTimestepEvents
    myEndOfTimestepEvents->execute(myStep);

    if (myLogExecutionTime) {
        myTraCIStepDuration -= SysUtils::getCurrentMillis();
    }
    libsumo::Helper::postProcessRemoteControl();
    if (myLogExecutionTime) {
        myTraCIStepDuration += SysUtils::getCurrentMillis();
        myTraCIMillis += myTraCIStepDuration;
    }
    if (MSGlobals::gCheck4Accidents) {
        // collisions from the previous step were kept to avoid duplicate
        // warnings. we must remove them now to ensure correct output.
        removeOutdatedCollisions();
    }
    // update and write (if needed) detector values
    writeOutput();

    if (myLogExecutionTime) {
        mySimStepDuration = SysUtils::getCurrentMillis() - mySimStepDuration;
        myVehiclesMoved += myVehicleControl->getRunningVehicleNo();
        if (myPersonControl != nullptr) {
            myPersonsMoved += myPersonControl->getRunningNumber();
        }
    }
    myStep += DELTA_T;
}


MSNet::SimulationState
MSNet::simulationState(SUMOTime stopTime) const {
    if (TraCIServer::wasClosed()) {
        return SIMSTATE_CONNECTION_CLOSED;
    }
    if (TraCIServer::getInstance() != nullptr && !TraCIServer::getInstance()->getLoadArgs().empty()) {
        return SIMSTATE_LOADING;
    }
    if ((stopTime < 0 || myStep > stopTime) && TraCIServer::getInstance() == nullptr && (stopTime > 0 || myStep > myEdgeDataEndTime)) {
        if ((myVehicleControl->getActiveVehicleCount() == 0)
                && (myInserter->getPendingFlowCount() == 0)
                && (myPersonControl == nullptr || !myPersonControl->hasNonWaiting())
                && (myContainerControl == nullptr || !myContainerControl->hasNonWaiting())
                && !MSDevice_Taxi::hasServableReservations()) {
            return SIMSTATE_NO_FURTHER_VEHICLES;
        }
    }
    if (stopTime >= 0 && myStep >= stopTime) {
        return SIMSTATE_END_STEP_REACHED;
    }
    if (myMaxTeleports >= 0 && myVehicleControl->getTeleportCount() > myMaxTeleports) {
        return SIMSTATE_TOO_MANY_TELEPORTS;
    }
    if (myAmInterrupted) {
        return SIMSTATE_INTERRUPTED;
    }
    return SIMSTATE_RUNNING;
}


MSNet::SimulationState
MSNet::adaptToState(MSNet::SimulationState state) const {
    if (state == SIMSTATE_LOADING) {
        OptionsIO::setArgs(TraCIServer::getInstance()->getLoadArgs());
        TraCIServer::getInstance()->getLoadArgs().clear();
    } else if (state != SIMSTATE_RUNNING && TraCIServer::getInstance() != nullptr && !TraCIServer::wasClosed()) {
        // overrides SIMSTATE_END_STEP_REACHED, e.g. (TraCI ignore SUMO's --end option)
        return SIMSTATE_RUNNING;
    } else if (state == SIMSTATE_NO_FURTHER_VEHICLES) {
        if (myPersonControl != nullptr) {
            myPersonControl->abortAnyWaitingForVehicle();
        }
        if (myContainerControl != nullptr) {
            myContainerControl->abortAnyWaitingForVehicle();
        }
        myVehicleControl->abortWaiting();
    }
    return state;
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
            return "An error occurred (see log).";
        case MSNet::SIMSTATE_INTERRUPTED:
            return "Interrupted.";
        case MSNet::SIMSTATE_TOO_MANY_TELEPORTS:
            return "Too many teleports.";
        case MSNet::SIMSTATE_LOADING:
            return "TraCI issued load command.";
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
    MSDevice::cleanupAll();
    MSTrigger::cleanup();
    MSCalibrator::cleanup();
    MSDevice_BTsender::cleanup();
    MSDevice_SSM::cleanup();
    MSDevice_ToC::cleanup();
    MSStopOut::cleanup();
    MSRailSignalConstraint::cleanup();
    MSRailSignalControl::cleanup();
    TraCIServer* t = TraCIServer::getInstance();
    if (t != nullptr) {
        t->cleanup();
    }
    libsumo::Helper::cleanup();
    OutputDevice::closeAll(true);
}


void
MSNet::clearState(const SUMOTime step) {
    MSGlobals::gClearState = true;
    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gMesoNet->clearState();
        for (int i = 0; i < MSEdge::dictSize(); i++) {
            for (MESegment* s = MSGlobals::gMesoNet->getSegmentForEdge(*MSEdge::getAllEdges()[i]); s != nullptr; s = s->getNextSegment()) {
                s->clearState();
            }
        }
    } else {
        for (int i = 0; i < MSEdge::dictSize(); i++) {
            const std::vector<MSLane*>& lanes = MSEdge::getAllEdges()[i]->getLanes();
            for (MSLane* lane : lanes) {
                lane->getVehiclesSecure();
                lane->clearState();
                lane->releaseVehicles();
            }
        }
    }
    myInserter->clearState();
    myVehicleControl->clearState(true);
    MSVehicleTransfer::getInstance()->clearState();
    MSRoute::dict_clearState(); // delete all routes after vehicles are deleted
    myLogics->clearState();
    myDetectorControl->updateDetectors(myStep);
    myDetectorControl->writeOutput(myStep, true);
    myDetectorControl->clearState();
    for (auto& item : myStoppingPlaces) {
        for (auto& item2 : item.second) {
            item2.second->clearState();
        }
    }
    myShapeContainer->clearState();
    myBeginOfTimestepEvents->clearState(myStep, step);
    myEndOfTimestepEvents->clearState(myStep, step);
    myInsertionEvents->clearState(myStep, step);
    if (myPersonControl != nullptr) {
        myPersonControl->clearState();
    }
    if (myContainerControl != nullptr) {
        myContainerControl->clearState();
    }
    myStep = step;
    MSGlobals::gClearState = false;
}


void
MSNet::writeOutput() {
    // update detector values
    myDetectorControl->updateDetectors(myStep);
    const OptionsCont& oc = OptionsCont::getOptions();

    // check state dumps
    if (oc.isSet("netstate-dump")) {
        MSXMLRawOut::write(OutputDevice::getDeviceByOption("netstate-dump"), *myEdges, myStep,
                           oc.getInt("netstate-dump.precision"));
    }

    // check fcd dumps
    if (OptionsCont::getOptions().isSet("fcd-output")) {
        MSFCDExport::write(OutputDevice::getDeviceByOption("fcd-output"), myStep, myHasElevation);
    }

    // check emission dumps
    if (OptionsCont::getOptions().isSet("emission-output")) {
        MSEmissionExport::write(OutputDevice::getDeviceByOption("emission-output"), myStep,
                                oc.getInt("emission-output.precision"));
    }

    // battery dumps
    if (OptionsCont::getOptions().isSet("battery-output")) {
        MSBatteryExport::write(OutputDevice::getDeviceByOption("battery-output"), myStep,
                               oc.getInt("battery-output.precision"));
    }

    // elecHybrid dumps
    if (OptionsCont::getOptions().isSet("elechybrid-output")) {
        std::string output = OptionsCont::getOptions().getString("elechybrid-output");

        if (oc.getBool("elechybrid-output.aggregated")) {
            // build a xml file with aggregated device.elechybrid output
            MSElecHybridExport::writeAggregated(OutputDevice::getDeviceByOption("elechybrid-output"), myStep,
                                                oc.getInt("elechybrid-output.precision"));
        } else {
            // build a separate xml file for each vehicle equipped with device.elechybrid
            // RICE_TODO: Does this have to be placed here in MSNet.cpp ?
            MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
            for (MSVehicleControl::constVehIt it = vc.loadedVehBegin(); it != vc.loadedVehEnd(); ++it) {
                const SUMOVehicle* veh = it->second;
                if (!veh->isOnRoad()) {
                    continue;
                }
                if (static_cast<MSDevice_ElecHybrid*>(veh->getDevice(typeid(MSDevice_ElecHybrid))) != nullptr) {
                    std::string vehID = veh->getID();
                    std::string filename2 = output + "_" + vehID + ".xml";
                    OutputDevice& dev = OutputDevice::getDevice(filename2);
                    std::map<SumoXMLAttr, std::string> attrs;
                    attrs[SUMO_ATTR_VEHICLE] = vehID;
                    attrs[SUMO_ATTR_MAXIMUMBATTERYCAPACITY] = toString(dynamic_cast<MSDevice_ElecHybrid*>(veh->getDevice(typeid(MSDevice_ElecHybrid)))->getMaximumBatteryCapacity());
                    attrs[SUMO_ATTR_RECUPERATIONENABLE] = toString(MSGlobals::gOverheadWireRecuperation);
                    dev.writeXMLHeader("elecHybrid-export", "", attrs);
                    MSElecHybridExport::write(OutputDevice::getDevice(filename2), veh, myStep, oc.getInt("elechybrid-output.precision"));
                }
            }
        }
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

            OutputDevice_File dev(filename, false);

            //build a huge mass of xml files
            MSVTKExport::write(dev, myStep);

        }

    }

    // summary output
    if (OptionsCont::getOptions().isSet("summary-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("summary-output");
        int departedVehiclesNumber = myVehicleControl->getDepartedVehicleNo();
        const double meanWaitingTime = departedVehiclesNumber != 0 ? myVehicleControl->getTotalDepartureDelay() / (double) departedVehiclesNumber : -1.;
        int endedVehicleNumber = myVehicleControl->getEndedVehicleNo();
        const double meanTravelTime = endedVehicleNumber != 0 ? myVehicleControl->getTotalTravelTime() / (double) endedVehicleNumber : -1.;
        od.openTag("step");
        od.writeAttr("time", time2string(myStep));
        od.writeAttr("loaded", myVehicleControl->getLoadedVehicleNo());
        od.writeAttr("inserted", myVehicleControl->getDepartedVehicleNo());
        od.writeAttr("running", myVehicleControl->getRunningVehicleNo());
        od.writeAttr("waiting", myInserter->getWaitingVehicleNo());
        od.writeAttr("ended", myVehicleControl->getEndedVehicleNo());
        od.writeAttr("arrived", myVehicleControl->getArrivedVehicleNo());
        od.writeAttr("collisions", myVehicleControl->getCollisionCount());
        od.writeAttr("teleports", myVehicleControl->getTeleportCount());
        od.writeAttr("halting", myVehicleControl->getHaltingVehicleNo());
        od.writeAttr("stopped", myVehicleControl->getStoppedVehiclesCount());
        od.writeAttr("meanWaitingTime", meanWaitingTime);
        od.writeAttr("meanTravelTime", meanTravelTime);
        std::pair<double, double> meanSpeed = myVehicleControl->getVehicleMeanSpeeds();
        od.writeAttr("meanSpeed", meanSpeed.first);
        od.writeAttr("meanSpeedRelative", meanSpeed.second);
        if (myLogExecutionTime) {
            od.writeAttr("duration", mySimStepDuration);
        }
        od.closeTag();
    }
    if (OptionsCont::getOptions().isSet("person-summary-output")) {
        OutputDevice& od = OutputDevice::getDeviceByOption("person-summary-output");
        MSTransportableControl& pc = getPersonControl();
        od.openTag("step");
        od.writeAttr("time", time2string(myStep));
        od.writeAttr("loaded", pc.getLoadedNumber());
        od.writeAttr("inserted", pc.getDepartedNumber());
        od.writeAttr("walking", pc.getMovingNumber());
        od.writeAttr("waitingForRide", pc.getWaitingForVehicleNumber());
        od.writeAttr("riding", pc.getRidingNumber());
        od.writeAttr("stopping", pc.getWaitingUntilNumber());
        od.writeAttr("jammed", pc.getJammedNumber());
        od.writeAttr("ended", pc.getEndedNumber());
        od.writeAttr("arrived", pc.getArrivedNumber());
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
        for (const MSEdge* const edge : myEdges->getEdges()) {
            for (const MSLane* const lane : edge->getLanes()) {
                for (const MSLink* const link : lane->getLinkCont()) {
                    link->writeApproaching(od, lane->getID());
                }
            }
        }
        od.closeTag();
    }

    // write SSM output
    for (MSDevice_SSM* dev : MSDevice_SSM::getInstances()) {
        dev->updateAndWriteOutput();
    }

    // write ToC output
    for (MSDevice_ToC* dev : MSDevice_ToC::getInstances()) {
        if (dev->generatesOutput()) {
            dev->writeOutput();
        }
    }

    if (OptionsCont::getOptions().isSet("collision-output")) {
        writeCollisions();
    }
}


bool
MSNet::logSimulationDuration() const {
    return myLogExecutionTime;
}


MSTransportableControl&
MSNet::getPersonControl() {
    if (myPersonControl == nullptr) {
        myPersonControl = new MSTransportableControl(true);
    }
    return *myPersonControl;
}


MSTransportableControl&
MSNet::getContainerControl() {
    if (myContainerControl == nullptr) {
        myContainerControl = new MSTransportableControl(false);
    }
    return *myContainerControl;
}

MSDynamicShapeUpdater*
MSNet::makeDynamicShapeUpdater() {
    myDynamicShapeUpdater = std::unique_ptr<MSDynamicShapeUpdater> (new MSDynamicShapeUpdater(*myShapeContainer));
    return myDynamicShapeUpdater.get();
}

MSEdgeWeightsStorage&
MSNet::getWeightsStorage() {
    if (myEdgeWeights == nullptr) {
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
        oss.setf(std::ios::fixed, std::ios::floatfield);     // use decimal format
        oss.setf(std::ios::showpoint);    // print decimal point
        oss << std::setprecision(gPrecision);
        if (mySimStepDuration != 0) {
            const double durationSec = (double)mySimStepDuration / 1000.;
            oss << " (" << mySimStepDuration << "ms ~= "
                << (TS / durationSec) << "*RT, ~"
                << ((double) myVehicleControl->getRunningVehicleNo() / durationSec);
        } else {
            oss << " (0ms ?*RT. ?";
        }
        oss << "UPS, ";
        if (TraCIServer::getInstance() != nullptr) {
            oss << "TraCI: " << myTraCIStepDuration << "ms, ";
        }
        oss << "vehicles TOT " << myVehicleControl->getDepartedVehicleNo()
            << " ACT " << myVehicleControl->getRunningVehicleNo()
            << " BUF " << myInserter->getWaitingVehicleNo()
            << ")                                              ";
        std::string prev = "Step #" + time2string(myStep - DELTA_T);
        std::cout << oss.str().substr(0, 90 - prev.length());
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
    std::vector<VehicleStateListener*>::iterator i = std::find(myVehicleStateListeners.begin(), myVehicleStateListeners.end(), listener);
    if (i != myVehicleStateListeners.end()) {
        myVehicleStateListeners.erase(i);
    }
}


void
MSNet::informVehicleStateListener(const SUMOVehicle* const vehicle, VehicleState to, const std::string& info) {
#ifdef HAVE_FOX
    FXConditionalLock lock(myVehicleStateListenerMutex, MSGlobals::gNumThreads > 1);
#endif
    for (VehicleStateListener* const listener : myVehicleStateListeners) {
        listener->vehicleStateChanged(vehicle, to, info);
    }
}


void
MSNet::addTransportableStateListener(TransportableStateListener* listener) {
    if (find(myTransportableStateListeners.begin(), myTransportableStateListeners.end(), listener) == myTransportableStateListeners.end()) {
        myTransportableStateListeners.push_back(listener);
    }
}


void
MSNet::removeTransportableStateListener(TransportableStateListener* listener) {
    std::vector<TransportableStateListener*>::iterator i = std::find(myTransportableStateListeners.begin(), myTransportableStateListeners.end(), listener);
    if (i != myTransportableStateListeners.end()) {
        myTransportableStateListeners.erase(i);
    }
}


void
MSNet::informTransportableStateListener(const MSTransportable* const transportable, TransportableState to, const std::string& info) {
#ifdef HAVE_FOX
    FXConditionalLock lock(myTransportableStateListenerMutex, MSGlobals::gNumThreads > 1);
#endif
    for (TransportableStateListener* const listener : myTransportableStateListeners) {
        listener->transportableStateChanged(transportable, to, info);
    }
}


bool
MSNet::registerCollision(const SUMOTrafficObject* collider, const SUMOTrafficObject* victim, const std::string& collisionType, const MSLane* lane, double pos) {
    auto it = myCollisions.find(collider->getID());
    if (it != myCollisions.end()) {
        for (Collision& old : it->second) {
            if (old.victim == victim->getID()) {
                // collision from previous step continues
                old.colliderSpeed = collider->getSpeed();
                old.victimSpeed = victim->getSpeed();
                old.type = collisionType;
                old.lane = lane;
                old.pos = pos;
                old.time = myStep;
                return false;
            }
        }
    }
    Collision c;
    c.victim = victim->getID();
    c.colliderType = collider->getVehicleType().getID();
    c.victimType = victim->getVehicleType().getID();
    c.colliderSpeed = collider->getSpeed();
    c.victimSpeed = victim->getSpeed();
    c.type = collisionType;
    c.lane = lane;
    c.pos = pos;
    c.time = myStep;
    myCollisions[collider->getID()].push_back(c);
    return true;
}


void
MSNet::removeOutdatedCollisions() {
    for (auto it = myCollisions.begin(); it != myCollisions.end();) {
        for (auto it2 = it->second.begin(); it2 != it->second.end();) {
            if (it2->time != myStep) {
                it2 = it->second.erase(it2);
            } else {
                it2++;
            }
        }
        if (it->second.size() == 0) {
            it = myCollisions.erase(it);
        } else {
            it++;
        }
    }
}


bool
MSNet::addStoppingPlace(const SumoXMLTag category, MSStoppingPlace* stop) {
    return myStoppingPlaces[category == SUMO_TAG_TRAIN_STOP ? SUMO_TAG_BUS_STOP : category].add(stop->getID(), stop);
}


bool
MSNet::addTractionSubstation(MSTractionSubstation* substation) {
    if (find(myTractionSubstations.begin(), myTractionSubstations.end(), substation) == myTractionSubstations.end()) {
        myTractionSubstations.push_back(substation);
        return true;
    }
    return false;
}


MSStoppingPlace*
MSNet::getStoppingPlace(const std::string& id, const SumoXMLTag category) const {
    if (myStoppingPlaces.count(category) > 0) {
        return myStoppingPlaces.find(category)->second.get(id);
    }
    return nullptr;
}


std::string
MSNet::getStoppingPlaceID(const MSLane* lane, const double pos, const SumoXMLTag category) const {
    if (myStoppingPlaces.count(category) > 0) {
        for (const auto& it : myStoppingPlaces.find(category)->second) {
            MSStoppingPlace* stop = it.second;
            if (&stop->getLane() == lane && stop->getBeginLanePosition() - POSITION_EPS <= pos && stop->getEndLanePosition() + POSITION_EPS >= pos) {
                return stop->getID();
            }
        }
    }
    return "";
}


const NamedObjectCont<MSStoppingPlace*>&
MSNet::getStoppingPlaces(SumoXMLTag category) const {
    auto it = myStoppingPlaces.find(category);
    if (it != myStoppingPlaces.end()) {
        return it->second;
    } else {
        throw ProcessError("No stoppingPlace of type '" + toString(category) + "' found");
    }
}


void
MSNet::writeChargingStationOutput() const {
    if (myStoppingPlaces.count(SUMO_TAG_CHARGING_STATION) > 0) {
        OutputDevice& output = OutputDevice::getDeviceByOption("chargingstations-output");
        for (const auto& it : myStoppingPlaces.find(SUMO_TAG_CHARGING_STATION)->second) {
            static_cast<MSChargingStation*>(it.second)->writeChargingStationOutput(output);
        }
    }
}


void
MSNet::writeRailSignalBlocks() const {
    OutputDevice& output = OutputDevice::getDeviceByOption("railsignal-block-output");
    for (auto tls : myLogics->getAllLogics()) {
        MSRailSignal* rs = dynamic_cast<MSRailSignal*>(tls);
        if (rs != nullptr) {
            rs->writeBlocks(output);
        }
    }
}


void
MSNet::writeOverheadWireSegmentOutput() const {
    if (myStoppingPlaces.count(SUMO_TAG_OVERHEAD_WIRE_SEGMENT) > 0) {
        OutputDevice& output = OutputDevice::getDeviceByOption("overheadwiresegments-output");
        for (const auto& it : myStoppingPlaces.find(SUMO_TAG_OVERHEAD_WIRE_SEGMENT)->second) {
            static_cast<MSOverheadWire*>(it.second)->writeOverheadWireSegmentOutput(output);
        }
    }
}


void
MSNet::writeSubstationOutput() const {
    if (myTractionSubstations.size() > 0) {
        OutputDevice& output = OutputDevice::getDeviceByOption("substations-output");
        output.setPrecision(OptionsCont::getOptions().getInt("substations-output.precision"));
        for (auto& it : myTractionSubstations) {
            it->writeTractionSubstationOutput(output);
        }
    }
}


MSTractionSubstation*
MSNet::findTractionSubstation(const std::string& substationId) {
    for (std::vector<MSTractionSubstation*>::iterator it = myTractionSubstations.begin(); it != myTractionSubstations.end(); ++it) {
        if ((*it)->getID() == substationId) {
            return *it;
        }
    }
    return nullptr;
}


bool
MSNet::existTractionSubstation(const std::string& substationId) {
    for (std::vector<MSTractionSubstation*>::iterator it = myTractionSubstations.begin(); it != myTractionSubstations.end(); ++it) {
        if ((*it)->getID() == substationId) {
            return true;
        }
    }
    return false;
}


SUMOAbstractRouter<MSEdge, SUMOVehicle>&
MSNet::getRouterTT(const int rngIndex, const MSEdgeVector& prohibited) const {
    if (myRouterTT.count(rngIndex) == 0) {
        const std::string routingAlgorithm = OptionsCont::getOptions().getString("routing-algorithm");
        if (routingAlgorithm == "dijkstra") {
            myRouterTT[rngIndex] = new DijkstraRouter<MSEdge, SUMOVehicle>(MSEdge::getAllEdges(), true, &MSNet::getTravelTime, nullptr, false, nullptr, true);
        } else {
            if (routingAlgorithm != "astar") {
                WRITE_WARNING("TraCI and Triggers cannot use routing algorithm '" + routingAlgorithm + "'. using 'astar' instead.");
            }
            myRouterTT[rngIndex] = new AStarRouter<MSEdge, SUMOVehicle>(MSEdge::getAllEdges(), true, &MSNet::getTravelTime, nullptr, true);
        }
    }
    myRouterTT[rngIndex]->prohibit(prohibited);
    return *myRouterTT[rngIndex];
}


SUMOAbstractRouter<MSEdge, SUMOVehicle>&
MSNet::getRouterEffort(const int rngIndex, const MSEdgeVector& prohibited) const {
    if (myRouterEffort.count(rngIndex) == 0) {
        myRouterEffort[rngIndex] = new DijkstraRouter<MSEdge, SUMOVehicle>(MSEdge::getAllEdges(), true, &MSNet::getEffort, &MSNet::getTravelTime, false, nullptr, true);
    }
    myRouterEffort[rngIndex]->prohibit(prohibited);
    return *myRouterEffort[rngIndex];
}


MSNet::MSPedestrianRouter&
MSNet::getPedestrianRouter(const int rngIndex, const MSEdgeVector& prohibited) const {
    if (myPedestrianRouter.count(rngIndex) == 0) {
        myPedestrianRouter[rngIndex] = new MSPedestrianRouter();
    }
    myPedestrianRouter[rngIndex]->prohibit(prohibited);
    return *myPedestrianRouter[rngIndex];
}


MSNet::MSIntermodalRouter&
MSNet::getIntermodalRouter(const int rngIndex, const int routingMode, const MSEdgeVector& prohibited) const {
    const OptionsCont& oc = OptionsCont::getOptions();
    const int key = rngIndex * oc.getInt("thread-rngs") + routingMode;
    if (myIntermodalRouter.count(key) == 0) {
        int carWalk = 0;
        for (const std::string& opt : oc.getStringVector("persontrip.transfer.car-walk")) {
            if (opt == "parkingAreas") {
                carWalk |= MSIntermodalRouter::Network::PARKING_AREAS;
            } else if (opt == "ptStops") {
                carWalk |= MSIntermodalRouter::Network::PT_STOPS;
            } else if (opt == "allJunctions") {
                carWalk |= MSIntermodalRouter::Network::ALL_JUNCTIONS;
            }
        }
        // XXX there is currently no reason to combine multiple values, thus getValueString rather than getStringVector
        const std::string& taxiDropoff = oc.getValueString("persontrip.transfer.taxi-walk");
        const std::string& taxiPickup = oc.getValueString("persontrip.transfer.walk-taxi");
        if (taxiDropoff == "") {
            if (MSDevice_Taxi::getTaxi() != nullptr) {
                carWalk |= MSIntermodalRouter::Network::TAXI_DROPOFF_ANYWHERE;
            }
        } else if (taxiDropoff == "ptStops") {
            carWalk |= MSIntermodalRouter::Network::TAXI_DROPOFF_PT;
        } else if (taxiDropoff == "allJunctions") {
            carWalk |= MSIntermodalRouter::Network::TAXI_DROPOFF_ANYWHERE;
        }
        if (taxiPickup == "") {
            if (MSDevice_Taxi::getTaxi() != nullptr) {
                carWalk |= MSIntermodalRouter::Network::TAXI_PICKUP_ANYWHERE;
            }
        } else if (taxiPickup == "ptStops") {
            carWalk |= MSIntermodalRouter::Network::TAXI_PICKUP_PT;
        } else if (taxiPickup == "allJunctions") {
            carWalk |= MSIntermodalRouter::Network::TAXI_PICKUP_ANYWHERE;
        }
        const std::string routingAlgorithm = OptionsCont::getOptions().getString("routing-algorithm");
        double taxiWait = STEPS2TIME(string2time(OptionsCont::getOptions().getString("persontrip.taxi.waiting-time")));
        if (routingMode == libsumo::ROUTING_MODE_COMBINED) {
            myIntermodalRouter[key] = new MSIntermodalRouter(MSNet::adaptIntermodalRouter, carWalk, taxiWait, routingAlgorithm, routingMode, new FareModul());
        } else {
            myIntermodalRouter[key] = new MSIntermodalRouter(MSNet::adaptIntermodalRouter, carWalk, taxiWait, routingAlgorithm, routingMode);
        }
    }
    myIntermodalRouter[key]->prohibit(prohibited);
    return *myIntermodalRouter[key];
}


void
MSNet::adaptIntermodalRouter(MSIntermodalRouter& router) {
    double taxiWait = STEPS2TIME(string2time(OptionsCont::getOptions().getString("persontrip.taxi.waiting-time")));
    // add access to all parking areas
    EffortCalculator* const external = router.getExternalEffort();
    for (const auto& stopType : myInstance->myStoppingPlaces) {
        // add access to all stopping places
        const SumoXMLTag element = stopType.first;
        for (const auto& i : stopType.second) {
            const MSEdge* const edge = &i.second->getLane().getEdge();
            router.getNetwork()->addAccess(i.first, edge, i.second->getBeginLanePosition(), i.second->getEndLanePosition(),
                                           i.second->getAccessDistance(edge), element, false, taxiWait);
            if (element == SUMO_TAG_BUS_STOP) {
                // add access to all public transport stops
                for (const auto& a : i.second->getAllAccessPos()) {
                    router.getNetwork()->addAccess(i.first, &std::get<0>(a)->getEdge(), std::get<1>(a), std::get<1>(a), std::get<2>(a), element, true, taxiWait);
                }
                if (external != nullptr) {
                    external->addStop(router.getNetwork()->getStopEdge(i.first)->getNumericalID(), *i.second);
                }
            }
        }
    }
    myInstance->getInsertionControl().adaptIntermodalRouter(router);
    myInstance->getVehicleControl().adaptIntermodalRouter(router);
    // add access to transfer from walking to taxi-use
    if ((router.getCarWalkTransfer() & MSIntermodalRouter::Network::TAXI_PICKUP_ANYWHERE) != 0) {
        for (MSEdge* edge : myInstance->getEdgeControl().getEdges()) {
            if ((edge->getPermissions() & SVC_PEDESTRIAN) != 0 && (edge->getPermissions() & SVC_TAXI) != 0) {
                router.getNetwork()->addCarAccess(edge, SVC_TAXI, taxiWait);
            }
        }
    }
}


bool
MSNet::checkElevation() {
    const MSEdgeVector& edges = myEdges->getEdges();
    for (MSEdgeVector::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        for (std::vector<MSLane*>::const_iterator i = (*e)->getLanes().begin(); i != (*e)->getLanes().end(); ++i) {
            if ((*i)->getShape().hasElevation()) {
                return true;
            }
        }
    }
    return false;
}


bool
MSNet::checkWalkingarea() {
    for (const MSEdge* e : myEdges->getEdges()) {
        if (e->getFunction() == SumoXMLEdgeFunc::WALKINGAREA) {
            return true;
        }
    }
    return false;
}


bool
MSNet::checkBidiEdges() {
    for (const MSEdge* e : myEdges->getEdges()) {
        if (e->getBidiEdge() != nullptr) {
            return true;
        }
    }
    return false;
}

bool
MSNet::warnOnce(const std::string& typeAndID) {
    if (myWarnedOnce.find(typeAndID) == myWarnedOnce.end()) {
        myWarnedOnce[typeAndID] = true;
        return true;
    }
    return false;
}


/****************************************************************************/
