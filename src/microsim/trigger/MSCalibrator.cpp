/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2005-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSCalibrator.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, May 2005
/// @version $Id$
///
// Calibrates the flow on an edge by removing an inserting vehicles
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <algorithm>
#include <cmath>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/output/MSRouteProbe.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/distribution/RandomDistributor.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include "MSCalibrator.h"

//#define MSCalibrator_DEBUG

// ===========================================================================
// static members
// ===========================================================================
std::vector<MSMoveReminder*> MSCalibrator::LeftoverReminders;
std::vector<SUMOVehicleParameter*> MSCalibrator::LeftoverVehicleParameters;

// ===========================================================================
// method definitions
// ===========================================================================
MSCalibrator::MSCalibrator(const std::string& id,
                           const MSEdge* const edge,
                           MSLane* lane,
                           const double pos,
                           const std::string& aXMLFilename,
                           const std::string& outputFilename,
                           const SUMOTime freq, const double length,
                           const MSRouteProbe* probe,
                           const std::string& vTypes,
                           bool addLaneMeanData) :
    MSTrigger(id),
    MSRouteHandler(aXMLFilename, true),
    MSDetectorFileOutput(id, vTypes, false), // detecting persons not yet supported
    myEdge(edge),
    myLane(lane),
    myPos(pos), myProbe(probe),
    myEdgeMeanData(nullptr, length, false, nullptr),
    myMeanDataParent(id + "_dummyMeanData", 0, 0, false, false, false, false, false, false, 1, 0, 0, vTypes),
    myCurrentStateInterval(myIntervals.begin()),
    myOutput(nullptr), myFrequency(freq), myRemoved(0),
    myInserted(0), myClearedInJam(0),
    mySpeedIsDefault(true), myDidSpeedAdaption(false), myDidInit(false),
    myDefaultSpeed(myLane == nullptr ? myEdge->getSpeedLimit() : myLane->getSpeedLimit()),
    myHaveWarnedAboutClearingJam(false),
    myAmActive(false) {
    if (outputFilename != "") {
        myOutput = &OutputDevice::getDevice(outputFilename);
        writeXMLDetectorProlog(*myOutput);
    }
    if (aXMLFilename != "") {
        XMLSubSys::runParser(*this, aXMLFilename);
        if (!myDidInit) {
            init();
        }
    }
    if (addLaneMeanData) {
        // disabled for METriggeredCalibrator
        for (int i = 0; i < (int)myEdge->getLanes().size(); ++i) {
            MSLane* lane = myEdge->getLanes()[i];
            if (myLane == nullptr || myLane == lane) {
                //std::cout << " cali=" << getID() << " myLane=" << Named::getIDSecure(myLane) << " checkLane=" << i << "\n";
                MSMeanData_Net::MSLaneMeanDataValues* laneData = new MSMeanData_Net::MSLaneMeanDataValues(lane, lane->getLength(), true, &myMeanDataParent);
                laneData->setDescription("meandata_calibrator_" + lane->getID());
                LeftoverReminders.push_back(laneData);
                myLaneMeanData.push_back(laneData);
                VehicleRemover* remover = new VehicleRemover(lane, (int)i, this);
                LeftoverReminders.push_back(remover);
                myVehicleRemovers.push_back(remover);
            }
        }
    }
}


void
MSCalibrator::init() {
    if (myIntervals.size() > 0) {
        if (myIntervals.back().end == -1) {
            myIntervals.back().end = SUMOTime_MAX;
        }
        // calibration should happen after regular insertions have taken place
        MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(new CalibratorCommand(this));
    } else {
        WRITE_WARNING("No flow intervals in calibrator '" + getID() + "'.");
    }
    myDidInit = true;
}


MSCalibrator::~MSCalibrator() {
    if (myCurrentStateInterval != myIntervals.end()) {
        intervalEnd();
    }
    for (std::vector<VehicleRemover*>::iterator it = myVehicleRemovers.begin(); it != myVehicleRemovers.end(); ++it) {
        (*it)->disable();
    }
}


