/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    MELoop.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
///
// The main mesocopic simulation loop
/****************************************************************************/
#include <config.h>

#include <queue>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include <utils/threadpool/WorkStealingThreadPool.h>
#include <microsim/MSVehicleType.h>

#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/RandHelper.h>
#include "MELoop.h"
#include "MEPartition.h"
#include "MESegment.h"
#include "MELSegment.h"
#include "MEVehicle.h"

long long int MELoop::LeaderEvent::myEventCounter(0);

class MELoop::ParallelEvents {
public:
    ParallelEvents(int threads, const std::string& partitionFile) :
        myThreads(threads), myPartitionFile(partitionFile) {}

    void loadPartition() {
        if (myPartitionFile.empty() || !myComponents.empty()) {
            return;
        }
        const auto edges = MEPartition::load(myPartitionFile);
        if (edges.empty()) {
            throw ProcessError(TLF("Mesoscopic partition file '%' does not assign any edges.", myPartitionFile));
        }
        std::unordered_map<std::string, int> ids;
        myComponents.reserve(edges.size());
        for (const auto& edge : edges) {
            const int group = ids.emplace(edge.second, (int)ids.size()).first->second;
            myComponents.emplace(edge.first, EdgeGroup{group});
        }
        myComponentCount = (int)ids.size();
    }

    bool runBatch(MELoop& loop, const LeaderEvent& first) {
        if (myComponents.empty()) {
            if (myPartitionFile.empty()) {
                buildComponents();
            } else {
                loadPartition();
            }
        }
        if (myComponentCount < 2 || loop.myLeaderCars.size() < MIN_PARALLEL_BATCH - 1
                || loop.myLeaderCars.top().time != first.time) {
            return false;
        }
        const Footprint firstEdges = workerFootprint(loop, first.veh);
        if (firstEdges.origin == nullptr) {
            return false;
        }
        ++myBatchIndex;
        myBatch.clear();
        claim(first.veh, firstEdges);
        // Retain a prefix of the serial order. Every move has exclusive access
        // to both edges it can touch. New same-time events follow this prefix.
        while (myBatch.size() < MAX_PARALLEL_BATCH && !loop.myLeaderCars.empty() && loop.myLeaderCars.top().time == first.time) {
            const LeaderEvent next = loop.myLeaderCars.top();
            if (loop.skipInvalidated(next)) {
                loop.myLeaderCars.pop();
                continue;
            }
            const Footprint edges = workerFootprint(loop, next.veh);
            if (edges.origin == nullptr || edges.origin->batch == myBatchIndex || edges.destination->batch == myBatchIndex) {
                break;
            }
            loop.myLeaderCars.pop();
            claim(next.veh, edges);
        }
        if (myBatch.size() < MIN_PARALLEL_BATCH || !runWorkers(loop)) {
            for (const Event& event : myBatch) {
                loop.checkCar(event.veh);
            }
        }
        assert(loop.myLeaderCars.empty() || loop.myLeaderCars.top().time >= first.time);
        return true;
    }

    static bool defer(MEVehicle* veh, MSLink* link, bool add) {
        if (myChanges == nullptr) {
            return false;
        }
        myChanges->push_back({veh, link, add});
        return true;
    }

private:
    struct QueueChange {
        MEVehicle* veh;
        MSLink* link;
        bool add;
    };

    struct EdgeGroup {
        int group;
        size_t batch = 0;
    };

    struct Footprint {
        EdgeGroup* origin = nullptr;
        EdgeGroup* destination = nullptr;
    };

    void claim(MEVehicle* veh, const Footprint& edges) {
        edges.origin->batch = myBatchIndex;
        edges.destination->batch = myBatchIndex;
        myBatch.push_back({veh, edges.origin->group});
    }

    struct Event {
        MEVehicle* veh;
        int group;
    };

    struct WorkerAssignment {
        int worker = 0;
        size_t batch = 0;
    };

    static constexpr size_t MIN_PARALLEL_BATCH = 32;
    static constexpr size_t MAX_PARALLEL_BATCH = 1024;

