/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEConnection.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
// A class for visualizing connections between lanes
/****************************************************************************/

#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_TLS.h>
#include <netedit/elements/moving/GNEMoveElementConnection.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>

#include "GNEConnection.h"
#include "GNEInternalLane.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEConnection::GNEConnection(GNELane* from, GNELane* to) :
    GNENetworkElement(from->getNet(), "from" + from->getID() + "to" + to->getID(), SUMO_TAG_CONNECTION),
    myMoveElementConnection(new GNEMoveElementConnection(this)),
    myLinkState(LINKSTATE_TL_OFF_NOSIGNAL),
    mySpecialColor(nullptr),
    myShapeDeprecated(true) {
    // set parents
    setParents<GNELane*>({from, to});
    setParents<GNEEdge*>({from->getParentEdge(), to->getParentEdge()});
}


GNEConnection::~GNEConnection() {
}


GNEMoveElement*
GNEConnection::getMoveElement() const {
    return myMoveElementConnection;
}


Parameterised*
GNEConnection::getParameters() {
    return &getNBEdgeConnection();
}


const Parameterised*
GNEConnection::getParameters() const {
    return &getNBEdgeConnection();

}


const PositionVector&
GNEConnection::getConnectionShape() const {
    if (myConnectionGeometry.getShape().size() > 0) {
        return myConnectionGeometry.getShape();
    } else {
        return getNBEdgeConnection().customShape;
    }
}


void
GNEConnection::updateGeometry() {
    // check if adjust shape
    if (myShapeDeprecated && existNBEdgeConnection()) {
        // Get shape of from and to lanes
        const NBEdge::Connection& nbCon = getNBEdgeConnection();
        // obtain lane shapes
        PositionVector laneShapeFrom = getParentLanes().front()->getLaneShape();
        PositionVector laneShapeTo = getParentLanes().back()->getLaneShape();
        // Calculate shape of connection depending of the size of Junction shape
        if (nbCon.customShape.size() > 0) {
            myConnectionGeometry.updateGeometry(nbCon.customShape);
        } else if (nbCon.shape.size() > 1) {
            PositionVector connectionShape;
            if ((nbCon.shape.length() < 3) && !nbCon.haveVia) {
                // apply offset to lane shape if we're in lane spread function center
                if (getParentLanes().front()->getParentEdge()->getNBEdge()->getLaneSpreadFunction() == LaneSpreadFunction::CENTER) {
                    laneShapeFrom.move2side(0.3);
                }
                if (getParentLanes().back()->getParentEdge()->getNBEdge()->getLaneSpreadFunction() == LaneSpreadFunction::CENTER) {
                    laneShapeTo.move2side(0.3);
                }
                // check if this connetion is a turn around
                bool turnAround = false;
                const auto fromOppositeEdges = getParentLanes().front()->getParentEdge()->getOppositeEdges();
                for (const auto& edge : fromOppositeEdges) {
                    if (edge == getParentLanes().back()->getParentEdge()) {
                        turnAround = true;
                        break;
                    }
                }
                // add from lane shape one step before
                if (laneShapeFrom.length() > 1) {
                    // set length depending of turn arounds
                    if (turnAround) {
                        connectionShape.push_back(laneShapeFrom.positionAtOffset(laneShapeFrom.length() - 0.5));
                    } else {
                        connectionShape.push_back(laneShapeFrom.positionAtOffset(laneShapeFrom.length() - 1));
                    }
                }
                // add from lane shape
                connectionShape.push_back(laneShapeFrom.back());
                // add to lane shape
                connectionShape.push_back(laneShapeTo.front());
                // add to lane shape one step after
                if (laneShapeTo.length() > 1) {
                    // set length depending of turn arounds
                    if (turnAround) {
                        connectionShape.push_back(laneShapeTo.positionAtOffset(0.5));
                    } else {
                        connectionShape.push_back(laneShapeTo.positionAtOffset(1));
                    }
                }
            } else {
                connectionShape = nbCon.shape;
                // only append via shape if it exists
                if (nbCon.haveVia) {
                    connectionShape.append(nbCon.viaShape);
                }
            }
            myConnectionGeometry.updateGeometry(connectionShape);
        } else if (getParentLanes().front()->getLane2laneConnections().exist(getParentLanes().back())) {
            myConnectionGeometry = getParentLanes().front()->getLane2laneConnections().getLane2laneGeometry(getParentLanes().back());
        } else {
            myConnectionGeometry.clearGeometry();
        }
        // check if internal junction marker must be calculated
        if (nbCon.haveVia && (nbCon.shape.size() > 0)) {
            // create marker for internal junction waiting position (contPos)
            const double orthoLength = 0.5;
            PositionVector internalJunctionMarker = nbCon.shape.getOrthogonal(nbCon.shape.back(), 10, true, 0.1);
            if (internalJunctionMarker.length() < orthoLength) {
                internalJunctionMarker.extrapolate(orthoLength - internalJunctionMarker.length());
            }
            myInternalJunctionMarkerGeometry.updateGeometry(internalJunctionMarker);
        } else {
            myInternalJunctionMarkerGeometry.clearGeometry();
        }
        // mark connection as non-deprecated
        myShapeDeprecated = false;
    }
}


