/****************************************************************************/
/// @file    NBOwnTLDef.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// A traffic light logics which must be computed (only nodes/edges are given)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#include <cassert>
#include "NBTrafficLightDefinition.h"
#include "NBNode.h"
#include "NBOwnTLDef.h"
#include "NBTrafficLightLogic.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
NBOwnTLDef::NBOwnTLDef(const std::string& id,
                       const std::vector<NBNode*>& junctions, SUMOTime offset,
                       TrafficLightType type) :
    NBTrafficLightDefinition(id, junctions, DefaultProgramID, offset, type),
    myHaveSinglePhase(false)
{}


NBOwnTLDef::NBOwnTLDef(const std::string& id, NBNode* junction, SUMOTime offset,
                       TrafficLightType type) :
    NBTrafficLightDefinition(id, junction, DefaultProgramID, offset, type),
    myHaveSinglePhase(false)
{}


NBOwnTLDef::NBOwnTLDef(const std::string& id, SUMOTime offset,
                       TrafficLightType type) :
    NBTrafficLightDefinition(id, DefaultProgramID, offset, type),
    myHaveSinglePhase(false)
{}


NBOwnTLDef::~NBOwnTLDef() {}


int
NBOwnTLDef::getToPrio(const NBEdge* const e) {
    return e->getJunctionPriority(e->getToNode());
}


SUMOReal
NBOwnTLDef::getDirectionalWeight(LinkDirection dir) {
    switch (dir) {
        case LINKDIR_STRAIGHT:
        case LINKDIR_PARTLEFT:
        case LINKDIR_PARTRIGHT:
            return 2.;
        case LINKDIR_LEFT:
        case LINKDIR_RIGHT:
            return .5;
        case LINKDIR_NODIR:
        case LINKDIR_TURN:
            return 0;
    }
    return 0;
}

SUMOReal
NBOwnTLDef::computeUnblockedWeightedStreamNumber(const NBEdge* const e1, const NBEdge* const e2) {
    SUMOReal val = 0;
    for (unsigned int e1l = 0; e1l < e1->getNumLanes(); e1l++) {
        std::vector<NBEdge::Connection> approached1 = e1->getConnectionsFromLane(e1l);
        for (unsigned int e2l = 0; e2l < e2->getNumLanes(); e2l++) {
            std::vector<NBEdge::Connection> approached2 = e2->getConnectionsFromLane(e2l);
            for (std::vector<NBEdge::Connection>::iterator e1c = approached1.begin(); e1c != approached1.end(); ++e1c) {
                if (e1->getTurnDestination() == (*e1c).toEdge) {
                    continue;
                }
                for (std::vector<NBEdge::Connection>::iterator e2c = approached2.begin(); e2c != approached2.end(); ++e2c) {
                    if (e2->getTurnDestination() == (*e2c).toEdge) {
                        continue;
                    }
                    if (!foes(e1, (*e1c).toEdge, e2, (*e2c).toEdge)) {
                        val += getDirectionalWeight(e1->getToNode()->getDirection(e1, (*e1c).toEdge));
                        val += getDirectionalWeight(e2->getToNode()->getDirection(e2, (*e2c).toEdge));
                    }
                }
            }
        }
    }
    return val;
}


std::pair<NBEdge*, NBEdge*>
NBOwnTLDef::getBestCombination(const EdgeVector& edges) {
    std::pair<NBEdge*, NBEdge*> bestPair(static_cast<NBEdge*>(0), static_cast<NBEdge*>(0));
    SUMOReal bestValue = -1;
    for (EdgeVector::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        for (EdgeVector::const_iterator j = i + 1; j != edges.end(); ++j) {
            const SUMOReal value = computeUnblockedWeightedStreamNumber(*i, *j);
            if (value > bestValue) {
                bestValue = value;
                bestPair = std::pair<NBEdge*, NBEdge*>(*i, *j);
            } else if (value == bestValue) {
                const SUMOReal ca = GeomHelper::getMinAngleDiff((*i)->getAngleAtNode((*i)->getToNode()), (*j)->getAngleAtNode((*j)->getToNode()));
                const SUMOReal oa = GeomHelper::getMinAngleDiff(bestPair.first->getAngleAtNode(bestPair.first->getToNode()), bestPair.second->getAngleAtNode(bestPair.second->getToNode()));
                if (fabs(oa - ca) < NUMERICAL_EPS) { // break ties by id
                    if (bestPair.first->getID() < (*i)->getID()) {
                        bestPair = std::pair<NBEdge*, NBEdge*>(*i, *j);
                    }
                } else if (oa < ca) {
                    bestPair = std::pair<NBEdge*, NBEdge*>(*i, *j);
                }
            }
        }
    }
    return bestPair;
}


