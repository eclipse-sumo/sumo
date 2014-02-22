/****************************************************************************/
/// @file    NBContHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Some methods for traversing lists of edges
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
#include <map>
#include <cassert>
#include "NBContHelper.h"
#include <utils/geom/GeomHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * utility methods
 * ----------------------------------------------------------------------- */
void
NBContHelper::nextCW(const EdgeVector& edges, EdgeVector::const_iterator& from) {
    from++;
    if (from == edges.end()) {
        from = edges.begin();
    }
}


void
NBContHelper::nextCCW(const EdgeVector& edges, EdgeVector::const_iterator& from) {
    if (from == edges.begin()) {
        from = edges.end() - 1;
    } else {
        --from;
    }
}


std::ostream&
NBContHelper::out(std::ostream& os, const std::vector<bool>& v) {
    for (std::vector<bool>::const_iterator i = v.begin(); i != v.end(); i++) {
        os << *i;
    }
    return os;
}


NBEdge*
NBContHelper::findConnectingEdge(const EdgeVector& edges,
                                 NBNode* from, NBNode* to) {
    for (EdgeVector::const_iterator i = edges.begin(); i != edges.end(); i++) {
        if ((*i)->getToNode() == to && (*i)->getFromNode() == from) {
            return *i;
        }
    }
    return 0;
}



SUMOReal
NBContHelper::maxSpeed(const EdgeVector& ev) {
    assert(ev.size() > 0);
    SUMOReal max = (*(ev.begin()))->getSpeed();
    for (EdgeVector::const_iterator i = ev.begin() + 1; i != ev.end(); i++) {
        max =
            max > (*i)->getSpeed()
            ? max : (*i)->getSpeed();
    }
    return max;
}



/* -------------------------------------------------------------------------
 * methods from node_with_incoming_finder
 * ----------------------------------------------------------------------- */
NBContHelper::node_with_incoming_finder::node_with_incoming_finder(const NBEdge* const e)
    : myEdge(e) {}


bool
NBContHelper::node_with_incoming_finder::operator()(const NBNode* const n) const {
    const EdgeVector& incoming = n->getIncomingEdges();
    return std::find(incoming.begin(), incoming.end(), myEdge) != incoming.end();
}



/* -------------------------------------------------------------------------
 * methods from node_with_outgoing_finder
 * ----------------------------------------------------------------------- */
NBContHelper::node_with_outgoing_finder::node_with_outgoing_finder(const NBEdge* const e)
    : myEdge(e) {}


bool
NBContHelper::node_with_outgoing_finder::operator()(const NBNode* const n) const {
    const EdgeVector& outgoing = n->getOutgoingEdges();
    return std::find(outgoing.begin(), outgoing.end(), myEdge) != outgoing.end();
}



/* -------------------------------------------------------------------------
 * methods from edge_with_destination_finder
 * ----------------------------------------------------------------------- */
NBContHelper::edge_with_destination_finder::edge_with_destination_finder(NBNode* dest)
    : myDestinationNode(dest) {}


bool
NBContHelper::edge_with_destination_finder::operator()(NBEdge* e) const {
    return e->getToNode() == myDestinationNode;
}

/* -------------------------------------------------------------------------
 * methods from relative_outgoing_edge_sorter
 * ----------------------------------------------------------------------- */
