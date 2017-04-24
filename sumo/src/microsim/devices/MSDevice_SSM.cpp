/****************************************************************************/
/// @file    MSDevice_SSM.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    11.06.2013
/// @version $Id$
///
// A device which stands as an implementation example and which outputs movereminder calls
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
#include "MSDevice_SSM.h"


// Debug constants
#define DEBUG_SSM

// ===========================================================================
// Constants
// ===========================================================================
// value indicating an invalid double parameter
#define INVALID -std::numeric_limits<double>::max()
// default value for the maximal episode length for logged encounters
#define DEFAULT_FREQUENCY 30.0
// default value for the detection range of potential opponents
#define DEFAULT_RANGE 50.0

// list of implemented SSMs (NOTE: To add more SSMs identifiers are added to AVAILABLE_SSMS
//                                 and a default threshold must be defined. A corresponding
//                                 case should be added to the switch in buildVehicleDevices)
#define AVAILABLE_SSMS "TTC DRAC PET"
#define DEFAULT_THRESHOLD_TTC 3. // in [s.], events get logged if below threshold
#define DEFAULT_THRESHOLD_DRAC 4. // in [m/s^2], events get logged if above threshold
#define DEFAULT_THRESHOLD_PET 2. // in seconds, events get logged if below threshold

// ===========================================================================
// method definitions
// ===========================================================================
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
        for (std::set<MSDevice*>::iterator ii = instances->begin(); ii != instances->end(); ++ii){
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
    oc.doRegister("device.ssm.frequency", new Option_Float(DEFAULT_FREQUENCY));
    oc.addDescription("device.ssm.frequency", "SSM Device", "Specifies the maximal length of stored conflict trajectories in seconds (the frequency of logging, default is " + toString(DEFAULT_FREQUENCY) + "s.).");
    oc.doRegister("device.ssm.range", new Option_Float(DEFAULT_RANGE));
    oc.addDescription("device.ssm.range", "SSM Device", "Specifies the detection range in meters (default is " + toString(DEFAULT_RANGE) + "m.). For vehicles below this distance from the equipped vehicle, SSM values are traced.");
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
        std::vector<double> thresholds;
        std::vector<std::string> measures;
        bool success = getMeasuresAndThresholds(v, deviceID, thresholds, measures);
        if (!success) return;

        // Trajectories
        bool trajectories = requestsTrajectories(v);

        // logging frequency
        double frequency = getLoggingFrequency(v);

        // detection range
        double range = getDetectionRange(v);

        // File
        std::string file = getOutputFilename(v, deviceID);

        // Build the device (XXX: who deletes it?)
        MSDevice_SSM* device = new MSDevice_SSM(v, deviceID, file, measures, thresholds, trajectories, frequency, range);
        into.push_back(device);
    }
}


MSDevice_SSM::Encounter::Encounter(const MSVehicle* _ego, const MSVehicle* const _foe, double _begin) :
    ego(_ego),
    foe(_foe),
    begin(_begin),
    end(INVALID),
    type(ENCOUNTER_TYPE_NOCONFLICT)
{
#ifdef DEBUG_SSM
    std::cout << "\n" << SIMTIME << " Constructing encounter of '"
            << ego->getID() << "' and '" << foe->getID() << "'" << std::endl;
#endif
}

MSDevice_SSM::Encounter::~Encounter()
{
#ifdef DEBUG_SSM
    std::cout << "\n" << SIMTIME << " Destroying encounter of '"
            << ego->getID() << "' and '" << foe->getID() << "' (begin was " << begin << ")" << std::endl;
#endif
}


void
MSDevice_SSM::Encounter::add(double time, Position egoX, Position egoV, Position foeX, Position foeV){
    this->timespan.push_back(time);
    this->egoTrajectory.x.push_back(egoX);
    this->egoTrajectory.v.push_back(egoV);
    this->foeTrajectory.x.push_back(foeX);
    this->foeTrajectory.v.push_back(foeV);
}

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

    for (FoeInfoMap::const_iterator foe = foes.begin(); foe != foes.end(); ++foe){
        std::pair<MSLane*, MSLane*> conflictLanes;
        Encounter* e = new Encounter(myHolderMS, foe->first, SIMTIME);
        updateEncounter(e, foe->second); // deletes foe->second
        myActiveEncounters.push_back(e);
    }
}

