/****************************************************************************/
/// @file    GNEEdge.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A road/street connecting two junctions (netedit-version, adapted from GUIEdge)
// Basically a container for an NBEdge with drawing and editing capabilities
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/common/MsgHandler.h>

#include "GNEEdge.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNEChange_Attribute.h"
#include "GNEChange_Lane.h"
#include "GNEJunction.h"
#include "GNELane.h"
#include "GNEAdditional.h"
#include "GNEConnection.h"
#include "GNERouteProbe.h"
#include "GNEVaporizer.h"
#include "GNERerouter.h"
#include "GNECrossing.h"




// ===========================================================================
// static
// ===========================================================================
const double GNEEdge::SNAP_RADIUS = SUMO_const_halfLaneWidth;

// ===========================================================================
// members methods
// ===========================================================================
GNEEdge::GNEEdge(NBEdge& nbe, GNENet* net, bool wasSplit, bool loaded):
    GNENetElement(net, nbe.getID(), GLO_EDGE, SUMO_TAG_EDGE, ICON_EDGE),
    myNBEdge(nbe) ,
    myGNEJunctionSource(myNet->retrieveJunction(myNBEdge.getFromNode()->getID())),
    myGNEJunctionDestiny(myNet->retrieveJunction(myNBEdge.getToNode()->getID())),
    myOrigShape(nbe.getInnerGeometry()),
    myLanes(0),
    myAmResponsible(false),
    myWasSplit(wasSplit),
    myConnectionStatus(loaded ? LOADED : GUESSED) {
    // Create lanes
    int numLanes = myNBEdge.getNumLanes();
    myLanes.reserve(numLanes);
    for (int i = 0; i < numLanes; i++) {
        myLanes.push_back(new GNELane(*this, i));
        myLanes.back()->incRef("GNEEdge::GNEEdge");
    }
    // update Lane geometries
    for (LaneVector::iterator i = myLanes.begin(); i != myLanes.end(); i++) {
        (*i)->updateGeometry();
    }
}


GNEEdge::~GNEEdge() {
    // Delete references to this eddge in lanes
    for (LaneVector::iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        (*i)->decRef("GNEEdge::~GNEEdge");
        if ((*i)->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting unreferenced " + toString((*i)->getTag()) + " '" + (*i)->getID() + "' in GNEEdge destructor");
            }
            delete *i;
        }
    }
    // delete references to this eddge in connections
    for (ConnectionVector::const_iterator i = myGNEConnections.begin(); i != myGNEConnections.end(); ++i) {
        (*i)->decRef("GNEEdge::~GNEEdge");
        if ((*i)->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting unreferenced " + toString((*i)->getTag()) + " '" + (*i)->getID() + "' in GNEEdge destructor");
            }
            delete(*i);
        }
    }
    if (myAmResponsible) {
        delete &myNBEdge;
    }
}


void
GNEEdge::updateGeometry() {
    // Update geometry of lanes
    for (LaneVector::iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        (*i)->updateGeometry();
    }
    // Update geometry of additionals vinculated to this edge
    for (AdditionalVector::iterator i = myAdditionals.begin(); i != myAdditionals.end(); ++i) {
        (*i)->updateGeometry();
    }
}


void GNEEdge::commitGeometryMoving(const PositionVector & oldShape, GNEUndoList * undoList) {

    // shape has to be cleaned
    PositionVector cleanedShape = myNBEdge.getInnerGeometry();
    auto it = cleanedShape.begin();
    // iterate over shape and remove nearest point to Junctions
    while (it != cleanedShape.end()) {
        if (it->distanceTo2D(myGNEJunctionSource->getPositionInView()) < POSITION_EPS) {
            it = cleanedShape.erase(it);
        }
        else if (it->distanceTo2D(myGNEJunctionDestiny->getPositionInView()) < POSITION_EPS) {
            it = cleanedShape.erase(it);
        }
        else {
            it++;
        }
    }
    if(oldShape != cleanedShape) {
        undoList->p_begin("moving " + toString(SUMO_ATTR_SHAPE) + " of " + toString(getTag()));
        myNBEdge.setGeometry(myOrigShape, true);
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_SHAPE, toString(cleanedShape)));
        undoList->p_end();
    } else {
        setGeometry(oldShape, true);
    }

}


Boundary
GNEEdge::getBoundary() const {
    Boundary ret;
    for (LaneVector::const_iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        ret.add((*i)->getBoundary());
    }
    ret.grow(10); // !!! magic value
    return ret;
}


Boundary
GNEEdge::getCenteringBoundary() const {
    Boundary b = getBoundary();
    b.grow(20);
    return b;
}


GUIGLObjectPopupMenu*
GNEEdge::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GNEJunction*
GNEEdge::getGNEJunctionSource() const {
    return myGNEJunctionSource;
}


GNEJunction*
GNEEdge::getGNEJunctionDestiny() const {
    return myGNEJunctionDestiny;
}