Position
GNEConnection::getPositionInView() const {
    // currently unused
    return Position(0, 0);
}


bool
GNEConnection::checkDrawFromContour() const {
    return false;
}


bool
GNEConnection::checkDrawToContour() const {
    return false;
}


bool
GNEConnection::checkDrawRelatedContour() const {
    // check opened popup
    if (myNet->getViewNet()->getPopup()) {
        return myNet->getViewNet()->getPopup()->getGLObject() == this;
    }
    return false;
}


bool
GNEConnection::checkDrawOverContour() const {
    return false;
}


bool
GNEConnection::checkDrawDeleteContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in delete mode
    if (editModes.isCurrentSupermodeNetwork() && (editModes.networkEditMode == NetworkEditMode::NETWORK_DELETE)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNEConnection::checkDrawDeleteContourSmall() const {
    return false;
}


bool
GNEConnection::checkDrawSelectContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in select mode
    if (editModes.isCurrentSupermodeNetwork() && (editModes.networkEditMode == NetworkEditMode::NETWORK_SELECT)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNEConnection::checkDrawMoveContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in move mode
    if (!myNet->getViewNet()->isCurrentlyMovingElements() && editModes.isCurrentSupermodeNetwork() &&
            (editModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) && myNet->getViewNet()->checkOverLockedElement(this, mySelected)) {
        // check if we're editing this network element
        const GNENetworkElement* editedNetworkElement = myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement();
        if (editedNetworkElement) {
            return editedNetworkElement == this;
        } else {
            // only move the first element
            return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
        }
    } else {
        return false;
    }
}


GNEEdge*
GNEConnection::getEdgeFrom() const {
    return getParentEdges().front();
}


GNEEdge*
GNEConnection::getEdgeTo() const {
    return getParentEdges().back();
}


GNELane*
GNEConnection::getLaneFrom() const {
    return getParentLanes().front();
}


GNELane*
GNEConnection::getLaneTo() const {
    return getParentLanes().back();
}


int
GNEConnection::getFromLaneIndex() const {
    return getParentLanes().front()->getIndex();
}


int
GNEConnection::getToLaneIndex() const {
    return getParentLanes().back()->getIndex();
}


NBEdge::Connection&
GNEConnection::getNBEdgeConnection() const {
    return getParentEdges().front()->getNBEdge()->getConnectionRef(getFromLaneIndex(), getParentEdges().back()->getNBEdge(), getToLaneIndex());
}


NBConnection
GNEConnection::getNBConnection() const {
    const NBEdge::Connection& c = getNBEdgeConnection();
    return NBConnection(getParentEdges().front()->getNBEdge(), getFromLaneIndex(),
                        getParentEdges().back()->getNBEdge(), getToLaneIndex(),
                        (int)c.tlLinkIndex, (int)c.tlLinkIndex2);
}


void
GNEConnection::updateConnectionID() {
    setNetworkElementID(getParentLanes().front()->getID() + " -> " + getParentLanes().back()->getID());
}


LinkState
GNEConnection::getLinkState() const {
    return myLinkState;
}


void
GNEConnection::markConnectionGeometryDeprecated() {
    myShapeDeprecated = true;
}


void
GNEConnection::updateLinkState() {
    const NBEdge::Connection& nbCon = getNBEdgeConnection();
    myLinkState = getParentEdges().front()->getNBEdge()->getToNode()->getLinkState(getParentEdges().front()->getNBEdge(),
                  nbCon.toEdge,
                  nbCon.fromLane,
                  nbCon.toLane,
                  nbCon.mayDefinitelyPass,
                  nbCon.tlID);
}


void
GNEConnection::smootShape() {
    auto shape = getConnectionShape();
    shape = shape.bezier(5);
    setAttribute(SUMO_ATTR_CUSTOMSHAPE, toString(shape), myNet->getViewNet()->getUndoList());
}


GUIGLObjectPopupMenu*
GNEConnection::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    if (myShapeEdited) {
        return getShapeEditedPopUpMenu(app, parent, getNBEdgeConnection().customShape);
    } else {
        // create popup
        GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, this);
        // build common options
        buildPopUpMenuCommonOptions(ret, app, myNet->getViewNet(), myTagProperty->getTag(), mySelected);
        // check if we're in supermode network
        if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
            // create menu commands
            FXMenuCommand* mcCustomShape = GUIDesigns::buildFXMenuCommand(ret, "Set custom connection shape", nullptr, &parent, MID_GNE_CONNECTION_EDIT_SHAPE);
            GUIDesigns::buildFXMenuCommand(ret, "Smooth connection shape", nullptr, &parent, MID_GNE_CONNECTION_SMOOTH_SHAPE);
            // check if menu commands has to be disabled
            NetworkEditMode editMode = myNet->getViewNet()->getEditModes().networkEditMode;
            // check if we're in the correct edit mode
            if ((editMode == NetworkEditMode::NETWORK_CONNECT) || (editMode == NetworkEditMode::NETWORK_TLS) || (editMode == NetworkEditMode::NETWORK_CREATE_EDGE)) {
                mcCustomShape->disable();
            }
        }
        return ret;
    }
}