void
MSCalibrator::myStartElement(int element,
                             const SUMOSAXAttributes& attrs) {
    if (element == SUMO_TAG_FLOW) {
        AspiredState state;
        SUMOTime lastEnd = -1;
        if (myIntervals.size() > 0) {
            lastEnd = myIntervals.back().end;
            if (lastEnd == -1) {
                lastEnd = myIntervals.back().begin;
            }
        }
        try {
            bool ok = true;
            state.q = attrs.getOpt<double>(SUMO_ATTR_VEHSPERHOUR, nullptr, ok, -1.);
            state.v = attrs.getOpt<double>(SUMO_ATTR_SPEED, nullptr, ok, -1.);
            state.begin = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, getID().c_str(), ok);
            if (state.begin < lastEnd) {
                WRITE_ERROR("Overlapping or unsorted intervals in calibrator '" + getID() + "'.");
            }
            state.end = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, getID().c_str(), ok, -1);
            state.vehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs, true, true, true);
            LeftoverVehicleParameters.push_back(state.vehicleParameter);
            // vehicles should be inserted with max speed unless stated otherwise
            if (state.vehicleParameter->departSpeedProcedure == DEPART_SPEED_DEFAULT) {
                state.vehicleParameter->departSpeedProcedure = DEPART_SPEED_MAX;
            }
            // vehicles should be inserted on any lane unless stated otherwise
            if (state.vehicleParameter->departLaneProcedure == DEPART_LANE_DEFAULT) {
                if (myLane == nullptr) {
                    state.vehicleParameter->departLaneProcedure = DEPART_LANE_ALLOWED_FREE;
                } else {
                    state.vehicleParameter->departLaneProcedure = DEPART_LANE_GIVEN;
                    state.vehicleParameter->departLane = myLane->getIndex();
                }
            } else if (myLane != nullptr && (
                           state.vehicleParameter->departLaneProcedure != DEPART_LANE_GIVEN
                           || state.vehicleParameter->departLane != myLane->getIndex())) {
                WRITE_WARNING("Insertion lane may differ from calibrator lane for calibrator '" + getID() + "'.");
            }
            if (state.vehicleParameter->vtypeid != DEFAULT_VTYPE_ID &&
                    MSNet::getInstance()->getVehicleControl().getVType(state.vehicleParameter->vtypeid) == nullptr) {
                WRITE_ERROR("Unknown vehicle type '" + state.vehicleParameter->vtypeid + "' in calibrator '" + getID() + "'.");
            }
        } catch (EmptyData&) {
            WRITE_ERROR("Mandatory attribute missing in definition of calibrator '" + getID() + "'.");
        } catch (NumberFormatException&) {
            WRITE_ERROR("Non-numeric value for numeric attribute in definition of calibrator '" + getID() + "'.");
        }
        if (state.q < 0 && state.v < 0) {
            WRITE_ERROR("Either 'vehsPerHour' or 'speed' has to be given in flow definition of calibrator '" + getID() + "'.");
        }
        if (myIntervals.size() > 0 && myIntervals.back().end == -1) {
            myIntervals.back().end = state.begin;
        }
        myIntervals.push_back(state);
        myCurrentStateInterval = myIntervals.begin();
    } else {
        MSRouteHandler::myStartElement(element, attrs);
    }
}


void
MSCalibrator::myEndElement(int element) {
    if (element == SUMO_TAG_CALIBRATOR) {
        if (!myDidInit) {
            init();
        }
    } else if (element != SUMO_TAG_FLOW) {
        MSRouteHandler::myEndElement(element);
    }
}


void
MSCalibrator::intervalEnd() {
    if (myOutput != nullptr) {
        writeXMLOutput(*myOutput, myCurrentStateInterval->begin, myCurrentStateInterval->end);
    }
    myDidSpeedAdaption = false;
    myInserted = 0;
    myRemoved = 0;
    myClearedInJam = 0;
    myHaveWarnedAboutClearingJam = false;
    reset();
}


bool
MSCalibrator::isCurrentStateActive(SUMOTime time) {
    while (myCurrentStateInterval != myIntervals.end() && myCurrentStateInterval->end <= time) {
        // XXX what about skipped intervals?
        myCurrentStateInterval++;
    }
    return myCurrentStateInterval != myIntervals.end() &&
           myCurrentStateInterval->begin <= time && myCurrentStateInterval->end > time;
}

int
MSCalibrator::totalWished() const {
    if (myCurrentStateInterval != myIntervals.end()) {
        const double totalHourFraction = STEPS2TIME(myCurrentStateInterval->end - myCurrentStateInterval->begin) / (double) 3600.;
        return (int)std::floor(myCurrentStateInterval->q * totalHourFraction + 0.5); // round to closest int
    } else {
        return -1;
    }
}


double
MSCalibrator::currentFlow() const {
    const double totalHourFraction = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep() - myCurrentStateInterval->begin) / (double) 3600.;
    return passed() / totalHourFraction;
}

double
MSCalibrator::currentSpeed() const {
    if (myEdgeMeanData.getSamples() > 0) {
        return myEdgeMeanData.getTravelledDistance() / myEdgeMeanData.getSamples();
    } else {
        return -1;
    }
}


