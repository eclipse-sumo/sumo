/****************************************************************************/
/// @file    MSDevice_BTreceiver.cpp
/// @author  Daniel Krajzewicz
/// @date    14.08.2013
/// @version $Id$
///
// A BT sender
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
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

#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/SUMOVehicle.h>
#include <utils/geom/Position.h>
#include <utils/geom/Line.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_BTreceiver.h"
#include "MSDevice_BTsender.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


#ifdef _DEBUG
#define DEBUG_OUTPUT 1
#else
#define DEBUG_OUTPUT 0
#endif



// ===========================================================================
// static members
// ===========================================================================
bool MSDevice_BTreceiver::myWasInitialised = false;
MTRand MSDevice_BTreceiver::myRecognitionRNG;
MSDevice_BTreceiver::BTreceiverUpdate *MSDevice_BTreceiver::myUpdater = 0;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_BTreceiver::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("btreceiver", "Communication", oc);

    oc.doRegister("device.btreceiver.range", new Option_Float(300));
    oc.addDescription("device.btreceiver.range", "Communication", "The range of the bt receiver");

    oc.doRegister("device.btreceiver.all-recognitions", new Option_Bool(false));
    oc.addDescription("device.btreceiver.all-recognitions", "Communication", "Whether all recognition point shall be written");
}


void
MSDevice_BTreceiver::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "btreceiver", v)) {
        MSDevice_BTreceiver* device = new MSDevice_BTreceiver(v, "btreceiver_" + v.getID(), oc.getFloat("device.btreceiver.range"));
        into.push_back(device);
        if(!myWasInitialised) {
            myUpdater = new BTreceiverUpdate();
            myWasInitialised = true;
            myRecognitionRNG.seed(oc.getInt("seed"));
        }
    }
}


// ---------------------------------------------------------------------------
// MSDevice_BTreceiver::BTreceiverUpdate-methods
// ---------------------------------------------------------------------------
MSDevice_BTreceiver::BTreceiverUpdate::BTreceiverUpdate() {
    MSNet::getInstance()->getEndOfTimestepEvents().addEvent(this, 0, MSEventControl::ADAPT_AFTER_EXECUTION);
    MSNet::getInstance()->addVehicleStateListener(this);
}


MSDevice_BTreceiver::BTreceiverUpdate::~BTreceiverUpdate() {
}


void 
MSDevice_BTreceiver::BTreceiverUpdate::vehicleStateChanged(const SUMOVehicle* const v, MSNet::VehicleState to) {
    MSVehicle *vehicle = (MSVehicle*) v;
    /// @todo: what about parking vehicles?
    if(to==MSNet::VEHICLE_STATE_DEPARTED) {
        if(vehicle->getDevice(typeid(MSDevice_BTreceiver))!=0) {
            myRunningReceiverVehicles.insert(vehicle);
        }
        if(vehicle->getDevice(typeid(MSDevice_BTsender))!=0) {
            myRunningSenderVehicles.insert(vehicle);
        }
        return;
    }
    // check other states (removal from net)
    if(to==MSNet::VEHICLE_STATE_ARRIVED) {
        std::set<MSVehicle*>::iterator i = myRunningReceiverVehicles.find(vehicle);
        if(i!=myRunningReceiverVehicles.end()) {
            MSDevice_BTreceiver *device = static_cast<MSDevice_BTreceiver*>(vehicle->getDevice(typeid(MSDevice_BTreceiver)));
            myArrivedReceiverVehicles.insert(new ArrivedVehicleInformation(vehicle->getID(), 
                vehicle->getSpeed(), vehicle->getPosition(), device->getCurrentlySeen(), device->getSeen()));
            device->unref();
            myRunningReceiverVehicles.erase(i);
        }
        i = myRunningSenderVehicles.find(vehicle);
        if(i!=myRunningSenderVehicles.end()) {
            myArrivedSenderVehicles.insert(new ArrivedVehicleInformation(vehicle->getID(), 
                vehicle->getSpeed(), vehicle->getPosition(), std::map<std::string, SeenDevice*>(), std::map<std::string, std::vector<SeenDevice*> >()));
            myRunningSenderVehicles.erase(i);
        }
        return;
    }
}


