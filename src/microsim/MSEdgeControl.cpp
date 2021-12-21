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
/// @file    MSEdgeControl.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 09 Apr 2001
///
// Stores edges and lanes, performs moving of vehicle
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <queue>
#include <vector>
#include "MSEdgeControl.h"
#include "MSVehicleControl.h"
#include "MSGlobals.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSVehicle.h"

#define PARALLEL_PLAN_MOVE
#define PARALLEL_EXEC_MOVE
//#define PARALLEL_CHANGE_LANES
//#define LOAD_BALANCING

//#define PARALLEL_STOPWATCH

// ===========================================================================
// member method definitions
// ===========================================================================
MSEdgeControl::MSEdgeControl(const std::vector< MSEdge* >& edges)
    : myEdges(edges),
      myLanes(MSLane::dictSize()),
      myWithVehicles2Integrate(MSGlobals::gNumSimThreads > 1),
      myLastLaneChange(MSEdge::dictSize()),
      myInactiveCheckCollisions(MSGlobals::gNumSimThreads > 1),
      myMinLengthGeometryFactor(1.),
#ifdef THREAD_POOL
      myThreadPool(false, std::vector<int>(MSGlobals::gNumThreads, 0)),
#endif
      myStopWatch(3) {
    // build the usage definitions for lanes
    for (MSEdge* const edge : myEdges) {
        const std::vector<MSLane*>& lanes = edge->getLanes();
        if (!edge->hasLaneChanger()) {
            const int pos = lanes.front()->getNumericalID();
            myLanes[pos].lane = lanes.front();
            myLanes[pos].amActive = false;
            myLanes[pos].haveNeighbors = false;
            myMinLengthGeometryFactor = MIN2(edge->getLengthGeometryFactor(), myMinLengthGeometryFactor);
        } else {
            for (MSLane* const l : lanes) {
                const int pos = l->getNumericalID();
                myLanes[pos].lane = l;
                myLanes[pos].amActive = false;
                myLanes[pos].haveNeighbors = true;
                myMinLengthGeometryFactor = MIN2(l->getLengthGeometryFactor(), myMinLengthGeometryFactor);
            }
            myLastLaneChange[edge->getNumericalID()] = -1;
        }
    }
#ifndef THREAD_POOL
#ifdef HAVE_FOX
    if (MSGlobals::gNumThreads > 1) {
        while (myThreadPool.size() < MSGlobals::gNumThreads) {
            new WorkerThread(myThreadPool);
        }
    }
#endif
#endif
}


MSEdgeControl::~MSEdgeControl() {
#ifndef THREAD_POOL
#ifdef HAVE_FOX
    myThreadPool.clear();
#endif
#endif
#ifdef PARALLEL_STOPWATCH
    StopWatch<std::chrono::nanoseconds> wPlan;
    for (MSEdge* const edge : myEdges) {
        for (MSLane* const l : edge->getLanes()) {
            wPlan.add(l->getStopWatch()[0]);
        }
    }
    std::cout << wPlan.getHistory().size() << " lane planmove calls, average " << wPlan.getAverage() << " ns, total " << wPlan.getTotal() / double(1e9) << " s" << std::endl;
    std::cout << myStopWatch[0].getHistory().size() << " planmove calls, average " << myStopWatch[0].getAverage() << " ns, total " << myStopWatch[0].getTotal() / double(1e9) << " s" << std::endl;
    std::cout << myStopWatch[1].getHistory().size() << " execmove calls, average " << myStopWatch[1].getAverage() << " ns, total " << myStopWatch[1].getTotal() / double(1e9) << " s" << std::endl;
#endif
}


void
MSEdgeControl::patchActiveLanes() {
    for (std::set<MSLane*, ComparatorNumericalIdLess>::iterator i = myChangedStateLanes.begin(); i != myChangedStateLanes.end(); ++i) {
        LaneUsage& lu = myLanes[(*i)->getNumericalID()];
        // if the lane was inactive but is now...
        if (!lu.amActive && (*i)->getVehicleNumber() > 0) {
            // ... add to active lanes and mark as such
            if (lu.haveNeighbors) {
                myActiveLanes.push_front(*i);
            } else {
                myActiveLanes.push_back(*i);
            }
            lu.amActive = true;
        }
    }
    myChangedStateLanes.clear();
}


