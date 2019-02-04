/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDemandElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
/// @version $Id$
///
// A abstract class for demand elements
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEDemandElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDemandElement::GNEDemandElement(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    Parameterised(),
    myViewNet(viewNet),
    myFirstDemandElementParent(nullptr),
    mySecondDemandElementParent(nullptr),
    myChildConnections(this) {
}


GNEDemandElement::GNEDemandElement(GNEDemandElement* singleDemandElementParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag) :
    GUIGlObject(type, singleDemandElementParent->generateDemandElementChildID(tag)),
    GNEAttributeCarrier(tag),
    Parameterised(),
    myViewNet(viewNet),
    myFirstDemandElementParent(singleDemandElementParent),
    mySecondDemandElementParent(nullptr),
    myChildConnections(this) {
    // check that demand element parent is of expected type
    assert(singleDemandElementParent->getTagProperty().getTag() == myTagProperty.getParentTag());
}


GNEDemandElement::GNEDemandElement(GNEDemandElement* firstDemandElementParent, GNEDemandElement* secondDemandElementParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag) :
    GUIGlObject(type, firstDemandElementParent->generateDemandElementChildID(tag)),
    GNEAttributeCarrier(tag),
    Parameterised(),
    myViewNet(viewNet),
    myFirstDemandElementParent(firstDemandElementParent),
    mySecondDemandElementParent(secondDemandElementParent),
    myChildConnections(this) {
    // check that demand element parent is of expected type
    assert(firstDemandElementParent->getTagProperty().getTag() == myTagProperty.getParentTag());
}


GNEDemandElement::GNEDemandElement(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::vector<GNEEdge*> edgeChilds) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    Parameterised(),
    myViewNet(viewNet),
    myFirstDemandElementParent(nullptr),
    mySecondDemandElementParent(nullptr),
    myEdgeChilds(edgeChilds),
    myChildConnections(this) {
}


GNEDemandElement::GNEDemandElement(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::vector<GNELane*> laneChilds) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    Parameterised(),
    myViewNet(viewNet),
    myFirstDemandElementParent(nullptr),
    mySecondDemandElementParent(nullptr),
    myLaneChilds(laneChilds),
    myChildConnections(this) {
}


GNEDemandElement::~GNEDemandElement() {}


bool
GNEDemandElement::isDemandElementValid() const {
    return true;
}


std::string
GNEDemandElement::getDemandElementProblem() const {
    return "";
}


void
GNEDemandElement::fixDemandElementProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


void
GNEDemandElement::openDemandElementDialog() {
    throw InvalidArgument(getTagStr() + " doesn't have an demand element dialog");
}


void
GNEDemandElement::startGeometryMoving() {
    // always save original position over view
    myMove.originalViewPosition = getPositionInView();
    // obtain tag properties (to improve code legibility)
    const TagProperties& tagProperties = myTagProperty;
    // check if position over lane or lanes has to be saved
    if (tagProperties.canBePlacedOverLane()) {
        if (tagProperties.canMaskStartEndPos()) {
            // obtain start and end position
            myMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_STARTPOS);
            myMove.secondOriginalPosition = getAttribute(SUMO_ATTR_ENDPOS);
        } else {
            // obtain position attribute
            myMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_POSITION);
        }
    } else if (tagProperties.canBePlacedOverLanes()) {
        // obtain start and end position
        myMove.firstOriginalLanePosition = getAttribute(SUMO_ATTR_POSITION);
        myMove.secondOriginalPosition = getAttribute(SUMO_ATTR_ENDPOS);
    }
    // save current centering boundary
    myMove.movingGeometryBoundary = getCenteringBoundary();
}


void
GNEDemandElement::endGeometryMoving() {
    // check that endGeometryMoving was called only once
    if (myMove.movingGeometryBoundary.isInitialised()) {
        // Remove object from net
        myViewNet->getNet()->removeGLObjectFromGrid(this);
        // reset myMovingGeometryBoundary
        myMove.movingGeometryBoundary.reset();
        // update geometry without updating grid
        updateGeometry(false);
        // add object into grid again (using the new centering boundary)
        myViewNet->getNet()->addGLObjectIntoGrid(this);
    }
}


