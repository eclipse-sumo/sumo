/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// A abstract class for representation of additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <iostream>
#include <utility>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>

#include "GNEAdditional.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    Parameterised(),
    myViewNet(viewNet),
    myAdditionalName(additionalName),
    myBlockMovement(blockMovement),
    myFirstAdditionalParent(nullptr),
    mySecondAdditionalParent(nullptr),
    myBlockIconRotation(0.) {
}


GNEAdditional::GNEAdditional(GNEAdditional* singleAdditionalParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement) :
    GUIGlObject(type, singleAdditionalParent->generateAdditionalChildID(tag)),
    GNEAttributeCarrier(tag),
    Parameterised(),
    myViewNet(viewNet),
    myAdditionalName(additionalName),
    myBlockMovement(blockMovement),
    myFirstAdditionalParent(singleAdditionalParent),
    mySecondAdditionalParent(nullptr),
    myBlockIconRotation(0.) {
    // check that additional parent is of expected type
    assert(singleAdditionalParent->getTag() == getTagProperties(getTag()).getParentTag());
}


GNEAdditional::GNEAdditional(GNEAdditional* firstAdditionalParent, GNEAdditional* secondAdditionalParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement) :
    GUIGlObject(type, firstAdditionalParent->generateAdditionalChildID(tag)),
    GNEAttributeCarrier(tag),
    Parameterised(),
    myViewNet(viewNet),
    myAdditionalName(additionalName),
    myBlockMovement(blockMovement),
    myFirstAdditionalParent(firstAdditionalParent),
    mySecondAdditionalParent(secondAdditionalParent),
    myBlockIconRotation(0.) {
    // check that additional parent is of expected type
    assert(firstAdditionalParent->getTag() == getTagProperties(getTag()).getParentTag());
}


GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement, std::vector<GNEEdge*> edgeChilds) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    Parameterised(),
    myViewNet(viewNet),
    myAdditionalName(additionalName),
    myBlockMovement(blockMovement),
    myFirstAdditionalParent(nullptr),
    mySecondAdditionalParent(nullptr),
    myEdgeChilds(edgeChilds),
    myBlockIconRotation(0.) {
}


GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag, std::string additionalName, bool blockMovement, std::vector<GNELane*> laneChilds) :
    GUIGlObject(type, id),
    GNEAttributeCarrier(tag),
    Parameterised(),
    myViewNet(viewNet),
    myAdditionalName(additionalName),
    myBlockMovement(blockMovement),
    myFirstAdditionalParent(nullptr),
    mySecondAdditionalParent(nullptr),
    myLaneChilds(laneChilds),
    myBlockIconRotation(0.) {
}


GNEAdditional::~GNEAdditional() {}


