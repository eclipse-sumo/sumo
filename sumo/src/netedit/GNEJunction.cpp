/****************************************************************************/
/// @file    GNEJunction.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A class for visualizing and editing junctions in netedit (adapted from
// GUIJunctionWrapper)
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

#include <string>
#include <utility>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netbuild/NBAlgorithms.h>
#include "GNENet.h"
#include "GNEEdge.h"
#include "GNECrossing.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEChange_Attribute.h"
#include "GNEChange_Connection.h"
#include "GNEChange_TLS.h"
#include "GNEConnection.h"
#include "GNEJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// method definitions
// ===========================================================================
GNEJunction::GNEJunction(NBNode& nbn, GNENet* net, bool loaded) :
    GNENetElement(net, nbn.getID(), GLO_JUNCTION, SUMO_TAG_JUNCTION, ICON_JUNCTION),
    myNBNode(nbn),
    myOrigPos(nbn.getPosition()),
    myAmCreateEdgeSource(false),
    myLogicStatus(loaded ? LOADED : GUESSED),
    myAmResponsible(false),
    myHasValidLogic(loaded),
    myAmTLSSelected(false) {
    updateGeometry();
}


GNEJunction::~GNEJunction() {
    if (myAmResponsible) {
        delete &myNBNode;
    }
    dropGNECrossings();
}


void
GNEJunction::updateGeometry() {
    const double EXTENT = 2;
    myBoundary = Boundary(
                     myOrigPos.x() - EXTENT, myOrigPos.y() - EXTENT,
                     myOrigPos.x() + EXTENT, myOrigPos.y() + EXTENT);
    if (myNBNode.getShape().size() > 0) {
        myBoundary.add(myNBNode.getShape().getBoxBoundary());
    }
    myMaxSize = MAX2(myBoundary.getWidth(), myBoundary.getHeight());
    rebuildGNECrossings();
}


void
GNEJunction::rebuildGNECrossings() {
    // drop existent GNECrossings
    dropGNECrossings();
    // rebuild GNECrossings only if create crossings and walkingAreas in net is enabled
    if (myNet->getNetBuilder()->haveNetworkCrossings() == true) {
        // build new NBNode::Crossings and walking areas and create GNECrossings
        myNBNode.buildCrossingsAndWalkingAreas();
        const std::vector<NBNode::Crossing>& crossings = myNBNode.getCrossings();
        for (std::vector<NBNode::Crossing>::const_iterator it = crossings.begin(); it != crossings.end(); it++) {
            myGNECrossings.push_back(new GNECrossing(this, (*it).id));
            myGNECrossings.back()->incRef();
        }
    }
}


GUIGLObjectPopupMenu*
GNEJunction::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    //if (parent.getVisualisationSettings()->editMode != GNE_MODE_CONNECT) {
    //    // XXX if joinable
    //    new FXMenuCommand(ret, "Join adjacent edges", 0, &parent, MID_GNE_JOIN_EDGES);
    //}
    FXMenuCommand* mcCustomShape = new FXMenuCommand(ret, "Set custom shape", 0, &parent, MID_GNE_NODE_SHAPE);
    FXMenuCommand* mcReplace = new FXMenuCommand(ret, "Replace by geometry node", 0, &parent, MID_GNE_NODE_REPLACE);
    const int editMode = parent.getVisualisationSettings()->editMode;
    const bool wrongMode = (editMode == GNE_MODE_CONNECT || editMode == GNE_MODE_TLS || editMode == GNE_MODE_CREATE_EDGE);
    if (wrongMode) {
        mcCustomShape->handle(&parent, FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), 0);
    }
    // checkIsRemovable requiers turnarounds to be computed. This is ugly
    if (myNBNode.getIncomingEdges().size() == 2 && myNBNode.getOutgoingEdges().size() == 2) {
        NBTurningDirectionsComputer::computeTurnDirectionsForNode(&myNBNode, false);
    }
    if (wrongMode || !myNBNode.checkIsRemovable()) {
        mcReplace->handle(&parent, FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), 0);
    }
    // let the GNEViewNet store the popup position
    (dynamic_cast<GNEViewNet&>(parent)).markPopupPosition();
    return ret;
}