    bool runWorkers(MELoop& loop) {
        const int workers = MIN2(myThreads, myComponentCount);
        myAssignments.resize(myComponentCount);
        myWorkerEvents.resize(workers);
        for (auto& events : myWorkerEvents) {
            events.clear();
        }
        int groups = 0;
        for (size_t index = 0; index < myBatch.size(); ++index) {
            WorkerAssignment& assignment = myAssignments[myBatch[index].group];
            if (assignment.batch != myBatchIndex) {
                assignment.batch = myBatchIndex;
                assignment.worker = groups++ % workers;
            }
            myWorkerEvents[assignment.worker].push_back(index);
        }
        if (groups < 2) {
            return false;
        }
        if (myPool == nullptr) {
            myPool = std::make_unique<WorkStealingThreadPool<>>(false, std::vector<int>(workers, 0));
        }
        myWorkerChanges.resize(MAX2(myWorkerChanges.size(), myBatch.size()));
        std::vector<std::future<void>> completed;
        completed.reserve(MIN2(groups, workers));
        for (int worker = 0; worker < MIN2(groups, workers); ++worker) {
            completed.push_back(myPool->executeAsync([this, &loop, worker](int) {
                for (const size_t index : myWorkerEvents[worker]) {
                    checkCar(loop, myBatch[index].veh, myWorkerChanges[index]);
                }
            }, worker));
        }
        for (auto& result : completed) {
            result.wait();
        }
        for (auto& result : completed) {
            result.get();
        }
        // Preserve serial event indices and approach registration order.
        for (size_t index = 0; index < myBatch.size(); ++index) {
            for (const QueueChange& change : myWorkerChanges[index]) {
                if (change.add) {
                    loop.addLeaderCar(change.veh, change.link);
                } else {
                    loop.removeLeaderCar(change.veh);
                }
            }
        }
        return true;
    }

    static void checkCar(MELoop& loop, MEVehicle* veh, std::vector<QueueChange>& changes) {
        changes.clear();
        myChanges = &changes;
        try {
            loop.checkCar(veh);
        } catch (...) {
            myChanges = nullptr;
            throw;
        }
        myChanges = nullptr;
    }

    void buildComponents() {
        const auto& edges = MSNet::getInstance()->getEdgeControl().getEdges();
        std::unordered_map<const MSJunction*, std::vector<const MSEdge*>> incident;
        for (const MSEdge* const edge : edges) {
            incident[edge->getFromJunction()].push_back(edge);
            incident[edge->getToJunction()].push_back(edge);
        }
        myComponents.reserve(edges.size());
        std::unordered_set<const MSJunction*> visited;
        for (const MSEdge* const edge : edges) {
            if (!myComponents.emplace(edge, EdgeGroup{myComponentCount}).second) {
                continue;
            }
            std::vector<const MSEdge*> pending{edge};
            while (!pending.empty()) {
                const MSEdge* current = pending.back();
                pending.pop_back();
                for (const MSJunction* const junction : {current->getFromJunction(), current->getToJunction()}) {
                    if (junction == nullptr || !visited.insert(junction).second) {
                        continue;
                    }
                    for (const MSEdge* const neighbor : incident.at(junction)) {
                        if (myComponents.emplace(neighbor, EdgeGroup{myComponentCount}).second) {
                            pending.push_back(neighbor);
                        }
                    }
                }
            }
            ++myComponentCount;
        }
    }

    Footprint workerFootprint(MELoop& loop, MEVehicle* veh) {
        const MESegment* const on = veh->getSegment();
        if (on == nullptr || veh->getEdge() != &on->getEdge() || !on->getEdge().isNormal() || veh->getQueIndex() == MESegment::PARKING_QUEUE
                || veh->hasStops() || veh->getPersonNumber() != 0 || veh->getContainerNumber() != 0
                || (veh->getVClass() & SVC_RAIL_CLASSES) != 0
                || veh->succEdge(1) == nullptr
                || (veh->getParameter().arrivalEdge >= 0 && veh->getRoutePosition() >= veh->getParameter().arrivalEdge)
                || !veh->hasOnlyLocalMoveReminders()) {
            return {};
        }
        // Eligibility must cover all effects of checkCar, including a failed entry attempt.
        const MESegment* to = on->getNextSegment();
        if (to == nullptr) {
            const int arrivalEdge = veh->getParameter().arrivalEdge;
            if (on->usesJunctionLinks() || veh->succEdge(2) == nullptr
                    || (arrivalEdge >= 0 && veh->getRoutePosition() + 1 >= arrivalEdge)) {
                return {};
            }
            to = loop.nextSegment(veh->getSegment(), veh);
            // Internal links, disconnected routes and vaporization have other side effects.
            if (to == nullptr || &to->getEdge() != veh->succEdge(1) || &to->getEdge() == &on->getEdge()
                    || !to->getEdge().isNormal() || to->getEdge().isVaporizing()
                    || on->getEdge().getToJunction() != to->getEdge().getFromJunction()) {
                return {};
            }
        }
        if (to->getNextSegment() == nullptr || to->hasDetectors()) {
            return {};
        }
        const SUMOTime waiting = veh->getWaitingTime();
        const SUMOTime teleport = veh->getVehicleType().getParameter().getTimeToTeleport(MSGlobals::gTimeToGridlock);
        if ((teleport > 0 && waiting > teleport)
                || (MSGlobals::gTimeToTeleportDisconnected >= 0 && waiting > MSGlobals::gTimeToTeleportDisconnected)) {
            return {};
        }
        const auto origin = myComponents.find(&on->getEdge());
        const auto destination = myComponents.find(&to->getEdge());
        if (origin == myComponents.end() || destination == myComponents.end()) {
            return {};
        }
        return {&origin->second, &destination->second};
    }