bool
MSCalibrator::removePending() {
    if (myToRemove.size() > 0) {
        MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
        // it is not save to remove the vehicles inside
        // VehicleRemover::notifyEnter so we do it here
        for (std::set<std::string>::iterator it = myToRemove.begin(); it != myToRemove.end(); ++it) {
            MSVehicle* vehicle = dynamic_cast<MSVehicle*>(vc.getVehicle(*it));
            if (vehicle != nullptr) {
                vehicle->onRemovalFromNet(MSMoveReminder::NOTIFICATION_VAPORIZED);
                vehicle->getLane()->removeVehicle(vehicle, MSMoveReminder::NOTIFICATION_VAPORIZED);
                vc.scheduleVehicleRemoval(vehicle, true);
            } else {
                WRITE_WARNING("Calibrator '" + getID() + "' could not remove vehicle '" + *it + "'.");
            }
        }
        myToRemove.clear();
        return true;
    }
    return false;
}


SUMOTime
MSCalibrator::execute(SUMOTime currentTime) {
    // get current simulation values (valid for the last simulation second)
    // XXX could we miss vehicle movements if this is called less often than every DELTA_T (default) ?
    updateMeanData();
    const bool hadRemovals = removePending();
    // check whether an adaptation value exists
    if (isCurrentStateActive(currentTime)) {
        myAmActive = true;
        // all happens in isCurrentStateActive()
    } else {
        myAmActive = false;
        reset();
        if (!mySpeedIsDefault) {
            // reset speed to default
            if (myLane == nullptr) {
                myEdge->setMaxSpeed(myDefaultSpeed);
            } else {
                myLane->setMaxSpeed(myDefaultSpeed);
            }
            mySpeedIsDefault = true;
        }
        if (myCurrentStateInterval == myIntervals.end()) {
            // keep calibrator alive for gui but do not call again
            return TIME2STEPS(86400);
        }
        return myFrequency;
    }
    // we are active
    if (!myDidSpeedAdaption && myCurrentStateInterval->v >= 0) {
        if (myLane == nullptr) {
            myEdge->setMaxSpeed(myCurrentStateInterval->v);
        } else {
            myLane->setMaxSpeed(myCurrentStateInterval->v);
        }
        mySpeedIsDefault = false;
        myDidSpeedAdaption = true;
    }

    const bool calibrateFlow = myCurrentStateInterval->q >= 0;
    const int totalWishedNum = totalWished();
    int adaptedNum = passed() + myClearedInJam;
#ifdef MSCalibrator_DEBUG
    std::cout << time2string(currentTime) << " " << getID()
              << " q=" << myCurrentStateInterval->q
              << " totalWished=" << totalWishedNum
              << " adapted=" << adaptedNum
              << " jam=" << invalidJam(myLane == 0 ? -1 : myLane->getIndex())
              << " entered=" << myEdgeMeanData.nVehEntered
              << " departed=" << myEdgeMeanData.nVehDeparted
              << " arrived=" << myEdgeMeanData.nVehArrived
              << " left=" << myEdgeMeanData.nVehLeft
              << " waitSecs=" << myEdgeMeanData.waitSeconds
              << " vaporized=" << myEdgeMeanData.nVehVaporized
              << "\n";
#endif
    if (calibrateFlow && adaptedNum < totalWishedNum && !hadRemovals) {
        // we need to insert some vehicles
        const double hourFraction = STEPS2TIME(currentTime - myCurrentStateInterval->begin + DELTA_T) / (double) 3600.;
        const int wishedNum = (int)std::floor(myCurrentStateInterval->q * hourFraction + 0.5); // round to closest int
        // only the difference between inflow and aspiredFlow should be added, thus
        // we should not count vehicles vaporized from a jam here
        // if we have enough time left we can add missing vehicles later
        const int relaxedInsertion = (int)std::floor(STEPS2TIME(myCurrentStateInterval->end - currentTime) / 3);
        const int insertionSlack = MAX2(0, adaptedNum + relaxedInsertion - totalWishedNum);
        // increase number of vehicles
#ifdef MSCalibrator_DEBUG
        std::cout
                << "   wished:" << wishedNum
                << " slack:" << insertionSlack
                << " before:" << adaptedNum
                << "\n";
#endif
        while (wishedNum > adaptedNum + insertionSlack) {
            SUMOVehicleParameter* pars = myCurrentStateInterval->vehicleParameter;
            const MSRoute* route = myProbe != nullptr ? myProbe->getRoute() : nullptr;
            if (route == nullptr) {
                route = MSRoute::dictionary(pars->routeid);
            }
            if (route == nullptr) {
                WRITE_WARNING("No valid routes in calibrator '" + getID() + "'.");
                break;
            }
            if (!route->contains(myEdge)) {
                WRITE_WARNING("Route '" + route->getID() + "' in calibrator '" + getID() + "' does not contain edge '" + myEdge->getID() + "'.");
                break;
            }
            const int routeIndex = (int)std::distance(route->begin(),
                                   std::find(route->begin(), route->end(), myEdge));
            MSVehicleType* vtype = MSNet::getInstance()->getVehicleControl().getVType(pars->vtypeid);
            assert(route != 0 && vtype != 0);
            // build the vehicle
            SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
            newPars->id = getID() + "." + toString((int)STEPS2TIME(myCurrentStateInterval->begin)) + "." + toString(myInserted);
            newPars->depart = currentTime;
            newPars->routeid = route->getID();
            newPars->departLaneProcedure = DEPART_LANE_FIRST_ALLOWED; // ensure successful vehicle creation
            MSVehicle* vehicle;
            try {
                vehicle = dynamic_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().buildVehicle(
                                                       newPars, route, vtype, true, false));
            } catch (const ProcessError& e) {
                if (!MSGlobals::gCheckRoutes) {
                    WRITE_WARNING(e.what());
                    vehicle = nullptr;
                    break;
                } else {
                    throw e;
                }
            }
#ifdef MSCalibrator_DEBUG
            std::cout << " resetting route pos: " << routeIndex << "\n";
#endif
            vehicle->resetRoutePosition(routeIndex, pars->departLaneProcedure);
            if (myEdge->insertVehicle(*vehicle, currentTime)) {
                if (!MSNet::getInstance()->getVehicleControl().addVehicle(vehicle->getID(), vehicle)) {
                    throw ProcessError("Emission of vehicle '" + vehicle->getID() + "' in calibrator '" + getID() + "'failed!");
                }
                myInserted++;
                adaptedNum++;
#ifdef MSCalibrator_DEBUG
                std::cout << "I ";
#endif
            } else {
                // could not insert vehicle
#ifdef MSCalibrator_DEBUG
                std::cout << "F ";
#endif
                MSNet::getInstance()->getVehicleControl().deleteVehicle(vehicle, true);
                break;
            }
        }
    }
    if (myCurrentStateInterval->end <= currentTime + myFrequency) {
        intervalEnd();
    }
    return myFrequency;
}

