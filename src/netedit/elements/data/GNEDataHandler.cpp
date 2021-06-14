/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEDataHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Builds data objects for netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/changes/GNEChange_DataSet.h>
#include <netedit/changes/GNEChange_DataInterval.h>
#include <netedit/changes/GNEChange_GenericData.h>
#include <netedit/elements/data/GNEEdgeRelData.h>
#include <netedit/elements/data/GNEEdgeData.h>
#include <netedit/elements/data/GNETAZRelData.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEDataHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDataHandler::GNEDataHandler(GNENet* net, const std::string& file, const bool allowUndoRedo) :
    DataHandler(file),
    myNet(net),
    myAllowUndoRedo(allowUndoRedo) {
}


GNEDataHandler::~GNEDataHandler() {}


void
GNEDataHandler::buildDataInterval(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, 
    const std::string& dataSetID, const double begin, const double end) {

}


void
GNEDataHandler::buildEdgeData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, 
    const std::string &edgeID, const std::map<std::string, double>& parameters) {

}


void
GNEDataHandler::buildEdgeRelationData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, 
    const std::string &fromEdge, const std::string &toEdge, const std::map<std::string, double>& parameters) {

}


void 
GNEDataHandler::buildTAZRelationData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, 
    const std::string &fromTAZID, const std::string &toTAZID, const std::map<std::string, double>& parameters) {

}

/****************************************************************************/