GUIParameterTableWindow*
GNEJunction::getParameterWindow(GUIMainWindow& /*app*/, GUISUMOAbstractView&) {
    return 0;
}


Boundary
GNEJunction::getCenteringBoundary() const {
    Boundary b = myBoundary;
    b.grow(20);
    return b;
}


void
GNEJunction::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    SUMOReal exaggeration = gSelected.isSelected(getType(), getGlID()) ? s.selectionScale : 1;
    exaggeration *= s.junctionSize.getExaggeration(s);
    if (s.scale * exaggeration * myMaxSize < 1.) {
        // draw something simple so that selection still works
        GLHelper::drawBoxLine(myNBNode.getPosition(), 0, 1, 1);
    } else {
        // node shape has been computed and is valid for drawing
        const bool drawShape = myNBNode.getShape().size() > 0 && s.drawJunctionShape;
        const bool drawBubble = (!drawShape || myNBNode.getShape().area() < 4) && s.drawJunctionShape; // magic threshold

        if (drawShape) {
            setColor(s, false);
            // recognize full transparency and simply don't draw
            GLfloat color[4];
            glGetFloatv(GL_CURRENT_COLOR, color);
            if (color[3] != 0) {
                glPushMatrix();
                glTranslated(0, 0, getType());
                PositionVector shape = myNBNode.getShape();
                shape.closePolygon();
                if (exaggeration > 1) {
                    shape.scaleRelative(exaggeration);
                }
                if (s.scale * exaggeration * myMaxSize < 40.) {
                    GLHelper::drawFilledPoly(shape, true);
                } else {
                    GLHelper::drawFilledPolyTesselated(shape, true);
                }
                glPopMatrix();
            }
            // Check if a  buuble must be drawed over junction
            if (myNet->getViewNet()->showJunctionAsBubbles()) {
                setColor(s, true);
                // recognize full transparency and simply don't draw
                GLfloat color[4];
                glGetFloatv(GL_CURRENT_COLOR, color);
                if (color[3] != 0) {
                    glPushMatrix();
                    Position pos = myNBNode.getPosition();
                    glTranslated(pos.x(), pos.y(), getType() + 0.05);
                    GLHelper::drawFilledCircle(4 * exaggeration, 32);
                    glPopMatrix();
                }
            }
        }
        if (drawBubble) {
            setColor(s, true);
            // recognize full transparency and simply don't draw
            GLfloat color[4];
            glGetFloatv(GL_CURRENT_COLOR, color);
            if (color[3] != 0) {
                glPushMatrix();
                Position pos = myNBNode.getPosition();
                glTranslated(pos.x(), pos.y(), getType() - 0.05);
                GLHelper::drawFilledCircle(4 * exaggeration, 32);
                glPopMatrix();
            }
        }

        if (s.editMode == GNE_MODE_TLS && myNBNode.isTLControlled() && !myAmTLSSelected) {
            glPushMatrix();
            Position pos = myNBNode.getPosition();
            glTranslated(pos.x(), pos.y(), getType() + 0.1);
            glColor3d(1, 1, 1);
            const SUMOReal halfWidth = 32 / s.scale;
            const SUMOReal halfHeight = 64 / s.scale;
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_TLS), -halfWidth, -halfHeight, halfWidth, halfHeight);
            glPopMatrix();
        }
        // draw crossings
        if (s.editMode != GNE_MODE_TLS) {
            for (std::vector<GNECrossing*>::const_iterator it = myGNECrossings.begin(); it != myGNECrossings.end(); it++) {
                (*it)->drawGL(s);
            }
        }
        // (optional) draw name @todo expose this setting
        drawName(myNBNode.getPosition(), s.scale, s.junctionName);
    }
    glPopName();
}

