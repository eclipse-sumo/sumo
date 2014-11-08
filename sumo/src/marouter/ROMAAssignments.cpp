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
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <router/ROEdge.h>
#include <utils/vehicle/RouteCostCalculator.h>
#include <router/RONet.h>
#include <router/RORoute.h>
#include <utils/distribution/Distribution_Points.h>
#include <od2trips/ODMatrix.h>
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
ROVehicle* ROMAAssignments::myDefaultVehicle = 0;


// ===========================================================================
// method definitions
// ===========================================================================

ROMAAssignments::ROMAAssignments(const SUMOTime begin, const SUMOTime end,
                                 RONet& net, ODMatrix& matrix,
                                 SUMOAbstractRouter<ROEdge, ROVehicle>& router) : myBegin(begin), myEnd(end), myNet(net), myMatrix(matrix), myRouter(router) {
    myDefaultVehicle = new ROVehicle(SUMOVehicleParameter(), 0, net.getVehicleTypeSecure(DEFAULT_VTYPE_ID), &net);
}


ROMAAssignments::~ROMAAssignments() {
    delete myDefaultVehicle;
}

// based on the definitions in PTV-Validate and in the VISUM-Köln network
SUMOReal
ROMAAssignments::capacityConstraintFunction(const ROEdge* edge, const SUMOReal flow) const {
    if (edge->getType() == ROEdge::ET_DISTRICT) {
        return 0;
    }
    const int roadClass = -edge->getPriority();
    // TODO: differ road class 1 from the unknown road class 1!!!
    if (roadClass == 0 || roadClass == 1)  {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 2000.*1.3)) * 2.); //CR13 in table.py
    } else if (roadClass == 2 && edge->getSpeed() <= 11.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 1333.33 * 0.9)) * 3.); //CR5 in table.py
    } else if (roadClass == 2 && edge->getSpeed() > 11. && edge->getSpeed() <= 16.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 1500.*1.)) * 2.); //CR3 in table.py
    } else if (roadClass == 2 && edge->getSpeed() > 16.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 2000.*1.3)) * 2.); //CR13 in table.py
    } else if (roadClass == 3 && edge->getSpeed() <= 11.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 800.*0.9)) * 3.); //CR5 in table.py
    } else if (roadClass == 3 && edge->getSpeed() > 11. && edge->getSpeed() <= 13.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 875.*0.9)) * 3.); //CR5 in table.py
    } else if (roadClass == 3 && edge->getSpeed() > 13. && edge->getSpeed() <= 16.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.7 * (flow / (edge->getLaneNo() * 1500.*1.)) * 2.); //CR4 in table.py
    } else if (roadClass == 3 && edge->getSpeed() > 16.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 1800.*1.3)) * 2.); //CR13 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() <= 5.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 200.*0.5)) * 3.); //CR7 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 5. && edge->getSpeed() <= 7.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 412.5 * 0.5)) * 3.); //CR7 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 7. && edge->getSpeed() <= 9.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 600.*0.8)) * 3.); //CR6 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 9. && edge->getSpeed() <= 11.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 800.*0.9)) * 3.); //CR5 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 11. && edge->getSpeed() <= 13.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 1125.*0.9)) * 3.); //CR5 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 13. && edge->getSpeed() <= 16.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.7 * (flow / (edge->getLaneNo() * 1583.*1.)) * 2.); //CR4 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 16. && edge->getSpeed() <= 18.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 1100.*1.)) * 2.); //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 18. && edge->getSpeed() <= 22.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 1200.*1.)) * 2.); //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 22. && edge->getSpeed() <= 26.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 1300.*1.)) * 2.); //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeed() > 26.) {
        return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 1400.*1.)) * 2.); //CR3 in table.py
    }
    return edge->getLength() / edge->getSpeed() * (1. + 1.*(flow / (edge->getLaneNo() * 800.*0.9)) * 3.); //CR5 in table.py
}


bool
ROMAAssignments::addRoute(std::vector<const ROEdge*>& edges, std::vector<RORoute*>& paths, std::string routeId, SUMOReal costs, SUMOReal prob) {
    RORoute* dup = 0;
    for (std::vector<RORoute*>::const_iterator p = paths.begin(); p != paths.end(); p++) {
        if (edges == (*p)->getEdgeVector()) {
            dup = *p;
            break;
        }
    }
    if (dup == 0) {
        paths.push_back(new RORoute(routeId, costs, prob, edges, 0, std::vector<SUMOVehicleParameter::Stop>()));
        return true;
    }
    dup->addProbability(prob);
    return false;
}


