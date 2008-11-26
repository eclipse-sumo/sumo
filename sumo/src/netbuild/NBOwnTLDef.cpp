/****************************************************************************/
/// @file    NBOwnTLDef.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// A traffic light logics which must be computed (only nodes/edges are given)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include "NBTrafficLightLogicVector.h"
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
NBOwnTLDef::NBOwnTLDef(const string &id,
                       const set<NBNode*> &junctions) throw()
        : NBTrafficLightDefinition(id, junctions)
{}


NBOwnTLDef::NBOwnTLDef(const string &id, NBNode *junction) throw()
        : NBTrafficLightDefinition(id, junction)
{}


NBOwnTLDef::NBOwnTLDef(const string &id,
                       string type,
                       NBNode *junction) throw()
        : NBTrafficLightDefinition(id, type, junction)
{}


NBOwnTLDef::NBOwnTLDef(const string &id) throw()
        : NBTrafficLightDefinition(id)
{}


NBOwnTLDef::~NBOwnTLDef() throw()
{}



int
NBOwnTLDef::getToPrio(NBEdge *e)
{
    return e->getJunctionPriority(e->getToNode());
}


SUMOReal
NBOwnTLDef::computeUnblockedWeightedStreamNumber(NBEdge* e1, NBEdge *e2)
{
    SUMOReal val = 0;
    for (unsigned int e1l=0; e1l<e1->getNoLanes(); e1l++) {
        vector<NBEdge::Connection> approached1 = e1->getConnectionsFromLane(e1l);
        for (unsigned int e2l=0; e2l<e2->getNoLanes(); e2l++) {
            vector<NBEdge::Connection> approached2 = e2->getConnectionsFromLane(e2l);
            for (vector<NBEdge::Connection>::iterator e1c=approached1.begin(); e1c!=approached1.end(); ++e1c) {
                for (vector<NBEdge::Connection>::iterator e2c=approached2.begin(); e2c!=approached2.end(); ++e2c) {
                    if (!foes(e1, (*e1c).toEdge, e2, (*e2c).toEdge)) {
                        val += e1->getToNode()->getMMLDirection(e1, (*e1c).toEdge)==MMLDIR_STRAIGHT ? 1 : .5;
                        val += e2->getToNode()->getMMLDirection(e2, (*e2c).toEdge)==MMLDIR_STRAIGHT ? 1 : .5;
                    }
                }
            }
        }
    }
    return val;
}


pair<NBEdge*, NBEdge*>
NBOwnTLDef::getBestCombination(const vector<NBEdge*> &edges)
{
    pair<NBEdge*, NBEdge*> bestPair(0,0);
    SUMOReal bestValue = -1;
    for (vector<NBEdge*>::const_iterator i=edges.begin(); i!=edges.end(); ++i) {
        for (vector<NBEdge*>::const_iterator j=i+1; j!=edges.end(); ++j) {
            SUMOReal value = computeUnblockedWeightedStreamNumber(*i, *j);
            if (value>bestValue) {
                bestValue = value;
                bestPair = pair<NBEdge*, NBEdge*>(*i, *j);
            }
        }
    }
    return bestPair;
}




pair<NBEdge*, NBEdge*>
NBOwnTLDef::getBestPair(vector<NBEdge*> &incoming)
{
    if (incoming.size()==1) {
        // only one there - return the one
        pair<NBEdge*, NBEdge*> ret(*incoming.begin(), 0);
        incoming.clear();
        return ret;
    }
    // determine the best combination
    //  by priority, first
    vector<NBEdge*> used;
    sort(incoming.begin(), incoming.end(), edge_by_incoming_priority_sorter());
    used.push_back(*incoming.begin()); // the first will definitely be used
    // get the ones with the same priority
    int prio = getToPrio(*used.begin());
    for (vector<NBEdge*>::iterator i=incoming.begin()+1; i!=incoming.end()&&prio!=getToPrio(*i); ++i) {
        used.push_back(*i);
    }
    //  if there only lower priorised, use these, too
    if (used.size()<2) {
        used = incoming;
    }
    pair<NBEdge*, NBEdge*> ret = getBestCombination(used);
    incoming.erase(find(incoming.begin(), incoming.end(), ret.first));
    incoming.erase(find(incoming.begin(), incoming.end(), ret.second));
    return ret;
}