Boundary
GNEJunction::getBoundary() const {
    return myBoundary;
}


NBNode*
GNEJunction::getNBNode() const {
    return &myNBNode;
}



void
GNEJunction::addIncomingGNEEdge(GNEEdge* edge) {
    // Check if incoming edge was already inserted
    std::vector<GNEEdge*>::iterator i = std::find(myGNEIncomingEdges.begin(), myGNEIncomingEdges.end(), edge);
    if (i != myGNEIncomingEdges.end()) {
        throw InvalidArgument("Incoming " + toString(SUMO_TAG_EDGE) + " with ID '" + edge->getID() + "' was already inserted into " + toString(getTag()) + " with ID " + getID() + "'");
    } else {
        // Add edge into containers
        myGNEIncomingEdges.push_back(edge);
        myGNEEdges.push_back(edge);
    }
}


void
GNEJunction::addOutgoingGNEEdge(GNEEdge* edge) {
    // Check if outgoing edge was already inserted
    std::vector<GNEEdge*>::iterator i = std::find(myGNEOutgoingEdges.begin(), myGNEOutgoingEdges.end(), edge);
    if (i != myGNEOutgoingEdges.end()) {
        throw InvalidArgument("Outgoing " + toString(SUMO_TAG_EDGE) + " with ID '" + edge->getID() + "' was already inserted into " + toString(getTag()) + " with ID " + getID() + "'");
    } else {
        // Add edge into containers
        myGNEOutgoingEdges.push_back(edge);
        myGNEEdges.push_back(edge);
    }
}


void
GNEJunction::removeIncomingGNEEdge(GNEEdge* edge) {
    // Check if incoming edge was already inserted
    std::vector<GNEEdge*>::iterator i = std::find(myGNEIncomingEdges.begin(), myGNEIncomingEdges.end(), edge);
    if (i == myGNEIncomingEdges.end()) {
        throw InvalidArgument("Incoming " + toString(SUMO_TAG_EDGE) + " with ID '" + edge->getID() + "' doesn't found into " + toString(getTag()) + " with ID " + getID() + "'");
    } else {
        // remove edge from containers
        myGNEIncomingEdges.erase(i);
        myGNEEdges.erase(std::find(myGNEEdges.begin(), myGNEEdges.end(), edge));
    }
}


void
GNEJunction::removeOutgoingGNEEdge(GNEEdge* edge) {
    // Check if outgoing edge was already inserted
    std::vector<GNEEdge*>::iterator i = std::find(myGNEOutgoingEdges.begin(), myGNEOutgoingEdges.end(), edge);
    if (i == myGNEOutgoingEdges.end()) {
        throw InvalidArgument("Outgoing " + toString(SUMO_TAG_EDGE) + " with ID '" + edge->getID() + "' doesn't found into " + toString(getTag()) + " with ID " + getID() + "'");
    } else {
        // remove edge from containers
        myGNEOutgoingEdges.erase(i);
        myGNEEdges.erase(std::find(myGNEEdges.begin(), myGNEEdges.end(), edge));
    }
}


const std::vector<GNEEdge*>&
GNEJunction::getGNEEdges() const {
    return myGNEEdges;
}


const std::vector<GNEEdge*>&
GNEJunction::getGNEIncomingEdges() const {
    return myGNEIncomingEdges;
}


const std::vector<GNEEdge*>&
GNEJunction::getGNEOutgoingEdges() const {
    return myGNEOutgoingEdges;
}

const std::vector<GNECrossing*>&
GNEJunction::getGNECrossings() const {
    return myGNECrossings;
}

void
GNEJunction::markAsCreateEdgeSource() {
    myAmCreateEdgeSource = true;
}


void
GNEJunction::unMarkAsCreateEdgeSource() {
    myAmCreateEdgeSource = false;
}


void
GNEJunction::selectTLS(bool selected) {
    myAmTLSSelected = selected;
}


