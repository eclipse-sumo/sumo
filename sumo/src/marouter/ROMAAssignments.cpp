/****************************************************************************/
/// @file    ROMAAssignments.cpp
/// @author  Yun-Pang Floetteroed
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Feb 2013
/// @version $Id$
///
// Assignment methods
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <algorithm>
#include <router/ROEdge.h>
#include <utils/vehicle/RouteCostCalculator.h>
#include <router/RONet.h>
#include <router/RORoute.h>
#include <utils/distribution/Distribution_Points.h>
#include <od/ODMatrix.h>
#include <utils/common/SUMOTime.h>
#include <utils/vehicle/SUMOAbstractRouter.h>
#include "ROMAEdge.h"
#include "ROMAAssignments.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
std::map<const ROEdge* const, SUMOReal> ROMAAssignments::myPenalties;


// ===========================================================================
// method definitions
// ===========================================================================

ROMAAssignments::ROMAAssignments(const SUMOTime begin, const SUMOTime end, const bool additiveTraffic,
                                 const SUMOReal adaptionFactor, RONet& net, ODMatrix& matrix,
                                 SUMOAbstractRouter<ROEdge, ROVehicle>& router)
    : myBegin(begin), myEnd(end), myAdditiveTraffic(additiveTraffic), myAdaptionFactor(adaptionFactor), myNet(net), myMatrix(matrix), myRouter(router) {
    myDefaultVehicle = new ROVehicle(SUMOVehicleParameter(), 0, net.getVehicleTypeSecure(DEFAULT_VTYPE_ID), &net);
}


ROMAAssignments::~ROMAAssignments() {
    delete myDefaultVehicle;
}

// based on the definitions in PTV-Validate and in the VISUM-Cologne network
SUMOReal
ROMAAssignments::getCapacity(const ROEdge* edge) {
    if (edge->getFunc() == ROEdge::ET_DISTRICT) {
        return 0;
    }
    const int roadClass = -edge->getPriority();
    // TODO: differ road class 1 from the unknown road class 1!!!
    if (edge->getLaneNo() == 0) {
        // TAZ have no cost
        return 0;
    } else if (roadClass == 0 || roadClass == 1)  {
        return edge->getLaneNo() * 2000.; //CR13 in table.py
    } else if (roadClass == 2 && edge->getSpeed() <= 11.) {
        return edge->getLaneNo() * 1333.33; //CR5 in table.py
    } else if (roadClass == 2 && edge->getSpeed() > 11. && edge->getSpeed() <= 16.) {
        return edge->getLaneNo() * 1500.; //CR3 in table.py
    } else if (roadClass == 2 && edge->getSpeed() > 16.) {
        return edge->getLaneNo() * 2000.; //CR13 in table.py
    } else if (roadClass == 3 && edge->getSpeed() <= 11.) {
        return edge->getLaneNo() * 800.; //CR5 in table.py
    } else if (roadClass == 3 && edge->getSpeed() > 11. && edge->getSpeed() <= 13.) {
        return edge->getLaneNo() * 875.; //CR5 in table.py
    } else if (roadClass == 3 && edge->getSpeed() > 13. && edge->getSpeed() <= 16.) {
        return edge->getLaneNo() * 1500.; //CR4 in table.py
    } else if (roadClass == 3 && edge->getSpeed() > 16.) {
        return edge->getLaneNo() * 1800.; //CR13 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() <= 5.) {
        return edge->getLaneNo() * 200.; //CR7 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 5. && edge->getSpeed() <= 7.) {
        return edge->getLaneNo() * 412.5; //CR7 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 7. && edge->getSpeed() <= 9.) {
        return edge->getLaneNo() * 600.; //CR6 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 9. && edge->getSpeed() <= 11.) {
        return edge->getLaneNo() * 800.; //CR5 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 11. && edge->getSpeed() <= 13.) {
        return edge->getLaneNo() * 1125.; //CR5 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 13. && edge->getSpeed() <= 16.) {
        return edge->getLaneNo() * 1583.; //CR4 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 16. && edge->getSpeed() <= 18.) {
        return edge->getLaneNo() * 1100.; //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 18. && edge->getSpeed() <= 22.) {
        return edge->getLaneNo() * 1200.; //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 22. && edge->getSpeed() <= 26.) {
        return edge->getLaneNo() * 1300.; //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 26.) {
        return edge->getLaneNo() * 1400.; //CR3 in table.py
    }
    return edge->getLaneNo() * 800.; //CR5 in table.py
}


