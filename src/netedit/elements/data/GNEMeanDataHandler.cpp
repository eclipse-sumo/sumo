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
GNEMeanDataHandler::buildEdgeMeanData(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string &ID, 
        const std::string &file, SUMOTime period, SUMOTime begin, SUMOTime end, const bool trackVehicles, 
        const std::vector<std::string> &writtenAttributes, const bool aggregate, const std::vector<std::string> &edges, 
        const std::string &edgeFile, std::string excludeEmpty, const bool withInternal, 
        const std::vector<std::string> &detectPersons, const double minSamples, const double maxTravelTime, 
        const std::vector<std::string> &vTypes, const double speedThreshold) {
    // check if meanData edge exists
    if (myNet->getAttributeCarriers()->retrieveMeanData(SUMO_TAG_MEANDATA_EDGE, ID, false) == nullptr) {
        GNEMeanData* edgeMeanData = new GNEMeanData(myNet, SUMO_TAG_MEANDATA_EDGE, ID, file, period, begin, end,
            trackVehicles, writtenAttributes,  aggregate, edges, edgeFile, excludeEmpty,  withInternal, 
            detectPersons, minSamples, maxTravelTime, vTypes, speedThreshold);
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(GUIIcon::MEANDATAEDGE, "add " + toString(SUMO_TAG_MEANDATA_EDGE));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_MeanData(edgeMeanData, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            edgeMeanData->incRef("buildEdgeMeanData");
        }
    } else {
        writeError("Could not build " + toString(SUMO_TAG_MEANDATA_LANE) + "; " + ID + " already exist");
    }
}


void
GNEMeanDataHandler::buildLaneMeanData(const CommonXMLStructure::SumoBaseObject* /*sumoBaseObject*/, const std::string &ID, 
        const std::string &file, SUMOTime period, SUMOTime begin, SUMOTime end, const bool trackVehicles, 
        const std::vector<std::string> &writtenAttributes, const bool aggregate, const std::vector<std::string> &edges, 
        const std::string &edgeFile, std::string excludeEmpty, const bool withInternal, 
        const std::vector<std::string> &detectPersons, const double minSamples, const double maxTravelTime, 
        const std::vector<std::string> &vTypes, const double speedThreshold) {
    // check if meanData lane exists
    if (myNet->getAttributeCarriers()->retrieveMeanData(SUMO_TAG_MEANDATA_LANE, ID, false) == nullptr) {
        GNEMeanData* laneMeanData = new GNEMeanData(myNet, SUMO_TAG_MEANDATA_LANE, ID, file, period, begin, end,
            trackVehicles, writtenAttributes,  aggregate, edges, edgeFile, excludeEmpty,  withInternal, 
            detectPersons, minSamples, maxTravelTime, vTypes, speedThreshold);
        if (myAllowUndoRedo) {
            myNet->getViewNet()->getUndoList()->begin(GUIIcon::MEANDATALANE, "add " + toString(SUMO_TAG_MEANDATA_LANE));
            myNet->getViewNet()->getUndoList()->add(new GNEChange_MeanData(laneMeanData, true), true);
            myNet->getViewNet()->getUndoList()->end();
        } else {
            laneMeanData->incRef("buildLaneMeanData");
        }
    } else {
        writeError("Could not build " + toString(SUMO_TAG_MEANDATA_LANE) + "; " + ID + " already exist");
    }
}

/****************************************************************************/