SUMOTime 
MSDevice_BTreceiver::BTreceiverUpdate::execute(SUMOTime /*currentTime*/) {
    // process arrived sender
    for (std::set<ArrivedVehicleInformation*>::iterator i = myArrivedSenderVehicles.begin(); i != myArrivedSenderVehicles.end(); ++i) {
        // remove from running receivers
        for(std::set<MSVehicle*>::iterator j=myRunningReceiverVehicles.begin(); j!=myRunningReceiverVehicles.end(); ++j) {
            MSDevice_BTreceiver *device = static_cast<MSDevice_BTreceiver*>((*j)->getDevice(typeid(MSDevice_BTreceiver)));
            //myCurrentlySeen.find(id)!=myCurrentlySeen.end();
            const std::map<std::string, SeenDevice*> &currentlySeen = device->getCurrentlySeen();
            if(currentlySeen.find((*i)->id)!=currentlySeen.end()) {
                device->leaveRange((*j)->getPosition(), (*j)->getSpeed(), (*i)->id, (*i)->position, (*i)->speed, 0, true);
            }
        }
        // remove from arrived receivers
        for(std::set<ArrivedVehicleInformation*>::iterator j=myArrivedReceiverVehicles.begin(); j!=myArrivedReceiverVehicles.end(); ++j) {
            std::map<std::string, SeenDevice*> &currentlySeen = (*j)->currentlySeen;
            if(currentlySeen.find((*i)->id)!=currentlySeen.end()) {
                leaveRange(currentlySeen, (*j)->seen, (*j)->position, (*j)->speed, (*i)->id, (*i)->position, (*i)->speed, 0, true);
            }
        }
        delete *i;
    }
    myArrivedSenderVehicles.clear();

    // process arrived receiver
    OptionsCont &oc = OptionsCont::getOptions();
    bool allRecognitions = oc.getBool("device.btreceiver.all-recognitions");
    bool haveOutput = oc.isSet("bt-output");
    for (std::set<ArrivedVehicleInformation*>::iterator i = myArrivedReceiverVehicles.begin(); i != myArrivedReceiverVehicles.end(); ++i) {
        // remove all (running) senders
        std::map<std::string, SeenDevice*> &currentlySeen = (*i)->currentlySeen;
        std::map<std::string, std::vector<SeenDevice*> > &seen = (*i)->seen;
        for (std::map<std::string, SeenDevice*>::iterator j = currentlySeen.begin(); j != currentlySeen.end(); ++j) {
            MSVehicle *vehicle = static_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().getVehicle((*j).first));
            leaveRange(currentlySeen, seen, (*i)->position, (*i)->speed, (*j).first, vehicle->getPosition(), vehicle->getSpeed(), 0, true);
        }
        // write results
        if(haveOutput) {
            writeOutput((*i)->id, seen, allRecognitions);
        }
        MSDevice_BTreceiver::cleanUp((*i)->currentlySeen, (*i)->seen);
        delete *i;
    }
    myArrivedReceiverVehicles.clear();

    // update running vehicles
    for(std::set<MSVehicle*>::const_iterator i=myRunningReceiverVehicles.begin(); i!=myRunningReceiverVehicles.end(); ++i) {
        if(!(*i)->isOnRoad()) {
            continue;
        }
        static_cast<MSDevice_BTreceiver*>((*i)->getDevice(typeid(MSDevice_BTreceiver)))->updateNeighbors();
    }
    return DELTA_T;
}



// ---------------------------------------------------------------------------
// MSDevice_BTreceiver-methods
// ---------------------------------------------------------------------------
MSDevice_BTreceiver::MSDevice_BTreceiver(SUMOVehicle& holder, const std::string& id,  SUMOReal range)
    : MSDevice(holder, id), myRange(range) {
}


MSDevice_BTreceiver::~MSDevice_BTreceiver() {
    // @todo: consider still recievers still running when the simulation is closed
    //myUpdater->vehicleStateChanged(&myHolder, MSNet::VEHICLE_STATE_ARRIVED);
    cleanUp(myCurrentlySeen, mySeen);
}


void
MSDevice_BTreceiver::cleanUp(std::map<std::string, MSDevice_BTreceiver::SeenDevice*> &c, 
                             std::map<std::string, std::vector<MSDevice_BTreceiver::SeenDevice*> > &s) {
    for(std::map<std::string, SeenDevice*>::iterator i=c.begin(); i!=c.end(); ++i) {
        delete (*i).second;
    }
    c.clear();
    for(std::map<std::string, std::vector<SeenDevice*> >::iterator i=s.begin(); i!=s.end(); ++i) {
        for(std::vector<SeenDevice*>::iterator j=(*i).second.begin(); j!=(*i).second.end(); ++j) {
            delete *j;
        }
    }
    s.clear();
}




