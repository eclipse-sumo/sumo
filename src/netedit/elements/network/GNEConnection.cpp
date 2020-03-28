/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_TLS.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>

#include "GNEConnection.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEInternalLane.h"


// ===========================================================================
// static member definitions
// ===========================================================================
int NUM_POINTS = 5;

// ===========================================================================
// method definitions
// ===========================================================================

GNEConnection::GNEConnection(GNELane* from, GNELane* to) :
    GNENetworkElement(from->getNet(), "from" + from->getID() + "to" + to->getID(),
                      GLO_CONNECTION, SUMO_TAG_CONNECTION,
{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
myFromLane(from),
myToLane(to),
myLinkState(LINKSTATE_TL_OFF_NOSIGNAL),
mySpecialColor(nullptr),
myShapeDeprecated(true) {
}


GNEConnection::~GNEConnection() {
}


std::string
GNEConnection::generateChildID(SumoXMLTag /*childTag*/) {
    // currently unused
    return "";
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
    // Get shape of from and to lanes
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    if (myShapeDeprecated) {
        // obtain lane shape from
        PositionVector laneShapeFrom;
        if ((int)getEdgeFrom()->getNBEdge()->getLanes().size() > nbCon.fromLane) {
            laneShapeFrom = getEdgeFrom()->getNBEdge()->getLanes().at(nbCon.fromLane).shape;
        } else {
            return;
        }
        // obtain lane shape to
        PositionVector laneShapeTo;
        if ((int)nbCon.toEdge->getLanes().size() > nbCon.toLane) {
            laneShapeTo = nbCon.toEdge->getLanes().at(nbCon.toLane).shape;
        } else {
            return;
        }
        // Calculate shape of connection depending of the size of Junction shape
        // value obtanied from GNEJunction::drawgl
        if (nbCon.customShape.size() != 0) {
            myConnectionGeometry.updateGeometry(nbCon.customShape);
        } else if (getEdgeFrom()->getNBEdge()->getToNode()->getShape().area() > 4) {
            if (nbCon.shape.size() != 0) {
                PositionVector connectionShape = nbCon.shape;
                // only append via shape if it exists
                if (nbCon.haveVia) {
                    connectionShape.append(nbCon.viaShape);
                }
                myConnectionGeometry.updateGeometry(connectionShape);
            } else {
                // Calculate shape so something can be drawn immidiately
                myConnectionGeometry.updateGeometry(getEdgeFrom()->getNBEdge()->getToNode()->computeSmoothShape(
                                                        laneShapeFrom, laneShapeTo, NUM_POINTS,
                                                        getEdgeFrom()->getNBEdge()->getTurnDestination() == nbCon.toEdge,
                                                        (double) 5. * (double) getEdgeFrom()->getNBEdge()->getNumLanes(),
                                                        (double) 5. * (double) nbCon.toEdge->getNumLanes()));
            }
        } else {
            myConnectionGeometry.updateGeometry({laneShapeFrom.positionAtOffset(MAX2(0.0, laneShapeFrom.length() - 1)),
                                                 laneShapeTo.positionAtOffset(MIN2(1.0, laneShapeFrom.length()))});
        }
        // check if internal junction marker must be calculated
        if (nbCon.haveVia && (nbCon.shape.size() != 0)) {
            // create marker for interal junction waiting position (contPos)
            const double orthoLength = 0.5;
            Position pos = nbCon.shape.back();
            myInternalJunctionMarker = nbCon.shape.getOrthogonal(pos, 10, true, 0.1);
            if (myInternalJunctionMarker.length() < orthoLength) {
                myInternalJunctionMarker.extrapolate(orthoLength - myInternalJunctionMarker.length());
            }
        } else {
            myInternalJunctionMarker.clear();
        }
        // mark connection as non-deprecated
        myShapeDeprecated = false;
        // mark dotted geometry deprecated
        myDottedGeometry.markDottedGeometryDeprecated();
    }
}


Position
GNEConnection::getPositionInView() const {
    // currently unused
    return Position(0, 0);
}


Boundary
GNEConnection::getBoundary() const {
    if (myConnectionGeometry.getShape().size() == 0) {
        // we need to use the center of junction parent as boundary if shape is empty
        Position junctionParentPosition = myFromLane->getParentEdge()->getGNEJunctionDestiny()->getPositionInView();
        return Boundary(junctionParentPosition.x() - 0.1, junctionParentPosition.y() - 0.1,
                        junctionParentPosition.x() + 0.1, junctionParentPosition.x() + 0.1);
    } else {
        return myConnectionGeometry.getShape().getBoxBoundary();
    }
}


GNEEdge*
GNEConnection::getEdgeFrom() const {
    return myFromLane->getParentEdge();
}


GNEEdge*
GNEConnection::getEdgeTo() const {
    return myToLane->getParentEdge();
}


GNELane*
GNEConnection::getLaneFrom() const {
    return myFromLane;
}


GNELane*
GNEConnection::getLaneTo() const {
    return myToLane;
}


int
GNEConnection::getFromLaneIndex() const {
    return myFromLane->getIndex();
}


int
GNEConnection::getToLaneIndex() const {
    return myToLane->getIndex();
}


NBEdge::Connection&
GNEConnection::getNBEdgeConnection() const {
    return getEdgeFrom()->getNBEdge()->getConnectionRef(getFromLaneIndex(), getEdgeTo()->getNBEdge(), getToLaneIndex());
}


NBConnection
GNEConnection::getNBConnection() const {
    const NBEdge::Connection& c = getNBEdgeConnection();
    return NBConnection(getEdgeFrom()->getNBEdge(), getFromLaneIndex(),
                        getEdgeTo()->getNBEdge(), getToLaneIndex(),
                        (int)c.tlLinkIndex, (int)c.tlLinkIndex2);
}


void
GNEConnection::updateID() {
    setMicrosimID(myFromLane->getID() + " -> " + myToLane->getID());
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
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    myLinkState = getEdgeFrom()->getNBEdge()->getToNode()->getLinkState(getEdgeFrom()->getNBEdge(),
                  nbCon.toEdge,
                  nbCon.fromLane,
                  nbCon.toLane,
                  nbCon.mayDefinitelyPass,
                  nbCon.tlID);
}


GUIGLObjectPopupMenu*
GNEConnection::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // build position copy entry
    buildPositionCopyEntry(ret, false);
    // check if we're in supermode network
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        // create menu commands
        FXMenuCommand* mcCustomShape = new FXMenuCommand(ret, "Set custom connection shape", nullptr, &parent, MID_GNE_CONNECTION_EDIT_SHAPE);
        // check if menu commands has to be disabled
        NetworkEditMode editMode = myNet->getViewNet()->getEditModes().networkEditMode;
        // check if we're in the correct edit mode
        if ((editMode == NetworkEditMode::NETWORK_CONNECT) || (editMode == NetworkEditMode::NETWORK_TLS) || (editMode == NetworkEditMode::NETWORK_CREATE_EDGE)) {
            mcCustomShape->disable();
        }
    }
    return ret;
}