    const int myThreads;
    const std::string myPartitionFile;
    int myComponentCount = 0;
    std::unordered_map<const MSEdge*, EdgeGroup> myComponents;
    size_t myBatchIndex = 0;
    std::vector<Event> myBatch;
    std::vector<WorkerAssignment> myAssignments;
    std::vector<std::vector<size_t>> myWorkerEvents;
    std::vector<std::vector<QueueChange>> myWorkerChanges;
    std::unique_ptr<WorkStealingThreadPool<>> myPool;
    static thread_local std::vector<QueueChange>* myChanges;
};

thread_local std::vector<MELoop::ParallelEvents::QueueChange>* MELoop::ParallelEvents::myChanges = nullptr;

// ===========================================================================
// method definitions
// ===========================================================================
MELoop::MELoop(const SUMOTime recheckInterval) : myFullRecheckInterval(recheckInterval), myLinkRecheckInterval(TIME2STEPS(1)) {
    const auto& options = OptionsCont::getOptions();
    const int mesoThreads = options.getInt("meso-threads");
    if (mesoThreads > 1) {
        myParallelEvents = std::make_unique<ParallelEvents>(mesoThreads,
                           options.isSet("meso-threads.partition-file") ? options.getString("meso-threads.partition-file") : "");
    }
    myInterpolateMesoPos = MSGlobals::gMesoInterpolatePos && (
                               options.isSet("fcd-output") || options.isSet("queue-output"));
}

MELoop::~MELoop() {
    myParallelEvents.reset();
    for (std::vector<MESegment*>::const_iterator j = myEdges2FirstSegments.begin(); j != myEdges2FirstSegments.end(); ++j) {
        for (MESegment* s = *j; s != nullptr;) {
            MESegment* n = s->getNextSegment();
            delete s;
            s = n;
        }
    }
}


void
MELoop::simulate(SUMOTime tMax) {
    if (myInterpolateMesoPos) {
        MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();
        for (const MSEdge* const edge : ec.getEdges()) {
            // needs to be called every stop because meso position one step t+1 depends on the position computed in step t
            for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*edge);
                    segment != nullptr; segment = segment->getNextSegment()) {
                if (segment->getCarNumber() > 0) {
                    segment->updateMesoPositions();
                }
            }
        }
    }
    while (!myLeaderCars.empty()) {
        LeaderEvent e = myLeaderCars.top();
        assert(e.time > tMax - DELTA_T);
        if (e.time > tMax) {
            return;
        }
        myLeaderCars.pop();
        if (skipInvalidated(e)) {
            continue;
        }
        if (myParallelEvents != nullptr && myParallelEvents->runBatch(*this, e)) {
            continue;
        }
        //std::cout << SIMTIME << " checkChar " << e.veh->getID() << " on=" << e.veh->getSegment()->getID() << " t=" << e.time << " nid=" << e.veh->getNumericalID() << "\n";
        checkCar(e.veh);
        assert(myLeaderCars.empty() || myLeaderCars.top().time >= e.time);
    }
}