void 
MSDevice_BTreceiver::enterRange(const Position &thisPos, SUMOReal thisSpeed, 
                                const std::string &otherID, const Position &atPos, SUMOReal otherSpeed, SUMOReal atOffset) {
    // add new vehicles to myCurrentlySeen
    if (myCurrentlySeen.find(otherID) == myCurrentlySeen.end()) {
        MeetingPoint mp(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())+atOffset, thisPos, thisSpeed, atPos, otherSpeed);
        SeenDevice* sd = new SeenDevice(mp);
        myCurrentlySeen[otherID] = sd;
    }
}


void 
MSDevice_BTreceiver::leaveRange(const Position &thisPos, SUMOReal thisSpeed, 
                                const std::string &otherID, const Position &otherPos, SUMOReal otherSpeed, SUMOReal otherOffset, bool remove) {
    leaveRange(myCurrentlySeen, mySeen, thisPos, thisSpeed,
        otherID, otherPos, otherSpeed, otherOffset, remove);
}


void 
MSDevice_BTreceiver::leaveRange(std::map<std::string, SeenDevice*> &currentlySeen, std::map<std::string, std::vector<SeenDevice*> > &seen,
                                const Position &thisPos, SUMOReal thisSpeed,
                                const std::string &otherID, const Position &otherPos, SUMOReal otherSpeed, SUMOReal tOffset, bool remove) {
    // check whether the other was recognized
    SUMOReal rp = recognizedAt(otherID, STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())+tOffset, currentlySeen);
    if(rp>0) {
        addRecognitionPoint(thisPos, thisSpeed, otherID, otherPos, otherSpeed, rp, currentlySeen);
    }
    // build leaving point
    MeetingPoint mp(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())+tOffset, thisPos, thisSpeed, otherPos, otherSpeed);
    std::map<std::string, SeenDevice*>::iterator i = currentlySeen.find(otherID);
    (*i).second->meetingEnd = mp;
    if (seen.find(otherID) == seen.end()) {
        seen[otherID] = std::vector<SeenDevice*>();
    }
    seen[otherID].push_back((*i).second);
    // 
    if(remove) {
        currentlySeen.erase(i);
    }
}


