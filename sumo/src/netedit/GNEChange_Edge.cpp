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
    myEdge(edge) {
    assert(myNet);
    edge->incRef("GNEChange_Edge");
    // Save additionals of edge
    myAdditionalChilds = myEdge->getAdditionalChilds();
    myAdditionalSetParents = myEdge->getAdditionalSets();
    // Iterate over lanes vinculated to edge
    for (std::vector<GNELane*>::const_iterator i = myEdge->getLanes().begin(); i != myEdge->getLanes().end(); i++) {
        // Save additionals vinculated to lane
        myAdditionalLanes[*i] = (*i)->getAdditionalChilds();
        myAdditionalSetsLanes[*i] = (*i)->getAdditionalSetParents();
    }
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
        // 1 - Remove additional sets vinculated with this edge of net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->deleteAdditional(*i);
        }
        // 2 - Remove references to this edge in their AdditionalSets
        for (std::vector<GNEAdditionalSet*>::iterator i = myAdditionalSetParents.begin(); i != myAdditionalSetParents.end(); i++) {
            (*i)->removeEdgeChild(myEdge);
            // Remove additional from net if the number of childs is >= 0
            if ((*i)->getNumberOfEdgeChilds() == 0) {
                myNet->deleteAdditional(*i);
            }
        }
        // 3 - Remove of the net all additional child of the lanes of edge
        for (std::map<GNELane*, std::vector<GNEAdditional*> >::iterator i = myAdditionalLanes.begin(); i != myAdditionalLanes.end(); i++) {
            for (std::vector<GNEAdditional*>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                myNet->deleteAdditional(*j);
            }
        }
        // 4 - Remove references to every lane of edge in their additionalSets
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
        // 1 - add additional sets vinculated with this edge to the net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->insertAdditional(*i);
        }
        // 2 - Add references to this edge in their AdditionalSets
        for (std::vector<GNEAdditionalSet*>::iterator i = myAdditionalSetParents.begin(); i != myAdditionalSetParents.end(); i++) {
            myNet->insertAdditional(*i, false);
            (*i)->addEdgeChild(myEdge);
        }
        // 3 - add in the net all additional child of the lanes of edge
        for (std::map<GNELane*, std::vector<GNEAdditional*> >::iterator i = myAdditionalLanes.begin(); i != myAdditionalLanes.end(); i++) {
            for (std::vector<GNEAdditional*>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                myNet->insertAdditional(*j);
            }
        }
        // 4 - Add references to every lane of edge in their additionalSets
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
        // 1 - Add additional sets vinculated with this edge to the net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->insertAdditional(*i);
        }
        // 2 - Add references to this edge in their AdditionalSets
        for (std::vector<GNEAdditionalSet*>::iterator i = myAdditionalSetParents.begin(); i != myAdditionalSetParents.end(); i++) {
            myNet->insertAdditional(*i, false);
            (*i)->addEdgeChild(myEdge);
        }
        // 3 - Add in the net all additional child of the lanes of edge
        for (std::map<GNELane*, std::vector<GNEAdditional*> >::iterator i = myAdditionalLanes.begin(); i != myAdditionalLanes.end(); i++) {
            for (std::vector<GNEAdditional*>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                myNet->insertAdditional(*j);
            }
        }
        // 4 - Add references to every lane of edge in their additionalSets
        for (std::map<GNELane*, std::vector<GNEAdditionalSet*> >::iterator i = myAdditionalSetsLanes.begin(); i != myAdditionalSetsLanes.end(); i++) {
            for (std::vector<GNEAdditionalSet*>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                myNet->insertAdditional(*j, false);
                (*j)->addLaneChild(i->first);
            }
        }
    } else {
        myNet->deleteSingleEdge(myEdge);
        // 1 - Remove additional sets vinculated with this edge of net
        for (std::vector<GNEAdditional*>::iterator i = myAdditionalChilds.begin(); i != myAdditionalChilds.end(); i++) {
            myNet->deleteAdditional(*i);
        }
        // 2 - Remove references to this edge in their AdditionalSets
        for (std::vector<GNEAdditionalSet*>::iterator i = myAdditionalSetParents.begin(); i != myAdditionalSetParents.end(); i++) {
            (*i)->removeEdgeChild(myEdge);
            // Remove additional from net if the number of childs is >= 0
            if ((*i)->getNumberOfEdgeChilds() == 0) {
                myNet->deleteAdditional(*i);
            }
        }
        // 3 - Remove of the net all additional child of the lanes of edge
        for (std::map<GNELane*, std::vector<GNEAdditional*> >::iterator i = myAdditionalLanes.begin(); i != myAdditionalLanes.end(); i++) {
            for (std::vector<GNEAdditional*>::iterator j = i->second.begin(); j != i->second.end(); j++) {
                myNet->deleteAdditional(*j);
            }
        }
        // 4 - Remove references to every lane of edge in their additionalSets
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