double
GNEConnection::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GNEConnection::getCenteringBoundary() const {
    return myNetworkElementContour.getContourBoundary();
}


void
GNEConnection::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
}


void
GNEConnection::drawGL(const GUIVisualizationSettings& s) const {
    // Check if connection must be drawed
    if (checkDrawConnection()) {
        // get connection exaggeration
        const double connectionExaggeration = isAttributeCarrierSelected() ? s.selectorFrameScale : 1;
        // get detail level
        const auto d = s.getDetailLevel(connectionExaggeration);
        // check if draw shape superposed (used in train lanes)
        PositionVector shapeSuperposed = myConnectionGeometry.getShape();
        if (getParentLanes().front()->getDrawingConstants()->drawSuperposed()) {
            shapeSuperposed.move2side(0.5);
        }
        GUIGeometry superposedGeometry(shapeSuperposed);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (!s.drawForViewObjectsHandler) {
            // draw connection
            drawConnection(s, d, superposedGeometry, connectionExaggeration);
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), getPositionInView(), 0.1);
            // draw dotted contour depending if we're editing the custom shape
            const GNENetworkElement* editedNetworkElement = myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement();
            if (editedNetworkElement && (editedNetworkElement == this)) {
                // draw dotted contour geometry points
                myNetworkElementContour.drawDottedContourGeometryPoints(s, d, this, shapeSuperposed, s.neteditSizeSettings.connectionGeometryPointRadius,
                        connectionExaggeration, s.dottedContourSettings.segmentWidthSmall);
            } else {
                // draw dotted contour
                myNetworkElementContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            }
        }
        // calculate contour
        calculateConnectionContour(s, d, shapeSuperposed, connectionExaggeration);
    }
}


void
GNEConnection::deleteGLObject() {
    myNet->deleteNetworkElement(this, myNet->getViewNet()->getUndoList());
}


void
GNEConnection::updateGLObject() {
    updateGeometry();
}


void
GNEConnection::setSpecialColor(const RGBColor* color) {
    mySpecialColor = color;
}