void 
MSDevice_BTreceiver::updateNeighbors() {
    if(!myHolder.isOnRoad()) {    
        return;
    }
    // collect edges around
    std::set<std::string> tmp;
    Named::StoringVisitor sv(tmp);
    if(DEBUG_OUTPUT) std::cout << "------------------" << std::endl;
    Position egoPosition = static_cast<MSVehicle&>(myHolder).getPosition();
    SUMOReal angle = static_cast<MSVehicle&>(myHolder).getAngle() * M_PI / 180.;
    SUMOReal speed = static_cast<MSVehicle&>(myHolder).getSpeed();
    SUMOReal dist = SPEED2DIST(speed);
    if(DEBUG_OUTPUT) std::cout << "ego " << myHolder.getID() << " " << MSNet::getInstance()->getCurrentTimeStep() << std::endl;
    if(DEBUG_OUTPUT) std::cout << " " << egoPosition << " " << angle << " " << speed << std::endl;
    Position egoP2 = egoPosition;
    Position egoP1(egoP2.x()-sin(angle)*dist, egoP2.y()+cos(angle)*dist);
    Position egoD = egoP2 - egoP1;
    if(DEBUG_OUTPUT) std::cout << " " << egoP1 << " " << egoP2 << " " << egoD << std::endl;
    Boundary b;
    b.add(egoPosition);
    b.grow(myRange + 500 / 3.6); // two vehicles passing at 250*2 km/h
    const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
    const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
    MSNet::getInstance()->getLanesRTree().Search(cmin, cmax, sv);

    // check vehicles in range first;
    //  determine when they've entered range
    std::set<std::string> seen;
    for (std::set<std::string>::const_iterator i = tmp.begin(); i != tmp.end(); ++i) {
        MSLane* l = MSLane::dictionary(*i);
        if (l == 0) {
            continue;
        }

        const MSLane::VehCont& vehs = l->getVehiclesSecure();
        for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
            if(static_cast<MSVehicle*>(*j)->getDevice(typeid(MSDevice_BTsender))==0) {
                // skipping not equipped vehicles
                continue;
            }
            if(&myHolder==*j) {
                continue;
            }
            
            // get the encountered vehicle's current and prior position (based on speed and direction)
            Position otherPosition = (*j)->getPosition();
            Position otherP2 = (*j)->getPosition();

            SUMOReal angle = (*j)->getAngle() * M_PI / 180.;
            SUMOReal speed = (*j)->getSpeed();
            SUMOReal dist = SPEED2DIST(speed);
            if(DEBUG_OUTPUT) std::cout << "other" << std::endl;
            if(DEBUG_OUTPUT) std::cout << " " << otherP2 << " " << angle << " " << speed << std::endl;
            Position otherP1(otherPosition.x()-sin(angle)*dist, otherPosition.y()+cos(angle)*dist);
            Position otherD = otherP2 - otherP1;
            if(DEBUG_OUTPUT) std::cout << " " << otherP1 << " " << otherP2 << " " << otherD << std::endl;
            // let the other's current position be the one obtained by applying the relative direction vector to the initial position
            otherP2 = otherP1 - egoD + otherD;
            if(DEBUG_OUTPUT) std::cout << " " << otherP1 << " " << otherP2 << std::endl;
            // find crossing points
            std::vector<SUMOReal> intersections;
            GeomHelper::FindLineCircleIntersections(egoP1, myRange, otherP1, otherP2, intersections);
            int count = intersections.size();
            if(DEBUG_OUTPUT) std::cout << " " << count << " " 
                << (*j)->getPosition().distanceTo(static_cast<const MSVehicle&>(myHolder).getPosition()) << " " 
                << otherP1.distanceTo(static_cast<const MSVehicle&>(myHolder).getPosition()) << " " 
                << otherP2.distanceTo(static_cast<const MSVehicle&>(myHolder).getPosition()) << " " 
                << std::endl;

            if(intersections.size()==0) {
                // no state change (is either still outside or still within the range)
                if((*j)->getPosition().distanceTo(egoPosition)<myRange) {
                    seen.insert((*j)->getID());
                    if(myCurrentlySeen.find((*j)->getID())==myCurrentlySeen.end()) {
                        // was not seen before, enter
                        enterRange(egoPosition, myHolder.getSpeed(), (*j)->getID(), (*j)->getPosition(), (*j)->getSpeed(), 0);
                    }
                }
                continue;
            }

            std::sort(intersections.begin(), intersections.end());
            seen.insert((*j)->getID());
            if(myCurrentlySeen.find((*j)->getID())!=myCurrentlySeen.end()) {
                // vehicle was within range
                if(count==2) {
                    // should not happen
                    WRITE_WARNING("Vehicle '" + (*j)->getID() + "' was in range of '" + myHolder.getID() + "' but crosses the circle twice.");
                } else {
                    // vehicle leaves range
                    Position intersection1Other = otherP1 + otherD * intersections.front();
                    Position intersection1Ego = egoP1 + egoD * intersections.front();
                    leaveRange(intersection1Ego, myHolder.getSpeed(), (*j)->getID(), intersection1Other, (*j)->getSpeed(), -1.+intersections.front(), true);
                }
            } else {
                // vehicle was outside range
                if(count==1) {
                    // entered only
                    Position intersection1Other = otherP1 + otherD * intersections.front();
                    Position intersection1Ego = egoP1 + egoD * intersections.front();
                    enterRange(intersection1Ego, myHolder.getSpeed(), (*j)->getID(), intersection1Other, (*j)->getSpeed(), -1.+intersections.front());
                } else {
                    Position intersection1Other = otherP1 + otherD * intersections.front();
                    Position intersection1Ego = egoP1 + egoD * intersections.front();
                    enterRange(intersection1Ego, myHolder.getSpeed(), (*j)->getID(), intersection1Other, (*j)->getSpeed(), -1.+intersections.front());
                    Position intersection2Other = otherP1 + otherD * intersections[1];
                    Position intersection2Ego = egoP1 + egoD * intersections.front();
                    leaveRange(intersection2Ego, myHolder.getSpeed(), (*j)->getID(), intersection2Other, (*j)->getSpeed(), -1.+intersections[1], true);
                }
            }
        }
        l->releaseVehicles();
    }
    // check vehicles that are not longer in range
    //  set their range exit information
    for (std::map<std::string, SeenDevice*>::iterator i = myCurrentlySeen.begin(); i != myCurrentlySeen.end();) {
        if (seen.find((*i).first) != seen.end()) {
            // skip those which are still visible
            MSVehicle *v = static_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().getVehicle((*i).first));
            const Position p(static_cast<const MSVehicle&>(myHolder).getPosition());
            SUMOReal rp = recognizedAt(v->getID(), STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())+1., myCurrentlySeen);
            if(rp>0) {
                addRecognitionPoint(p, myHolder.getSpeed(), v->getID(), v->getPosition(), v->getSpeed(), rp, myCurrentlySeen);
            }
            ++i;
            continue;
        }
        MSVehicle *v = static_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().getVehicle((*i).first));
        const Position p(static_cast<const MSVehicle&>(myHolder).getPosition());
        if(v->getPosition().distanceTo(p)<myRange) {
            // still in range
            SUMOReal rp = recognizedAt(v->getID(), STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())+1., myCurrentlySeen);
            if(rp>0) {
                addRecognitionPoint(p, myHolder.getSpeed(), v->getID(), v->getPosition(), v->getSpeed(), rp, myCurrentlySeen);
            }
            ++i;
        } else {
            // leaving range
            WRITE_WARNING("Vehicle '" + (*i).first + "' was in range of '" + myHolder.getID() + "' and is no longer visible.");
            leaveRange(p, myHolder.getSpeed(), v->getID(), v->getPosition(), v->getSpeed(), 0);
            myCurrentlySeen.erase(i++);
        }
    }
}