bool
MELoop::skipInvalidated(const LeaderEvent& e) {
    while (!myInvalidatedLeaderCars.empty() && e.time > myInvalidatedLeaderCars.top().time) {
        myInvalidatedLeaderCars.pop();
    }
    return !myInvalidatedLeaderCars.empty() && wasInvalidated(e);
}


bool
MELoop::wasInvalidated(const LeaderEvent& e) {
    if (e.time == myInvalidatedLeaderCars.top().time) {
        // we need to check whether any of the events with the same time
        // is our current vehicle because the order in which
        // removeLeaderCar was called is unrelated to the order of addLeaderCar
        std::vector<LeaderEvent> tmp;
        bool curInvalid = false;
        while (!myInvalidatedLeaderCars.empty() && e.time == myInvalidatedLeaderCars.top().time) {
            if (e == myInvalidatedLeaderCars.top()) {
                myInvalidatedLeaderCars.pop();
                curInvalid = true;
                break;
            } else {
                tmp.push_back(myInvalidatedLeaderCars.top());
                myInvalidatedLeaderCars.pop();
            }
        }
        for (LeaderEvent& le : tmp) {
            myInvalidatedLeaderCars.push(le);
        }
        return curInvalid;
    }
    return false;
}


SUMOTime
MELoop::changeSegment(MEVehicle* veh, SUMOTime leaveTime, MESegment* const toSegment, MSMoveReminder::Notification reason, const bool ignoreLink) const {
    int qIdx = 0;
    MESegment* const onSegment = veh->getSegment();
    if (MESegment::isInvalid(toSegment)) {
        if (veh->isStoppedTriggered()) {
            return leaveTime + MAX2(SUMOTime(1), myLinkRecheckInterval);
        }
        if (onSegment != nullptr) {
            onSegment->send(veh, toSegment, qIdx, leaveTime, reason);
        } else {
            WRITE_WARNINGF(TL("Vehicle '%' teleports beyond arrival edge '%', time=%."),
                           veh->getID(), veh->getEdge()->getID(), time2string(leaveTime));
        }
        veh->setSegment(toSegment); // signal arrival
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return leaveTime;
    } else if ((!MSGlobals::gCheckRoutes || veh->ignoreTransientPermissions())
               && !ignoreLink
               && !MESegment::isInvalid(onSegment)
               && &onSegment->getEdge() != &toSegment->getEdge()
               && veh->getEdge()->allowedLanes(*veh->succEdge(1), veh->getVClass()) == nullptr) {
        if (veh->isStopped()) {
            veh->processStop();
        }
        // all queues on the next segment are forbidden. We don't want to check
        // too often whether the forbiden edge has become permitted again
        const SUMOTime ttt = veh->getVehicleType().getParameter().getTimeToTeleport(MSGlobals::gTimeToGridlock);
        if (ttt > 0) {
            // if teleporting is enabled, make sure we look at the vehicle when the gridlock-time is up
            const SUMOTime recheck = MSGlobals::gTimeToTeleportDisconnected >= 0 ? MIN2(ttt, MSGlobals::gTimeToTeleportDisconnected) : ttt;
            return MAX2(MIN2(leaveTime + myLinkRecheckInterval, MIN2(veh->getBlockTime(), leaveTime) + recheck + 1), leaveTime + 1);
        }
        return leaveTime + myLinkRecheckInterval;
    }
    toSegment->updateEntryBlockTime(leaveTime);
    const SUMOTime entry = toSegment->hasSpaceFor(veh, leaveTime, qIdx);
    int ps = MESegment::PROCEED_UNKNOWN;
    if (entry == leaveTime) {
        if (ignoreLink) {
            ps = MESegment::PROCEED_YES;
        } else {
            ps = veh->mayProceed();
        }
    }
    if (ps == MESegment::PROCEED_YES) {
        if (onSegment != nullptr) {
            if (veh->getQueIndex() == MESegment::PARKING_QUEUE) { // parking or just aborted parking
                if (veh->isParking()) {
                    veh->processStop();
                }
                veh->getEdge()->getLanes()[0]->removeParking(veh);  // TODO for GUI only
            } else {
                onSegment->send(veh, toSegment, qIdx, leaveTime, onSegment->getNextSegment() == nullptr ? MSMoveReminder::NOTIFICATION_JUNCTION : MSMoveReminder::NOTIFICATION_SEGMENT);
            }
            toSegment->receive(veh, qIdx, leaveTime, false, ignoreLink, &onSegment->getEdge() != &toSegment->getEdge());
        } else {
            WRITE_WARNINGF(TL("Vehicle '%' ends teleporting on edge '%':%, time=%."),
                           veh->getID(), toSegment->getEdge().getID(), toSegment->getIndex(), time2string(leaveTime));
            // this is not quite correct but suffices for interrogation by
            // subsequent methods (veh->getSpeed() needs segment != 0)
            veh->setSegment(myEdges2FirstSegments[veh->getEdge()->getNumericalID()]);
            // clean up detectors (do not add traffic data)
            // note: updateDatector is not called if leaveTime == getLastEntryTime()
            veh->updateDetectors(veh->getLastEntryTime(), veh->getEventTime(), true, MSMoveReminder::NOTIFICATION_TELEPORT);
            toSegment->receive(veh, qIdx, leaveTime, false, true, true);
        }
        return entry;
    } else {
        if (veh->getQueIndex() != MESegment::PARKING_QUEUE && veh->isStopped() && veh->endTriggeredStop()) {
            veh->processStop();
        }
    }
    if ((ps & MESegment::PROCEED_NO) != 0) {
        if ((ps & MESegment::PROCEED_NO_TLS) != 0) {
            // recheck on the step grid: link states only change between steps, so keeping the sub-step phase of the
            // arrival would release the car up to one step before the link opened, depending on that phase alone
            return entry == SIMSTEP ? SIMSTEP + DELTA_T : SIMSTEP;
        } else {
            return entry + myLinkRecheckInterval;
        }
    }
    return entry;
}


