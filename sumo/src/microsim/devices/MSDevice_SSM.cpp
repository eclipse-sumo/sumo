/****************************************************************************/
/// @file    MSDevice_SSM.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Leonhard Luecken
/// @date    11.06.2013
/// @version $Id$
///
// An SSM-device logs encounters / conflicts of the carrying vehicle with other surrounding vehicles
// XXX: Do not use! Implementation is not complete, yet.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors
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

#include <iostream>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLane.h>
#include <microsim/output/MSCrossSection.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include "MSDevice_SSM.h"

// ===========================================================================
// Debug constants
// ===========================================================================
//#define DEBUG_SSM
//#define DEBUG_SSM_SURROUNDING
// #define DEBUG_SSM_NOTIFICATIONS

// ===========================================================================
// Constants
// ===========================================================================
// value indicating an invalid double parameter
#define INVALID std::numeric_limits<double>::max()
// default value for the maximal episode length for logged encounters
#define DEFAULT_MAX_ENCOUNTER_LENGTH 30.0
// default value for the detection range of potential opponents
#define DEFAULT_RANGE 50.0

// list of implemented SSMs (NOTE: To add more SSMs identifiers are added to AVAILABLE_SSMS
//                                 and a default threshold must be defined. A corresponding
//                                 case should be added to the switch in buildVehicleDevices)
#define AVAILABLE_SSMS "TTC DRAC PET"
#define DEFAULT_THRESHOLD_TTC 3. // in [s.], events get logged if below threshold
#define DEFAULT_THRESHOLD_DRAC 4. // in [m/s^2], events get logged if above threshold
#define DEFAULT_THRESHOLD_PET 2. // in seconds, events get logged if below threshold
#define DEFAULT_EXTRA_TIME 5.      // in seconds, events get logged if below threshold

// ===========================================================================
// method definitions
// ===========================================================================



/// Nicer output for EncounterType enum
std::ostream& operator<<(std::ostream& out, MSDevice_SSM::EncounterType type) {
    switch (type) {
        case MSDevice_SSM::ENCOUNTER_TYPE_NOCONFLICT_AHEAD: out << "NOCONFLICT_AHEAD"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_FOLLOWING: out << "FOLLOWING"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_FOLLOWING_FOLLOWER: out << "FOLLOWING_FOLLOWER"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_FOLLOWING_LEADER: out << "FOLLOWING_LEADER"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_MERGING: out << "MERGING"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_MERGING_LEADER: out << "MERGING_LEADER"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_MERGING_FOLLOWER: out << "MERGING_FOLLOWER"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_CROSSING: out << "CROSSING"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_CROSSING_LEADER: out << "CROSSING_LEADER"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_CROSSING_FOLLOWER: out << "CROSSING_FOLLOWER"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_EGO_PASSED_CP: out << "EGO_PASSED_CP"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_FOE_PASSED_CP: out << "FOE_PASSED_CP"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_BOTH_PASSED_CP: out << "BOTH_PASSED_CP"; break;
        case MSDevice_SSM::ENCOUNTER_TYPE_COLLISION: out << "COLLISION"; break;
        default: out << "unknown type (" << int(type) << ")"; break;
    }
    return out;
}


// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------

std::set<MSDevice*>* MSDevice_SSM::instances = new std::set<MSDevice*>();

const std::set<MSDevice*>&
MSDevice_SSM::getInstances() {
    return *instances;
}

void
MSDevice_SSM::cleanup() {
    // Close current encounters and flush conflicts to file for all existing devices
    if (instances != 0) {
        for (std::set<MSDevice*>::iterator ii = instances->begin(); ii != instances->end(); ++ii) {
            static_cast<MSDevice_SSM*>(*ii)->resetEncounters();
            static_cast<MSDevice_SSM*>(*ii)->flushConflicts(true);
        }
        instances->clear();
    }
}

void
MSDevice_SSM::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("SSM Device");
    insertDefaultAssignmentOptions("ssm", "SSM Device", oc);

    // custom options
    oc.doRegister("device.ssm.measures", new Option_String(""));
    oc.addDescription("device.ssm.measures", "SSM Device", "Specifies which measures will be logged (as a space seperated sequence of IDs in ('TTC', 'DRAC', 'PET')).");
    oc.doRegister("device.ssm.thresholds", new Option_String(""));
    oc.addDescription("device.ssm.thresholds", "SSM Device", "Specifies thresholds corresponding to the specified measures (see documentation and watch the order!). Only events exceeding the thresholds will be logged.");
    oc.doRegister("device.ssm.trajectories", new Option_Bool(false));
    oc.addDescription("device.ssm.trajectories", "SSM Device", "Specifies whether trajectories will be logged (if false, only the extremal values and times are reported).");
    oc.doRegister("device.ssm.maxencounterlength", new Option_Float(DEFAULT_MAX_ENCOUNTER_LENGTH));
    oc.addDescription("device.ssm.maxencounterlength", "SSM Device", "Specifies the maximal length of stored conflict trajectories in seconds (the frequency of logging, default is " + toString(DEFAULT_MAX_ENCOUNTER_LENGTH) + "s.).");
    oc.doRegister("device.ssm.range", new Option_Float(DEFAULT_RANGE));
    oc.addDescription("device.ssm.range", "SSM Device", "Specifies the detection range in meters (default is " + toString(DEFAULT_RANGE) + "m.). For vehicles below this distance from the equipped vehicle, SSM values are traced.");
    oc.doRegister("device.ssm.extratime", new Option_Float(DEFAULT_EXTRA_TIME));
    oc.addDescription("device.ssm.extratime", "SSM Device", "Specifies the time in seconds to be logged after a conflict is over (default is " + toString(DEFAULT_EXTRA_TIME) + "secs.). Required >0 if PET is to be calculated for crossing conflicts.");
}

void
MSDevice_SSM::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    if (equippedByDefaultAssignmentOptions(OptionsCont::getOptions(), "ssm", v)) {
        if (MSGlobals::gUseMesoSim) {
            WRITE_WARNING("SSM Device for vehicle '" + v.getID() + "' will not be built. (SSMs not supported in MESO)");
            return;
        }
        // ID for the device
        std::string deviceID = "ssm_" + v.getID();

        // Load parameters:

        // Measures and thresholds
        std::map<std::string, double> thresholds;
        bool success = getMeasuresAndThresholds(v, deviceID, thresholds);
        if (!success) {
            return;
        }

        // Trajectories
        bool trajectories = requestsTrajectories(v);

        // max encounter length
        double maxEncounterLength = getMaxEncounterLength(v);

        // detection range
        double range = getDetectionRange(v);

        // extra time
        double extraTime = getExtraTime(v);

        // File
        std::string file = getOutputFilename(v, deviceID);

        // Build the device (XXX: who deletes it?)
        MSDevice_SSM* device = new MSDevice_SSM(v, deviceID, file, thresholds, trajectories, maxEncounterLength, range, extraTime);
        into.push_back(device);
    }
}


MSDevice_SSM::Encounter::Encounter(const MSVehicle* _ego, const MSVehicle* const _foe, double _begin, double extraTime) :
    ego(_ego),
    foe(_foe),
    egoID(_ego->getID()),
    foeID(_foe->getID()),
    begin(_begin),
    end(-INVALID),
    remainingExtraTime(extraTime),
    egoConflictLane(0),
    foeConflictLane(0),
    egoConflictEntryTime(INVALID),
    egoConflictExitTime(INVALID),
    foeConflictEntryTime(INVALID),
    foeConflictExitTime(INVALID),
    minTTC(std::make_pair(INVALID, INVALID)),
    maxDRAC(std::make_pair(INVALID, INVALID)),
    PET(std::make_pair(INVALID, INVALID)),
    closingRequested(false) {
#ifdef DEBUG_SSM
    std::cout << "\n" << SIMTIME << " Constructing encounter of '"
              << ego->getID() << "' and '" << foe->getID() << "'" << std::endl;
#endif
}

MSDevice_SSM::Encounter::~Encounter() {
#ifdef DEBUG_SSM
    std::cout << "\n" << SIMTIME << " Destroying encounter of '"
            << egoID << "' and '" << foeID << "' (begin was " << begin << ")" << std::endl;
#endif
}


void
MSDevice_SSM::Encounter::add(double time, const EncounterType type, Position egoX, Position egoV, Position foeX, Position foeV,
        double egoDistToConflict, double foeDistToConflict, double ttc, double drac, std::pair<double,double> pet) {
#ifdef DEBUG_SSM
    std::cout << time << " Adding data point for encounter of '" << egoID << "' and '" << foeID << "':\n"
            << "type="<<type<<", egoDistToConflict="<<(egoDistToConflict==INVALID?"NA":toString(egoDistToConflict))
            <<", foeDistToConflict="<<(foeDistToConflict==INVALID?"NA":toString(foeDistToConflict))
            <<",\nttc="<<(ttc==INVALID?"NA":toString(ttc))
            <<", drac="<<(drac==INVALID?"NA":toString(drac))
            <<", pet="<<(pet.second==INVALID?"NA":toString(pet.second))
            << std::endl;
#endif
    timeSpan.push_back(time);
    typeSpan.push_back(type);
    egoTrajectory.x.push_back(egoX);
    egoTrajectory.v.push_back(egoV);
    foeTrajectory.x.push_back(foeX);
    foeTrajectory.v.push_back(foeV);
    egoDistsToConflict.push_back(egoDistToConflict);
    foeDistsToConflict.push_back(foeDistToConflict);

    TTCspan.push_back(ttc);
    if (ttc != INVALID && (ttc < minTTC.second || minTTC.second==INVALID)) {
        minTTC = std::make_pair(time, ttc);
    }

    DRACspan.push_back(drac);
    if (drac != INVALID && (drac > maxDRAC.second || maxDRAC.second==INVALID)) {
        maxDRAC = std::make_pair(time, drac);
    }

    if (pet.first != INVALID && (PET.second >= pet.second || PET.second == INVALID)) {
        PET = pet;
    }
}


void
MSDevice_SSM::Encounter::resetExtraTime(double value){
    remainingExtraTime = value;
}


void
MSDevice_SSM::Encounter::countDownExtraTime(double amount){
    remainingExtraTime -= amount;
}


double
MSDevice_SSM::Encounter::getRemainingExtraTime() const{
    return remainingExtraTime;
}


MSDevice_SSM::EncounterApproachInfo::EncounterApproachInfo(Encounter* e) :
            encounter(e),
            type(ENCOUNTER_TYPE_NOCONFLICT_AHEAD),
            egoConflictLane(0),
            foeConflictLane(0),
            egoConflictEntryDist(INVALID),
            foeConflictEntryDist(INVALID),
            egoConflictExitDist(INVALID),
            foeConflictExitDist(INVALID),
            egoEstimatedConflictEntryTime(INVALID),
            foeEstimatedConflictEntryTime(INVALID),
            egoEstimatedConflictExitTime(INVALID),
            foeEstimatedConflictExitTime(INVALID),
            egoConflictAreaLength(INVALID),
            foeConflictAreaLength(INVALID),
            egoLeftConflict(false),
            foeLeftConflict(false),
            ttc(INVALID),
            drac(INVALID),
            pet(std::make_pair(INVALID, INVALID))
{}


void
MSDevice_SSM::updateAndWriteOutput() {
    if (myHolder.isOnRoad()) {
        update();
        flushConflicts();
    } else {
        resetEncounters();
        flushConflicts(true);
    }
}

