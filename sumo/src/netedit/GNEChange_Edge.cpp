/****************************************************************************/
/// @file    GNEChange_Edge.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A network change in which a single junction is created or deleted
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

#include "GNEChange_Edge.h"
#include "GNENet.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEAdditionalSet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Edge, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


// Constructor for creating an edge
GNEChange_Edge::GNEChange_Edge(GNENet* net, GNEEdge* edge, bool forward):
    GNEChange(net, forward),
    myEdge(edge),
    myAdditionalSetsEdge(myEdge->getAdditionalSets()) {
    for (std::vector<GNELane*>::const_iterator i = myEdge->getLanes().begin(); i != myEdge->getLanes().end(); i++) {
        myAdditionalSetsLanes[*i] = (*i)->getAdditionalSets();
    }
    assert(myNet);
    edge->incRef("GNEChange_Edge");
}


GNEChange_Edge::~GNEChange_Edge() {
    assert(myEdge);
    myEdge->decRef("GNEChange_Edge");
    if (myEdge->unreferenced()) {
        delete myEdge;
    }
}


void GNEChange_Edge::undo() {
    if (myForward) {
        myNet->deleteSingleEdge(myEdge);
        // Remove references to this edge in their AdditionalSets
        for (std::vector<GNEAdditionalSet*>::iterator i = myAdditionalSetsEdge.begin(); i != myAdditionalSetsEdge.end(); i++) {
            (*i)->removeEdgeChild(myEdge);
            // Remove additional from net if the number of childs is >= 0
            if ((*i)->getNumberOfEdgeChilds() == 0) {
                myNet->deleteAdditional(*i);
            }
        }
        // Remove references to every lane of edge in their additionalSets
        for (std::map<GNELane*, std::vector<GNEAdditionalSet*> >::iterator i = myAdditionalSetsLanes.begin(); i != myAdditionalSetsLanes.end(); i++) {
            for (std::vector<GNEAdditionalSet*>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                (*j)->removeLaneChild(i->first);
                // Remove additional from net if the number of childs is >= 0
                if ((*j)->getNumberOfLaneChilds() == 0) {
                    myNet->deleteAdditional(*j);
                }
            }
        }
    } else {
        myNet->insertEdge(myEdge);
        // Add references to this edge in their AdditionalSets
        for (std::vector<GNEAdditionalSet*>::iterator i = myAdditionalSetsEdge.begin(); i != myAdditionalSetsEdge.end(); i++) {
            myNet->insertAdditional(*i, false);
            (*i)->addEdgeChild(myEdge);
        }
        // Add references to every lane of edge in their additionalSets
        for (std::map<GNELane*, std::vector<GNEAdditionalSet*> >::iterator i = myAdditionalSetsLanes.begin(); i != myAdditionalSetsLanes.end(); i++) {
            for (std::vector<GNEAdditionalSet*>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                myNet->insertAdditional(*j, false);
                (*j)->addLaneChild(i->first);
            }
        }
    }
}


void GNEChange_Edge::redo() {
    if (myForward) {
        myNet->insertEdge(myEdge);
        // Add references to this edge in their AdditionalSets
        for (std::vector<GNEAdditionalSet*>::iterator i = myAdditionalSetsEdge.begin(); i != myAdditionalSetsEdge.end(); i++) {
            myNet->insertAdditional(*i, false);
            (*i)->addEdgeChild(myEdge);
        }
        // Add references to every lane of edge in their additionalSets
        for (std::map<GNELane*, std::vector<GNEAdditionalSet*> >::iterator i = myAdditionalSetsLanes.begin(); i != myAdditionalSetsLanes.end(); i++) {
            for (std::vector<GNEAdditionalSet*>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                myNet->insertAdditional(*j, false);
                (*j)->addLaneChild(i->first);
            }
        }
    } else {
        myNet->deleteSingleEdge(myEdge);
        // Remove references to this edge in their AdditionalSets
        for (std::vector<GNEAdditionalSet*>::iterator i = myAdditionalSetsEdge.begin(); i != myAdditionalSetsEdge.end(); i++) {
            (*i)->removeEdgeChild(myEdge);
            // Remove additional from net if the number of childs is >= 0
            if ((*i)->getNumberOfEdgeChilds() == 0) {
                myNet->deleteAdditional(*i);
            }
        }
        // Remove references to every lane of edge in their additionalSets
        for (std::map<GNELane*, std::vector<GNEAdditionalSet*> >::iterator i = myAdditionalSetsLanes.begin(); i != myAdditionalSetsLanes.end(); i++) {
            for (std::vector<GNEAdditionalSet*>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                (*j)->removeLaneChild(i->first);
                // Remove if from net if the number of childs is >= 0
                if ((*j)->getNumberOfLaneChilds() == 0) {
                    myNet->deleteAdditional(*j);
                }
            }
        }
    }
}


FXString GNEChange_Edge::undoName() const {
    if (myForward) {
        return ("Undo create edge");
    } else {
        return ("Undo delete edge");
    }
}


FXString GNEChange_Edge::redoName() const {
    if (myForward) {
        return ("Redo create edge");
    } else {
        return ("Redo delete edge");
    }
}
