/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEConnection.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// A class for visualizing connections between lanes
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
#include <time.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <netbuild/NBLoadedSUMOTLDef.h>

#include "GNEConnection.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEChange_TLS.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEInternalLane.h"



// ===========================================================================
// static member definitions
// ===========================================================================
int NUM_POINTS = 5;

// ===========================================================================
// method definitions
// ===========================================================================

GNEConnection::GNEConnection(GNELane* from, GNELane* to) :
    GNENetElement(from->getNet(), "from" + from->getMicrosimID() + "to" + to->getMicrosimID(),
                  GLO_CONNECTION, SUMO_TAG_CONNECTION, ICON_CONNECTION),
    myFromLane(from),
    myToLane(to),
    myLinkState(LINKSTATE_TL_OFF_NOSIGNAL) {
    // geometry will be updated later
}


GNEConnection::~GNEConnection() {
    if (myShape.size() > 0) {
        myNet->getVisualisationSpeedUp().removeAdditionalGLObject(this);
    }
}


void
GNEConnection::updateGeometry() {
    const bool init = myShape.size() == 0;
    // Clear containers
    myShapeRotations.clear();
    myShapeLengths.clear();
    // Get shape of from and to lanes
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    PositionVector laneShapeFrom;
    if ((int)getEdgeFrom()->getNBEdge()->getLanes().size() > nbCon.fromLane) {
        laneShapeFrom = getEdgeFrom()->getNBEdge()->getLanes().at(nbCon.fromLane).shape;
    } else {
        return;
    }
    PositionVector laneShapeTo;
    if ((int)nbCon.toEdge->getLanes().size() > nbCon.toLane) {
        laneShapeTo = nbCon.toEdge->getLanes().at(nbCon.toLane).shape;
    } else {
        return;
    }
    // Calculate shape of connection depending of the size of Junction shape
    // value obtanied from GNEJunction::drawgl
    if (nbCon.customShape.size() != 0) {
        myShape = nbCon.customShape;
    } else if (getEdgeFrom()->getNBEdge()->getToNode()->getShape().area() > 4) {
        if (nbCon.shape.size() != 0) {
            myShape = nbCon.shape;
            myShape.append(nbCon.viaShape);
        } else {
            // Calculate shape so something can be drawn immidiately
            myShape = getEdgeFrom()->getNBEdge()->getToNode()->computeSmoothShape(
                          laneShapeFrom,
                          laneShapeTo,
                          NUM_POINTS, getEdgeFrom()->getNBEdge()->getTurnDestination() == nbCon.toEdge,
                          (double) 5. * (double) getEdgeFrom()->getNBEdge()->getNumLanes(),
                          (double) 5. * (double) nbCon.toEdge->getNumLanes());
        }
    } else {
        myShape.clear();
        myShape.push_back(laneShapeFrom.positionAtOffset(laneShapeFrom.length() - 1));
        myShape.push_back(laneShapeTo.positionAtOffset(1));
    }
    if (nbCon.haveVia && nbCon.shape.size() != 0) {
        // create marker for interal junction waiting position (contPos)
        const double orthoLength = 0.5;
        Position pos = nbCon.shape.back();
        myInternalJunctionMarker = nbCon.shape.getOrthogonal(pos, 10, true, 0.1);
        if (myInternalJunctionMarker.length() < orthoLength) {
            myInternalJunctionMarker.extrapolate(orthoLength - myInternalJunctionMarker.length());
        }
    }
    // Obtain lengths and shape rotations
    int segments = (int) myShape.size() - 1;
    if (segments >= 0) {
        myShapeRotations.reserve(segments);
        myShapeLengths.reserve(segments);
        for (int i = 0; i < segments; ++i) {
            const Position& f = myShape[i];
            const Position& s = myShape[i + 1];
            myShapeLengths.push_back(f.distanceTo2D(s));
            myShapeRotations.push_back((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }
    if (!init) {
        myNet->getVisualisationSpeedUp().removeAdditionalGLObject(this);
    }
    myNet->getVisualisationSpeedUp().addAdditionalGLObject(this);
}


Boundary
GNEConnection::getBoundary() const {
    return myShape.getBoxBoundary();
}


GNEEdge*
GNEConnection::getEdgeFrom() const {
    return &(myFromLane->getParentEdge());
}


GNEEdge*
GNEConnection::getEdgeTo() const {
    return &(myToLane->getParentEdge());
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
    return NBConnection(getEdgeFrom()->getNBEdge(), getFromLaneIndex(),
                        getEdgeTo()->getNBEdge(), getToLaneIndex(),
                        (int)getNBEdgeConnection().tlLinkNo);
}


void
GNEConnection::updateID() {
    setMicrosimID(myFromLane->getMicrosimID() + " -> " + myToLane->getMicrosimID());
}


LinkState
GNEConnection::getLinkState() const {
    return myLinkState;
}


PositionVector
GNEConnection::getShape() const {
    if (myShape.size() > 0) {
        return myShape;
    } else {
        return getNBEdgeConnection().customShape;
    }
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
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    // build position copy entry
    buildPositionCopyEntry(ret, false);
    // create menu commands
    FXMenuCommand* mcCustomShape = new FXMenuCommand(ret, "Set custom connection shape", 0, &parent, MID_GNE_CONNECTION_EDIT_SHAPE);
    // check if menu commands has to be disabled
    EditMode editMode = myNet->getViewNet()->getCurrentEditMode();
    const bool wrongMode = (editMode == GNE_MODE_CONNECT || editMode == GNE_MODE_TLS || editMode == GNE_MODE_CREATE_EDGE);
    if (wrongMode) {
        mcCustomShape->disable();
    }
    return ret;
}


Boundary
GNEConnection::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(20);
    return b;
}


void
GNEConnection::drawGL(const GUIVisualizationSettings& s) const {
    // Check if connection must be drawed
    if (myNet->getViewNet()->showConnections()) {
        // Push draw matrix 1
        glPushMatrix();
        // Push name
        glPushName(getGlID());
        // Traslate matrix
        glTranslated(0, 0, GLO_JUNCTION + 0.1); // must draw on top of junction
        // Set color
        if (gSelected.isSelected(getType(), getGlID()) && s.junctionColorer.getActive() != 1) {
            // override with special colors (unless the color scheme is based on selection)
            GLHelper::setColor(GNENet::selectedConnectionColor);
        } else {
            // Set color depending of the link state
            GLHelper::setColor(GNEInternalLane::colorForLinksState(getLinkState()));
        }
        // draw connection checking whether it is not too small
        if (s.scale < 1.) {
            // If it's small, dra a simple line
            GLHelper::drawLine(myShape);
        } else {
            // draw a list of lines
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, 0.2);
            glTranslated(0, 0, 0.1);
            GLHelper::setColor(GLHelper::getColor().changedBrightness(51));;
            GLHelper::drawLine(myInternalJunctionMarker);
        }
        // Pop name
        glPopName();
        // Pop draw matrix 1
        glPopMatrix();
    }
}


std::string
GNEConnection::getAttribute(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_ID) {
        // used by GNEReferenceCounter
        // @note: may be called for connections without a valid nbCon reference
        return getMicrosimID();
    }
    NBEdge::Connection& nbCon = getNBEdgeConnection();
    switch (key) {
        case SUMO_ATTR_FROM:
            return getEdgeFrom()->getID();
        case SUMO_ATTR_TO:
            return nbCon.toEdge->getID();
        case SUMO_ATTR_FROM_LANE:
            return toString(nbCon.toLane);
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
            return toString(nbCon.tlLinkNo);
        case SUMO_ATTR_SPEED:
            return toString(nbCon.speed);
        case SUMO_ATTR_CUSTOMSHAPE:
            return toString(nbCon.customShape);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEConnection::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_CUSTOMSHAPE:
            // no special handling
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        case SUMO_ATTR_TLLINKINDEX:
            if (value != getAttribute(key)) {
                // trigger GNEChange_TLS
                undoList->p_begin("change tls linkIndex for connection");
                // make a copy
                std::set<NBTrafficLightDefinition*> defs = getEdgeFrom()->getNBEdge()->getToNode()->getControllingTLS();
                for (NBTrafficLightDefinition* tlDef : defs) {
                    NBTrafficLightLogic* tllogic = tlDef->compute(OptionsCont::getOptions());
                    NBLoadedSUMOTLDef* newDef = new NBLoadedSUMOTLDef(tlDef, tllogic);
                    newDef->addConnection(getEdgeFrom()->getNBEdge(), getEdgeTo()->getNBEdge(),
                                          getLaneFrom()->getIndex(), getLaneTo()->getIndex(), parse<int>(value), false);
                    std::vector<NBNode*> nodes = tlDef->getNodes();
                    for (NBNode* node : nodes) {
                        GNEJunction* junction = getNet()->retrieveJunction(node->getID());
                        undoList->add(new GNEChange_TLS(junction, tlDef, false), true);
                        undoList->add(new GNEChange_TLS(junction, newDef, true), true);
                    }
                    undoList->p_end();
                }
            }
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
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
        case SUMO_ATTR_KEEP_CLEAR:
            return canParse<bool>(value);
        case SUMO_ATTR_CONTPOS:
            return canParse<double>(value);
        case SUMO_ATTR_UNCONTROLLED:
            return canParse<bool>(value);
        case SUMO_ATTR_VISIBILITY_DISTANCE:
            return canParse<double>(value) && isPositive<double>(value);
        case SUMO_ATTR_TLLINKINDEX:
            if (getNBEdgeConnection().tlID != "" && canParse<int>(value) && parse<int>(value) >= 0
                    && getEdgeFrom()->getNBEdge()->getToNode()->getControllingTLS().size() > 0) {
                NBTrafficLightDefinition* def = *getEdgeFrom()->getNBEdge()->getToNode()->getControllingTLS().begin();
                def->setParticipantsInformation();
                NBTrafficLightLogic* logic = def->compute(OptionsCont::getOptions());
                return logic != 0 && logic->getNumLinks() > parse<int>(value);
            } else {
                return false;
            }
        case SUMO_ATTR_SPEED:
            return canParse<double>(value) && isPositive<double>(value);
        case SUMO_ATTR_CUSTOMSHAPE: {
            bool ok = true;
            PositionVector shape = GeomConvHelper::parseShapeReporting(value, "user-supplied shape", 0, ok, true);
            return ok;
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


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
        case SUMO_ATTR_CUSTOMSHAPE: {
            bool ok;
            nbCon.customShape = GeomConvHelper::parseShapeReporting(value, "user-supplied shape", 0, ok, true);
            updateGeometry();
            myNet->getViewNet()->update();
            break;
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


/****************************************************************************/