void
MELoop::checkCar(MEVehicle* veh) {
    const SUMOTime leaveTime = veh->getEventTime();
    MESegment* const onSegment = veh->getSegment();
    MESegment* const toSegment = veh->getQueIndex() == MESegment::PARKING_QUEUE ? onSegment : nextSegment(onSegment, veh);
    const bool teleporting = (onSegment == nullptr); // is the vehicle currently teleporting?
    // @note reason is only evaluated if toSegment == nullptr
    const SUMOTime nextEntry = changeSegment(veh, leaveTime, toSegment, MSMoveReminder::NOTIFICATION_ARRIVED, teleporting);
    if (nextEntry == leaveTime) {
        return;
    }
    const SUMOTime ttt = veh->getVehicleType().getParameter().getTimeToTeleport(MSGlobals::gTimeToGridlock);
    const bool r1 = ttt > 0 && veh->getWaitingTime() > ttt;
    const bool r3 = MSGlobals::gTimeToTeleportDisconnected >= 0 && veh->getWaitingTime() > MSGlobals::gTimeToTeleportDisconnected;
    if (!veh->isStopped() && (r1 || r3)) {
        const bool disconnected = (MSGlobals::gTimeToTeleportDisconnected >= 0
                                   && veh->succEdge(1) != nullptr
                                   && veh->getEdge()->allowedLanes(*veh->succEdge(1), veh->getVClass()) == nullptr);
        if ((r1 && !disconnected) || (r3 && disconnected)) {
            teleportVehicle(veh, toSegment, disconnected);
            return;
        }
    }
    if (veh->getBlockTime() == SUMOTime_MAX && (!veh->isStopped()
            || (!veh->isStoppedTriggered() && veh->isStoppedParking()))) {
        veh->setBlockTime(leaveTime);
    }
    if (nextEntry == SUMOTime_MAX) {
        // all usable queues on the next segment are full
        SUMOTime newEventTime = MAX3(toSegment->getEventTime() + 1, leaveTime + 1, leaveTime + myFullRecheckInterval);
        if (ttt) {
            // if teleporting is enabled, make sure we look at the vehicle when the gridlock-time is up
            const SUMOTime recheck = veh->getBlockTime() + 1 + ttt;
            if (recheck > leaveTime) {
                newEventTime = MIN2(newEventTime, recheck);
            }
        }
        if (MSGlobals::gTimeToTeleportDisconnected >= 0) {
            const SUMOTime recheck = veh->getBlockTime() + 1 + MSGlobals::gTimeToTeleportDisconnected;
            if (recheck > leaveTime && veh->getEdge()->allowedLanes(*veh->succEdge(1), veh->getVClass()) == nullptr) {
                newEventTime = MIN2(newEventTime, recheck);
            }
        }
        veh->setEventTime(newEventTime);
    } else {
        // receiving segment has recently received another vehicle or the junction is blocked
        veh->setEventTime(nextEntry);
    }
    addLeaderCar(veh, teleporting ? nullptr : onSegment->getLink(veh));
}