void
GNEJunction::move(Position pos) {
    const Position orig = myNBNode.getPosition();
    setPosition(pos);
    const EdgeVector& incident = getNBNode()->getEdges();
    for (EdgeVector::const_iterator it = incident.begin(); it != incident.end(); it++) {
        GNEEdge* edge = myNet->retrieveEdge((*it)->getID());
        edge->updateJunctionPosition(this, orig);
    }
    // Update shapes without include connections, because the aren't showed in Move mode
    updateShapesAndGeometries();
}


void
GNEJunction::registerMove(GNEUndoList* undoList) {
    Position newPos = myNBNode.getPosition();
    std::string newPosValue = getAttribute(SUMO_ATTR_POSITION);
    // actually the geometry is already up to date
    // set the restore point to the end of the last change-set
    setPosition(myOrigPos);
    // do not execute the command to avoid changing the edge geometry twice
    undoList->add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, newPosValue), false);
    setPosition(newPos);
    // Refresh element to avoid grabbing problems
    myNet->refreshElement(this);
}


void
GNEJunction::updateShapesAndGeometries() {
    // First declare three sets with all affected GNEJunctions, GNEEdges and GNEConnections
    std::set<GNEJunction*> affectedJunctions;
    std::set<GNEEdge*> affectedEdges;
    // Iterate over GNEEdges
    for (std::vector<GNEEdge*>::const_iterator i = myGNEEdges.begin(); i != myGNEEdges.end(); i++) {
        // Add source and destiny junctions
        affectedJunctions.insert((*i)->getGNEJunctionSource());
        affectedJunctions.insert((*i)->getGNEJunctionDestiny());
        // Obtain neighbors of Junction source
        for (std::vector<GNEEdge*>::const_iterator j = (*i)->getGNEJunctionSource()->getGNEEdges().begin(); j != (*i)->getGNEJunctionSource()->getGNEEdges().end(); j++) {
            affectedEdges.insert(*j);
        }
        // Obtain neighbors of Junction destiny
        for (std::vector<GNEEdge*>::const_iterator j = (*i)->getGNEJunctionDestiny()->getGNEEdges().begin(); j != (*i)->getGNEJunctionDestiny()->getGNEEdges().end(); j++) {
            affectedEdges.insert(*j);
        }
    }
    // Iterate over affected Junctions
    for (std::set<GNEJunction*>::iterator i = affectedJunctions.begin(); i != affectedJunctions.end(); i++) {
        // Check that Node doesn't have a custom shape
        if ((*i)->getNBNode()->hasCustomShape() == false) {
            // Compute polygon
            (*i)->getNBNode()->sortEdges(false);
            (*i)->getNBNode()->computeNodeShape(-1);
        }
        // Update geometry of Junction
        (*i)->updateGeometry();
    }
    // Iterate over affected Edges
    for (std::set<GNEEdge*>::iterator i = affectedEdges.begin(); i != affectedEdges.end(); i++) {
        // Compute full edge shape because this funcion is related to computeNodeShape
        (*i)->getNBEdge()->computeEdgeShape();
        // Update edge geometry
        (*i)->updateGeometry();
    }
    // Finally update geometry of this edge
    updateGeometry();
    // Update view to show the new shapes
    if (myNet->getViewNet()) {
        myNet->getViewNet()->update();
    }
}


void
GNEJunction::invalidateShape() {
    if (!myNBNode.hasCustomShape()) {
        myNBNode.myPoly.clear();
        myNet->requireRecompute();
    }
}


