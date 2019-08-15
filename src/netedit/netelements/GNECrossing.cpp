/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECrossing.cpp
/// @author  Jakob Erdmann
/// @date    June 2011
/// @version $Id$
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StringTokenizer.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNECrossing.h"
#include "GNEJunction.h"
#include "GNEEdge.h"


// ===========================================================================
// method definitions
// ===========================================================================
GNECrossing::GNECrossing(GNEJunction* parentJunction, std::vector<NBEdge*> crossingEdges) :
    GNENetElement(parentJunction->getNet(), parentJunction->getNBNode()->getCrossing(crossingEdges)->id, GLO_CROSSING, SUMO_TAG_CROSSING),
    myParentJunction(parentJunction),
    myCrossingEdges(crossingEdges) {
}


GNECrossing::~GNECrossing() {}


std::string
GNECrossing::generateChildID(SumoXMLTag /*childTag*/) {
    // currently unused
    return "";
}


void
GNECrossing::updateGeometry() {
    // first clear geometry
    myGeometry.clearGeometry();
    // rebuild crossing and walking areas form node parent
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    // obtain shape
    myGeometry.shape = crossing->customShape.size() > 0 ?  crossing->customShape : crossing->shape;
    // only rebuild shape if junction's shape isn't in Buuble mode
    if (myParentJunction->getNBNode()->getShape().size() > 0) {
        myGeometry.calculateShapeRotationsAndLengths();
    }
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
            (myGeometry.shapeRotations.size() > 0) &&
            (myGeometry.shapeLengths.size() > 0) &&
            (s.scale > 3.0)) {
        auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
        if (myNet->getViewNet()->getEditModes().networkEditMode != GNE_NMODE_TLS) {
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
            for (int i = 0; i < (int)myGeometry.shape.size() - 1; ++i) {
                // push three draw matrix
                glPushMatrix();
                // translate and rotate
                glTranslated(myGeometry.shape[i].x(), myGeometry.shape[i].y(), 0.0);
                glRotated(myGeometry.shapeRotations[i], 0, 0, 1);
                // draw crossing depending if isn't being drawn for selecting
                if (!s.drawForSelecting) {
                    for (double t = 0; t < myGeometry.shapeLengths[i]; t += spacing) {
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
                    glVertex2d(-halfWidth, -myGeometry.shapeLengths.back());
                    glVertex2d(halfWidth, -myGeometry.shapeLengths.back());
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
        if (s.drawLinkTLIndex.show && !s.drawForSelecting) {
            drawTLSLinkNo(s);
        }
        // check if dotted contour has to be drawn
        if (myNet->getViewNet()->getDottedAC() == this) {
            GLHelper::drawShapeDottedContourAroundShape(s, getType(), myGeometry.shape, crossing->width * 0.5);
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
    GLHelper::drawTextAtEnd(toString(linkNo2), shape, 0, s.drawLinkTLIndex.size, s.drawLinkTLIndex.color);
    GLHelper::drawTextAtEnd(toString(linkNo), shape.reverse(), 0, s.drawLinkTLIndex.size, s.drawLinkTLIndex.color);
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
    // create menu commands
    FXMenuCommand* mcCustomShape = new FXMenuCommand(ret, "Set custom crossing shape", nullptr, &parent, MID_GNE_CROSSING_EDIT_SHAPE);
    // check if menu commands has to be disabled
    NetworkEditMode editMode = myNet->getViewNet()->getEditModes().networkEditMode;
    const bool wrongMode = (editMode == GNE_NMODE_CONNECT || editMode == GNE_NMODE_TLS || editMode == GNE_NMODE_CREATE_EDGE);
    if (wrongMode) {
        mcCustomShape->disable();
    }
    return ret;
}


Boundary
GNECrossing::getCenteringBoundary() const {
    throw ProcessError("Crossings doesn't have a boundary");
}


std::string
GNECrossing::getAttribute(SumoXMLAttr key) const {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges, (key != SUMO_ATTR_ID));
    switch (key) {
        case SUMO_ATTR_ID:
            // get attribute requieres a special case
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
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
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
        case GNE_ATTR_GENERIC:
            undoList->add(new GNEChange_Attribute(this, myNet, key, value), true);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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
            return (crossing->tlID != "" && canParse<int>(value)
                    // -1 means that tlLinkIndex2 takes on the same value as tlLinkIndex when setting idnices
                    && ((parse<double>(value) >= 0) || ((parse<double>(value) == -1) && (key == SUMO_ATTR_TLLINKINDEX2)))
                    && myParentJunction->getNBNode()->getControllingTLS().size() > 0
                    && (*myParentJunction->getNBNode()->getControllingTLS().begin())->getMaxValidIndex() >= parse<int>(value));
        case SUMO_ATTR_CUSTOMSHAPE: {
            // empty shapes are allowed
            return canParse<PositionVector>(value);
        }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNECrossing::getGenericParametersStr() const {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : crossing->getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNECrossing::getGenericParameters() const {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : crossing->getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNECrossing::setGenericParametersStr(const std::string& value) {
    auto crossing = myParentJunction->getNBNode()->getCrossing(myCrossingEdges);
    // clear parameters
    crossing->clearParameter();
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
            crossing->setParameter(parsedParameters.front(), parsedParameters.back());
        }
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
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // Crossing are a special case and we need ot update geometry of junction instead of crossing
    if ((key != SUMO_ATTR_ID) && (key != GNE_ATTR_GENERIC) && (key != GNE_ATTR_SELECTED)) {
        myParentJunction->updateGeometry();
    }
}

/****************************************************************************/