void
GNEAdditional::writeAdditional(OutputDevice& device) const {
    // obtain tag properties
    const TagValues& tagProperties = getTagProperties(getTag());
    // first check if minimum number of childs is correct
    if ((tagProperties.hasMinimumNumberOfChilds() || tagProperties.hasMinimumNumberOfChilds()) && !checkAdditionalChildRestriction()) {
        WRITE_WARNING(toString(getTag()) + " with ID='" + getID() + "' cannot be written");
    } else {
        // Open Tag or synonim Tag
        if (tagProperties.hasTagSynonym()) {
            device.openTag(tagProperties.getTagSynonym());
        } else {
            device.openTag(getTag());
        }
        // iterate over attributes and write it
        for (auto i : tagProperties) {
            // obtain attribute
            std::string attribute = getAttribute(i.first);
            if (i.second.isOptional() && i.second.hasDefaultValue() && !i.second.isCombinable()) {
                // Only write attributes with default value if is different of original
                if (i.second.getDefaultValue() != attribute) {
                    // check if attribute must be written using a synonim
                    if (i.second.hasAttrSynonym()) {
                        device.writeAttr(i.second.getAttrSynonym(), attribute);
                    } else {
                        // SVC permissions uses their own writting function
                        if (i.second.isSVCPermission()) {
                            // disallow attribute musn't be written
                            if (i.first != SUMO_ATTR_DISALLOW) {
                                writePermissions(device, parseVehicleClasses(attribute));
                            }
                        } else {
                            device.writeAttr(i.first, attribute);
                        }
                    }
                }
            } else {
                // Attributes without default values are always writted
                if (i.second.hasAttrSynonym()) {
                    device.writeAttr(i.second.getAttrSynonym(), attribute);
                } else {
                    // SVC permissions uses their own writting function
                    if (i.second.isSVCPermission()) {
                        // disallow attribute musn't be written
                        if (i.first != SUMO_ATTR_DISALLOW) {
                            writePermissions(device, parseVehicleClasses(attribute));
                        }
                    } else {
                        device.writeAttr(i.first, attribute);
                    }
                }
            }
        }
        // save generic parameters
        writeParams(device);
        // iterate over childs and write it in XML (or in a different file)
        if (tagProperties.canWriteChildsSeparate() && tagProperties.hasAttribute(SUMO_ATTR_FILE) && !getAttribute(SUMO_ATTR_FILE).empty()) {
            // we assume that rerouter values files is placed in the same folder as the additional file
            OutputDevice& deviceChilds = OutputDevice::getDevice(FileHelpers::getFilePath(OptionsCont::getOptions().getString("sumo-additionals-file")) + getAttribute(SUMO_ATTR_FILE));
            deviceChilds.writeXMLHeader("rerouterValue", "additional_file.xsd");
            // save childs in a different filename
            for (auto i : myAdditionalChilds) {
                // avoid to write two times additionals that haben two parents (Only write as child of first parent)
                if (i->getSecondAdditionalParent() == nullptr) {
                    i->writeAdditional(deviceChilds);
                } else if (getTag() == getTagProperties(i->getTag()).getParentTag()) {
                    i->writeAdditional(deviceChilds);
                }
            }
            deviceChilds.close();
        } else {
            for (auto i : myAdditionalChilds) {
                // avoid to write two times additionals that haben two parents (Only write as child of first parent)
                if (i->getSecondAdditionalParent() == nullptr) {
                    i->writeAdditional(device);
                } else if (getTag() == getTagProperties(i->getTag()).getParentTag()) {
                    i->writeAdditional(device);
                }
            }
        }
        // Close tag
        device.closeTag();
    }
}


void
GNEAdditional::openAdditionalDialog() {
    throw InvalidArgument(toString(getTag()) + " doesn't have an additional dialog");
}


void
GNEAdditional::startGeometryMoving() {
    // save current centering boundary
    myMovingGeometryBoundary = getCenteringBoundary();
}


void
GNEAdditional::endGeometryMoving() {
    // check that endGeometryMoving was called only once
    if (myMovingGeometryBoundary.isInitialised()) {
        // Remove object from net
        myViewNet->getNet()->removeGLObjectFromGrid(this);
        // reset myMovingGeometryBoundary
        myMovingGeometryBoundary.reset();
        // update geometry without updating grid
        updateGeometry(false);
        // add object into grid again (using the new centering boundary)
        myViewNet->getNet()->addGLObjectIntoGrid(this);
    }
}


GNEViewNet*
GNEAdditional::getViewNet() const {
    return myViewNet;
}


PositionVector
GNEAdditional::getShape() const {
    return myShape;
}


bool
GNEAdditional::isAdditionalBlocked() const {
    return myBlockMovement;
}


GNEAdditional*
GNEAdditional::getFirstAdditionalParent() const {
    return myFirstAdditionalParent;
}


GNEAdditional*
GNEAdditional::getSecondAdditionalParent() const {
    return mySecondAdditionalParent;
}


