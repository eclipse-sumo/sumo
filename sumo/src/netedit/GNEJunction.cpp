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
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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
#include <utils/gui/images/GUIGifSubSys.h>
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
#include "GNEJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// method definitions
// ===========================================================================
GNEJunction::GNEJunction(NBNode& nbn, GNENet* net, bool loaded) :
    GNENetElement(net, nbn.getID(), GLO_JUNCTION, SUMO_TAG_JUNCTION),
    myNBNode(nbn),
    myOrigPos(nbn.getPosition()),
    myAmCreateEdgeSource(false),
    myLogicStatus(loaded ? LOADED : GUESSED),
    myAmResponsible(false),
    myHasValidLogic(loaded),
    myAmTLSSelected(false) {
    updateGeometry();
    rebuildCrossings(false);
}


GNEJunction::~GNEJunction() {
    if (myAmResponsible) {
        delete &myNBNode;
    }
    rebuildCrossings(true);
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
}


void
GNEJunction::rebuildCrossings(bool deleteOnly) {
    for (std::vector<GNECrossing*>::const_iterator it = myCrossings.begin(); it != myCrossings.end(); it++) {
        (*it)->decRef();
        if ((*it)->unreferenced()) {
            delete *it;
        }
    }
    myCrossings.clear();
    if (!deleteOnly) {
        const std::vector<NBNode::Crossing>& crossings = myNBNode.getCrossings();
        for (std::vector<NBNode::Crossing>::const_iterator it = crossings.begin(); it != crossings.end(); it++) {
            myCrossings.push_back(new GNECrossing(*this, (*it).id));
            myCrossings.back()->incRef();
        }
    }
}


