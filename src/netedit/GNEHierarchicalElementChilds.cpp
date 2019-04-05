/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEHierarchicalElementChilds.cpp
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
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/demandelements/GNEDemandElement.h>

#include "GNEHierarchicalElementChilds.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEHierarchicalElementChilds::GNEHierarchicalElementChilds(GNEAttributeCarrier* AC,
        const std::vector<GNEEdge*> &edgeChilds,
        const std::vector<GNELane*> &laneChilds,
        const std::vector<GNEShape*> &shapeChilds,
        const std::vector<GNEAdditional*> &additionalChilds,
        const std::vector<GNEDemandElement*> &demandElementChilds) :
    myEdgeChilds(edgeChilds),
    myLaneChilds(laneChilds),
    myShapeChilds(shapeChilds),
    myAdditionalChilds(additionalChilds),
    myDemandElementChilds(demandElementChilds),
    myChildConnections(this),
    myAC(AC) {
}


GNEHierarchicalElementChilds::~GNEHierarchicalElementChilds() {}


void
GNEHierarchicalElementChilds::addAdditionalChild(GNEAdditional* additional) {
    // First check that additional wasn't already inserted
    if (std::find(myAdditionalChilds.begin(), myAdditionalChilds.end(), additional) != myAdditionalChilds.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myAdditionalChilds.push_back(additional);
        // only execute post operations if update geometry is enabled
        if (additional->getViewNet()->getNet()->isUpdateGeometryEnabled()) {
            // Check if childs has to be sorted automatically
            if (myAC->getTagProperty().canAutomaticSortChilds()) {
                sortAdditionalChilds();
            }
            // update additional parent after add additional (note: by default non-implemented)
            updateAdditionalParent();
            updateGeometry(true);
        }
    }
}


void
GNEHierarchicalElementChilds::removeAdditionalChild(GNEAdditional* additional) {
    // First check that additional was already inserted
    auto it = std::find(myAdditionalChilds.begin(), myAdditionalChilds.end(), additional);
    if (it == myAdditionalChilds.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myAdditionalChilds.erase(it);
        // only execute post operations if update geometry is enabled
        if (additional->getViewNet()->getNet()->isUpdateGeometryEnabled()) {
            // Check if childs has to be sorted automatically
            if (myAC->getTagProperty().canAutomaticSortChilds()) {
                sortAdditionalChilds();
            }
            // update additional parent after add additional (note: by default non-implemented)
            updateAdditionalParent();
            updateGeometry(true);
        }
    }
}


const std::vector<GNEAdditional*>&
GNEHierarchicalElementChilds::getAdditionalChilds() const {
    return myAdditionalChilds;
}


void
GNEHierarchicalElementChilds::sortAdditionalChilds() {
    if (myAC->getTagProperty().getTag() == SUMO_TAG_E3DETECTOR) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedEntryExits;
        // obtain all entrys
        for (auto i : myAdditionalChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
                sortedEntryExits.push_back(i);
            }
        }
        // obtain all exits
        for (auto i : myAdditionalChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
                sortedEntryExits.push_back(i);
            }
        }
        // change myAdditionalChilds for sortedEntryExits
        if (sortedEntryExits.size() == myAdditionalChilds.size()) {
            myAdditionalChilds = sortedEntryExits;
        } else {
            throw ProcessError("Some additional childs were lost during sorting");
        }
    } else if (myAC->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        // we need to sort Entry/Exits due additional.xds model
        std::vector<GNEAdditional*> sortedTAZSourceSink;
        // obtain all TAZSources
        for (auto i : myAdditionalChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
                sortedTAZSourceSink.push_back(i);
            }
        }
        // obtain all TAZSinks
        for (auto i : myAdditionalChilds) {
            if (i->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
                sortedTAZSourceSink.push_back(i);
            }
        }
        // change myAdditionalChilds for sortedEntryExits
        if (sortedTAZSourceSink.size() == myAdditionalChilds.size()) {
            myAdditionalChilds = sortedTAZSourceSink;
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
            if (i->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
                sortedChilds.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_TIME));
            } else if (i->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
                sortedChilds.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
            }
            // set end attribute
            if (i->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
                sortedChilds.back().first.second = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_END));
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
GNEHierarchicalElementChilds::checkAdditionalChildsOverlapping() const {
    // declare a vector to keep sorted childs
    std::vector<std::pair<std::pair<double, double>, GNEAdditional*> > sortedChilds;
    // iterate over additional childs
    for (auto i : myAdditionalChilds) {
        sortedChilds.push_back(std::make_pair(std::make_pair(0., 0.), i));
        // set begin/start attribute
        if (i->getTagProperty().hasAttribute(SUMO_ATTR_TIME) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_TIME))) {
            sortedChilds.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_TIME));
        } else if (i->getTagProperty().hasAttribute(SUMO_ATTR_BEGIN) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_BEGIN))) {
            sortedChilds.back().first.first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
        }
        // set end attribute
        if (i->getTagProperty().hasAttribute(SUMO_ATTR_END) && GNEAttributeCarrier::canParse<double>(i->getAttribute(SUMO_ATTR_END))) {
            sortedChilds.back().first.second = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_END));
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
GNEHierarchicalElementChilds::addDemandElementChild(GNEDemandElement* demandElement) {
    // First check that demandElement wasn't already inserted
    if (std::find(myDemandElementChilds.begin(), myDemandElementChilds.end(), demandElement) != myDemandElementChilds.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' was already inserted in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myDemandElementChilds.push_back(demandElement);
        // Check if childs has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChilds()) {
            sortDemandElementChilds();
        }
        // update demandElement parent after add demandElement (note: by default non-implemented)
        updateDemandElementParent();
        // update geometry (for set geometry of lines between Parents and Childs)
        if (demandElement->getViewNet()->getNet()->isUpdateGeometryEnabled()) {
            updateGeometry(true);
        }
    }
}