// based on the definitions in PTV-Validate and in the VISUM-Cologne network
SUMOReal
ROMAAssignments::capacityConstraintFunction(const ROEdge* edge, const SUMOReal flow) const {
    if (edge->getFunc() == ROEdge::ET_DISTRICT) {
        return 0;
    }
    const int roadClass = -edge->getPriority();
    const SUMOReal capacity = getCapacity(edge);
    // TODO: differ road class 1 from the unknown road class 1!!!
    if (edge->getLaneNo() == 0) {
        // TAZ have no cost
        return 0;
    } else if (roadClass == 0 || roadClass == 1)  {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 1.3)) * 2.); //CR13 in table.py
    } else if (roadClass == 2 && edge->getSpeed() <= 11.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
    } else if (roadClass == 2 && edge->getSpeed() > 11. && edge->getSpeed() <= 16.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 1.)) * 2.); //CR3 in table.py
    } else if (roadClass == 2 && edge->getSpeed() > 16.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 1.3)) * 2.); //CR13 in table.py
    } else if (roadClass == 3 && edge->getSpeed() <= 11.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
    } else if (roadClass == 3 && edge->getSpeed() > 11. && edge->getSpeed() <= 13.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
    } else if (roadClass == 3 && edge->getSpeed() > 13. && edge->getSpeed() <= 16.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.7 * (flow / (capacity * 1.)) * 2.); //CR4 in table.py
    } else if (roadClass == 3 && edge->getSpeed() > 16.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 1.3)) * 2.); //CR13 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() <= 5.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 0.5)) * 3.); //CR7 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 5. && edge->getSpeed() <= 7.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 0.5)) * 3.); //CR7 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 7. && edge->getSpeed() <= 9.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 0.8)) * 3.); //CR6 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 9. && edge->getSpeed() <= 11.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 11. && edge->getSpeed() <= 13.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 13. && edge->getSpeed() <= 16.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.7 * (flow / (capacity * 1.)) * 2.); //CR4 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 16. && edge->getSpeed() <= 18.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 1.)) * 2.); //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 18. && edge->getSpeed() <= 22.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 1.)) * 2.); //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 22. && edge->getSpeed() <= 26.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 1.)) * 2.); //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 26.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 1.)) * 2.); //CR3 in table.py
    }
    return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
}


bool
ROMAAssignments::addRoute(ConstROEdgeVector& edges, std::vector<RORoute*>& paths, std::string routeId, SUMOReal prob) {
    std::vector<RORoute*>::iterator p;
    for (p = paths.begin(); p != paths.end(); p++) {
        if (edges == (*p)->getEdgeVector()) {
            break;
        }
    }
    if (p == paths.end()) {
        paths.push_back(new RORoute(routeId, 0., prob, edges, 0, std::vector<SUMOVehicleParameter::Stop>()));
        return true;
    }
    (*p)->addProbability(prob);
    std::iter_swap(paths.end() - 1, p);
    return false;
}


void
ROMAAssignments::getKPaths(const int kPaths, const SUMOReal penalty) {
    for (std::vector<ODCell*>::const_iterator i = myMatrix.getCells().begin(); i != myMatrix.getCells().end(); ++i) {
        ODCell* c = *i;
        myPenalties.clear();
        for (int k = 0; k < kPaths; k++) {
            ConstROEdgeVector edges;
            myRouter.compute(myNet.getEdge(c->origin + "-source"), myNet.getEdge(c->destination + "-sink"), myDefaultVehicle, 0, edges);
            for (ConstROEdgeVector::iterator e = edges.begin(); e != edges.end(); e++) {
                myPenalties[*e] = penalty;
            }
            addRoute(edges, c->pathsVector, c->origin + c->destination + toString(c->pathsVector.size()), 0);
        }
    }
    myPenalties.clear();
}


void
ROMAAssignments::resetFlows() {
    const SUMOReal begin = STEPS2TIME(MIN2(myBegin, myMatrix.getCells().front()->begin));
    for (std::map<std::string, ROEdge*>::const_iterator i = myNet.getEdgeMap().begin(); i != myNet.getEdgeMap().end(); ++i) {
        ROMAEdge* edge = static_cast<ROMAEdge*>(i->second);
        edge->setFlow(begin, STEPS2TIME(myEnd), 0.);
        edge->setHelpFlow(begin, STEPS2TIME(myEnd), 0.);
    }
}