std::pair<NBEdge*, NBEdge*>
NBOwnTLDef::getBestPair(EdgeVector& incoming) {
    if (incoming.size() == 1) {
        // only one there - return the one
        std::pair<NBEdge*, NBEdge*> ret(*incoming.begin(), static_cast<NBEdge*>(0));
        incoming.clear();
        return ret;
    }
    // determine the best combination
    //  by priority, first
    EdgeVector used;
    std::sort(incoming.begin(), incoming.end(), edge_by_incoming_priority_sorter());
    used.push_back(*incoming.begin()); // the first will definitely be used
    // get the ones with the same priority
    int prio = getToPrio(*used.begin());
    for (EdgeVector::iterator i = incoming.begin() + 1; i != incoming.end() && prio != getToPrio(*i); ++i) {
        used.push_back(*i);
    }
    //  if there only lower priorised, use these, too
    if (used.size() < 2) {
        used = incoming;
    }
    std::pair<NBEdge*, NBEdge*> ret = getBestCombination(used);
    incoming.erase(find(incoming.begin(), incoming.end(), ret.first));
    incoming.erase(find(incoming.begin(), incoming.end(), ret.second));
    return ret;
}


NBTrafficLightLogic*
NBOwnTLDef::myCompute(const NBEdgeCont&,
                      unsigned int brakingTimeSeconds) {
    const SUMOTime brakingTime = TIME2STEPS(brakingTimeSeconds);
    const SUMOTime leftTurnTime = TIME2STEPS(6); // make configurable ?
    // build complete lists first
    const EdgeVector& incoming = getIncomingEdges();
    EdgeVector fromEdges, toEdges;
    std::vector<bool> isLeftMoverV, isTurnaround;
    unsigned int noLanesAll = 0;
    unsigned int noLinksAll = 0;
    for (unsigned int i1 = 0; i1 < incoming.size(); i1++) {
        unsigned int noLanes = incoming[i1]->getNumLanes();
        noLanesAll += noLanes;
        for (unsigned int i2 = 0; i2 < noLanes; i2++) {
            NBEdge* fromEdge = incoming[i1];
            std::vector<NBEdge::Connection> approached = fromEdge->getConnectionsFromLane(i2);
            noLinksAll += (unsigned int) approached.size();
            for (unsigned int i3 = 0; i3 < approached.size(); i3++) {
                if (!fromEdge->mayBeTLSControlled(i2, approached[i3].toEdge, approached[i3].toLane)) {
                    --noLinksAll;
                    continue;
                }
                assert(i3 < approached.size());
                NBEdge* toEdge = approached[i3].toEdge;
                fromEdges.push_back(fromEdge);
                //myFromLanes.push_back(i2);
                toEdges.push_back(toEdge);
                if (toEdge != 0) {
                    isLeftMoverV.push_back(
                        isLeftMover(fromEdge, toEdge)
                        ||
                        fromEdge->isTurningDirectionAt(fromEdge->getToNode(), toEdge));

                    isTurnaround.push_back(
                        fromEdge->isTurningDirectionAt(
                            fromEdge->getToNode(), toEdge));
                } else {
                    isLeftMoverV.push_back(true);
                    isTurnaround.push_back(true);
                }
            }
        }
    }

    NBTrafficLightLogic* logic = new NBTrafficLightLogic(getID(), getProgramID(), noLinksAll, myOffset, myType);
    EdgeVector toProc = incoming;
    const SUMOTime greenTime = TIME2STEPS(OptionsCont::getOptions().getInt("tls.green.time"));
    // build all phases
    while (toProc.size() > 0) {
        std::pair<NBEdge*, NBEdge*> chosen;
        if (incoming.size() == 2) {
            chosen = std::pair<NBEdge*, NBEdge*>(toProc[0], static_cast<NBEdge*>(0));
            toProc.erase(toProc.begin());
        } else {
            chosen = getBestPair(toProc);
        }
        unsigned int pos = 0;
        std::string state((size_t) noLinksAll, 'o');
        // plain straight movers
        for (unsigned int i1 = 0; i1 < (unsigned int) incoming.size(); ++i1) {
            NBEdge* fromEdge = incoming[i1];
            const bool inChosen = fromEdge == chosen.first || fromEdge == chosen.second; //chosen.find(fromEdge)!=chosen.end();
            const unsigned int numLanes = fromEdge->getNumLanes();
            for (unsigned int i2 = 0; i2 < numLanes; i2++) {
                std::vector<NBEdge::Connection> approached = fromEdge->getConnectionsFromLane(i2);
                for (unsigned int i3 = 0; i3 < approached.size(); ++i3) {
                    if (!fromEdge->mayBeTLSControlled(i2, approached[i3].toEdge, approached[i3].toLane)) {
                        continue;
                    }
                    if (inChosen) {
                        state[pos] = 'G';
                    } else {
                        state[pos] = 'r';
                    }
                    ++pos;
                }
            }
        }
        // correct behaviour for those that are not in chosen, but may drive, though
        for (unsigned int i1 = 0; i1 < pos; ++i1) {
            if (state[i1] == 'G') {
                continue;
            }
            bool isForbidden = false;
            for (unsigned int i2 = 0; i2 < pos && !isForbidden; ++i2) {
                if (state[i2] == 'G' && !isTurnaround[i2] &&
                        (forbids(fromEdges[i2], toEdges[i2], fromEdges[i1], toEdges[i1], true) || forbids(fromEdges[i1], toEdges[i1], fromEdges[i2], toEdges[i2], true))) {
                    isForbidden = true;
                }
            }
            if (!isForbidden) {
                state[i1] = 'G';
            }
        }
        // correct behaviour for those that have to wait (mainly left-mover)
        bool haveForbiddenLeftMover = false;
        for (unsigned int i1 = 0; i1 < pos; ++i1) {
            if (state[i1] != 'G') {
                continue;
            }
            for (unsigned int i2 = 0; i2 < pos; ++i2) {
                if ((state[i2] == 'G' || state[i2] == 'g') && forbids(fromEdges[i2], toEdges[i2], fromEdges[i1], toEdges[i1], true)) {
                    state[i1] = 'g';
                    if (!isTurnaround[i1]) {
                        haveForbiddenLeftMover = true;
                    }
                }
            }
        }
        // add step
        logic->addStep(greenTime, state);

        if (brakingTime > 0) {
            // build yellow (straight)
            for (unsigned int i1 = 0; i1 < pos; ++i1) {
                if (state[i1] != 'G' && state[i1] != 'g') {
                    continue;
                }
                if ((state[i1] >= 'a' && state[i1] <= 'z') && haveForbiddenLeftMover) {
                    continue;
                }
                state[i1] = 'y';
            }
            // add step
            logic->addStep(brakingTime, state);
        }

        if (haveForbiddenLeftMover && !myHaveSinglePhase) {
            // build left green
            for (unsigned int i1 = 0; i1 < pos; ++i1) {
                if (state[i1] == 'Y' || state[i1] == 'y') {
                    state[i1] = 'r';
                    continue;
                }
                if (state[i1] == 'g') {
                    state[i1] = 'G';
                }
            }
            // add step
            logic->addStep(leftTurnTime, state);

            // build left yellow
            if (brakingTime > 0) {
                for (unsigned int i1 = 0; i1 < pos; ++i1) {
                    if (state[i1] != 'G' && state[i1] != 'g') {
                        continue;
                    }
                    state[i1] = 'y';
                }
                // add step
                logic->addStep(brakingTime, state);
            }
        }
    }
    const SUMOTime totalDuration = logic->getDuration();
    if (totalDuration > 0) {
        if (totalDuration > 3 * (greenTime + 2 * brakingTime + leftTurnTime)) {
            WRITE_WARNING("The traffic light '" + getID() + "' has a high cycle time of " + time2string(totalDuration) + ".");
        }
        return logic;
    } else {
        delete logic;
        return 0;
    }
}


void
NBOwnTLDef::collectNodes() {}


void
NBOwnTLDef::collectLinks() {
    collectAllLinks();
}


void
NBOwnTLDef::setParticipantsInformation() {
    // assign participating nodes to the request
    collectNodes();
    // collect the information about participating edges and links
    collectEdges();
    collectLinks();
}


void
NBOwnTLDef::setTLControllingInformation(const NBEdgeCont&) const {
    // set the information about the link's positions within the tl into the
    //  edges the links are starting at, respectively
    for (NBConnectionVector::const_iterator j = myControlledLinks.begin(); j != myControlledLinks.end(); ++j) {
        const NBConnection& conn = *j;
        NBEdge* edge = conn.getFrom();
        edge->setControllingTLInformation(conn, getID());
    }
}


void
NBOwnTLDef::remapRemoved(NBEdge* /*removed*/, const EdgeVector& /*incoming*/,
                         const EdgeVector& /*outgoing*/) {}


void
NBOwnTLDef::replaceRemoved(NBEdge* /*removed*/, int /*removedLane*/,
                           NBEdge* /*by*/, int /*byLane*/) {}



/****************************************************************************/