int
NBContHelper::relative_outgoing_edge_sorter::operator()(NBEdge* e1, NBEdge* e2) const {
    if (e1 == 0 || e2 == 0) {
        return -1;
    }
    SUMOReal relAngle1 = NBHelpers::normRelAngle(
                             myEdge->getEndAngle(), e1->getStartAngle());
    SUMOReal relAngle2 = NBHelpers::normRelAngle(
                             myEdge->getEndAngle(), e2->getStartAngle());

    SUMOReal lookAhead = 2 * NBEdge::ANGLE_LOOKAHEAD;
    while (fabs(relAngle1 - relAngle2) < 3.0) {
        // look at further geometry segments to resolve ambiguity
        const Position referencePos1 = e1->getGeometry().positionAtOffset2D(lookAhead);
        const Position referencePos2 = e2->getGeometry().positionAtOffset2D(lookAhead);
        relAngle1 = NBHelpers::normRelAngle(myEdge->getEndAngle(), NBHelpers::angle(
                                                e1->getFromNode()->getPosition().x(), e1->getFromNode()->getPosition().y(),
                                                referencePos1.x(), referencePos1.y()));
        relAngle2 = NBHelpers::normRelAngle(myEdge->getEndAngle(), NBHelpers::angle(
                                                e2->getFromNode()->getPosition().x(), e2->getFromNode()->getPosition().y(),
                                                referencePos2.x(), referencePos2.y()));
        if (lookAhead > MAX2(e1->getLength(), e2->getLength())) {
            break;
        }
        lookAhead *= 2;
    }
    return relAngle1 > relAngle2;
}


/* -------------------------------------------------------------------------
 * methods from relative_incoming_edge_sorter
 * ----------------------------------------------------------------------- */
int
NBContHelper::relative_incoming_edge_sorter::operator()(NBEdge* e1, NBEdge* e2) const {
    if (e1 == 0 || e2 == 0) {
        return -1;
    }
    SUMOReal relAngle1 = NBHelpers::normRelAngle(
                             myEdge->getStartAngle(), e1->getEndAngle());
    SUMOReal relAngle2 = NBHelpers::normRelAngle(
                             myEdge->getStartAngle(), e2->getEndAngle());

    SUMOReal lookAhead = 2 * NBEdge::ANGLE_LOOKAHEAD;
    while (fabs(relAngle1 - relAngle2) < 3.0) {
        // look at further geometry segments to resolve ambiguity
        const Position referencePos1 = e1->getGeometry().positionAtOffset2D(e1->getGeometry().length() - lookAhead);
        const Position referencePos2 = e2->getGeometry().positionAtOffset2D(e2->getGeometry().length() - lookAhead);
        relAngle1 = NBHelpers::normRelAngle(myEdge->getStartAngle(), NBHelpers::angle(
                                                referencePos1.x(), referencePos1.y(),
                                                e1->getToNode()->getPosition().x(), e1->getToNode()->getPosition().y()));
        relAngle2 = NBHelpers::normRelAngle(myEdge->getStartAngle(), NBHelpers::angle(
                                                referencePos2.x(), referencePos2.y(),
                                                e2->getToNode()->getPosition().x(), e2->getToNode()->getPosition().y()));
        if (lookAhead > MAX2(e1->getLength(), e2->getLength())) {
            break;
        }
        lookAhead *= 2;
    }
    return relAngle1 > relAngle2;
}


std::ostream&
operator<<(std::ostream& os, const EdgeVector& ev) {
    for (EdgeVector::const_iterator i = ev.begin(); i != ev.end(); i++) {
        if (i != ev.begin()) {
            os << ", ";
        }
        os << (*i)->getID();
    }
    return os;
}




SUMOReal
NBContHelper::getMaxSpeed(const EdgeVector& edges) {
    if (edges.size() == 0) {
        return -1;
    }
    SUMOReal ret = (*(edges.begin()))->getSpeed();
    for (EdgeVector::const_iterator i = edges.begin() + 1; i != edges.end(); i++) {
        if ((*i)->getSpeed() > ret) {
            ret = (*i)->getSpeed();
        }
    }
    return ret;
}


SUMOReal
NBContHelper::getMinSpeed(const EdgeVector& edges) {
    if (edges.size() == 0) {
        return -1;
    }
    SUMOReal ret = (*(edges.begin()))->getSpeed();
    for (EdgeVector::const_iterator i = edges.begin() + 1; i != edges.end(); i++) {
        if ((*i)->getSpeed() < ret) {
            ret = (*i)->getSpeed();
        }
    }
    return ret;
}



/****************************************************************************/