Boundary
GNEConnection::getCenteringBoundary() const {
    Boundary b = getBoundary();
    b.grow(20);
    return b;
}


void
GNEConnection::drawGL(const GUIVisualizationSettings& s) const {
    // declare a flag to check if shape has to be draw
    bool drawConnection = true;
    if ((myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) &&
            s.drawDetail(s.detailSettings.connectionsDemandMode, s.addSize.getExaggeration(s, this))) {
        drawConnection = !myShapeDeprecated;
    } else if ((myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) &&
               myNet->getViewNet()->getNetworkViewOptions().showConnections()) {
        drawConnection = !myShapeDeprecated;
    } else {
        drawConnection = false;
    }
    // Check if connection must be drawed
    if (drawConnection) {
        // check if boundary has to be drawn
        if (s.drawBoundaries) {
            GLHelper::drawBoundary(getBoundary());
        }
        // Push draw matrix 1
        glPushMatrix();
        // Push name
        glPushName(getGlID());
        // Traslate matrix
        glTranslated(0, 0, GLO_JUNCTION + 0.1); // must draw on top of junction
        // Set color
        if (drawUsingSelectColor()) {
            // override with special colors (unless the color scheme is based on selection)
            GLHelper::setColor(s.colorSettings.selectedConnectionColor);
        } else if (mySpecialColor != nullptr) {
            GLHelper::setColor(*mySpecialColor);
        } else {
            // Set color depending of the link state
            GLHelper::setColor(GNEInternalLane::colorForLinksState(getLinkState()));
        }
        // draw connection checking whether it is not too small if isn't being drawn for selecting
        const double selectionScale = isAttributeCarrierSelected() ? s.selectionScale : 1;
        if ((s.scale * selectionScale < 5.) && !s.drawForRectangleSelection) {
            // If it's small, draw a simple line
            GLHelper::drawLine(myConnectionGeometry.getShape());
        } else {
            // draw a list of lines
            const bool spreadSuperposed = s.scale >= 1 && s.spreadSuperposed && myFromLane->drawAsRailway(s) && getEdgeFrom()->getNBEdge()->isBidiRail();
            PositionVector shapeSuperposed = myConnectionGeometry.getShape();
            if (spreadSuperposed) {
                shapeSuperposed.move2side(0.5);
            }
            GLHelper::drawBoxLines(shapeSuperposed, myConnectionGeometry.getShapeRotations(), myConnectionGeometry.getShapeLengths(), 0.2 * selectionScale);
            glTranslated(0, 0, 0.1);
            GLHelper::setColor(GLHelper::getColor().changedBrightness(51));
            // check if internal junction marker has to be drawn
            if (myInternalJunctionMarker.size() > 0) {
                GLHelper::drawLine(myInternalJunctionMarker);
            }
            // check if dotted contour has to be drawn (not useful at high zoom)
            if (myNet->getViewNet()->getDottedAC() == this) {
                GNEGeometry::drawShapeDottedContour(s, getType(), 1, myDottedGeometry);
            }
        }
        // Pop draw matrix 1
        glPopMatrix();

        if (s.edgeValue.show) {
            NBEdge::Connection& nbCon = getNBEdgeConnection();
            std::string value = nbCon.getParameter(s.edgeParam, "");
            if (value != "") {
                int shapeIndex = (int)myConnectionGeometry.getShape().size() / 2;
                Position p = (myConnectionGeometry.getShape().size() == 2
                              ? (myConnectionGeometry.getShape().front() * 0.67 + myConnectionGeometry.getShape().back() * 0.33)
                              : myConnectionGeometry.getShape()[shapeIndex]);
                GLHelper::drawTextSettings(s.edgeValue, value, p, s.scale, 0);
            }
        }

        // Pop name
        glPopName();
    }
}


