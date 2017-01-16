/****************************************************************************/
/// @file    GNERerouter.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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
#include "GNERerouter.h"
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

// ---------------------------------------------------------------------------
// GNERerouter::closingReroute - methods
// ---------------------------------------------------------------------------

GNERerouter::closingReroute::closingReroute(std::string closedEdgeId, std::vector<std::string> allowVehicles, std::vector<std::string> disallowVehicles) :
    myClosedEdgeId(closedEdgeId),
    myAllowVehicles(allowVehicles),
    myDisallowVehicles(disallowVehicles) {
}


GNERerouter::closingReroute::~closingReroute() {
}


void
GNERerouter::closingReroute::insertAllowVehicle(std::string vehicleid) {
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
GNERerouter::closingReroute::removeAllowVehicle(std::string vehicleid) {
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
GNERerouter::closingReroute::insertDisallowVehicle(std::string vehicleid) {
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
GNERerouter::closingReroute::removeDisallowVehicle(std::string vehicleid) {
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
GNERerouter::closingReroute::getAllowVehicles() const {
    return myAllowVehicles;
}


std::vector<std::string>
GNERerouter::closingReroute::getDisallowVehicles() const {
    return myDisallowVehicles;
}


std::string
GNERerouter::closingReroute::getClosedEdgeId() const {
    return myClosedEdgeId;
}

// ---------------------------------------------------------------------------
// GNERerouter::destProbReroute - methods
// ---------------------------------------------------------------------------

GNERerouter::destProbReroute::destProbReroute(std::string newDestinationId, SUMOReal probability):
    myNewDestinationId(newDestinationId),
    myProbability(probability) {
}


GNERerouter::destProbReroute::~destProbReroute() {
}


std::string
GNERerouter::destProbReroute::getNewDestinationId() const {
    return myNewDestinationId;
}


SUMOReal
GNERerouter::destProbReroute::getProbability() const {
    return myProbability;
}


void
GNERerouter::destProbReroute::setProbability(SUMOReal probability) {
    if (probability >= 0 && probability <= 1) {
        myProbability = probability;
    } else {
        throw InvalidArgument(toString(probability) + " isn't a probability");
    }
}

// ---------------------------------------------------------------------------
// GNERerouter::routeProbReroute - methods
// ---------------------------------------------------------------------------

GNERerouter::routeProbReroute::routeProbReroute(std::string newRouteId, SUMOReal probability) :
    myNewRouteId(newRouteId),
    myProbability(probability) {
}


GNERerouter::routeProbReroute::~routeProbReroute() {
}


std::string
GNERerouter::routeProbReroute::getNewRouteId() const {
    return myNewRouteId;
}


SUMOReal
GNERerouter::routeProbReroute::getProbability() const {
    return myProbability;
}


void
GNERerouter::routeProbReroute::setProbability(SUMOReal probability) {
    if (probability >= 0 && probability <= 1) {
        myProbability = probability;
    } else {
        throw InvalidArgument(toString(probability) + " isn't a probability");
    }
}

// ---------------------------------------------------------------------------
// GNERerouter::rerouterInterval - methods
// ---------------------------------------------------------------------------

GNERerouter::rerouterInterval::rerouterInterval(SUMOReal begin, SUMOReal end) :
    std::pair<SUMOReal, SUMOReal>(begin, end) {
}


GNERerouter::rerouterInterval::~rerouterInterval() {
}


void
GNERerouter::rerouterInterval::insertClosingReroutes(GNERerouter::closingReroute* cr) {
    // Check if was already inserted
    for (std::vector<closingReroute*>::iterator i = myClosingReroutes.begin(); i != myClosingReroutes.end(); i++) {
        if ((*i) == cr) {
            throw ProcessError("closing reroute " + cr->getClosedEdgeId() + " already inserted");
        }
    }
    // insert in vector
    myClosingReroutes.push_back(cr);
}


void
GNERerouter::rerouterInterval::removeClosingReroutes(GNERerouter::closingReroute* cr) {
    // find and remove
    for (std::vector<closingReroute*>::iterator i = myClosingReroutes.begin(); i != myClosingReroutes.end(); i++) {
        if ((*i) == cr) {
            myClosingReroutes.erase(i);
            return;
        }
    }
    // Throw error if don't exist
    throw ProcessError("closing reroute " + cr->getClosedEdgeId() + " not exist");
}


void
GNERerouter::rerouterInterval::insertDestProbReroutes(GNERerouter::destProbReroute* dpr) {
    // Check if was already inserted
    for (std::vector<destProbReroute*>::iterator i = myDestProbReroutes.begin(); i != myDestProbReroutes.end(); i++) {
        if ((*i) == dpr) {
            throw ProcessError("destiny probability reroute " + dpr->getNewDestinationId() + " already inserted");
        }
    }
    // insert in vector
    myDestProbReroutes.push_back(dpr);
}


void
GNERerouter::rerouterInterval::removeDestProbReroutes(GNERerouter::destProbReroute* dpr) {
    // find and remove
    for (std::vector<destProbReroute*>::iterator i = myDestProbReroutes.begin(); i != myDestProbReroutes.end(); i++) {
        if ((*i) == dpr) {
            myDestProbReroutes.erase(i);
            return;
        }
    }
    // Throw error if don't exist
    throw ProcessError("destiny probability reroute " + dpr->getNewDestinationId() + " not exist");
}


void
GNERerouter::rerouterInterval::insertRouteProbReroute(GNERerouter::routeProbReroute* rpr) {
    // Check if was already inserted
    for (std::vector<routeProbReroute*>::iterator i = myRouteProbReroutes.begin(); i != myRouteProbReroutes.end(); i++) {
        if ((*i) == rpr) {
            throw ProcessError("route probability reroute " + rpr->getNewRouteId() + " already inserted");
        }
    }
    // insert in vector
    myRouteProbReroutes.push_back(rpr);
}


void
GNERerouter::rerouterInterval::removeRouteProbReroute(GNERerouter::routeProbReroute* rpr) {
    // find and remove
    for (std::vector<routeProbReroute*>::iterator i = myRouteProbReroutes.begin(); i != myRouteProbReroutes.end(); i++) {
        if ((*i) == rpr) {
            myRouteProbReroutes.erase(i);
            return;
        }
    }
    // Throw error if don't exist
    throw ProcessError("route probability reroute " + rpr->getNewRouteId() + " not exist");
}


SUMOReal
GNERerouter::rerouterInterval::getBegin() const {
    return first;
}


SUMOReal
GNERerouter::rerouterInterval::getEnd() const {
    return second;
}


std::vector<GNERerouter::closingReroute*>
GNERerouter::rerouterInterval::getClosingReroutes() const {
    return myClosingReroutes;
}


std::vector<GNERerouter::destProbReroute*>
GNERerouter::rerouterInterval::getDestProbReroutes() const {
    return myDestProbReroutes;
}


std::vector<GNERerouter::routeProbReroute*>
GNERerouter::rerouterInterval::getRouteProbReroutes() const {
    return myRouteProbReroutes;
}


// ---------------------------------------------------------------------------
// GNERerouter - methods
// ---------------------------------------------------------------------------

GNERerouter::GNERerouter(const std::string& id, GNEViewNet* viewNet, Position pos, std::vector<GNEEdge*> edges, const std::string& filename, SUMOReal probability, bool off, const std::set<rerouterInterval>& rerouterIntervals) :
    GNEAdditional(id, viewNet, pos, SUMO_TAG_REROUTER, ICON_REROUTER),
    myFilename(filename),
    myProbability(probability),
    myOff(off),
    myRerouterIntervals(rerouterIntervals) {
    // Update geometry;
    updateGeometry();
    // Set colors
    myBaseColor = RGBColor(76, 170, 50, 255);
    myBaseColorSelected = RGBColor(161, 255, 135, 255);
}


GNERerouter::~GNERerouter() {
}


void
GNERerouter::updateGeometry() {
    // Clear shape
    myShape.clear();

    // Set block icon position
    myBlockIconPosition = myPosition;

    // Set block icon offset
    myBlockIconOffset = Position(-0.5, -0.5);

    // Set block icon rotation, and using their rotation for draw logo
    setBlockIconRotation();

    // Set position
    myShape.push_back(myPosition);

    /*
    // Add shape of childs (To avoid graphics errors)
    for (childEdges::iterator i = myChildEdges.begin(); i != myChildEdges.end(); i++) {
        myShape.append(i->edge->getLanes().at(0)->getShape());
    }
    
    // Update geometry of additional parent
    updateConnections();
    */
    // Refresh element (neccesary to avoid grabbing problems)
    myViewNet->getNet()->refreshAdditional(this);
}


Position
GNERerouter::getPositionInView() const {
    return myPosition;
}


void
GNERerouter::openAdditionalDialog() {
    //GNERerouterDialog rerouterDialog(this);
}


void
GNERerouter::moveAdditionalGeometry(SUMOReal offsetx, SUMOReal offsety) {
    // change Position
    myPosition = Position(offsetx, offsety);
    updateGeometry();
}


void
GNERerouter::commmitAdditionalGeometryMoved(SUMOReal oldPosx, SUMOReal oldPosy, GNEUndoList* undoList) {
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPosition), true, toString(Position(oldPosx, oldPosy))));
    undoList->p_end();
    // Refresh element
    myViewNet->getNet()->refreshAdditional(this);
}


void
GNERerouter::writeAdditional(OutputDevice& device, const std::string&) {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_EDGES, /*joinToString(getEdgeChildIds(), " ").c_str()*/ "");
    device.writeAttr(SUMO_ATTR_PROB, myProbability);
    if (!myFilename.empty()) {
        device.writeAttr(SUMO_ATTR_FILE, myFilename);
    }
    device.writeAttr(SUMO_ATTR_X, myPosition.x());
    device.writeAttr(SUMO_ATTR_Y, myPosition.y());
    if (myBlocked) {
        device.writeAttr(GNE_ATTR_BLOCK_MOVEMENT, myBlocked);
    }
    // Close tag
    device.closeTag();
}