void
MELoop::teleportVehicle(MEVehicle* veh, MESegment* const toSegment, bool disconnected) {
    const SUMOTime leaveTime = veh->getEventTime();
    MESegment* const onSegment = veh->getSegment();
    if (MSGlobals::gRemoveGridlocked) {
        WRITE_WARNINGF(TL("Teleporting vehicle '%'; waited too long, from edge '%':%, time=%."),
                       veh->getID(), onSegment->getEdge().getID(), onSegment->getIndex(),
                       time2string(leaveTime));
        MSNet::getInstance()->getVehicleControl().registerTeleportJam();
        int qIdx = 0;
        onSegment->send(veh, nullptr, qIdx, leaveTime, MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED);
        veh->setSegment(nullptr);
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return;
    }
    const bool teleporting = (onSegment == nullptr); // is the vehicle already teleporting?
    // try to find a place on the current edge
    MESegment* teleSegment = disconnected ? toSegment : toSegment->getNextSegment();
    while (teleSegment != nullptr && changeSegment(veh, leaveTime, teleSegment, MSMoveReminder::NOTIFICATION_TELEPORT, true) != leaveTime) {
        // @caution the time to get to the next segment here is ignored XXX
        teleSegment = teleSegment->getNextSegment();
    }
    if (teleSegment != nullptr) {
        if (!teleporting) {
            // we managed to teleport in a single jump
            const std::string reason = disconnected ? " (disconnected)" : "";
            WRITE_WARNINGF(TL("Teleporting vehicle '%'; waited too long%, from edge '%':% to edge '%':%, time=%."),
                           veh->getID(), reason, onSegment->getEdge().getID(), onSegment->getIndex(),
                           teleSegment->getEdge().getID(), teleSegment->getIndex(), time2string(leaveTime));
            MSNet::getInstance()->getVehicleControl().registerTeleportJam();
        }
    } else {
        // teleport across the current edge and try insertion later
        if (!teleporting) {
            int qIdx = 0;
            // announce start of multi-step teleport, arrival will be announced in changeSegment()
            WRITE_WARNINGF(TL("Teleporting vehicle '%'; waited too long, from edge '%':%, time=%."),
                           veh->getID(), onSegment->getEdge().getID(), onSegment->getIndex(), time2string(leaveTime));
            MSNet::getInstance()->getVehicleControl().registerTeleportJam();
            // remove from current segment
            onSegment->send(veh, nullptr, qIdx, leaveTime, MSMoveReminder::NOTIFICATION_TELEPORT);
            // mark veh as teleporting
            veh->setSegment(nullptr);
        } else {
            veh->updateDetectors(veh->getLastEntryTime(), leaveTime, true, MSMoveReminder::NOTIFICATION_TELEPORT);
        }
        // @caution microsim uses current travel time teleport duration
        const SUMOTime teleArrival = leaveTime + TIME2STEPS(veh->getEdge()->getLength() / MAX2(veh->getEdge()->getSpeedLimit(), NUMERICAL_EPS));
        const bool atDest = veh->moveRoutePointer();
        if (atDest) {
            // teleporting to end of route
            changeSegment(veh, teleArrival, nullptr, MSMoveReminder::NOTIFICATION_TELEPORT_ARRIVED, true);
        } else {
            veh->setEventTime(teleArrival);
            addLeaderCar(veh, nullptr);
            // teleporting vehicles must react to rerouters
            getSegmentForEdge(*veh->getEdge())->addReminders(veh);
            veh->activateReminders(MSMoveReminder::NOTIFICATION_JUNCTION);
        }
    }
}


void
MELoop::addLeaderCar(MEVehicle* veh, MSLink* link, const bool fromState) {
    if (myParallelEvents != nullptr && !fromState && ParallelEvents::defer(veh, link, true)) {
        return;
    }
    if (!fromState) {
        veh->setEventIndex(LeaderEvent::myEventCounter++);
    }
    myLeaderCars.push(LeaderEvent(veh));
    veh->setApproaching(link);
}


void
MELoop::clearState() {
    myLeaderCars = LeaderEventQueue();
    myInvalidatedLeaderCars = LeaderEventQueue();
}