void
GNEEdge::drawGL(const GUIVisualizationSettings& s) const {
    /* do something different for connectors?
    if (myNBEdge.isMacroscopicConnector()) {
    }
    */

    // draw the lanes
    for (LaneVector::const_iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        (*i)->drawGL(s);
    }

    // draw the connections
    if (s.scale >= 2) {
        for (ConnectionVector::const_iterator i = myGNEConnections.begin(); i != myGNEConnections.end(); ++i) {
            (*i)->drawGL(s);
        }
    }

    // draw geometry hints
    if (s.scale * SNAP_RADIUS > 1.) { // check whether it is not too small
        GLHelper::setColor(s.junctionColorer.getSchemes()[0].getColor(2));
        if (gSelected.isSelected(getType(), getGlID()) && s.laneColorer.getActive() != 1) {
            // override with special colors (unless the color scheme is based on selection)
            GLHelper::setColor(GNENet::selectionColor.changedBrightness(-20));
        }
        // recognize full transparency and simply don't draw
        GLfloat color[4];
        glGetFloatv(GL_CURRENT_COLOR, color);
        if (color[3] > 0) {
            glPushName(getGlID());
            PositionVector geom = myNBEdge.getGeometry();
            // draw geometry points
            for (int i = 1; i < (int)geom.size() - 1; i++) {
                Position pos = geom[i];
                glPushMatrix();
                glTranslated(pos.x(), pos.y(), GLO_JUNCTION - 0.01);
                // resolution of drawn circle depending of the zoom (To improve smothness)
                if(s.scale >= 10) {
                    GLHelper:: drawFilledCircle(SNAP_RADIUS * MIN2((double)1, s.laneWidthExaggeration), 32);
                } else if (s.scale >= 2) {
                    GLHelper:: drawFilledCircle(SNAP_RADIUS * MIN2((double)1, s.laneWidthExaggeration), 16);
                } else if (s.scale >= 1) {
                    GLHelper:: drawFilledCircle(SNAP_RADIUS * MIN2((double)1, s.laneWidthExaggeration), 8);
                } else {
                    GLHelper:: drawFilledCircle(SNAP_RADIUS * MIN2((double)1, s.laneWidthExaggeration), 4);
                }
                glPopMatrix();
            }
            glPopName();
        }
    }

    // (optionally) draw the name and/or the street name
    const bool drawStreetName = s.streetName.show && myNBEdge.getStreetName() != "";
    if (s.edgeName.show || drawStreetName) {
        glPushName(getGlID());
        GNELane* lane1 = myLanes[0];
        GNELane* lane2 = myLanes[myLanes.size() - 1];
        Position p = lane1->getShape().positionAtOffset(lane1->getShape().length() / (double) 2.);
        p.add(lane2->getShape().positionAtOffset(lane2->getShape().length() / (double) 2.));
        p.mul(.5);
        double angle = lane1->getShape().rotationDegreeAtOffset(lane1->getShape().length() / (double) 2.);
        angle += 90;
        if (angle > 90 && angle < 270) {
            angle -= 180;
        }
        if (s.edgeName.show) {
            drawName(p, s.scale, s.edgeName, angle);
        }
        if (drawStreetName) {
            GLHelper::drawText(myNBEdge.getStreetName(), p, GLO_MAX,
                               s.streetName.size / s.scale, s.streetName.color, angle);
        }
        glPopName();
    }
}


GUIParameterTableWindow*
GNEEdge::getParameterWindow(GUIMainWindow& app,
                            GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = 0;
    UNUSED_PARAMETER(&app);
    return ret;
}


void
GNEEdge::updateJunctionPosition(GNEJunction* junction, const Position& origPos) {
    Position delta = junction->getNBNode()->getPosition() - origPos;
    PositionVector geom = myNBEdge.getGeometry();
    // geometry endpoint need not equal junction position hence we modify it with delta
    if (junction == myGNEJunctionSource) {
        geom[0].add(delta);
    } else {
        geom[-1].add(delta);
    }
    setGeometry(geom, false);
}

NBEdge*
GNEEdge::getNBEdge() {
    return &myNBEdge;
}


Position
GNEEdge::getSplitPos(const Position& clickPos) {
    const PositionVector& geom = myNBEdge.getGeometry();
    int index = geom.indexOfClosest(clickPos);
    if (geom[index].distanceTo(clickPos) < SNAP_RADIUS) {
        // split at existing geometry point
        return geom[index];
    } else {
        // split straight between the next two points
        return geom.positionAtOffset(geom.nearest_offset_to_point2D(clickPos));
    }
}


Position
GNEEdge::moveGeometry(const Position& oldPos, const Position& newPos, bool relative) {
    PositionVector geom = myNBEdge.getGeometry();
    bool changed = changeGeometry(geom, getMicrosimID(), oldPos, newPos, relative);
    if (changed) {
        setGeometry(geom, false);
        return newPos;
    } else {
        return oldPos;
    }
}


bool
GNEEdge::changeGeometry(PositionVector& geom, const std::string& id, const Position& oldPos, const Position& newPos, bool relative, bool moveEndPoints) {
    if (geom.size() < 2) {
        throw ProcessError("Invalid geometry size in " + toString(SUMO_TAG_EDGE) + " with ID='" + id + "'");
    } else {
        int index = geom.indexOfClosest(oldPos);
        const double nearestOffset = geom.nearest_offset_to_point2D(oldPos, true);
        if (nearestOffset != GeomHelper::INVALID_OFFSET
                && (moveEndPoints || (nearestOffset >= SNAP_RADIUS
                                      && nearestOffset <= geom.length2D() - SNAP_RADIUS))) {
            const Position nearest = geom.positionAtOffset2D(nearestOffset);
            const double distance = geom[index].distanceTo2D(nearest);
            if (distance < SNAP_RADIUS) { //move existing
                if (moveEndPoints || (index != 0 && index != (int)geom.size() - 1)) {
                    const bool closed = geom.isClosed();
                    if (relative) {
                        geom[index] = geom[index] + newPos;
                    } else {
                        geom[index] = newPos;
                    }
                    if (closed && moveEndPoints && (index == 0 || index == (int)geom.size() - 1)) {
                        const int otherIndex = (int)geom.size() - 1 - index;
                        geom[otherIndex] = geom[index];
                    }
                    return true;
                }
            } else {
                if (relative) {
                    int index = geom.insertAtClosest(nearest);
                    geom[index] = geom[index] + newPos;
                    return true;
                } else {
                    geom.insertAtClosest(newPos); // insert new
                    return true;
                }
            }
        }
        return false;
    }
}