void
MSCalibrator::reset() {
    myEdgeMeanData.reset();
    for (std::vector<MSMeanData_Net::MSLaneMeanDataValues*>::iterator it = myLaneMeanData.begin(); it != myLaneMeanData.end(); ++it) {
        (*it)->reset();
    }
}


bool
MSCalibrator::invalidJam(int laneIndex) const {
    if (laneIndex < 0) {
        const int numLanes = (int)myEdge->getLanes().size();
        for (int i = 0; i < numLanes; ++i) {
            if (invalidJam(i)) {
                return true;
            }
        }
        return false;
    }
    assert(laneIndex < (int)myEdge->getLanes().size());
    const MSLane* const lane = myEdge->getLanes()[laneIndex];
    if (lane->getVehicleNumber() < 4) {
        // cannot reliably detect invalid jams
        return false;
    }
    // maxSpeed reflects the calibration target
    const bool toSlow = lane->getMeanSpeed() < 0.5 * myEdge->getSpeedLimit();
    return toSlow && remainingVehicleCapacity(laneIndex) < 1;
}


int
MSCalibrator::remainingVehicleCapacity(int laneIndex) const {
    if (laneIndex < 0) {
        const int numLanes = (int)myEdge->getLanes().size();
        int result = 0;
        for (int i = 0; i < numLanes; ++i) {
            result = MAX2(result, remainingVehicleCapacity(i));
        }
        return result;
    }
    assert(laneIndex < (int)myEdge->getLanes().size());
    MSLane* lane = myEdge->getLanes()[laneIndex];
    MSVehicle* last = lane->getLastFullVehicle();
    const SUMOVehicleParameter* pars = myCurrentStateInterval->vehicleParameter;
    const MSVehicleType* vtype = MSNet::getInstance()->getVehicleControl().getVType(pars->vtypeid);
    const double spacePerVehicle = vtype->getLengthWithGap() + myEdge->getSpeedLimit() * vtype->getCarFollowModel().getHeadwayTime();
    if (last == nullptr) {
        // ensure vehicles can be inserted on short edges
        return MAX2(1, (int)(myEdge->getLength() / spacePerVehicle));
    } else {
        return (int)(last->getPositionOnLane() / spacePerVehicle);
    }
}