void
ROMAAssignments::getKPaths(const int kPaths, const SUMOReal penalty) {
    for (std::vector<ODCell*>::const_iterator i = myMatrix.getCells().begin(); i != myMatrix.getCells().end(); ++i) {
        ODCell* c = *i;
        myPenalties.clear();
        for (int k = 0; k < kPaths; k++) {
            std::vector<const ROEdge*> edges;
            myRouter.compute(myNet.getEdge(c->origin + "-source"), myNet.getEdge(c->destination + "-sink"), myDefaultVehicle, 0, edges);
            for (std::vector<const ROEdge*>::iterator e = edges.begin(); e != edges.end(); e++) {
                myPenalties[*e] = penalty;
            }
            addRoute(edges, c->pathsVector, c->origin + c->destination + toString(c->pathsVector.size()), 0, 0);
        }
    }
    myPenalties.clear();
}


void
ROMAAssignments::incremental(const int numIter) {
    for (int t = 0; t < numIter; t++) {
        for (std::vector<ODCell*>::const_iterator i = myMatrix.getCells().begin(); i != myMatrix.getCells().end(); i++) {
            ODCell* c = *i;
            std::vector<const ROEdge*> edges;
            SUMOReal linkFlow = c->vehicleNumber / numIter;
            myRouter.compute(myNet.getEdge(c->origin + "-source"), myNet.getEdge(c->destination + "-sink"), myDefaultVehicle, 0, edges);
            SUMOReal costs = 0.;
            for (std::vector<const ROEdge*>::iterator e = edges.begin(); e != edges.end(); e++) {
                ROEdge* edge = myNet.getEdge((*e)->getID());
                edge->addEffort(linkFlow, STEPS2TIME(myBegin), STEPS2TIME(myEnd));
                const SUMOReal travelTime = capacityConstraintFunction(edge, linkFlow);
                edge->addTravelTime(travelTime, STEPS2TIME(myBegin), STEPS2TIME(myEnd));
                costs += travelTime;
            }
            addRoute(edges, c->pathsVector, c->origin + c->destination + toString(c->pathsVector.size()), costs, linkFlow);
        }
    }
}


void
ROMAAssignments::sue(const int maxOuterIteration, const int maxInnerIteration, const int kPaths, const SUMOReal penalty, const SUMOReal tolerance, const std::string /* routeChoiceMethod */) {
    getKPaths(kPaths, penalty);
    for (int outer = 0; outer < maxOuterIteration; outer++) {
        for (int inner = 0; inner < maxInnerIteration; inner++) {
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
                    const SUMOReal pathFlow = r->getProbability() * c->vehicleNumber;
                    // assign edge flow deltas
                    for (std::vector<const ROEdge*>::const_iterator e = r->getEdgeVector().begin(); e != r->getEdgeVector().end(); e++) {
                        ROMAEdge* edge = static_cast<ROMAEdge*>(myNet.getEdge((*e)->getID()));
                        edge->setHelpFlow(edge->getHelpFlow() + pathFlow);
                    }
                }
            }
            // calculate new edge flows and check for stability
            int unstableEdges = 0;
            for (std::map<std::string, ROEdge*>::const_iterator i = myNet.getEdgeMap().begin(); i != myNet.getEdgeMap().end(); ++i) {
                ROMAEdge* edge = static_cast<ROMAEdge*>((*i).second);
                const SUMOReal oldFlow = edge->getEffort(myDefaultVehicle, 0.);
                SUMOReal newFlow = oldFlow;
                if (inner == 0 && outer == 0) {
                    newFlow += edge->getHelpFlow();
                } else {
                    newFlow += (edge->getHelpFlow() - oldFlow) / (inner + 1);
                }
//                if not lohse:
                if (newFlow > 0.) {
                    if (abs(newFlow - oldFlow) / newFlow > tolerance) {
                        unstableEdges++;
                    }
                } else if (newFlow == 0.) {
                    if (oldFlow != 0. && (abs(newFlow - oldFlow) / oldFlow > tolerance)) {
                        unstableEdges++;
                    }
                } else { // newFlow < 0.
                    unstableEdges++;
                    newFlow = 0.;
                }
                edge->addEffort(newFlow, STEPS2TIME(myBegin), STEPS2TIME(myEnd));
                const SUMOReal travelTime = capacityConstraintFunction(edge, newFlow);
                edge->addTravelTime(travelTime, STEPS2TIME(myBegin), STEPS2TIME(myEnd));
                edge->setHelpFlow(0.);
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
            std::vector<const ROEdge*> edges;
            myRouter.compute(myNet.getEdge(c->origin + "-source"), myNet.getEdge(c->destination + "-sink"), myDefaultVehicle, 0, edges);
            newRoute |= addRoute(edges, c->pathsVector, c->origin + c->destination + toString(c->pathsVector.size()), 0, 0);
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
