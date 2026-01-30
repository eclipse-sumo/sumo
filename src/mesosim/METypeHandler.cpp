/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    METypeHandler.cpp
/// @author  Jakob Erdmann
/// @date    Jan 2026
///
// The XML-Handler for loading meso edge types
// This is a dedicated handler because meso types must be loaded from additional
// files before any other objects are loaded from them
/****************************************************************************/
#include <config.h>

#include <string>
#include "METypeHandler.h"
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <microsim/MSNet.h>
#include <mesosim/MESegment.h>


// ===========================================================================
// method definitions
// ===========================================================================
METypeHandler::METypeHandler(const std::string& file, MSNet& net) :
    SUMOSAXHandler(file),
    myNet(net),
    myHaveSeenMesoEdgeType(false)
{
}


METypeHandler::~METypeHandler() {}


void
METypeHandler::myStartElement(int element,
                          const SUMOSAXAttributes& attrs) {
    try {
        switch (element) {
            case SUMO_TAG_TYPE: {
                bool ok = true;
                myCurrentTypeID = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
                break;
            }
            case SUMO_TAG_MESO: {
                addMesoEdgeType(attrs);
                break;
            }
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


void
METypeHandler::myEndElement(int element) {
    switch (element) {
        case SUMO_TAG_TYPE:
            myCurrentTypeID = "";
            break;
        default:
            break;
    }
}



void
METypeHandler::addMesoEdgeType(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    MESegment::MesoEdgeType edgeType = myNet.getMesoType(""); // init defaults
    edgeType.tauff = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MESO_TAUFF, myCurrentTypeID.c_str(), ok, edgeType.tauff);
    edgeType.taufj = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MESO_TAUFJ, myCurrentTypeID.c_str(), ok, edgeType.taufj);
    edgeType.taujf = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MESO_TAUJF, myCurrentTypeID.c_str(), ok, edgeType.taujf);
    edgeType.taujj = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MESO_TAUJJ, myCurrentTypeID.c_str(), ok, edgeType.taujj);
    edgeType.jamThreshold = attrs.getOpt<double>(SUMO_ATTR_JAM_DIST_THRESHOLD, myCurrentTypeID.c_str(), ok, edgeType.jamThreshold);
    edgeType.junctionControl = attrs.getOpt<bool>(SUMO_ATTR_MESO_JUNCTION_CONTROL, myCurrentTypeID.c_str(), ok, edgeType.junctionControl);
    edgeType.tlsPenalty = attrs.getOpt<double>(SUMO_ATTR_MESO_TLS_PENALTY, myCurrentTypeID.c_str(), ok, edgeType.tlsPenalty);
    edgeType.tlsFlowPenalty = attrs.getOpt<double>(SUMO_ATTR_MESO_TLS_FLOW_PENALTY, myCurrentTypeID.c_str(), ok, edgeType.tlsFlowPenalty);
    edgeType.minorPenalty = attrs.getOptSUMOTimeReporting(SUMO_ATTR_MESO_MINOR_PENALTY, myCurrentTypeID.c_str(), ok, edgeType.minorPenalty);
    edgeType.overtaking = attrs.getOpt<bool>(SUMO_ATTR_MESO_OVERTAKING, myCurrentTypeID.c_str(), ok, edgeType.overtaking);
    edgeType.edgeLength = attrs.getOpt<double>(SUMO_ATTR_MESO_EDGELENGHT, myCurrentTypeID.c_str(), ok, edgeType.edgeLength);

    if (ok) {
        myNet.addMesoType(myCurrentTypeID, edgeType);
    }
    myHaveSeenMesoEdgeType = true;
}


/****************************************************************************/