void
MSCalibrator::cleanup() {
    for (std::vector<MSMoveReminder*>::iterator it = LeftoverReminders.begin(); it != LeftoverReminders.end(); ++it) {
        delete *it;
    }
    LeftoverReminders.clear();
    for (std::vector<SUMOVehicleParameter*>::iterator it = LeftoverVehicleParameters.begin();
            it != LeftoverVehicleParameters.end(); ++it) {
        delete *it;
    }
    LeftoverVehicleParameters.clear();
}


void
MSCalibrator::updateMeanData() {
    myEdgeMeanData.reset();
    for (std::vector<MSMeanData_Net::MSLaneMeanDataValues*>::iterator it = myLaneMeanData.begin();
            it != myLaneMeanData.end(); ++it) {
        (*it)->addTo(myEdgeMeanData);
    }
}

bool MSCalibrator::VehicleRemover::notifyEnter(SUMOTrafficObject& veh, Notification /* reason */, const MSLane* /* enteredLane */) {
    if (myParent == nullptr) {
        return false;
    }
    if (!myParent->vehicleApplies(veh)) {
        return false;
    }
    if (myParent->isActive()) {
        myParent->updateMeanData();
        const bool calibrateFlow = myParent->myCurrentStateInterval->q >= 0;
        const int totalWishedNum = myParent->totalWished();
        int adaptedNum = myParent->passed() + myParent->myClearedInJam;
        MSVehicle* vehicle = dynamic_cast<MSVehicle*>(&veh);
        if (calibrateFlow && adaptedNum > totalWishedNum) {
#ifdef MSCalibrator_DEBUG
            std::cout << time2string(MSNet::getInstance()->getCurrentTimeStep()) << " " << myParent->getID()
                      << " vaporizing " << vehicle->getID() << " to reduce flow\n";
#endif
            if (myParent->scheduleRemoval(vehicle)) {
                myParent->myRemoved++;
            }
        } else if (myParent->invalidJam(myLaneIndex)) {
#ifdef MSCalibrator_DEBUG
            std::cout << time2string(MSNet::getInstance()->getCurrentTimeStep()) << " " << myParent->getID()
                      << " vaporizing " << vehicle->getID() << " to clear jam\n";
#endif
            if (!myParent->myHaveWarnedAboutClearingJam) {
                WRITE_WARNING("Clearing jam at calibrator '" + myParent->getID() + "' at time "
                              + time2string(MSNet::getInstance()->getCurrentTimeStep()));
                myParent->myHaveWarnedAboutClearingJam = true;
            }
            if (myParent->scheduleRemoval(vehicle)) {
                myParent->myClearedInJam++;
            }
        }
    }
    return true;
}

void
MSCalibrator::writeXMLOutput(OutputDevice& dev, SUMOTime startTime, SUMOTime stopTime) {
    updateMeanData();
    const int p = passed();
    // meandata will be off if vehicles are removed on the next edge instead of this one
    const int discrepancy = myEdgeMeanData.nVehEntered + myEdgeMeanData.nVehDeparted - myEdgeMeanData.nVehVaporized - passed();
    assert(discrepancy >= 0);
    const std::string ds = (discrepancy > 0 ? "\" vaporizedOnNextEdge=\"" + toString(discrepancy) : "");
    const double durationSeconds = STEPS2TIME(stopTime - startTime);
    dev << "    <interval begin=\"" << time2string(startTime) <<
        "\" end=\"" << time2string(stopTime) <<
        "\" id=\"" << getID() <<
        "\" nVehContrib=\"" << p <<
        "\" removed=\"" << myRemoved <<
        "\" inserted=\"" << myInserted <<
        "\" cleared=\"" << myClearedInJam <<
        "\" flow=\"" << p * 3600.0 / durationSeconds <<
        "\" aspiredFlow=\"" << myCurrentStateInterval->q <<
        "\" speed=\"" << myEdgeMeanData.getTravelledDistance() / myEdgeMeanData.getSamples() <<
        "\" aspiredSpeed=\"" << myCurrentStateInterval->v <<
        ds << //optional
        "\"/>\n";
}

void
MSCalibrator::writeXMLDetectorProlog(OutputDevice& dev) const {
    dev.writeXMLHeader("calibratorstats", "calibratorstats_file.xsd");
}


/****************************************************************************/

