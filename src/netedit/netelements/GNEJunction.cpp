/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEJunction.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A class for visualizing and editing junctions in netedit (adapted from
// GUIJunctionWrapper)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netbuild/NBAlgorithms.h>
#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBOwnTLDef.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/changes/GNEChange_TLS.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>

#include "GNEEdge.h"
#include "GNEConnection.h"
#include "GNEJunction.h"
#include "GNECrossing.h"


// ===========================================================================
// static members
// ===========================================================================

const double GNEJunction::BUBBLE_RADIUS(4);

// ===========================================================================
// method definitions
// ===========================================================================

GNEJunction::GNEJunction(NBNode& nbn, GNENet* net, bool loaded) :
    GNENetElement(net, nbn.getID(), GLO_JUNCTION, SUMO_TAG_JUNCTION),
    myNBNode(nbn),
    myAmCreateEdgeSource(false),
    myLogicStatus(loaded ? FEATURE_LOADED : FEATURE_GUESSED),
    myAmResponsible(false),
    myHasValidLogic(loaded),
    myAmTLSSelected(false) {
}


GNEJunction::~GNEJunction() {
    // delete all GNECrossing
    for (auto it : myGNECrossings) {
        it->decRef();
        if (it->unreferenced()) {
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + it->getTagStr() + " '" + it->getID() + "' in GNEJunction destructor");
            delete it;
        }
    }

    if (myAmResponsible) {
        // show extra information for tests
        WRITE_DEBUG("Deleting NBNode of '" + getID() + "' in GNEJunction destructor");
        delete &myNBNode;
    }
}


std::string
GNEJunction::generateChildID(SumoXMLTag childTag) {
    int counter = 0;
    while (myNet->retrieveJunction(getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


void
GNEJunction::updateGeometry() {
    updateGeometryAfterNetbuild(true);
}

void
GNEJunction::updateGeometryAfterNetbuild(bool rebuildNBNodeCrossings) {
    myMaxSize = MAX2(getCenteringBoundary().getWidth(), getCenteringBoundary().getHeight());
    rebuildGNECrossings(rebuildNBNodeCrossings);
}


Position
GNEJunction::getPositionInView() const {
    return myNBNode.getPosition();
}


void
GNEJunction::rebuildGNECrossings(bool rebuildNBNodeCrossings) {
    // rebuild GNECrossings only if create crossings and walkingAreas in net is enabled
    if (myNet->getNetBuilder()->haveNetworkCrossings()) {
        if (rebuildNBNodeCrossings) {
            // build new NBNode::Crossings and walking areas
            mirrorXLeftHand();
            myNBNode.buildCrossingsAndWalkingAreas();
            mirrorXLeftHand();
        }
        // create a vector to keep retrieved and created crossings
        std::vector<GNECrossing*> retrievedCrossings;
        // iterate over NBNode::Crossings of GNEJunction
        for (auto NBc : myNBNode.getCrossingsIncludingInvalid()) {
            // retrieve existent GNECrossing, or create it
            GNECrossing* retrievedGNECrossing = retrieveGNECrossing(NBc);
            retrievedCrossings.push_back(retrievedGNECrossing);
            // check if previously this GNECrossings exists, and if true, remove it from myGNECrossings and insert in tree again
            std::vector<GNECrossing*>::iterator retrievedExists = std::find(myGNECrossings.begin(), myGNECrossings.end(), retrievedGNECrossing);
            if (retrievedExists != myGNECrossings.end()) {
                myGNECrossings.erase(retrievedExists);
                // update geometry of retrieved crossing
                retrievedGNECrossing->updateGeometry();
            } else {
                // include reference to created GNECrossing
                retrievedGNECrossing->incRef();
            }
        }
        // delete non retrieved GNECrossings (we don't need to extract if from Tree two times)
        for (auto it : myGNECrossings) {
            it->decRef();
            // check if crossing is selected
            if (it->isAttributeCarrierSelected()) {
                it->unselectAttributeCarrier();
            }
            if (it->unreferenced()) {
                // show extra information for tests
                WRITE_DEBUG("Deleting unreferenced " + it->getTagStr() + " in rebuildGNECrossings()");
                delete it;
            }
        }
        // copy retrieved (existent and created) GNECrossigns to myGNECrossings
        myGNECrossings = retrievedCrossings;
    }
}

void
GNEJunction::mirrorXLeftHand() {
    if (OptionsCont::getOptions().getBool("lefthand")) {
        myNBNode.mirrorX();
        for (NBEdge* e : myNBNode.getEdges()) {
            e->mirrorX();

        }
    }
}


GUIGLObjectPopupMenu*
GNEJunction::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    //if (parent.getVisualisationSettings()->editMode != GNE_MODE_CONNECT) {
    //    // XXX if joinable
    //    new FXMenuCommand(ret, "Join adjacent edges", 0, &parent, MID_GNE_JOIN_EDGES);
    //}
    const int numEndpoints = (int)myNBNode.getEndPoints().size();
    // check if we're handling a selection
    bool handlingSelection = isAttributeCarrierSelected() && (myNet->retrieveJunctions(true).size() > 1);
    // check if menu commands has to be disabled
    const bool wrongMode = (myNet->getViewNet()->getEditModes().networkEditMode == GNE_NMODE_CONNECT) ||
                           (myNet->getViewNet()->getEditModes().networkEditMode == GNE_NMODE_TLS) ||
                           (myNet->getViewNet()->getEditModes().networkEditMode == GNE_NMODE_CREATE_EDGE);
    // create menu commands
    FXMenuCommand* mcCustomShape = new FXMenuCommand(ret, "Set custom junction shape", nullptr, &parent, MID_GNE_JUNCTION_EDIT_SHAPE);
    FXMenuCommand* mcResetCustomShape = new FXMenuCommand(ret, "Reset junction shape", nullptr, &parent, MID_GNE_JUNCTION_RESET_SHAPE);
    FXMenuCommand* mcReplace = new FXMenuCommand(ret, "Replace junction by geometry point", nullptr, &parent, MID_GNE_JUNCTION_REPLACE);
    FXMenuCommand* mcSplit = new FXMenuCommand(ret, ("Split junction (" + toString(numEndpoints) + " end points)").c_str(), nullptr, &parent, MID_GNE_JUNCTION_SPLIT);
    FXMenuCommand* mcSplitReconnect = new FXMenuCommand(ret, "Split junction and reconnect", nullptr, &parent, MID_GNE_JUNCTION_SPLIT_RECONNECT);
    FXMenuCommand* mcClearConnections = new FXMenuCommand(ret, "Clear connections", nullptr, &parent, MID_GNE_JUNCTION_CLEAR_CONNECTIONS);
    FXMenuCommand* mcResetConnections = new FXMenuCommand(ret, "Reset connections", nullptr, &parent, MID_GNE_JUNCTION_RESET_CONNECTIONS);
    // check if current mode  is correct
    if (wrongMode) {
        mcCustomShape->disable();
        mcClearConnections->disable();
        mcResetConnections->disable();
    }
    // check if we're handling a selection
    if (handlingSelection) {
        mcResetCustomShape->setText("Reset junction shapes");
    }
    // disable mcClearConnections if juction hasn't connections
    if (getGNEConnections().empty()) {
        mcClearConnections->disable();
    }
    // disable mcResetCustomShape if junction doesn't have a custom shape
    if (myNBNode.getShape().size() == 0) {
        mcResetCustomShape->disable();
    }
    // checkIsRemovable requiers turnarounds to be computed. This is ugly
    if ((myNBNode.getIncomingEdges().size() == 2) && (myNBNode.getOutgoingEdges().size() == 2)) {
        NBTurningDirectionsComputer::computeTurnDirectionsForNode(&myNBNode, false);
    }
    std::string reason = "wrong edit mode";
    if (wrongMode || !myNBNode.checkIsRemovableReporting(reason)) {
        mcReplace->setText(mcReplace->getText() + " (" + reason.c_str() + ")");
        mcReplace->disable();
    }
    if (numEndpoints == 1) {
        mcSplit->disable();
        mcSplitReconnect->disable();
    }
    return ret;
}


Boundary
GNEJunction::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMovingGeometryBoundary.isInitialised()) {
        return myMovingGeometryBoundary;
    } else if (myNBNode.getShape().size() > 0) {
        Boundary b = myNBNode.getShape().getBoxBoundary();
        b.grow(10);
        return b;
    } else {
        // calculate boundary using EXTENT as size
        const double EXTENT = 2;
        Boundary b(myNBNode.getPosition().x() - EXTENT, myNBNode.getPosition().y() - EXTENT,
                   myNBNode.getPosition().x() + EXTENT, myNBNode.getPosition().y() + EXTENT);
        b.grow(10);
        return b;
    }
}


