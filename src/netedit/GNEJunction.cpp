/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utility>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netbuild/NBAlgorithms.h>
#include <utils/common/MsgHandler.h>

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

const double GNEJunction::BUBBLE_RADIUS(4);

// ===========================================================================
// method definitions
// ===========================================================================
GNEJunction::GNEJunction(NBNode& nbn, GNENet* net, bool loaded) :
    GNENetElement(net, nbn.getID(), GLO_JUNCTION, SUMO_TAG_JUNCTION, ICON_JUNCTION),
    myNBNode(nbn),
    myAmCreateEdgeSource(false),
    myLogicStatus(loaded ? LOADED : GUESSED),
    myAmResponsible(false),
    myHasValidLogic(loaded),
    myAmTLSSelected(false) {
    updateGeometry();
}


GNEJunction::~GNEJunction() {
    // delete all GNECrossing
    for (auto it : myGNECrossings) {
        it->decRef();
        if (it->unreferenced()) {
            // show extra information for tests
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Deleting unreferenced " + toString(it->getTag()) + " '" + it->getID() + "' in GNEJunction destructor");
            }
            delete it;
        }
    }

    if (myAmResponsible) {
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Deleting NBNode of '" + getID() + "' in GNEJunction destructor");
        }
        delete &myNBNode;
    }
}


void
GNEJunction::updateGeometry() {
    // calculate boundary using EXTENT as size
    const double EXTENT = 2;
    myBoundary = Boundary(myNBNode.getPosition().x() - EXTENT, myNBNode.getPosition().y() - EXTENT,
                          myNBNode.getPosition().x() + EXTENT, myNBNode.getPosition().y() + EXTENT);
    // if junctio own a extra shape, add it to boundary
    if (myNBNode.getShape().size() > 0) {
        myBoundary.add(myNBNode.getShape().getBoxBoundary());
    }
    myMaxSize = MAX2(myBoundary.getWidth(), myBoundary.getHeight());
    // rebuild GNECrossings
    // (but don't rebuild the crossings in NBNode because they are already finished)
    rebuildGNECrossings(false);
}


void
GNEJunction::rebuildGNECrossings(bool rebuildNBNodeCrossings) {
    // rebuild GNECrossings only if create crossings and walkingAreas in net is enabled
    if (myNet->getNetBuilder()->haveNetworkCrossings()) {
        if (rebuildNBNodeCrossings) {
            // build new NBNode::Crossings and walking areas
            myNBNode.buildCrossingsAndWalkingAreas();
        }
        // create a vector to keep retrieved and created crossings
        std::vector<GNECrossing*> retrievedCrossings;
        // iterate over NBNode::Crossings of GNEJunction
        for (auto c : myNBNode.getCrossingsIncludingInvalid()) {
            // retrieve existent GNECrossing, or create it
            GNECrossing* retrievedGNECrossing = retrieveGNECrossing(c);
            retrievedCrossings.push_back(retrievedGNECrossing);
            // check if previously this GNECrossings exists, and if true, remove it from myGNECrossings
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
        // delete non retrieved GNECrossings
        for (auto it : myGNECrossings) {
            it->decRef();
            if (it->unreferenced()) {
                // show extra information for tests
                if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                    WRITE_WARNING("Deleting unreferenced " + toString(it->getTag()) + " '" + it->getID() + "' in rebuildGNECrossings()");
                }
                delete it;
            }
        }
        // copy retrieved (existent and created) GNECrossigns to myGNECrossings
        myGNECrossings = retrievedCrossings;
    }
}