std::string
GNEAdditional::generateAdditionalChildID(SumoXMLTag childTag) {
    int counter = 0;
    while (myViewNet->getNet()->retrieveAdditional(childTag, getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


void
GNEAdditional::addAdditionalChild(GNEAdditional* additional) {
    // First check that additional wasn't already inserted
    if (std::find(myAdditionalChilds.begin(), myAdditionalChilds.end(), additional) != myAdditionalChilds.end()) {
        throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' was already inserted in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myAdditionalChilds.push_back(additional);
        // Check if childs has to be sorted automatically
        if (getTagProperties(getTag()).canAutomaticSortChilds()) {
            sortAdditionalChilds();
        }
        // update geometry (for set geometry of lines between Parents and Childs)
        updateGeometry(true);
    }
}


void
GNEAdditional::removeAdditionalChild(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myAdditionalChilds.begin(), myAdditionalChilds.end(), additional);
    if (it == myAdditionalChilds.end()) {
        throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' doesn't exist in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myAdditionalChilds.erase(it);
        // Check if childs has to be sorted automatically
        if (getTagProperties(getTag()).canAutomaticSortChilds()) {
            sortAdditionalChilds();
        }
        // update geometry (for remove geometry of lines between Parents and Childs)
        updateGeometry(true);
    }
}


const std::vector<GNEAdditional*>&
GNEAdditional::getAdditionalChilds() const {
    return myAdditionalChilds;
}


void
GNEAdditional::sortAdditionalChilds() {
    if (getTag() == SUMO_TAG_E3DETECTOR) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedEntryExits;
        // obtain all entrys
        for (auto i : myAdditionalChilds) {
            if (i->getTag() == SUMO_TAG_DET_ENTRY) {
                sortedEntryExits.push_back(i);
            }
        }
        // obtain all exits
        for (auto i : myAdditionalChilds) {
            if (i->getTag() == SUMO_TAG_DET_EXIT) {
                sortedEntryExits.push_back(i);
            }
        }
        // change myAdditionalChilds for sortedEntryExits
        if (sortedEntryExits.size() == myAdditionalChilds.size()) {
            myAdditionalChilds = sortedEntryExits;
        } else {
            throw ProcessError("Some additional childs were lost during sorting");
        }
    } else {
        // declare a vector to keep sorted childs
        std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChilds;
        // iterate over additional childs
        for (auto i : myAdditionalChilds) {
            sortedChilds.push_back(std::make_pair(std::make_pair(0., 0.), i));
            // set begin/start attribute
            if (getTagProperties(i->getTag()).hasAttribute(SUMO_ATTR_TIME) && canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
                sortedChilds.back().first.first = parse<double>(i->getAttribute(SUMO_ATTR_TIME));
            } else if (getTagProperties(i->getTag()).hasAttribute(SUMO_ATTR_BEGIN) && canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
                sortedChilds.back().first.first = parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
            }
            // set end attribute
            if (getTagProperties(i->getTag()).hasAttribute(SUMO_ATTR_END) && canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
                sortedChilds.back().first.second = parse<double>(i->getAttribute(SUMO_ATTR_END));
            } else {
                sortedChilds.back().first.second = sortedChilds.back().first.first;
            }
        }
        // sort childs
        std::sort(sortedChilds.begin(), sortedChilds.end());
        // make sure that number of sorted childs is the same as the additional childs
        if (sortedChilds.size() == myAdditionalChilds.size()) {
            myAdditionalChilds.clear();
            for (auto i : sortedChilds) {
                myAdditionalChilds.push_back(i.second);
            }
        } else {
            throw ProcessError("Some additional childs were lost during sorting");
        }
    }
}


bool
GNEAdditional::checkAdditionalChildsOverlapping() const {
    // declare a vector to keep sorted childs
    std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChilds;
    // iterate over additional childs
    for (auto i : myAdditionalChilds) {
        sortedChilds.push_back(std::make_pair(std::make_pair(0., 0.), i));
        // set begin/start attribute
        if (getTagProperties(i->getTag()).hasAttribute(SUMO_ATTR_TIME) && canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
            sortedChilds.back().first.first = parse<double>(i->getAttribute(SUMO_ATTR_TIME));
        } else if (getTagProperties(i->getTag()).hasAttribute(SUMO_ATTR_BEGIN) && canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
            sortedChilds.back().first.first = parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
        }
        // set end attribute
        if (getTagProperties(i->getTag()).hasAttribute(SUMO_ATTR_END) && canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
            sortedChilds.back().first.second = parse<double>(i->getAttribute(SUMO_ATTR_END));
        } else {
            sortedChilds.back().first.second = sortedChilds.back().first.first;
        }
    }
    // sort childs
    std::sort(sortedChilds.begin(), sortedChilds.end());
    // make sure that number of sorted childs is the same as the additional childs
    if (sortedChilds.size() == myAdditionalChilds.size()) {
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
        throw ProcessError("Some additional childs were lost during sorting");
    }
}


void
GNEAdditional::addEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " child in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else if (std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge) != myEdgeChilds.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " child in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myEdgeChilds.push_back(edge);
    }
}