std::string
GNEConnection::getAttribute(SumoXMLAttr key) const {
    // first get attributes in which nbConnection reference can be invalid
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_FROM:
            return getParentLanes().front()->getParentEdge()->getID();
        case SUMO_ATTR_TO:
            return getParentLanes().back()->getParentEdge()->getID();
        case SUMO_ATTR_FROM_LANE:
            return getParentLanes().front()->getAttribute(SUMO_ATTR_INDEX);
        case GNE_ATTR_FROM_LANEID:
            return getParentLanes().front()->getID();
        case SUMO_ATTR_TO_LANE:
            return getParentLanes().back()->getAttribute(SUMO_ATTR_INDEX);
        case GNE_ATTR_TO_LANEID:
            return getParentLanes().back()->getID();
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_FRONTELEMENT:
            return getCommonAttribute(key);
        case GNE_ATTR_PARENT:
            return getParentEdges().front()->getToJunction()->getID();
        default:
            break;
    }
    // now continue with attributes that needs a nbConnection reference
    const NBEdge::Connection& nbCon = getNBEdgeConnection();
    switch (key) {
        case SUMO_ATTR_PASS:
            return toString(nbCon.mayDefinitelyPass);
        case SUMO_ATTR_INDIRECT:
            return toString(nbCon.indirectLeft);
        case SUMO_ATTR_TYPE:
            return toString(nbCon.edgeType);
        case SUMO_ATTR_KEEP_CLEAR:
            return toString(nbCon.keepClear);
        case SUMO_ATTR_CONTPOS:
            return toString(nbCon.contPos);
        case SUMO_ATTR_UNCONTROLLED:
            return toString(nbCon.uncontrolled);
        case SUMO_ATTR_VISIBILITY_DISTANCE:
            return toString(nbCon.visibility);
        case SUMO_ATTR_TLLINKINDEX:
            return toString(nbCon.tlLinkIndex);
        case SUMO_ATTR_TLLINKINDEX2:
            return toString(nbCon.tlLinkIndex2);
        case SUMO_ATTR_ALLOW:
            if (nbCon.permissions == SVC_UNSPECIFIED) {
                return getVehicleClassNames(nbCon.toEdge->getLanes()[nbCon.toLane].permissions);
            } else {
                return getVehicleClassNames(nbCon.permissions);
            }
        case SUMO_ATTR_DISALLOW:
            if (nbCon.permissions == SVC_UNSPECIFIED) {
                return getVehicleClassNames(invertPermissions(nbCon.toEdge->getLanes()[nbCon.toLane].permissions));
            } else {
                return getVehicleClassNames(invertPermissions(nbCon.permissions));
            }
        case SUMO_ATTR_CHANGE_LEFT:
            if (nbCon.changeLeft == SVC_UNSPECIFIED) {
                return "all";
            } else {
                return getVehicleClassNames(nbCon.changeLeft);
            }
        case SUMO_ATTR_CHANGE_RIGHT:
            if (nbCon.changeRight == SVC_UNSPECIFIED) {
                return "all";
            } else {
                return getVehicleClassNames(nbCon.changeRight);
            }
        case SUMO_ATTR_SPEED:
            if (nbCon.speed == NBEdge::UNSPECIFIED_SPEED) {
                return "default";
            } else {
                return toString(nbCon.speed);
            }
        case SUMO_ATTR_LENGTH:
            return toString(nbCon.customLength);
        case SUMO_ATTR_DIR:
            return toString(getParentEdges().front()->getNBEdge()->getToNode()->getDirection(
                                getParentEdges().front()->getNBEdge(), nbCon.toEdge, OptionsCont::getOptions().getBool("lefthand")));
        case SUMO_ATTR_STATE:
            return toString(getParentEdges().front()->getNBEdge()->getToNode()->getLinkState(
                                getParentEdges().front()->getNBEdge(), nbCon.toEdge, nbCon.fromLane, nbCon.toLane, nbCon.mayDefinitelyPass, nbCon.tlID));
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CUSTOMSHAPE:
            return toString(nbCon.customShape);
        default:
            return getCommonAttribute(key);
    }
}


double
GNEConnection::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


Position
GNEConnection::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNEConnection::getAttributePositionVector(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CUSTOMSHAPE:
            return getNBEdgeConnection().customShape;
        default:
            return getCommonAttributePositionVector(key);
    }
}