void
MSDevice_SSM::update() {
#ifdef DEBUG_SSM
    std::cout << "\n" << SIMTIME << " Device '" << getID() << "' update()\n"
              << "Size of myActiveEncounters: " << myActiveEncounters.size()
              << "\nSize of myPastConflicts: " << myPastConflicts.size()
              << std::endl;
#endif
    // Scan surroundings for other vehicles
    FoeInfoMap foes;
    findSurroundingVehicles(*myHolderMS, myRange, foes);

#ifdef DEBUG_SSM
    if (foes.size()>0) {
        std::cout << "Scanned surroundings: Found potential foes:\n";
        for (FoeInfoMap::const_iterator i = foes.begin(); i!=foes.end(); ++i){
            std::cout << i->first->getID() << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "Scanned surroundings: No potential conflict could be identified." << std::endl;
    }
#endif

    // Update encounters and conflicts -> removes all foes (and deletes corresponding FoeInfos) for which already a corresponding encounter exists
    processEncounters(foes);

    // Make new encounters for all foes, which were not removed by processEncounters (and deletes corresponding FoeInfos)
    createEncounters(foes);
    foes.clear();

    // Write out past conflicts
    flushConflicts();

}

void
MSDevice_SSM::createEncounters(FoeInfoMap& foes) {
#ifdef DEBUG_SSM
    std::cout << "\n" << SIMTIME << " Device '" << getID() << "' createEncounters()" << std::endl;
    std::cout << "New foes:\n";
    for (FoeInfoMap::const_iterator vi = foes.begin(); vi != foes.end(); ++vi) {
        std::cout << vi->first->getID() << "\n";
    }
    std::cout << std::endl;
#endif

    for (FoeInfoMap::const_iterator foe = foes.begin(); foe != foes.end(); ++foe) {
        std::pair<MSLane*, MSLane*> conflictLanes;
        Encounter* e = new Encounter(myHolderMS, foe->first, SIMTIME, myExtraTime);
        updateEncounter(e, foe->second); // deletes foe->second
        myActiveEncounters.push_back(e);
    }
}

void
MSDevice_SSM::resetEncounters() {
    // Call processEncounters() with empty vehicle set
    FoeInfoMap foes;
    // processEncounters with empty argument closes all encounters
    processEncounters(foes, true);
}

void
MSDevice_SSM::processEncounters(FoeInfoMap& foes, bool forceClose) {
#ifdef DEBUG_SSM
    std::cout << "\n" << SIMTIME << " Device '" << getID() << "' processEncounters()" << std::endl;
    std::cout << "Currently present foes:\n";
    for (FoeInfoMap::const_iterator vi = foes.begin(); vi != foes.end(); ++vi) {
        std::cout << vi->first->getID() << "\n";
    }
    std::cout << std::endl;
#endif

    // Run through active encounters. If corresponding foe is still present in foes update and
    // remove foe from foes. If the foe has disappeared close the encounter (check if it qualifies
    // as a conflict and in case transfer it to myPastConflicts).
    // Afterwards run through remaining elements in foes and create new encounters for them.

    EncounterVector::iterator ei = myActiveEncounters.begin();
    while (ei != myActiveEncounters.end()) {
        Encounter* e = *ei;
        // check whether foe is still on net
        bool foeExists = !(MSNet::getInstance()->getVehicleControl().getVehicle(e->foeID) == 0);
        if (!foeExists) e->foe = 0;
        if (foes.find(e->foe) != foes.end()) {
            FoeInfo* foeInfo = foes[e->foe];
            // Update encounter
            updateEncounter(e, foeInfo); // deletes foeInfo
            // Erase foes which were already encountered
            foes.erase(e->foe);
        } else {
            if (e->getRemainingExtraTime() <= 0. || forceClose || !foeExists) {
                // Close encounter, extra time has expired (deletes e if it does not qualify as conflict)
                e->closingRequested = true;
            } else {
                updateEncounter(e, 0); // counts down extra time
            }
        }

        if (e->closingRequested) {
            closeEncounter(e);
            ei = myActiveEncounters.erase(ei);
        } else {
            ++ei;
        }
    }
}


bool
MSDevice_SSM::qualifiesAsConflict(Encounter* e) {
    // Check if conflict measure thresholds are exceeded (to decide whether to keep the encounter for writing out)
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " qualifiesAsConflict() for encounter of vehicles '"
            << e->egoID << "' and '" << e->foeID
            << "'" << std::endl;
#endif

    if (myComputePET && e->PET.second != INVALID && e->PET.second <= myThresholds["PET"]) {
        return true;
    }
    if (myComputeTTC && e->minTTC.second != INVALID && e->minTTC.second <= myThresholds["TTC"]) {
        return true;
    }
    if (myComputeDRAC && e->maxDRAC.second != INVALID && e->maxDRAC.second >= myThresholds["DRAC"]) {
        return true;
    }
    return false;
}


void
MSDevice_SSM::closeEncounter(Encounter* e) {
    // erase pointers (encounter is stored before being destroyed and pointers could become invalid)
    e->ego = 0; e->foe = 0;
    e->end = e->timeSpan.back();
    bool wasConflict=qualifiesAsConflict(e);
    if (wasConflict) {
        myPastConflicts.push(e);
    } else {
        delete e;
    }
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " closeEncounter() of vehicles '"
            << e->egoID << "' and '" << e->foeID
            << "' (was ranked as "<< (wasConflict?"conflict":"non-conflict") <<")" << std::endl;
#endif

    return;
}


void
MSDevice_SSM::updateEncounter(Encounter* e, FoeInfo* foeInfo) {
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " updateEncounter() of vehicles '"
              << e->egoID << "' and '" << e->foeID
              << "'" << std::endl;
#endif
    assert(e->foe!=0);

    // Struct storing distances (determined in classifyEncounter()) and times to potential conflict entry / exit (in estimateConflictTimes())
    EncounterApproachInfo eInfo(e);

    // Classify encounter type based on the present information (more details on follower/lead situation are determined in a second step below)
    eInfo.type = classifyEncounter(foeInfo, eInfo);

    // Remember the conflict lane for the ego vehicle
    // (this assignment is not done directly in classifyEncounter because that mthod should not manipulate e)
    if (eInfo.egoConflictLane != 0) e->egoConflictLane = eInfo.egoConflictLane;
    // Remember the conflict lane for the foe vehicle
    if (eInfo.foeConflictLane != 0) e->foeConflictLane = eInfo.foeConflictLane;

    if (eInfo.type == ENCOUNTER_TYPE_NOCONFLICT_AHEAD) {
        // At this state, eInfo.type == ENCOUNTER_TYPE_NOCONFLICT_AHEAD implies that the foe
        // is either out of the device's range or its route does not interfere with the ego's route.
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " Encounter of vehicles '"
            << e->egoID << "' and '" << e->foeID
            << "' is classified as passed encounter." << std::endl;
#endif
        updatePassedEncounter(e, foeInfo, eInfo);
//        return;
    } else {
        // TODO: reset entry and exit times after reclassification to pre-conflict, eventually

        // Estimate times until a possible conflict / collision
        // Not all are used for all types of encounters:
        // Follow/lead situation doesn't need them at all, currently (might change if more SSMs are implemented).
        // Crossing / Merging calculates entry times to determine leader/follower and calculates the exit time for the leader.
        estimateConflictTimes(eInfo);

        // reset the remaining extra time (foe could have re-entered the device range after beginning extra time countdown already)
        e->resetExtraTime(myExtraTime);
    }

    // update entry/exit times for conflict area
    checkConflictEntryAndExit(eInfo);

    // Compute SSMs
    computeSSMs(eInfo);

    // Add current states to trajectories
    e->add(SIMTIME, eInfo.type, e->ego->getPosition(), e->ego->getVelocityVector(), e->foe->getPosition(), e->foe->getVelocityVector(),
            eInfo.egoConflictEntryDist, eInfo.foeConflictEntryDist, eInfo.ttc, eInfo.drac, eInfo.pet);

    // free foeInfo
    delete foeInfo;

}


void
MSDevice_SSM::estimateConflictTimes(EncounterApproachInfo& eInfo) {

    EncounterType& type = eInfo.type;
    Encounter* e = eInfo.encounter;

    assert(type != ENCOUNTER_TYPE_NOCONFLICT_AHEAD); // arrival times not defined, if no conflict is ahead.
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " estimateConflictTimes() for ego '" << e->egoID << "' and foe '" << e->foeID << "'\n"
            << "    egoConflictEntryDist=" << (eInfo.egoConflictEntryDist==INVALID?"NA":toString(eInfo.egoConflictEntryDist))
            << ", foeConflictEntryDist=" << (eInfo.foeConflictEntryDist==INVALID?"NA":toString(eInfo.foeConflictEntryDist))
            << "\n    ego speed=" << e->ego->getSpeed()
            << ", foe speed=" << e->foe->getSpeed()
            << std::endl;
#endif

    if (type == ENCOUNTER_TYPE_FOLLOWING_FOLLOWER || type == ENCOUNTER_TYPE_FOLLOWING_LEADER){
        // No need to know the times until ...ConflictDistEntry, currently. They would correspond to an estimated time headway or similar.
        // TTC must take into account the movement of the leader, as would DRAC, PET doesn't need the time either, since it uses aposteriori
        // values.
#ifdef DEBUG_SSM
    std::cout << "    Follow/Lead situation no entry/exit times to be calculated."
            << std::endl;
#endif
    return;
    }

    assert(type == ENCOUNTER_TYPE_MERGING || type == ENCOUNTER_TYPE_CROSSING);

    // Determine exit distances
    if (type == ENCOUNTER_TYPE_CROSSING) {
        eInfo.egoConflictExitDist = eInfo.egoConflictEntryDist + eInfo.egoConflictAreaLength + e->ego->getVehicleType().getLength();
        eInfo.foeConflictExitDist = eInfo.foeConflictEntryDist + eInfo.foeConflictAreaLength + e->foe->getVehicleType().getLength();
    } else {
        assert(type == ENCOUNTER_TYPE_MERGING);
        eInfo.egoConflictExitDist = eInfo.egoConflictEntryDist + e->ego->getVehicleType().getLength();
        eInfo.foeConflictExitDist = eInfo.foeConflictEntryDist + e->foe->getVehicleType().getLength();
    }

    // Estimate entry times to stipulate a leader / follower relation for the encounter.
    if (eInfo.egoConflictEntryDist > 0.){
        eInfo.egoEstimatedConflictEntryTime = e->ego->getCarFollowModel().estimateArrivalTime(eInfo.egoConflictEntryDist, e->ego->getSpeed(), e->ego->getMaxSpeedOnLane(), MIN2(0., e->ego->getAcceleration()));
        assert(eInfo.egoEstimatedConflictEntryTime > 0.);
    } else {
        // ego already entered conflict area
        eInfo.egoEstimatedConflictEntryTime = 0.;
    }
    if (eInfo.foeConflictEntryDist > 0.){
        eInfo.foeEstimatedConflictEntryTime = e->foe->getCarFollowModel().estimateArrivalTime(eInfo.foeConflictEntryDist, e->foe->getSpeed(), e->foe->getMaxSpeedOnLane(), MIN2(0., e->foe->getAcceleration()));
        assert(eInfo.foeEstimatedConflictEntryTime > 0.);
    } else {
        // foe already entered conflict area
        eInfo.foeEstimatedConflictEntryTime = 0.;
    }

#ifdef DEBUG_SSM
    std::cout << "    Potential conflict type: " << (type == ENCOUNTER_TYPE_CROSSING ? "CROSSING" : "MERGING") << "\n"
            << "    egoConflictEntryTime=" << (eInfo.egoEstimatedConflictEntryTime==INVALID?"INVALID":toString(eInfo.egoEstimatedConflictEntryTime))
            << ", foeConflictEntryTime=" << (eInfo.foeEstimatedConflictEntryTime==INVALID?"INVALID":toString(eInfo.foeEstimatedConflictEntryTime))
            << std::endl;
#endif

    // Estimate exit times from conflict area for leader / follower.
    if (eInfo.egoConflictExitDist >= 0.) {
        eInfo.egoEstimatedConflictExitTime = e->ego->getCarFollowModel().estimateArrivalTime(eInfo.egoConflictExitDist, e->ego->getSpeed(), e->ego->getMaxSpeedOnLane(), MIN2(0., e->ego->getAcceleration()));
    } else {
        eInfo.egoEstimatedConflictExitTime = 0.;
    }
    if (eInfo.foeConflictExitDist >= 0.) {
        eInfo.foeEstimatedConflictExitTime = e->foe->getCarFollowModel().estimateArrivalTime(eInfo.foeConflictExitDist, e->foe->getSpeed(), e->foe->getMaxSpeedOnLane(), MIN2(0., e->foe->getAcceleration()));
    } else {
        eInfo.foeEstimatedConflictExitTime = 0.;
    }

    // For merging and crossing situation, the leader/follower relation not determined by classifyEncounter()
    // This is done below based on the estimated conflict entry times
    if (eInfo.egoEstimatedConflictEntryTime == 0. && eInfo.foeEstimatedConflictEntryTime == 0.){
        eInfo.type = ENCOUNTER_TYPE_COLLISION;
        std::stringstream ss;
        ss << "SSM device of vehicle '" << e->egoID << "' detected collision with vehicle '" << e->foeID << "'";
        WRITE_WARNING(ss.str());
    } else if (eInfo.egoEstimatedConflictEntryTime < eInfo.foeEstimatedConflictEntryTime){
        // ego is estimated first at conflict point
#ifdef DEBUG_SSM
    std::cout << "    -> ego is estimated leader at conflict entry."
            << " egoConflictExitTime=" << eInfo.egoEstimatedConflictExitTime
            << std::endl;
#endif
        type = type == ENCOUNTER_TYPE_CROSSING ? ENCOUNTER_TYPE_CROSSING_LEADER : ENCOUNTER_TYPE_MERGING_LEADER;
    } else {
        // ego is estimated second at conflict point
#ifdef DEBUG_SSM
    std::cout << "    -> foe is estimated leader at conflict entry."
            << " foeConflictExitTime=" << eInfo.foeEstimatedConflictExitTime
            << std::endl;
#endif
        type = type == ENCOUNTER_TYPE_CROSSING ? ENCOUNTER_TYPE_CROSSING_FOLLOWER : ENCOUNTER_TYPE_MERGING_FOLLOWER;
    }

}



void
MSDevice_SSM::computeSSMs(EncounterApproachInfo& eInfo) const {
    // TODO: log the conflict point coordinates!

#ifdef DEBUG_SSM
    Encounter* e = eInfo.encounter;
    std::cout << SIMTIME << " computeSSMs() for vehicles '"
            << e->ego->getID() << "' and '" << e->foe->getID()
            << "'" << std::endl;
#endif

    const EncounterType& type = eInfo.type;

    if (type == ENCOUNTER_TYPE_CROSSING_FOLLOWER || type == ENCOUNTER_TYPE_CROSSING_LEADER
            || type == ENCOUNTER_TYPE_MERGING_FOLLOWER || type == ENCOUNTER_TYPE_MERGING_LEADER
            || type == ENCOUNTER_TYPE_FOLLOWING_FOLLOWER || type == ENCOUNTER_TYPE_FOLLOWING_LEADER) {
        determineTTCandDRAC(eInfo);
        determinePET(eInfo);
    } else if (type == ENCOUNTER_TYPE_BOTH_PASSED_CP) {
        determinePET(eInfo);
    } else if (type == ENCOUNTER_TYPE_COLLISION) {
        // TODO: handle collision
    } else if (type == ENCOUNTER_TYPE_EGO_PASSED_CP || type == ENCOUNTER_TYPE_FOE_PASSED_CP
        || type == ENCOUNTER_TYPE_NOCONFLICT_AHEAD) {
        // No conflict measures apply for these states, which correspond to intermediate times between
        // one vehicle leaving the conflict area and the arrival time for the other (difference corresponds to the PET)
    } else if (type == ENCOUNTER_TYPE_NOCONFLICT_AHEAD) {
        // No conflict measures apply for this state
    } else {
        WRITE_WARNING("Unknown or undetermined encounter type at computeSSMs().");
    }

#ifdef DEBUG_SSM
    std::cout << "computeSSMs() for encounter of vehicles '" << e->egoID << "' and '" << e->foeID <<"':\n"
                 << "  ttc=" << (eInfo.ttc==INVALID?"INVALID":toString(eInfo.ttc))
                 << ", drac=" << (eInfo.drac==INVALID?"INVALID":toString(eInfo.drac))
                 << ", pet=" << (eInfo.pet.second==INVALID?"INVALID":toString(eInfo.pet.second))
                 << std::endl;
#endif
}


void
MSDevice_SSM::determinePET(EncounterApproachInfo& eInfo) const {
    Encounter* e = eInfo.encounter;
    if (e->size() == 0) return;
    const EncounterType& type = eInfo.type;
    std::pair<double,double>& pet = eInfo.pet;

#ifdef DEBUG_SSM
    std::cout << SIMTIME << " determinePET() for encounter of vehicles '" << e->egoID << "' and '" << e->foeID <<"'"
                 << std::endl;
#endif

    if (type == ENCOUNTER_TYPE_FOLLOWING_FOLLOWER || type == ENCOUNTER_TYPE_FOLLOWING_LEADER) {
        // For a following situation, the corresponding PET-value is merely the time-headway.
        // TODO: Determining these can be done by comparison of memorized gaps with memorized covered distances
        //       Implementation is postponed. Tracing the time gaps (in contrast to crossing PET) corresponds to
        //       a vector of values not a single value.

        // pass

    } else if (type == ENCOUNTER_TYPE_BOTH_PASSED_CP) {

        EncounterType prevType = static_cast<EncounterType>(e->typeSpan.back());
        if (prevType == ENCOUNTER_TYPE_BOTH_PASSED_CP) {
#ifdef DEBUG_SSM
            std::cout << "PET for crossing encounter already calculated as " << e->PET.second
                    << std::endl;
#endif
            assert(e->PET.second != INVALID);
            return; // pet must have been calculated already
        }

        // this situation should have emerged from one of the following
        assert (prevType == ENCOUNTER_TYPE_CROSSING_FOLLOWER
                || prevType == ENCOUNTER_TYPE_CROSSING_LEADER
                || prevType == ENCOUNTER_TYPE_EGO_PASSED_CP
                || prevType == ENCOUNTER_TYPE_FOE_PASSED_CP);


#ifdef DEBUG_SSM
        std::cout << "e->egoDistsToConflict.back() = " << e->egoDistsToConflict.back()
                    << "\ne->egoConflictEntryTime = " << e->egoConflictEntryTime
                    << "\ne->egoConflictExitTime = " << e->egoConflictExitTime
                    << "\ne->foeDistsToConflict.back() = " << e->foeDistsToConflict.back()
                    << "\ne->foeConflictEntryTime = " << e->foeConflictEntryTime
                    << "\ne->foeConflictExitTime = " << e->foeConflictExitTime
                    << std::endl;
#endif

        assert(e->foeConflictEntryTime != INVALID);
        assert(e->egoConflictEntryTime != INVALID);

        // Both have passed the conflict entry but no collision -> one must have left the conflict region already
        // XXX: but being an encounter of type BOTH_PASSED_CP, we should have a '&&'
        assert(e->foeConflictExitTime != INVALID && e->egoConflictExitTime != INVALID);
        // both have left the conflict region already (can this really occur?)
        if (e->egoConflictEntryTime > e->foeConflictExitTime) {
            pet.first = e->egoConflictEntryTime;
            pet.second = e->egoConflictEntryTime - e->foeConflictExitTime;
        } else if (e->foeConflictEntryTime > e->egoConflictExitTime) {
            pet.first = e->foeConflictEntryTime;
            pet.second = e->foeConflictEntryTime - e->egoConflictExitTime;
        } else {

#ifdef DEBUG_SSM
            std::cout << "Unexpected branch in determinePET: Both passed conflict area in the same step."
                    << std::endl;
#endif
            pet.first = INVALID;
            pet.second = INVALID;
            assert (prevType != ENCOUNTER_TYPE_EGO_PASSED_CP
                    && prevType != ENCOUNTER_TYPE_FOE_PASSED_CP);
        }
#ifdef DEBUG_SSM
        std::cout << "Calculated PET = " << pet.second << " (at t=" << pet.first << ")"
                << std::endl;
#endif
    } else {
        // other cases (merging and pre-crossing situations) do not correspond to a PET calculation.
#ifdef DEBUG_SSM
        std::cout << "PET unappropriate for merging and pre-crossing situations. No calculation performed."
                << std::endl;
#endif
        return;
    }
}


void
MSDevice_SSM::determineTTCandDRAC(EncounterApproachInfo& eInfo) const {

    Encounter* e = eInfo.encounter;
    const EncounterType& type = eInfo.type;
    double& ttc = eInfo.ttc;
    double& drac = eInfo.drac;

#ifdef DEBUG_SSM
    std::cout << SIMTIME << " determineTTCandDRAC() for encounter of vehicles '" << e->egoID << "' and '" << e->foeID <<"' (type = " << eInfo.type << ")"
                 << std::endl;
#endif

    // Dependent on the actual encounter situation (eInfo.type) calculate the TTC.
    // For merging and crossing, different cases occur when a collision during the merging / crossing process is predicted.
    if (type == ENCOUNTER_TYPE_FOLLOWING_FOLLOWER) {
        double gap = eInfo.egoConflictEntryDist;
        ttc = computeTTC(gap, e->ego->getSpeed(), e->foe->getSpeed());
        drac = computeDRAC(gap, e->ego->getSpeed(), e->foe->getSpeed());
    } else if (type == ENCOUNTER_TYPE_FOLLOWING_LEADER) {
        double gap = eInfo.foeConflictEntryDist;
        ttc = computeTTC(gap, e->foe->getSpeed(), e->ego->getSpeed());
        drac = computeDRAC(gap, e->foe->getSpeed(), e->ego->getSpeed());
    } else if (type == ENCOUNTER_TYPE_MERGING_FOLLOWER || type == ENCOUNTER_TYPE_MERGING_LEADER) {
        // TODO: calculate more specifically whether a following situation in the merge conflict area
        //       is predicted when assuming constant speeds or whether a side collision is predicted.
        //       Currently, we ignore any conflict area before the actual merging point of the lanes.


        // linearly extrapolated arrival times at the conflict
        double egoEntryTime = e->ego->getSpeed()>0?eInfo.egoConflictEntryDist/e->ego->getSpeed():INVALID;
        double egoExitTime = e->ego->getSpeed()>0?eInfo.egoConflictExitDist/e->ego->getSpeed():INVALID;
        double foeEntryTime = e->foe->getSpeed()>0?eInfo.foeConflictEntryDist/e->foe->getSpeed():INVALID;
        double foeExitTime = e->foe->getSpeed()>0?eInfo.foeConflictExitDist/e->foe->getSpeed():INVALID;

#ifdef DEBUG_SSM
    std::cout << "   Conflict times with constant speed extrapolation for merging situation:\n   "
            << " egoEntryTime=" << egoEntryTime
            << ", egoExitTime=" << egoExitTime
            << ", foeEntryTime=" << foeEntryTime
            << ", foeExitTime=" << foeExitTime
            << std::endl;
#endif

        // based on that, we obtain
        if (egoEntryTime == INVALID || foeEntryTime == INVALID) {
            // at least one vehicle is stopped
            ttc = INVALID; drac = INVALID;
#ifdef DEBUG_SSM
            std::cout << "    No TTC and DRAC computed as one vehicle is stopped." << std::endl;
#endif
        } else {
            double leaderEntryTime = MIN2(egoEntryTime, foeEntryTime);
            double followerEntryTime = MAX2(egoEntryTime, foeEntryTime);
            double leaderExitTime = leaderEntryTime==egoEntryTime?egoExitTime:foeExitTime;
            double followerExitTime = leaderEntryTime==egoEntryTime?foeExitTime:egoExitTime;
            double leaderSpeed = leaderEntryTime==egoEntryTime?e->ego->getSpeed():e->foe->getSpeed();
            double followerSpeed = leaderEntryTime==egoEntryTime?e->foe->getSpeed():e->ego->getSpeed();
            double leaderConflictDist = leaderEntryTime==egoEntryTime?eInfo.egoConflictEntryDist:eInfo.foeConflictEntryDist;
            double followerConflictDist = leaderEntryTime==egoEntryTime?eInfo.foeConflictEntryDist:eInfo.egoConflictEntryDist;
            double leaderLength = leaderEntryTime==egoEntryTime?e->ego->getLength():e->foe->getLength();
            if (leaderExitTime >= followerEntryTime){
                // collision would occur at merge area (XXX: currently only the crossection corresponding to the target lane's begin)
                ttc = computeTTC(followerConflictDist, followerSpeed, 0.);
                // TODO: calculate more specific drac (no need to completely stop before conflict area)
                drac = computeDRAC(followerConflictDist, followerSpeed, 0.);

#ifdef DEBUG_SSM
                std::cout << "    Extrapolation predicts collision *at* merge point with TTC="<<ttc
                        << ", drac="<<drac<< std::endl;
#endif

            } else {
                // -> No collision at the merge area
                // Check if after merge a collision would occur if speeds are hold constant.
                double gapAfterMerge = followerConflictDist - leaderExitTime*followerSpeed;
                assert(gapAfterMerge>=0);

                // ttc as for following situation (no collision until leader merged)
                double ttcAfterMerge = computeTTC(gapAfterMerge, followerSpeed, leaderSpeed);
                ttc = ttcAfterMerge==INVALID ? INVALID : leaderExitTime + ttcAfterMerge;

                // Intitial gap. (May be negative only if the leader speed is higher than the follower speed, i.e., dv < 0)
                double g0 = followerConflictDist - leaderConflictDist - leaderLength;
                // Speed difference. (Must be positive if g0<0)
                double dv = leaderSpeed - followerSpeed;
                if (g0 < 0) {
                    assert(dv>0);
                    // no deceleration needed for dv>0 and gap after merge >= 0
                    drac = INVALID;
                } else {
                    // compute drac as for a following situation
                    drac = computeDRAC(g0, followerSpeed, leaderSpeed);
                }

#ifdef DEBUG_SSM
                if (ttc == INVALID) {
                    assert(dv>=0);
                    assert(drac==INVALID);
                    std::cout << "    Extrapolation does not predict any collision." << std::endl;
                } else {
                    std::cout << "    Extrapolation predicts collision *after* merge point with TTC="
                            <<(ttc==INVALID?"NA":toString(ttc))
                            << ", drac="<<(drac==INVALID?"NA":toString(drac))<< std::endl;
                }
#endif

            }
        }
    } else if (type == ENCOUNTER_TYPE_CROSSING_FOLLOWER) {
        if (eInfo.egoEstimatedConflictEntryTime <= eInfo.foeEstimatedConflictExitTime) {
            // follower's predicted arrival at the crossing area is earlier than the leader's predicted exit -> collision predicted
            double gap = eInfo.egoConflictEntryDist;
            drac = computeDRAC(gap, e->ego->getSpeed(), 0.);
            ttc = computeTTC(gap, e->ego->getSpeed(), 0.);
        } else {
            // encounter is expected to happen without collision
            ttc = INVALID;
            drac = INVALID;
        }
    } else if (type == ENCOUNTER_TYPE_CROSSING_LEADER) {
        if (eInfo.foeEstimatedConflictEntryTime <= eInfo.egoEstimatedConflictExitTime) {
            // follower's predicted arrival at the crossing area is earlier than the leader's predicted exit -> collision predicted
            double gap = eInfo.foeConflictEntryDist;
            drac = computeDRAC(gap, e->foe->getSpeed(), 0.);
            ttc = computeTTC(gap, e->foe->getSpeed(), 0.);
        } else {
            // encounter is expected to happen without collision
            ttc = INVALID;
            drac = INVALID;
        }
    } else {
#ifdef DEBUG_SSM
        WRITE_WARNING("Underspecified or unknown encounter type in MSDevice_SSM::determineTTCandDRAC()");
#endif
    }

#ifdef DEBUG_SSM
    std::cout << "ttc=" << (ttc==INVALID?"INVALID":toString(ttc)) << ", drac=" << (drac==INVALID?"INVALID":toString(drac))
                 << std::endl;
#endif
}


double
MSDevice_SSM::computeTTC(double gap, double followerSpeed, double leaderSpeed) const {
#ifdef DEBUG_SSM
    std::cout << "computeTTC() with gap="<<gap<<", followerSpeed="<<followerSpeed<<", leaderSpeed="<<leaderSpeed
                 << std::endl;
#endif
    if (gap <= 0.) return 0.; // collision already happend
    double dv = followerSpeed - leaderSpeed;
    if (dv <= 0.) return INVALID; // no collision

    return gap/dv;
}


double
MSDevice_SSM::computeDRAC(double gap, double followerSpeed, double leaderSpeed) const {
    // TODO: in merging or crossing situations, the DRAC may be lower than the one computed here for following situations
    // More specifically, the followers conflict time entry should be less than the leaders conflict exit time.
    // For merging conflicts, the minimum has to be taken from the two if a collsion at merge was predicted.
#ifdef DEBUG_SSM
    std::cout << "computeDRAC() with gap="<<gap<<", followerSpeed="<<followerSpeed<<", leaderSpeed="<<leaderSpeed
                 << std::endl;
#endif
    if (gap <= 0.) return INVALID; // collision already happend
    double dv = followerSpeed - leaderSpeed;
    if (dv <= 0.) return INVALID; // no collision
    assert(followerSpeed > 0.);
    double timeHeadway = gap/followerSpeed;
    return dv/timeHeadway;
}

void
MSDevice_SSM::checkConflictEntryAndExit(EncounterApproachInfo& eInfo) {
    // determine exact entry and exit times
    Encounter* e = eInfo.encounter;

#ifdef DEBUG_SSM
    std::cout << SIMTIME << " checkConflictEntryAndExit() for encounter of vehicles '" << e->egoID << "' and '" << e->foeID << "'" << std::endl;
#endif

    // Distances to conflict area boundaries in previous step
    double prevEgoConflictEntryDist = eInfo.egoConflictEntryDist + e->ego->getLastStepDist();
    double prevFoeConflictEntryDist = eInfo.foeConflictEntryDist + e->foe->getLastStepDist();
    double prevEgoConflictExitDist = prevEgoConflictEntryDist + eInfo.egoConflictAreaLength + e->ego->getLength();
    double prevFoeConflictExitDist = prevFoeConflictEntryDist + eInfo.foeConflictAreaLength + e->foe->getLength();

    if (e->timeSpan.size() == 0) {
        // Encounter has just been created, no data points yet.
        // If a vehicle was already beyond conflict area boundary in last step, we set the passing time to the previous time step
        prevEgoConflictEntryDist = MAX2(prevEgoConflictEntryDist, 0.);
        prevFoeConflictEntryDist = MAX2(prevFoeConflictEntryDist, 0.);
        prevEgoConflictExitDist = MAX2(prevEgoConflictExitDist, 0.);
        prevFoeConflictExitDist = MAX2(prevFoeConflictExitDist, 0.);
    }

    if (e->egoConflictEntryTime == INVALID && eInfo.egoConflictEntryDist < 0 && prevEgoConflictEntryDist >= 0){
        // ego must have entered the conflict in the last step. Determine exact entry time
        e->egoConflictEntryTime = SIMTIME - TS + MSCFModel::passingTime(-e->egoDistsToConflict.back(), 0., -eInfo.egoConflictEntryDist, e->ego->getPreviousSpeed(), e->ego->getSpeed());
#ifdef DEBUG_SSM
        std::cout << "    ego entered conflict area at t=" << e->egoConflictEntryTime << std::endl;
#endif
    }
    if (e->foeConflictEntryTime == INVALID && eInfo.foeConflictEntryDist < 0. && prevFoeConflictEntryDist >= 0) {
        // foe must have entered the conflict in the last step. Determine exact entry time
        assert(e->foeConflictEntryTime == INVALID);
        e->foeConflictEntryTime = SIMTIME - TS + MSCFModel::passingTime(-e->foeDistsToConflict.back(), 0., -eInfo.foeConflictEntryDist, e->foe->getPreviousSpeed(), e->foe->getSpeed());
#ifdef DEBUG_SSM
        std::cout << "    foe entered conflict area at t=" << e->foeConflictEntryTime << std::endl;
#endif
    }
    if (e->egoConflictExitTime == INVALID && eInfo.egoConflictExitDist < 0 && prevEgoConflictExitDist >= 0){
        // ego must have left the conflict area in the last step. Determine exact exit time
        assert(e->egoConflictExitTime == INVALID);
        e->egoConflictExitTime = SIMTIME - TS + MSCFModel::passingTime(-prevEgoConflictExitDist, 0., -eInfo.egoConflictExitDist, e->ego->getPreviousSpeed(), e->ego->getSpeed());
#ifdef DEBUG_SSM
        std::cout << "    ego left conflict area at t=" << e->egoConflictExitTime << std::endl;
#endif
    }
    if (e->foeConflictExitTime == INVALID && eInfo.foeConflictExitDist < 0 && prevFoeConflictExitDist >= 0){
        // foe must have left the conflict area in the last step. Determine exact exit time
        assert(e->foeConflictExitTime == INVALID);
        e->foeConflictExitTime = SIMTIME - TS + MSCFModel::passingTime(-prevFoeConflictExitDist, 0., -eInfo.foeConflictExitDist, e->foe->getPreviousSpeed(), e->foe->getSpeed());
#ifdef DEBUG_SSM
        std::cout << "    foe left conflict area at t=" << e->foeConflictExitTime << std::endl;
#endif
    }
}


void
MSDevice_SSM::updatePassedEncounter(Encounter* e, FoeInfo* foeInfo, EncounterApproachInfo& eInfo) {

#ifdef DEBUG_SSM
    std::cout << SIMTIME << " updatePassedEncounter() for vehicles '"
            << e->egoID << "' and '" << e->foeID << "'"
            << std::endl;
#endif

    if(foeInfo == 0) {
        // the foe is out of the device's range, proceed counting down the remaining extra time to trace
        e->countDownExtraTime(TS);
#ifdef DEBUG_SSM
    std::cout << "    Foe is out of range. Counting down extra time."
            << " Remaining seconds before closing encounter: " << e->getRemainingExtraTime()
            << std::endl;
#endif

    } else {
        // reset the remaining extra time (foe could have re-entered the device range after beginning extra time countdown already)
        e->resetExtraTime(myExtraTime);
    }

    // Check, whether this was really a potential conflict at some time:
    // Search through typeSpan for a type other than no conflict
    EncounterType lastPotentialConflictType = e->typeSpan.size() > 0 ? static_cast<EncounterType>(e->typeSpan.back()) : ENCOUNTER_TYPE_NOCONFLICT_AHEAD;

    if (lastPotentialConflictType == ENCOUNTER_TYPE_NOCONFLICT_AHEAD) {
        // This encounter was no conflict in the last step -> remains so
#ifdef DEBUG_SSM
        std::cout << "    This encounter wasn't classified as a potential conflict lately." << std::endl;
#endif
        if (foeInfo == 0) {
            // Encounter was either never a potential conflict and foe is out of range
            // or the foe has left the network
            // -> no use in further tracing this encounter
            e->closingRequested = true;
#ifdef DEBUG_SSM
            std::cout << "    Closing encounter." << std::endl;
#endif
            eInfo.type = ENCOUNTER_TYPE_NOCONFLICT_AHEAD;
        }
    } else if(lastPotentialConflictType == ENCOUNTER_TYPE_FOLLOWING_FOLLOWER
            || lastPotentialConflictType == ENCOUNTER_TYPE_FOLLOWING_LEADER) {
        // if a following situation leads to a no-conflict situation this encounter switches no-conflict, since no further computations (PET) are needed.
        // XXX: Resetting to NOCONFLICT_AHEAD may lead to an early abort of conflict logging, since the next call will close the encounter
        //      Consider using the commented TYPE instead
        eInfo.type = ENCOUNTER_TYPE_NOCONFLICT_AHEAD;
//        eInfo.type = ENCOUNTER_TYPE_FOLLOWING_PASSED;
#ifdef DEBUG_SSM
        std::cout << "    Encounter was previously classified as a follow/lead situation." << std::endl;
#endif
    } else if(lastPotentialConflictType == ENCOUNTER_TYPE_MERGING_FOLLOWER
            || lastPotentialConflictType == ENCOUNTER_TYPE_MERGING_LEADER) {
        // if a merging situation leads to a no-conflict situation the leader was either removed from the net (we disregard special treatment)
        // or route- or lane-changes removed the conflict.
        // XXX: Resetting to NOCONFLICT_AHEAD may lead to an early abort of conflict logging, since the next call will close the encounter
        //      Consider using the commented TYPE instead
        eInfo.type = ENCOUNTER_TYPE_NOCONFLICT_AHEAD;
//        eInfo.type = ENCOUNTER_TYPE_MERGING_PASSED;
#ifdef DEBUG_SSM
        std::cout << "    Encounter was previously classified as a merging situation." << std::endl;
#endif
    } if(lastPotentialConflictType == ENCOUNTER_TYPE_CROSSING_FOLLOWER
            || lastPotentialConflictType == ENCOUNTER_TYPE_CROSSING_LEADER
            || lastPotentialConflictType == ENCOUNTER_TYPE_EGO_PASSED_CP
            || lastPotentialConflictType == ENCOUNTER_TYPE_FOE_PASSED_CP
            || lastPotentialConflictType == ENCOUNTER_TYPE_BOTH_PASSED_CP
            || lastPotentialConflictType == ENCOUNTER_TYPE_COLLISION) {
        // Encounter has been a crossing situation.

#ifdef DEBUG_SSM
        std::cout << "    Encounter was previously classified as a crossing situation of type " << lastPotentialConflictType << "." << std::endl;
#endif
        // For passed encounters, the xxxConflictAreaLength variables are not determined before -> we use the stored values.
        if (eInfo.egoConflictAreaLength == INVALID) eInfo.egoConflictAreaLength = e->foeConflictLane->getWidth();
        if (eInfo.foeConflictAreaLength == INVALID) eInfo.foeConflictAreaLength = e->egoConflictLane->getWidth();

        // TODO: foe might not exist anymore... would result in an invalid pointer access below
        eInfo.egoConflictEntryDist = e->egoDistsToConflict.back() - e->ego->getLastStepDist();
        eInfo.egoConflictExitDist = eInfo.egoConflictEntryDist + eInfo.egoConflictAreaLength + e->ego->getLength();
        eInfo.foeConflictEntryDist = e->foeDistsToConflict.back() - e->foe->getLastStepDist();
        eInfo.foeConflictExitDist = eInfo.foeConflictEntryDist + eInfo.foeConflictAreaLength + e->foe->getLength();

#ifdef DEBUG_SSM
        std::cout << "    egoConflictEntryDist = " << eInfo.egoConflictEntryDist
                << ", egoConflictExitDist = " << eInfo.egoConflictExitDist
                << "\n    foeConflictEntryDist = " << eInfo.foeConflictEntryDist
                << ", foeConflictExitDist = " << eInfo.foeConflictExitDist
                << std::endl;
#endif

        // Determine actual encounter type
        bool egoLeftConflict =  eInfo.egoConflictExitDist < 0.;
        bool foeLeftConflict =  eInfo.foeConflictExitDist < 0.;

        if((!egoLeftConflict) && !foeLeftConflict) {
            assert(lastPotentialConflictType == ENCOUNTER_TYPE_CROSSING_FOLLOWER
                    || lastPotentialConflictType == ENCOUNTER_TYPE_CROSSING_LEADER);
            // XXX: do we need to recompute the follow/lead order, here?
            eInfo.type = lastPotentialConflictType;
        } else if (egoLeftConflict) {
            if (foeLeftConflict) {
                eInfo.type = ENCOUNTER_TYPE_BOTH_PASSED_CP;
            } else {
                eInfo.type = ENCOUNTER_TYPE_EGO_PASSED_CP;
            }
        } else {
            if (foeLeftConflict) {
                eInfo.type = ENCOUNTER_TYPE_FOE_PASSED_CP;
            } else {
                // check for collision
                bool egoEnteredConflict =  eInfo.egoConflictEntryDist < 0.;
                bool foeEnteredConflict =  eInfo.foeConflictEntryDist < 0.;
                if (foeEnteredConflict && egoEnteredConflict) {
                    eInfo.type = ENCOUNTER_TYPE_COLLISION;
                } else {
                    // It should not occur that both leave the conflict area in the same time step
                    // (given that there was no collision before)
                    assert(false);
                }
            }
        }


#ifdef DEBUG_SSM
        std::cout << "    Updated classification: " << eInfo.type << std::endl;
#endif
    }
}


MSDevice_SSM::EncounterType
MSDevice_SSM::classifyEncounter(const FoeInfo* foeInfo, EncounterApproachInfo& eInfo)  const {
    if (foeInfo == 0) {
        // foeInfo == 0 signalizes, that no corresponding foe info was returned by findSurroundingVehicles(),
        // i.e. the foe is actually out of range (This may also mean that it has left the network)
        return ENCOUNTER_TYPE_NOCONFLICT_AHEAD;
    }
    const Encounter* e = eInfo.encounter;

    // Ego's current Lane
    const MSLane* egoLane = e->ego->getLane();
    // Foe's current Lane
    const MSLane* foeLane = e->foe->getLane();

    // Ego's conflict lane is memorized in foeInfo
    const MSLane* egoConflictLane = foeInfo->egoConflictLane;
    double egoDistToConflictLane = foeInfo->egoDistToConflictLane;
    // Find conflicting lane and the distance to its entry link for the foe
    double foeDistToConflictLane;
    const MSLane* foeConflictLane = findFoeConflictLane(e->foe, foeInfo->egoConflictLane, foeDistToConflictLane);

#ifdef DEBUG_SSM
    std::cout << "egoConflictLane: '" << (egoConflictLane == 0 ? "NULL" : egoConflictLane->getID()) << "'\n"
              << "foeConflictLane: '" << (foeConflictLane == 0 ? "NULL" : foeConflictLane->getID()) << "'"
              << "\nEgo's distance to conflict lane: " << egoDistToConflictLane
              << "\nFoe's distance to conflict lane: " << foeDistToConflictLane
              << std::endl;
#endif

    // Treat different cases for foeConflictLane and egoConflictLane (internal or non-internal / equal to egoLane or to foeLane),
    // and thereby determine encounterType and the ego/foeEncounterDistance.
    // The encounter distance has a different meaning for different types of encounters:
    // 1) For rear-end conflicts (lead/follow situations) the follower's encounter distance is the distance to the actual back position of the leader. The leaders's distance is undefined.
    // 2) For merging encounters the encounter distance is the distance until the begin of the common target edge/lane.
    //    (XXX: Perhaps this should be adjusted to include the entry point to the region where a simultaneous occupancy of
    //          both merging lanes could imply a collision)
    // 3) For crossing encounters the encounter distances is the distance until the entry point to the conflicting lane.

    EncounterType type;

    if (foeConflictLane == 0) {
        // foe vehicle is not on course towards the ego's route (see findFoeConflictLane)
        type = ENCOUNTER_TYPE_NOCONFLICT_AHEAD;
#ifdef DEBUG_SSM
        std::cout << "-> Encounter type: No conflict." << std::endl;
#endif
    } else if (!egoConflictLane->isInternal()) {
        // The conflict lane is non-internal, therefore we either have no potential conflict or a lead/follow situation (i.e., no crossing or merging)
        if (egoConflictLane == egoLane) {
            // The conflict point is on the ego's current lane.
            if (foeLane == egoLane) {
                // Foe is on the same non-internal lane
                if (e->ego->getPositionOnLane() > e->foe->getPositionOnLane()) {
                    type = ENCOUNTER_TYPE_FOLLOWING_LEADER;
                    eInfo.foeConflictEntryDist = e->ego->getBackPositionOnLane() - e->foe->getBackPositionOnLane();
                } else {
                    type = ENCOUNTER_TYPE_FOLLOWING_FOLLOWER;
                    eInfo.egoConflictEntryDist = e->foe->getBackPositionOnLane() - e->ego->getBackPositionOnLane();
                }
#ifdef DEBUG_SSM
                std::cout << "-> Encounter type: Lead/follow-situation on non-internal lane '" << egoLane->getID() << "'" << std::endl;
#endif
            } else if (&(foeLane->getEdge()) == &(egoLane->getEdge())) {
                // Foe is on the same non-internal edge but not on the same lane. Treat this as no conflict for now
                // XXX: this disregards conflicts for vehicles on adjacent lanes
                type = ENCOUNTER_TYPE_NOCONFLICT_AHEAD;
#ifdef DEBUG_SSM
                std::cout << "-> Encounter type: No conflict (adjacent lanes)." << std::endl;
#endif
            } else {
                // Foe must be on a route leading into the ego's lane
                type = ENCOUNTER_TYPE_FOLLOWING_LEADER;
                eInfo.foeConflictEntryDist = foeDistToConflictLane + e->ego->getBackPositionOnLane();
#ifdef DEBUG_SSM
    std::cout << "-> Encounter type: Ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' leads foe '"
            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
            << " (gap = " << eInfo.foeConflictEntryDist << ")"
            << std::endl;
#endif
                assert(foeConflictLane == egoLane);
            }
        } else {
            // The egoConflictLane is a non-internal lane which is not the ego's current lane. Thus it must lie ahead of the ego vehicle and equal the foe's current lane.
            // see findSurroundingVehicles() (otherwise the foe would have had to enter the ego's route along a junction and the corresponding
            // conflict lane would be internal)
            type = ENCOUNTER_TYPE_FOLLOWING_FOLLOWER;
            eInfo.egoConflictEntryDist = egoDistToConflictLane + e->foe->getBackPositionOnLane();
#ifdef DEBUG_SSM
    std::cout << "-> Encounter type: Ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' follows foe '"
            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
            << " (gap = " << eInfo.egoConflictEntryDist << ")"
            << std::endl;
#endif
            assert(foeLane == egoConflictLane);
            assert(foeDistToConflictLane <= 0);
        }
    } else {
        // egoConflictLane is internal, i.e., lies on a junction. Besides the lead/follow situation (which may stretch over different lanes of a connection),
        // merging or crossing of the conflict lanes is possible.
        assert(foeConflictLane->isInternal());
        MSLink* egoEntryLink = egoConflictLane->getEntryLink();
        MSLink* foeEntryLink = foeConflictLane->getEntryLink();
        if (&(egoEntryLink->getViaLane()->getEdge()) == &(foeEntryLink->getViaLane()->getEdge())) {
            if (egoEntryLink != foeEntryLink) {
                // XXX: this disregards conflicts for vehicles on adjacent lanes
                type = ENCOUNTER_TYPE_NOCONFLICT_AHEAD;
#ifdef DEBUG_SSM
                std::cout << "-> Encounter type: No conflict (adjacent lanes)." << std::endl;
#endif
            } else {
                // Lead / follow situation on connection
                if (egoLane == egoConflictLane && foeLane != foeConflictLane) {
                    // ego on junction, foe not yet
                    type = ENCOUNTER_TYPE_FOLLOWING_LEADER;
                    eInfo.foeConflictEntryDist = foeDistToConflictLane + e->ego->getBackPositionOnLane();
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: Ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' leads foe '"
                            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
                            << " (gap = " << eInfo.foeConflictEntryDist << ")"
                            << std::endl;
#endif
                } else if (egoLane != egoConflictLane && foeLane == foeConflictLane) {
                    // foe on junction, ego not yet
                    type = ENCOUNTER_TYPE_FOLLOWING_FOLLOWER;
                    eInfo.egoConflictEntryDist = egoDistToConflictLane + e->foe->getBackPositionOnLane();
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: Ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' follows foe '"
                            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
                            << " (gap = " << eInfo.egoConflictEntryDist << ")"
                            << std::endl;
#endif
                } else {
                    // Both must be already on the junction in a lead / follow situation on a connection
                    // (since they approach via the same link, findSurroundingVehicles() would have determined a
                    // different conflictLane if both are not on the junction)
                    assert(egoLane == egoConflictLane);
                    assert(foeLane == foeConflictLane);
                    if (egoLane == foeLane){
                        if (e->ego->getPositionOnLane() > e->foe->getPositionOnLane()) {
                            type = ENCOUNTER_TYPE_FOLLOWING_LEADER;
                        } else {
                            type = ENCOUNTER_TYPE_FOLLOWING_FOLLOWER;
                        }
                    } else {
                        MSLane* lane = egoEntryLink->getViaLane();
                        while (true) {
                            if (egoLane == lane) {
                                type = ENCOUNTER_TYPE_FOLLOWING_FOLLOWER;
                                break;
                            } else if (foeLane == lane) {
                                type = ENCOUNTER_TYPE_FOLLOWING_LEADER;
                            }
                            lane = lane->getLinkCont()[0]->getViaLane();
                            assert(lane != 0);
                        }
                    }
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: Lead/follow-situation on connection from '" << egoEntryLink->getLaneBefore()->getID()
                              << "' to '" << egoEntryLink->getLane()->getID() << "'" << std::endl;
#endif
                }
            }
        } else {
            // Entry links to junctions lead to different internal edges.
            // There are three possibilities, either the edges cross, merge or have no conflict
            const std::vector<MSLink*>& egoFoeLinks = egoEntryLink->getFoeLinks();
            const std::vector<MSLink*>& foeFoeLinks = foeEntryLink->getFoeLinks();
            // Determine whether ego and foe links are foes
            bool crossOrMerge = (find(egoFoeLinks.begin(), egoFoeLinks.end(), foeEntryLink) != egoFoeLinks.end()
                                 || find(foeFoeLinks.begin(), foeFoeLinks.end(), egoEntryLink) != foeFoeLinks.end());
            if (!crossOrMerge) {
                if (&(foeEntryLink->getLane()->getEdge()) == &(egoEntryLink->getLane()->getEdge())) {
                    // XXX: the situation of merging into adjacent lanes is disregarded for now
                    type = ENCOUNTER_TYPE_NOCONFLICT_AHEAD;
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: No conflict (adjacent lanes)." << std::endl;
#endif
                } else {
                    type = ENCOUNTER_TYPE_NOCONFLICT_AHEAD;
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: No conflict." << std::endl;
#endif
                }
            } else if (foeEntryLink->getLane() == egoEntryLink->getLane()) {
                type = ENCOUNTER_TYPE_MERGING;
                eInfo.egoConflictEntryDist = egoDistToConflictLane + egoEntryLink->getInternalLengthsAfter();
                eInfo.foeConflictEntryDist = foeDistToConflictLane + foeEntryLink->getInternalLengthsAfter();
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: Merging situation of ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' and foe '"
                            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
                            << "\nDistances to merge-point: ego: " << eInfo.egoConflictEntryDist << ", foe: " << eInfo.foeConflictEntryDist
                            << std::endl;
#endif
            } else {
                type = ENCOUNTER_TYPE_CROSSING;

                assert(egoConflictLane->isInternal());
                assert(foeConflictLane->getEdge().getToJunction() == egoConflictLane->getEdge().getToJunction());

                // If the conflict lanes are internal, they may not correspond to the
                // actually crossing parts of the corresponding connections.
                // Adjust the conflict lanes accordingly.
                // set back both to the first parts of the corresponding connections
                double offset = 0.;
                egoConflictLane = egoConflictLane->getFirstInternalInConnection(offset);
                egoDistToConflictLane -= offset;
                foeConflictLane = foeConflictLane->getFirstInternalInConnection(offset);
                foeDistToConflictLane -= offset;
                // find the distances to the conflict from the junction entry for both vehicles
                // for the ego
                // TODO: determine distance to foe based on its lateral position and width instead of distance to lane
                double distToConflictFromJunctionEntry = INVALID;
                while (foeConflictLane != 0 && foeConflictLane->isInternal()) {
                    distToConflictFromJunctionEntry = egoEntryLink->getLengthsBeforeCrossing(foeConflictLane);
                    if (distToConflictFromJunctionEntry != INVALID) break; // found correct foeConflictLane
                    foeConflictLane = foeConflictLane->getCanonicalSuccessorLane();
                    assert(foeConflictLane != 0 && foeConflictLane->isInternal()); // this loop should be ended by the break! Otherwise the lanes do not cross, which should be the case here.
                }
                assert(distToConflictFromJunctionEntry != INVALID);
                eInfo.egoConflictEntryDist = egoDistToConflictLane + distToConflictFromJunctionEntry;
                // for the foe
                // TODO: determine distance to ego based on its lateral position and width instead of distance to lane
                distToConflictFromJunctionEntry = -INVALID;
                while (egoConflictLane != 0 && egoConflictLane->isInternal()) {
                    distToConflictFromJunctionEntry = foeEntryLink->getLengthsBeforeCrossing(egoConflictLane);
                    if (distToConflictFromJunctionEntry != INVALID) break; // found correct foeConflictLane
                    egoConflictLane = egoConflictLane->getCanonicalSuccessorLane();
                    assert(egoConflictLane != 0 && egoConflictLane->isInternal()); // this loop should be ended by the break! Otherwise the lanes do not cross, which should be the case here.
                }
                assert(distToConflictFromJunctionEntry != INVALID);
                eInfo.foeConflictEntryDist = foeDistToConflictLane + distToConflictFromJunctionEntry;

                // TODO: This should more precisely be calculated in reference to the width of the vehicles, their lateral positions and the angle of the crossing. This may also imply different values for ego and foe.
                eInfo.egoConflictAreaLength = foeConflictLane->getWidth();
                eInfo.foeConflictAreaLength = egoConflictLane->getWidth();
#ifdef DEBUG_SSM
                std::cout << "real egoConflictLane: '" << (egoConflictLane == 0 ? "NULL" : egoConflictLane->getID()) << "'\n"
                        << "real foeConflictLane: '" << (foeConflictLane == 0 ? "NULL" : foeConflictLane->getID()) << "'\n"
                        << "-> Encounter type: Crossing situation of ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' and foe '"
                        << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
                        << "\nDistances to crossing-point: ego: " << eInfo.egoConflictEntryDist << ", foe: " << eInfo.foeConflictEntryDist
                        << std::endl;
#endif
            }
        }
    }
    // remember the conflict lane for the ego vehicle
    eInfo.egoConflictLane = egoConflictLane;
    // remember the conflict lane for the foe vehicle
    eInfo.foeConflictLane = foeConflictLane;
    return type;
}


const MSLane*
MSDevice_SSM::findFoeConflictLane(const MSVehicle* foe, const MSLane* egoConflictLane, double& distToConflictLane) const {

#ifdef DEBUG_SSM
    std::cout << SIMTIME << " findFoeConflictLane() for foe '"
              << foe->getID() << "' on lane '" << foe->getLane()->getID()
              << "' (with egoConflictLane="<<(egoConflictLane==0?"NULL":egoConflictLane->getID())
              << ")\nfoeBestLanes: " << toString(foe->getBestLanesContinuation())
              << std::endl;
#endif
    MSLane* foeLane = foe->getLane();
    std::vector<MSLane*>::const_iterator laneIter = foe->getBestLanesContinuation().begin();
    std::vector<MSLane*>::const_iterator foeBestLanesEnd = foe->getBestLanesContinuation().end();
    assert(foeLane->isInternal() || *laneIter == foeLane);
    distToConflictLane = -foe->getPositionOnLane();

    // Potential conflict lies on junction if egoConflictLane is internal
    const MSJunction* conflictJunction = egoConflictLane->isInternal() ? egoConflictLane->getEdge().getToJunction() : 0;
#ifdef DEBUG_SSM
    if (conflictJunction != 0) {
        std::cout << "Potential conflict on junction '" << conflictJunction->getID()
                  << std::endl;
    }
#endif
    if (foeLane->isInternal() && foeLane->getEdge().getToJunction() == conflictJunction) {
        // foe is already on the conflict junction
        return foeLane;
    }

    // Foe is not on the conflict junction

    // Leading internal lanes in bestlanes are resembled as a single NULL-pointer skip them
    if (*laneIter==0) {
        while(foeLane != 0 && foeLane->isInternal()){
            distToConflictLane += foeLane->getLength();
            foeLane = foeLane->getLinkCont()[0]->getViaLane();
        }
        ++laneIter;
        assert(laneIter==foeBestLanesEnd || *laneIter!=0);
    }

    // Look for the junction downstream along foeBestLanes
    while (laneIter != foeBestLanesEnd && distToConflictLane <= myRange) {
        // Eventual internal lanes were skipped
        assert(*laneIter == foeLane || foeLane == 0);
        foeLane = *laneIter;
        assert(!foeLane->isInternal());
        if (&foeLane->getEdge() == &egoConflictLane->getEdge()) {
#ifdef DEBUG_SSM
            std::cout << "Found conflict lane for foe: '" << foeLane->getID() << "'" << std::endl;
#endif
            // found the potential conflict edge along foeBestLanes
            return foeLane;
        }
        // No conflict on foeLane
        distToConflictLane += foeLane->getLength();

        // set laneIter to next non internal lane along foeBestLanes
        ++laneIter;
        if (laneIter == foeBestLanesEnd) {
            return 0;
        }
        MSLane* nextNonInternalLane = *laneIter;
        MSLink* link = foeLane->getLinkTo(nextNonInternalLane);
        // Set foeLane to first internal lane on the next junction
        foeLane = link->getViaLane();
        assert(foeLane->isInternal());
        if (foeLane->getEdge().getToJunction() == conflictJunction) {
#ifdef DEBUG_SSM
            std::cout << "Found conflict lane for foe: '" << foeLane->getID() << "'" << std::endl;
#endif
            // found egoConflictLane, resp. the conflict junction, along foeBestLanes
            return foeLane;
        }
        // No conflict on junction
        distToConflictLane += link->getInternalLengthsAfter();
        foeLane = nextNonInternalLane;
    }
    // Didn't find conflicting lane on foeBestLanes within range.
    return 0;
}

void
MSDevice_SSM::flushConflicts(bool flushAll) {
#ifdef DEBUG_SSM
    std::cout << "\n" << SIMTIME << " Device '" << getID() << "' flushConflicts()" << std::endl;
#endif
    double t = SIMTIME;
    while (!myPastConflicts.empty()) {
        if (flushAll || myPastConflicts.top()->begin <= t-myMaxEncounterLength) {
            writeOutConflict(myPastConflicts.top());
            delete myPastConflicts.top();
            myPastConflicts.pop();
        } else {
            break;
        }
    }
}


void
MSDevice_SSM::writeOutConflict(Encounter* e) {
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " writeOutConflict() of vehicles '"
            << e->egoID << "' and '" << e->foeID
            << "'" << std::endl;
#endif

    myOutputFile->openTag("conflict");
    myOutputFile->writeAttr("begin", e->begin).writeAttr("end", e->end);
    myOutputFile->writeAttr("ego", e->egoID).writeAttr("foe", e->foeID);

    if (mySaveTrajectories) {
        myOutputFile->openTag("timeSpan").writeAttr("values", e->timeSpan).closeTag();
        myOutputFile->openTag("typeSpan").writeAttr("values", e->typeSpan).closeTag();

        myOutputFile->openTag("egoPosition").writeAttr("values", toString(e->egoTrajectory.x)).closeTag();
        myOutputFile->openTag("egoVelocity").writeAttr("values", toString(e->egoTrajectory.v)).closeTag();

        myOutputFile->openTag("foePosition").writeAttr("values", toString(e->foeTrajectory.x)).closeTag();
        myOutputFile->openTag("foeVelocity").writeAttr("values", toString(e->foeTrajectory.v)).closeTag();
    }

    if (myComputeTTC) {
        if (mySaveTrajectories) {
            myOutputFile->openTag("TTCSpan").writeAttr("values", makeStringWithNAs(e->TTCspan, INVALID)).closeTag();
        }
        std::string s = (e->minTTC.first==INVALID)?("NA,NA"):(""+toString(e->minTTC.first)+","+toString(e->minTTC.second));
        myOutputFile->openTag("minTTC").writeAttr("value", s).closeTag();
    }
    if (myComputeDRAC) {
        if (mySaveTrajectories) {
            myOutputFile->openTag("DRACSpan").writeAttr("values", makeStringWithNAs(e->DRACspan,INVALID)).closeTag();
        }
        std::string s = (e->maxDRAC.first==INVALID)?("NA,NA"):(""+toString(e->maxDRAC.first)+","+toString(e->maxDRAC.second));
        myOutputFile->openTag("maxDRAC").writeAttr("value", s).closeTag();
    }
    if (myComputePET) {
        std::string s = (e->PET.first==INVALID)?("NA,NA"):(""+toString(e->PET.first)+","+toString(e->PET.second));
        myOutputFile->openTag("minPET").writeAttr("value", s).closeTag();
    }
    myOutputFile->closeTag();
}

std::string
MSDevice_SSM::makeStringWithNAs(std::vector<double> v, double NA, std::string sep) {
    std::string res="";
    for (std::vector<double>::const_iterator i = v.begin(); i!=v.end(); ++i){
        res += (i==v.begin()?"":sep)+(*i==NA?"NA":toString(*i));
    }
    return res;
}


// ---------------------------------------------------------------------------
// MSDevice_SSM-methods
// ---------------------------------------------------------------------------
MSDevice_SSM::MSDevice_SSM(SUMOVehicle& holder, const std::string& id, std::string outputFilename, std::map<std::string, double> thresholds,
        bool trajectories, double maxEncounterLength, double range, double extraTime) :
    MSDevice(holder, id),
    myThresholds(thresholds),
    mySaveTrajectories(trajectories),
    myMaxEncounterLength(maxEncounterLength),
    myRange(range),
    myExtraTime(extraTime)
{
    // Take care! Holder is currently being constructed. Cast occurs before completion.
    myHolderMS = static_cast<MSVehicle*>(&holder);

    myComputeTTC = myThresholds.find("TTC") != myThresholds.end();
    myComputeDRAC = myThresholds.find("DRAC") != myThresholds.end();
    myComputePET = myThresholds.find("PET") != myThresholds.end();

    maxTrajectorySize = (int)std::ceil(myMaxEncounterLength/TS)+1;
    myActiveEncounters = EncounterVector();
    myPastConflicts = EncounterQueue();

    // XXX: Who deletes the OutputDevice?
    myOutputFile = &OutputDevice::getDevice(outputFilename);
//    TODO: make xsd, include header
//    myOutputFile.writeXMLHeader("SSMLog", "SSMLog.xsd");
    myOutputFile->openTag("SSMLog");

    // register at static instance container
    instances->insert(this);

#ifdef DEBUG_SSM
    std::vector<std::string> measures;
    std::vector<double> threshVals;
    for (std::map<std::string,double>::const_iterator i=myThresholds.begin(); i!=myThresholds.end();++i) {
        measures.push_back(i->first);
        threshVals.push_back(i->second);
    }
    std::cout << "Initialized ssm device '" << id << "' with "
            << "myMeasures=" << joinToString(measures, " ")
            << ", myThresholds=" << joinToString(threshVals, " ")
            << ", mySaveTrajectories=" << mySaveTrajectories << ", myMaxEncounterLength=" << myMaxEncounterLength
            << ", myRange=" << myRange << ", output file=" << outputFilename << ", extra time=" << myExtraTime << "\n";
#endif
}


/// @brief Destructor.
MSDevice_SSM::~MSDevice_SSM() {
    // XXX: Who deletes this device?
    // unregister from static instance container
    instances->erase((MSDevice*) this);
    resetEncounters();
    flushConflicts(true);
    myOutputFile->closeTag();
}


bool
MSDevice_SSM::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
#ifdef DEBUG_SSM_NOTIFICATIONS
    std::cout << "device '" << getID() << "' notifyEnter: reason=" << reason << " currentEdge=" << veh.getLane()->getEdge().getID() << "\n";
#else
    UNUSED_PARAMETER(veh);
    UNUSED_PARAMETER(reason);
#endif
    return true; // keep the device
}