void
GNEJunction::setLogicValid(bool valid, GNEUndoList* undoList, const std::string& status) {
    myHasValidLogic = valid;
    if (!valid) {
        assert(undoList != 0);
        assert(undoList->hasCommandGroup());
        NBTurningDirectionsComputer::computeTurnDirectionsForNode(&myNBNode, false);
        EdgeVector incoming = myNBNode.getIncomingEdges();
        for (EdgeVector::iterator it = incoming.begin(); it != incoming.end(); it++) {
            NBEdge* srcNBE = *it;
            NBEdge* turnEdge = srcNBE->getTurnDestination();
            GNEEdge* srcEdge = myNet->retrieveEdge(srcNBE->getID());
            // Make a copy of connections
            std::vector<NBEdge::Connection> connections = srcNBE->getConnections();
            // delete in reverse so that undoing will add connections in the original order
            for (std::vector<NBEdge::Connection>::reverse_iterator con_it = connections.rbegin(); con_it != connections.rend(); con_it++) {
                bool hasTurn = con_it->toEdge == turnEdge;
                undoList->add(new GNEChange_Connection(srcEdge, *con_it, false), true);
                // needs to come after GNEChange_Connection
                // XXX bug: this code path will not be used on a redo!
                if (hasTurn) {
                    myNet->addExplicitTurnaround(srcNBE->getID());
                }
            }
            undoList->add(new GNEChange_Attribute(srcEdge, GNE_ATTR_MODIFICATION_STATUS, status), true);
        }
        undoList->add(new GNEChange_Attribute(this, GNE_ATTR_MODIFICATION_STATUS, status), true);
        invalidateTLS(undoList);
    } else {
        rebuildGNECrossings();
    }
}


void
GNEJunction::markAsModified(GNEUndoList* undoList) {
    EdgeVector incoming = myNBNode.getIncomingEdges();
    for (EdgeVector::iterator it = incoming.begin(); it != incoming.end(); it++) {
        NBEdge* srcNBE = *it;
        GNEEdge* srcEdge = myNet->retrieveEdge(srcNBE->getID());
        undoList->add(new GNEChange_Attribute(srcEdge, GNE_ATTR_MODIFICATION_STATUS, MODIFIED), true);
    }
}


void
GNEJunction::invalidateTLS(GNEUndoList* undoList, const NBConnection& deletedConnection) {
    assert(undoList->hasCommandGroup());
    // NBLoadedSUMOTLDef becomes invalid, replace with NBOwnTLDef which will be dynamically recomputed
    const std::set<NBTrafficLightDefinition*> tls = myNBNode.getControllingTLS(); // make a copy!
    for (std::set<NBTrafficLightDefinition*>::iterator it = tls.begin(); it != tls.end(); it++) {
        NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(*it);
        if (tlDef != 0) {
            NBTrafficLightDefinition* replacementDef = 0;
            std::string newID = tlDef->getID(); // + "_reguessed"; // changes due to reguessing will be visible in diff
            if (deletedConnection != NBConnection::InvalidConnection) {
                // create replacement before deleting the original because deletion will mess up saving original nodes
                NBLoadedSUMOTLDef* repl = new NBLoadedSUMOTLDef(tlDef, tlDef->getLogic());
                repl->removeConnection(deletedConnection);
                replacementDef = repl;
            } else {
                replacementDef = new NBOwnTLDef(newID, tlDef->getOffset(), tlDef->getType());
                replacementDef->setProgramID(tlDef->getProgramID());
            }
            undoList->add(new GNEChange_TLS(this, tlDef, false), true);
            undoList->add(new GNEChange_TLS(this, replacementDef, true, false, newID), true);
            // the removed traffic light may have controlled more than one junction. These too have become invalid now
            const std::vector<NBNode*> coNodes = tlDef->getNodes(); // make a copy!
            for (std::vector<NBNode*>::const_iterator it_node = coNodes.begin(); it_node != coNodes.end(); it_node++) {
                GNEJunction* sharing = myNet->retrieveJunction((*it_node)->getID());
                undoList->add(new GNEChange_TLS(sharing, tlDef, false), true);
                undoList->add(new GNEChange_TLS(sharing, replacementDef, true, false, newID), true);
            }
        }
    }
}