void
GNEConnection::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    const NBEdge::Connection& c = getNBEdgeConnection();
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_FROM_LANE:
        case SUMO_ATTR_TO_LANE:
        case SUMO_ATTR_PASS:
        case SUMO_ATTR_KEEP_CLEAR:
        case SUMO_ATTR_CONTPOS:
        case SUMO_ATTR_VISIBILITY_DISTANCE:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
        case SUMO_ATTR_CHANGE_LEFT:
        case SUMO_ATTR_CHANGE_RIGHT:
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CUSTOMSHAPE:
        case SUMO_ATTR_TYPE:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        case SUMO_ATTR_TLLINKINDEX:
            if (isAttributeEnabled(SUMO_ATTR_TLLINKINDEX) && (value != getAttribute(key))) {
                changeTLIndex(key, parse<int>(value), c.tlLinkIndex2, undoList);
            }
            break;
        case SUMO_ATTR_TLLINKINDEX2:
            if (isAttributeEnabled(SUMO_ATTR_TLLINKINDEX) && (value != getAttribute(key))) {
                changeTLIndex(key, c.tlLinkIndex, parse<int>(value), undoList);
            }
            break;
        case SUMO_ATTR_UNCONTROLLED:
            undoList->begin(this, "change attribute controlled for connection");
            {
                const bool wasUncontrolled = c.uncontrolled;
                GNEChange_Attribute::changeAttribute(this, key, value, undoList);
                if (wasUncontrolled && !c.uncontrolled) {
                    GNEEdge* srcEdge = getParentEdges().front();
                    NBConnection newNBCon(srcEdge->getNBEdge(), c.fromLane, c.toEdge, c.toLane);
                    srcEdge->getToJunction()->invalidateTLS(undoList, NBConnection::InvalidConnection, newNBCon);
                }
            }
            undoList->end();
            break;
        case SUMO_ATTR_INDIRECT:
            undoList->begin(this, "change attribute indirect for connection");
            if (isAttributeEnabled(SUMO_ATTR_TLLINKINDEX) && (value != getAttribute(key))) {
                GNEChange_Attribute::changeAttribute(this, key, value, undoList);
                int linkIndex2 = -1;
                if (parse<bool>(value)) {
                    // find straight connection with the same toEdge
                    std::set<NBTrafficLightDefinition*> defs = getParentEdges().front()->getNBEdge()->getToNode()->getControllingTLS();
                    NBEdge* from = getParentEdges().front()->getNBEdge();
                    for (NBTrafficLightDefinition* tlDef : defs) {
                        for (const NBConnection& c2 : tlDef->getControlledLinks()) {
                            if (c2.getTo() == c.toEdge && c2.getFrom() != from) {
                                if (from->getToNode()->getDirection(c2.getFrom(), c2.getTo()) == LinkDirection::STRAIGHT) {
                                    linkIndex2 = c2.getTLIndex();
                                    break;
                                }
                            }
                        }
                    }
                }
                changeTLIndex(key, c.tlLinkIndex, linkIndex2, undoList);
            }
            undoList->end();
            break;
        case SUMO_ATTR_DIR:
            throw InvalidArgument("Attribute of '" + toString(key) + "' cannot be modified");
        case SUMO_ATTR_STATE:
            throw InvalidArgument("Attribute of '" + toString(key) + "' cannot be modified");
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


void
GNEConnection::changeTLIndex(SumoXMLAttr key, int tlIndex, int tlIndex2, GNEUndoList* undoList) {
    // trigger GNEChange_TLS
    undoList->begin(this, "change tls linkIndex for connection");
    // make a copy
    std::set<NBTrafficLightDefinition*> defs = getParentEdges().front()->getNBEdge()->getToNode()->getControllingTLS();
    for (const auto& tlDef : defs) {
        NBLoadedSUMOTLDef* sumoDef = dynamic_cast<NBLoadedSUMOTLDef*>(tlDef);
        NBTrafficLightLogic* tllogic = sumoDef ? sumoDef->getLogic() : tlDef->compute(OptionsCont::getOptions());
        if (tllogic != nullptr) {
            NBLoadedSUMOTLDef* newDef = new NBLoadedSUMOTLDef(*tlDef, *tllogic);
            newDef->addConnection(getParentEdges().front()->getNBEdge(), getParentEdges().back()->getNBEdge(),
                                  getLaneFrom()->getIndex(), getLaneTo()->getIndex(), tlIndex, tlIndex2, false);
            // make a copy
            std::vector<NBNode*> nodes = tlDef->getNodes();
            for (const auto& node : nodes) {
                GNEJunction* junction = getNet()->getAttributeCarriers()->retrieveJunction(node->getID());
                undoList->add(new GNEChange_TLS(junction, tlDef, false), true);
                undoList->add(new GNEChange_TLS(junction, newDef, true), true);
            }
        } else {
            WRITE_ERRORF(TL("Could not set attribute '%' (tls is broken)"), toString(key));
        }
    }
    undoList->end();
}


bool
GNEConnection::existNBEdgeConnection() const {
    return getParentEdges().front()->getNBEdge()->getConnectionsFromLane(getFromLaneIndex(), getParentEdges().back()->getNBEdge(), getToLaneIndex()).size() > 0;
}