std::string
GNERerouter::getFilename() const {
    return myFilename;
}


SUMOReal
GNERerouter::getProbability() const {
    return myProbability;
}


bool
GNERerouter::getOff() const {
    return myOff;
}


void
GNERerouter::setFilename(std::string filename) {
    myFilename = filename;
}


void
GNERerouter::setProbability(SUMOReal probability) {
    myProbability = probability;
}


void
GNERerouter::setOff(bool off) {
    myOff = off;
}


const std::string&
GNERerouter::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNERerouter::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType());
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);

    // Draw icon depending of rerouter is or isn't selected
    if (isAdditionalSelected()) {
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_REROUTERSELECTED), 1);
    } else {
        GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_REROUTER), 1);
    }

    // Pop draw matrix
    glPopMatrix();

    // Show Lock icon depending of the Edit mode
    drawLockIcon(0.4);

    /*
    // Draw symbols in every lane
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    
    if (s.scale * exaggeration >= 3) {
        // draw rerouter symbol over all lanes
        
        for (childEdges::const_iterator i = myChildEdges.begin(); i != myChildEdges.end(); i++) {
            for (int lanePosIt = 0; lanePosIt < (int)i->positionsOverLanes.size(); lanePosIt++) {
                glPushMatrix();
                glTranslated(i->positionsOverLanes.at(lanePosIt).x(), i->positionsOverLanes.at(lanePosIt).y(), 0);
                glRotated(i->rotationsOverLanes.at(lanePosIt), 0, 0, 1);
                glTranslated(0, 0, getType());
                glScaled(exaggeration, exaggeration, 1);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                glBegin(GL_TRIANGLES);
                glColor3d(1, .8f, 0);
                // base
                glVertex2d(0 - 1.4, 0);
                glVertex2d(0 - 1.4, 6);
                glVertex2d(0 + 1.4, 6);
                glVertex2d(0 + 1.4, 0);
                glVertex2d(0 - 1.4, 0);
                glVertex2d(0 + 1.4, 6);
                glEnd();

                glTranslated(0, 0, .1);
                glColor3d(0, 0, 0);
                pfSetPosition(0, 0);
                pfSetScale(3.f);
                SUMOReal w = pfdkGetStringWidth("U");
                glRotated(180, 0, 1, 0);
                glTranslated(-w / 2., 2, 0);
                pfDrawString("U");

                glTranslated(w / 2., -2, 0);
                std::string str = toString((int)(myProbability * 100)) + "%";
                pfSetPosition(0, 0);
                pfSetScale(.7f);
                w = pfdkGetStringWidth(str.c_str());
                glTranslated(-w / 2., 4, 0);
                pfDrawString(str.c_str());
                glPopMatrix();
            }
        }
        glPopName();
    }

    // Draw connections
    drawConnections();
    */
    // Pop name
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


