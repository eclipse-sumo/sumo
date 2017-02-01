/****************************************************************************/
/// @file    GNEClosingLaneReroute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id: GNERerouter.cpp 22699 2017-01-25 14:56:03Z behrisch $
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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

#include <string>
#include <iostream>
#include <utility>
#include <utils/geom/GeomConvHelper.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEViewNet.h"
#include "GNEClosingLaneReroute.h"
#include "GNERerouterDialog.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNEClosingLaneReroute::GNEClosingLaneReroute(GNERerouterInterval *rerouterIntervalParent, std::string closedEdgeId, std::vector<std::string> allowVehicles, std::vector<std::string> disallowVehicles) :
    myClosedEdgeId(closedEdgeId),
    myAllowVehicles(allowVehicles),
    myDisallowVehicles(disallowVehicles),
    myRerouterIntervalParent(rerouterIntervalParent),
    myTag(SUMO_TAG_CLOSING_LANE_REROUTE) {
}


GNEClosingLaneReroute::~GNEClosingLaneReroute() {
}


void
GNEClosingLaneReroute::insertAllowVehicle(std::string vehicleid) {
    // Check if was already inserted
    for (std::vector<std::string>::iterator i = myAllowVehicles.begin(); i != myAllowVehicles.end(); i++) {
        if ((*i) == vehicleid) {
            throw ProcessError(vehicleid + " already inserted");
        }
    }
    // insert in vector
    myAllowVehicles.push_back(vehicleid);
}


void
GNEClosingLaneReroute::removeAllowVehicle(std::string vehicleid) {
    // find and remove
    for (std::vector<std::string>::iterator i = myAllowVehicles.begin(); i != myAllowVehicles.end(); i++) {
        if ((*i) == vehicleid) {
            myAllowVehicles.erase(i);
            return;
        }
    }
    // Throw error if don't exist
    throw ProcessError(vehicleid + " not exist");
}


void
GNEClosingLaneReroute::insertDisallowVehicle(std::string vehicleid) {
    // Check if was already inserted
    for (std::vector<std::string>::iterator i = myDisallowVehicles.begin(); i != myDisallowVehicles.end(); i++) {
        if ((*i) == vehicleid) {
            throw ProcessError(vehicleid + " already inserted");
        }
    }
    // insert in vector
    myDisallowVehicles.push_back(vehicleid);
}


void
GNEClosingLaneReroute::removeDisallowVehicle(std::string vehicleid) {
    // find and remove
    for (std::vector<std::string>::iterator i = myDisallowVehicles.begin(); i != myDisallowVehicles.end(); i++) {
        if ((*i) == vehicleid) {
            myDisallowVehicles.erase(i);
            return;
        }
    }
    // Throw error if don't exist
    throw ProcessError(vehicleid + " not exist");
}


std::vector<std::string>
GNEClosingLaneReroute::getAllowVehicles() const {
    return myAllowVehicles;
}


std::vector<std::string>
GNEClosingLaneReroute::getDisallowVehicles() const {
    return myDisallowVehicles;
}


std::string
GNEClosingLaneReroute::getClosedEdgeId() const {
    return myClosedEdgeId;
}


SumoXMLTag 
GNEClosingLaneReroute::getTag() const {
    return myTag;
}

/****************************************************************************/