void
ROMAAssignments::incremental(const int numIter, const bool verbose) {
    SUMOTime lastBegin = -1;
    std::vector<int> intervals;
    int count = 0;
    for (std::vector<ODCell*>::const_iterator i = myMatrix.getCells().begin(); i != myMatrix.getCells().end(); ++i) {
        if ((*i)->begin != lastBegin) {
            intervals.push_back(count);
            lastBegin = (*i)->begin;
        }
        count++;
    }
    lastBegin = -1;
    for (std::vector<int>::const_iterator offset = intervals.begin(); offset != intervals.end(); offset++) {
        std::vector<ODCell*>::const_iterator cellsEnd = myMatrix.getCells().end();
        if (offset != intervals.end() - 1) {
            cellsEnd = myMatrix.getCells().begin() + (*(offset + 1));
        }
        const SUMOTime intervalStart = (*(myMatrix.getCells().begin() + (*offset)))->begin;
        if (verbose) {
            WRITE_MESSAGE(" starting interval " + time2string(intervalStart));
        }
        std::map<const ROMAEdge*, SUMOReal> loadedTravelTimes;
        for (std::map<std::string, ROEdge*>::const_iterator i = myNet.getEdgeMap().begin(); i != myNet.getEdgeMap().end(); ++i) {
            ROMAEdge* edge = static_cast<ROMAEdge*>(i->second);
            if (edge->hasLoadedTravelTime(STEPS2TIME(intervalStart))) {
                loadedTravelTimes[edge] = edge->getTravelTime(myDefaultVehicle, STEPS2TIME(intervalStart));
            }
        }
        for (int t = 0; t < numIter; t++) {
            if (verbose) {
                WRITE_MESSAGE("  starting iteration " + toString(t));
            }
            std::string lastOrigin = "";
            int workerIndex = 0;
            for (std::vector<ODCell*>::const_iterator i = myMatrix.getCells().begin() + (*offset); i != cellsEnd; i++) {
                ODCell* const c = *i;
                const SUMOReal linkFlow = c->vehicleNumber / numIter;
                const SUMOTime begin = myAdditiveTraffic ? myBegin : c->begin;
#ifdef HAVE_FOX
                if (myNet.getThreadPool().size() > 0) {
                    if (lastOrigin != c->origin) {
                        workerIndex++;
                        if (workerIndex == myNet.getThreadPool().size()) {
                            workerIndex = 0;
                        }
                        myNet.getThreadPool().add(new RONet::BulkmodeTask(false), workerIndex);
                        lastOrigin = c->origin;
                        myNet.getThreadPool().add(new RoutingTask(*this, c, begin, linkFlow), workerIndex);
                        myNet.getThreadPool().add(new RONet::BulkmodeTask(true), workerIndex);
                    } else {
                        myNet.getThreadPool().add(new RoutingTask(*this, c, begin, linkFlow), workerIndex);
                    }
                    continue;
                }
#endif
                if (lastOrigin != c->origin) {
                    myRouter.setBulkMode(false);
                    lastOrigin = c->origin;
                }
                ConstROEdgeVector edges;
                myRouter.compute(myNet.getEdge(c->origin + "-source"), myNet.getEdge(c->destination + "-sink"), myDefaultVehicle, begin, edges);
                myRouter.setBulkMode(true);
                addRoute(edges, c->pathsVector, c->origin + c->destination + toString(c->pathsVector.size()), linkFlow);
            }
#ifdef HAVE_FOX
            if (myNet.getThreadPool().size() > 0) {
                myNet.getThreadPool().waitAll();
            }
#endif
            for (std::vector<ODCell*>::const_iterator i = myMatrix.getCells().begin() + (*offset); i != cellsEnd; i++) {
                ODCell* const c = *i;
                const SUMOReal linkFlow = c->vehicleNumber / numIter;
                const SUMOTime begin = myAdditiveTraffic ? myBegin : c->begin;
                const SUMOTime end = myAdditiveTraffic ? myEnd : c->end;
                const SUMOReal intervalLengthInHours = STEPS2TIME(end - begin) / 3600.;
                const ConstROEdgeVector& edges = c->pathsVector.back()->getEdgeVector();
                for (ConstROEdgeVector::const_iterator e = edges.begin(); e != edges.end(); e++) {
                    ROMAEdge* edge = static_cast<ROMAEdge*>(myNet.getEdge((*e)->getID()));
                    const SUMOReal newFlow = edge->getFlow(STEPS2TIME(begin)) + linkFlow;
                    edge->setFlow(STEPS2TIME(begin), STEPS2TIME(end), newFlow);
                    SUMOReal travelTime = capacityConstraintFunction(edge, newFlow / intervalLengthInHours);
                    if (lastBegin >= 0 && myAdaptionFactor > 0.) {
                        if (loadedTravelTimes.count(edge) != 0) {
                            travelTime = loadedTravelTimes[edge] * myAdaptionFactor + (1. - myAdaptionFactor) * travelTime;
                        } else {
                            travelTime = edge->getTravelTime(myDefaultVehicle, STEPS2TIME(lastBegin)) * myAdaptionFactor + (1. - myAdaptionFactor) * travelTime;
                        }
                    }
                    edge->addTravelTime(travelTime, STEPS2TIME(begin), STEPS2TIME(end));
                }
            }
        }
        lastBegin = intervalStart;
    }
}