void
GNEEdge::moveGeometry(const Position& delta) {
    PositionVector geom = myNBEdge.getInnerGeometry();
    if (geom.size() == 0) {
        return;
    }
    geom.add(delta.x(), delta.y(), delta.z());
    setGeometry(geom, true);
}


bool
GNEEdge::deleteGeometry(const Position& pos, GNEUndoList* undoList) {
    PositionVector geom = myNBEdge.getInnerGeometry();
    if (geom.size() == 0) {
        return false;
    }
    int index = geom.indexOfClosest(pos);
    if (geom[index].distanceTo(pos) < SNAP_RADIUS) {
        geom.erase(geom.begin() + index);
        setAttribute(SUMO_ATTR_SHAPE, toString(geom), undoList);
        return true;
    } else {
        return false;
    }
}


void
GNEEdge::setEndpoint(Position pos, GNEUndoList* undoList) {
    undoList->p_begin("set endpoint");
    PositionVector geom = myNBEdge.getGeometry();
    int index = geom.indexOfClosest(pos);
    if (geom[index].distanceTo(pos) < SNAP_RADIUS) { // snap to existing geometry
        pos = geom[index];
    }
    Position destPos = myGNEJunctionDestiny->getNBNode()->getPosition();
    Position sourcePos = myGNEJunctionSource->getNBNode()->getPosition();
    if (pos.distanceTo2D(destPos) < pos.distanceTo2D(sourcePos)) {
        setAttribute(GNE_ATTR_SHAPE_END, toString(pos), undoList);
        myGNEJunctionDestiny->invalidateShape();
    } else {
        setAttribute(GNE_ATTR_SHAPE_START, toString(pos), undoList);
        myGNEJunctionSource->invalidateShape();
    }
    // possibly existing inner point is no longer needed
    deleteGeometry(pos, undoList);
    undoList->p_end();
}


void
GNEEdge::resetEndpoint(const Position& pos, GNEUndoList* undoList) {
    Position destPos = myGNEJunctionDestiny->getNBNode()->getPosition();
    Position sourcePos = myGNEJunctionSource->getNBNode()->getPosition();
    if (pos.distanceTo2D(destPos) < pos.distanceTo2D(sourcePos)) {
        setAttribute(GNE_ATTR_SHAPE_END, toString(destPos), undoList);
        myGNEJunctionDestiny->invalidateShape();
    } else {
        setAttribute(GNE_ATTR_SHAPE_START, toString(sourcePos), undoList);
        myGNEJunctionSource->invalidateShape();
    }
}


void
GNEEdge::setGeometry(PositionVector geom, bool inner) {
    myNBEdge.setGeometry(geom, inner);
    updateGeometry();
    myGNEJunctionSource->invalidateShape();
    myGNEJunctionDestiny->invalidateShape();
    myNet->refreshElement(this);
}


void
GNEEdge::remakeGNEConnections() {
    // create new and removed unused GNEConnectinos
    const std::vector<NBEdge::Connection>& connections = myNBEdge.getConnections();
    // create a vector to keep retrieved and created connections
    std::vector<GNEConnection*> retrievedConnections;
    // iterate over NBEdge::Connections of GNEEdge
    for (std::vector<NBEdge::Connection>::const_iterator it = connections.begin(); it != connections.end(); it++) {
        // retrieve existent GNEConnection, or create it
        GNEConnection* retrievedGNEConnection = retrieveGNEConnection(it->fromLane, it->toEdge, it->toLane);
        retrievedConnections.push_back(retrievedGNEConnection);
        // check if previously this GNEConnections exists, and if true, remove it from myGNEConnections
        std::vector<GNEConnection*>::iterator retrievedExists = std::find(myGNEConnections.begin(), myGNEConnections.end(), retrievedGNEConnection);
        if(retrievedExists != myGNEConnections.end()) {
            myGNEConnections.erase(retrievedExists);
        } else {
            // include reference to created GNEConnection
            retrievedGNEConnection->incRef("GNEEdge::remakeGNEConnections");
            retrievedGNEConnection->updateLinkState();
        }
    }
    // delete non retrieved GNEConnections
    for (std::vector<GNEConnection*>::const_iterator it = myGNEConnections.begin(); it != myGNEConnections.end(); it++) {
        (*it)->decRef();
        if ((*it)->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting unreferenced " + toString((*it)->getTag()) + " '" + (*it)->getID() + "' in rebuildGNEConnections()");
            }
            delete *it;
        }
    }
    // copy retrieved (existent and created) GNECrossigns to myGNEConnections 
    myGNEConnections = retrievedConnections;
}


void
GNEEdge::clearGNEConnections() {
    // Drop all existents connections that aren't referenced anymore
    for (ConnectionVector::iterator i = myGNEConnections.begin(); i != myGNEConnections.end(); i++) {
        // Dec reference of connection
        (*i)->decRef("GNEEdge::clearGNEConnections");
        // Delete GNEConnectionToErase if is unreferenced
        if ((*i)->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting unreferenced " + toString((*i)->getTag()) + " '" + (*i)->getID() + "' in clearGNEConnections()");
            }
            delete(*i);
        }
    }
    myGNEConnections.clear();
}