void
GNEJunction::removeFromCrossings(GNEEdge* edge, GNEUndoList* undoList) {
    // @todo implement GNEChange_Crossing
    UNUSED_PARAMETER(undoList);
    // make a copy because the original will be modified
    const std::vector<NBNode::Crossing> crossings = myNBNode.getCrossings();
    for (std::vector<NBNode::Crossing>::const_iterator it = crossings.begin(); it != crossings.end(); it++) {
        EdgeSet edgeSet((*it).edges.begin(), (*it).edges.end());
        if (edgeSet.count(edge->getNBEdge()) == 1) {
            myNBNode.removeCrossing((*it).edges);
        }
    }
}


bool
GNEJunction::isLogicValid() {
    return myHasValidLogic;
}


void
GNEJunction::dropGNECrossings() {
    // delete all GNECrossing
    for (std::vector<GNECrossing*>::const_iterator it = myGNECrossings.begin(); it != myGNECrossings.end(); it++) {
        (*it)->decRef();
        if ((*it)->unreferenced()) {
            delete *it;
        }
    }
    myGNECrossings.clear();
}


std::string
GNEJunction::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
            break;
        case SUMO_ATTR_POSITION:
            return toString(myNBNode.getPosition());
            break;
        case SUMO_ATTR_TYPE:
            return toString(myNBNode.getType());
            break;
        case GNE_ATTR_MODIFICATION_STATUS:
            return myLogicStatus;
            break;
        case SUMO_ATTR_SHAPE:
            return toString(myNBNode.getShape());
        case SUMO_ATTR_RADIUS:
            return toString(myNBNode.getRadius());
        case SUMO_ATTR_TLTYPE:
            // @todo this causes problems if the node were to have multiple programs of different type (plausible)
            return myNBNode.isTLControlled() ? toString((*myNBNode.getControllingTLS().begin())->getType()) : "";
        case SUMO_ATTR_TLID:
            return myNBNode.isTLControlled() ? toString((*myNBNode.getControllingTLS().begin())->getID()) : "";
        case SUMO_ATTR_KEEP_CLEAR:
            return myNBNode.getKeepClear() ? "true" : "false";
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEJunction::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_POSITION:
        case GNE_ATTR_MODIFICATION_STATUS:
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_RADIUS:
        case SUMO_ATTR_TLTYPE:
        case SUMO_ATTR_KEEP_CLEAR:
            undoList->add(new GNEChange_Attribute(this, key, value), true);
            break;
        case SUMO_ATTR_TYPE: {
            undoList->p_begin("change " + toString(getTag()) + " type");
            if (NBNode::isTrafficLight(SUMOXMLDefinitions::NodeTypes.get(value))) {
                if (!getNBNode()->isTLControlled()) {
                    // create new traffic light
                    undoList->add(new GNEChange_TLS(this, 0, true), true);
                }
            } else if (getNBNode()->isTLControlled()) {
                // delete old traffic light
                // make a copy because we will modify the original
                const std::set<NBTrafficLightDefinition*> tls = myNBNode.getControllingTLS();
                for (std::set<NBTrafficLightDefinition*>::iterator it = tls.begin(); it != tls.end(); it++) {
                    undoList->add(new GNEChange_TLS(this, *it, false), true);
                }
            }
            // must be the final step, otherwise we do not know which traffic lights to remove via GNEChange_TLS
            undoList->add(new GNEChange_Attribute(this, key, value), true);
            undoList->p_end();
            break;
        }
        case SUMO_ATTR_TLID: {
            undoList->p_begin("change " + toString(SUMO_TAG_TRAFFIC_LIGHT) + " id");
            // junction is already controlled, remove from previous tls
            const std::set<NBTrafficLightDefinition*> tls = myNBNode.getControllingTLS();
            for (std::set<NBTrafficLightDefinition*>::iterator it = tls.begin(); it != tls.end(); it++) {
                undoList->add(new GNEChange_TLS(this, *it, false), true);
            }
            NBTrafficLightLogicCont& tlCont = myNet->getTLLogicCont();
            const std::map<std::string, NBTrafficLightDefinition*>& programs = tlCont.getPrograms(value);
            if (programs.size() > 0) {
                // add to existing tls definitions
                for (std::map<std::string, NBTrafficLightDefinition*>::const_iterator it = programs.begin(); it != programs.end(); it++) {
                    NBTrafficLightDefinition* oldTLS = it->second;
                    if (dynamic_cast<NBOwnTLDef*>(oldTLS) != 0) {
                        undoList->add(new GNEChange_TLS(this, oldTLS, true), true);
                    } else {
                        // delete and re-create the definition because the loaded phases are now invalid
                        const std::vector<NBNode*> nodes = oldTLS->getNodes();
                        for (std::vector<NBNode*>::const_iterator it_node = nodes.begin(); it_node != nodes.end(); ++it_node) {
                            GNEJunction* oldJunction = myNet->retrieveJunction((*it_node)->getID());
                            undoList->add(new GNEChange_TLS(oldJunction, oldTLS, false), true);
                        }
                        undoList->add(new GNEChange_TLS(this, 0, true, false, value), true);
                        NBTrafficLightDefinition* newTLS = *myNBNode.getControllingTLS().begin();
                        // re-add existing nodes
                        for (std::vector<NBNode*>::const_iterator it_node = nodes.begin(); it_node != nodes.end(); ++it_node) {
                            GNEJunction* oldJunction = myNet->retrieveJunction((*it_node)->getID());
                            undoList->add(new GNEChange_TLS(oldJunction, newTLS, true), true);
                        }
                    }
                }
            } else {
                // create new traffic light
                undoList->add(new GNEChange_TLS(this, 0, true, false, value), true);
            }
            undoList->p_end();
            break;
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEJunction::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidID(value) && myNet->retrieveJunction(value, false) == 0;
            break;
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::NodeTypes.hasString(value);
            break;
        case SUMO_ATTR_POSITION:
            bool ok;
            return GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false).size() == 1;
            break;
        case SUMO_ATTR_SHAPE: {
            bool ok = true;
            PositionVector shape = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, true);
            return ok;
            break;
        }
        case SUMO_ATTR_RADIUS:
            return canParse<SUMOReal>(value);
            break;
        case SUMO_ATTR_TLTYPE:
            return myNBNode.isTLControlled() && SUMOXMLDefinitions::TrafficLightTypes.hasString(value);
        case SUMO_ATTR_TLID:
            return myNBNode.isTLControlled() && value != "";
        case SUMO_ATTR_KEEP_CLEAR:
            return value == "true" || value == "false";
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEJunction::setResponsible(bool newVal) {
    myAmResponsible = newVal;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEJunction::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myNet->renameJunction(this, value);
            break;
        case SUMO_ATTR_TYPE: {
            myNBNode.reinit(myNBNode.getPosition(), SUMOXMLDefinitions::NodeTypes.get(value));
            break;
        }
        case SUMO_ATTR_POSITION:
            bool ok;
            myOrigPos = GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, false)[0];
            move(myOrigPos);
            // Refresh element to avoid grabbing problems
            myNet->refreshElement(this);
            break;
        case GNE_ATTR_MODIFICATION_STATUS:
            if (myLogicStatus == GUESSED && value != GUESSED) {
                // clear guessed connections. previous connections will be restored
                myNBNode.invalidateIncomingConnections();
                // Clear GNEConnections of incoming edges
                for (std::vector<GNEEdge*>::iterator i = myGNEIncomingEdges.begin(); i != myGNEIncomingEdges.end(); i++) {
                    (*i)->clearGNEConnections();
                }
            }
            myLogicStatus = value;
            break;
        case SUMO_ATTR_SHAPE: {
            bool ok;
            const PositionVector shape = GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, true);
            myNBNode.setCustomShape(shape);
            break;
        }
        case SUMO_ATTR_RADIUS:
            myNBNode.setRadius(parse<SUMOReal>(value));
            break;
        case SUMO_ATTR_TLTYPE: {
            const std::set<NBTrafficLightDefinition*> tls = myNBNode.getControllingTLS();
            for (std::set<NBTrafficLightDefinition*>::iterator it = tls.begin(); it != tls.end(); it++) {
                (*it)->setType(SUMOXMLDefinitions::TrafficLightTypes.get(value));
            }
            break;
        }
        case SUMO_ATTR_KEEP_CLEAR:
            myNBNode.setKeepClear(value == "true");
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEJunction::setPosition(Position pos) {
    const Position& orig = myNBNode.getPosition();
    myBoundary.moveby(pos.x() - orig.x(), pos.y() - orig.y());
    myNBNode.reinit(pos, myNBNode.getType());
    /* //reshift also shifts the junction shape. this is not needed because shape is not yet computed
     * const Position& orig = myNBNode.getPosition();
     * myNBNode.reshiftPosition(pos.x() - orig.x(), pos.y() - orig.y());
     */
}