GUIGLObjectPopupMenu*
GNEJunction::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    // build selection and show parameters menu
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    //if (parent.getVisualisationSettings()->editMode != GNE_MODE_CONNECT) {
    //    // XXX if joinable
    //    new FXMenuCommand(ret, "Join adjacent edges", 0, &parent, MID_GNE_JOIN_EDGES);
    //}
    // create menu commands
    FXMenuCommand* mcCustomShape = new FXMenuCommand(ret, "Set custom junction shape", 0, &parent, MID_GNE_JUNCTION_EDIT_SHAPE);
    FXMenuCommand* mcReplace = new FXMenuCommand(ret, "Replace junction by geometry point", 0, &parent, MID_GNE_JUNCTION_REPLACE);
    FXMenuCommand* mcClearConnections = new FXMenuCommand(ret, "Clear connections", 0, &parent, MID_GNE_JUNCTION_CLEAR_CONNECTIONS);
    FXMenuCommand* mcResetConnections = new FXMenuCommand(ret, "Reset connections", 0, &parent, MID_GNE_JUNCTION_RESET_CONNECTIONS);
    // check if menu commands has to be disabled
    EditMode editMode = myNet->getViewNet()->getCurrentEditMode();
    const bool wrongMode = (editMode == GNE_MODE_CONNECT || editMode == GNE_MODE_TLS || editMode == GNE_MODE_CREATE_EDGE);
    if (wrongMode) {
        mcCustomShape->disable();
        mcClearConnections->disable();
        mcResetConnections->disable();
    }
    // disable mcClearConnections if juction hasn't connections
    if (getGNEConnections().empty()) {
        mcClearConnections->disable();
    }
    // checkIsRemovable requiers turnarounds to be computed. This is ugly
    if (myNBNode.getIncomingEdges().size() == 2 && myNBNode.getOutgoingEdges().size() == 2) {
        NBTurningDirectionsComputer::computeTurnDirectionsForNode(&myNBNode, false);
    }
    std::string reason = "wrong edit mode";
    if (wrongMode || !myNBNode.checkIsRemovableReporting(reason)) {
        mcReplace->setText(mcReplace->getText() + " (" + reason.c_str() + ")");
        mcReplace->disable();
    }
    return ret;
}


Boundary
GNEJunction::getCenteringBoundary() const {
    Boundary b = myBoundary;
    b.grow(20);
    return b;
}


void
GNEJunction::drawGL(const GUIVisualizationSettings& s) const {
    // declare variables
    GLfloat color[4];
    double exaggeration = gSelected.isSelected(getType(), getGlID()) ? s.selectionScale : 1;
    exaggeration *= s.junctionSize.getExaggeration(s);
    // push name
    glPushName(getGlID());
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
                glGetFloatv(GL_CURRENT_COLOR, color);
                if (color[3] != 0) {
                    glPushMatrix();
                    Position pos = myNBNode.getPosition();
                    glTranslated(pos.x(), pos.y(), getType() + 0.05);
                    GLHelper::drawFilledCircle(BUBBLE_RADIUS * exaggeration, 32);
                    glPopMatrix();
                }
            }
        }
        if (drawBubble) {
            setColor(s, true);
            // recognize full transparency and simply don't draw
            glGetFloatv(GL_CURRENT_COLOR, color);
            if (color[3] != 0) {
                glPushMatrix();
                Position pos = myNBNode.getPosition();
                glTranslated(pos.x(), pos.y(), getType() - 0.05);
                // resolution of drawn circle depending of the zoom (To improve smothness)
                if (s.scale >= 10) {
                    GLHelper::drawFilledCircle(BUBBLE_RADIUS * exaggeration, 32);
                } else if (s.scale >= 2) {
                    GLHelper::drawFilledCircle(BUBBLE_RADIUS * exaggeration, 16);
                } else if (s.scale >= 1) {
                    GLHelper::drawFilledCircle(BUBBLE_RADIUS * exaggeration, 8);
                } else {
                    GLHelper::drawFilledCircle(BUBBLE_RADIUS * exaggeration, 4);
                }
                glPopMatrix();
            }
        }

        if (s.editMode == GNE_MODE_TLS && myNBNode.isTLControlled() && !myAmTLSSelected) {
            glPushMatrix();
            Position pos = myNBNode.getPosition();
            glTranslated(pos.x(), pos.y(), getType() + 0.1);
            glColor3d(1, 1, 1);
            const double halfWidth = 32 / s.scale;
            const double halfHeight = 64 / s.scale;
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_TLS), -halfWidth, -halfHeight, halfWidth, halfHeight);
            glPopMatrix();
        }
        // draw crossings
        if (s.editMode != GNE_MODE_TLS) {
            for (auto it : myGNECrossings) {
                it->drawGL(s);
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


Position
GNEJunction::getPositionInView() const {
    return myNBNode.getPosition();
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
        Position newPosition = oldPos;
        newPosition.add(offset);
        moveJunctionGeometry(newPosition);
    }
}


void
GNEJunction::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
    if (isValid(SUMO_ATTR_POSITION, toString(myNBNode.getPosition()))) {
        undoList->p_begin("position of " + toString(getTag()));
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myNBNode.getPosition()), true, toString(oldPos)));
        undoList->p_end();
    } else {
        // tried to set an invalid position, revert back to the previous one
        moveJunctionGeometry(oldPos);
    }
}