GNEViewNet*
GNEDemandElement::getViewNet() const {
    return myViewNet;
}


PositionVector
GNEDemandElement::getShape() const {
    return myGeometry.shape;
}


GNEDemandElement*
GNEDemandElement::getFirstDemandElementParent() const {
    return myFirstDemandElementParent;
}


GNEDemandElement*
GNEDemandElement::getSecondDemandElementParent() const {
    return mySecondDemandElementParent;
}


std::string
GNEDemandElement::generateDemandElementChildID(SumoXMLTag childTag) {
    int counter = 0;
    while (myViewNet->getNet()->retrieveDemandElement(childTag, getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


void
GNEDemandElement::addDemandElementChild(GNEDemandElement* demandElementName) {
    // First check that demand element wasn't already inserted
    if (std::find(myDemandElementChilds.begin(), myDemandElementChilds.end(), demandElementName) != myDemandElementChilds.end()) {
        throw ProcessError(demandElementName->getTagStr() + " with ID='" + demandElementName->getID() + "' was already inserted in " + getTagStr() + " with ID='" + getID() + "'");
    } else {
        myDemandElementChilds.push_back(demandElementName);
        // Check if childs has to be sorted automatically
        if (myTagProperty.canAutomaticSortChilds()) {
            sortDemandElementChilds();
        }
        // update demand element parent after add demand element (note: by default non-implemented)
        updateDemandElementParent();
        // update geometry (for set geometry of lines between Parents and Childs)
        updateGeometry(true);
    }
}


void
GNEDemandElement::removeDemandElementChild(GNEDemandElement* demandElementName) {
    // First check that demand element was already inserted
    auto it = std::find(myDemandElementChilds.begin(), myDemandElementChilds.end(), demandElementName);
    if (it == myDemandElementChilds.end()) {
        throw ProcessError(demandElementName->getTagStr() + " with ID='" + demandElementName->getID() + "' doesn't exist in " + getTagStr() + " with ID='" + getID() + "'");
    } else {
        myDemandElementChilds.erase(it);
        // Check if childs has to be sorted automatically
        if (myTagProperty.canAutomaticSortChilds()) {
            sortDemandElementChilds();
        }
        // update demand element parent after add demand element (note: by default non-implemented)
        updateDemandElementParent();
        // update geometry (for remove geometry of lines between Parents and Childs)
        updateGeometry(true);
    }
}


const std::vector<GNEDemandElement*>&
GNEDemandElement::getDemandElementChilds() const {
    return myDemandElementChilds;
}


void
GNEDemandElement::sortDemandElementChilds() {
    if (myTagProperty.getTag() == SUMO_TAG_E3DETECTOR) {
        // we need to sort Entry/Exits due demand element.xds model
        std::vector<GNEDemandElement*> sortedEntryExits;
        // obtain all entrys
        for (auto i : myDemandElementChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
                sortedEntryExits.push_back(i);
            }
        }
        // obtain all exits
        for (auto i : myDemandElementChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
                sortedEntryExits.push_back(i);
            }
        }
        // change myDemandElementChilds for sortedEntryExits
        if (sortedEntryExits.size() == myDemandElementChilds.size()) {
            myDemandElementChilds = sortedEntryExits;
        } else {
            throw ProcessError("Some demand element childs were lost during sorting");
        }
    } else if (myTagProperty.getTag() == SUMO_TAG_TAZ) {
        // we need to sort Entry/Exits due demand element.xds model
        std::vector<GNEDemandElement*> sortedTAZSourceSink;
        // obtain all TAZSources
        for (auto i : myDemandElementChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                sortedTAZSourceSink.push_back(i);
            }
        }
        // obtain all TAZSinks
        for (auto i : myDemandElementChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
                sortedTAZSourceSink.push_back(i);
            }
        }
        // change myDemandElementChilds for sortedEntryExits
        if (sortedTAZSourceSink.size() == myDemandElementChilds.size()) {
            myDemandElementChilds = sortedTAZSourceSink;
        } else {
            throw ProcessError("Some demand element childs were lost during sorting");
        }
    } else {
        // declare a vector to keep sorted childs
        std::vector<std::pair<std::pair<double, double>, GNEDemandElement*> > sortedChilds;
        // iterate over demand element childs
        for (auto i : myDemandElementChilds) {
            sortedChilds.push_back(std::make_pair(std::make_pair(0., 0.), i));
            // set begin/start attribute
            if (i->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
                sortedChilds.back().first.first = parse<double>(i->getAttribute(SUMO_ATTR_TIME));
            } else if (i->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
                sortedChilds.back().first.first = parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
            }
            // set end attribute
            if (i->getTagProperty().hasAttribute(SUMO_ATTR_END) && canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
                sortedChilds.back().first.second = parse<double>(i->getAttribute(SUMO_ATTR_END));
            } else {
                sortedChilds.back().first.second = sortedChilds.back().first.first;
            }
        }
        // sort childs
        std::sort(sortedChilds.begin(), sortedChilds.end());
        // make sure that number of sorted childs is the same as the demand element childs
        if (sortedChilds.size() == myDemandElementChilds.size()) {
            myDemandElementChilds.clear();
            for (auto i : sortedChilds) {
                myDemandElementChilds.push_back(i.second);
            }
        } else {
            throw ProcessError("Some demand element childs were lost during sorting");
        }
    }
}


