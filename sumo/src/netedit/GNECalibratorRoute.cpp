/****************************************************************************/
/// @file    GNECalibratorRoute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2016
/// @version $Id$
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
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNECalibratorRoute.h"
#include "GNECalibrator.h"
#include "GNEEdge.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEJunction.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorRoute::GNECalibratorRoute(GNECalibrator* calibratorParent) :
    myCalibratorParent(calibratorParent), myRouteID(calibratorParent->generateRouteID()), myColor("") {
}


GNECalibratorRoute::GNECalibratorRoute(GNECalibrator* calibratorParent, std::string routeID, std::vector<std::string> edges, std::string color) :
    myCalibratorParent(calibratorParent), myRouteID(calibratorParent->generateRouteID()), myColor("") {
    // set values using set functions to avoid non-valid values
    setRouteID(routeID);
    setEdges(edges);
    setColor(color);
}


GNECalibratorRoute::GNECalibratorRoute(GNECalibrator* calibratorParent, std::string routeID, std::vector<GNEEdge*> edges, std::string color) :
    myCalibratorParent(calibratorParent), myRouteID(""), myColor("") {
    // set values using set functions to avoid non-valid values
    setRouteID(routeID);
    setEdges(edges);
    setColor(color);
}

GNECalibratorRoute::~GNECalibratorRoute() {}


GNECalibrator*
GNECalibratorRoute::getCalibratorParent() const {
    return myCalibratorParent;
}


SumoXMLTag
GNECalibratorRoute::getTag() const {
    return SUMO_TAG_ROUTE;
}


const std::string&
GNECalibratorRoute::getRouteID() const {
    return myRouteID;
}


std::vector<std::string>
GNECalibratorRoute::getEdgesIDs() const {
    std::vector<std::string> edgeIDs;
    for (std::vector<GNEEdge*>::const_iterator i = myEdges.begin(); i != myEdges.end(); i++) {
        edgeIDs.push_back((*i)->getID());
    }
    return edgeIDs;
}


const std::vector<GNEEdge*>&
GNECalibratorRoute::getEdges() const {
    return myEdges;
}


const std::string&
GNECalibratorRoute::getColor() const {
    return myColor;
}


bool
GNECalibratorRoute::setRouteID(std::string routeID) {
    if (routeID.empty()) {
        return false;
    } else if (myCalibratorParent->getViewNet()->getNet()->routeExists(routeID) == true) {
        return false;
    } else {
        myRouteID = routeID;
        return true;
    }
}


bool
GNECalibratorRoute::setEdges(const std::vector<std::string>& edgeIDs) {
    std::vector<GNEEdge*> edges;
    GNEEdge* edgeTmp;
    for (std::vector<std::string>::const_iterator i = edgeIDs.begin(); i != edgeIDs.end(); i++) {
        // check that current edges exist in the net
        edgeTmp = myCalibratorParent->getViewNet()->getNet()->retrieveEdge((*i), false);
        if (edgeTmp != NULL) {
            edges.push_back(edgeTmp);
        } else {
            return false;
        }
    }
    return setEdges(edges);
}


bool
GNECalibratorRoute::setEdges(const std::vector<GNEEdge*>& edges) {
    myEdges = edges;
    return true;
}


bool
GNECalibratorRoute::setEdges(const std::string& edgeIDs) {
    if (GNEAttributeCarrier::canParse<std::vector<std::string> >(edgeIDs)) {
        return setEdges(GNEAttributeCarrier::parse<std::vector<std::string> >(edgeIDs));
    } else {
        return false;
    }
}


bool
GNECalibratorRoute::setColor(std::string color) {
    myColor = color;
    return true;
}


std::string
GNECalibratorRoute::checkEdgeRoute(const std::vector<std::string>& edgeIDs) const {
    std::vector<GNEEdge*> edges;
    // check that there aren't to equal adjacent edges
    for (std::vector<std::string>::const_iterator i = edgeIDs.begin() + 1; i != edgeIDs.end(); i++) {
        GNEEdge* retrievedEdge = myCalibratorParent->getViewNet()->getNet()->retrieveEdge((*i), false);
        if (retrievedEdge != NULL) {
            edges.push_back(retrievedEdge);
        } else {
            return (toString(SUMO_TAG_EDGE) + " '" + *i + "' doesn't exist");
        }
    }
    // check that there aren't to equal adjacent edges
    for (std::vector<GNEEdge*>::const_iterator i = edges.begin() + 1; i != edges.end(); i++) {
        if ((*(i - 1))->getID() == (*i)->getID()) {
            return (toString(SUMO_TAG_EDGE) + " '" + (*i)->getID() + "' is adjacent to itself");
        }
    }
    // check that edges are adjacents
    for (std::vector<GNEEdge*>::const_iterator i = edges.begin() + 1; i != edges.end(); i++) {
        std::vector<GNEEdge*> adyacents = (*(i - 1))->getGNEJunctionDestiny()->getGNEOutgoingEdges();
        if (std::find(adyacents.begin(), adyacents.end(), (*i)) == adyacents.end()) {
            return (toString(SUMO_TAG_EDGE) + " '" + (*(i - 1))->getID() + "' isn't adjacent to " + toString(SUMO_TAG_EDGE) + " '" + (*i)->getID() + "'");
        }
    }
    // all ok, then return ""
    return "";
}


bool
GNECalibratorRoute::operator==(const GNECalibratorRoute& calibratorRoute) const {
    return (myRouteID == calibratorRoute.getRouteID());
}

/****************************************************************************/
