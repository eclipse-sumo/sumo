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
/// @file    GNECrossing.cpp
/// @author  Jakob Erdmann
/// @date    June 2011
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNECrossing.h"
#include "GNEJunction.h"
#include "GNEEdge.h"


// ===========================================================================
// method definitions
// ===========================================================================
GNECrossing::GNECrossing(GNEJunction* parentJunction, std::vector<NBEdge*> crossingEdges) :
    GNENetworkElement(parentJunction->getNet(), parentJunction->getNBNode()->getCrossing(crossingEdges)->id,
                      GLO_CROSSING, SUMO_TAG_CROSSING,
{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
myParentJunction(parentJunction),
myCrossingEdges(crossingEdges) {
}


GNECrossing::~GNECrossing() {}


std::string
GNECrossing::generateChildID(SumoXMLTag /*childTag*/) {
    // currently unused
    return "";
}


const PositionVector&
GNECrossing::getCrossingShape() const {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    if (crossing) {
        return (crossing->customShape.size() > 0) ? crossing->customShape : crossing->shape;
    } else {
        throw ProcessError("Crossing doesn't exist");
    }
}


void
GNECrossing::updateGeometry() {
    // rebuild crossing and walking areas form node parent
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    // obtain shape
    myCrossingGeometry.updateGeometry(crossing->customShape.size() > 0 ?  crossing->customShape : crossing->shape);
    /*
    // only rebuild shape if junction's shape isn't in Buuble mode
    if (myParentJunction->getNBNode()->getShape().size() > 0) {
        myCrossingGeometry.calculateShapeRotationsAndLengths();
    }
    */
    // mark dotted geometry deprecated
    myDottedGeometry.markDottedGeometryDeprecated();
}


Position
GNECrossing::getPositionInView() const {
    // currently unused
    return Position(0, 0);
}


GNEJunction*
GNECrossing::getParentJunction() const {
    return myParentJunction;
}


const std::vector<NBEdge*>&
GNECrossing::getCrossingEdges() const {
    return myCrossingEdges;
}


NBNode::Crossing*
GNECrossing::getNBCrossing() const {
    return myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
}


void
GNECrossing::drawGL(const GUIVisualizationSettings& s) const {
    // only draw if option drawCrossingsAndWalkingareas is enabled and size of shape is greather than 0 and zoom is close enough
    if (s.drawCrossingsAndWalkingareas &&
            (myCrossingGeometry.getShapeRotations().size() > 0) &&
            (myCrossingGeometry.getShapeLengths().size() > 0) &&
            (s.scale > 3.0)) {
        auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
        if (myNet->getViewNet()->getEditModes().networkEditMode != NetworkEditMode::NETWORK_TLS) {
            // push first draw matrix
            glPushMatrix();
            // push name
            glPushName(getGlID());
            // must draw on top of junction
            glTranslated(0, 0, GLO_JUNCTION + 0.1);
            // set color depending of selection and priority
            if (drawUsingSelectColor()) {
                GLHelper::setColor(s.colorSettings.selectedCrossingColor);
            } else if (!crossing->valid) {
                GLHelper::setColor(s.colorSettings.crossingInvalid);
            } else if (crossing->priority) {
                GLHelper::setColor(s.colorSettings.crossingPriority);
            } else if (myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
                GLHelper::setColor(s.laneColorer.getSchemes()[0].getColor(8));
            } else {
                GLHelper::setColor(s.colorSettings.crossing);
            }
            // traslate to front
            glTranslated(0, 0, .2);
            // set default values
            double length = 0.5;
            double spacing = 1.0;
            double halfWidth = crossing->width * 0.5;
            // push second draw matrix
            glPushMatrix();
            // draw on top of of the white area between the rails
            glTranslated(0, 0, 0.1);
            for (int i = 0; i < (int)myCrossingGeometry.getShape().size() - 1; ++i) {
                // push three draw matrix
                glPushMatrix();
                // translate and rotate
                glTranslated(myCrossingGeometry.getShape()[i].x(), myCrossingGeometry.getShape()[i].y(), 0.0);
                glRotated(myCrossingGeometry.getShapeRotations()[i], 0, 0, 1);
                // draw crossing depending if isn't being drawn for selecting
                if (!s.drawForRectangleSelection) {
                    for (double t = 0; t < myCrossingGeometry.getShapeLengths()[i]; t += spacing) {
                        glBegin(GL_QUADS);
                        glVertex2d(-halfWidth, -t);
                        glVertex2d(-halfWidth, -t - length);
                        glVertex2d(halfWidth, -t - length);
                        glVertex2d(halfWidth, -t);
                        glEnd();
                    }
                } else {
                    // only draw a single rectangle if it's being drawn only for selecting
                    glBegin(GL_QUADS);
                    glVertex2d(-halfWidth, 0);
                    glVertex2d(-halfWidth, -myCrossingGeometry.getShapeLengths().back());
                    glVertex2d(halfWidth, -myCrossingGeometry.getShapeLengths().back());
                    glVertex2d(halfWidth, 0);
                    glEnd();
                }
                // pop three draw matrix
                glPopMatrix();
            }
            // pop second draw matrix
            glPopMatrix();
            // traslate to back
            glTranslated(0, 0, -.2);
            // pop name
            glPopName();
            // pop draw matrix
            glPopMatrix();
        }
        // link indices must be drawn in all edit modes if isn't being drawn for selecting
        if (s.drawLinkTLIndex.show && !s.drawForRectangleSelection) {
            drawTLSLinkNo(s);
        }
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->getDottedAC() == this) {
            GNEGeometry::drawShapeDottedContour(s, getType(), 1, myDottedGeometry);
        }
    }
}


void
GNECrossing::drawTLSLinkNo(const GUIVisualizationSettings& s) const {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    glPushMatrix();
    glTranslated(0, 0, GLO_JUNCTION + 0.5);
    PositionVector shape = crossing->shape;
    shape.extrapolate(0.5); // draw on top of the walking area
    int linkNo = crossing->tlLinkIndex;
    int linkNo2 = crossing->tlLinkIndex2 > 0 ? crossing->tlLinkIndex2 : linkNo;
    GLHelper::drawTextAtEnd(toString(linkNo2), shape, 0, s.drawLinkTLIndex, s.scale);
    GLHelper::drawTextAtEnd(toString(linkNo), shape.reverse(), 0, s.drawLinkTLIndex, s.scale);
    glPopMatrix();
}


GUIGLObjectPopupMenu*
GNECrossing::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
        FXMenuCommand* mcCustomShape = new FXMenuCommand(ret, "Set custom crossing shape", nullptr, &parent, MID_GNE_CROSSING_EDIT_SHAPE);
        // check if menu commands has to be disabled
        NetworkEditMode editMode = myNet->getViewNet()->getEditModes().networkEditMode;
        if ((editMode == NetworkEditMode::NETWORK_CONNECT) || (editMode == NetworkEditMode::NETWORK_TLS) || (editMode == NetworkEditMode::NETWORK_CREATE_EDGE)) {
            mcCustomShape->disable();
        }
    }
    return ret;
}


