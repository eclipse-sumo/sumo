/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEChange_RegisterJoin.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// A network change in which something is changed (for undo/redo)
/****************************************************************************/
#include <config.h>

#include <netbuild/NBNodeCont.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/elements/data/GNEDataSet.h>

#include "GNEChange_RegisterJoin.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_RegisterJoin, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

void
GNEChange_RegisterJoin::registerJoin(const std::set<NBNode*, ComparatorIdLess>& cluster, NBNodeCont& nc, GNEUndoList* undoList) {
    auto change = new GNEChange_RegisterJoin(cluster, nc);
    undoList->begin(GUIIcon::UNDO, TLF("Register joined junctions '%'", joinNamedToString(cluster, ",")));
    undoList->add(change, true);
    undoList->end();
}


GNEChange_RegisterJoin::~GNEChange_RegisterJoin() {
}


void
GNEChange_RegisterJoin::undo() {
    // show extra information for tests
    WRITE_DEBUG("Reverting join registration");
    myNC.unregisterJoinedCluster(myNodeIDs);
}


void
GNEChange_RegisterJoin::redo() {
    // show extra information for tests
    WRITE_DEBUG("Redo join registration");
    myNC.registerJoinedCluster(myNodeIDs);
}


std::string
GNEChange_RegisterJoin::undoName() const {
    return (TL("Undo register joined junctions"));
}


std::string
GNEChange_RegisterJoin::redoName() const {
    return (TL("Redo register joined junctions"));
}


GNEChange_RegisterJoin::GNEChange_RegisterJoin(const std::set<NBNode*, ComparatorIdLess>& cluster, NBNodeCont& nc) :
    GNEChange(Supermode::NETWORK, true, false),
    myNC(nc)
{
    for (NBNode* n : cluster) {
        myNodeIDs.insert(n->getID());
    }
}


/****************************************************************************/
