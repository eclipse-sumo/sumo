/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSTransportable.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @author  Michael Behrisch
/// @date    Thu, 12 Jun 2014
/// @version $Id$
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <math.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOTime.h>
//#include <microsim/MSVehicle.h>
#include <microsim/pedestrians/MSPerson.h>
//#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
//#include <microsim/MSGlobals.h>
//#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/MSTrafficItem.h>
#include "MSDriverState.h"

/* -------------------------------------------------------------------------
* static member definitions
* ----------------------------------------------------------------------- */
// hash function
std::hash<std::string> MSDriverState::MSTrafficItem::hash = std::hash<std::string>();


// ===========================================================================
// Default value definitions
// ===========================================================================
double TCIDefaults::myMinTaskCapability = 0.1;
double TCIDefaults::myMaxTaskCapability = 10.0;
double TCIDefaults::myMaxTaskDemand = 20.0;
double TCIDefaults::myMaxDifficulty = 10.0;
double TCIDefaults::mySubCriticalDifficultyCoefficient = 0.1;
double TCIDefaults::mySuperCriticalDifficultyCoefficient = 1.0;
double TCIDefaults::myOppositeDirectionDrivingFactor = 1.3;
double TCIDefaults::myHomeostasisDifficulty = 1.5;
double TCIDefaults::myCapabilityTimeScale = 0.5;
double TCIDefaults::myAccelerationErrorTimeScaleCoefficient = 1.0;
double TCIDefaults::myAccelerationErrorNoiseIntensityCoefficient = 1.0;
double TCIDefaults::myActionStepLengthCoefficient = 1.0;
double TCIDefaults::myMinActionStepLength = 0.0;
double TCIDefaults::myMaxActionStepLength = 3.0;
double TCIDefaults::mySpeedPerceptionErrorTimeScaleCoefficient = 1.0;
double TCIDefaults::mySpeedPerceptionErrorNoiseIntensityCoefficient = 1.0;
double TCIDefaults::myHeadwayPerceptionErrorTimeScaleCoefficient = 1.0;
double TCIDefaults::myHeadwayPerceptionErrorNoiseIntensityCoefficient = 1.0;


// ===========================================================================
// method definitions
// ===========================================================================


MSDriverState::MSTrafficItem::MSTrafficItem(MSTrafficItemType type, const std::string& id, std::shared_ptr<MSTrafficItemCharacteristics> data) :
    type(type),
    id_hash(hash(id)),
    data(data),
    remainingIntegrationTime(0.),
    integrationDemand(0.),
    latentDemand(0.)
{}


MSDriverState::OUProcess::OUProcess(double initialState, double timeScale, double noiseIntensity)
    : myState(initialState),
      myTimeScale(timeScale),
      myNoiseIntensity(noiseIntensity) {}


MSDriverState::OUProcess::~OUProcess() {}


void
MSDriverState::OUProcess::step(double dt) {
    myState = exp(-dt/myTimeScale)*myState + myNoiseIntensity*sqrt(2*dt/myTimeScale)*RandHelper::randNorm(0, 1);
}


double
MSDriverState::OUProcess::getState() const {
    return myState;
}


MSDriverState::MSDriverState(MSVehicle* veh) :
            myVehicle(veh),
            myMinTaskCapability(TCIDefaults::myMinTaskCapability),
            myMaxTaskCapability(TCIDefaults::myMaxTaskCapability),
            myMaxTaskDemand(TCIDefaults::myMaxTaskDemand),
            myMaxDifficulty(TCIDefaults::myMaxDifficulty),
            mySubCriticalDifficultyCoefficient(TCIDefaults::mySubCriticalDifficultyCoefficient),
            mySuperCriticalDifficultyCoefficient(TCIDefaults::mySuperCriticalDifficultyCoefficient),
            myOppositeDirectionDrivingDemandFactor(TCIDefaults::myOppositeDirectionDrivingFactor),
            myHomeostasisDifficulty(TCIDefaults::myHomeostasisDifficulty),
            myCapabilityTimeScale(TCIDefaults::myCapabilityTimeScale),
            myAccelerationErrorTimeScaleCoefficient(TCIDefaults::myAccelerationErrorTimeScaleCoefficient),
            myAccelerationErrorNoiseIntensityCoefficient(TCIDefaults::myAccelerationErrorNoiseIntensityCoefficient),
            myActionStepLengthCoefficient(TCIDefaults::myActionStepLengthCoefficient),
            myMinActionStepLength(TCIDefaults::myMinActionStepLength),
            myMaxActionStepLength(TCIDefaults::myMaxActionStepLength),
            mySpeedPerceptionErrorTimeScaleCoefficient(TCIDefaults::mySpeedPerceptionErrorTimeScaleCoefficient),
            mySpeedPerceptionErrorNoiseIntensityCoefficient(TCIDefaults::mySpeedPerceptionErrorNoiseIntensityCoefficient),
            myHeadwayPerceptionErrorTimeScaleCoefficient(TCIDefaults::myHeadwayPerceptionErrorTimeScaleCoefficient),
            myHeadwayPerceptionErrorNoiseIntensityCoefficient(TCIDefaults::myHeadwayPerceptionErrorNoiseIntensityCoefficient),
            myAmOpposite(false),
            myAccelerationError(0., 1.,1.),
            myHeadwayPerceptionError(0., 1.,1.),
            mySpeedPerceptionError(0., 1.,1.),
            myTaskDemand(0.),
            myTaskCapability(myMaxTaskCapability),
            myCurrentDrivingDifficulty(myTaskDemand/myTaskCapability),
            myActionStepLength(TS),
            myStepDuration(TS),
            myLastUpdateTime(SIMTIME-TS),
            myCurrentSpeed(0.),
            myCurrentAcceleration(0.)
{}