Boundary
GNECrossing::getCenteringBoundary() const {
    Boundary b;
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    if (crossing) {
        if (crossing->customShape.size() > 0) {
            b = crossing->customShape.getBoxBoundary();
        } else if (crossing->shape.size() > 0) {
            b = crossing->shape.getBoxBoundary();
        } else {
            return myParentJunction->getCenteringBoundary();
        }
        b.grow(10);
        return b;
    }
    // in other case return boundary of parent junction
    return myParentJunction->getCenteringBoundary();
}


std::string
GNECrossing::getAttribute(SumoXMLAttr key) const {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges, (key != SUMO_ATTR_ID));
    switch (key) {
        case SUMO_ATTR_ID:
            // get attribute requires a special case
            if (crossing) {
                return crossing->id;
            } else {
                return "Temporal Unreferenced";
            }
        case SUMO_ATTR_WIDTH:
            return toString(crossing->customWidth);
        case SUMO_ATTR_PRIORITY:
            return crossing->priority ? "true" : "false";
        case SUMO_ATTR_EDGES:
            return toString(crossing->edges);
        case SUMO_ATTR_TLLINKINDEX:
            return toString(crossing->customTLIndex);
        case SUMO_ATTR_TLLINKINDEX2:
            return toString(crossing->customTLIndex2);
        case SUMO_ATTR_CUSTOMSHAPE:
            return toString(crossing->customShape);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return crossing->getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNECrossing::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_PRIORITY:
        case SUMO_ATTR_TLLINKINDEX:
        case SUMO_ATTR_TLLINKINDEX2:
        case SUMO_ATTR_CUSTOMSHAPE:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->add(new GNEChange_Attribute(this, myNet, key, value), true);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNECrossing::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            // id isn't editable
            return false;
        case SUMO_ATTR_TLLINKINDEX:
        case SUMO_ATTR_TLLINKINDEX2:
            return (myParentJunction->getNBNode()->getCrossing(myCrossingEdges)->tlID != "");
        default:
            return true;
    }
}


