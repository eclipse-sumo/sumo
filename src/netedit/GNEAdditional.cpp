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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utility>
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
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEAdditional.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, bool movable) :
    GUIGlObject(GLO_ADDITIONAL, id),
    GNEAttributeCarrier(tag, icon),
    myViewNet(viewNet),
    myBlocked(false),
    myAdditionalParent(NULL),
    myBlockIconRotation(0),
    myMovable(movable) {
}


GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, bool movable, GNEAdditional* additionalParent) :
    GUIGlObject(GLO_ADDITIONAL, id),
    GNEAttributeCarrier(tag, icon),
    myViewNet(viewNet),
    myBlocked(false),
    myAdditionalParent(additionalParent),
    myBlockIconRotation(0),
    myMovable(movable) {
}


GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, bool movable, std::vector<GNEEdge*> edgeChilds) :
    GUIGlObject(GLO_ADDITIONAL, id),
    GNEAttributeCarrier(tag, icon),
    myViewNet(viewNet),
    myBlocked(false),
    myAdditionalParent(NULL),
    myEdgeChilds(edgeChilds),
    myBlockIconRotation(0),
    myMovable(movable) {
}


GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, bool movable, std::vector<GNELane*> laneChilds) :
    GUIGlObject(GLO_ADDITIONAL, id),
    GNEAttributeCarrier(tag, icon),
    myViewNet(viewNet),
    myBlocked(false),
    myAdditionalParent(NULL),
    myLaneChilds(laneChilds),
    myBlockIconRotation(0),
    myMovable(movable) {
}


GNEAdditional::~GNEAdditional() {}


void
GNEAdditional::openAdditionalDialog() {
    throw InvalidArgument(toString(getTag()) + " doesn't have an additional dialog");
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
    return myBlocked;
}


bool
GNEAdditional::isAdditionalSelected() const {
    return gSelected.isSelected(getType(), getGlID());
}


GNEAdditional*
GNEAdditional::getAdditionalParent() const {
    return myAdditionalParent;
}



void
GNEAdditional::addAdditionalChild(GNEAdditional* additional) {
    // First check that additional wasn't already inserted
    if (std::find(myAdditionalChilds.begin(), myAdditionalChilds.end(), additional) != myAdditionalChilds.end()) {
        throw ProcessError(toString(additional->getTag()) + " with ID='" + additional->getID() + "' was already inserted in " + toString(getTag()) + " with ID='" + getID() + "'");
    } else {
        myAdditionalChilds.push_back(additional);
        updateGeometry();
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
        updateGeometry();
    }
}


const std::vector<GNEAdditional*>&
GNEAdditional::getAdditionalChilds() const {
    return myAdditionalChilds;
}