std::string
GNERerouter::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGES:
            return /*joinToString(getEdgeChildIds(), " ")*/ "";
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case SUMO_ATTR_OFF:
            return toString(myOff);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlocked);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNERerouter::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_PROB:
        case SUMO_ATTR_OFF:
        case GNE_ATTR_BLOCK_MOVEMENT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNERerouter::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (myViewNet->getNet()->getAdditional(getTag(), value) == NULL) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_EDGES: {
            std::vector<std::string> edgeIds;
            SUMOSAXAttributes::parseStringVector(value, edgeIds);
            // Empty Edges aren't valid
            if (edgeIds.empty()) {
                return false;
            }
            // Iterate over parsed edges
            for (int i = 0; i < (int)edgeIds.size(); i++) {
                if (myViewNet->getNet()->retrieveEdge(edgeIds.at(i), false) == NULL) {
                    return false;
                }
            }
            return true;
        }
        case SUMO_ATTR_POSITION:
            bool ok;
            return GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false).size() == 1;
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        case SUMO_ATTR_PROB:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_OFF:
            return canParse<bool>(value);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNERerouter::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            setAdditionalID(value);
            break;
        case SUMO_ATTR_EDGES: {
            // Declare variables
            std::vector<std::string> edgeIds;
            std::vector<GNEEdge*> edges;
            GNEEdge* edge;
            SUMOSAXAttributes::parseStringVector(value, edgeIds);
            // Iterate over parsed edges and obtain pointer to edges
            for (int i = 0; i < (int)edgeIds.size(); i++) {
                edge = myViewNet->getNet()->retrieveEdge(edgeIds.at(i), false);
                if (edge) {
                    edges.push_back(edge);
                }
            }
            // Set new childs
            //setEdgeChilds(edges);
            break;
        }
        case SUMO_ATTR_POSITION:
            bool ok;
            myPosition = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false)[0];
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_PROB:
            myProbability = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_OFF:
            myOff = parse<bool>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlocked = parse<bool>(value);
            getViewNet()->update();
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}

/****************************************************************************/