void
GNEAdditional::removeEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " child in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else if (std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge) == myEdgeChilds.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " child in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myEdgeChilds.erase(std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge));
    }
}


const std::vector<GNEEdge*>&
GNEAdditional::getEdgeChilds() const {
    return myEdgeChilds;
}


void
GNEAdditional::addLaneChild(GNELane* lane) {
    // Check that lane is valid and doesn't exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " child in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else if (std::find(myLaneChilds.begin(), myLaneChilds.end(), lane) != myLaneChilds.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " child in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myLaneChilds.push_back(lane);
    }
}


void
GNEAdditional::removeLaneChild(GNELane* lane) {
    // Check that lane is valid and exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " child in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else if (std::find(myLaneChilds.begin(), myLaneChilds.end(), lane) == myLaneChilds.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " child in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myLaneChilds.erase(std::find(myLaneChilds.begin(), myLaneChilds.end(), lane));
    }
}


const std::vector<GNELane*>&
GNEAdditional::getLaneChilds() const {
    return myLaneChilds;
}


GUIGLObjectPopupMenu*
GNEAdditional::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // obtain tag properties
    const auto& tagProperties = getTagProperties(getTag());
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buld menu commands for names
    new FXMenuCommand(ret, ("Copy " + toString(getTag()) + " name to clipboard").c_str(), 0, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + toString(getTag()) + " typed name to clipboard").c_str(), 0, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myViewNet->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open additional dialog
    if (getTagProperties(getTag()).hasDialog()) {
        new FXMenuCommand(ret, ("Open " + toString(getTag()) + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    // Show position parameters
    if (tagProperties.hasAttribute(SUMO_ATTR_LANE)) {
        GNELane* lane = myViewNet->getNet()->retrieveLane(getAttribute(SUMO_ATTR_LANE));
        // Show menu command inner position
        const double innerPos = myShape.nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position inner additional: " + toString(innerPos)).c_str(), 0, 0, 0);
        // If shape isn't empty, show menu command lane position
        if (myShape.size() > 0) {
            const double lanePos = lane->getShape().nearest_offset_to_point2D(myShape[0]);
            new FXMenuCommand(ret, ("Cursor position over " + toString(SUMO_TAG_LANE) + ": " + toString(innerPos + lanePos)).c_str(), 0, 0, 0);
        }
    } else if (tagProperties.hasAttribute(SUMO_ATTR_EDGE)) {
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(getAttribute(SUMO_ATTR_EDGE));
        // Show menu command inner position
        const double innerPos = myShape.nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position inner additional: " + toString(innerPos)).c_str(), 0, 0, 0);
        // If shape isn't empty, show menu command edge position
        if (myShape.size() > 0) {
            const double edgePos = edge->getLanes().at(0)->getShape().nearest_offset_to_point2D(myShape[0]);
            new FXMenuCommand(ret, ("Mouse position over " + toString(SUMO_TAG_EDGE) + ": " + toString(innerPos + edgePos)).c_str(), 0, 0, 0);
        }
    } else {
        new FXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), 0, 0, 0);
    }
    return ret;
}