bool
GNEConnection::checkDrawConnection() const {
    // declare a flag to check if shape has to be draw (by deafult false)
    bool drawConnection = false;
    // only draw connections if shape isn't deprecated
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand() && myNet->getViewNet()->getNetworkViewOptions().showConnections()) {
        drawConnection = !myShapeDeprecated;
    } else if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        if (myNet->getViewNet()->getNetworkViewOptions().showConnections() || isAttributeCarrierSelected()) {
            drawConnection = !myShapeDeprecated;
        } else {
            drawConnection = false;
        }
    } else {
        drawConnection = false;
    }
    // check if we're editing this connection
    const GNENetworkElement* editedNetworkElement = myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement();
    if (editedNetworkElement && (editedNetworkElement->getTagProperty()->getTag() == SUMO_TAG_CONNECTION)) {
        if (editedNetworkElement->getAttribute(GNE_ATTR_PARENT) == getAttribute(GNE_ATTR_PARENT)) {
            drawConnection = true;
        }
    }
    return drawConnection;
}


RGBColor
GNEConnection::getConnectionColor(const GUIVisualizationSettings& s) const {
    // check conditions
    if (myShapeEdited) {
        // return shape edit color
        return s.colorSettings.editShapeColor;
    } else if (drawUsingSelectColor()) {
        // override with special colors (unless the color scheme is based on selection)
        return s.colorSettings.selectedConnectionColor;
    } else if (mySpecialColor != nullptr) {
        // return special color
        return *mySpecialColor;
    } else {
        // Set color depending of the link state
        return GNEInternalLane::colorForLinksState(getLinkState());
    }
}


void
GNEConnection::drawConnection(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                              const GUIGeometry& superposedGeometry, const double exaggeration) const {
    // get color
    RGBColor connectionColor = getConnectionColor(s);
    // Push layer matrix
    GLHelper::pushMatrix();
    // move top if is selected
    if (mySelected) {
        glTranslated(0, 0, 0.2);
    }
    // translate to front
    if (myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement() == this) {
        drawInLayer(GLO_CONNECTION, 200);
    } else {
        drawInLayer(GLO_CONNECTION, 200);
    }
    // Set color
    GLHelper::setColor(connectionColor);
    // continue depending of detail level
    if (d <= GUIVisualizationSettings::Detail::JunctionElementDetails) {
        // draw geometry
        GLHelper::drawBoxLines(superposedGeometry.getShape(), superposedGeometry.getShapeRotations(), superposedGeometry.getShapeLengths(),
                               s.connectionSettings.connectionWidth * exaggeration);
        // draw arrows over connection
        drawConnectionArrows(s, superposedGeometry, connectionColor);
        // check if internal junction marker has to be drawn
        if (myInternalJunctionMarkerGeometry.getShape().size() > 0) {
            GLHelper::setColor(RGBColor::GREY);
            GUIGeometry::drawGeometry(d, myInternalJunctionMarkerGeometry, s.connectionSettings.connectionWidth * exaggeration * 0.5);
        }
        // draw edge values
        drawEdgeValues(s, superposedGeometry.getShape());
        // draw shape points only in Network supemode
        if (myShapeEdited && s.drawMovingGeometryPoint(1, s.neteditSizeSettings.connectionGeometryPointRadius)) {
            // draw geometry points
            GUIGeometry::drawGeometryPoints(d, superposedGeometry.getShape(), connectionColor.changedBrightness(-32),
                                            s.neteditSizeSettings.connectionGeometryPointRadius, exaggeration,
                                            myNet->getViewNet()->getNetworkViewOptions().editingElevation());
        }
    } else {
        GLHelper::drawLine(superposedGeometry.getShape());
    }
    // Pop layer matrix
    GLHelper::popMatrix();
}


void
GNEConnection::drawConnectionArrows(const GUIVisualizationSettings& s, const GUIGeometry& superposedGeometry,
                                    const RGBColor& color) const {
    if (s.showLaneDirection) {
        // Push matrix
        GLHelper::pushMatrix();
        // move front
        glTranslated(0, 0, 0.1);
        // change color
        GLHelper::setColor(color.changedBrightness(51));
        // draw triangles
        for (int i = 1; i < (int)superposedGeometry.getShape().size(); i++) {
            const auto& posA = superposedGeometry.getShape()[i - 1];
            const auto& posB = superposedGeometry.getShape()[i];
            GLHelper::drawTriangleAtEnd(posA, posB, (double) .2, (double) .1);
        }
        // Pop matrix
        GLHelper::popMatrix();
    }
}