void
MSDevice_SSM::resetEncounters() {
    // Call processEncounters() with empty vehicle set
    FoeInfoMap foes;
    // processEncounters with empty argument closes all encounters
    processEncounters(foes);
}

void
MSDevice_SSM::processEncounters(FoeInfoMap& foes) {
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
    while (ei != myActiveEncounters.end()){
        Encounter* e = *ei;
        if (foes.find(e->foe) != foes.end()){
            FoeInfo* foeInfo = foes[e->foe];
            // Update encounter
            updateEncounter(e, foeInfo); // deletes foeInfo
            // Erase foes which were already encountered
            foes.erase(e->foe);
            ++ei;
        } else {
            // Close encounter
            closeEncounter(e);
            if (qualifiesAsConflict(e)) {
                myPastConflicts.push(e);
            } else {
                delete e;
            }
            ei = myActiveEncounters.erase(ei);
        }
    }

}


bool
MSDevice_SSM::qualifiesAsConflict(Encounter* e){
    // TODO: Check if conflict measure thresholds are exceeded
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " qualifiesAsConflict() for encounter of vehicles '"
            << e->ego->getID() << "' and '" << e->foe->getID()
            << "'" << std::endl;
#endif
    return true;
}


void
MSDevice_SSM::closeEncounter(Encounter* e){
    // TODO: is there anything specific to do here?
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " closeEncounter() of vehicles '"
            << e->ego->getID() << "' and '" << e->foe->getID()
            << "'" << std::endl;
#endif
    return;
}