NBTrafficLightLogicVector *
NBOwnTLDef::myCompute(const NBEdgeCont &,
                      size_t breakingTime, string type)
{
    // build complete lists first
    const EdgeVector &incoming = getIncomingEdges();
    vector<NBEdge*> fromEdges, toEdges;
    vector<bool> isLeftMoverV, isTurnaround;
    unsigned int noLanesAll = 0;
    unsigned int noLinksAll = 0;
    for (unsigned int i1=0; i1<incoming.size(); i1++) {
        unsigned int noLanes = incoming[i1]->getNoLanes();
        noLanesAll += noLanes;
        for (unsigned int i2=0; i2<noLanes; i2++) {
            NBEdge *fromEdge = incoming[i1];
            vector<NBEdge::Connection> approached = fromEdge->getConnectionsFromLane(i2);
            noLinksAll += (unsigned int) approached.size();
            for (unsigned int i3=0; i3<approached.size(); i3++) {
                assert(i3<approached.size());
                NBEdge *toEdge = approached[i3].toEdge;
                fromEdges.push_back(fromEdge);
                //myFromLanes.push_back(i2);
                toEdges.push_back(toEdge);
                if (toEdge!=0) {
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

    NBTrafficLightLogic *logic = new NBTrafficLightLogic(getID(), "0", type, noLinksAll);
    vector<NBEdge*> toProc = incoming;
    // build all phases
    while (toProc.size()>0) {
        pair<NBEdge*, NBEdge*> chosen = getBestPair(toProc);
        size_t pos = 0;
        size_t duration = 31;
        if (OptionsCont::getOptions().isSet("traffic-light-green")) {
            duration = OptionsCont::getOptions().getInt("traffic-light-green");
        }
        std::bitset<64> driveMask(0);
        std::bitset<64> brakeMask(0);
        std::bitset<64> yellowMask(0);
        // plain straight movers
        for (unsigned int i1=0; i1<(unsigned int) incoming.size(); i1++) {
            NBEdge *fromEdge = incoming[i1];
            bool inChosen = fromEdge==chosen.first||fromEdge==chosen.second;//chosen.find(fromEdge)!=chosen.end();
            unsigned int noLanes = fromEdge->getNoLanes();
            for (unsigned int i2=0; i2<noLanes; i2++) {
                vector<NBEdge::Connection> approached = fromEdge->getConnectionsFromLane(i2);
                noLinksAll += (unsigned int) approached.size();
                for (unsigned int i3=0; i3<approached.size(); i3++) {
                    if (inChosen) {
                        driveMask.set(pos, true);
                        brakeMask.set(pos, false);
                        yellowMask.set(pos, false);
                    } else {
                        driveMask.set(pos, false);
                        brakeMask.set(pos, true);
                        yellowMask.set(pos, false);
                    }
                    ++pos;
                }
            }
        }
        // correct behaviour for those that are not in chosen, but may drive, though
        for (unsigned int i1=0; i1<pos; ++i1) {
            if (driveMask.test(i1)) {
                continue;
            }
            bool isForbidden = false;
            for (unsigned int i2=0; i2<pos&&!isForbidden; ++i2) {
                if (driveMask.test(i2)&&!isTurnaround[i2]&&
                        (forbids(fromEdges[i2], toEdges[i2], fromEdges[i1], toEdges[i1], true)||forbids(fromEdges[i1], toEdges[i1], fromEdges[i2], toEdges[i2], true))) {
                    isForbidden = true;
                }
            }
            if (!isForbidden) {
                driveMask.set(i1, true);
                brakeMask.set(i1, false);
                yellowMask.set(i1, false);
            }
        }
        // correct behaviour for those that have to wait (mainly left-mover)
        bool haveForbiddenLeftMover = false;
        for (unsigned int i1=0; i1<pos; ++i1) {
            if (!driveMask.test(i1)) {
                continue;
            }
            for (unsigned int i2=0; i2<pos; ++i2) {
                if (driveMask.test(i2)&&forbids(fromEdges[i2], toEdges[i2], fromEdges[i1], toEdges[i1], true)) {
                    brakeMask.set(i1, true);
                    if (!isTurnaround[i1]) {
                        haveForbiddenLeftMover = true;
                    }
                }
            }
        }

        // add step
        logic->addStep(duration, driveMask, brakeMask, yellowMask);

        if (breakingTime>0) {
            // build yellow (straight)
            duration = breakingTime;
            for (unsigned int i1=0; i1<pos; ++i1) {
                if (!driveMask.test(i1)) {
                    continue;
                }
                if (brakeMask.test(i1)&&haveForbiddenLeftMover) {
                    continue;
                }
                driveMask.set(i1, false);
                brakeMask.set(i1, true);
                yellowMask.set(i1, true);
            }
            // add step
            logic->addStep(duration, driveMask, brakeMask, yellowMask);
        }

        if (haveForbiddenLeftMover) {

            // build left green
            duration = 6;
            for (unsigned int i1=0; i1<pos; ++i1) {
                if (yellowMask.test(i1)) {
                    driveMask.set(i1, false);
                    brakeMask.set(i1, true);
                    yellowMask.set(i1, false);
                    continue;
                }
                if (driveMask.test(i1)) {
                    brakeMask.set(i1, false);
                }
            }
            // add step
            logic->addStep(duration, driveMask, brakeMask, yellowMask);

            // build left yellow
            if (breakingTime>0) {
                duration = breakingTime;
                for (unsigned int i1=0; i1<pos; ++i1) {
                    if (!driveMask.test(i1)) {
                        continue;
                    }
                    driveMask.set(i1, false);
                    brakeMask.set(i1, true);
                    yellowMask.set(i1, true);
                }
                // add step
                logic->addStep(duration, driveMask, brakeMask, yellowMask);
            }
        }
    }
    if (logic->getDuration()>0) {
        NBTrafficLightLogicVector *lv = new NBTrafficLightLogicVector(NBConnectionVector(), "static");
        lv->add(logic);
        return lv;
    } else {
        return 0;
    }

}


void
NBOwnTLDef::collectNodes()
{}


void
NBOwnTLDef::collectLinks()
{
    // build the list of links which are controled by the traffic light
    for (EdgeVector::iterator i=myIncomingEdges.begin(); i!=myIncomingEdges.end(); i++) {
        NBEdge *incoming = *i;
        unsigned int noLanes = incoming->getNoLanes();
        for (unsigned int j=0; j<noLanes; j++) {
            vector<NBEdge::Connection> connected = incoming->getConnectionsFromLane(j);
            for (vector<NBEdge::Connection>::iterator k=connected.begin(); k!=connected.end(); k++) {
                const NBEdge::Connection &el = *k;
                if (el.toEdge!=0) {
                    if (el.toLane>=(int) el.toEdge->getNoLanes()) {
                        throw ProcessError("Connection '" + incoming->getID() + "_" + toString(j) + "->" + el.toEdge->getID() + "_" + toString(el.toLane) + "' yields in a not existing lane.");

                    }
                    myControlledLinks.push_back(NBConnection(incoming, j, el.toEdge, el.toLane));
                }
            }
        }
    }
}


void
NBOwnTLDef::setParticipantsInformation()
{
    // assign participating nodes to the request
    collectNodes();
    // collect the information about participating edges and links
    collectEdges();
    collectLinks();
}

void
NBOwnTLDef::setTLControllingInformation(const NBEdgeCont &) const
{
    // set the information about the link's positions within the tl into the
    //  edges the links are starting at, respectively
    size_t pos = 0;
    for (NBConnectionVector::const_iterator j=myControlledLinks.begin(); j!=myControlledLinks.end(); j++) {
        const NBConnection &conn = *j;
        NBEdge *edge = conn.getFrom();
        if (edge->setControllingTLInformation(
                    conn.getFromLane(), conn.getTo(), conn.getToLane(),
                    getID(), pos)) {
            pos++;
        }
    }
}


void
NBOwnTLDef::remapRemoved(NBEdge * /*removed*/, const EdgeVector &/*incoming*/,
                         const EdgeVector &/*outgoing*/)
{}


void
NBOwnTLDef::replaceRemoved(NBEdge * /*removed*/, int /*removedLane*/,
                           NBEdge * /*by*/, int /*byLane*/)
{}



/****************************************************************************/