bool
GNEDemandElement::checkDemandElementChildsOverlapping() const {
    // declare a vector to keep sorted childs
    std::vector<std::pair<std::pair<double, double>, GNEDemandElement*> > sortedChilds;
    // iterate over demand element childs
    for (auto i : myDemandElementChilds) {
        sortedChilds.push_back(std::make_pair(std::make_pair(0., 0.), i));
        // set begin/start attribute
        if (i->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
            sortedChilds.back().first.first = parse<double>(i->getAttribute(SUMO_ATTR_TIME));
        } else if (i->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
            sortedChilds.back().first.first = parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
        }
        // set end attribute
        if (i->getTagProperty().hasAttribute(SUMO_ATTR_END) && canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
            sortedChilds.back().first.second = parse<double>(i->getAttribute(SUMO_ATTR_END));
        } else {
            sortedChilds.back().first.second = sortedChilds.back().first.first;
        }
    }
    // sort childs
    std::sort(sortedChilds.begin(), sortedChilds.end());
    // make sure that number of sorted childs is the same as the demand element childs
    if (sortedChilds.size() == myDemandElementChilds.size()) {
        if (sortedChilds.size() <= 1) {
            return true;
        } else {
            // check overlapping
            for (int i = 0; i < (int)sortedChilds.size() - 1; i++) {
                if (sortedChilds.at(i).first.second > sortedChilds.at(i + 1).first.first) {
                    return false;
                }
            }
        }
        return true;
    } else {
        throw ProcessError("Some demand element childs were lost during sorting");
    }
}


void
GNEDemandElement::addEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " child in " + getTagStr() + " with ID='" + getID() + "'");
    } else if (std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge) != myEdgeChilds.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " child in " + getTagStr() + " with ID='" + getID() + "'");
    } else {
        myEdgeChilds.push_back(edge);
    }
}


void
GNEDemandElement::removeEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " child in " + getTagStr() + " with ID='" + getID() + "'");
    } else if (std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge) == myEdgeChilds.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " child in " + getTagStr() + " with ID='" + getID() + "'");
    } else {
        myEdgeChilds.erase(std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge));
    }
}


const std::vector<GNEEdge*>&
GNEDemandElement::getEdgeChilds() const {
    return myEdgeChilds;
}


void
GNEDemandElement::addLaneChild(GNELane* lane) {
    // Check that lane is valid and doesn't exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " child in " + getTagStr() + " with ID='" + getID() + "'");
    } else if (std::find(myLaneChilds.begin(), myLaneChilds.end(), lane) != myLaneChilds.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " child in " + getTagStr() + " with ID='" + getID() + "'");
    } else {
        myLaneChilds.push_back(lane);
    }
}