void
GNEHierarchicalElementChilds::removeDemandElementChild(GNEDemandElement* demandElement) {
    // First check that demandElement was already inserted
    auto it = std::find(myDemandElementChilds.begin(), myDemandElementChilds.end(), demandElement);
    if (it == myDemandElementChilds.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myDemandElementChilds.erase(it);
        // Check if childs has to be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChilds()) {
            sortDemandElementChilds();
        }
        // update demandElement parent after add demandElement (note: by default non-implemented)
        updateDemandElementParent();
        // update geometry (for remove geometry of lines between Parents and Childs)
        if (demandElement->getViewNet()->getNet()->isUpdateGeometryEnabled()) {
            updateGeometry(true);
        }
    }
}


const std::vector<GNEDemandElement*>&
GNEHierarchicalElementChilds::getDemandElementChilds() const {
    return myDemandElementChilds;
}


void
GNEHierarchicalElementChilds::sortDemandElementChilds() {
   // by default empty
}


bool
GNEHierarchicalElementChilds::checkDemandElementChildsOverlapping() const {
  return true;
}


void
GNEHierarchicalElementChilds::addEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and doesn't exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge) != myEdgeChilds.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myEdgeChilds.push_back(edge);
    }
}


void
GNEHierarchicalElementChilds::removeEdgeChild(GNEEdge* edge) {
    // Check that edge is valid and exist previously
    if (edge == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge) == myEdgeChilds.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myEdgeChilds.erase(std::find(myEdgeChilds.begin(), myEdgeChilds.end(), edge));
    }
}


const std::vector<GNEEdge*>&
GNEHierarchicalElementChilds::getEdgeChilds() const {
    return myEdgeChilds;
}


void
GNEHierarchicalElementChilds::addLaneChild(GNELane* lane) {
    // Check that lane is valid and doesn't exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myLaneChilds.begin(), myLaneChilds.end(), lane) != myLaneChilds.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myLaneChilds.push_back(lane);
    }
}