bool
MSDevice_SSM::notifyLeave(SUMOVehicle& veh, double /*lastPos*/,
                          MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
#ifdef DEBUG_SSM_NOTIFICATIONS
    std::cout << "device '" << getID() << "' notifyLeave: reason=" << reason << " currentEdge=" << veh.getLane()->getEdge().getID() << "\n";
#else
    UNUSED_PARAMETER(veh);
    UNUSED_PARAMETER(reason);
#endif
    return true; // keep the device
}

bool
MSDevice_SSM::notifyMove(SUMOVehicle& /* veh */, double /* oldPos */,
                         double /* newPos */, double newSpeed) {
#ifdef DEBUG_SSM_NOTIFICATIONS
    std::cout << "device '" << getID() << "' notifyMove: newSpeed=" << newSpeed << "\n";
#else
    UNUSED_PARAMETER(newSpeed);
#endif
    return true; // keep the device
}


void
MSDevice_SSM::findSurroundingVehicles(const MSVehicle& veh, double range, FoeInfoMap& foeCollector) {
#ifdef DEBUG_SSM_SURROUNDING
    std::cout << SIMTIME << " Looking for surrounding vehicles for ego vehicle '" << veh.getID()
              << "' on edge '" << veh.getLane()->getEdge().getID()
              << "'."
              << "\nVehicle's best lanes = " << toString(veh.getBestLanesContinuation())
              << std::endl;
#endif

    if (!veh.isOnRoad()) {
        return;
    }

    // The requesting vehicle's current route
    // XXX: Restriction to route scanning may have to be generalized to scanning of possible continuations when
    //      considering situations involving sudden route changes. See also the definition of the EncounterTypes.
    //      A second problem is that following situations on deviating routes may result in closing encounters
    //      too early if a leading foe is not traced on its new lane. (see test 'foe_leader_deviating_routes')

    // If veh is on an internal edge, the edgeIter points towards the last edge before the junction
    ConstMSEdgeVector::const_iterator edgeIter = veh.getCurrentRouteEdge();
    assert(*edgeIter != 0);

    // Best continuation lanes for the ego vehicle
    const std::vector<MSLane*> egoBestLanes = veh.getBestLanesContinuation();
    std::vector<MSLane*>::const_iterator laneIter = egoBestLanes.begin();

    // current lane in loop below
    const MSLane* lane = veh.getLane();
    assert(lane->isInternal() || lane == *laneIter);
    assert(lane != 0);
    // next non-internal lane on the route
    const MSLane* nextNonInternalLane = 0;

    const MSEdge* edge; // current edge in loop below

    // Init pos with vehicle's current position. Below pos is set to zero to denote
    // the beginning position of the currently considered edge
    double pos = veh.getPositionOnLane();
    // remainingRange is the range minus the distance that is already scanned downstream along the vehicles route
    double remainingDownstreamRange = range;
    // distToConflictLane is the distance of the ego vehicle to the start of the currently considered potential conflict lane (can be negative for its current lane)
    double distToConflictLane = -pos;

    // if the current edge is internal, collect all vehicles from the junction and below range upstream (except on the vehicles own edge),
    // this is analogous to the code treating junctions in the loop below. Note that the distance on the junction itself is not included into
    // range, so vehicles farther away than range can be collected, too.
    if (lane->isInternal()) {
        edge = &(lane->getEdge());

#ifdef DEBUG_SSM_SURROUNDING
        std::cout << SIMTIME << " Vehicle '" << veh.getID() << "' is on internal edge " << edge->getID() << "'.\n"
                  << "Previous edge of its route: '" << (*edgeIter)->getID() << "'" << std::endl;
#endif

        assert(edge->getToJunction() == edge->getFromJunction());

        const MSJunction* junction = edge->getToJunction();
        // Collect vehicles on the junction (TODO: Consider the case that this is an internal junction / the vehicles lane is the second part of a two-piece internal lane!!!)
        getVehiclesOnJunction(junction, distToConflictLane, lane, foeCollector);

        // Collect vehicles on incoming edges.
        // Note that this includes the previous edge on the ego vehicle's route.
        // (The distance on the current internal edge is ignored)
        const ConstMSEdgeVector& incoming = junction->getIncoming();
        for (ConstMSEdgeVector::const_iterator ei = incoming.begin(); ei != incoming.end(); ++ei) {
            if ((*ei)->isInternal()) {
                continue;
            }
            // Upstream range is taken from the vehicle's back
            getUpstreamVehicles(*ei, (*ei)->getLength(), range+veh.getLength(), distToConflictLane, lane, foeCollector);
        }

//        // Take into account internal distance covered on the current lane
//        (commented out, because upstream scanning disregards internal lanes on the last scanned junction
//        -- this makes the scanning symmetric between leader and follower)
//        remainingDownstreamRange -= lane->getLength() - pos;

        // Take into account non-internal lengths until next non-internal lane
        MSLink* link = lane->getLinkCont()[0];
        remainingDownstreamRange -= link->getInternalLengthsAfter();
        distToConflictLane += lane->getLength() + link->getInternalLengthsAfter();

        // The next non-internal lane
        pos = 0.;
        lane = *(++laneIter);
        edge = &lane->getEdge();
    } else {
        // Collect all vehicles in range behind ego vehicle
        edge = &(lane->getEdge());
        getUpstreamVehicles(edge, pos, range+veh.getLength(), distToConflictLane, lane, foeCollector);
    }

    assert(lane != 0);
    assert(!lane->isInternal());

    // Advance downstream the ego vehicle's route for distance 'range'.
    // Collect all vehicles on the traversed edges and on incoming edges at junctions.
    while (remainingDownstreamRange > 0.) {
#ifdef DEBUG_SSM_SURROUNDING
        std::cout << SIMTIME << " Scanning downstream for vehicle '" << veh.getID() << "'.\n"
                  << "Considering edge '" << edge->getID() << "' Remaining downstream range = " << remainingDownstreamRange
                  << "\n"
                  << std::endl;
#endif
        assert(!edge->isInternal());
        assert(!lane->isInternal());
        assert(pos == 0 || lane == veh.getLane());
        if (pos + remainingDownstreamRange < lane->getLength()) {
            // scan range ends on this lane
            getUpstreamVehicles(edge, pos + remainingDownstreamRange, remainingDownstreamRange, distToConflictLane, lane, foeCollector);
            // scanned required downstream range
            break;
        } else {
            // Also need to scan area that reaches beyond the lane
            // Collecting vehicles on non-internal edge ahead
            getUpstreamVehicles(edge, edge->getLength(), edge->getLength() - pos, distToConflictLane, lane, foeCollector);
            // account for scanned distance on lane
            remainingDownstreamRange -= lane->getLength() - pos;
            distToConflictLane += lane->getLength();
            pos = 0.;

            // proceed to next non-internal lane
            ++laneIter;
            assert(laneIter == egoBestLanes.end() || *laneIter != 0);

            // If the vehicle's best lanes go on, collect vehicles on the upcoming junction
            if (laneIter != egoBestLanes.end()) {
                // Upcoming junction
                const MSJunction* junction = lane->getEdge().getToJunction();

                // Find connection for ego on the junction
                nextNonInternalLane = *laneIter;
                MSLink* link = lane->getLinkTo(nextNonInternalLane);
                assert(link != 0);
                // First lane of the connection
                lane = link->getViaLane();
                assert(lane != 0);              // Collect vehicles on the junction

                getVehiclesOnJunction(junction, distToConflictLane, lane, foeCollector);

                // Collect vehicles on incoming edges (except the last edge, where we already collected). Use full range.
                const ConstMSEdgeVector& incoming = junction->getIncoming();
                for (ConstMSEdgeVector::const_iterator ei = incoming.begin(); ei != incoming.end(); ++ei) {
                    if (*ei == edge || (*ei)->isInternal()) {
                        continue;
                    }
                    getUpstreamVehicles(*ei, (*ei)->getLength(), range, distToConflictLane, lane, foeCollector);
                }

                // account for scanned distance on junction
                double linkLength = link->getInternalLengthsAfter();
                remainingDownstreamRange -= linkLength;
                distToConflictLane += linkLength;

                // update ego's lane to next non internal edge
                lane = nextNonInternalLane;
                edge = &(lane->getEdge());
            }
        }
    }
    // remove ego vehicle
    foeCollector.erase(&veh);
}