void
GNEJunction::drawGL(const GUIVisualizationSettings& s) const {
    // check if boundary has to be drawn
    if (s.drawBoundaries) {
        GLHelper::drawBoundary(getCenteringBoundary());
    }
    // declare variables
    double exaggeration = isAttributeCarrierSelected() ? s.selectionScale : 1;
    exaggeration *= s.junctionSize.getExaggeration(s, this, 4);
    // declare values for circles
    double circleWidth = BUBBLE_RADIUS * exaggeration;
    double circleWidthSquared = circleWidth * circleWidth;
    // push name
    if (s.scale * exaggeration * myMaxSize < 1.) {
        // draw something simple so that selection still works
        glPushName(getGlID());
        GLHelper::drawBoxLine(myNBNode.getPosition(), 0, 1, 1);
        glPopName();
    } else {
        // node shape has been computed and is valid for drawing
        glPushName(getGlID());
        // declare flag for drawing junction shape
        const bool drawShape = (myNBNode.getShape().size() > 0) && s.drawJunctionShape;
        // declare flag for drawing junction as bubbles
        bool drawBubble = (!drawShape || (myNBNode.getShape().area() < 4)) && s.drawJunctionShape;
        // check if show junctions as bubbles checkbox is enabled
        if (myNet->getViewNet()->showJunctionAsBubbles()) {
            drawBubble = true;
        }
        // in supermode demand Bubble musn't be drawn
        if (myNet->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) {
            drawBubble = false;
        }
        // check if shape has to be drawn
        if (drawShape) {
            RGBColor color = setColor(s, false);
            // recognize full transparency and simply don't draw
            if (color.alpha() != 0) {
                glPushMatrix();
                glTranslated(0, 0, getType());
                PositionVector shape = myNBNode.getShape();
                shape.closePolygon();
                if (exaggeration > 1) {
                    shape.scaleRelative(exaggeration);
                }
                if ((s.drawForSelecting) || (s.scale * exaggeration * myMaxSize < 40.)) {
                    GLHelper::drawFilledPoly(shape, true);
                } else {
                    GLHelper::drawFilledPolyTesselated(shape, true);
                }
                // check if dotted contour has to be drawn
                if ((myNet->getViewNet()->getDottedAC() == this) && !drawBubble) {
                    GLHelper::drawShapeDottedContourAroundClosedShape(s, getType(), shape);
                }
                glPopMatrix();
            }
        }
        // check if bubble has to be drawn
        if (drawBubble) {
            RGBColor color = setColor(s, true);
            // recognize full transparency and simply don't draw
            if (color.alpha() != 0) {
                glPushMatrix();
                glTranslated(myNBNode.getPosition().x(), myNBNode.getPosition().y(), getType() + 0.05);
                if (!s.drawForSelecting || (myNet->getViewNet()->getPositionInformation().distanceSquaredTo2D(myNBNode.getPosition()) <= (circleWidthSquared + 2))) {
                    std::vector<Position> vertices = GLHelper::drawFilledCircleReturnVertices(circleWidth, s.getCircleResolution());
                    // check if dotted contour has to be drawn
                    if (myNet->getViewNet()->getDottedAC() == this) {
                        GLHelper::drawShapeDottedContourAroundClosedShape(s, getType(), vertices);
                    }
                } else {
                    GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
                }
                glPopMatrix();
            }
        }
        // draw TLS icon if isn't being drawn for selecting
        if ((myNet->getViewNet()->getEditModes().networkEditMode == GNE_NMODE_TLS) &&
                (myNBNode.isTLControlled()) && !myAmTLSSelected && !s.drawForSelecting) {
            glPushMatrix();
            Position pos = myNBNode.getPosition();
            glTranslated(pos.x(), pos.y(), getType() + 0.1);
            glColor3d(1, 1, 1);
            const double halfWidth = 32 / s.scale;
            const double halfHeight = 64 / s.scale;
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_TLS), -halfWidth, -halfHeight, halfWidth, halfHeight);
            glPopMatrix();
        }
        // (optional) draw name @todo expose this setting if isn't drawed if isn't being drawn for selecting
        if (!s.drawForSelecting) {
            drawName(myNBNode.getPosition(), s.scale, s.junctionName);
        }
        // draw elevation
        if (!s.drawForSelecting && myNet->getViewNet()->getNetworkViewOptions().editingElevation()) {
            glPushMatrix();
            // Translate to center of junction
            glTranslated(myNBNode.getPosition().x(), myNBNode.getPosition().y(), getType() + 1);
            // draw Z value
            GLHelper::drawText(toString(myNBNode.getPosition().z()), Position(), GLO_MAX - 5, s.junctionName.scaledSize(s.scale), s.junctionName.color);
            glPopMatrix();
        }
        // name must be removed from selection stack before drawing crossings
        glPopName();
        // draw crossings only if junction isn't being moved
        if (!myMovingGeometryBoundary.isInitialised()) {
            for (const auto& i : myGNECrossings) {
                i->drawGL(s);
            }
        }
        // draw connections and route elements connections (Only for incoming edges)
        for (const auto& i : myGNEIncomingEdges) {
            // first draw connections
            for (const auto& j : i->getGNEConnections()) {
                j->drawGL(s);
            }
            // first check if Demand elements can be shown
            if (myNet->getViewNet()->getNetworkViewOptions().showDemandElements()) {
                // certain demand elements children can contain loops (for example, routes) and it causes overlapping problems. It's needed to filter it before drawing
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_ROUTE)) {
                    // first check if route can be drawn
                    if (myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(j)) {
                        // draw partial route
                        i->drawPartialRoute(s, j, this);
                    }
                }
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_EMBEDDEDROUTE)) {
                    // first check if embedded route can be drawn
                    if (myNet->getViewNet()->getDemandViewOptions().showNonInspectedDemandElements(j)) {
                        // draw partial route
                        i->drawPartialRoute(s, j, this);
                    }
                }
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_TRIP)) {
                    // Start drawing adding an gl identificator
                    glPushName(j->getGlID());
                    // draw partial trip only if is being inspected or selected
                    if ((myNet->getViewNet()->getDottedAC() == j) || j->isAttributeCarrierSelected()) {
                        i->drawPartialTripFromTo(s, j, this);
                    }
                    // only draw trip in the first edge
                    if (j->getAttribute(SUMO_ATTR_FROM) == getID()) {
                        j->drawGL(s);
                    }
                    // Pop name
                    glPopName();
                }
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_FLOW)) {
                    // Start drawing adding an gl identificator
                    glPushName(j->getGlID());
                    // draw partial trip only if is being inspected or selected
                    if ((myNet->getViewNet()->getDottedAC() == j) || j->isAttributeCarrierSelected()) {
                        i->drawPartialTripFromTo(s, j, this);
                    }
                    // only draw flow in the first edge
                    if (j->getAttribute(SUMO_ATTR_FROM) == getID()) {
                        j->drawGL(s);
                    }
                    // Pop name
                    glPopName();
                }
                // draw partial person plan elements
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_PERSONTRIP_FROMTO)) {
                    i->drawPartialPersonPlan(s, j, this);
                }
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_PERSONTRIP_BUSSTOP)) {
                    i->drawPartialPersonPlan(s, j, this);
                }
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_WALK_EDGES)) {
                    i->drawPartialPersonPlan(s, j, this);
                }
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_WALK_FROMTO)) {
                    i->drawPartialPersonPlan(s, j, this);
                }
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_WALK_BUSSTOP)) {
                    i->drawPartialPersonPlan(s, j, this);
                }
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_WALK_ROUTE)) {
                    i->drawPartialPersonPlan(s, j, this);
                }
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_RIDE_FROMTO)) {
                    i->drawPartialPersonPlan(s, j, this);
                }
                for (const auto& j : i->getSortedDemandElementChildrenByType(SUMO_TAG_RIDE_BUSSTOP)) {
                    i->drawPartialPersonPlan(s, j, this);
                }
            }
        }
    }
}