GUIParameterTableWindow*
GNEAdditional::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, getTagProperties(getTag()).getNumberOfAttributes());
    // Iterate over attributes
    for (auto i : getTagProperties(getTag())) {
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
GNEAdditional::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMovingGeometryBoundary.isInitialised()) {
        return myMovingGeometryBoundary;
    } else if (myShape.size() > 0) {
        Boundary b = myShape.getBoxBoundary();
        b.grow(20);
        return b;
    } else if (myFirstAdditionalParent) {
        return myFirstAdditionalParent->getCenteringBoundary();
    } else {
        return Boundary(-0.1, 0.1, 0, 1, 0, 1);
    }
}


bool
GNEAdditional::isRouteValid(const std::vector<GNEEdge*>& edges, bool report) {
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
                    WRITE_WARNING("Parameter 'Route' invalid. " + toString(currentEdge->getTag()) + " '" + currentEdge->getID() +
                                  "' ins't consecutive to " + toString(nextEdge->getTag()) + " '" + nextEdge->getID() + "'");
                }
                return false;
            }
            it++;
        }
    }
    return true;
}


void
GNEAdditional::setDefaultValues() {
    // iterate over attributes and set default value
    for (auto i : getTagProperties(getTag())) {
        if (i.second.hasDefaultValue()) {
            setAttribute(i.first, i.second.getDefaultValue());
        }
    }
}


void
GNEAdditional::setBlockIconRotation(GNELane* additionalLane) {
    if (myShape.size() > 0 && myShape.length() != 0) {
        // If length of the shape is distint to 0, Obtain rotation of center of shape
        myBlockIconRotation = myShape.rotationDegreeAtOffset((myShape.length() / 2.)) - 90;
    } else if (additionalLane) {
        // If additional is over a lane, set rotation in the position over lane
        double posOverLane = additionalLane->getShape().nearest_offset_to_point2D(getPositionInView());
        myBlockIconRotation = additionalLane->getShape().rotationDegreeAtOffset(posOverLane) - 90;
    } else {
        // In other case, rotation is 0
        myBlockIconRotation = 0;
    }
}


void
GNEAdditional::drawLockIcon(double size) const {
    if (myViewNet->showLockIcon()) {
        // Start pushing matrix
        glPushMatrix();
        // Traslate to middle of shape
        glTranslated(myBlockIconPosition.x(), myBlockIconPosition.y(), getType() + 0.1);
        // Set draw color
        glColor3d(1, 1, 1);
        // Rotate depending of myBlockIconRotation
        glRotated(myBlockIconRotation, 0, 0, -1);
        // Rotate 180 degrees
        glRotated(180, 0, 0, 1);
        // Traslate depending of the offset
        glTranslated(myBlockIconOffset.x(), myBlockIconOffset.y(), 0);
        // Draw icon depending of the state of additional
        if (mySelected) {
            if (!getTagProperties(getTag()).canBlockMovement()) {
                // Draw not movable texture if additional isn't movable and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_NOTMOVINGSELECTED), size);
            } else if (myBlockMovement) {
                // Draw lock texture if additional is movable, is blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LOCKSELECTED), size);
            } else {
                // Draw empty texture if additional is movable, isn't blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_EMPTYSELECTED), size);
            }
        } else {
            if (!getTagProperties(getTag()).canBlockMovement()) {
                // Draw not movable texture if additional isn't movable
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_NOTMOVING), size);
            } else if (myBlockMovement) {
                // Draw lock texture if additional is movable and is blocked
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LOCK), size);
            } else {
                // Draw empty texture if additional is movable and isn't blocked
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_EMPTY), size);
            }
        }
        // Pop matrix
        glPopMatrix();
    }
}