void
GNEDemandElement::removeLaneChild(GNELane* lane) {
    // Check that lane is valid and exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " child in " + getTagStr() + " with ID='" + getID() + "'");
    } else if (std::find(myLaneChilds.begin(), myLaneChilds.end(), lane) == myLaneChilds.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " child in " + getTagStr() + " with ID='" + getID() + "'");
    } else {
        myLaneChilds.erase(std::find(myLaneChilds.begin(), myLaneChilds.end(), lane));
    }
}


const std::vector<GNELane*>&
GNEDemandElement::getLaneChilds() const {
    return myLaneChilds;
}


GUIGLObjectPopupMenu*
GNEDemandElement::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
    // Show position parameters
    if (myTagProperty.hasAttribute(SUMO_ATTR_LANE)) {
        GNELane* lane = myViewNet->getNet()->retrieveLane(getAttribute(SUMO_ATTR_LANE));
        // Show menu command inner position
        const double innerPos = myGeometry.shape.nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position inner demand element: " + toString(innerPos)).c_str(), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command lane position
        if (myGeometry.shape.size() > 0) {
            const double lanePos = lane->getShape().nearest_offset_to_point2D(myGeometry.shape[0]);
            new FXMenuCommand(ret, ("Cursor position over " + toString(SUMO_TAG_LANE) + ": " + toString(innerPos + lanePos)).c_str(), nullptr, nullptr, 0);
        }
    } else if (myTagProperty.hasAttribute(SUMO_ATTR_EDGE)) {
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(getAttribute(SUMO_ATTR_EDGE));
        // Show menu command inner position
        const double innerPos = myGeometry.shape.nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position inner demand element: " + toString(innerPos)).c_str(), nullptr, nullptr, 0);
        // If shape isn't empty, show menu command edge position
        if (myGeometry.shape.size() > 0) {
            const double edgePos = edge->getLanes().at(0)->getShape().nearest_offset_to_point2D(myGeometry.shape[0]);
            new FXMenuCommand(ret, ("Mouse position over " + toString(SUMO_TAG_EDGE) + ": " + toString(innerPos + edgePos)).c_str(), nullptr, nullptr, 0);
        }
    } else {
        new FXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), nullptr, nullptr, 0);
    }
    return ret;
}