int
GNEEdge::getRouteProbeRelativePosition(GNERouteProbe* routeProbe) const {
    AdditionalVector routeProbes;
    for (AdditionalVector::const_iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++) {
        if ((*i)->getTag() == routeProbe->getTag()) {
            routeProbes.push_back(*i);
        }
    }
    // return index of routeProbe in routeProbes vector
    AdditionalVector::const_iterator it = std::find(routeProbes.begin(), routeProbes.end(), routeProbe);
    if (it == routeProbes.end()) {
        return -1;
    } else {
        return (int)(it - routeProbes.begin());
    }
}


int
GNEEdge::getVaporizerRelativePosition(GNEVaporizer* vaporizer) const {
    AdditionalVector vaporizers;
    for (AdditionalVector::const_iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++) {
        if ((*i)->getTag() == vaporizer->getTag()) {
            vaporizers.push_back(*i);
        }
    }
    // return index of routeProbe in routeProbes vector
    AdditionalVector::const_iterator it = std::find(vaporizers.begin(), vaporizers.end(), vaporizer);
    if (it == vaporizers.end()) {
        return -1;
    } else {
        return (int)(it - vaporizers.begin());
    }
}


void
GNEEdge::copyTemplate(GNEEdge* tpl, GNEUndoList* undoList) {
    undoList->p_begin("copy template");
    setAttribute(SUMO_ATTR_NUMLANES,   tpl->getAttribute(SUMO_ATTR_NUMLANES),  undoList);
    setAttribute(SUMO_ATTR_TYPE,       tpl->getAttribute(SUMO_ATTR_TYPE),     undoList);
    setAttribute(SUMO_ATTR_SPEED,      tpl->getAttribute(SUMO_ATTR_SPEED),    undoList);
    setAttribute(SUMO_ATTR_PRIORITY,   tpl->getAttribute(SUMO_ATTR_PRIORITY), undoList);
    setAttribute(SUMO_ATTR_SPREADTYPE, tpl->getAttribute(SUMO_ATTR_SPREADTYPE), undoList);
    setAttribute(SUMO_ATTR_WIDTH,      tpl->getAttribute(SUMO_ATTR_WIDTH), undoList);
    setAttribute(SUMO_ATTR_ENDOFFSET,     tpl->getAttribute(SUMO_ATTR_ENDOFFSET), undoList);
    // copy lane attributes as well
    for (int i = 0; i < (int)myLanes.size(); i++) {
        myLanes[i]->setAttribute(SUMO_ATTR_ALLOW, tpl->myLanes[i]->getAttribute(SUMO_ATTR_ALLOW), undoList);
        myLanes[i]->setAttribute(SUMO_ATTR_DISALLOW, tpl->myLanes[i]->getAttribute(SUMO_ATTR_DISALLOW), undoList);
        myLanes[i]->setAttribute(SUMO_ATTR_SPEED, tpl->myLanes[i]->getAttribute(SUMO_ATTR_SPEED), undoList);
        myLanes[i]->setAttribute(SUMO_ATTR_WIDTH, tpl->myLanes[i]->getAttribute(SUMO_ATTR_WIDTH), undoList);
        myLanes[i]->setAttribute(SUMO_ATTR_ENDOFFSET, tpl->myLanes[i]->getAttribute(SUMO_ATTR_ENDOFFSET), undoList);
    }
    undoList->p_end();
}


std::set<GUIGlID>
GNEEdge::getLaneGlIDs() {
    std::set<GUIGlID> result;
    for (size_t i = 0; i < myLanes.size(); i++) {
        result.insert(myLanes[i]->getGlID());
    }
    return result;
}


const std::vector<GNELane*>&
GNEEdge::getLanes() {
    return myLanes;
}


const std::vector<GNEConnection*>&
GNEEdge::getGNEConnections() {
    return myGNEConnections;
}


bool
GNEEdge::wasSplit() {
    return myWasSplit;
}


std::string
GNEEdge::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_FROM:
            return myGNEJunctionSource->getMicrosimID();
        case SUMO_ATTR_TO:
            return myGNEJunctionDestiny->getMicrosimID();
        case SUMO_ATTR_NUMLANES:
            return toString(myNBEdge.getNumLanes());
        case SUMO_ATTR_PRIORITY:
            return toString(myNBEdge.getPriority());
        case SUMO_ATTR_LENGTH:
            return toString(myNBEdge.getFinalLength());
        case SUMO_ATTR_TYPE:
            return myNBEdge.getTypeID();
        case SUMO_ATTR_SHAPE:
            return toString(myNBEdge.getInnerGeometry());
        case SUMO_ATTR_SPREADTYPE:
            return toString(myNBEdge.getLaneSpreadFunction());
        case SUMO_ATTR_NAME:
            return myNBEdge.getStreetName();
        case SUMO_ATTR_ALLOW:
            return (getVehicleClassNames(myNBEdge.getPermissions()) + (myNBEdge.hasLaneSpecificPermissions() ? " (combined!)" : ""));
        case SUMO_ATTR_DISALLOW: {
            return (getVehicleClassNames(invertPermissions(myNBEdge.getPermissions())) + (myNBEdge.hasLaneSpecificPermissions() ? " (combined!)" : ""));
        }
        case SUMO_ATTR_SPEED:
            if (myNBEdge.hasLaneSpecificSpeed()) {
                return "lane specific";
            } else {
                return toString(myNBEdge.getSpeed());
            }
        case SUMO_ATTR_WIDTH:
            if (myNBEdge.hasLaneSpecificWidth()) {
                return "lane specific";
            } else if (myNBEdge.getLaneWidth() == NBEdge::UNSPECIFIED_WIDTH) {
                return "default";
            } else {
                return toString(myNBEdge.getLaneWidth());
            }
        case SUMO_ATTR_ENDOFFSET:
            if (myNBEdge.hasLaneSpecificEndOffset()) {
                return "lane specific";
            } else {
                return toString(myNBEdge.getEndOffset());
            }
        case GNE_ATTR_MODIFICATION_STATUS:
            return myConnectionStatus;
        case GNE_ATTR_SHAPE_START:
            if (myNBEdge.getGeometry()[0] == myGNEJunctionSource->getPositionInView()) {
                return "";
            } else {
                return toString(myNBEdge.getGeometry()[0]);
            }
        case GNE_ATTR_SHAPE_END:
            if (myNBEdge.getGeometry()[-1] == myGNEJunctionDestiny->getPositionInView()) {
                return "";
            } else {
                return toString(myNBEdge.getGeometry()[-1]);
            }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