void
MSEdgeControl::planMovements(SUMOTime t) {
#ifdef PARALLEL_STOPWATCH
    myStopWatch[0].start();
#endif
#ifdef THREAD_POOL
    std::vector<std::future<void>> results;
#endif
    for (std::list<MSLane*>::iterator i = myActiveLanes.begin(); i != myActiveLanes.end();) {
        const int vehNum = (*i)->getVehicleNumber();
        if (vehNum == 0) {
            myLanes[(*i)->getNumericalID()].amActive = false;
            i = myActiveLanes.erase(i);
        } else {
#ifdef THREAD_POOL
            if (MSGlobals::gNumSimThreads > 1) {
                results.push_back(myThreadPool.executeAsync([i, t](int) {
                    (*i)->planMovements(t);
                }, (*i)->getRNGIndex() % MSGlobals::gNumSimThreads));
                ++i;
                continue;
            }
#else
#ifdef HAVE_FOX
            if (MSGlobals::gNumSimThreads > 1) {
                myThreadPool.add((*i)->getPlanMoveTask(t), (*i)->getRNGIndex() % myThreadPool.size());
                ++i;
                continue;
            }
#endif
#endif
            (*i)->planMovements(t);
            ++i;
        }
    }
#ifdef THREAD_POOL
    for (auto& r : results) {
        r.wait();
    }
#else
#ifdef HAVE_FOX
    if (MSGlobals::gNumSimThreads > 1) {
        myThreadPool.waitAll(false);
    }
#endif
#endif
#ifdef PARALLEL_STOPWATCH
    myStopWatch[0].stop();
#endif
}


void
MSEdgeControl::setJunctionApproaches(SUMOTime t) {
    for (MSLane* const lane : myActiveLanes) {
        lane->setJunctionApproaches(t);
    }
}


void
MSEdgeControl::executeMovements(SUMOTime t) {
#ifdef PARALLEL_STOPWATCH
    myStopWatch[1].start();
#endif
    std::vector<MSLane*> wasActive(myActiveLanes.begin(), myActiveLanes.end());
    myWithVehicles2Integrate.clear();
#ifdef PARALLEL_EXEC_MOVE
#ifdef THREAD_POOL
    if (MSGlobals::gNumSimThreads > 1) {
        for (MSLane* const lane : myActiveLanes) {
            myThreadPool.executeAsync([lane, t](int) {
                lane->executeMovements(t);
            }, lane->getRNGIndex() % MSGlobals::gNumSimThreads);
        }
        myThreadPool.waitAll();
    }
#else
#ifdef HAVE_FOX
    if (MSGlobals::gNumSimThreads > 1) {
        for (MSLane* const lane : myActiveLanes) {
            myThreadPool.add(lane->getExecuteMoveTask(t), lane->getRNGIndex() % myThreadPool.size());
        }
        myThreadPool.waitAll(false);
    }
#endif
#endif
#endif
    for (std::list<MSLane*>::iterator i = myActiveLanes.begin(); i != myActiveLanes.end();) {
        if (
#ifdef PARALLEL_EXEC_MOVE
            MSGlobals::gNumSimThreads <= 1 &&
#endif
            (*i)->getVehicleNumber() > 0) {
            (*i)->executeMovements(t);
        }
        if ((*i)->getVehicleNumber() == 0) {
            myLanes[(*i)->getNumericalID()].amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            ++i;
        }
    }
    for (MSLane* lane : wasActive) {
        lane->updateLengthSum();
    }
    MSNet::getInstance()->getVehicleControl().removePending();
    std::vector<MSLane*>& toIntegrate = myWithVehicles2Integrate.getContainer();
    std::sort(toIntegrate.begin(), toIntegrate.end(), ComparatorIdLess());
    /// @todo: sorting only needed to account for lane-ordering dependencies.
    //This should disappear when parallelization is working. Until then it would
    //be better to use ComparatorNumericalIdLess instead of ComparatorIdLess
    myWithVehicles2Integrate.unlock();
    for (MSLane* const lane : toIntegrate) {
        const bool wasInactive = lane->getVehicleNumber() == 0;
        lane->integrateNewVehicles();
        if (wasInactive && lane->getVehicleNumber() > 0) {
            LaneUsage& lu = myLanes[lane->getNumericalID()];
            if (!lu.amActive) {
                if (lu.haveNeighbors) {
                    myActiveLanes.push_front(lane);
                } else {
                    myActiveLanes.push_back(lane);
                }
                lu.amActive = true;
            }
        }
    }
#ifdef PARALLEL_STOPWATCH
    myStopWatch[1].stop();
#endif
}