void
ROMAAssignments::sue(const int maxOuterIteration, const int maxInnerIteration, const int kPaths, const SUMOReal penalty, const SUMOReal tolerance, const std::string /* routeChoiceMethod */) {
    getKPaths(kPaths, penalty);
    std::map<const SUMOReal, SUMOReal> intervals;
    if (myAdditiveTraffic) {
        intervals[STEPS2TIME(myBegin)] = STEPS2TIME(myEnd);
    } else {
        for (std::vector<ODCell*>::const_iterator i = myMatrix.getCells().begin(); i != myMatrix.getCells().end(); ++i) {
            intervals[STEPS2TIME((*i)->begin)] = STEPS2TIME((*i)->end);
        }
    }
    for (int outer = 0; outer < maxOuterIteration; outer++) {
        for (int inner = 0; inner < maxInnerIteration; inner++) {
            for (std::vector<ODCell*>::const_iterator i = myMatrix.getCells().begin(); i != myMatrix.getCells().end(); ++i) {
                ODCell* const c = *i;
                const SUMOTime begin = myAdditiveTraffic ? myBegin : c->begin;
                const SUMOTime end = myAdditiveTraffic ? myEnd : c->end;
                // update path cost
                for (std::vector<RORoute*>::const_iterator j = c->pathsVector.begin(); j != c->pathsVector.end(); ++j) {
                    RORoute* r = *j;
                    r->setCosts(myRouter.recomputeCosts(r->getEdgeVector(), myDefaultVehicle, 0));
//                    std::cout << std::setprecision(20) << r->getID() << ":" << r->getCosts() << std::endl;
                }
                // calculate route utilities and probabilities
                RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().calculateProbabilities(c->pathsVector, myDefaultVehicle, 0);
                // calculate route flows
                for (std::vector<RORoute*>::const_iterator j = c->pathsVector.begin(); j != c->pathsVector.end(); ++j) {
                    RORoute* r = *j;
                    const SUMOReal pathFlow = r->getProbability() * c->vehicleNumber;
                    // assign edge flow deltas
                    for (ConstROEdgeVector::const_iterator e = r->getEdgeVector().begin(); e != r->getEdgeVector().end(); e++) {
                        ROMAEdge* edge = static_cast<ROMAEdge*>(myNet.getEdge((*e)->getID()));
                        edge->setHelpFlow(STEPS2TIME(begin), STEPS2TIME(end), edge->getHelpFlow(STEPS2TIME(begin)) + pathFlow);
                    }
                }
            }
            // calculate new edge flows and check for stability
            int unstableEdges = 0;
            for (std::map<const SUMOReal, SUMOReal>::const_iterator i = intervals.begin(); i != intervals.end(); ++i) {
                const SUMOReal intervalLengthInHours = STEPS2TIME(i->second - i->first) / 3600.;
                for (std::map<std::string, ROEdge*>::const_iterator e = myNet.getEdgeMap().begin(); e != myNet.getEdgeMap().end(); ++e) {
                    ROMAEdge* edge = static_cast<ROMAEdge*>(e->second);
                    const SUMOReal oldFlow = edge->getFlow(i->first);
                    SUMOReal newFlow = oldFlow;
                    if (inner == 0 && outer == 0) {
                        newFlow += edge->getHelpFlow(i->first);
                    } else {
                        newFlow += (edge->getHelpFlow(i->first) - oldFlow) / (inner + 1);
                    }
                    //                if not lohse:
                    if (newFlow > 0.) {
                        if (fabs(newFlow - oldFlow) / newFlow > tolerance) {
                            unstableEdges++;
                        }
                    } else if (newFlow == 0.) {
                        if (oldFlow != 0. && (fabs(newFlow - oldFlow) / oldFlow > tolerance)) {
                            unstableEdges++;
                        }
                    } else { // newFlow < 0.
                        unstableEdges++;
                        newFlow = 0.;
                    }
                    edge->setFlow(i->first, i->second, newFlow);
                    const SUMOReal travelTime = capacityConstraintFunction(edge, newFlow / intervalLengthInHours);
                    edge->addTravelTime(travelTime, i->first, i->second);
                    edge->setHelpFlow(i->first, i->second, 0.);
                }
            }
            // if stable break
            if (unstableEdges == 0) {
                break;
            }
            // additional stability check from python script: if notstable < math.ceil(net.geteffEdgeCounts()*0.005) or notstable < 3: stable = True
        }
        // check for a new route, if none available, break
        // several modifications about when a route is new and when to break are in the original script
        bool newRoute = false;
        for (std::vector<ODCell*>::const_iterator i = myMatrix.getCells().begin(); i != myMatrix.getCells().end(); ++i) {
            ODCell* c = *i;
            ConstROEdgeVector edges;
            myRouter.compute(myNet.getEdge(c->origin + "-source"), myNet.getEdge(c->destination + "-sink"), myDefaultVehicle, 0, edges);
            newRoute |= addRoute(edges, c->pathsVector, c->origin + c->destination + toString(c->pathsVector.size()), 0);
        }
        if (!newRoute) {
            break;
        }
    }
    // final round of assignment
    for (std::vector<ODCell*>::const_iterator i = myMatrix.getCells().begin(); i != myMatrix.getCells().end(); ++i) {
        ODCell* c = *i;
        // update path cost
        for (std::vector<RORoute*>::const_iterator j = c->pathsVector.begin(); j != c->pathsVector.end(); ++j) {
            RORoute* r = *j;
            r->setCosts(myRouter.recomputeCosts(r->getEdgeVector(), myDefaultVehicle, 0));
        }
        // calculate route utilities and probabilities
        RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().calculateProbabilities(c->pathsVector, myDefaultVehicle, 0);
        // calculate route flows
        for (std::vector<RORoute*>::const_iterator j = c->pathsVector.begin(); j != c->pathsVector.end(); ++j) {
            RORoute* r = *j;
            r->setProbability(r->getProbability() * c->vehicleNumber);
        }
    }
}