void
GNEConnection::drawEdgeValues(const GUIVisualizationSettings& s, const PositionVector& shape) const {
    // check if edge value has to be shown
    if (s.edgeValue.show(this)) {
        const NBEdge::Connection& nbCon = getNBEdgeConnection();
        const std::string value = nbCon.getParameter(s.edgeParam, "");
        if (value != "") {
            int shapeIndex = (int)shape.size() / 2;
            const Position p = (myConnectionGeometry.getShape().size() == 2) ? (shape.front() * 0.67 + shape.back() * 0.33) : shape[shapeIndex];
            GLHelper::drawTextSettings(s.edgeValue, value, p, s.scale, 0);
        }
    }
}


void
GNEConnection::calculateConnectionContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const PositionVector& shape, const double exaggeration) const {
    // first check if junction parent was inserted with full boundary
    if (!gViewObjectsHandler.checkBoundaryParentObject(this, getType(), getParentLanes().front()->getParentEdge()->getToJunction())) {
        // calculate geometry points contour if we're editing shape
        if (myShapeEdited) {
            myNetworkElementContour.calculateContourAllGeometryPoints(s, d, this, shape, getType(), s.neteditSizeSettings.connectionGeometryPointRadius,
                    exaggeration, true);
        } else {
            // in move mode, add to selected object if this is the edited element
            const auto& editModes = myNet->getViewNet()->getEditModes();
            const bool addToSelectedObjects = (editModes.isCurrentSupermodeNetwork() && editModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) ?
                                              (myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement() == this) : true;
            // calculate connection shape contour
            myNetworkElementContour.calculateContourExtrudedShape(s, d, this, shape, getType(), s.connectionSettings.connectionWidth, exaggeration,
                    true, true, 0, nullptr, getParentLanes().front()->getParentEdge()->getToJunction(), addToSelectedObjects);
        }
    }
}


bool
GNEConnection::isValid(SumoXMLAttr key, const std::string& value) {
    // Currently ignored before implementation to avoid warnings
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_FROM_LANE:
        case SUMO_ATTR_TO_LANE:
            return false;
        case SUMO_ATTR_PASS:
            return canParse<bool>(value);
        case SUMO_ATTR_INDIRECT:
            return canParse<bool>(value);
        case SUMO_ATTR_TYPE:
            return true;
        case SUMO_ATTR_KEEP_CLEAR:
            return canParse<bool>(value);
        case SUMO_ATTR_CONTPOS:
            return canParse<double>(value) && (parse<double>(value) >= -1);
        case SUMO_ATTR_UNCONTROLLED:
            return canParse<bool>(value);
        case SUMO_ATTR_VISIBILITY_DISTANCE:
            return canParse<double>(value) && (parse<double>(value) >= -1);
        case SUMO_ATTR_TLLINKINDEX:
        case SUMO_ATTR_TLLINKINDEX2:
            if (isAttributeEnabled(SUMO_ATTR_TLLINKINDEX) &&
                    !getNBEdgeConnection().uncontrolled &&
                    (getParentEdges().front()->getNBEdge()->getToNode()->getControllingTLS().size() > 0) &&
                    canParse<int>(value) &&
                    (parse<int>(value) >= 0 || parse<int>(value) == -1)) {
                // obtain Traffic light definition
                NBTrafficLightDefinition* def = *getParentEdges().front()->getNBEdge()->getToNode()->getControllingTLS().begin();
                return def->getMaxValidIndex() >= parse<int>(value);
            } else {
                return false;
            }
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
        case SUMO_ATTR_CHANGE_LEFT:
        case SUMO_ATTR_CHANGE_RIGHT:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_SPEED:
            if (value.empty() || value == "default") {
                return true;
            } else {
                return canParse<double>(value) && ((parse<double>(value) >= 0) || (parse<double>(value) == NBEdge::UNSPECIFIED_SPEED));
            }
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value) && (parse<double>(value) >= -1);
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CUSTOMSHAPE:
            // empty custom shapes are allowed
            return canParse<PositionVector>(value);
        case SUMO_ATTR_STATE:
            return false;
        case SUMO_ATTR_DIR:
            return false;
        default:
            return isCommonAttributeValid(key, value);
    }
}