NBNode*
GNEJunction::getNBNode() const {
    return &myNBNode;
}


std::vector<GNEJunction*>
GNEJunction::getJunctionNeighbours() const {
    // use set to avoid duplicates junctions
    std::set<GNEJunction*> junctions;
    for (auto i : myGNEIncomingEdges) {
        junctions.insert(i->getGNEJunctionSource());
    }
    for (auto i : myGNEOutgoingEdges) {
        junctions.insert(i->getGNEJunctionDestiny());
    }
    return std::vector<GNEJunction*>(junctions.begin(), junctions.end());
}


void
GNEJunction::addIncomingGNEEdge(GNEEdge* edge) {
    // Check if incoming edge was already inserted
    std::vector<GNEEdge*>::iterator i = std::find(myGNEIncomingEdges.begin(), myGNEIncomingEdges.end(), edge);
    if (i != myGNEIncomingEdges.end()) {
        throw InvalidArgument("Incoming " + toString(SUMO_TAG_EDGE) + " with ID '" + edge->getID() + "' was already inserted into " + getTagStr() + " with ID " + getID() + "'");
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
        throw InvalidArgument("Outgoing " + toString(SUMO_TAG_EDGE) + " with ID '" + edge->getID() + "' was already inserted into " + getTagStr() + " with ID " + getID() + "'");
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
        throw InvalidArgument("Incoming " + toString(SUMO_TAG_EDGE) + " with ID '" + edge->getID() + "' doesn't found into " + getTagStr() + " with ID " + getID() + "'");
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
        throw InvalidArgument("Outgoing " + toString(SUMO_TAG_EDGE) + " with ID '" + edge->getID() + "' doesn't found into " + getTagStr() + " with ID " + getID() + "'");
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


std::vector<GNEConnection*>
GNEJunction::getGNEConnections() const {
    std::vector<GNEConnection*> connections;
    for (auto i : myGNEIncomingEdges) {
        for (auto j : i->getGNEConnections()) {
            connections.push_back(j);
        }
    }
    return connections;
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
GNEJunction::startGeometryMoving(bool extendToNeighbors) {
    // save current centering boundary
    myMovingGeometryBoundary = getCenteringBoundary();
    // First declare three sets with all affected GNEJunctions, GNEEdges and GNEConnections
    std::set<GNEJunction*> affectedJunctions;
    std::set<GNEEdge*> affectedEdges;
    // Iterate over GNEEdges
    for (auto i : myGNEEdges) {
        // Add source and destiny junctions
        affectedJunctions.insert(i->getGNEJunctionSource());
        affectedJunctions.insert(i->getGNEJunctionDestiny());
        // Obtain neighbors of Junction source
        for (auto j : i->getGNEJunctionSource()->getGNEEdges()) {
            affectedEdges.insert(j);
        }
        // Obtain neighbors of Junction destiny
        for (auto j : i->getGNEJunctionDestiny()->getGNEEdges()) {
            affectedEdges.insert(j);
        }
    }
    // Iterate over affected Junctions only if extendToNeighbors is enabled
    if (extendToNeighbors) {
        for (auto i : affectedJunctions) {
            // don't include this junction (to avoid start more than one times)
            if (i != this) {
                // start geometry moving in edges
                i->startGeometryMoving(false);
            }
        }
    }
    // Iterate over affected Edges
    for (auto i : affectedEdges) {
        // start geometry moving in edges
        i->startGeometryMoving();
    }
}


void
GNEJunction::endGeometryMoving(bool extendToNeighbors) {
    // check that endGeometryMoving was called only once
    if (myMovingGeometryBoundary.isInitialised()) {
        // Remove object from net
        myNet->removeGLObjectFromGrid(this);
        // reset myMovingGeometryBoundary
        myMovingGeometryBoundary.reset();
        // First declare three sets with all affected GNEJunctions, GNEEdges and GNEConnections
        std::set<GNEJunction*> affectedJunctions;
        std::set<GNEEdge*> affectedEdges;
        // Iterate over GNEEdges
        for (auto i : myGNEEdges) {
            // Add source and destiny junctions
            affectedJunctions.insert(i->getGNEJunctionSource());
            affectedJunctions.insert(i->getGNEJunctionDestiny());
            // Obtain neighbors of Junction source
            for (auto j : i->getGNEJunctionSource()->getGNEEdges()) {
                affectedEdges.insert(j);
            }
            // Obtain neighbors of Junction destiny
            for (auto j : i->getGNEJunctionDestiny()->getGNEEdges()) {
                affectedEdges.insert(j);
            }
        }
        // Iterate over affected Junctions
        if (extendToNeighbors) {
            for (auto i : affectedJunctions) {
                // don't include this junction (to avoid end it more than one times)
                if (i != this) {
                    // end geometry moving in edges
                    i->endGeometryMoving(false);
                }
            }
        }
        // Iterate over affected Edges
        for (auto i : affectedEdges) {
            // end geometry moving in edges
            i->endGeometryMoving();
        }
        // add object into grid again (using the new centering boundary)
        myNet->addGLObjectIntoGrid(this);
    }
}


void
GNEJunction::moveGeometry(const Position& oldPos, const Position& offset) {
    // Abort moving if there is another junction in the exactly target position
    bool abort = false;
    std::vector<GNEJunction*> junctionNeighbours = getJunctionNeighbours();
    for (auto i : junctionNeighbours) {
        if (i->getPositionInView() == myNBNode.getPosition()) {
            abort = true;
        }
    }
    if (!abort) {
        // calculate new position
        Position newPosition = oldPos;
        newPosition.add(offset);
        // filtern position using snap to active grid
        newPosition = myNet->getViewNet()->snapToActiveGrid(newPosition, offset.z() == 0);
        // move junction geometry without updating grid
        moveJunctionGeometry(newPosition);
    }
}


void
GNEJunction::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    // first end geometry point
    endGeometryMoving();
    if (isValid(SUMO_ATTR_POSITION, toString(myNBNode.getPosition()))) {
        undoList->p_begin("position of " + getTagStr());
        undoList->p_add(new GNEChange_Attribute(this, myNet, SUMO_ATTR_POSITION, toString(myNBNode.getPosition()), true, toString(oldPos)));
        undoList->p_end();
    } else {
        // tried to set an invalid position, revert back to the previous one
        moveJunctionGeometry(oldPos);
    }
}


void
GNEJunction::invalidateShape() {
    if (!myNBNode.hasCustomShape()) {
        if (myNBNode.myPoly.size() > 0) {
            // write GL Debug
            WRITE_GLDEBUG("<-- Invalidating shape of junction '" + getID() + "' -->");
            // remove Juntion from grid
            myNet->removeGLObjectFromGrid(this);
            // clear poly
            myNBNode.myPoly.clear();
            // add Juntion into grid
            myNet->addGLObjectIntoGrid(this);
        }
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
            GNEEdge* srcEdge = myNet->retrieveEdge((*it)->getID());
            removeConnectionsFrom(srcEdge, undoList, false); // false, because the whole tls will be invalidated at the end
            undoList->add(new GNEChange_Attribute(srcEdge, myNet, GNE_ATTR_MODIFICATION_STATUS, status), true);
        }
        undoList->add(new GNEChange_Attribute(this, myNet, GNE_ATTR_MODIFICATION_STATUS, status), true);
        invalidateTLS(undoList);
    } else {
        // logic valed, then rebuild GNECrossings to adapt it to the new logic
        // (but don't rebuild the crossings in NBNode because they are already finished)
        rebuildGNECrossings(false);
    }
}


void
GNEJunction::removeConnectionsFrom(GNEEdge* edge, GNEUndoList* undoList, bool updateTLS, int lane) {
    NBEdge* srcNBE = edge->getNBEdge();
    NBEdge* turnEdge = srcNBE->getTurnDestination();
    // Make a copy of connections
    std::vector<NBEdge::Connection> connections = srcNBE->getConnections();
    // delete in reverse so that undoing will add connections in the original order
    for (std::vector<NBEdge::Connection>::reverse_iterator con_it = connections.rbegin(); con_it != connections.rend(); con_it++) {
        if (lane >= 0 && (*con_it).fromLane != lane) {
            continue;
        }
        bool hasTurn = con_it->toEdge == turnEdge;
        undoList->add(new GNEChange_Connection(edge, *con_it, false, false), true);
        // needs to come after GNEChange_Connection
        // XXX bug: this code path will not be used on a redo!
        if (hasTurn) {
            myNet->addExplicitTurnaround(srcNBE->getID());
        }
    }
    if (updateTLS) {
        std::vector<NBConnection> removeConnections;
        for (NBEdge::Connection con : connections) {
            removeConnections.push_back(NBConnection(srcNBE, con.fromLane, con.toEdge, con.toLane));
        }
        removeTLSConnections(removeConnections, undoList);
    }
}


void
GNEJunction::removeConnectionsTo(GNEEdge* edge, GNEUndoList* undoList, bool updateTLS, int lane) {
    NBEdge* destNBE = edge->getNBEdge();
    std::vector<NBConnection> removeConnections;
    for (NBEdge* srcNBE : myNBNode.getIncomingEdges()) {
        GNEEdge* srcEdge = myNet->retrieveEdge(srcNBE->getID());
        std::vector<NBEdge::Connection> connections = srcNBE->getConnections();
        for (std::vector<NBEdge::Connection>::reverse_iterator con_it = connections.rbegin(); con_it != connections.rend(); con_it++) {
            if ((*con_it).toEdge == destNBE) {
                if (lane >= 0 && (*con_it).toLane != lane) {
                    continue;
                }
                bool hasTurn = srcNBE->getTurnDestination() == destNBE;
                undoList->add(new GNEChange_Connection(srcEdge, *con_it, false, false), true);
                // needs to come after GNEChange_Connection
                // XXX bug: this code path will not be used on a redo!
                if (hasTurn) {
                    myNet->addExplicitTurnaround(srcNBE->getID());
                }
                removeConnections.push_back(NBConnection(srcNBE, (*con_it).fromLane, destNBE, (*con_it).toLane));
            }
        }
    }
    if (updateTLS) {
        removeTLSConnections(removeConnections, undoList);
    }
}


void
GNEJunction::removeTLSConnections(std::vector<NBConnection>& connections, GNEUndoList* undoList) {
    if (connections.size() == 0) {
        return;
    }
    const std::set<NBTrafficLightDefinition*> coypOfTls = myNBNode.getControllingTLS(); // make a copy!
    for (auto it : coypOfTls) {
        NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(it);
        // guessed TLS (NBOwnTLDef) do not need to be updated
        if (tlDef != nullptr) {
            std::string newID = tlDef->getID();
            // create replacement before deleting the original because deletion will mess up saving original nodes
            NBLoadedSUMOTLDef* replacementDef = new NBLoadedSUMOTLDef(tlDef, tlDef->getLogic());
            for (NBConnection& con : connections) {
                replacementDef->removeConnection(con);
            }
            undoList->add(new GNEChange_TLS(this, tlDef, false), true);
            undoList->add(new GNEChange_TLS(this, replacementDef, true, false, newID), true);
            // the removed traffic light may have controlled more than one junction. These too have become invalid now
            const std::vector<NBNode*> copyOfNodes = tlDef->getNodes(); // make a copy!
            for (auto it_node : copyOfNodes) {
                GNEJunction* sharing = myNet->retrieveJunction(it_node->getID());
                undoList->add(new GNEChange_TLS(sharing, tlDef, false), true);
                undoList->add(new GNEChange_TLS(sharing, replacementDef, true, false, newID), true);
            }
        }
    }
}


void
GNEJunction::replaceIncomingConnections(GNEEdge* which, GNEEdge* by, GNEUndoList* undoList) {
    // remap connections of the edge
    assert(which->getLanes().size() == by->getLanes().size());
    std::vector<NBEdge::Connection> connections = which->getNBEdge()->getConnections();
    for (NBEdge::Connection& c : connections) {
        undoList->add(new GNEChange_Connection(which, c, false, false), true);
        undoList->add(new GNEChange_Connection(by, c, false, true), true);
    }
    // also remap tls connections
    const std::set<NBTrafficLightDefinition*> coypOfTls = myNBNode.getControllingTLS(); // make a copy!
    for (auto it : coypOfTls) {
        NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(it);
        // guessed TLS (NBOwnTLDef) do not need to be updated
        if (tlDef != nullptr) {
            std::string newID = tlDef->getID();
            // create replacement before deleting the original because deletion will mess up saving original nodes
            NBLoadedSUMOTLDef* replacementDef = new NBLoadedSUMOTLDef(tlDef, tlDef->getLogic());
            for (int i = 0; i < (int)which->getLanes().size(); ++i) {
                replacementDef->replaceRemoved(which->getNBEdge(), i, by->getNBEdge(), i);
            }
            undoList->add(new GNEChange_TLS(this, tlDef, false), true);
            undoList->add(new GNEChange_TLS(this, replacementDef, true, false, newID), true);
            // the removed traffic light may have controlled more than one junction. These too have become invalid now
            const std::vector<NBNode*> copyOfNodes = tlDef->getNodes(); // make a copy!
            for (auto it_node : copyOfNodes) {
                GNEJunction* sharing = myNet->retrieveJunction(it_node->getID());
                undoList->add(new GNEChange_TLS(sharing, tlDef, false), true);
                undoList->add(new GNEChange_TLS(sharing, replacementDef, true, false, newID), true);
            }
        }
    }
}


void
GNEJunction::markAsModified(GNEUndoList* undoList) {
    EdgeVector incoming = myNBNode.getIncomingEdges();
    for (EdgeVector::iterator it = incoming.begin(); it != incoming.end(); it++) {
        NBEdge* srcNBE = *it;
        GNEEdge* srcEdge = myNet->retrieveEdge(srcNBE->getID());
        undoList->add(new GNEChange_Attribute(srcEdge, myNet, GNE_ATTR_MODIFICATION_STATUS, FEATURE_MODIFIED), true);
    }
}


void
GNEJunction::invalidateTLS(GNEUndoList* undoList, const NBConnection& deletedConnection, const NBConnection& addedConnection) {
    assert(undoList->hasCommandGroup());
    // NBLoadedSUMOTLDef becomes invalid, replace with NBOwnTLDef which will be dynamically recomputed
    const std::set<NBTrafficLightDefinition*> coypOfTls = myNBNode.getControllingTLS(); // make a copy!
    for (auto it : coypOfTls) {
        NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(it);
        if (tlDef != nullptr) {
            NBTrafficLightDefinition* replacementDef = nullptr;
            std::string newID = tlDef->getID(); // + "_reguessed"; // changes due to reguessing will be visible in diff
            if (deletedConnection != NBConnection::InvalidConnection) {
                // create replacement before deleting the original because deletion will mess up saving original nodes
                NBLoadedSUMOTLDef* repl = new NBLoadedSUMOTLDef(tlDef, tlDef->getLogic());
                repl->removeConnection(deletedConnection);
                replacementDef = repl;
            } else if (addedConnection != NBConnection::InvalidConnection) {
                if (addedConnection.getTLIndex() == NBConnection::InvalidTlIndex) {
                    // custom tl indices of crossings might become invalid upon recomputation so we must save them
                    // however, the could remain valud so we register a change but keep them at their old value
                    for (GNECrossing* c : myGNECrossings) {
                        const std::string oldValue = c->getAttribute(SUMO_ATTR_TLLINKINDEX);
                        undoList->add(new GNEChange_Attribute(c, myNet, SUMO_ATTR_TLLINKINDEX, toString(NBConnection::InvalidTlIndex)), true);
                        undoList->add(new GNEChange_Attribute(c, myNet, SUMO_ATTR_TLLINKINDEX, oldValue), true);
                        const std::string oldValue2 = c->getAttribute(SUMO_ATTR_TLLINKINDEX);
                        undoList->add(new GNEChange_Attribute(c, myNet, SUMO_ATTR_TLLINKINDEX2, toString(NBConnection::InvalidTlIndex)), true);
                        undoList->add(new GNEChange_Attribute(c, myNet, SUMO_ATTR_TLLINKINDEX2, oldValue2), true);
                    }
                }
                NBLoadedSUMOTLDef* repl = new NBLoadedSUMOTLDef(tlDef, tlDef->getLogic());
                repl->addConnection(addedConnection.getFrom(), addedConnection.getTo(),
                                    addedConnection.getFromLane(), addedConnection.getToLane(), addedConnection.getTLIndex());
                replacementDef = repl;
            } else {
                replacementDef = new NBOwnTLDef(newID, tlDef->getOffset(), tlDef->getType());
                replacementDef->setProgramID(tlDef->getProgramID());
            }
            undoList->add(new GNEChange_TLS(this, tlDef, false), true);
            undoList->add(new GNEChange_TLS(this, replacementDef, true, false, newID), true);
            // the removed traffic light may have controlled more than one junction. These too have become invalid now
            const std::vector<NBNode*> copyOfNodes = tlDef->getNodes(); // make a copy!
            for (auto it_node : copyOfNodes) {
                GNEJunction* sharing = myNet->retrieveJunction(it_node->getID());
                undoList->add(new GNEChange_TLS(sharing, tlDef, false), true);
                undoList->add(new GNEChange_TLS(sharing, replacementDef, true, false, newID), true);
            }
        }
    }
}

void
GNEJunction::removeEdgeFromCrossings(GNEEdge* edge, GNEUndoList* undoList) {
    // obtain a copy of GNECrossing of junctions
    auto copyOfGNECrossings = myGNECrossings;
    // iterate over copy of GNECrossings
    for (int i = 0; i < (int)myGNECrossings.size(); i++) {
        auto c = myGNECrossings.at(i);
        // obtain the set of edges vinculated with the crossing (due it works as ID)
        EdgeSet edgeSet(c->getCrossingEdges().begin(), c->getCrossingEdges().end());
        // If this edge is part of the set of edges of crossing
        if (edgeSet.count(edge->getNBEdge()) == 1) {
            // delete crossing if this is their last edge
            if ((c->getCrossingEdges().size() == 1) && (c->getCrossingEdges().front() == edge->getNBEdge())) {
                myNet->deleteCrossing(c, undoList);
                i = 0;
            } else {
                // remove this edge of the edge's attribute of crossing (note: This can invalidate the crossing)
                std::vector<std::string> edges = GNEAttributeCarrier::parse<std::vector<std::string>>(c->getAttribute(SUMO_ATTR_EDGES));
                edges.erase(std::find(edges.begin(), edges.end(), edge->getID()));
                c->setAttribute(SUMO_ATTR_EDGES, joinToString(edges, " "), undoList);
            }
        }
    }
}


bool
GNEJunction::isLogicValid() {
    return myHasValidLogic;
}


GNECrossing*
GNEJunction::retrieveGNECrossing(NBNode::Crossing* crossing, bool createIfNoExist) {
    // iterate over all crossing
    for (auto i : myGNECrossings) {
        // if found, return it
        if (i->getCrossingEdges() == crossing->edges) {
            return i;
        }
    }
    if (createIfNoExist) {
        // create new GNECrossing
        GNECrossing* createdGNECrossing = new GNECrossing(this, crossing->edges);
        // show extra information for tests
        WRITE_DEBUG("Created " + createdGNECrossing->getTagStr() + " '" + createdGNECrossing->getID() + "' in retrieveGNECrossing()");
        // update geometry after creating
        createdGNECrossing->updateGeometry();
        return createdGNECrossing;
    } else {
        return nullptr;
    }
}


void
GNEJunction::markConnectionsDeprecated(bool includingNeighbours) {
    // only it's needed to mark the connections of incoming edges
    for (auto i : myGNEIncomingEdges) {
        for (auto j : i->getGNEConnections()) {
            j->markConnectionGeometryDeprecated();
        }
        if (includingNeighbours) {
            i->getGNEJunctionSource()->markConnectionsDeprecated(false);
        }
    }
}


std::string
GNEJunction::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_POSITION:
            return toString(myNBNode.getPosition());
        case SUMO_ATTR_TYPE:
            return toString(myNBNode.getType());
        case GNE_ATTR_MODIFICATION_STATUS:
            return myLogicStatus;
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
            // keep clear is only used as a convenience feature in plain xml
            // input. When saving to .net.xml the status is saved only for the connections
            // to show the correct state we must check all connections
            if (!myNBNode.getKeepClear()) {
                return toString(false);
            } else {
                for (auto i : myGNEIncomingEdges) {
                    for (auto j : i->getGNEConnections()) {
                        if (j->getNBEdgeConnection().keepClear) {
                            return toString(true);
                        }
                    }
                }
                return toString(false);
            }
        case SUMO_ATTR_RIGHT_OF_WAY:
            return SUMOXMLDefinitions::RightOfWayValues.getString(myNBNode.getRightOfWay());
        case SUMO_ATTR_FRINGE:
            return SUMOXMLDefinitions::FringeTypeValues.getString(myNBNode.getFringeType());
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
        case SUMO_ATTR_RIGHT_OF_WAY:
        case SUMO_ATTR_FRINGE:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->add(new GNEChange_Attribute(this, myNet, key, value), true);
            break;
        case SUMO_ATTR_KEEP_CLEAR:
            // change Keep Clear attribute in all connections
            undoList->p_begin("change keepClear for whole junction");
            for (auto i : myGNEIncomingEdges) {
                for (auto j : i->getGNEConnections()) {
                    undoList->add(new GNEChange_Attribute(j, myNet, key, value), true);
                }
            }
            undoList->add(new GNEChange_Attribute(this, myNet, key, value), true);
            undoList->p_end();
            break;
        case SUMO_ATTR_TYPE: {
            undoList->p_begin("change " + getTagStr() + " type");
            if (NBNode::isTrafficLight(SUMOXMLDefinitions::NodeTypes.get(value))) {
                if (getNBNode()->isTLControlled() &&
                        // if switching changing from or to traffic_light_right_on_red we need to remove the old plan
                        (getNBNode()->getType() == NODETYPE_TRAFFIC_LIGHT_RIGHT_ON_RED
                         || SUMOXMLDefinitions::NodeTypes.get(value) == NODETYPE_TRAFFIC_LIGHT_RIGHT_ON_RED)
                   ) {
                    // make a copy because we will modify the original
                    const std::set<NBTrafficLightDefinition*> copyOfTls = myNBNode.getControllingTLS();
                    for (auto it : copyOfTls) {
                        undoList->add(new GNEChange_TLS(this, it, false), true);
                    }
                }
                if (!getNBNode()->isTLControlled()) {
                    // create new traffic light
                    undoList->add(new GNEChange_TLS(this, nullptr, true), true);
                }
            } else if (getNBNode()->isTLControlled()) {
                // delete old traffic light
                // make a copy because we will modify the original
                const std::set<NBTrafficLightDefinition*> copyOfTls = myNBNode.getControllingTLS();
                for (auto it : copyOfTls) {
                    undoList->add(new GNEChange_TLS(this, it, false, false), true);
                }
            }
            // must be the final step, otherwise we do not know which traffic lights to remove via GNEChange_TLS
            undoList->add(new GNEChange_Attribute(this, myNet, key, value), true);
            for (auto it : myGNECrossings) {
                undoList->add(new GNEChange_Attribute(it, myNet, SUMO_ATTR_TLLINKINDEX, "-1"), true);
                undoList->add(new GNEChange_Attribute(it, myNet, SUMO_ATTR_TLLINKINDEX2, "-1"), true);
            }
            undoList->p_end();
            break;
        }
        case SUMO_ATTR_TLTYPE: {
            undoList->p_begin("change " + getTagStr() + " tl-type");
            // make a copy because we will modify the original
            const std::set<NBTrafficLightDefinition*> copyOfTls = myNBNode.getControllingTLS();
            for (auto oldDef : copyOfTls) {
                NBLoadedSUMOTLDef* oldLoaded = dynamic_cast<NBLoadedSUMOTLDef*>(oldDef);
                if (oldLoaded != nullptr) {
                    NBLoadedSUMOTLDef* newDef = new NBLoadedSUMOTLDef(oldLoaded, oldLoaded->getLogic());
                    newDef->guessMinMaxDuration();
                    std::vector<NBNode*> nodes = oldDef->getNodes();
                    for (auto it : nodes) {
                        GNEJunction* junction = myNet->retrieveJunction(it->getID());
                        undoList->add(new GNEChange_TLS(junction, oldDef, false), true);
                        undoList->add(new GNEChange_TLS(junction, newDef, true), true);
                    }
                }
            }
            undoList->add(new GNEChange_Attribute(this, myNet, key, value), true);
            undoList->p_end();
            break;
        }
        case SUMO_ATTR_TLID: {
            undoList->p_begin("change " + toString(SUMO_TAG_TRAFFIC_LIGHT) + " id");
            const std::set<NBTrafficLightDefinition*> copyOfTls = myNBNode.getControllingTLS();
            assert(copyOfTls.size() > 0);
            NBTrafficLightDefinition* currentTLS = *copyOfTls.begin();
            NBTrafficLightDefinition* currentTLSCopy = nullptr;
            const bool currentIsSingle = currentTLS->getNodes().size() == 1;
            const bool currentIsLoaded = dynamic_cast<NBLoadedSUMOTLDef*>(currentTLS) != nullptr;
            if (currentIsLoaded) {
                currentTLSCopy = new NBLoadedSUMOTLDef(currentTLS,
                                                       dynamic_cast<NBLoadedSUMOTLDef*>(currentTLS)->getLogic());
            }
            // remove from previous tls
            for (auto it : copyOfTls) {
                undoList->add(new GNEChange_TLS(this, it, false), true);
            }
            NBTrafficLightLogicCont& tlCont = myNet->getTLLogicCont();
            // programs to which the current node shall be added
            const std::map<std::string, NBTrafficLightDefinition*> programs = tlCont.getPrograms(value);
            if (programs.size() > 0) {
                for (auto it : programs) {
                    NBTrafficLightDefinition* oldTLS = it.second;
                    if (dynamic_cast<NBOwnTLDef*>(oldTLS) != nullptr) {
                        undoList->add(new GNEChange_TLS(this, oldTLS, true), true);
                    } else {
                        // delete and re-create the definition because the loaded phases are now invalid
                        if (dynamic_cast<NBLoadedSUMOTLDef*>(oldTLS) != nullptr &&
                                dynamic_cast<NBLoadedSUMOTLDef*>(oldTLS)->usingSignalGroups()) {
                            // keep the old program and add all-red state for the added links
                            NBLoadedSUMOTLDef* newTLSJoined = new NBLoadedSUMOTLDef(oldTLS, dynamic_cast<NBLoadedSUMOTLDef*>(oldTLS)->getLogic());
                            newTLSJoined->joinLogic(currentTLSCopy);
                            undoList->add(new GNEChange_TLS(this, newTLSJoined, true, true), true);
                        } else {
                            undoList->add(new GNEChange_TLS(this, nullptr, true, false, value), true);
                        }
                        NBTrafficLightDefinition* newTLS = *myNBNode.getControllingTLS().begin();
                        // switch from old to new definition
                        const std::vector<NBNode*> copyOfNodes = oldTLS->getNodes();
                        for (auto it_node : copyOfNodes) {
                            GNEJunction* oldJunction = myNet->retrieveJunction(it_node->getID());
                            undoList->add(new GNEChange_TLS(oldJunction, oldTLS, false), true);
                            undoList->add(new GNEChange_TLS(oldJunction, newTLS, true), true);
                        }
                    }
                }
            } else {
                if (currentIsSingle && currentIsLoaded) {
                    // rename the traffic light but keep everything else
                    NBTrafficLightLogic* renamedLogic = dynamic_cast<NBLoadedSUMOTLDef*>(currentTLSCopy)->getLogic();
                    renamedLogic->setID(value);
                    NBLoadedSUMOTLDef* renamedTLS = new NBLoadedSUMOTLDef(currentTLSCopy, renamedLogic);
                    renamedTLS->setID(value);
                    undoList->add(new GNEChange_TLS(this, renamedTLS, true, true), true);
                } else {
                    // create new traffic light
                    undoList->add(new GNEChange_TLS(this, nullptr, true, false, value), true);
                }
            }
            delete currentTLSCopy;
            undoList->p_end();
            break;
        }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEJunction::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->retrieveJunction(value, false) == nullptr);
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::NodeTypes.hasString(value);
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
        case SUMO_ATTR_SHAPE:
            // empty shapes are allowed
            return canParse<PositionVector>(value);
        case SUMO_ATTR_RADIUS:
            return canParse<double>(value) && (parse<double>(value) >= -1);
        case SUMO_ATTR_TLTYPE:
            return myNBNode.isTLControlled() && SUMOXMLDefinitions::TrafficLightTypes.hasString(value);
        case SUMO_ATTR_TLID:
            return myNBNode.isTLControlled() && (value != "");
        case SUMO_ATTR_KEEP_CLEAR:
            return canParse<bool>(value);
        case SUMO_ATTR_RIGHT_OF_WAY:
            return SUMOXMLDefinitions::RightOfWayValues.hasString(value);
        case SUMO_ATTR_FRINGE:
            return SUMOXMLDefinitions::FringeTypeValues.hasString(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEJunction::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : myNBNode.getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEJunction::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : myNBNode.getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEJunction::setGenericParametersStr(const std::string& value) {
    // clear parameters
    myNBNode.clearParameter();
    // separate value in a vector of string using | as separator
    std::vector<std::string> parsedValues;
    StringTokenizer stValues(value, "|", true);
    while (stValues.hasNext()) {
        parsedValues.push_back(stValues.next());
    }
    // check that parsed values (A=B)can be parsed in generic parameters
    for (auto i : parsedValues) {
        std::vector<std::string> parsedParameters;
        StringTokenizer stParam(i, "=", true);
        while (stParam.hasNext()) {
            parsedParameters.push_back(stParam.next());
        }
        // Check that parsed parameters are exactly two and contains valid chracters
        if (parsedParameters.size() == 2 && SUMOXMLDefinitions::isValidGenericParameterKey(parsedParameters.front()) && SUMOXMLDefinitions::isValidGenericParameterValue(parsedParameters.back())) {
            myNBNode.setParameter(parsedParameters.front(), parsedParameters.back());
        }
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
        case SUMO_ATTR_ID: {
            myNet->renameJunction(this, value);
            break;
        }
        case SUMO_ATTR_TYPE: {
            SumoXMLNodeType type = SUMOXMLDefinitions::NodeTypes.get(value);
            if (myNBNode.getType() == NODETYPE_PRIORITY && type == NODETYPE_RIGHT_BEFORE_LEFT) {
                myNet->getNetBuilder()->getEdgeCont().removeRoundabout(&myNBNode);
            }
            myNBNode.reinit(myNBNode.getPosition(), type);
            break;
        }
        case SUMO_ATTR_POSITION: {
            // start geometry moving (because new position affect all junction children)
            startGeometryMoving();
            // set new position in NBNode without updating grid
            moveJunctionGeometry(parse<Position>(value));
            // end geometry moving
            endGeometryMoving();
            // mark this connections and all of the junction's Neighbours as deprecated
            markConnectionsDeprecated(true);
            break;
        }
        case GNE_ATTR_MODIFICATION_STATUS:
            if (myLogicStatus == FEATURE_GUESSED && value != FEATURE_GUESSED) {
                // clear guessed connections. previous connections will be restored
                myNBNode.invalidateIncomingConnections();
                // Clear GNEConnections of incoming edges
                for (auto i : myGNEIncomingEdges) {
                    i->clearGNEConnections();
                }
            }
            myLogicStatus = value;
            break;
        case SUMO_ATTR_SHAPE: {
            // start geometry moving (because new position affect all junction children)
            startGeometryMoving();
            // set new shape (without updating grid)
            const PositionVector shape = parse<PositionVector>(value);
            myNBNode.setCustomShape(shape);
            // end geometry moving
            endGeometryMoving();
            // mark this connections and all of the junction's Neighbours as deprecated
            markConnectionsDeprecated(true);
            break;
        }
        case SUMO_ATTR_RADIUS: {
            myNBNode.setRadius(parse<double>(value));
            break;
        }
        case SUMO_ATTR_TLTYPE: {
            const std::set<NBTrafficLightDefinition*> copyOfTls = myNBNode.getControllingTLS();
            for (auto it : copyOfTls) {
                it->setType(SUMOXMLDefinitions::TrafficLightTypes.get(value));
            }
            break;
        }
        case SUMO_ATTR_KEEP_CLEAR: {
            myNBNode.setKeepClear(parse<bool>(value));
            break;
        }
        case SUMO_ATTR_RIGHT_OF_WAY:
            myNBNode.setRightOfWay(SUMOXMLDefinitions::RightOfWayValues.get(value));
            break;
        case SUMO_ATTR_FRINGE:
            myNBNode.setFringeType(SUMOXMLDefinitions::FringeTypeValues.get(value));
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEJunction::getColorValue(const GUIVisualizationSettings& /* s */, int activeScheme) const {
    switch (activeScheme) {
        case 0:
            // ensure visibility of red connections
            if (!(myNet->getViewNet()->getEditModes().networkEditMode == GNE_NMODE_TLS && myNBNode.isTLControlled())) {
                return 1;
            } else {
                return 0;
            }
        case 1:
            return isAttributeCarrierSelected();
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
                    return 8; // may happen before first network computation
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
        case 3:
            return myNBNode.getPosition().z();
        default:
            assert(false);
            return 0;
    }
}


void
GNEJunction::moveJunctionGeometry(const Position& pos) {
    // obtain NBNode position
    const Position orig = myNBNode.getPosition();
    // reinit NBNode
    myNBNode.reinit(pos, myNBNode.getType());
    // set new position of adjacent edges
    for (auto i : getNBNode()->getEdges()) {
        myNet->retrieveEdge(i->getID())->updateJunctionPosition(this, orig);
    }
    // declare three sets with all affected GNEJunctions, GNEEdges and GNEConnections
    std::set<GNEJunction*> affectedJunctions;
    std::set<GNEEdge*> affectedEdges;
    // Iterate over GNEEdges
    for (auto i : myGNEEdges) {
        // Add source and destiny junctions
        affectedJunctions.insert(i->getGNEJunctionSource());
        affectedJunctions.insert(i->getGNEJunctionDestiny());
        // Obtain neighbors of Junction source
        for (auto j : i->getGNEJunctionSource()->getGNEEdges()) {
            affectedEdges.insert(j);
        }
        // Obtain neighbors of Junction destiny
        for (auto j : i->getGNEJunctionDestiny()->getGNEEdges()) {
            affectedEdges.insert(j);
        }
    }
    // Iterate over affected Edges
    for (auto i : affectedEdges) {
        // Update edge geometry
        i->updateGeometry();
    }
}


RGBColor
GNEJunction::setColor(const GUIVisualizationSettings& s, bool bubble) const {
    const int scheme = s.junctionColorer.getActive();
    RGBColor color = s.junctionColorer.getScheme().getColor(getColorValue(s, scheme));
    if (!bubble && scheme == 0) {
        color = s.junctionColorer.getScheme().getColor(0.);
    }
    // override with special colors (unless the color scheme is based on selection)
    if (drawUsingSelectColor() && scheme != 1) {
        color = s.colorSettings.selectionColor;
    }
    if (myAmCreateEdgeSource) {
        color = RGBColor(0, 255, 0);
    }
    GLHelper::setColor(color);
    return color;
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