void
MSDriverState::updateStepDuration() {
    myStepDuration = SIMTIME - myLastUpdateTime;
    myLastUpdateTime = SIMTIME;
}


void
MSDriverState::calculateDrivingDifficulty(double capability, double demand) {
    assert(capability > 0.);
    assert(demand >= 0.);
    if (myAmOpposite) {
        myCurrentDrivingDifficulty = difficultyFunction(myOppositeDirectionDrivingDemandFactor*demand/capability);
    } else {
        myCurrentDrivingDifficulty = difficultyFunction(demand/capability);
    }
}


double
MSDriverState::difficultyFunction(double demandCapabilityQuotient) const {
    double difficulty;
    if (demandCapabilityQuotient <= 1) {
        // demand does not exceed capability -> we are in the region for a slight ascend of difficulty
        difficulty = mySubCriticalDifficultyCoefficient*demandCapabilityQuotient;
    } else {// demand exceeds capability -> we are in the region for a steeper ascend of the effect of difficulty
        difficulty = mySubCriticalDifficultyCoefficient + (demandCapabilityQuotient - 1)*mySuperCriticalDifficultyCoefficient;
    }
    return MIN2(myMaxDifficulty, difficulty);
}


void
MSDriverState::adaptTaskCapability() {
    myTaskCapability = myTaskCapability + myCapabilityTimeScale*myStepDuration*(myTaskDemand - myHomeostasisDifficulty*myTaskCapability);
}


void
MSDriverState::updateAccelerationError() {
    updateErrorProcess(myAccelerationError, myAccelerationErrorTimeScaleCoefficient, myAccelerationErrorNoiseIntensityCoefficient);
}

void
MSDriverState::updateSpeedPerceptionError() {
    updateErrorProcess(mySpeedPerceptionError, mySpeedPerceptionErrorTimeScaleCoefficient, mySpeedPerceptionErrorNoiseIntensityCoefficient);
}

void
MSDriverState::updateHeadwayPerceptionError() {
    updateErrorProcess(myHeadwayPerceptionError, myHeadwayPerceptionErrorTimeScaleCoefficient, myHeadwayPerceptionErrorNoiseIntensityCoefficient);
}

void
MSDriverState::updateActionStepLength() {
    if (myActionStepLengthCoefficient*myCurrentDrivingDifficulty <= myMinActionStepLength) {
        myActionStepLength = myMinActionStepLength;
    } else {
        myActionStepLength = MIN2(myActionStepLengthCoefficient*myCurrentDrivingDifficulty - myMinActionStepLength, myMaxActionStepLength);
    }
}


void
MSDriverState::updateErrorProcess(OUProcess& errorProcess, double timeScaleCoefficient, double noiseIntensityCoefficient) const {
    if (myCurrentDrivingDifficulty == 0) {
        errorProcess.setState(0.);
    } else {
        errorProcess.setTimeScale(timeScaleCoefficient/myCurrentDrivingDifficulty);
        errorProcess.setNoiseIntensity(myCurrentDrivingDifficulty*noiseIntensityCoefficient);
        errorProcess.step(myStepDuration);
    }
}