void
GNEAdditional::updateChildConnections() {
    // first clear connection positions
    myChildConnectionPositions.clear();
    mySymbolsPositionAndRotation.clear();

    // calculate position and rotation of every simbol for every edge
    for (auto i : myEdgeChilds) {
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
            mySymbolsPositionAndRotation.push_back(posRot);
        }
    }

    // calculate position and rotation of every symbol for every lane
    for (auto i : myLaneChilds) {
        std::pair<Position, double> posRot;
        // set position and lenght depending of shape's lengt
        if (i->getShape().length() - 6 > 0) {
            posRot.first = i->getShape().positionAtOffset(i->getShape().length() - 6);
            posRot.second = i->getShape().rotationDegreeAtOffset(i->getShape().length() - 6);
        } else {
            posRot.first = i->getShape().positionAtOffset(i->getShape().length());
            posRot.second = i->getShape().rotationDegreeAtOffset(i->getShape().length());
        }
        mySymbolsPositionAndRotation.push_back(posRot);
    }

    // calculate position for every additional child
    for (auto i : myAdditionalChilds) {
        // check that position is different of position
        if (i->getPositionInView() != getPositionInView()) {
            std::vector<Position> posConnection;
            double A = std::abs(i->getPositionInView().x() - getPositionInView().x());
            double B = std::abs(i->getPositionInView().y() - getPositionInView().y());
            // Set positions of connection's vertex. Connection is build from Entry to E3
            posConnection.push_back(i->getPositionInView());
            if (getPositionInView().x() > i->getPositionInView().x()) {
                if (getPositionInView().y() > i->getPositionInView().y()) {
                    posConnection.push_back(Position(i->getPositionInView().x() + A, i->getPositionInView().y()));
                } else {
                    posConnection.push_back(Position(i->getPositionInView().x(), i->getPositionInView().y() - B));
                }
            } else {
                if (getPositionInView().y() > i->getPositionInView().y()) {
                    posConnection.push_back(Position(i->getPositionInView().x(), i->getPositionInView().y() + B));
                } else {
                    posConnection.push_back(Position(i->getPositionInView().x() - A, i->getPositionInView().y()));
                }
            }
            posConnection.push_back(getPositionInView());
            myChildConnectionPositions.push_back(posConnection);
        }
    }

    // calculate geometry for connections between parent and childs
    for (auto i : mySymbolsPositionAndRotation) {
        std::vector<Position> posConnection;
        double A = std::abs(i.first.x() - getPositionInView().x());
        double B = std::abs(i.first.y() - getPositionInView().y());
        // Set positions of connection's vertex. Connection is build from Entry to E3
        posConnection.push_back(i.first);
        if (getPositionInView().x() > i.first.x()) {
            if (getPositionInView().y() > i.first.y()) {
                posConnection.push_back(Position(i.first.x() + A, i.first.y()));
            } else {
                posConnection.push_back(Position(i.first.x(), i.first.y() - B));
            }
        } else {
            if (getPositionInView().y() > i.first.y()) {
                posConnection.push_back(Position(i.first.x(), i.first.y() + B));
            } else {
                posConnection.push_back(Position(i.first.x() - A, i.first.y()));
            }
        }
        posConnection.push_back(getPositionInView());
        myChildConnectionPositions.push_back(posConnection);
    }
}


void
GNEAdditional::drawChildConnections() const {
    // Iterate over myConnectionPositions
    for (auto i : myChildConnectionPositions) {
        // Add a draw matrix
        glPushMatrix();
        // traslate in the Z axis
        glTranslated(0, 0, getType() - 0.01);
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
GNEAdditional::getAdditionalID() const {
    return getMicrosimID();
}


bool
GNEAdditional::isValidAdditionalID(const std::string& newID) const {
    if (SUMOXMLDefinitions::isValidNetID(newID) && (myViewNet->getNet()->retrieveAdditional(getTag(), newID, false) == nullptr)) {
        return true;
    } else {
        return false;
    }
}


void
GNEAdditional::changeAdditionalID(const std::string& newID) {
    if (myViewNet->getNet()->retrieveAdditional(getTag(), newID, false) != nullptr) {
        throw InvalidArgument("An Additional with tag " + toString(getTag()) + " and ID = " + newID + " already exists");
    } else if (!SUMOXMLDefinitions::isValidNetID(newID)) {
        throw InvalidArgument("Additional ID " + newID + " contains invalid characters");
    } else {
        // Save old ID
        std::string oldID = getMicrosimID();
        // set New ID
        setMicrosimID(newID);
        // update additional ID in the container of net
        myViewNet->getNet()->updateAdditionalID(oldID, this);
    }
}


GNEEdge*
GNEAdditional::changeEdge(GNEEdge* oldEdge, const std::string& newEdgeID) {
    if (oldEdge == nullptr) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't belong to an " + toString(SUMO_TAG_EDGE));
    } else {
        oldEdge->removeAdditionalChild(this);
        GNEEdge* newEdge = myViewNet->getNet()->retrieveEdge(newEdgeID);
        newEdge->addAdditionalChild(this);
        updateGeometry(true);
        return newEdge;
    }
}


GNELane*
GNEAdditional::changeLane(GNELane* oldLane, const std::string& newLaneID) {
    if (oldLane == nullptr) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't belong to a " + toString(SUMO_TAG_LANE));
    } else {
        oldLane->removeAdditionalChild(this);
        GNELane* newLane = myViewNet->getNet()->retrieveLane(newLaneID);
        newLane->addAdditionalChild(this);
        updateGeometry(true);
        return newLane;
    }
}


