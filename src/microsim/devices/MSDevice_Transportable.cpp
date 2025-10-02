/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_Transportable.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Fri, 30.01.2009
///
// A device which is used to keep track of persons and containers riding with a vehicle
/****************************************************************************/
#include <config.h>

#include <utils/xml/SUMOSAXAttributes.h>
#include <microsim/output/MSStopOut.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSStop.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/transportables/MSPerson.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/transportables/MSStageDriving.h>
#include "MSDevice_Transportable.h"
#include "MSDevice_Taxi.h"


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
MSDevice_Transportable*
MSDevice_Transportable::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into, const bool isContainer) {
    MSDevice_Transportable* device = new MSDevice_Transportable(v, isContainer ? "container_" + v.getID() : "person_" + v.getID(), isContainer);
    into.push_back(device);
    return device;
}


// ---------------------------------------------------------------------------
// MSDevice_Transportable-methods
// ---------------------------------------------------------------------------
MSDevice_Transportable::MSDevice_Transportable(SUMOVehicle& holder, const std::string& id, const bool isContainer) :
    MSVehicleDevice(holder, id),
    myAmContainer(isContainer),
    myTransportables(),
    myStopped(holder.isStopped()),
    myOriginalType(&holder.getVehicleType()),
    myLoadedType(nullptr) {
    const std::string key = "device." + deviceName() + ".loadedType";
    const std::string loadedTypeID = holder.getStringParam(key);
    if (loadedTypeID != "") {
        myLoadedType = MSNet::getInstance()->getVehicleControl().getVType(loadedTypeID, getEquipmentRNG());
        if (myLoadedType == nullptr) {
            throw InvalidArgument(TLF("Vehicle type '%' in parameter '%' of vehicle '%' is not known.", loadedTypeID, key, holder.getID()));
        }
    }
}


MSDevice_Transportable::~MSDevice_Transportable() {
    // flush any unfortunate riders still remaining
    for (auto it = myTransportables.begin(); it != myTransportables.end();) {
        MSTransportable* transportable = *it;
        WRITE_WARNING((myAmContainer ? "Removing container '" : "Removing person '") + transportable->getID() +
                      "' at removal of vehicle '" + myHolder.getID() + "'");
        MSStageDriving* const stage = dynamic_cast<MSStageDriving*>(transportable->getCurrentStage());
        if (stage != nullptr) {
            stage->setVehicle(nullptr);
        }
        if (myAmContainer) {
            MSNet::getInstance()->getContainerControl().erase(transportable);
        } else {
            MSNet::getInstance()->getPersonControl().erase(transportable);
        }
        it = myTransportables.erase(it);
    }
}

void
MSDevice_Transportable::notifyMoveInternal(const SUMOTrafficObject& veh,
        const double /* frontOnLane */,
        const double /* timeOnLane */,
        const double /* meanSpeedFrontOnLane */,
        const double /* meanSpeedVehicleOnLane */,
        const double travelledDistanceFrontOnLane,
        const double /* travelledDistanceVehicleOnLane */,
        const double /* meanLengthOnLane */) {
    notifyMove(const_cast<SUMOTrafficObject&>(veh), -1, travelledDistanceFrontOnLane, veh.getEdge()->getVehicleMaxSpeed(&veh));
}

bool
MSDevice_Transportable::anyLeavingAtStop(const MSStop& stop) const {
    for (const MSTransportable* t : myTransportables) {
        MSStageDriving* const stage = dynamic_cast<MSStageDriving*>(t->getCurrentStage());
        if (stage->canLeaveVehicle(t, myHolder, stop)) {
            return true;
        }
    }
    return false;
}


void
MSDevice_Transportable::transferAtSplitOrJoin(MSBaseVehicle* otherVeh) {
    const MSStop& stop = myHolder.getNextStop();
    for (auto it = myTransportables.begin(); it != myTransportables.end();) {
        MSTransportable* t = *it;
        if (t->getNumRemainingStages() > 1) {
            MSStageDriving* const stage = dynamic_cast<MSStageDriving*>(t->getCurrentStage());
            if (stage->canLeaveVehicle(t, myHolder, stop)) {
                MSStageDriving* const stage2 = dynamic_cast<MSStageDriving*>(t->getNextStage(1));
                if (stage2 && stage2->isWaitingFor(otherVeh)) {
                    it = myTransportables.erase(it);
                    // proceeding registers t as waiting on edge
                    t->proceed(MSNet::getInstance(), SIMSTEP);
                    MSTransportableControl& tc = (t->isPerson() ?
                                                  MSNet::getInstance()->getPersonControl() :
                                                  MSNet::getInstance()->getContainerControl());
                    tc.abortWaitingForVehicle(t);
                    t->getEdge()->removeTransportable(t);
                    otherVeh->addTransportable(t);
                    stage2->setVehicle(otherVeh);
                    continue;
                }
            }
        }
        it++;
    }
}


