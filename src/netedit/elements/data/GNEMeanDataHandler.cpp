/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEMeanDataHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 22
///
// Builds meanData objects for netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <netedit/changes/GNEChange_MeanData.h>
#include <netedit/elements/data/GNEMeanData.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEMeanDataHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEMeanDataHandler::GNEMeanDataHandler(GNENet* net, const std::string& file, const bool allowUndoRedo) :
    MeanDataHandler(file),
    myNet(net),
    myAllowUndoRedo(allowUndoRedo) {
}


GNEMeanDataHandler::~GNEMeanDataHandler() {}


void
GNEMeanDataHandler::buildEdgeMeanData(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, 
        const std::string& edgeID, const std::string& file) {
    GNEEdge* edge = myNet->getAttributeCarriers()->retrieveEdge(edgeID, false);
    if (edge) {
        GNEMeanData* edgeMeanData = new GNEMeanData(myNet, edge, file);
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(GUIIcon::MEANDATAEDGE, "add " + toString(SUMO_TAG_MEANDATA_EDGE));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_MeanData(edgeMeanData, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            edge->addChildElement(edgeMeanData);
            edgeMeanData->incRef("buildEdgeMeanData");
        }
    } else {
        writeError("Could not build " + toString(SUMO_TAG_MEANDATA_EDGE) + "; edge " + edgeID + " doesn't exist");
    }
}


void
GNEMeanDataHandler::buildLaneMeanData(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, 
        const std::string& laneID, const std::string& file) {
    GNELane* lane = myNet->getAttributeCarriers()->retrieveLane(laneID, false);
    if (lane) {
        GNEMeanData* laneMeanData = new GNEMeanData(myNet, lane, file);
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(GUIIcon::MEANDATALANE, "add " + toString(SUMO_TAG_MEANDATA_EDGE));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_MeanData(laneMeanData, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            lane->addChildElement(laneMeanData);
            laneMeanData->incRef("buildLaneMeanData");
        }
    } else {
        writeError("Could not build " + toString(SUMO_TAG_MEANDATA_EDGE) + "; lane " + laneID + " doesn't exist");
    }
}

/****************************************************************************/