void
MSEdgeControl::changeLanes(const SUMOTime t) {
    std::vector<MSLane*> toAdd;
#ifdef PARALLEL_CHANGE_LANES
    std::vector<const MSEdge*> recheckLaneUsage;
#endif
    MSGlobals::gComputeLC = true;
    for (const MSLane* const l : myActiveLanes) {
        if (myLanes[l->getNumericalID()].haveNeighbors) {
            const MSEdge& edge = l->getEdge();
            if (myLastLaneChange[edge.getNumericalID()] != t) {
                myLastLaneChange[edge.getNumericalID()] = t;
#ifdef PARALLEL_CHANGE_LANES
                if (MSGlobals::gNumSimThreads > 1) {
                    MSLane* lane = edge.getLanes()[0];
                    myThreadPool.add(lane->getLaneChangeTask(t), lane->getRNGIndex() % myThreadPool.size());
                    recheckLaneUsage.push_back(&edge);
                } else {
#endif
                    edge.changeLanes(t);
                    for (MSLane* const lane : edge.getLanes()) {
                        LaneUsage& lu = myLanes[lane->getNumericalID()];
                        //if ((*i)->getID() == "disabled") {
                        //    std::cout << SIMTIME << " vehicles=" << toString((*i)->getVehiclesSecure()) << "\n";
                        //    (*i)->releaseVehicles();
                        //}
                        if (lane->getVehicleNumber() > 0 && !lu.amActive) {
                            toAdd.push_back(lane);
                            lu.amActive = true;
                        }
                        if (MSGlobals::gLateralResolution > 0) {
                            lane->sortManeuverReservations();
                        }
                    }
#ifdef PARALLEL_CHANGE_LANES
                }
#endif
            }
        } else {
            break;
        }
    }

#ifdef PARALLEL_CHANGE_LANES
    if (MSGlobals::gNumSimThreads > 1) {
        myThreadPool.waitAll(false);
        for (const MSEdge* e : recheckLaneUsage) {
            for (MSLane* const l : e->getLanes()) {
                LaneUsage& lu = myLanes[l->getNumericalID()];
                if (l->getVehicleNumber() > 0 && !lu.amActive) {
                    toAdd.push_back(l);
                    lu.amActive = true;
                }
                if (MSGlobals::gLateralResolution > 0) {
                    l->sortManeuverReservations();
                }
            }
        }
    }
#endif

    MSGlobals::gComputeLC = false;
    for (std::vector<MSLane*>::iterator i = toAdd.begin(); i != toAdd.end(); ++i) {
        myActiveLanes.push_front(*i);
    }
}


void
MSEdgeControl::detectCollisions(SUMOTime timestep, const std::string& stage) {
    // Detections is made by the edge's lanes, therefore hand over.
    for (MSLane* lane : myActiveLanes) {
        if (lane->needsCollisionCheck()) {
            lane->detectCollisions(timestep, stage);
        }
    }
    if (myInactiveCheckCollisions.size() > 0) {
        for (MSLane* lane : myInactiveCheckCollisions.getContainer()) {
            lane->detectCollisions(timestep, stage);
        }
        myInactiveCheckCollisions.clear();
        myInactiveCheckCollisions.unlock();
    }
}


void
MSEdgeControl::gotActive(MSLane* l) {
    myChangedStateLanes.insert(l);
}

void
MSEdgeControl::checkCollisionForInactive(MSLane* l) {
    myInactiveCheckCollisions.insert(l);
}

void
MSEdgeControl::setAdditionalRestrictions() {
    for (MSEdge* e : myEdges) {
        e->inferEdgeType();
        const std::vector<MSLane*>& lanes = e->getLanes();
        for (std::vector<MSLane*>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
            (*j)->initRestrictions();
        }
    }
}

void
MSEdgeControl::setMesoTypes() {
    for (MSEdge* edge : myEdges) {
        edge->updateMesoType();
    }
}


/****************************************************************************/