void
MSDevice_SSM::getUpstreamVehicles(const MSEdge* edge, double pos, double range, double egoDistToConflictLane, const MSLane* const egoConflictLane, FoeInfoMap& foeCollector) {
#ifdef DEBUG_SSM_SURROUNDING
    std::cout << SIMTIME << " getUpstreamVehicles() for edge '" << edge->getID() << "'"
              << " pos = " << pos << " range = " << range
              << "\nFound vehicles:"
              << std::endl;
#endif
    if (range <= 0) {
        return;
    }

    const std::vector<MSLane*>& lanes = edge->getLanes();
    // Collect vehicles on the given edge with position in [pos-range,pos]
    for (std::vector<MSLane*>::const_iterator li = lanes.begin(); li != lanes.end(); ++li) {
        MSLane* lane = *li;
        const MSLane::VehCont& vehicles = lane->getVehiclesSecure();
        for (MSLane::VehCont::const_iterator vi = vehicles.begin(); vi != vehicles.end(); ++vi) {
            MSVehicle* veh = *vi;
            if (veh->getPositionOnLane() - veh->getLength() <= pos && veh->getPositionOnLane() >= pos - range) {
#ifdef DEBUG_SSM
                std::cout << veh->getID()  << "\n";
#endif
                FoeInfo* c = new FoeInfo(); // c is deleted in updateEncounter()
                c->egoDistToConflictLane = egoDistToConflictLane;
                c->egoConflictLane = egoConflictLane;
                foeCollector[veh] = c;
            }
        }
        lane->releaseVehicles();
    }

#ifdef DEBUG_SSM
    std::cout << std::endl;
#endif

    // TODO: Gather vehicles from opposite direction. This should happen in any case, where opposite direction overtaking is possible.
    //       If it isn't it might still be nicer to trace oncoming vehicles for the resulting trajectories in the encounters
    //    if (edge->hasOpposite...)

    if (range <= pos) {
        return;
    }

    // Here we have: range > pos, i.e. we proceed collecting vehicles on preceding edges
    range -= pos;

    // Collect vehicles from incoming edges of the junction representing the origin of 'edge'
    const MSJunction* junction = edge->getFromJunction();
    if (!edge->isInternal()) {
        // collect vehicles on preceding junction (for internal edges this is already done in caller,
        // i.e. findSurroundingVehicles() or the recursive call from getUpstreamVehicles())
        getVehiclesOnJunction(junction, egoDistToConflictLane, egoConflictLane, foeCollector);
    }
    // Collect vehicles from incoming edges from the junction representing the origin of 'edge'
    const ConstMSEdgeVector& incoming = junction->getIncoming();
    for (ConstMSEdgeVector::const_iterator ei = incoming.begin(); ei != incoming.end(); ++ei) {
        if ((*ei)->isInternal()) {
            continue;
        }
        const MSEdge* inEdge = *ei;
        assert(inEdge != 0);
        double distOnJunction = edge->isInternal() ? 0. : inEdge->getInternalFollowingLengthTo(edge);
        if (distOnJunction >= range) {
            continue;
        }
        // account for vehicles on the predecessor edge
        getUpstreamVehicles(inEdge, inEdge->getLength(), range - distOnJunction, egoDistToConflictLane, egoConflictLane, foeCollector);
    }
}

