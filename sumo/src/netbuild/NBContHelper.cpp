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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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
        relAngle1 = NBHelpers::normRelAngle(myEdge->getEndAngle(), GeomHelper::legacyDegree(
                                                e1->getFromNode()->getPosition().angleTo2D(referencePos1), true));
        relAngle2 = NBHelpers::normRelAngle(myEdge->getEndAngle(), GeomHelper::legacyDegree(
                                                e2->getFromNode()->getPosition().angleTo2D(referencePos2), true));
        if (lookAhead > MAX2(e1->getLength(), e2->getLength())) {
            break;
        }
        lookAhead *= 2;
    }
    return relAngle1 > relAngle2;
}


/* -------------------------------------------------------------------------
 * methods from straightness_sorter
 * ----------------------------------------------------------------------- */
int
NBContHelper::straightness_sorter::operator()(NBEdge* e1, NBEdge* e2) const {
    if (e1 == 0 || e2 == 0) {
        return -1;
    }
    SUMOReal relAngle1 = NBHelpers::normRelAngle(
                             myReferenceAngle, myRefIncoming ? e1->getShapeStartAngle() : e1->getShapeEndAngle());
    SUMOReal relAngle2 = NBHelpers::normRelAngle(
                             myReferenceAngle, myRefIncoming ? e2->getShapeStartAngle() : e2->getShapeEndAngle());
    const int geomIndex = myRefIncoming ? 0 : -1;

    //std::cout << " e1=" << e1->getID() << " e2=" << e2->getID() << " refA=" << myReferenceAngle << " initially a1=" << relAngle1 << " a2=" << relAngle2 << "\n";
    const SUMOReal e1Length = e1->getGeometry().length2D();
    const SUMOReal e2Length = e2->getGeometry().length2D();
    const SUMOReal maxLookAhead = MAX2(e1Length, e2Length);
    SUMOReal lookAhead = MIN2(maxLookAhead, 2 * NBEdge::ANGLE_LOOKAHEAD);
    while (fabs(relAngle1 - relAngle2) < 3.0) {
        // look at further geometry segments to resolve ambiguity
        const SUMOReal offset1 = myRefIncoming ? lookAhead : e1Length - lookAhead;
        const SUMOReal offset2 = myRefIncoming ? lookAhead : e2Length - lookAhead;
        const Position referencePos1 = e1->getGeometry().positionAtOffset2D(offset1);
        const Position referencePos2 = e2->getGeometry().positionAtOffset2D(offset2);

        relAngle1 = NBHelpers::normRelAngle(myReferenceAngle, GeomHelper::legacyDegree(
                                                e1->getGeometry()[geomIndex].angleTo2D(referencePos1), true));
        relAngle2 = NBHelpers::normRelAngle(myReferenceAngle, GeomHelper::legacyDegree(
                                                e2->getGeometry()[geomIndex].angleTo2D(referencePos2), true));

        if (lookAhead > maxLookAhead) {
            break;
        }
        lookAhead *= 2;
    }
    if (fabs(relAngle1 - relAngle2) < 3.0) {
        // use angle to end of reference edge as tiebraker
        relAngle1 = NBHelpers::normRelAngle(myReferenceAngle, GeomHelper::legacyDegree(
                                                myReferencePos.angleTo2D(e1->getLaneShape(0)[geomIndex]), true));
        relAngle2 = NBHelpers::normRelAngle(myReferenceAngle, GeomHelper::legacyDegree(
                                                myReferencePos.angleTo2D(e2->getLaneShape(0)[geomIndex]), true));
        //std::cout << "  tiebraker refPos=" << myReferencePos << " abs1="
        //    << GeomHelper::legacyDegree(myReferencePos.angleTo2D(e1->getLaneShape(0).front()), true)
        //    << " abs2=" << GeomHelper::legacyDegree(myReferencePos.angleTo2D(e2->getLaneShape(0).front()), true) <<  "\n";
    }
    //std::cout << " e1=" << e1->getID() << " e2=" << e2->getID() << " a1=" << relAngle1 << " a2=" << relAngle2 << "\n";
    return fabs(relAngle1) < fabs(relAngle2);
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
        relAngle1 = NBHelpers::normRelAngle(myEdge->getStartAngle(), GeomHelper::legacyDegree(
                                                referencePos1.angleTo2D(e1->getToNode()->getPosition()), true));
        relAngle2 = NBHelpers::normRelAngle(myEdge->getStartAngle(), GeomHelper::legacyDegree(
                                                referencePos2.angleTo2D(e2->getToNode()->getPosition()), true));
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


int
NBContHelper::edge_by_angle_to_nodeShapeCentroid_sorter::operator()(const NBEdge* e1, const NBEdge* e2) const {
    assert(e1->getFromNode() == myNode || e1->getToNode() == myNode);
    assert(e2->getFromNode() == myNode || e2->getToNode() == myNode);
    const SUMOReal angle1 = e1->getAngleAtNodeToCenter(myNode);
    const SUMOReal angle2 = e2->getAngleAtNodeToCenter(myNode);
    const SUMOReal absDiff = fabs(angle1 - angle2);

    // cannot trust the angle difference hence a heuristic:
    if (absDiff < 2 || absDiff > (360 - 2)) {
        const bool sameDir = ((e1->getFromNode() == myNode && e2->getFromNode() == myNode)
                              || (e1->getToNode() == myNode && e2->getToNode() == myNode));
        if (sameDir) {
            // put edges that allow pedestrians on the 'outside', but be aware if both allow / disallow
            const bool e1Peds = (e1->getPermissions() & SVC_PEDESTRIAN) != 0;
            const bool e2Peds = (e2->getPermissions() & SVC_PEDESTRIAN) != 0;
            if (e1->getToNode() == myNode) {
                if (e1Peds && !e2Peds) {
                    return true;
                } else if (!e1Peds && e2Peds) {
                    return false;
                }
            } else {
                if (!e1Peds && e2Peds) {
                    return true;
                } else if (e1Peds && !e2Peds) {
                    return false;
                }
            }
            // break ties to ensure strictly weak ordering
            return e1->getID() < e2->getID();
        } else {
            // sort incoming before outgoing, no need to break ties here
            return e1->getToNode() == myNode;
        }
    }
    return angle1 < angle2;
}

/****************************************************************************/