GUIParameterTableWindow*
GNEDemandElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, myTagProperty.getNumberOfAttributes());
    // Iterate over attributes
    for (const auto& i : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (i.second.isUnique()) {
            ret->mkItem(toString(i.first).c_str(), false, getAttribute(i.first));
        } else {
            ret->mkItem(toString(i.first).c_str(), true, getAttribute(i.first));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNEDemandElement::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMove.movingGeometryBoundary.isInitialised()) {
        return myMove.movingGeometryBoundary;
    } else if (myGeometry.shape.size() > 0) {
        Boundary b = myGeometry.shape.getBoxBoundary();
        b.grow(20);
        return b;
    } else if (myGeometry.multiShape.size() > 0) {
        // obtain boundary of multishape fixed
        Boundary b = myGeometry.multiShapeUnified.getBoxBoundary();
        b.grow(20);
        return b;
    } else if (myFirstDemandElementParent) {
        return myFirstDemandElementParent->getCenteringBoundary();
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


bool
GNEDemandElement::isRouteValid(const std::vector<GNEEdge*>& edges, bool report) {
    if (edges.size() == 0) {
        // routes cannot be empty
        return false;
    } else if (edges.size() == 1) {
        // routes with a single edge are valid
        return true;
    } else {
        // iterate over edges to check that compounds a chain
        auto it = edges.begin();
        while (it != edges.end() - 1) {
            GNEEdge* currentEdge = *it;
            GNEEdge* nextEdge = *(it + 1);
            // consecutive edges aren't allowed
            if (currentEdge->getID() == nextEdge->getID()) {
                return false;
            }
            // make sure that edges are consecutives
            if (std::find(currentEdge->getGNEJunctionDestiny()->getGNEOutgoingEdges().begin(),
                          currentEdge->getGNEJunctionDestiny()->getGNEOutgoingEdges().end(),
                          nextEdge) == currentEdge->getGNEJunctionDestiny()->getGNEOutgoingEdges().end()) {
                if (report) {
                    WRITE_WARNING("Parameter 'Route' invalid. " + currentEdge->getTagStr() + " '" + currentEdge->getID() +
                                  "' ins't consecutive to " + nextEdge->getTagStr() + " '" + nextEdge->getID() + "'");
                }
                return false;
            }
            it++;
        }
    }
    return true;
}


void
GNEDemandElement::updateDemandElementParent() {
    // by default nothing to do
}


GNEDemandElement::DemandElementGeometry::DemandElementGeometry() {}


void
GNEDemandElement::DemandElementGeometry::clearGeometry() {
    shape.clear();
    multiShape.clear();
    shapeRotations.clear();
    shapeLengths.clear();
    multiShapeRotations.clear();
    multiShapeLengths.clear();
    multiShapeUnified.clear();
}


void
GNEDemandElement::DemandElementGeometry::calculateMultiShapeUnified() {
    // merge all multishape parts in a single shape
    for (auto i : multiShape) {
        multiShapeUnified.append(i);
    }
}


void
GNEDemandElement::DemandElementGeometry::calculateShapeRotationsAndLengths() {
    // Get number of parts of the shape
    int numberOfSegments = (int)shape.size() - 1;
    // If number of segments is more than 0
    if (numberOfSegments >= 0) {
        // Reserve memory (To improve efficiency)
        shapeRotations.reserve(numberOfSegments);
        shapeLengths.reserve(numberOfSegments);
        // For every part of the shape
        for (int i = 0; i < numberOfSegments; ++i) {
            // Obtain first position
            const Position& f = shape[i];
            // Obtain next position
            const Position& s = shape[i + 1];
            // Save distance between position into myShapeLengths
            shapeLengths.push_back(f.distanceTo(s));
            // Save rotation (angle) of the vector constructed by points f and s
            shapeRotations.push_back((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }
}


void
GNEDemandElement::DemandElementGeometry::calculateMultiShapeRotationsAndLengths() {
    // Get number of parts of the shape for every part shape
    std::vector<int> numberOfSegments;
    for (auto i : multiShape) {
        // numseg cannot be 0
        int numSeg = (int)i.size() - 1;
        numberOfSegments.push_back((numSeg >= 0) ? numSeg : 0);
        multiShapeRotations.push_back(std::vector<double>());
        multiShapeLengths.push_back(std::vector<double>());
    }
    // If number of segments is more than 0
    for (int i = 0; i < (int)multiShape.size(); i++) {
        // Reserve size for every part
        multiShapeRotations.back().reserve(numberOfSegments.at(i));
        multiShapeLengths.back().reserve(numberOfSegments.at(i));
        // iterate over each segment
        for (int j = 0; j < numberOfSegments.at(i); j++) {
            // Obtain first position
            const Position& f = multiShape[i][j];
            // Obtain next position
            const Position& s = multiShape[i][j + 1];
            // Save distance between position into myShapeLengths
            multiShapeLengths.at(i).push_back(f.distanceTo(s));
            // Save rotation (angle) of the vector constructed by points f and s
            multiShapeRotations.at(i).push_back((double)atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
        }
    }
}


GNEDemandElement::ChildConnections::ChildConnections(GNEDemandElement* demandElementName) :
    myDemandElement(demandElementName) {}


void
GNEDemandElement::ChildConnections::update() {
    // first clear connection positions
    connectionPositions.clear();
    symbolsPositionAndRotation.clear();

    // calculate position and rotation of every simbol for every edge
    for (auto i : myDemandElement->myEdgeChilds) {
        for (auto j : i->getLanes()) {
            std::pair<Position, double> posRot;
            // set position and lenght depending of shape's lengt
            if (j->getShape().length() - 6 > 0) {
                posRot.first = j->getShape().positionAtOffset(j->getShape().length() - 6);
                posRot.second = j->getShape().rotationDegreeAtOffset(j->getShape().length() - 6);
            } else {
                posRot.first = j->getShape().positionAtOffset(j->getShape().length());
                posRot.second = j->getShape().rotationDegreeAtOffset(j->getShape().length());
            }
            symbolsPositionAndRotation.push_back(posRot);
        }
    }

    // calculate position and rotation of every symbol for every lane
    for (auto i : myDemandElement->myLaneChilds) {
        std::pair<Position, double> posRot;
        // set position and lenght depending of shape's lengt
        if (i->getShape().length() - 6 > 0) {
            posRot.first = i->getShape().positionAtOffset(i->getShape().length() - 6);
            posRot.second = i->getShape().rotationDegreeAtOffset(i->getShape().length() - 6);
        } else {
            posRot.first = i->getShape().positionAtOffset(i->getShape().length());
            posRot.second = i->getShape().rotationDegreeAtOffset(i->getShape().length());
        }
        symbolsPositionAndRotation.push_back(posRot);
    }

    // calculate position for every demand element child
    for (auto i : myDemandElement->myDemandElementChilds) {
        // check that position is different of position
        if (i->getPositionInView() != myDemandElement->getPositionInView()) {
            std::vector<Position> posConnection;
            double A = std::abs(i->getPositionInView().x() - myDemandElement->getPositionInView().x());
            double B = std::abs(i->getPositionInView().y() - myDemandElement->getPositionInView().y());
            // Set positions of connection's vertex. Connection is build from Entry to E3
            posConnection.push_back(i->getPositionInView());
            if (myDemandElement->getPositionInView().x() > i->getPositionInView().x()) {
                if (myDemandElement->getPositionInView().y() > i->getPositionInView().y()) {
                    posConnection.push_back(Position(i->getPositionInView().x() + A, i->getPositionInView().y()));
                } else {
                    posConnection.push_back(Position(i->getPositionInView().x(), i->getPositionInView().y() - B));
                }
            } else {
                if (myDemandElement->getPositionInView().y() > i->getPositionInView().y()) {
                    posConnection.push_back(Position(i->getPositionInView().x(), i->getPositionInView().y() + B));
                } else {
                    posConnection.push_back(Position(i->getPositionInView().x() - A, i->getPositionInView().y()));
                }
            }
            posConnection.push_back(myDemandElement->getPositionInView());
            connectionPositions.push_back(posConnection);
        }
    }

    // calculate geometry for connections between parent and childs
    for (auto i : symbolsPositionAndRotation) {
        std::vector<Position> posConnection;
        double A = std::abs(i.first.x() - myDemandElement->getPositionInView().x());
        double B = std::abs(i.first.y() - myDemandElement->getPositionInView().y());
        // Set positions of connection's vertex. Connection is build from Entry to E3
        posConnection.push_back(i.first);
        if (myDemandElement->getPositionInView().x() > i.first.x()) {
            if (myDemandElement->getPositionInView().y() > i.first.y()) {
                posConnection.push_back(Position(i.first.x() + A, i.first.y()));
            } else {
                posConnection.push_back(Position(i.first.x(), i.first.y() - B));
            }
        } else {
            if (myDemandElement->getPositionInView().y() > i.first.y()) {
                posConnection.push_back(Position(i.first.x(), i.first.y() + B));
            } else {
                posConnection.push_back(Position(i.first.x() - A, i.first.y()));
            }
        }
        posConnection.push_back(myDemandElement->getPositionInView());
        connectionPositions.push_back(posConnection);
    }
}


void
GNEDemandElement::ChildConnections::draw() const {
    // Iterate over myConnectionPositions
    for (auto i : connectionPositions) {
        // Add a draw matrix
        glPushMatrix();
        // traslate in the Z axis
        glTranslated(0, 0, myDemandElement->getType() - 0.01);
        // Set color of the base
        GLHelper::setColor(RGBColor(255, 235, 0));
        for (auto j = i.begin(); (j + 1) != i.end(); j++) {
            // Draw Lines
            GLHelper::drawLine((*j), (*(j + 1)));
        }
        // Pop draw matrix
        glPopMatrix();
    }
}


const std::string&
GNEDemandElement::getDemandElementID() const {
    return getMicrosimID();
}


bool
GNEDemandElement::isValidDemandElementID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidNetID(newID) && (myViewNet->getNet()->retrieveDemandElement(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


bool
GNEDemandElement::isValidDetectorID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidDetectorID(newID) && (myViewNet->getNet()->retrieveDemandElement(myTagProperty.getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


void
GNEDemandElement::changeDemandElementID(const std::string& newID) {
    if (myViewNet->getNet()->retrieveDemandElement(myTagProperty.getTag(), newID, false) != nullptr) {
        throw InvalidArgument("An DemandElement with tag " + getTagStr() + " and ID = " + newID + " already exists");
    } else {
        // Save old ID
        std::string oldID = getMicrosimID();
        // set New ID
        setMicrosimID(newID);
        // update demand element ID in the container of net
        myViewNet->getNet()->updateDemandElementID(oldID, this);
    }
}


GNEEdge*
GNEDemandElement::changeEdge(GNEEdge* oldEdge, const std::string& newEdgeID) {
    if (oldEdge == nullptr) {
        throw InvalidArgument(getTagStr() + " with ID '" + getMicrosimID() + "' doesn't belong to an " + toString(SUMO_TAG_EDGE));
    } else {
        oldEdge->removeDemandElementChild(this);
        GNEEdge* newEdge = myViewNet->getNet()->retrieveEdge(newEdgeID);
        newEdge->addDemandElementChild(this);
        updateGeometry(true);
        return newEdge;
    }
}


GNELane*
GNEDemandElement::changeLane(GNELane* oldLane, const std::string& newLaneID) {
    if (oldLane == nullptr) {
        throw InvalidArgument(getTagStr() + " with ID '" + getMicrosimID() + "' doesn't belong to a " + toString(SUMO_TAG_LANE));
    } else {
        oldLane->removeDemandElementChild(this);
        GNELane* newLane = myViewNet->getNet()->retrieveLane(newLaneID);
        newLane->addDemandElementChild(this);
        updateGeometry(true);
        return newLane;
    }
}


void
GNEDemandElement::changeFirstDemandElementParent(const std::string& newDemandElementParentID) {
    if (myFirstDemandElementParent == nullptr) {
        throw InvalidArgument(getTagStr() + " with ID '" + getMicrosimID() + "' doesn't have an demand element parent");
    } else {
        // remove this demand element of the childs of parent demand element
        myFirstDemandElementParent->removeDemandElementChild(this);
        // set new demand element parent
        myFirstDemandElementParent = myViewNet->getNet()->retrieveDemandElement(myFirstDemandElementParent->getTagProperty().getTag(), newDemandElementParentID);
        // add this demand element int the childs of parent demand element
        myFirstDemandElementParent->addDemandElementChild(this);
        updateGeometry(true);
    }
}


void
GNEDemandElement::changeSecondDemandElementParent(const std::string& newDemandElementParentID) {
    if (mySecondDemandElementParent == nullptr) {
        throw InvalidArgument(getTagStr() + " with ID '" + getMicrosimID() + "' doesn't have an demand element parent");
    } else {
        // remove this demand element of the childs of parent demand element
        mySecondDemandElementParent->removeDemandElementChild(this);
        // set new demand element parent
        mySecondDemandElementParent = myViewNet->getNet()->retrieveDemandElement(mySecondDemandElementParent->getTagProperty().getTag(), newDemandElementParentID);
        // add this demand element int the childs of parent demand element
        mySecondDemandElementParent->addDemandElementChild(this);
        updateGeometry(true);
    }
}


bool
GNEDemandElement::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEDemandElement::drawUsingSelectColor() const {
    if (mySelected && (myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND)) {
        return true;
    } else {
        return false;
    }
}


bool
GNEDemandElement::checkDemandElementChildRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError("Calling non-implemented function checkDemandElementChildRestriction during saving of " + getTagStr() + ". It muss be reimplemented in child class");
}


std::string
GNEDemandElement::getGenericParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (auto i : getParametersMap()) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEDemandElement::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEDemandElement::setGenericParametersStr(const std::string& value) {
    // clear parameters
    clearParameter();
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
            setParameter(parsedParameters.front(), parsedParameters.back());
        }
    }
}

/****************************************************************************/