void
MSDevice_SSM::getVehiclesOnJunction(const MSJunction* junction, double egoDistToConflictLane, const MSLane* const egoConflictLane, FoeInfoMap& foeCollector) {
#ifdef DEBUG_SSM_SURROUNDING
    std::cout << SIMTIME << " getVehiclesOnJunction() for junction '" << junction->getID() << "'"
              << "\nFound vehicles:"
              << std::endl;
#endif
    // Collect vehicles on internal lanes
    const std::vector<MSLane*> lanes = junction->getInternalLanes();
    for (std::vector<MSLane*>::const_iterator li = lanes.begin(); li != lanes.end(); ++li) {
        MSLane* lane = *li;
        const MSLane::VehCont& vehicles = lane->getVehiclesSecure();

        // Add FoeInfos (XXX: for some situations, a vehicle may be collected twice. Then the later finding overwrites the earlier in foeCollector.
        // This could lead to neglecting a conflict when determining foeConflictLane later.)
        for (MSLane::VehCont::const_iterator vi = vehicles.begin(); vi != vehicles.end(); ++vi) {
            FoeInfo* c = new FoeInfo();
            c->egoConflictLane = egoConflictLane;
            c->egoDistToConflictLane = egoDistToConflictLane;
            foeCollector[*vi] = c;
#ifdef DEBUG_SSM_SURROUNDING
            for (MSLane::VehCont::const_iterator vi = vehicles.begin(); vi != vehicles.end(); ++vi) {
                std::cout << (*vi)->getID() << "\n";
            }
#endif
        }
        lane->releaseVehicles();

        // If there is an internal continuation lane, also collect vehicles on that lane
        if (lane->getLinkCont().size() > 1 && lane->getLinkCont()[0]->getViaLane() != 0) {
            // There's a second internal lane of the connection
            lane = lane->getLinkCont()[0]->getViaLane();
            // This code must be modified, if more than two-piece internal lanes are allowed. Thus, assert:
            assert(lane->getLinkCont().size() == 0 || lane->getLinkCont()[0]->getViaLane() == 0);

            // collect vehicles
            const MSLane::VehCont& vehicles2 = lane->getVehiclesSecure();
            // Add FoeInfos. This duplicates the loop for the first internal lane
            for (MSLane::VehCont::const_iterator vi = vehicles2.begin(); vi != vehicles2.end(); ++vi) {
                FoeInfo* c = new FoeInfo();
                c->egoConflictLane = egoConflictLane;
                c->egoDistToConflictLane = egoDistToConflictLane;
                foeCollector[*vi] = c;
    #ifdef DEBUG_SSM_SURROUNDING
                for (MSLane::VehCont::const_iterator vi = vehicles2.begin(); vi != vehicles2.end(); ++vi){
                    std::cout << (*vi)->getID()<< "\n";
                }
#endif
            }
            lane->releaseVehicles();
        }
    }

#ifdef DEBUG_SSM_SURROUNDING
    std::cout << std::endl;
#endif
}