bool
GNEConnection::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_FROM_LANE:
        case SUMO_ATTR_TO_LANE:
        case SUMO_ATTR_DIR:
        case SUMO_ATTR_STATE:
            // this attributes cannot be edited
            return false;
        case SUMO_ATTR_TLLINKINDEX:
        case SUMO_ATTR_TLLINKINDEX2:
            // get Traffic Light definitions
            if (getParentEdges().front()->getNBEdge()->getToNode()->isTLControlled()) {
                NBTrafficLightDefinition* tlDef = *getParentEdges().front()->getNBEdge()->getToNode()->getControllingTLS().begin();
                NBLoadedSUMOTLDef* sumoDef = dynamic_cast<NBLoadedSUMOTLDef*>(tlDef);
                NBTrafficLightLogic* tllogic = sumoDef != nullptr ? sumoDef->getLogic() : tlDef->compute(OptionsCont::getOptions());
                if (tllogic != nullptr) {
                    return true;
                } else {
                    return false;
                }
            }
            return false;
        default:
            return true;
    }
}


bool
GNEConnection::isAttributeComputed(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_SPEED:
            return (getNBEdgeConnection().speed == NBEdge::UNSPECIFIED_SPEED);
        default:
            return false;
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEConnection::setAttribute(SumoXMLAttr key, const std::string& value) {
    if (!existNBEdgeConnection()) {
        WRITE_WARNINGF("Cannot restore attribute '%=%' for computed connection from lane '%'", toString(key), value, getParentLanes().front()->getID());
        return;
    }
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    switch (key) {
        case SUMO_ATTR_PASS:
            nbCon.mayDefinitelyPass = parse<bool>(value);
            break;
        case SUMO_ATTR_INDIRECT:
            nbCon.indirectLeft = parse<bool>(value);
            break;
        case SUMO_ATTR_KEEP_CLEAR:
            if (value == toString(KEEPCLEAR_UNSPECIFIED)) {
                nbCon.keepClear = KEEPCLEAR_UNSPECIFIED;
            } else {
                nbCon.keepClear = parse<bool>(value) ? KEEPCLEAR_TRUE : KEEPCLEAR_FALSE;
            }
            break;
        case SUMO_ATTR_UNCONTROLLED:
            nbCon.uncontrolled = parse<bool>(value);
            break;
        case SUMO_ATTR_CONTPOS:
            nbCon.contPos = parse<double>(value);
            break;
        case SUMO_ATTR_VISIBILITY_DISTANCE:
            nbCon.visibility = parse<double>(value);
            break;
        case SUMO_ATTR_SPEED:
            if (value.empty() || (value == "default")) {
                nbCon.speed = NBEdge::UNSPECIFIED_SPEED;
            } else {
                nbCon.speed = parse<double>(value);
            }
            break;
        case SUMO_ATTR_LENGTH:
            nbCon.customLength = parse<double>(value);
            break;
        case SUMO_ATTR_ALLOW:
            nbCon.permissions = parseVehicleClasses(value);
            break;
        case SUMO_ATTR_DISALLOW:
            nbCon.permissions = invertPermissions(parseVehicleClasses(value));
            break;
        case SUMO_ATTR_CHANGE_LEFT:
            nbCon.changeLeft = value == "" ? SVC_UNSPECIFIED : parseVehicleClasses(value);
            break;
        case SUMO_ATTR_CHANGE_RIGHT:
            nbCon.changeRight = value == "" ? SVC_UNSPECIFIED : parseVehicleClasses(value);
            break;
        case SUMO_ATTR_STATE:
            throw InvalidArgument("Attribute of '" + toString(key) + "' cannot be modified");
        case SUMO_ATTR_DIR:
            throw InvalidArgument("Attribute of '" + toString(key) + "' cannot be modified");
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CUSTOMSHAPE:
            nbCon.customShape = parse<PositionVector>(value);
            break;
        case SUMO_ATTR_TYPE:
            nbCon.edgeType = value;
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
    // Update Geometry after setting a new attribute (but avoided for certain attributes)
    if ((key != SUMO_ATTR_ID) && (key != GNE_ATTR_PARAMETERS) && (key != GNE_ATTR_SELECTED)) {
        markConnectionGeometryDeprecated();
        updateGeometry();
    }
    // invalidate demand path calculator
    myNet->getDemandPathManager()->getPathCalculator()->invalidatePathCalculator();
}

/****************************************************************************/