std::string 
GNEEdge::getAttributeForSelection(SumoXMLAttr key) const {
    std::string result = getAttribute(key);
    if ((key == SUMO_ATTR_ALLOW || key == SUMO_ATTR_DISALLOW) && result.find("all") != std::string::npos) {
        result += " " + getVehicleClassNames(SVCAll, true);
    }
    return result;
}

void
GNEEdge::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_ENDOFFSET:
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW: {
            undoList->p_begin("change " + toString(getTag()) + " attribute");
            const std::string origValue = myLanes.at(0)->getAttribute(key); // will have intermediate value of "lane specific"
            // lane specific attributes need to be changed via lanes to allow undo
            for (LaneVector::iterator it = myLanes.begin(); it != myLanes.end(); it++) {
                (*it)->setAttribute(key, value, undoList);
            }
            // ensure that the edge value is also changed. Actually this sets the lane attributes again but it does not matter
            undoList->p_add(new GNEChange_Attribute(this, key, value, true, origValue));
            undoList->p_end();
            break;
        }
        case SUMO_ATTR_FROM: {
            undoList->p_begin("change  " + toString(getTag()) + "  attribute");
            // Remove edge from crossings of junction source
            removeEdgeFromCrossings(myGNEJunctionSource, undoList);
            // continue changing from junction
            GNEJunction *oldGNEJunctionSource = myGNEJunctionSource;
            myGNEJunctionSource->setLogicValid(false, undoList);
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            myGNEJunctionSource->setLogicValid(false, undoList);
            myNet->retrieveJunction(value)->setLogicValid(false, undoList);
            setAttribute(GNE_ATTR_SHAPE_START, toString(myGNEJunctionSource->getNBNode()->getPosition()), undoList);
            myGNEJunctionSource->invalidateShape();
            undoList->p_end();
            // update geometries of all implicated junctions
            oldGNEJunctionSource->updateGeometry();
            myGNEJunctionSource->updateGeometry();
            myGNEJunctionDestiny->updateGeometry();
            break;
        }
        case SUMO_ATTR_TO: {
            undoList->p_begin("change  " + toString(getTag()) + "  attribute");
            // Remove edge from crossings of junction destiny
            removeEdgeFromCrossings(myGNEJunctionDestiny, undoList);
            // continue changing destiny junction
            GNEJunction *oldGNEJunctionDestiny = myGNEJunctionDestiny;
            myGNEJunctionDestiny->setLogicValid(false, undoList);
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            myGNEJunctionDestiny->setLogicValid(false, undoList);
            myNet->retrieveJunction(value)->setLogicValid(false, undoList);
            setAttribute(GNE_ATTR_SHAPE_END, toString(myGNEJunctionDestiny->getNBNode()->getPosition()), undoList);
            myGNEJunctionDestiny->invalidateShape();
            undoList->p_end();
            // update geometries of all implicated junctions
            oldGNEJunctionDestiny->updateGeometry();
            myGNEJunctionDestiny->updateGeometry();
            myGNEJunctionSource->updateGeometry();
            break;
        }
        case SUMO_ATTR_ID:
        case SUMO_ATTR_PRIORITY:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_SPREADTYPE:
        case GNE_ATTR_MODIFICATION_STATUS:
        case GNE_ATTR_SHAPE_START:
        case GNE_ATTR_SHAPE_END:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        case SUMO_ATTR_NAME:
            // user cares about street names. Make sure they appear in the output
            OptionsCont::getOptions().resetWritable();
            OptionsCont::getOptions().set("output.street-names", "true");
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        case SUMO_ATTR_NUMLANES:
            if (value != getAttribute(key)) {
                // Remove edge from crossings of junction source
                removeEdgeFromCrossings(myGNEJunctionSource, undoList);
                // Remove edge from crossings of junction destiny
                removeEdgeFromCrossings(myGNEJunctionDestiny, undoList);
                // set num lanes
                setNumLanes(parse<int>(value), undoList);
            }
            break;
        case SUMO_ATTR_SHAPE:
            // @note: assumes value of inner geometry!
            // actually the geometry is already updated (incrementally
            // during mouse movement). We set the restore point to the end
            // of the last change-set
            myNBEdge.setGeometry(myOrigShape, true);
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEEdge::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidID(value) && (myNet->retrieveEdge(value, false) == 0);
        case SUMO_ATTR_FROM: {
            // check that is a valid ID and is different of ID of junction destiny
            if(isValidID(value) && (value != myGNEJunctionDestiny->getMicrosimID())) {
                GNEJunction *junctionFrom = myNet->retrieveJunction(value, false);
                // check that there isn't already another edge with the same From and To Edge
                if((junctionFrom != NULL) && (myNet->retrieveEdge(junctionFrom, myGNEJunctionDestiny, false) == NULL)) {
                    return true;
                } else {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        case SUMO_ATTR_TO: {
            // check that is a valid ID and is different of ID of junction Source
            if(isValidID(value) && (value != myGNEJunctionSource->getMicrosimID())) {
                GNEJunction *junctionTo = myNet->retrieveJunction(value, false);
                // check that there isn't already another edge with the same From and To Edge
                if((junctionTo != NULL) && (myNet->retrieveEdge(myGNEJunctionSource, junctionTo, false) == NULL)) {
                    return true;
                } else {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        case SUMO_ATTR_SPEED:
            return canParse<double>(value) && isPositive<double>(value);
        case SUMO_ATTR_NUMLANES:
            return canParse<int>(value) && isPositive<int>(value);
        case SUMO_ATTR_PRIORITY:
            return canParse<int>(value);
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value) && (isPositive<double>(value) || (parse<double>(value) == NBEdge::UNSPECIFIED_LOADED_LENGTH));
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_TYPE:
            return true;
        case SUMO_ATTR_SHAPE: {
            bool ok = true;
            PositionVector shape = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, true);
            return ok;
        }
        case SUMO_ATTR_SPREADTYPE:
            return SUMOXMLDefinitions::LaneSpreadFunctions.hasString(value);
        case SUMO_ATTR_NAME:
            return true;
        case SUMO_ATTR_WIDTH:
            if (value == "default") {
                return true;
            } else {
                return canParse<double>(value) && (isPositive<double>(value) || (parse<double>(value) == NBEdge::UNSPECIFIED_WIDTH));
            }
        case SUMO_ATTR_ENDOFFSET:
            return canParse<double>(value);
        case GNE_ATTR_SHAPE_START: {
            bool ok;
            if(value != "") {
                PositionVector shapeStart = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false);
                if((shapeStart.size() == 1) && (shapeStart[0] != myNBEdge.getGeometry()[-1])) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return true;
            }
        }
        case GNE_ATTR_SHAPE_END: {
            bool ok;
            if(value != "") {
                PositionVector shapeStart = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false);
                if((shapeStart.size() == 1) && (shapeStart[0] != myNBEdge.getGeometry()[0])) {
                    return true;
                } else {
                    return false;
                }
            } else {
                return true;
            }
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEEdge::setResponsible(bool newVal) {
    myAmResponsible = newVal;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEEdge::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myNet->renameEdge(this, value);
            break;
        case SUMO_ATTR_FROM:
            myNet->changeEdgeEndpoints(this, value, myGNEJunctionDestiny->getMicrosimID());
            // update this edge of list of outgoings edges of the old GNEJunctionSource
            myGNEJunctionSource->removeOutgoingGNEEdge(this);
            // update GNEJunctionSource
            myGNEJunctionSource = myNet->retrieveJunction(myNBEdge.getFromNode()->getID());
            // update this edge of list of outgoings edges of the new GNEJunctionSource
            myGNEJunctionSource->addOutgoingGNEEdge(this);
            break;
        case SUMO_ATTR_TO:
            myNet->changeEdgeEndpoints(this, myGNEJunctionSource->getMicrosimID(), value);
            // update this edge of list of incomings edges of the old GNEJunctionDestiny
            myGNEJunctionDestiny->removeIncomingGNEEdge(this);
            // update GNEJunctionDestiny
            myGNEJunctionDestiny = myNet->retrieveJunction(myNBEdge.getToNode()->getID());
            // update this edge of list of incomings edges of the new GNEJunctionDestiny
            myGNEJunctionDestiny->addIncomingGNEEdge(this);
            break;
        case SUMO_ATTR_NUMLANES:
            throw InvalidArgument("GNEEdge::setAttribute (private) called for attr SUMO_ATTR_NUMLANES. This should never happen");
            break;
        case SUMO_ATTR_PRIORITY:
            myNBEdge.myPriority = parse<int>(value);
            break;
        case SUMO_ATTR_LENGTH:
            myNBEdge.setLoadedLength(parse<double>(value));
            break;
        case SUMO_ATTR_TYPE:
            myNBEdge.myType = value;
            break;
        case SUMO_ATTR_SHAPE:
            bool ok;
            myOrigShape = GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, true);
            setGeometry(myOrigShape, true);
            break;
        case SUMO_ATTR_SPREADTYPE:
            myNBEdge.setLaneSpreadFunction(SUMOXMLDefinitions::LaneSpreadFunctions.get(value));
            break;
        case SUMO_ATTR_NAME:
            myNBEdge.setStreetName(value);
            break;
        case SUMO_ATTR_SPEED:
            myNBEdge.setSpeed(-1, parse<double>(value));
            break;
        case SUMO_ATTR_WIDTH:
            if (value == "default") {
                myNBEdge.setLaneWidth(-1, NBEdge::UNSPECIFIED_WIDTH);
            } else {
                myNBEdge.setLaneWidth(-1, parse<double>(value));
            }
            break;
        case SUMO_ATTR_ENDOFFSET:
            myNBEdge.setEndOffset(-1, parse<double>(value));
            break;
        case SUMO_ATTR_ALLOW:
            break;  // no edge value
        case SUMO_ATTR_DISALLOW:
            break; // no edge value
        case GNE_ATTR_MODIFICATION_STATUS:
            myConnectionStatus = value;
            if (value == GUESSED) {
                myNBEdge.invalidateConnections(true);
                clearGNEConnections();
            } else if (value != GUESSED) {
                myNBEdge.declareConnectionsAsLoaded();
            }
            break;
        case GNE_ATTR_SHAPE_START: {
            // get geometry of NBEdge, remove FIRST element with the new value (or with the Junction Source position) and set it back to edge
            PositionVector geom = myNBEdge.getGeometry();
            geom.erase(geom.begin());
            if (value == "") {
                geom.push_front_noDoublePos(myGNEJunctionSource->getPositionInView());
            } else {
                geom.push_front_noDoublePos(GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, false)[0]);
            }
            setGeometry(geom, false);
            break;
        }
        case GNE_ATTR_SHAPE_END: {
            // get geometry of NBEdge, remove LAST element with the new value (or with the Junction Destiny position) and set it back to edge
            PositionVector geom = myNBEdge.getGeometry();
            geom.pop_back();
            if (value == "") {
                geom.push_back_noDoublePos(myGNEJunctionDestiny->getPositionInView());
            } else {
                geom.push_back_noDoublePos(GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, false)[0]);
            }
            setGeometry(geom, false);
            break;
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEEdge::setNumLanes(int numLanes, GNEUndoList* undoList) {
    undoList->p_begin("change number of " + toString(SUMO_TAG_LANE) +  "s");
    myGNEJunctionSource->setLogicValid(false, undoList);
    myGNEJunctionDestiny->setLogicValid(false, undoList);

    const int oldNumLanes = (int)myLanes.size();
    for (int i = oldNumLanes; i < numLanes; i++) {
        // since the GNELane does not exist yet, it cannot have yet been referenced so we only pass a zero-pointer
        undoList->add(new GNEChange_Lane(this, 0,
                                         myNBEdge.getLaneStruct(oldNumLanes - 1), true), true);
    }
    for (int i = oldNumLanes - 1; i > numLanes - 1; i--) {
        // delete leftmost lane
        undoList->add(new GNEChange_Lane(this, myLanes[i], myNBEdge.getLaneStruct(i), false), true);
    }
    undoList->p_end();
}


void
GNEEdge::addLane(GNELane* lane, const NBEdge::Lane& laneAttrs) {
    const int index = lane ? lane->getIndex() : myNBEdge.getNumLanes();
    // the laneStruct must be created first to ensure we have some geometry
    myNBEdge.addLane(index);
    if (lane) {
        // restore a previously deleted lane
        myLanes.insert(myLanes.begin() + index, lane);

    } else {
        // create a new lane by copying leftmost lane
        lane = new GNELane(*this, index);
        myLanes.push_back(lane);
    }
    lane->incRef("GNEEdge::addLane");
    // we copy all attributes except shape since this is recomputed from edge shape
    myNBEdge.setSpeed(lane->getIndex(), laneAttrs.speed);
    myNBEdge.setPermissions(laneAttrs.permissions, lane->getIndex());
    myNBEdge.setPreferredVehicleClass(laneAttrs.preferred, lane->getIndex());
    myNBEdge.setEndOffset(lane->getIndex(), laneAttrs.endOffset);
    myNBEdge.setLaneWidth(lane->getIndex(), laneAttrs.width);
    // udate indices
    for (int i = 0; i < (int)myLanes.size(); ++i) {
        myLanes[i]->setIndex(i);
    }
    /* while technically correct, this looks ugly
    myGNEJunctionSource->invalidateShape();
    myGNEJunctionDestiny->invalidateShape();
    */
    // Remake connections for this edge and all edges that target this lane
    remakeGNEConnections();
    // remake connections of all edges of junction source and destiny
    for(std::vector<GNEEdge*>::const_iterator i = myGNEJunctionSource->getGNEEdges().begin(); i != myGNEJunctionSource->getGNEEdges().end(); i++) {
        (*i)->remakeGNEConnections();
    }
    // remake connections of all edges of junction source and destiny
    for(std::vector<GNEEdge*>::const_iterator i = myGNEJunctionDestiny->getGNEEdges().begin(); i != myGNEJunctionDestiny->getGNEEdges().end(); i++) {
        (*i)->remakeGNEConnections();
    }
    // Update element
    myNet->refreshElement(this);
    updateGeometry();
}


void
GNEEdge::removeLane(GNELane* lane) {
    if (myLanes.size() == 0) {
        throw ProcessError("Should not remove the last " + toString(SUMO_TAG_LANE) + " from an " + toString(getTag()));
    }
    if (lane == 0) {
        lane = myLanes.back();
    }
    // Delete lane of edge's container
    myNBEdge.deleteLane(lane->getIndex());
    lane->decRef("GNEEdge::removeLane");
    myLanes.erase(myLanes.begin() + lane->getIndex());
    // Delete lane if is unreferenced
    if (lane->unreferenced()) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Deleting unreferenced " + toString(lane->getTag()) + " '" + lane->getID() + "' in removeLane()");
        }
        delete lane;
    }
    // udate indices
    for (int i = 0; i < (int)myLanes.size(); ++i) {
        myLanes[i]->setIndex(i);
    }
    /* while technically correct, this looks ugly
    myGNEJunctionSource->invalidateShape();
    myGNEJunctionDestiny->invalidateShape();
    */
    // Remake connections of this edge
    remakeGNEConnections();
    // remake connections of all edges of junction source and destiny
    for(std::vector<GNEEdge*>::const_iterator i = myGNEJunctionSource->getGNEEdges().begin(); i != myGNEJunctionSource->getGNEEdges().end(); i++) {
        (*i)->remakeGNEConnections();
    }
    // remake connections of all edges of junction source and destiny
    for(std::vector<GNEEdge*>::const_iterator i = myGNEJunctionDestiny->getGNEEdges().begin(); i != myGNEJunctionDestiny->getGNEEdges().end(); i++) {
        (*i)->remakeGNEConnections();
    }
    // Update element
    myNet->refreshElement(this);
    updateGeometry();
}