bool
MSDevice_Transportable::willTransferAtJoin(const MSTransportable* t, const MSBaseVehicle* joinVeh) {
    if (joinVeh && t->getNumRemainingStages() > 1) {
        MSStageDriving* const stage2 = dynamic_cast<MSStageDriving*>(t->getNextStage(1));
        return stage2->isWaitingFor(joinVeh);
    }
    return false;
}


bool
MSDevice_Transportable::notifyMove(SUMOTrafficObject& /*tObject*/, double /*oldPos*/, double newPos, double newSpeed) {
    SUMOVehicle& veh = myHolder;
    const SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
    if (myStopped) {
        if (!veh.isStopped()) {
            const SUMOTime freeFlowTimeCorrection = MSGlobals::gUseMesoSim ? TIME2STEPS(newPos / newSpeed) : 0;
            for (MSTransportable* const transportable : myTransportables) {
                transportable->setDeparted(currentTime - freeFlowTimeCorrection);
            }
            myStopped = false;
        }
    } else {
        if (veh.isStopped()) {
            myStopped = true;
            MSStop& stop = veh.getNextStopMutable();
            const MSVehicle* joinVeh = dynamic_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().getVehicle(stop.pars.join));
            const SUMOTime boardingDuration = veh.getVehicleType().getLoadingDuration(!myAmContainer);
            int numUnloaded = 0;
            for (std::vector<MSTransportable*>::iterator i = myTransportables.begin(); i != myTransportables.end();) {
                MSTransportable* transportable = *i;
                MSStageDriving* const stage = dynamic_cast<MSStageDriving*>(transportable->getCurrentStage());
                if (stage->canLeaveVehicle(transportable, myHolder, stop) && !willTransferAtJoin(transportable, joinVeh)) {
                    SUMOTime& timeForNext = myAmContainer ? stop.timeToLoadNextContainer : stop.timeToBoardNextPerson;
                    MSDevice_Taxi* taxiDevice = static_cast<MSDevice_Taxi*>(myHolder.getDevice(typeid(MSDevice_Taxi)));
                    if (taxiDevice != nullptr && timeForNext == 0 && !MSGlobals::gUseMesoSim) {
                        // taxi passengers must leave at the end of the stop duration
                        timeForNext = stop.pars.started + stop.pars.duration;
                    }
                    if (timeForNext - DELTA_T > currentTime) {
                        // try deboarding again in the next step
                        myStopped = false;
                        break;
                    }
                    if (stage->getDestinationStop() != nullptr) {
                        stage->getDestinationStop()->addTransportable(transportable);
                    }

                    SUMOTime arrivalTime = currentTime;
                    if (MSGlobals::gUseMesoSim) {
                        // no boarding / unboarding time in meso
                        arrivalTime += 1;
                    } else {
                        const SUMOTime boardingTime = (SUMOTime)((double)boardingDuration * transportable->getVehicleType().getBoardingFactor());
                        if (timeForNext > currentTime - DELTA_T) {
                            timeForNext += boardingTime;
                        } else {
                            timeForNext = currentTime + boardingTime;
                        }
                    }
                    //ensure that vehicle stops long enough for deboarding
                    stop.duration = MAX2(stop.duration, timeForNext - currentTime);

                    veh.removeTransportableMass(transportable);
                    i = myTransportables.erase(i); // erase first in case proceed throws an exception
                    numUnloaded++;
                    if (taxiDevice != nullptr) {
                        taxiDevice->customerArrived(transportable);
                    }
                    if (!transportable->proceed(MSNet::getInstance(), arrivalTime)) {
                        if (myAmContainer) {
                            MSNet::getInstance()->getContainerControl().erase(transportable);
                        } else {
                            MSNet::getInstance()->getPersonControl().erase(transportable);
                        }
                    }
                    if (MSStopOut::active()) {
                        SUMOVehicle* vehicle = dynamic_cast<SUMOVehicle*>(&veh);
                        if (myAmContainer) {
                            MSStopOut::getInstance()->unloadedContainers(vehicle, 1);
                        } else {
                            MSStopOut::getInstance()->unloadedPersons(vehicle, 1);
                        }
                    }
                    continue;
                }
                ++i;
            }
            if (numUnloaded != 0) {
                changeAttached();
            }
        }
    }
    return true;
}


bool
MSDevice_Transportable::notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
        const SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
        for (MSTransportable* const transportable : myTransportables) {
            transportable->setDeparted(currentTime);
        }
    }
    if (MSGlobals::gUseMesoSim) {
        // to trigger vehicle leaving
        notifyMove(veh, -1., -1., -1.);
    }
    return true;
}


