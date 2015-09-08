/****************************************************************************/
/// @file    GNEJunction.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A class for visualizing and editing junctions in netedit (adapted from
// GUIJunctionWrapper)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netbuild/NBAlgorithms.h>
#include "tlslogo.cpp"
#include "GNENet.h"
#include "GNEEdge.h"
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
// static members
// ===========================================================================
int GNEJunction::TLSDecalGlID = 0;
bool GNEJunction::TLSDecalInitialized = false;

// ===========================================================================
// method definitions
// ===========================================================================
GNEJunction::GNEJunction(NBNode& nbn, GNENet* net, bool loaded) :
    GUIGlObject(GLO_JUNCTION, nbn.getID()),
    GNEAttributeCarrier(SUMO_TAG_JUNCTION),
    myNBNode(nbn),
    myOrigPos(nbn.getPosition()),
    myAmCreateEdgeSource(false),
    myNet(net),
    myLogicStatus(loaded ? LOADED : GUESSED),
    myAmResponsible(false),
    myHasValidLogic(loaded),
    myAmTLSSelected(false) {
    const double EXTENT = 2;
    myBoundary = Boundary(
                     myOrigPos.x() - EXTENT, myOrigPos.y() - EXTENT,
                     myOrigPos.x() + EXTENT, myOrigPos.y() + EXTENT);
    myMaxSize = 2 * EXTENT;
}


GNEJunction::~GNEJunction() {
    if (myAmResponsible) {
        delete &myNBNode;
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
            setColor(false);
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
            setColor(true);
            Position pos = myNBNode.getPosition();
            glTranslated(pos.x(), pos.y(), getType() - 0.05);
            GLHelper::drawFilledCircle(myMaxSize * selectionScale, 32);
            glPopMatrix();
        }

        if (s.editMode == GNE_MODE_TLS && myNBNode.isTLControlled() && !myAmTLSSelected) {
            // decorate in tls mode
            if (!TLSDecalInitialized) {
                FXImage* i = new FXGIFImage(myNet->getApp(), tlslogo, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
                TLSDecalGlID = GUITexturesHelper::add(i);
                TLSDecalInitialized = true;
                delete i;
            }
            glPushMatrix();
            Position pos = myNBNode.getPosition();
            glTranslated(pos.x(), pos.y(), getType() + 0.1);
            glColor3d(1, 1, 1);
            const SUMOReal halfWidth = 32 / s.scale;
            const SUMOReal halfHeight = 64 / s.scale;
            GUITexturesHelper::drawTexturedBox(TLSDecalGlID, -halfWidth, -halfHeight, halfWidth, halfHeight);
            glPopMatrix();
        }
        // (optional) draw name @todo expose this setting
        drawName(myNBNode.getPosition(), s.scale, s.junctionName);
    }
    glPopName();
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
    if (!valid) {
        assert(undoList != 0);
        assert(undoList->hasCommandGroup());
        undoList->add(new GNEChange_Attribute(this, GNE_ATTR_MODIFICATION_STATUS, status));
        // allow edges to recompute their connections
        NBTurningDirectionsComputer::computeTurnDirectionsForNode(&myNBNode, false);
        EdgeVector incoming = EdgeVector(myNBNode.getIncomingEdges());
        for (EdgeVector::iterator it = incoming.begin(); it != incoming.end(); it++) {
            NBEdge* srcNBE = *it;
            NBEdge* turnEdge = srcNBE->getTurnDestination();
            GNEEdge* srcEdge = myNet->retrieveEdge(srcNBE->getID());
            std::vector<NBEdge::Connection> connections = srcNBE->getConnections(); // make a copy!
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
        invalidateTLS(undoList);
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
            NBLoadedSUMOTLDef* replacementDef = 0;
            std::string newID = tlDef->getID(); // + "_reguessed"; // changes due to reguessing will be visible in diff
            if (deletedConnection != NBConnection::InvalidConnection) {
                // create replacement before deleting the original because deletion will mess up saving original nodes
                replacementDef = new NBLoadedSUMOTLDef(tlDef, tlDef->getLogic());
                replacementDef->removeConnection(deletedConnection);
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
            undoList->add(new GNEChange_Attribute(this, key, value), true);
            break;
        case SUMO_ATTR_TYPE: {
            undoList->p_begin("change junction type");
            if (SUMOXMLDefinitions::NodeTypes.get(value) == NODETYPE_TRAFFIC_LIGHT ||
                    SUMOXMLDefinitions::NodeTypes.get(value) == NODETYPE_TRAFFIC_LIGHT_NOJUNCTION) {
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
        default:
            throw InvalidArgument("junction attribute '" + toString(key) + "' not allowed");
    }
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


void
GNEJunction::setColor(bool bubble) const {
    if (bubble) {
        glColor3d(0.8, 0, 0);
    } else {
        glColor3d(0.4, 0, 0);
    }
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

/****************************************************************************/