void
MSDevice_SSM::generateOutput() const {
    // This is called once at vehicle removal.
    //       Also: flush myOutputFile? Or is this done automatically?
    // myOutputFile->closeTag();
}

// ---------------------------------------------------------------------------
// Static parameter load helpers
// ---------------------------------------------------------------------------
std::string
MSDevice_SSM::getOutputFilename(const SUMOVehicle& v, std::string deviceID) {
    std::string file = deviceID + ".xml";
    if (v.getParameter().knowsParameter("device.ssm.file")) {
        try {
            file = v.getParameter().getParameter("device.ssm.file", file);
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.ssm.file", file) + "'for vehicle parameter 'ssm.measures'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.ssm.file")) {
        try {
            file = v.getVehicleType().getParameter().getParameter("device.ssm.file", file);
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.ssm.file", file) + "'for vType parameter 'ssm.measures'");
        }
    } else {
        std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'device.ssm.file'. Using default of '" << file << "'\n";
    }
    return file;
}


double
MSDevice_SSM::getDetectionRange(const SUMOVehicle& v) {
    OptionsCont& oc = OptionsCont::getOptions();
    double range = -INVALID;
    if (v.getParameter().knowsParameter("device.ssm.range")) {
        try {
            range = TplConvert::_2double(v.getParameter().getParameter("device.ssm.range", "").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.ssm.range", "") + "'for vehicle parameter 'ssm.range'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.ssm.range")) {
        try {
            range = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("device.ssm.range", "").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.ssm.range", "") + "'for vType parameter 'ssm.range'");
        }
    } else {
        range = oc.getFloat("device.ssm.range");
#ifdef DEBUG_SSM
        std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'device.ssm.range'. Using default of '" << range << "'\n";
#endif
    }
    return range;
}


double
MSDevice_SSM::getExtraTime(const SUMOVehicle& v) {
    OptionsCont& oc = OptionsCont::getOptions();
    double extraTime = INVALID;
    if (v.getParameter().knowsParameter("device.ssm.extratime")) {
        try {
            extraTime = TplConvert::_2double(v.getParameter().getParameter("device.ssm.extratime", "").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.ssm.extratime", "") + "'for vehicle parameter 'ssm.extratime'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.ssm.extratime")) {
        try {
            extraTime = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("device.ssm.extratime", "").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.ssm.extratime", "") + "'for vType parameter 'ssm.extratime'");
        }
    } else {
        extraTime = oc.getFloat("device.ssm.extratime");
#ifdef DEBUG_SSM
        std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'device.ssm.extratime'. Using default of '" << extraTime << "'\n";
#endif
    }
    if (extraTime < 0.) {
        extraTime = DEFAULT_EXTRA_TIME;
        WRITE_WARNING("Negative (or no) value encountered for vehicle parameter 'device.ssm.extratime' in vehicle '" + v.getID() + "' using default value " + toString(extraTime) + " instead");
    }
    return extraTime;
}


double
MSDevice_SSM::getMaxEncounterLength(const SUMOVehicle& v) {
    OptionsCont& oc = OptionsCont::getOptions();
    double maxEncounterLength = INVALID;
    if (v.getParameter().knowsParameter("device.ssm.maxencounterlength")) {
        try {
            maxEncounterLength = TplConvert::_2double(v.getParameter().getParameter("device.ssm.maxencounterlength", "").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.ssm.maxencounterlength", "") + "'for vehicle parameter 'ssm.maxencounterlength'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.ssm.maxencounterlength")) {
        try {
            maxEncounterLength = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("device.ssm.maxencounterlength", "").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.ssm.maxencounterlength", "") + "'for vType parameter 'ssm.maxencounterlength'");
        }
    } else {
        maxEncounterLength = oc.getFloat("device.ssm.maxencounterlength");
#ifdef DEBUG_SSM
        std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'device.ssm.maxencounterlength'. Using default of '" << maxEncounterLength << "'\n";
#endif
    }
    return maxEncounterLength;
}

bool
MSDevice_SSM::requestsTrajectories(const SUMOVehicle& v) {
    OptionsCont& oc = OptionsCont::getOptions();
    bool trajectories = false;
    if (v.getParameter().knowsParameter("device.ssm.trajectories")) {
        try {
            trajectories = TplConvert::_2bool(v.getParameter().getParameter("device.ssm.trajectories", "no").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.ssm.trajectories", "no") + "'for vehicle parameter 'ssm.trajectories'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.ssm.trajectories")) {
        try {
            trajectories = TplConvert::_2bool(v.getVehicleType().getParameter().getParameter("device.ssm.trajectories", "no").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.ssm.trajectories", "no") + "'for vType parameter 'ssm.trajectories'");
        }
    } else {
        trajectories = oc.getBool("device.ssm.trajectories");
#ifdef DEBUG_SSM
        std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'device.ssm.trajectories'. Using default of '" << trajectories << "'\n";
#endif
    }
    return trajectories;
}


bool
MSDevice_SSM::getMeasuresAndThresholds(const SUMOVehicle& v, std::string deviceID, std::map<std::string, double>& thresholds) {
    OptionsCont& oc = OptionsCont::getOptions();

    // Measures
    std::string measures_str = "";
    if (v.getParameter().knowsParameter("device.ssm.measures")) {
        try {
            measures_str = v.getParameter().getParameter("device.ssm.measures", "");
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.ssm.measures", "") + "'for vehicle parameter 'ssm.measures'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.ssm.measures")) {
        try {
            measures_str = v.getVehicleType().getParameter().getParameter("device.ssm.measures", "");
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.ssm.measures", "") + "'for vType parameter 'ssm.measures'");
        }
    } else {
        measures_str = oc.getString("device.ssm.measures");
        std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'device.ssm.measures'. Using default of '" << measures_str << "'\n";
    }

    // Check retrieved measures
    if (measures_str == "") {
        WRITE_WARNING("No measures specified for ssm device of vehicle '" + v.getID() + "'. Registering all available SSMs.");
        measures_str = AVAILABLE_SSMS;
    }
    StringTokenizer st = StringTokenizer(AVAILABLE_SSMS);
    std::vector<std::string> available = st.getVector();
    st = StringTokenizer(measures_str);
    std::vector<std::string> measures = st.getVector();
    for (std::vector<std::string>::const_iterator i = measures.begin(); i != measures.end(); ++i) {
        if (std::find(available.begin(), available.end(), *i) == available.end()) {
            // Given identifier is unknown
            WRITE_ERROR("SSM identifier '" + *i + "' is not supported. Aborting construction of SSM device '" + deviceID + "'.");
            return false;
        }
    }

    // Thresholds
    std::string thresholds_str = "";
    if (v.getParameter().knowsParameter("device.ssm.thresholds")) {
        try {
            thresholds_str = v.getParameter().getParameter("device.ssm.thresholds", "");
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.ssm.thresholds", "") + "'for vehicle parameter 'ssm.thresholds'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.ssm.thresholds")) {
        try {
            thresholds_str = v.getVehicleType().getParameter().getParameter("device.ssm.thresholds", "");
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.ssm.thresholds", "") + "'for vType parameter 'ssm.thresholds'");
        }
    } else {
        thresholds_str = oc.getString("device.ssm.thresholds");
#ifdef DEBUG_SSM
        std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'device.ssm.thresholds'. Using default of '" << thresholds_str << "'\n";
#endif
    }

    // Parse vector of doubles from threshold_str
    int count = 0;
    if (thresholds_str != "") {
        st = StringTokenizer(thresholds_str);
        while (st.hasNext() && count < (int)measures.size()) {
            double thresh = TplConvert::_2double(st.next().c_str());
            thresholds.insert(std::make_pair(measures[count],thresh));
            ++count;
        }
        if (thresholds.size() != measures.size()) {
            WRITE_ERROR("Given list of thresholds ('" + thresholds_str + "') has not the same length as the assumed list of measures ('" + measures_str + "').");
            return false;
        }
    } else {
        // assume default thresholds if none are given
        for (std::vector<std::string>::const_iterator i = measures.begin(); i != measures.end(); ++i) {
            if (*i == "TTC") {
                thresholds.insert(std::make_pair(*i, DEFAULT_THRESHOLD_TTC));
            } else if (*i == "DRAC") {
                thresholds.insert(std::make_pair(*i, DEFAULT_THRESHOLD_DRAC));
            } else if (*i == "PET") {
                thresholds.insert(std::make_pair(*i, DEFAULT_THRESHOLD_PET));
            } else {
                WRITE_ERROR("Unknown SSM identifier '" + (*i) + "'. Aborting construction of ssm device."); // should never occur
                return false;
            }
        }
    }
    return true;
}



/****************************************************************************/