GUIGLObjectPopupMenu*
GNEJunction::getPopUpMenu(GUIMainWindow& app,
                          GUISUMOAbstractView& parent) {
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
GNEJunction::getParameterWindow(GUIMainWindow& /*app*/,
                                GUISUMOAbstractView&) {
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
    SUMOReal selectionScale = gSelected.isSelected(getType(), getGlID()) ? s.selectionScale : 1;
    if (s.scale * selectionScale * myMaxSize < 1.) {
        // draw something simple so that selection still works
        GLHelper::drawBoxLine(myNBNode.getPosition(), 0, 1, 1);
    } else {
        // node shape has been computed and is valid for drawing
        const bool drawShape = myNBNode.getShape().size() > 0 && s.drawJunctionShape;
        const bool drawBubble = (!drawShape || myNBNode.getShape().area() < 4) && s.drawJunctionShape; // magic threshold

        if (drawShape) {
            glPushMatrix();
            setColor(s, false);
            glTranslated(0, 0, getType());
            PositionVector shape = myNBNode.getShape();
            shape.closePolygon();
            if (selectionScale > 1) {
                shape.scaleRelative(selectionScale);
            }
            if (s.scale * selectionScale * myMaxSize < 40.) {
                GLHelper::drawFilledPoly(shape, true);
            } else {
                GLHelper::drawFilledPolyTesselated(shape, true);
            }
            glPopMatrix();
        }
        if (drawBubble) {
            glPushMatrix();
            setColor(s, true);
            Position pos = myNBNode.getPosition();
            glTranslated(pos.x(), pos.y(), getType() - 0.05);
            GLHelper::drawFilledCircle(4 * selectionScale, 32);
            glPopMatrix();
        }

        if (s.editMode == GNE_MODE_TLS && myNBNode.isTLControlled() && !myAmTLSSelected) {
            glPushMatrix();
            Position pos = myNBNode.getPosition();
            glTranslated(pos.x(), pos.y(), getType() + 0.1);
            glColor3d(1, 1, 1);
            const SUMOReal halfWidth = 32 / s.scale;
            const SUMOReal halfHeight = 64 / s.scale;
            GUITexturesHelper::drawTexturedBox(GUIGifSubSys::getGif(GNELOGO_TLS), -halfWidth, -halfHeight, halfWidth, halfHeight);
            glPopMatrix();
        }
        // draw crossings
        if (s.editMode != GNE_MODE_TLS) {
            for (std::vector<GNECrossing*>::const_iterator it = myCrossings.begin(); it != myCrossings.end(); it++) {
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
    myNet->refreshElement(this);
    const EdgeVector& incident = getNBNode()->getEdges();
    for (EdgeVector::const_iterator it = incident.begin(); it != incident.end(); it++) {
        GNEEdge* edge = myNet->retrieveEdge((*it)->getID());
        edge->updateJunctionPosition(this, orig);
    }
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
    // If new logic isn't valid
    if (!valid) {
        // Check preconditions
        assert(undoList != 0);
        assert(undoList->hasCommandGroup());
        // Registre a modification of status
        undoList->add(new GNEChange_Attribute(this, GNE_ATTR_MODIFICATION_STATUS, status));
        // allow edges to recompute their connections
        NBTurningDirectionsComputer::computeTurnDirectionsForNode(&myNBNode, false);
        // Obtain a copy of incoming edges
        EdgeVector incoming = myNBNode.getIncomingEdges();
        // Iterate over incoming edges
        for (EdgeVector::iterator it = incoming.begin(); it != incoming.end(); it++) {
            NBEdge* srcNBE = *it;
            NBEdge* turnEdge = srcNBE->getTurnDestination();
            GNEEdge* srcEdge = myNet->retrieveEdge(srcNBE->getID());
            // Make a copy of connections
            std::vector<NBEdge::Connection> connections = srcNBE->getConnections();
            // delete in reverse so that undoing will add connections in the original order
            for (std::vector<NBEdge::Connection>::reverse_iterator con_it = connections.rbegin(); con_it != connections.rend(); con_it++) {
                bool hasTurn = con_it->toEdge == turnEdge;
                undoList->add(new GNEChange_Connection(
                                  srcEdge, con_it->fromLane, con_it->toEdge->getID(),
                                  con_it->toLane, con_it->mayDefinitelyPass, false), true);
                // needs to come after GNEChange_Connection
                // XXX bug: this code path will not be used on a redo!
                if (hasTurn) {
                    myNet->addExplicitTurnaround(srcNBE->getID());
                }
            }
            undoList->add(new GNEChange_Attribute(srcEdge, GNE_ATTR_MODIFICATION_STATUS, status), true);
        }
        // Invalidate traffic light
        invalidateTLS(undoList);
    } else
        rebuildCrossings(false);
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
        case SUMO_ATTR_KEEP_CLEAR:
            return myNBNode.getKeepClear() ? "true" : "false";
        default:
            throw InvalidArgument("junction attribute '" + toString(key) + "' not allowed");
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
        case SUMO_ATTR_KEEP_CLEAR:
            undoList->add(new GNEChange_Attribute(this, key, value), true);
            break;
        case SUMO_ATTR_TYPE: {
            undoList->p_begin("change junction type");
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
        default:
            throw InvalidArgument("junction attribute '" + toString(key) + "' not allowed");
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
            PositionVector shape = GeomConvHelper::parseShapeReporting(
                                       value, "user-supplied position", 0, ok, true);
            return ok;
            break;
        }
        case SUMO_ATTR_RADIUS:
            return canParse<SUMOReal>(value);
            break;
        case SUMO_ATTR_KEEP_CLEAR:
            return value == "true" || value == "false";
            break;
        default:
            throw InvalidArgument("junction attribute '" + toString(key) + "' not allowed");
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
            break;
        case GNE_ATTR_MODIFICATION_STATUS:
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
        case SUMO_ATTR_KEEP_CLEAR:
            myNBNode.setKeepClear(value == "true");
            break;
        default:
            throw InvalidArgument("junction attribute '" + toString(key) + "' not allowed");
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
    // override with special colors
    if (gSelected.isSelected(getType(), getGlID())) {
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
    tlCont.extract(tlDef);
    myNBNode.removeTrafficLight(tlDef);
}

void
GNEJunction::updateCrossingAttributes(NBNode::Crossing crossing) {
    EdgeSet edgeSet(crossing.edges.begin(), crossing.edges.end());
    for (std::vector<NBNode::Crossing>::iterator it = myNBNode.myCrossings.begin(); it != myNBNode.myCrossings.end(); ++it) {
        EdgeSet edgeSet2((*it).edges.begin(), (*it).edges.end());
        if (edgeSet == edgeSet2) {
            (*it).width = crossing.width;
            (*it).priority = crossing.priority;
            myNet->refreshElement(this);
            break;
        }
    }
}

/****************************************************************************/