SUMOReal
GNEJunction::getColorValue(const GUIVisualizationSettings& s, bool bubble) const {
    switch (s.junctionColorer.getActive()) {
        case 0:
            if (bubble) {
                return 1;
            } else {
                return 0;
            }
        case 1:
            return gSelected.isSelected(getType(), getGlID()) ? 1 : 0;
        case 2:
            switch (myNBNode.getType()) {
                case NODETYPE_TRAFFIC_LIGHT:
                    return 0;
                case NODETYPE_TRAFFIC_LIGHT_NOJUNCTION:
                    return 1;
                case NODETYPE_PRIORITY:
                    return 2;
                case NODETYPE_PRIORITY_STOP:
                    return 3;
                case NODETYPE_RIGHT_BEFORE_LEFT:
                    return 4;
                case NODETYPE_ALLWAY_STOP:
                    return 5;
                case NODETYPE_DISTRICT:
                    return 6;
                case NODETYPE_NOJUNCTION:
                    return 7;
                case NODETYPE_DEAD_END:
                case NODETYPE_DEAD_END_DEPRECATED:
                    return 8;
                case NODETYPE_UNKNOWN:
                case NODETYPE_INTERNAL:
                    assert(false);
                    return 8;
                case NODETYPE_RAIL_SIGNAL:
                    return 9;
                case NODETYPE_ZIPPER:
                    return 10;
                case NODETYPE_TRAFFIC_LIGHT_RIGHT_ON_RED:
                    return 11;
                case NODETYPE_RAIL_CROSSING:
                    return 12;
            }
        default:
            assert(false);
            return 0;
    }
}


void
GNEJunction::setColor(const GUIVisualizationSettings& s, bool bubble) const {
    GLHelper::setColor(s.junctionColorer.getScheme().getColor(getColorValue(s, bubble)));
    // override with special colors (unless the color scheme is based on selection)
    if (gSelected.isSelected(getType(), getGlID()) && s.junctionColorer.getActive() != 1) {
        GLHelper::setColor(GNENet::selectionColor);
    }
    if (myAmCreateEdgeSource) {
        glColor3d(0, 1, 0);
    }
}

void
GNEJunction::addTrafficLight(NBTrafficLightDefinition* tlDef, bool forceInsert) {
    NBTrafficLightLogicCont& tlCont = myNet->getTLLogicCont();
    tlCont.insert(tlDef, forceInsert); // may return false for tlDef which controls multiple junctions
    tlDef->addNode(&myNBNode);
}


void
GNEJunction::removeTrafficLight(NBTrafficLightDefinition* tlDef) {
    NBTrafficLightLogicCont& tlCont = myNet->getTLLogicCont();
    if (tlDef->getNodes().size() == 1) {
        tlCont.extract(tlDef);
    }
    myNBNode.removeTrafficLight(tlDef);
}

/****************************************************************************/
