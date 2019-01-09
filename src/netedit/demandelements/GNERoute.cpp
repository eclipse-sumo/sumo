/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERoute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// A class for visualizing routes in Netedit
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
#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_TLS.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/netelements/GNELane.h>
#include <utils/options/OptionsCont.h>

#include "GNERoute.h"


// ===========================================================================
// static member definitions
// ===========================================================================
int NUM_POINTS = 5;

// ===========================================================================
// method definitions
// ===========================================================================

GNERoute::GNERoute(GNELane* from, GNELane* to) :
    GNEDemandElement(from->getNet(), "from" + from->getMicrosimID() + "to" + to->getMicrosimID(),
                  GLO_CONNECTION, SUMO_TAG_ROUTE) {
}


GNERoute::~GNERoute() {
}


void
GNERoute::updateGeometry(bool updateGrid) {
    // first check if object has to be removed from grid (SUMOTree)
    if (updateGrid) {
        myNet->removeGLObjectFromGrid(this);
    }
    // Clear containers
    myShape.clear();
    myShapeRotations.clear();
    myShapeLengths.clear();

    // last step is to check if object has to be added into grid (SUMOTree) again
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
    }
}


Boundary
GNERoute::getBoundary() const {
    return myShape.getBoxBoundary();
}


const PositionVector&
GNERoute::getShape() const {
    return myShape;
}


GUIGLObjectPopupMenu*
GNERoute::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
    FXMenuCommand* mcCustomShape = new FXMenuCommand(ret, "Set custom connection shape", nullptr, &parent, MID_GNE_CONNECTION_EDIT_SHAPE);
    // check if menu commands has to be disabled
    NetworkEditMode editMode = myNet->getViewNet()->getCurrentNetworkEditMode();
    const bool wrongMode = (editMode == GNE_NMODE_CONNECT || editMode == GNE_NMODE_TLS || editMode == GNE_NMODE_CREATE_EDGE);
    if (wrongMode) {
        mcCustomShape->disable();
    }
    return ret;
}


Boundary
GNERoute::getCenteringBoundary() const {
    Boundary b = getBoundary();
    b.grow(20);
    return b;
}


void
GNERoute::drawGL(const GUIVisualizationSettings& s) const {
/**
    // Check if connection must be drawed
    if (!myShapeDeprecated && myNet->getViewNet()->showConnections()) {
        // Push draw matrix 1
        glPushMatrix();
        // Push name
        glPushName(getGlID());
        // Traslate matrix
        glTranslated(0, 0, GLO_JUNCTION + 0.1); // must draw on top of junction
        // Set color
        if (isAttributeCarrierSelected()) {
            // override with special colors (unless the color scheme is based on selection)
            GLHelper::setColor(s.selectedConnectionColor);
        } else if (mySpecialColor != nullptr) {
            GLHelper::setColor(*mySpecialColor);
        } else {
            // Set color depending of the link state
            GLHelper::setColor(GNEInternalLane::colorForLinksState(getLinkState()));
        }
        // draw connection checking whether it is not too small if isn't being drawn for selecting
        if ((s.scale < 1.) && !s.drawForSelecting) {
            // If it's small, dra a simple line
            GLHelper::drawLine(myShape);
        } else {
            // draw a list of lines
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, 0.2);
            glTranslated(0, 0, 0.1);
            GLHelper::setColor(GLHelper::getColor().changedBrightness(51));
            // check if internal junction marker has to be drawn
            if (myInternalJunctionMarker.size() > 0) {
                GLHelper::drawLine(myInternalJunctionMarker);
            }
        }
        // check if dotted contour has to be drawn
        if (!s.drawForSelecting && (myNet->getViewNet()->getDottedAC() == this)) {
            GLHelper::drawShapeDottedContour(getType(), myShape, 0.25);
        }
        // Pop name
        glPopName();
        // Pop draw matrix 1
        glPopMatrix();
    }
**/
}

std::string
GNERoute::getAttribute(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_ID) {
        // used by GNEReferenceCounter
        // @note: may be called for connections without a valid nbCon reference
        return getMicrosimID();
    }
    switch (key) {
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERoute::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            // no special handling
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERoute::isValid(SumoXMLAttr key, const std::string& value) {
    // Currently ignored before implementation to avoid warnings
    switch (key) {
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNERoute::getGenericParametersStr() const {
    std::string result;
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNERoute::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    /*
    // iterate over parameters map and fill result
    for (auto i : getNBEdgeConnection().getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    */
    return result;
}


void
GNERoute::setGenericParametersStr(const std::string& value) {
    /*
    // clear parameters
    getNBEdgeConnection().clearParameter();
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
            getNBEdgeConnection().setParameter(parsedParameters.front(), parsedParameters.back());
        }
    }
    */
}

// ===========================================================================
// private
// ===========================================================================

void
GNERoute::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
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
    // Update Geometry after setting a new attribute (but avoided for certain attributes)
    if((key != SUMO_ATTR_ID) && (key != GNE_ATTR_GENERIC) && (key != GNE_ATTR_SELECTED)) {
        updateGeometry(true);
    }
}


void
GNERoute::mouseOverObject(const GUIVisualizationSettings&) const {
}


/****************************************************************************/