void
MSDriverState::registerLeader(const MSVehicle* leader, double gap, double latGap) {
    std::shared_ptr<MSTrafficItemCharacteristics> tic = std::dynamic_pointer_cast<MSTrafficItemCharacteristics>(std::make_shared<VehicleCharacteristics>(leader, gap, latGap));
    std::shared_ptr<MSTrafficItem> ti = std::make_shared<MSTrafficItem>(TRAFFIC_ITEM_VEHICLE, leader->getID(), tic);
    registerTrafficItem(ti);
}

void
MSDriverState::registerPedestrian(const MSPerson* pedestrian, double gap) {
    std::shared_ptr<MSTrafficItemCharacteristics> tic = std::dynamic_pointer_cast<MSTrafficItemCharacteristics>(std::make_shared<PedestrianCharacteristics>(pedestrian, gap));
    std::shared_ptr<MSTrafficItem> ti = std::make_shared<MSTrafficItem>(TRAFFIC_ITEM_PEDESTRIAN, pedestrian->getID(), tic);
    registerTrafficItem(ti);
}

void
MSDriverState::registerSpeedLimit(const MSLane* lane, double speedLimit, double dist) {
    std::shared_ptr<MSTrafficItemCharacteristics> tic = std::dynamic_pointer_cast<MSTrafficItemCharacteristics>(std::make_shared<SpeedLimitCharacteristics>(lane, dist, speedLimit));
    std::shared_ptr<MSTrafficItem> ti = std::make_shared<MSTrafficItem>(TRAFFIC_ITEM_SPEED_LIMIT, lane->getID(), tic);
    registerTrafficItem(ti);
}

void
MSDriverState::registerTLS(MSLink* link, double dist) {
    const MSTrafficLightLogic* tls = link->getTLLogic();
    if (tls == nullptr) {
        return;
    }
    LinkState state = tls->getCurrentPhaseDef().getSignalState(link->getTLIndex());
    int nrLanes = myVehicle->getLane()->getEdge().getLanes().size();
    std::shared_ptr<MSTrafficItemCharacteristics> tic = std::dynamic_pointer_cast<MSTrafficItemCharacteristics>(std::make_shared<TLSCharacteristics>(dist, state, nrLanes));
    std::shared_ptr<MSTrafficItem> ti = std::make_shared<MSTrafficItem>(TRAFFIC_ITEM_SPEED_LIMIT, tls->getID(), tic);
    registerTrafficItem(ti);
}

void
MSDriverState::registerJunction(MSLink* link, double dist) {
    const MSJunction* junction = link->getJunction();
    std::shared_ptr<MSTrafficItemCharacteristics> tic = std::dynamic_pointer_cast<MSTrafficItemCharacteristics>(std::make_shared<JunctionCharacteristics>(junction, link, dist));
    std::shared_ptr<MSTrafficItem> ti = std::make_shared<MSTrafficItem>(TRAFFIC_ITEM_JUNCTION, junction->getID(), tic);
    registerTrafficItem(ti);
}

void
MSDriverState::registerEgoVehicleState() {
    myAmOpposite = myVehicle->getLaneChangeModel().isOpposite();
    myCurrentSpeed = myVehicle->getSpeed();
    myCurrentAcceleration = myVehicle->getAcceleration();
}


void
MSDriverState::registerTrafficItem(std::shared_ptr<MSTrafficItem> ti) {
    if (myNewTrafficItems.find(ti->id_hash) == myNewTrafficItems.end()) {

        // Update demand associated with the item
        auto knownTiIt = myTrafficItems.find(ti->id_hash);
        if (knownTiIt == myTrafficItems.end()) {
            // new item --> init integration demand and latent task demand
            calculateIntegrationDemandAndTime(ti);
        } else {
            // known item --> only update latent task demand associated with the item
            ti = knownTiIt->second;
        }
        calculateLatentDemand(ti);

        // Take into account the task demand associated with the item
        integrateDemand(ti);

        if (ti->remainingIntegrationTime>0) {
            updateItemIntegration(ti);
        }

        // Track item
        myNewTrafficItems[ti->id_hash] = ti;
    }
}


//void
//MSDriverState::flushTrafficItems() {
//    myTrafficItems = myNewTrafficItems;
//}


void
MSDriverState::updateItemIntegration(std::shared_ptr<MSTrafficItem> ti) {
    // Eventually decrease integration time and take into account integration cost.
    ti->remainingIntegrationTime -= myStepDuration;
    if (ti->remainingIntegrationTime <= 0) {
        ti->remainingIntegrationTime = 0;
        ti->integrationDemand = 0;
    }
}