void
GNEConnection::setSpecialColor(const RGBColor* color) {
    mySpecialColor = color;
}

std::string
GNEConnection::getAttribute(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_ID) {
        // used by GNEReferenceCounter
        // @note: may be called for connections without a valid nbCon reference
        return getID();
    }
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    switch (key) {
        case SUMO_ATTR_FROM:
            return getEdgeFrom()->getID();
        case SUMO_ATTR_TO:
            return nbCon.toEdge->getID();
        case SUMO_ATTR_FROM_LANE:
            return toString(nbCon.fromLane);
        case SUMO_ATTR_TO_LANE:
            return toString(nbCon.toLane);
        case SUMO_ATTR_PASS:
            return toString(nbCon.mayDefinitelyPass);
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
        case SUMO_ATTR_SPEED:
            return toString(nbCon.speed);
        case SUMO_ATTR_LENGTH:
            return toString(nbCon.customLength);
        case SUMO_ATTR_DIR:
            return toString(getEdgeFrom()->getNBEdge()->getToNode()->getDirection(
                                getEdgeFrom()->getNBEdge(), nbCon.toEdge, OptionsCont::getOptions().getBool("lefthand")));
        case SUMO_ATTR_STATE:
            return toString(getEdgeFrom()->getNBEdge()->getToNode()->getLinkState(
                                getEdgeFrom()->getNBEdge(), nbCon.toEdge, nbCon.fromLane, nbCon.toLane, nbCon.mayDefinitelyPass, nbCon.tlID));
        case SUMO_ATTR_CUSTOMSHAPE:
            return toString(nbCon.customShape);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return nbCon.getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
        case SUMO_ATTR_UNCONTROLLED:
        case SUMO_ATTR_VISIBILITY_DISTANCE:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_CUSTOMSHAPE:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            // no special handling
            undoList->p_add(new GNEChange_Attribute(this, myNet, key, value));
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
        case SUMO_ATTR_DIR:
            throw InvalidArgument("Attribute of '" + toString(key) + "' cannot be modified");
        case SUMO_ATTR_STATE:
            throw InvalidArgument("Attribute of '" + toString(key) + "' cannot be modified");
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEConnection::changeTLIndex(SumoXMLAttr key, int tlIndex, int tlIndex2, GNEUndoList* undoList) {
    // trigger GNEChange_TLS
    undoList->p_begin("change tls linkIndex for connection");
    // make a copy
    std::set<NBTrafficLightDefinition*> defs = getEdgeFrom()->getNBEdge()->getToNode()->getControllingTLS();
    for (NBTrafficLightDefinition* tlDef : defs) {
        NBLoadedSUMOTLDef* sumoDef = dynamic_cast<NBLoadedSUMOTLDef*>(tlDef);
        NBTrafficLightLogic* tllogic = sumoDef ? sumoDef->getLogic() : tlDef->compute(OptionsCont::getOptions());
        if (tllogic != nullptr) {
            NBLoadedSUMOTLDef* newDef = new NBLoadedSUMOTLDef(*tlDef, *tllogic);
            newDef->addConnection(getEdgeFrom()->getNBEdge(), getEdgeTo()->getNBEdge(),
                                  getLaneFrom()->getIndex(), getLaneTo()->getIndex(), tlIndex, tlIndex2, false);
            // iterate over NBNodes
            for (NBNode* node : tlDef->getNodes()) {
                GNEJunction* junction = getNet()->retrieveJunction(node->getID());
                undoList->add(new GNEChange_TLS(junction, tlDef, false), true);
                undoList->add(new GNEChange_TLS(junction, newDef, true), true);
            }
        } else {
            WRITE_ERROR("Could not set attribute '" + toString(key) + "' (tls is broken)");
        }
    }
    undoList->p_end();
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
                    (getNBEdgeConnection().uncontrolled == false) &&
                    (getEdgeFrom()->getNBEdge()->getToNode()->getControllingTLS().size() > 0) &&
                    canParse<int>(value) &&
                    (parse<int>(value) >= 0 || parse<int>(value) == -1)) {
                // obtan Traffic light definition
                NBTrafficLightDefinition* def = *getEdgeFrom()->getNBEdge()->getToNode()->getControllingTLS().begin();
                return def->getMaxValidIndex() >= parse<int>(value);
            } else {
                return false;
            }
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_SPEED:
            return canParse<double>(value) && (parse<double>(value) >= -1);
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value) && (parse<double>(value) >= -1);
        case SUMO_ATTR_CUSTOMSHAPE: {
            // empty custom shapes are allowed
            return canParse<PositionVector>(value);
        }
        case SUMO_ATTR_STATE:
            return false;
        case SUMO_ATTR_DIR:
            return false;
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
            if (getEdgeFrom()->getNBEdge()->getToNode()->isTLControlled()) {
                NBTrafficLightDefinition* tlDef = *getEdgeFrom()->getNBEdge()->getToNode()->getControllingTLS().begin();
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

// ===========================================================================
// private
// ===========================================================================

void
GNEConnection::setAttribute(SumoXMLAttr key, const std::string& value) {
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    switch (key) {
        case SUMO_ATTR_PASS:
            nbCon.mayDefinitelyPass = parse<bool>(value);
            break;
        case SUMO_ATTR_KEEP_CLEAR:
            nbCon.keepClear = parse<bool>(value);
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
            nbCon.speed = parse<double>(value);
            break;
        case SUMO_ATTR_LENGTH:
            nbCon.customLength = parse<double>(value);
            break;
        case SUMO_ATTR_ALLOW: {
            const SVCPermissions successorAllows = nbCon.toEdge->getLanes()[nbCon.toLane].permissions;
            SVCPermissions customPermissions = parseVehicleClasses(value);
            if (successorAllows != customPermissions) {
                nbCon.permissions = customPermissions;
            }
            break;
        }
        case SUMO_ATTR_DISALLOW: {
            const SVCPermissions successorDisallows = invertPermissions(nbCon.toEdge->getLanes()[nbCon.toLane].permissions);
            SVCPermissions customPermissions = invertPermissions(parseVehicleClasses(value));
            if (successorDisallows != customPermissions) {
                nbCon.permissions = customPermissions;
            }
            break;
        }
        case SUMO_ATTR_STATE:
            throw InvalidArgument("Attribute of '" + toString(key) + "' cannot be modified");
        case SUMO_ATTR_DIR:
            throw InvalidArgument("Attribute of '" + toString(key) + "' cannot be modified");
        case SUMO_ATTR_CUSTOMSHAPE: {
            nbCon.customShape = parse<PositionVector>(value);
            break;
        }
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            nbCon.setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // Update Geometry after setting a new attribute (but avoided for certain attributes)
    if ((key != SUMO_ATTR_ID) && (key != GNE_ATTR_PARAMETERS) && (key != GNE_ATTR_SELECTED)) {
        markConnectionGeometryDeprecated();
        updateGeometry();
    }
}


void
GNEConnection::updateDottedContour() {
    // build contour using connection geometry
    PositionVector contourFront = myConnectionGeometry.getShape();
    PositionVector contourback = contourFront;
    // move both 0.25 to side
    contourFront.move2side(0.25);
    contourback.move2side(-0.25);
    // reverse contourback
    contourback = contourback.reverse();
    // add contour back to contourfront
    for (const auto& position : contourback) {
        contourFront.push_back(position);
    }
    // close contour front
    contourFront.closePolygon();
    // set as dotted contour
    myDottedGeometry.updateDottedGeometry(myNet->getViewNet()->getVisualisationSettings(), contourFront);
}


/****************************************************************************/