bool
GNECrossing::isValid(SumoXMLAttr key, const std::string& value) {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        case SUMO_ATTR_EDGES:
            if (canParse<std::vector<GNEEdge*> >(myNet, value, false)) {
                // parse edges and save their IDs in a set
                std::vector<GNEEdge*> parsedEdges = parse<std::vector<GNEEdge*> >(myNet, value);
                EdgeVector nbEdges;
                for (auto i : parsedEdges) {
                    nbEdges.push_back(i->getNBEdge());
                }
                std::sort(nbEdges.begin(), nbEdges.end());
                //
                EdgeVector originalEdges = crossing->edges;
                std::sort(originalEdges.begin(), originalEdges.end());
                // return true if we're setting the same edges
                if (toString(nbEdges) == toString(originalEdges)) {
                    return true;
                } else {
                    return !myParentJunction->getNBNode()->checkCrossingDuplicated(nbEdges);
                }
            } else {
                return false;
            }
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && ((parse<double>(value) > 0) || (parse<double>(value) == -1)); // kann NICHT 0 sein, oder -1 (bedeutet default)
        case SUMO_ATTR_PRIORITY:
            return canParse<bool>(value);
        case SUMO_ATTR_TLLINKINDEX:
        case SUMO_ATTR_TLLINKINDEX2:
            // -1 means that tlLinkIndex2 takes on the same value as tlLinkIndex when setting idnices
            return (isAttributeEnabled(key) &&
                    canParse<int>(value)
                    && ((parse<double>(value) >= 0) || ((parse<double>(value) == -1) && (key == SUMO_ATTR_TLLINKINDEX2)))
                    && myParentJunction->getNBNode()->getControllingTLS().size() > 0
                    && (*myParentJunction->getNBNode()->getControllingTLS().begin())->getMaxValidIndex() >= parse<int>(value));
        case SUMO_ATTR_CUSTOMSHAPE: {
            // empty shapes are allowed
            return canParse<PositionVector>(value);
        }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNECrossing::checkEdgeBelong(GNEEdge* edge) const {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    if (std::find(crossing->edges.begin(), crossing->edges.end(), edge->getNBEdge()) !=  crossing->edges.end()) {
        return true;
    } else {
        return false;
    }
}


bool
GNECrossing::checkEdgeBelong(const std::vector<GNEEdge*>& edges) const {
    for (auto i : edges) {
        if (checkEdgeBelong(i)) {
            return true;
        }
    }
    return false;
}

// ===========================================================================
// private
// ===========================================================================

void
GNECrossing::setAttribute(SumoXMLAttr key, const std::string& value) {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("Modifying attribute '" + toString(key) + "' of " + getTagStr() + " isn't allowed");
        case SUMO_ATTR_EDGES: {
            // obtain GNEEdges
            std::vector<GNEEdge*> edges = parse<std::vector<GNEEdge*> >(myNet, value);
            // remove NBEdges of crossing
            crossing->edges.clear();
            // set NBEdge of every GNEEdge into Crossing Edges
            for (auto i : edges) {
                crossing->edges.push_back(i->getNBEdge());
            }
            // sort new edges
            std::sort(crossing->edges.begin(), crossing->edges.end());
            // change myCrossingEdges by the new edges
            myCrossingEdges = crossing->edges;
            // update geometry of parent junction
            myParentJunction->updateGeometry();
            break;
        }
        case SUMO_ATTR_WIDTH:
            // Change width an refresh element
            crossing->customWidth = parse<double>(value);
            break;
        case SUMO_ATTR_PRIORITY:
            crossing->priority = parse<bool>(value);
            break;
        case SUMO_ATTR_TLLINKINDEX:
            crossing->customTLIndex = parse<int>(value);
            // make new value visible immediately
            crossing->tlLinkIndex = crossing->customTLIndex;
            break;
        case SUMO_ATTR_TLLINKINDEX2:
            crossing->customTLIndex2 = parse<int>(value);
            // make new value visible immediately
            crossing->tlLinkIndex2 = crossing->customTLIndex2;
            break;
        case SUMO_ATTR_CUSTOMSHAPE: {
            // set custom shape
            crossing->customShape = parse<PositionVector>(value);
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
            crossing->setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // Crossing are a special case and we need ot update geometry of junction instead of crossing
    if ((key != SUMO_ATTR_ID) && (key != GNE_ATTR_PARAMETERS) && (key != GNE_ATTR_SELECTED)) {
        myParentJunction->updateGeometry();
    }
}


void
GNECrossing::updateDottedContour() {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    // build contour using connection geometry
    PositionVector contourFront = myCrossingGeometry.getShape();
    PositionVector contourback = contourFront;
    // move both to side
    contourFront.move2side(crossing->width * 0.5);
    contourback.move2side(crossing->width * -0.5);
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