void
GNEEdge::addConnection(NBEdge::Connection nbCon, bool selectAfterCreation) {
    // If a new connection was sucesfully created
    if (myNBEdge.setConnection(nbCon.fromLane, nbCon.toEdge, nbCon.toLane, NBEdge::L2L_USER, true, nbCon.mayDefinitelyPass, nbCon.keepClear, nbCon.contPos, nbCon.visibility)) {
        // Create  or retrieve existent GNEConection
        GNEConnection* con = retrieveGNEConnection(nbCon.fromLane, nbCon.toEdge, nbCon.toLane);
        // add it to GNEConnection container
        myGNEConnections.push_back(con);
        // Add reference
        myGNEConnections.back()->incRef("GNEEdge::addConnection");
        // select GNEConnection if needed
        if(selectAfterCreation) {
            gSelected.deselect(con->getGlID());
        }
        // update geometry
        con->updateGeometry();
    }
    myNet->refreshElement(this); // actually we only do this to force a redraw
}


void
GNEEdge::removeConnection(NBEdge::Connection nbCon) {
    // check if is a explicit turnaround
    if (nbCon.toEdge == myNBEdge.getTurnDestination()) {
        myNet->removeExplicitTurnaround(getMicrosimID());
    }
    // remove NBEdge::connection from NBEdge
    myNBEdge.removeFromConnections(nbCon);
    // remove their associated GNEConnection
    GNEConnection* con = retrieveGNEConnection(nbCon.fromLane, nbCon.toEdge, nbCon.toLane, false);
    if(con != NULL) {
        con->decRef("GNEEdge::removeConnection");
        myGNEConnections.erase(std::find(myGNEConnections.begin(), myGNEConnections.end(), con));
        if (con->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting unreferenced " + toString(con->getTag()) + " '" + con->getID() + "' in removeConnection()");
            }
            delete con;
            myNet->refreshElement(this); // actually we only do this to force a redraw
        }
    }
}