void
MSDevice_SSM::updateEncounter(Encounter* e, FoeInfo* foeInfo){
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " updateEncounter() of vehicles '"
            << e->ego->getID() << "' and '" << e->foe->getID()
            << "'" << std::endl;
#endif

    // init encounter type with most general identifier
    EncounterType type;
    UNUSED_PARAMETER(type); // is currently written but never read

    // Add current states to trajectories
    e->add(SIMTIME, e->ego->getPosition(), e->ego->getVelocityVector(), e->foe->getPosition(), e->foe->getVelocityVector());

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
    // 3) For crossing encounters the encounter distance is the distance until crossing point of the conflicting lanes.
    double egoEncounterDist = INVALID;
    double foeEncounterDist = INVALID;

    if (foeConflictLane == 0) {
        // foe vehicle is not on course towards the ego's route (see findFoeConflictLane)
        type = ENCOUNTER_TYPE_NOCONFLICT;
#ifdef DEBUG_SSM
    std::cout << "-> Encounter type: No conflict." << std::endl;
#endif
    } else if (!egoConflictLane->isInternal()) {
        // The conflict lane is non-internal, therefore we either have no potential conflict or a lead/follow situation (i.e., no crossing or merging)
        if (egoConflictLane == egoLane) {
            // The conflict point is on the ego's current lane.
            if (foeLane == egoLane) {
                // Foe is on the same non-internal lane
                type = ENCOUNTER_TYPE_FOLLOWING; // TODO: specify: check for leader / follower
#ifdef DEBUG_SSM
    std::cout << "-> Encounter type: Lead/follow-situation on non-internal lane '" << egoLane->getID() << "'" << std::endl;
#endif
            } else if (&(foeLane->getEdge()) == &(egoLane->getEdge())) {
                // Foe is on the same non-internal edge but not on the same lane. Treat this as no conflict for now
                // XXX: this disregards conflicts for vehicles on adjacent lanes
                type = ENCOUNTER_TYPE_NOCONFLICT;
#ifdef DEBUG_SSM
    std::cout << "-> Encounter type: No conflict (adjacent lanes)." << std::endl;
#endif
            } else {
                // Foe must be on a route leading into the ego's lane
                type = ENCOUNTER_TYPE_FOLLOWING_LEADER;
                foeEncounterDist = foeDistToConflictLane + e->ego->getBackPositionOnLane();
#ifdef DEBUG_SSM
    std::cout << "-> Encounter type: Ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' leads foe '"
            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
            << " (gap = " << foeEncounterDist << ")"
            << std::endl;
#endif
                assert(foeConflictLane == egoLane);
            }
        } else {
            // The egoConflictLane is a non-internal lane which is not the ego's current lane. Thus it must lie ahead of the ego vehicle and equal the foe's current lane.
            // see finSurroundingVehicles() (otherwise the foe would have had to enter the ego's route along a junction and the corresponding
            // conflict lane would be internal)
            type = ENCOUNTER_TYPE_FOLLOWING_FOLLOWER;
            egoEncounterDist = egoDistToConflictLane + e->foe->getBackPositionOnLane();
#ifdef DEBUG_SSM
    std::cout << "-> Encounter type: Ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' follows foe '"
            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
            << " (gap = " << egoEncounterDist << ")"
            << std::endl;
#endif
            assert(foeLane == egoConflictLane);
            assert(foeDistToConflictLane <= 0);
        }
    } else {
        // egoConflictLane is internal, i.e., lies on a junction. Besides the lead/follow situation (which may stretch over different lanes of a connection),
        // merging or crossing of the conflict lanes is possible.
        MSLink* egoEntryLink = egoConflictLane->getEntryLink();
        MSLink* foeEntryLink = foeConflictLane->getEntryLink();
        if (&(egoEntryLink->getViaLane()->getEdge()) == &(foeEntryLink->getViaLane()->getEdge())) {
            if (egoEntryLink != foeEntryLink) {
                // XXX: this disregards conflicts for vehicles on adjacent lanes
                type = ENCOUNTER_TYPE_NOCONFLICT;
#ifdef DEBUG_SSM
    std::cout << "-> Encounter type: No conflict (adjacent lanes)." << std::endl;
#endif
            } else {
                // Lead / follow situation on connection
                if (egoLane == egoConflictLane && foeLane != foeConflictLane) {
                    // ego on junction, foe not yet
                    type = ENCOUNTER_TYPE_FOLLOWING_LEADER;
                    foeEncounterDist = foeDistToConflictLane + e->ego->getBackPositionOnLane();
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: Ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' leads foe '"
                            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
                            << " (gap = " << foeEncounterDist << ")"
                            << std::endl;
#endif
                } else if (egoLane != egoConflictLane && foeLane == foeConflictLane) {
                    // foe on junction, ego not yet
                    type = ENCOUNTER_TYPE_FOLLOWING_FOLLOWER;
                    egoEncounterDist = egoDistToConflictLane + e->foe->getBackPositionOnLane();
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: Ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' follows foe '"
                            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
                            << " (gap = " << egoEncounterDist << ")"
                            << std::endl;
#endif
                } else {
                    // Both must be already on the junction in a lead / follow situation on a connection
                    // (since they approach via the same link, findSurroundingVehicles() would have determined a
                    // different conflictLane if both are not on the junction)
                    assert(egoLane == egoConflictLane);
                    assert(foeLane == foeConflictLane);
                    type = ENCOUNTER_TYPE_FOLLOWING; // TODO: specify: check for leader / follower
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
                if (&(foeEntryLink->getLane()->getEdge()) == &(egoEntryLink->getLane()->getEdge())){
                    // XXX: the situation of merging into adjacent lanes is disregarded for now
                    type = ENCOUNTER_TYPE_NOCONFLICT;
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: No conflict (adjacent lanes)." << std::endl;
#endif
                } else {
                    type = ENCOUNTER_TYPE_NOCONFLICT;
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: No conflict." << std::endl;
#endif
                }
            } else if (foeEntryLink->getLane() == egoEntryLink->getLane()) {
                type = ENCOUNTER_TYPE_MERGING; // TODO: determine projected lead/follow, conflict entries, etc.
                egoEncounterDist = egoDistToConflictLane + egoEntryLink->getInternalLengthsAfter();
                foeEncounterDist = foeDistToConflictLane + foeEntryLink->getInternalLengthsAfter();
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: Merging situation of ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' and foe '"
                            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
                            << "\nDistances to merge-point: ego: " << egoEncounterDist << ", foe: " << foeEncounterDist
                            << std::endl;
#endif
            } else {
                type = ENCOUNTER_TYPE_CROSSING; // TODO: determine projected lead/follow, conflict entries, etc.
                // XXX: GO ON HERE, implement MSLink::getLengthBeforeCrossing()
                egoEncounterDist = egoDistToConflictLane + egoEntryLink->getLengthBeforeCrossing(foeEntryLink);
                foeEncounterDist = foeDistToConflictLane + foeEntryLink->getLengthBeforeCrossing(egoEntryLink);
#ifdef DEBUG_SSM
                    std::cout << "-> Encounter type: Crossing situation of ego '" << e->ego->getID() << "' on lane '" << egoLane->getID() << "' and foe '"
                            << e->foe->getID() << "' on lane '" << foeLane->getID() << "'"
                            << "\nDistances to crossing-point: ego: " << egoEncounterDist << ", foe: " << foeEncounterDist
                            << std::endl;
#endif
            }
        }
    }
    // free foeInfo
    delete foeInfo;

    // Compute SSMs
    computeSSMs(e);

}

const MSLane*
MSDevice_SSM::findFoeConflictLane(const MSVehicle* foe, const MSLane* egoConflictLane, double& distToConflictLane) {

#ifdef DEBUG_SSM
    std::cout << SIMTIME << " findFoeConflictLane() for foe '"
            << foe->getID() << "' on lane '" << foe->getLane()->getID() << "'"
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
    if(conflictJunction != 0) {
        std::cout << "Potential conflict on junction '" << conflictJunction->getID()
                    << std::endl;
    }
#endif
    if (foeLane->isInternal() && foeLane->getEdge().getToJunction() == conflictJunction) {
        // foe is already on the conflict junction
        return foeLane;
    }

    // Foe is not on the conflict junction

    // Leading internal lanes in bestlanes are resembled as NULL-pointers skip them
    while (laneIter != foeBestLanesEnd && *laneIter == 0) {
        assert(foeLane->isInternal());
        distToConflictLane += foeLane->getLength();
        foeLane = foeLane->getLinkCont()[0]->getViaLane();
        ++laneIter;
    }

    // Look for the junction downstream along foeBestLanes
    while (laneIter != foeBestLanesEnd && distToConflictLane <= myRange) {
        // Eventual internal lanes were skipped
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
        if (laneIter == foeBestLanesEnd) return 0;
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
MSDevice_SSM::computeSSMs(Encounter* e) {
    // TODO
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " computeSSMs() for vehicles '"
            << e->ego->getID() << "' and '" << e->foe->getID()
            << "'" << std::endl;
#endif

    // Determine situation type. Can be:
    // 1) following
    // 2) crossing / merging (only one vehicle before conflict point)
    // 3) crossing / merging (both before conflict point)
    // 4) non-interacting
    // 5) collision
    // For each of 1),3),4), we have two subtypes, one where the ego is leader and one, where it is follower.
    // In general a situation develops as (3)->(2)->(1/4/5) or as some part starting later.



}

void
MSDevice_SSM::flushConflicts(bool flushAll) {
#ifdef DEBUG_SSM
    std::cout << "\n" << SIMTIME << " Device '" << getID() << "' flushConflicts()" << std::endl;
#endif
    double t = SIMTIME;
    while (!myPastConflicts.empty()) {
        if (flushAll || myPastConflicts.top()->begin <= t-myFrequency) {
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
    std::cout << SIMTIME << "writeOutConflict() of vehicles '"
            << e->ego->getID() << "' and '" << e->foe->getID()
            << "'" << std::endl;
#endif
    myOutputFile->openTag("conflict");
    myOutputFile->writeAttr("begin", e->begin).writeAttr("end", e->end);
    myOutputFile->writeAttr("ego", e->ego->getID()).writeAttr("foe", e->foe->getID());

    myOutputFile->openTag("timespan").writeAttr("values", e->timespan).closeTag();

    myOutputFile->openTag("egoTrajectory");
    myOutputFile->writeAttr("x", toString(e->egoTrajectory.x));
    myOutputFile->writeAttr("v", toString(e->egoTrajectory.v));
    myOutputFile->closeTag();

    myOutputFile->openTag("foeTrajectory");
    myOutputFile->writeAttr("x", toString(e->foeTrajectory.x));
    myOutputFile->writeAttr("v", toString(e->foeTrajectory.v));
    myOutputFile->closeTag();

    // TODO: output SSMs when implemented

    myOutputFile->closeTag();
}





// ---------------------------------------------------------------------------
// MSDevice_SSM-methods
// ---------------------------------------------------------------------------
MSDevice_SSM::MSDevice_SSM(SUMOVehicle& holder, const std::string& id, std::string outputFilename, std::vector<std::string> measures, std::vector<double> thresholds,
        bool trajectories, double frequency, double range) :
    MSDevice(holder, id),
    myMeasures(measures),
    myThresholds(thresholds),
    mySaveTrajectories(trajectories),
    myFrequency(frequency),
    myRange(range)
{
    // Take care! Holder is currently being constructed. Cast occurs before completion.
    myHolderMS = static_cast<MSVehicle*> (&holder);

    myComputeTTC = find(myMeasures.begin(), myMeasures.end(), "TTC") != myMeasures.end();
    myComputeDRAC = find(myMeasures.begin(), myMeasures.end(), "DRAC") != myMeasures.end();
    myComputePET = find(myMeasures.begin(), myMeasures.end(), "PET") != myMeasures.end();

    maxTrajectorySize = (int)std::ceil(myFrequency/TS)+1;
    myActiveEncounters = EncounterVector();
    myPastConflicts = EncounterQueue();

    // XXX: Who deletes the OutputDevice?
    myOutputFile = &OutputDevice::getDevice(outputFilename);
//    myOutputFile.writeXMLHeader("ssm_log", "ssm_log.xsd");
    myOutputFile->openTag("ssm_log");

    // register at static instance container
    instances->insert(this);

#ifdef DEBUG_SSM
    std::cout << "Initialized ssm device '" << id << "' with "
            << "myMeasures=" << joinToString(myMeasures, " ")
            << ", myThresholds=" << joinToString(myThresholds, " ")
            << ", mySaveTrajectories=" << mySaveTrajectories << ", myFrequency=" << myFrequency
            << ", myRange=" << myRange << ", output file=" << outputFilename << "\n";
#endif
}


/// @brief Destructor.
MSDevice_SSM::~MSDevice_SSM() {
    // XXX: Who deletes this device?
    // unregister from static instance container
    instances->erase((MSDevice*) this);
    resetEncounters();
    flushConflicts(true);
}


bool
MSDevice_SSM::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
#ifdef DEBUG_SSM
    std::cout << "device '" << getID() << "' notifyEnter: reason=" << reason << " currentEdge=" << veh.getLane()->getEdge().getID() << "\n";
#endif
    return true; // keep the device
}

bool
MSDevice_SSM::notifyLeave(SUMOVehicle& veh, double /*lastPos*/,
                              MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
#ifdef DEBUG_SSM
    std::cout << "device '" << getID() << "' notifyLeave: reason=" << reason << " currentEdge=" << veh.getLane()->getEdge().getID() << "\n";
#endif
    return true; // keep the device
}

bool
MSDevice_SSM::notifyMove(SUMOVehicle& /* veh */, double /* oldPos */,
                             double /* newPos */, double newSpeed) {
#ifdef DEBUG_SSM
    std::cout << "device '" << getID() << "' notifyMove: newSpeed=" << newSpeed << "\n";
#endif
    return true; // keep the device
}


void
MSDevice_SSM::findSurroundingVehicles(const MSVehicle& veh, double range, FoeInfoMap& foeCollector) {
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " Looking for surrounding vehicles for ego vehicle '" << veh.getID()
            << "' on edge '" << veh.getLane()->getEdge().getID()
            << "'."
            <<"\nVehicle's best lanes = " << toString(veh.getBestLanesContinuation())
            << std::endl;
#endif

    if (!veh.isOnRoad()) return;

    // The requesting vehicle's current route
    // XXX: Restriction to route scanning may have to be generalized to scanning of possible continuations when
    //      considering situations involving sudden route changes. See also the definition of the EncounterTypes.

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

#ifdef DEBUG_SSM
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
        for (ConstMSEdgeVector::const_iterator ei = incoming.begin(); ei != incoming.end(); ++ei){
            if ((*ei)->isInternal()) continue;
            getUpstreamVehicles(*ei, (*ei)->getLength(), range, distToConflictLane, lane, foeCollector);
        }

        // Take into account internal distance covered on the current lane
        remainingDownstreamRange -= lane->getLength()-pos;

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
        getUpstreamVehicles(edge, pos, range, distToConflictLane, lane, foeCollector);
    }

    assert(lane != 0);
    assert(!lane->isInternal());

    // Advance downstream the ego vehicle's route for distance 'range'.
    // Collect all vehicles on the traversed edges and on incoming edges at junctions.
    while (remainingDownstreamRange > 0.) {
#ifdef DEBUG_SSM
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
                assert(lane!=0);                // Collect vehicles on the junction

                getVehiclesOnJunction(junction, distToConflictLane, lane, foeCollector);

                // Collect vehicles on incoming edges (except the last edge, where we already collected). Use full range.
                const ConstMSEdgeVector& incoming = junction->getIncoming();
                for (ConstMSEdgeVector::const_iterator ei = incoming.begin(); ei != incoming.end(); ++ei){
                    if (*ei == edge || (*ei)->isInternal()) continue;
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
MSDevice_SSM::getUpstreamVehicles(const MSEdge* edge, double pos, double range, double egoDistToConflictLane, const MSLane* const egoConflictLane, FoeInfoMap& foeCollector){
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " getUpstreamVehicles() for edge '" << edge->getID() << "'"
            << " pos = " << pos << " range = " << range
            << "\nFound vehicles:"
            << std::endl;
#endif
    if (range <= 0) return;

    const std::vector<MSLane*>& lanes = edge->getLanes();
    // Collect vehicles on the given edge with position in [pos-range,pos]
    for (std::vector<MSLane*>::const_iterator li = lanes.begin(); li != lanes.end(); ++li){
        MSLane* lane = *li;
        const MSLane::VehCont& vehicles = lane->getVehiclesSecure();
        for (MSLane::VehCont::const_iterator vi = vehicles.begin(); vi != vehicles.end(); ++vi) {
            MSVehicle* veh = *vi;
            if (veh->getPositionOnLane() >= pos - range){
#ifdef DEBUG_SSM
                std::cout << veh->getID()  << "\n";
#endif
                FoeInfo* c = new FoeInfo(); // c is deleted in updateEncounter()
                c->egoDistToConflictLane = egoDistToConflictLane;
                c->egoConflictLane = egoConflictLane;
                foeCollector[veh] = c;
            }
        }
    }

#ifdef DEBUG_SSM
    std::cout << std::endl;
#endif

    // TODO: Gather vehicles from opposite direction. This should happen in any case, where opposite direction overtaking is possible.
    //       If it isn't it might still be nicer to trace oncoming vehicles for the resulting trajectories in the encounters
    //    if (edge->hasOpposite...)

    if (range <= pos) return;

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
    for (ConstMSEdgeVector::const_iterator ei = incoming.begin(); ei != incoming.end(); ++ei){
        if ((*ei)->isInternal()) continue;
        const MSEdge* inEdge = *ei;
        assert(inEdge != 0);
        double distOnJunction = edge->isInternal() ? 0. : inEdge->getInternalFollowingLengthTo(edge);
        if (distOnJunction >= range) continue;
        // account for vehicles on the predecessor edge
        getUpstreamVehicles(inEdge, inEdge->getLength(), range-distOnJunction, egoDistToConflictLane, egoConflictLane, foeCollector);
    }
}

void
MSDevice_SSM::getVehiclesOnJunction(const MSJunction* junction, double egoDistToConflictLane, const MSLane* const egoConflictLane, FoeInfoMap& foeCollector){
#ifdef DEBUG_SSM
    std::cout << SIMTIME << " getVehiclesOnJunction() for junction '" << junction->getID() << "'"
            << "\nFound vehicles:"
            << std::endl;
#endif
    // Collect vehicles on internal lanes
    const std::vector<MSLane*>& lanes = junction->getInternalLanes();
    for (std::vector<MSLane*>::const_iterator li = lanes.begin(); li != lanes.end(); ++li){
        MSLane* lane = *li;
        const MSLane::VehCont& vehicles = lane->getVehiclesSecure();

        // Add FoeInfos (XXX: for some situations, a vehicle may be collected twice. Then the later finding overwrites the earlier in foeCollector.
        // This could lead to neglecting a conflict when determining foeConflictLane later.)
        for (MSLane::VehCont::const_iterator vi = vehicles.begin(); vi != vehicles.end(); ++vi) {
            FoeInfo* c = new FoeInfo();
            c->egoConflictLane = egoConflictLane;
            c->egoDistToConflictLane = egoDistToConflictLane;
            foeCollector[*vi] = c;
#ifdef DEBUG_SSM
            for (MSLane::VehCont::const_iterator vi = vehicles.begin(); vi != vehicles.end(); ++vi){
                std::cout << (*vi)->getID()<< "\n";
            }
#endif
        }

        // If there is an internal continuation lane, also collect vehicles on that lane
        if (lane->getLinkCont().size()>1 && lane->getLinkCont()[0]->getViaLane()!=0){
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
    #ifdef DEBUG_SSM
                for (MSLane::VehCont::const_iterator vi = vehicles.begin(); vi != vehicles.end(); ++vi){
                    std::cout << (*vi)->getID()<< "\n";
                }
    #endif
            }
        }
    }

#ifdef DEBUG_SSM
    std::cout << std::endl;
#endif
}



void
MSDevice_SSM::generateOutput() const {
    // This is called once at vehicle removal.
    //       Also: flush myOutputFile? Or is this done automatically?
    myOutputFile->closeTag();
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
    double range = INVALID;
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
MSDevice_SSM::getLoggingFrequency(const SUMOVehicle& v) {
    OptionsCont& oc = OptionsCont::getOptions();
    double frequency = INVALID;
    if (v.getParameter().knowsParameter("device.ssm.frequency")) {
        try {
            frequency = TplConvert::_2double(v.getParameter().getParameter("device.ssm.frequency", "").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getParameter().getParameter("device.ssm.frequency", "") + "'for vehicle parameter 'ssm.frequency'");
        }
    } else if (v.getVehicleType().getParameter().knowsParameter("device.ssm.frequency")) {
        try {
            frequency = TplConvert::_2double(v.getVehicleType().getParameter().getParameter("device.ssm.frequency", "").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + v.getVehicleType().getParameter().getParameter("device.ssm.frequency", "") + "'for vType parameter 'ssm.frequency'");
        }
    } else {
        frequency = oc.getFloat("device.ssm.frequency");
#ifdef DEBUG_SSM
        std::cout << "vehicle '" << v.getID() << "' does not supply vehicle parameter 'device.ssm.frequency'. Using default of '" << frequency << "'\n";
#endif
    }
    return frequency;
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
MSDevice_SSM::getMeasuresAndThresholds(const SUMOVehicle& v, std::string deviceID, std::vector<double>& thresholds, std::vector<std::string>& measures){
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
    measures = st.getVector();
    for (std::vector<std::string>::const_iterator i = measures.begin(); i != measures.end(); ++i){
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
    if (thresholds_str != "") {
        st = StringTokenizer(thresholds_str);
        while (st.hasNext()){
            thresholds.push_back(TplConvert::_2double(st.next().c_str()));
        }
        if (thresholds.size() != measures.size()) {
            WRITE_ERROR("Given list of thresholds ('"+ thresholds_str +"') has not the same length as the assumed list of measures ('" + measures_str + "').");
            return false;
        }
    } else {
        // assume default thresholds if none are given
        for (std::vector<std::string>::const_iterator i = measures.begin(); i != measures.end(); ++i){
            if (*i == "TTC") {
                thresholds.push_back(DEFAULT_THRESHOLD_TTC);
            } else if (*i == "DRAC") {
                thresholds.push_back(DEFAULT_THRESHOLD_DRAC);
            } else if (*i == "PET") {
                thresholds.push_back(DEFAULT_THRESHOLD_PET);
            } else {
                WRITE_ERROR("Unknown SSM identifier '"+(*i)+"'. Aborting construction of ssm device."); // should never occur
                return false;
            }
        }
    }
    return true;
}





/****************************************************************************/