void
GNEAdditional::addEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == NULL) {
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
    if (edge == NULL) {
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
    if (lane == NULL) {
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
    if (lane == NULL) {
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


const std::string&
GNEAdditional::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


GUIGLObjectPopupMenu*
GNEAdditional::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
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
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    // show option to open additional dialog
    if (canOpenDialog(getTag())) {
        new FXMenuCommand(ret, ("Open " + toString(getTag()) + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    }
    // get attributes
    std::vector<SumoXMLAttr> attributes = getAttrs();
    // Show position parameters
    if (hasAttribute(getTag(), SUMO_ATTR_LANE)) {
        GNELane* lane = myViewNet->getNet()->retrieveLane(getAttribute(SUMO_ATTR_LANE));
        // Show menu command inner position
        const double innerPos = myShape.nearest_offset_to_point2D(parent.getPositionInformation());
        new FXMenuCommand(ret, ("Cursor position inner additional: " + toString(innerPos)).c_str(), 0, 0, 0);
        // If shape isn't empty, show menu command lane position
        if (myShape.size() > 0) {
            const double lanePos = lane->getShape().nearest_offset_to_point2D(myShape[0]);
            new FXMenuCommand(ret, ("Cursor position over " + toString(SUMO_TAG_LANE) + ": " + toString(innerPos + lanePos)).c_str(), 0, 0, 0);
        }
    } else if (hasAttribute(getTag(), SUMO_ATTR_EDGE)) {
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
    // get attributes
    std::vector<SumoXMLAttr> attributes = getAttrs();
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, (int)attributes.size());
    // Iterate over attributes
    for (auto i : attributes) {
        // Add attribute and set it dynamic if aren't unique
        if (GNEAttributeCarrier::isUnique(getTag(), i)) {
            ret->mkItem(toString(i).c_str(), false, getAttribute(i));
        } else {
            ret->mkItem(toString(i).c_str(), true, getAttribute(i));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNEAdditional::getCenteringBoundary() const {
    // Return Boundary depenpding of myShape
    if (myShape.size() > 0) {
        Boundary b = myShape.getBoxBoundary();
        b.grow(20);
        return b;
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
GNEAdditional::setBlockIconRotation(GNELane* additionalLane) {
    if (myShape.size() > 0 && myShape.length() != 0) {
        // If length of the shape is distint to 0, Obtain rotation of center of shape
        myBlockIconRotation = myShape.rotationDegreeAtOffset((myShape.length() / 2.)) - 90;
    } else if (hasAttribute(getTag(), SUMO_ATTR_LANE)) {
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
        if (isAdditionalSelected()) {
            if (myMovable == false) {
                // Draw not movable texture if additional isn't movable and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_NOTMOVINGSELECTED), size);
            } else if (myBlocked) {
                // Draw lock texture if additional is movable, is blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_LOCKSELECTED), size);
            } else {
                // Draw empty texture if additional is movable, isn't blocked and is selected
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_EMPTYSELECTED), size);
            }
        } else {
            if (myMovable == false) {
                // Draw not movable texture if additional isn't movable
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_NOTMOVING), size);
            } else if (myBlocked) {
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

    // calculate position and rotation of every simbol for every lane
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
    if (isValidID(newID) && (myViewNet->getNet()->getAdditional(getTag(), newID) == NULL)) {
        return true;
    } else {
        return false;
    }
}


void
GNEAdditional::changeAdditionalID(const std::string& newID) {
    if (myViewNet->getNet()->getAdditional(getTag(), newID) != NULL) {
        throw InvalidArgument("An Additional with tag " + toString(getTag()) + " and ID = " + newID + " already exists");
    } else if (isValidID(newID) == false) {
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
    if (oldEdge == NULL) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't belong to an " + toString(SUMO_TAG_EDGE));
    } else {
        oldEdge->removeAdditionalChild(this);
        GNEEdge* newEdge = myViewNet->getNet()->retrieveEdge(newEdgeID);
        newEdge->addAdditionalChild(this);
        updateGeometry();
        return newEdge;
    }
}


GNELane*
GNEAdditional::changeLane(GNELane* oldLane, const std::string& newLaneID) {
    if (oldLane == NULL) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't belong to a " + toString(SUMO_TAG_LANE));
    } else {
        oldLane->removeAdditionalChild(this);
        GNELane* newLane = myViewNet->getNet()->retrieveLane(newLaneID);
        newLane->addAdditionalChild(this);
        updateGeometry();
        return newLane;
    }
}


void
GNEAdditional::changeAdditionalParent(const std::string& newAdditionalParentID) {
    if (myAdditionalParent == NULL) {
        throw InvalidArgument(toString(getTag()) + " with ID '" + getMicrosimID() + "' doesn't have an additional parent");
    } else {
        // remove this additional of the childs of parent additional
        myAdditionalParent->removeAdditionalChild(this);
        myAdditionalParent = myViewNet->getNet()->retrieveAdditional(newAdditionalParentID);
        // add this additional int the childs of parent additional
        myAdditionalParent->addAdditionalChild(this);
        updateGeometry();
    }
}


/****************************************************************************/