void
MELoop::removeLeaderCar(MEVehicle* v) {
    if (myParallelEvents != nullptr && ParallelEvents::defer(v, nullptr, false)) {
        return;
    }
    myInvalidatedLeaderCars.push(LeaderEvent(v));
}


void
MELoop::vaporizeCar(MEVehicle* v, MSMoveReminder::Notification reason) {
    int qIdx = 0;
    v->getSegment()->send(v, nullptr, qIdx, MSNet::getInstance()->getCurrentTimeStep(), reason);
    removeLeaderCar(v);
}


MESegment*
MELoop::nextSegment(MESegment* s, MEVehicle* v) {
    if (s != nullptr) { // vehicle is not teleporting
        MESegment* next = s->getNextSegment();
        if (next != nullptr) {
            // ok, the street continues
            return next;
        }
    }
    // we have to check the next edge in the vehicle's route
    const MSEdge* nextEdge = v->succEdge(1);
    if (nextEdge == nullptr) {
        // end of route
        return nullptr;
    }
    if (MSGlobals::gUsingInternalLanes && s != nullptr && s->getEdge().isNormal()) {
        const MSEdge* internal = s->getEdge().getInternalFollowingEdge(nextEdge, v->getVClass());
        if (internal) {
            nextEdge = internal;
        }
    }
    return myEdges2FirstSegments[nextEdge->getNumericalID()];
}


int
MELoop::numSegmentsFor(const double length, const double sLength) {
    int no = (int)floor(length / sLength + 0.5);
    if (no == 0) { // assure there is at least one segment
        return 1;
    } else {
        return no;
    }
}


void
MELoop::buildSegmentsFor(const MSEdge& e, const OptionsCont& oc) {
    if (myParallelEvents != nullptr) {
        myParallelEvents->loadPartition();
    }
    const MESegment::MesoEdgeType& edgeType = MSNet::getInstance()->getMesoType(e.getEdgeType());
    const double length = e.getLength();
    const int numSegments = numSegmentsFor(length, edgeType.edgeLength);
    const double slength = length / (double)numSegments;
    MESegment* newSegment = nullptr;
    MESegment* nextSegment = nullptr;
    const bool laneQueue = oc.getBool("meso-lane-queue");
    const bool lift = oc.getBool("meso-ltm");
    bool multiQueue = laneQueue || (oc.getBool("meso-multi-queue") && e.getLanes().size() > 1 && e.getNumSuccessors() > 1);
    for (int s = numSegments - 1; s >= 0; s--) {
        std::string id = e.getID() + ":" + toString(s);
        newSegment = lift
                     ? new MELSegment(id, e, nextSegment, slength, e.getLanes()[0]->getSpeedLimit(), s, multiQueue, edgeType)
                     : new MESegment(id, e, nextSegment, slength, e.getLanes()[0]->getSpeedLimit(), s, multiQueue, edgeType);
        multiQueue = laneQueue;
        nextSegment = newSegment;
    }
    while (e.getNumericalID() >= static_cast<int>(myEdges2FirstSegments.size())) {
        myEdges2FirstSegments.push_back(0);
    }
    myEdges2FirstSegments[e.getNumericalID()] = newSegment;
    for (MSLane* lane : e.getLanes()) {
        lane->updateMesoGUISegments();
    }
}


MESegment*
MELoop::getSegmentForEdge(const MSEdge& e, double pos) {
    if (e.getNumericalID() >= (int)myEdges2FirstSegments.size()) {
        return nullptr;
    }
    MESegment* s = myEdges2FirstSegments[e.getNumericalID()];
    if (pos > 0) {
        double cpos = 0;
        while (s->getNextSegment() != nullptr && cpos + s->getLength() < pos) {
            cpos += s->getLength();
            s = s->getNextSegment();
        }
    }
    return s;
}


bool
MELoop::isEnteringRoundabout(const MSEdge& e) {
    for (const MSEdge* succ : e.getSuccessors()) {
        if (succ->isRoundabout()) {
            return true;
        }
    }
    return false;
}


MELoop::LeaderEvent::LeaderEvent(MEVehicle* v):
    time(v->getEventTime()),
    eventIndex(v->getEventIndex()),
    nid(v->getNumericalID()),
    veh(v) { }

/****************************************************************************/