SUMOReal
MSDevice_BTreceiver::recognizedAt(const std::string &otherID, SUMOReal tEnd, std::map<std::string, SeenDevice*> &currentlySeen) {
    SUMOReal t = tEnd - currentlySeen.find(otherID)->second->lastView;
    if(myRecognitionRNG.rand() <= 1-exp(-0.24*pow(t, 2.68))) {
        currentlySeen.find(otherID)->second->lastView = tEnd;
        return tEnd;
    }
    return -1;
}


void 
MSDevice_BTreceiver::addRecognitionPoint(const Position &thisPos, SUMOReal thisSpeed, 
                                         const std::string &otherID, const Position &otherPos, SUMOReal otherSpeed, SUMOReal t, 
                                         std::map<std::string, SeenDevice*> &currentlySeen) {
    MeetingPoint *mp = new MeetingPoint(t, thisPos, thisSpeed, otherPos, otherSpeed);
    currentlySeen.find(otherID)->second->recognitionPoints.push_back(mp);
}


void 
MSDevice_BTreceiver::writeOutput(const std::string &id, const std::map<std::string, std::vector<SeenDevice*> > &seen, bool allRecognitions) {
            OutputDevice& os = OutputDevice::getDeviceByOption("bt-output");
            os.openTag("bt").writeAttr("id", id);
            for(std::map<std::string, std::vector<SeenDevice*> >::const_iterator j=seen.begin(); j!=seen.end(); ++j) {
                const std::vector<SeenDevice*> &sts = (*j).second;
                for(std::vector<SeenDevice*>::const_iterator k=sts.begin(); k!=sts.end(); ++k) {
                    os.openTag("seen").writeAttr("id", (*j).first);
                    os.writeAttr("tBeg", (*k)->meetingBegin.t)
                        .writeAttr("observerPosBeg", (*k)->meetingBegin.observerPos).writeAttr("observerSpeedBeg", (*k)->meetingBegin.observerSpeed)
                        .writeAttr("seenPosBeg", (*k)->meetingBegin.seenPos).writeAttr("seenSpeedBeg", (*k)->meetingBegin.seenSpeed);
                    os.writeAttr("tEnd", (*k)->meetingEnd.t)
                        .writeAttr("observerPosEnd", (*k)->meetingEnd.observerPos).writeAttr("observerSpeedEnd", (*k)->meetingEnd.observerSpeed)
                        .writeAttr("seenPosEnd", (*k)->meetingEnd.seenPos).writeAttr("seenSpeedEnd", (*k)->meetingEnd.seenSpeed);
                    for(std::vector<MeetingPoint*>::iterator l=(*k)->recognitionPoints.begin(); l!=(*k)->recognitionPoints.end(); ++l) {
                        os.openTag("recognitionPoint").writeAttr("t", (*l)->t)
                            .writeAttr("observerPos", (*l)->observerPos).writeAttr("observerSpeed", (*l)->observerSpeed)
                            .writeAttr("seenPos", (*l)->seenPos).writeAttr("seenSpeed", (*l)->seenSpeed)
                            .closeTag();
                        if(!allRecognitions) {
                            break;
                        }
                    }
                    os.closeTag();
                }
            }
            os.closeTag();
}



/****************************************************************************/