void
GNEAdditional::changeFirstAdditionalParent(const std::string& newAdditionalParentID) {
    if (myFirstAdditionalParent == nullptr) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't have an additional parent");
    } else {
        // remove this additional of the childs of parent additional
        myFirstAdditionalParent->removeAdditionalChild(this);
        // set new additional parent
        myFirstAdditionalParent = myViewNet->getNet()->retrieveAdditional(myFirstAdditionalParent->getTag(), newAdditionalParentID);
        // add this additional int the childs of parent additional
        myFirstAdditionalParent->addAdditionalChild(this);
        updateGeometry(true);
    }
}


void
GNEAdditional::changeSecondAdditionalParent(const std::string& newAdditionalParentID) {
    if (mySecondAdditionalParent == nullptr) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't have an additional parent");
    } else {
        // remove this additional of the childs of parent additional
        mySecondAdditionalParent->removeAdditionalChild(this);
        // set new additional parent
        mySecondAdditionalParent = myViewNet->getNet()->retrieveAdditional(mySecondAdditionalParent->getTag(), newAdditionalParentID);
        // add this additional int the childs of parent additional
        mySecondAdditionalParent->addAdditionalChild(this);
        updateGeometry(true);
    }
}


void
GNEAdditional::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(dynamic_cast<GUIGlObject*>(this)->getGlID());
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNEAdditional::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(dynamic_cast<GUIGlObject*>(this)->getGlID());
        if (changeFlag) {
            mySelected = false;
        }
    }
}


bool
GNEAdditional::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEAdditional::checkAdditionalChildRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError("Calling non-implemented function checkAdditionalChildRestriction during saving of " + toString(getTag()) + ". It muss be reimplemented in child class");
}


bool
GNEAdditional::addGenericParameter(const std::string& key, const std::string& value) {
    if (!knowsParameter(key)) {
        setParameter(key, value);
        return true;
    } else {
        return false;
    }
}


bool
GNEAdditional::removeGenericParameter(const std::string& key) {
    if (knowsParameter(key)) {
        unsetParameter(key);
        return true;
    } else {
        return false;
    }
}


bool
GNEAdditional::updateGenericParameter(const std::string& oldKey, const std::string& newKey) {
    if (knowsParameter(oldKey) && !knowsParameter(newKey)) {
        std::string value = getParameter(oldKey);
        unsetParameter(oldKey);
        setParameter(newKey, value);
        return true;
    } else {
        return false;
    }
}


bool
GNEAdditional::updateGenericParameterValue(const std::string& key, const std::string& newValue) {
    if (knowsParameter(key)) {
        setParameter(key, newValue);
        return true;
    } else {
        return false;
    }
}


std::string
GNEAdditional::getGenericParametersStr() const {
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
GNEAdditional::getGenericParameters() const {
    std::vector<std::pair<std::string, std::string> >  result;
    // iterate over parameters map and fill result
    for (auto i : getParametersMap()) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void
GNEAdditional::setGenericParametersStr(const std::string& value) {
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


void
GNEAdditional::mouseOverObject(const GUIVisualizationSettings&) const {
}


/****************************************************************************/