GNEConnection*
GNEEdge::retrieveGNEConnection(int fromLane, NBEdge* to, int toLane, bool createIfNoExist) {
    for (ConnectionVector::iterator i = myGNEConnections.begin(); i != myGNEConnections.end(); ++i) {
        if ((*i)->getFromLaneIndex() == fromLane
                && (*i)->getEdgeTo()->getNBEdge() == to
                && (*i)->getToLaneIndex() == toLane) {
            return *i;
        }
    }
    if(createIfNoExist) {
        // create new connection
        GNEConnection *createdConnection = new GNEConnection(myLanes[fromLane], myNet->retrieveEdge(to->getID())->getLanes()[toLane]);
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Created " + toString(createdConnection->getTag()) + " '" + createdConnection->getID() + "' in retrieveGNEConnection()");
        }
        return createdConnection;
    } else {
        return NULL;
    }
}



void
GNEEdge::setMicrosimID(const std::string& newID) {
    GUIGlObject::setMicrosimID(newID);
    for (LaneVector::iterator i = myLanes.begin(); i != myLanes.end(); ++i) {
        (*i)->setMicrosimID(getNBEdge()->getLaneID((*i)->getIndex()));
    }
}


void
GNEEdge::addAdditionalChild(GNEAdditional* additional) {
    // First check that additional wasn't already inserted
    if (std::find(myAdditionals.begin(), myAdditionals.end(), additional) != myAdditionals.end()) {
        throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' was already inserted in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myAdditionals.push_back(additional);
        // update geometry is needed for stacked additionals (routeProbes and Vaporicers)
        updateGeometry();
    }
}