bool
MSDevice_Transportable::notifyLeave(SUMOTrafficObject& veh, double /*lastPos*/,
                                    MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        for (std::vector<MSTransportable*>::iterator i = myTransportables.begin(); i != myTransportables.end();) {
            MSTransportableControl& tc = myAmContainer ? MSNet::getInstance()->getContainerControl() : MSNet::getInstance()->getPersonControl();
            MSTransportable* transportable = *i;
            if (transportable->getDestination() != veh.getEdge()) {
                WRITE_WARNINGF("Teleporting % '%' from vehicle destination edge '%' to intended destination edge '%' time=%",
                               myAmContainer ? "container" : "person", transportable->getID(), veh.getEdge()->getID(),
                               transportable->getDestination()->getID(), time2string(SIMSTEP));
                tc.registerTeleportWrongDest();
            }
            if (!transportable->proceed(MSNet::getInstance(), MSNet::getInstance()->getCurrentTimeStep(), true)) {
                tc.erase(transportable);
            }
            i = myTransportables.erase(i);
        }
    }
    return true;
}


void
MSDevice_Transportable::addTransportable(MSTransportable* transportable) {
    if (myTransportables.empty()) {
        myOriginalType = &myHolder.getVehicleType();
    }
    myTransportables.push_back(transportable);
    if (MSStopOut::active()) {
        if (myAmContainer) {
            MSStopOut::getInstance()->loadedContainers(&myHolder, 1);
        } else {
            MSStopOut::getInstance()->loadedPersons(&myHolder, 1);
        }
    }
    MSDevice_Taxi* taxiDevice = static_cast<MSDevice_Taxi*>(myHolder.getDevice(typeid(MSDevice_Taxi)));
    if (taxiDevice != nullptr) {
        taxiDevice->customerEntered(transportable);
    }
    changeAttached();
}


void
MSDevice_Transportable::removeTransportable(MSTransportable* transportable) {
    auto it = std::find(myTransportables.begin(), myTransportables.end(), transportable);
    if (it != myTransportables.end()) {
        myTransportables.erase(it);
        if (MSStopOut::active() && myHolder.isStopped()) {
            if (myAmContainer) {
                MSStopOut::getInstance()->unloadedContainers(&myHolder, 1);
            } else {
                MSStopOut::getInstance()->unloadedPersons(&myHolder, 1);
            }
        }
        MSDevice_Taxi* taxiDevice = static_cast<MSDevice_Taxi*>(myHolder.getDevice(typeid(MSDevice_Taxi)));
        if (taxiDevice != nullptr) {
            taxiDevice->customerArrived(transportable);
        }
        changeAttached();
    }
}


void
MSDevice_Transportable::changeAttached() {
    if (myLoadedType != nullptr) {
        int perAttached = myAmContainer ? myLoadedType->getContainerCapacity() : myLoadedType->getPersonCapacity();
        if (perAttached > 0) {
            MSBaseVehicle& veh = dynamic_cast<MSBaseVehicle&>(myHolder);
            SUMOVehicleClass oldVC = myHolder.getVClass();
            const double numAttached = ceil(myTransportables.size() / perAttached);
            if (numAttached > 0.) {
                MSVehicleType* stype = &veh.getSingularType();
                stype->setVClass(myLoadedType->getVehicleClass());
                stype->setGUIShape(myLoadedType->getGuiShape());
                stype->setLength(myOriginalType->getLength() + numAttached * myLoadedType->getLength());
                stype->setMass(myOriginalType->getMass() + numAttached * myLoadedType->getMass());
                SUMOVTypeParameter& sparam = const_cast<SUMOVTypeParameter&>(stype->getParameter());
                sparam.carriageLength = myLoadedType->getParameter().carriageLength;
                sparam.locomotiveLength = myLoadedType->getParameter().locomotiveLength;
                sparam.carriageGap = myLoadedType->getParameter().carriageGap;
            } else {
                myHolder.replaceVehicleType(myOriginalType);
            }
            if (oldVC != myHolder.getVClass()) {
                veh.reroute(SIMSTEP, "device." + deviceName() + ".loadedType", veh.getRouterTT());
            }
        }
    }
}


void
MSDevice_Transportable::saveState(OutputDevice& out) const {
    out.openTag(SUMO_TAG_DEVICE);
    out.writeAttr(SUMO_ATTR_ID, getID());
    std::vector<std::string> internals;
    internals.push_back(toString(myStopped));
    out.writeAttr(SUMO_ATTR_STATE, toString(internals));
    out.closeTag();
}


void
MSDevice_Transportable::loadState(const SUMOSAXAttributes& attrs) {
    std::istringstream bis(attrs.getString(SUMO_ATTR_STATE));
    bis >> myStopped;
}


std::string
MSDevice_Transportable::getParameter(const std::string& key) const {
    if (key == "IDList") {
        std::vector<std::string> ids;
        for (const MSTransportable* t : myTransportables) {
            ids.push_back(t->getID());
        }
        return toString(ids);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


/****************************************************************************/
