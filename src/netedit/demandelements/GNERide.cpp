/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERide.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
/// @version $Id$
///
// A class for visualizing rides in Netedit
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <utils/gui/div/GUIGlobalSelection.h>

#include "GNERide.h"
#include "GNEVehicle.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNERide::GNERide(GNEViewNet* viewNet, GNEDemandElement *personParent, GNEEdge* from, GNEEdge* to, double arrivalPosition, const std::vector<std::string> &lines) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", SUMO_TAG_RIDE), viewNet, GLO_RIDE, SUMO_TAG_RIDE,
    {from, to}, {}, {}, {}, {personParent}, {}, {}, {}, {}, {}),
    myLines(lines),
    myArrivalPosition(arrivalPosition) {
}


GNERide::GNERide(GNEViewNet* viewNet, GNEDemandElement *personParent, GNEEdge* from, GNEAdditional *busStop, const std::vector<std::string> &lines) :
    GNEDemandElement(viewNet->getNet()->generateDemandElementID("", SUMO_TAG_RIDE), viewNet, GLO_RIDE, SUMO_TAG_RIDE,
    {from}, {}, {}, {busStop}, {personParent}, {}, {}, {}, {}, {}),
    myLines(lines),
    myArrivalPosition(-1) {
}


GNERide::~GNERide() {}


SUMOVehicleClass 
GNERide::getVClass() const {
    return getDemandElementParents().front()->getVClass();
}


GUIGLObjectPopupMenu*
GNERide::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buld menu commands for names
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " name to clipboard").c_str(), nullptr, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " typed name to clipboard").c_str(), nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myViewNet->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open demand element dialog
    if (myTagProperty.hasDialog()) {
        new FXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    new FXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), nullptr, nullptr, 0);
    return ret;
}


const RGBColor&
GNERide::getColor() const {
    return getDemandElementParents().front()->getColor();
}


void
GNERide::writeDemandElement(OutputDevice& device) const {
    device.openTag(SUMO_TAG_RIDE);
    device.writeAttr(SUMO_ATTR_FROM, getEdgeParents().front());
    // check if write busStop or edge to
    if (getAdditionalParents().size() > 0) {
        device.writeAttr(SUMO_ATTR_BUS_STOP, getAdditionalParents().front());
    } else {
        device.writeAttr(SUMO_ATTR_TO, getEdgeParents());
    }
    // only write arrivalPos if is different of -1
    if (myArrivalPosition != -1) {
        device.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPosition);
    }
    device.closeTag();
}


bool
GNERide::isDemandElementValid() const {
    if (getEdgeParents().size() == 0) {
        return false;
    } else if (getEdgeParents().size() == 1) {
        return true;
    } else {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getEdgeParents().size(); i++) {
            if (getRouteCalculatorInstance()->areEdgesConsecutives(getDemandElementParents().front()->getVClass(), getEdgeParents().at((int)i - 1), getEdgeParents().at(i)) == false) {
                return false;
            }
        }
        // there is connections bewteen all edges, then return true
        return true;
    }
}


std::string 
GNERide::getDemandElementProblem() const {
    if (getEdgeParents().size() == 0) {
        return ("A ride need at least one edge");
    } else {
        // check if exist at least a connection between every edge
        for (int i = 1; i < (int)getEdgeParents().size(); i++) {
            if (getRouteCalculatorInstance()->areEdgesConsecutives(getDemandElementParents().front()->getVClass(), getEdgeParents().at((int)i - 1), getEdgeParents().at(i)) == false) {
                return ("Edge '" + getEdgeParents().at((int)i - 1)->getID() + "' and edge '" + getEdgeParents().at(i)->getID() + "' aren't consecutives");
            }
        }
        // there is connections bewteen all edges, then all ok
        return "";
    }
}


void 
GNERide::fixDemandElementProblem() {
    // currently the only solution is removing Ride
}


void 
GNERide::startGeometryMoving() {
    // Rides cannot be moved
}


void 
GNERide::endGeometryMoving() {
    // Rides cannot be moved
}


void
GNERide::moveGeometry(const Position&) {
    // Rides cannot be moved
}


void
GNERide::commitGeometryMoving(GNEUndoList*) {
    // Rides cannot be moved
}


void
GNERide::updateGeometry() {
    // only update demand element childs, because Ride uses the geometry of lane parent
    for (const auto& i : getDemandElementChilds()) {
        i->updateGeometry();
    }
}


Position
GNERide::getPositionInView() const {
    return Position();
}


std::string
GNERide::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


Boundary
GNERide::getCenteringBoundary() const {
    Boundary rideBoundary;
    // return the combination of all edge parents's boundaries
    for (const auto &i : getEdgeParents()) {
        rideBoundary.add(i->getCenteringBoundary());
    }
    // check if is valid
    if (rideBoundary.isInitialised()) {
        return rideBoundary;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNERide::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // Rides are drawn in GNEEdges
}


void
GNERide::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(getType());
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNERide::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(getType());
        if (changeFlag) {
            mySelected = false;

        }
    }
}


std::string
GNERide::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_FROM:
            return getEdgeParents().front()->getID();
        case SUMO_ATTR_TO:
            return getEdgeParents().back()->getID();
        case SUMO_ATTR_BUS_STOP:
            return getAdditionalParents().front()->getID();
        case SUMO_ATTR_LINES:
            return joinToString(myLines, " ");
        case SUMO_ATTR_ARRIVALPOS:
            return toString(myArrivalPosition);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERide::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_BUS_STOP:
        case SUMO_ATTR_LINES:
        case SUMO_ATTR_ARRIVALPOS:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERide::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myViewNet->getNet()->retrieveEdge(value, false) != nullptr);
        case SUMO_ATTR_BUS_STOP:
            return (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        case SUMO_ATTR_LINES:
            return canParse<std::vector<std::string> >(value);
        case SUMO_ATTR_ARRIVALPOS:
            if (canParse<double>(value)) {
                double parsedValue = canParse<double>(value);
                // a arrival pos with value -1 means that it will be ignored
                if (parsedValue == -1) {
                    return true;
                } else {
                    return parsedValue >= 0;
                }
            } else {
                return false;
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNERide::isAttributeSet(SumoXMLAttr /*key*/) const {
    return true;
}


std::string
GNERide::getPopUpID() const {
    return getTagStr();
}


std::string
GNERide::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) ;
}

// ===========================================================================
// private
// ===========================================================================

void
GNERide::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM: {
            // create a string vector with new from-to edges
            std::vector<std::string> newEdges;
            // add edges depending if ride as a from-to edges or a from-busStop
            if (getEdgeParents().size() == 2) {
                newEdges.push_back(myViewNet->getNet()->retrieveEdge(value, false)->getID());
            }
            newEdges.push_back(getEdgeParents().back()->getID());
            // change edge parents
            changeEdgeParents(this, toString(newEdges));
            break;
        }
        case SUMO_ATTR_TO: {
            // create a string vector with new from-to edges
            std::vector<std::string> newEdges;
            // add edges depending if ride as a from-to edges or a from-busStop
            newEdges.push_back(getEdgeParents().front()->getID());
            if (getEdgeParents().size() == 2) {
                newEdges.push_back(myViewNet->getNet()->retrieveEdge(value, false)->getID());
            }
            // change edge parents
            changeEdgeParents(this, toString(newEdges));
            break;
        }
        case SUMO_ATTR_BUS_STOP:
            changeAdditionalParent(this, value, 0);
            break;
        case SUMO_ATTR_LINES:
            myLines = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
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

/****************************************************************************/