void
GNEEdge::removeAdditionalChild(GNEAdditional* additional) {
    // First check that additional was already inserted
    AdditionalVector::iterator it = std::find(myAdditionals.begin(), myAdditionals.end(), additional);
    if (it == myAdditionals.end()) {
        throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' doesn't exist in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myAdditionals.erase(it);
        // update geometry is needed for stacked additionals (routeProbes and Vaporicers)
        updateGeometry();
    }
}


const std::vector<GNEAdditional*>&
GNEEdge::getAdditionalChilds() const {
    return myAdditionals;
}


void
GNEEdge::addGNERerouter(GNERerouter* rerouter) {
    if (std::find(myReroutes.begin(), myReroutes.end(), rerouter) == myReroutes.end()) {
        myReroutes.push_back(rerouter);
    } else {
        throw ProcessError(toString(rerouter->getTag()) + " '" + rerouter->getID() + "' was previously inserted");
    }
}


void
GNEEdge::removeGNERerouter(GNERerouter* rerouter) {
    std::vector<GNERerouter*>::iterator it = std::find(myReroutes.begin(), myReroutes.end(), rerouter);
    if (it != myReroutes.end()) {
        myReroutes.erase(it);
    } else {
        throw ProcessError(toString(rerouter->getTag()) + " '" + rerouter->getID() + "' wasn't previously inserted");
    }
}


const std::vector<GNERerouter*>&
GNEEdge::getGNERerouters() const {
    return myReroutes;
}


int
GNEEdge::getNumberOfGNERerouters() const {
    return (int)myReroutes.size();
}


bool
GNEEdge::hasRestrictedLane(SUMOVehicleClass vclass) const {
    for (std::vector<GNELane*>::const_iterator i = myLanes.begin(); i != myLanes.end(); i++) {
        if ((*i)->isRestricted(vclass)) {
            return true;
        }
    }
    return false;
}


void 
GNEEdge::removeEdgeFromCrossings(GNEJunction *junction, GNEUndoList* undoList) {
    while(junction->getGNECrossings().size() > 0) {
        myNet->deleteCrossing(junction->getGNECrossings().front(), undoList);
    }
}

/****************************************************************************/