void
GNEHierarchicalElementChilds::removeLaneChild(GNELane* lane) {
    // Check that lane is valid and exist previously
    if (lane == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else if (std::find(myLaneChilds.begin(), myLaneChilds.end(), lane) == myLaneChilds.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    } else {
        myLaneChilds.erase(std::find(myLaneChilds.begin(), myLaneChilds.end(), lane));
    }
}


const std::vector<GNELane*>&
GNEHierarchicalElementChilds::getLaneChilds() const {
    return myLaneChilds;
}


void
GNEHierarchicalElementChilds::addShapeChild(GNEShape* shape) {
    // Check that shape is valid and doesn't exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to add an empty " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    }
    else if (std::find(myShapeChilds.begin(), myShapeChilds.end(), shape) != myShapeChilds.end()) {
        throw InvalidArgument("Trying to add a duplicate " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    }
    else {
        myShapeChilds.push_back(shape);
    }
}


void
GNEHierarchicalElementChilds::removeShapeChild(GNEShape* shape) {
    // Check that shape is valid and exist previously
    if (shape == nullptr) {
        throw InvalidArgument("Trying to remove an empty " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    }
    else if (std::find(myShapeChilds.begin(), myShapeChilds.end(), shape) == myShapeChilds.end()) {
        throw InvalidArgument("Trying to remove a non previously inserted " + toString(SUMO_TAG_EDGE) + " child in " + myAC->getTagStr() + " with ID='" + myAC->getID() + "'");
    }
    else {
        myShapeChilds.erase(std::find(myShapeChilds.begin(), myShapeChilds.end(), shape));
    }
}


const std::vector<GNEShape*>&
GNEHierarchicalElementChilds::getShapeChilds() const {
    return myShapeChilds;
}


void
GNEHierarchicalElementChilds::updateAdditionalParent() {
    // by default nothing to do
}


void 
GNEHierarchicalElementChilds::updateDemandElementParent() {
    // by default nothing to do
}

// ---------------------------------------------------------------------------
// GNEHierarchicalElementChilds::ChildConnections - methods
// ---------------------------------------------------------------------------

GNEHierarchicalElementChilds::ChildConnections::ChildConnections(GNEHierarchicalElementChilds* hierarchicalElement) :
    myHierarchicalElement(hierarchicalElement) {}


void
GNEHierarchicalElementChilds::ChildConnections::update() {
    // first clear connection positions
    connectionPositions.clear();
    symbolsPositionAndRotation.clear();

    // calculate position and rotation of every simbol for every edge
    for (const auto &i : myHierarchicalElement->myEdgeChilds) {
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
    for (const auto &i : myHierarchicalElement->myLaneChilds) {
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

    // calculate position for every additional child
    for (const auto &i : myHierarchicalElement->myAdditionalChilds) {
        // check that position is different of position
        if (i->getPositionInView() != myHierarchicalElement->getPositionInView()) {
            std::vector<Position> posConnection;
            double A = std::abs(i->getPositionInView().x() - myHierarchicalElement->getPositionInView().x());
            double B = std::abs(i->getPositionInView().y() - myHierarchicalElement->getPositionInView().y());
            // Set positions of connection's vertex. Connection is build from Entry to E3
            posConnection.push_back(i->getPositionInView());
            if (myHierarchicalElement->getPositionInView().x() > i->getPositionInView().x()) {
                if (myHierarchicalElement->getPositionInView().y() > i->getPositionInView().y()) {
                    posConnection.push_back(Position(i->getPositionInView().x() + A, i->getPositionInView().y()));
                } else {
                    posConnection.push_back(Position(i->getPositionInView().x(), i->getPositionInView().y() - B));
                }
            } else {
                if (myHierarchicalElement->getPositionInView().y() > i->getPositionInView().y()) {
                    posConnection.push_back(Position(i->getPositionInView().x(), i->getPositionInView().y() + B));
                } else {
                    posConnection.push_back(Position(i->getPositionInView().x() - A, i->getPositionInView().y()));
                }
            }
            posConnection.push_back(myHierarchicalElement->getPositionInView());
            connectionPositions.push_back(posConnection);
        }
    }

    // calculate geometry for connections between parent and childs
    for (const auto &i : symbolsPositionAndRotation) {
        std::vector<Position> posConnection;
        double A = std::abs(i.first.x() - myHierarchicalElement->getPositionInView().x());
        double B = std::abs(i.first.y() - myHierarchicalElement->getPositionInView().y());
        // Set positions of connection's vertex. Connection is build from Entry to E3
        posConnection.push_back(i.first);
        if (myHierarchicalElement->getPositionInView().x() > i.first.x()) {
            if (myHierarchicalElement->getPositionInView().y() > i.first.y()) {
                posConnection.push_back(Position(i.first.x() + A, i.first.y()));
            } else {
                posConnection.push_back(Position(i.first.x(), i.first.y() - B));
            }
        } else {
            if (myHierarchicalElement->getPositionInView().y() > i.first.y()) {
                posConnection.push_back(Position(i.first.x(), i.first.y() + B));
            } else {
                posConnection.push_back(Position(i.first.x() - A, i.first.y()));
            }
        }
        posConnection.push_back(myHierarchicalElement->getPositionInView());
        connectionPositions.push_back(posConnection);
    }
}


void
GNEHierarchicalElementChilds::ChildConnections::draw(GUIGlObjectType parentType) const {
    // Iterate over myConnectionPositions
    for (const auto &i : connectionPositions) {
        // Add a draw matrix
        glPushMatrix();
        // traslate in the Z axis
        glTranslated(0, 0, parentType - 0.01);
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

/****************************************************************************/