void
GNEJunction::updateShapesAndGeometries() {
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
    for (auto i : affectedJunctions) {
        // Update geometry of Junction
        i->updateGeometry();
    }
    // Iterate over affected Edges
    for (auto i : affectedEdges) {
        // Update edge geometry
        i->updateGeometry();
    }
    // Finally update geometry of this junction
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
            GNEEdge* srcEdge = myNet->retrieveEdge((*it)->getID());
            removeConnectionsFrom(srcEdge, undoList, false); // false, because the whole tls will be invalidated at the end
            undoList->add(new GNEChange_Attribute(srcEdge, GNE_ATTR_MODIFICATION_STATUS, status), true);
        }
        undoList->add(new GNEChange_Attribute(this, GNE_ATTR_MODIFICATION_STATUS, status), true);
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
        if (tlDef != 0) {
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
        if (tlDef != 0) {
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
        undoList->add(new GNEChange_Attribute(srcEdge, GNE_ATTR_MODIFICATION_STATUS, MODIFIED), true);
    }
}


void
GNEJunction::invalidateTLS(GNEUndoList* undoList, const NBConnection& deletedConnection) {
    assert(undoList->hasCommandGroup());
    // NBLoadedSUMOTLDef becomes invalid, replace with NBOwnTLDef which will be dynamically recomputed
    const std::set<NBTrafficLightDefinition*> coypOfTls = myNBNode.getControllingTLS(); // make a copy!
    for (auto it : coypOfTls) {
        NBLoadedSUMOTLDef* tlDef = dynamic_cast<NBLoadedSUMOTLDef*>(it);
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
    for (auto c : copyOfGNECrossings) {
        // obtain the set of edges vinculated with the crossing (due it works as ID)
        EdgeSet edgeSet(c->getNBCrossing()->edges.begin(), c->getNBCrossing()->edges.end());
        // If this edge is part of the set of edges of crossing
        if (edgeSet.count(edge->getNBEdge()) == 1) {
            // delete crossing if this is their last edge
            if ((c->getNBCrossing()->edges.size() == 1) && (c->getNBCrossing()->edges.front() == edge->getNBEdge())) {
                myNet->deleteCrossing(c, undoList);
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
    for (auto i : myGNECrossings) {
        if (i->getNBCrossing() == crossing) {
            return i;
        }
    }
    if (createIfNoExist) {
        // create new GNECrossing
        GNECrossing* createdGNECrossing = new GNECrossing(this, crossing);
        // show extra information for tests
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Created " + toString(createdGNECrossing->getTag()) + " '" + createdGNECrossing->getID() + "' in retrieveGNECrossing()");
        }
        return createdGNECrossing;
    } else {
        return NULL;
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
                    undoList->add(new GNEChange_TLS(this, 0, true), true);
                }
            } else if (getNBNode()->isTLControlled()) {
                // delete old traffic light
                // make a copy because we will modify the original
                const std::set<NBTrafficLightDefinition*> copyOfTls = myNBNode.getControllingTLS();
                for (auto it : copyOfTls) {
                    undoList->add(new GNEChange_TLS(this, it, false, OptionsCont::getOptions().getBool("gui-testing-debug")), true);
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
            const std::set<NBTrafficLightDefinition*> copyOfTls = myNBNode.getControllingTLS();
            for (auto it : copyOfTls) {
                undoList->add(new GNEChange_TLS(this, it, false), true);
            }
            NBTrafficLightLogicCont& tlCont = myNet->getTLLogicCont();
            const std::map<std::string, NBTrafficLightDefinition*> programs = tlCont.getPrograms(value);
            if (programs.size() > 0) {
                // add to existing tls definitions
                for (auto it : programs) {
                    NBTrafficLightDefinition* oldTLS = it.second;
                    if (dynamic_cast<NBOwnTLDef*>(oldTLS) != 0) {
                        undoList->add(new GNEChange_TLS(this, oldTLS, true), true);
                    } else {
                        // delete and re-create the definition because the loaded phases are now invalid
                        const std::vector<NBNode*> copyOfNodes = oldTLS->getNodes();
                        for (auto it_node : copyOfNodes) {
                            GNEJunction* oldJunction = myNet->retrieveJunction(it_node->getID());
                            undoList->add(new GNEChange_TLS(oldJunction, oldTLS, false), true);
                        }
                        undoList->add(new GNEChange_TLS(this, 0, true, false, value), true);
                        NBTrafficLightDefinition* newTLS = *myNBNode.getControllingTLS().begin();
                        // re-add existing nodes
                        for (auto it_node : copyOfNodes) {
                            GNEJunction* oldJunction = myNet->retrieveJunction(it_node->getID());
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
            return isValidID(value) && (myNet->retrieveJunction(value, false) == 0);
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::NodeTypes.hasString(value);
        case SUMO_ATTR_POSITION: {
            bool ok;
            return GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false).size() == 1;
        }
        case SUMO_ATTR_SHAPE: {
            bool ok = true;
            PositionVector shape = GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, true);
            return ok;
        }
        case SUMO_ATTR_RADIUS:
            return canParse<double>(value);
        case SUMO_ATTR_TLTYPE:
            return myNBNode.isTLControlled() && SUMOXMLDefinitions::TrafficLightTypes.hasString(value);
        case SUMO_ATTR_TLID:
            return myNBNode.isTLControlled() && (value != "");
        case SUMO_ATTR_KEEP_CLEAR:
            return canParse<bool>(value);
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
        case SUMO_ATTR_ID: {
            myNet->renameJunction(this, value);
            break;
        }
        case SUMO_ATTR_TYPE: {
            myNBNode.reinit(myNBNode.getPosition(), SUMOXMLDefinitions::NodeTypes.get(value));
            break;
        }
        case SUMO_ATTR_POSITION: {
            // set new position in NBNode
            bool ok;
            moveJunctionGeometry(GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, false)[0]);
            // Refresh element to avoid grabbing problems
            myNet->refreshElement(this);
            break;
        }
        case GNE_ATTR_MODIFICATION_STATUS:
            if (myLogicStatus == GUESSED && value != GUESSED) {
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
            bool ok;
            const PositionVector shape = GeomConvHelper::parseShapeReporting(value, "netedit-given", 0, ok, true);
            myNBNode.setCustomShape(shape);
            // Refresh element to avoid grabbing problems
            myNet->refreshElement(this);
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
            myNBNode.setKeepClear(value == "true");
            break;
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
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
        case 3:
            return myNBNode.getPosition().z();
        default:
            assert(false);
            return 0;
    }
}


void
GNEJunction::moveJunctionGeometry(const Position& pos) {
    const Position orig = myNBNode.getPosition();
    myNBNode.reinit(pos, myNBNode.getType());
    // set new position of adjacent edges
    for (auto i : getNBNode()->getEdges()) {
        myNet->retrieveEdge(i->getID())->updateJunctionPosition(this, orig);
    }
    // Update shapes without include connections, because the aren't showed in Move mode
    updateShapesAndGeometries();
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