SUMOReal
ROMAAssignments::getPenalizedEffort(const ROEdge* const e, const ROVehicle* const v, SUMOReal t) {
    const std::map<const ROEdge* const, SUMOReal>::const_iterator i = myPenalties.find(e);
    return i == myPenalties.end() ? e->getEffort(v, t) : e->getEffort(v, t) + i->second;
}


SUMOReal
ROMAAssignments::getPenalizedTT(const ROEdge* const e, const ROVehicle* const v, SUMOReal t) {
    const std::map<const ROEdge* const, SUMOReal>::const_iterator i = myPenalties.find(e);
    return i == myPenalties.end() ? e->getTravelTime(v, t) : e->getTravelTime(v, t) + i->second;
}


SUMOReal
ROMAAssignments::getTravelTime(const ROEdge* const e, const ROVehicle* const v, SUMOReal t) {
    return e->getTravelTime(v, t);
}


#ifdef HAVE_FOX
// ---------------------------------------------------------------------------
// ROMAAssignments::RoutingTask-methods
// ---------------------------------------------------------------------------
void
ROMAAssignments::RoutingTask::run(FXWorkerThread* context) {
    ConstROEdgeVector edges;
    static_cast<RONet::WorkerThread*>(context)->getVehicleRouter().compute(myAssign.myNet.getEdge(myCell->origin + "-source"), myAssign.myNet.getEdge(myCell->destination + "-sink"), myAssign.myDefaultVehicle, myBegin, edges);
    myAssign.addRoute(edges, myCell->pathsVector, myCell->origin + myCell->destination + toString(myCell->pathsVector.size()), myLinkFlow);
}
#endif