void
MSDriverState::calculateIntegrationDemandAndTime(std::shared_ptr<MSTrafficItem> ti) {
// @todo
}


void
MSDriverState::calculateLatentDemand(std::shared_ptr<MSTrafficItem> ti) {
    switch (ti->type) {
    case TRAFFIC_ITEM_JUNCTION: {
        // Latent demand for junction is proportional to number of conflicting lanes
        // for the vehicle's path plus a factor for the total number of incoming lanes
        // at the junction. Further, the distance to the junction is inversely proportional
        // to the induced demand [~1/(c*dist + 1)].
        std::shared_ptr<JunctionCharacteristics> ch = std::dynamic_pointer_cast<JunctionCharacteristics>(ti->data);
        const MSJunction* j = ch->junction;
        double LATENT_DEMAND_COEFF_JUNCTION_INCOMING = 0.1;
        double LATENT_DEMAND_COEFF_JUNCTION_FOES = 0.5;
        double LATENT_DEMAND_COEFF_JUNCTION_DIST = 0.1;
        ti->latentDemand = (LATENT_DEMAND_COEFF_JUNCTION_INCOMING*j->getNrOfIncomingLanes()
                                 + LATENT_DEMAND_COEFF_JUNCTION_FOES*j->getFoeLinks(ch->egoLink).size())
                                         /(1 + ch->dist*LATENT_DEMAND_COEFF_JUNCTION_DIST);
    }
        break;
    case TRAFFIC_ITEM_PEDESTRIAN: {
        // Latent demand for pedestrian is proportional to the euclidean distance to the
        // pedestrian (i.e. its potential to 'jump in front of the car) [~1/(c*dist + 1)]
        std::shared_ptr<PedestrianCharacteristics> ch = std::dynamic_pointer_cast<PedestrianCharacteristics>(ti->data);
        const MSPerson* p = ch->pedestrian;
        ti->latentDemand = 0;
        WRITE_WARNING("MSDriverState::calculateLatentDemand(pedestrian) not implemented")
    }
        break;
    case TRAFFIC_ITEM_SPEED_LIMIT: {
        // Latent demand for speed limit is proportional to speed difference to current vehicle speed
        // during approach [~c*(1+deltaV) if dist<threshold].
        std::shared_ptr<SpeedLimitCharacteristics> ch = std::dynamic_pointer_cast<SpeedLimitCharacteristics>(ti->data);
        ti->latentDemand = 0;
        WRITE_WARNING("MSDriverState::calculateLatentDemand(speedlimit) not implemented")
    }
        break;
    case TRAFFIC_ITEM_TLS: {
        // Latent demand for tls is proportional to vehicle's approaching speed
        // and dependent on the tls state as well as the number of approaching lanes
        // [~c(tlsState, nLanes)*(1+V) if dist<threshold].
        std::shared_ptr<TLSCharacteristics> ch = std::dynamic_pointer_cast<TLSCharacteristics>(ti->data);
        ti->latentDemand = 0;
        WRITE_WARNING("MSDriverState::calculateLatentDemand(TLS) not implemented")

    }
        break;
    case TRAFFIC_ITEM_VEHICLE: {
        // Latent demand for neighboring vehicle is determined from the relative and absolute speed,
        // and from the lateral and longitudinal distance.

        double LATENT_DEMAND_VEHILCE_EUCLIDEAN_DIST_THRESHOLD = 20;

        std::shared_ptr<VehicleCharacteristics> ch = std::dynamic_pointer_cast<VehicleCharacteristics>(ti->data);
        if (myVehicle->getEdge() == nullptr){
            return;
        }
        const MSVehicle* foe = ch->foe;
        if (foe->getEdge() == myVehicle->getEdge()) {
            // on same edge
        } else if (foe->getEdge() == myVehicle->getEdge()->getOppositeEdge()) {
            // on opposite edges
        } else if (myVehicle->getPosition().distanceSquaredTo2D(foe->getPosition()) < LATENT_DEMAND_VEHILCE_EUCLIDEAN_DIST_THRESHOLD) {
            // close enough
        }


    }
        break;
    default:
        WRITE_WARNING("Unknown traffic item type!")
        break;
    }
}


void
MSDriverState::integrateDemand(std::shared_ptr<MSTrafficItem> ti) {
    myMaxTaskDemand += ti->integrationDemand;
    myMaxTaskDemand += ti->latentDemand;
}




/****************************************************************************/
