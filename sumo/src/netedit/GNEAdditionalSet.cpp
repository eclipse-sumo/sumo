/****************************************************************************/
/// @file    GNEAdditionalSet.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2015
/// @version $Id$
///
/// A abstract class for representation of set of additional elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
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
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEAdditionalSet.h"
#include "GNEAdditional.h"
#include "GNEUndoList.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNENet.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalSet::GNEAdditionalSet(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, bool blocked, std::vector<GNEAdditional*> additionalChilds, std::vector<GNEEdge*> edgeChilds, std::vector<GNELane*> laneChilds) :
    GNEAdditional(id, viewNet, pos, tag, NULL, blocked) {
    // Insert additionals
    for (int i = 0; i < (int)additionalChilds.size(); i++) {
        addAdditionalChild(additionalChilds.at(i));
    }
    // Insert edges
    for (int i = 0; i < (int)edgeChilds.size(); i++) {
        edgeChilds.at(i)->addAdditionalSet(this);
        edgeChild myEdgeChild;
        myEdgeChild.edge = edgeChilds.at(i);
        myChildEdges.push_back(myEdgeChild);
    }
    // Insert lanes
    for (int i = 0; i < (int)laneChilds.size(); i++) {
        laneChilds.at(i)->addAdditionalSet(this);
        laneChild myLaneChild;
        myLaneChild.lane = laneChilds.at(i);
        myChildLanes.push_back(myLaneChild);
    }
    // Update connections
    updateConnections();
}


GNEAdditionalSet::~GNEAdditionalSet() {
    // Remove references to this additional Set in edges
    childEdges childEdgesToRemove = myChildEdges;
    for (childEdges::iterator i = childEdgesToRemove.begin(); i != childEdgesToRemove.end(); i++) {
        (i->edge->removeAdditionalGeometrySet(this));
    }
    // Remove references to this additional Set in lanes
    childLanes childLanesToRemove = myChildLanes;
    for (childLanes::iterator i = childLanesToRemove.begin(); i != childLanesToRemove.end(); i++) {
        (i->lane->removeAdditionalGeometrySet(this));
    }
}


bool
GNEAdditionalSet::addAdditionalChild(GNEAdditional* additional) {
    for (childAdditionals::iterator i = myChildAdditionals.begin(); i != myChildAdditionals.end(); i++) {
        if ((*i) == additional) {
            return false;
        }
    }
    // If wasn't found, insert it
    myChildAdditionals.push_back(additional);
    return true;
}


bool
GNEAdditionalSet::removeAdditionalGeometryChild(GNEAdditional* additional) {
    for (childAdditionals::iterator i = myChildAdditionals.begin(); i != myChildAdditionals.end(); i++) {
        if ((*i) == additional) {
            myChildAdditionals.erase(i);
            updateConnections();
            return true;
        }
    }
    // If wasn't found, return false
    return false;
}


bool
GNEAdditionalSet::addEdgeChild(GNEEdge* edge) {
    for (childEdges::iterator i = myChildEdges.begin(); i != myChildEdges.end(); i++) {
        if (i->edge == edge) {
            return false;
        }
    }
    // If wasn't found, insert it
    edgeChild myEdgeChild;
    myEdgeChild.edge = edge;
    myChildEdges.push_back(myEdgeChild);
    updateConnections();
    return true;
}


bool
GNEAdditionalSet::removeEdgeChild(GNEEdge* edge) {
    for (childEdges::iterator i = myChildEdges.begin(); i != myChildEdges.end(); i++) {
        if (i->edge == edge) {
            myChildEdges.erase(i);
            updateConnections();
            return true;
        }
    }
    // If wasn't found, return false
    return false;
}


bool
GNEAdditionalSet::addLaneChild(GNELane* lane) {
    for (childLanes::iterator i = myChildLanes.begin(); i != myChildLanes.end(); i++) {
        if (i->lane == lane) {
            return false;
        }
    }
    // If wasn't found, insert it
    laneChild myLaneChild;
    myLaneChild.lane = lane;
    myChildLanes.push_back(myLaneChild);
    updateConnections();
    return true;
}


bool
GNEAdditionalSet::removeLaneChild(GNELane* lane) {
    for (childLanes::iterator i = myChildLanes.begin(); i != myChildLanes.end(); i++) {
        if (i->lane == lane) {
            myChildLanes.erase(i);
            updateConnections();
            return true;
        }
    }
    // If wasn't found, return false
    return false;
}


void
GNEAdditionalSet::updateConnections() {
    // Clear map with the middle positions
    myConnectionMiddlePosition.clear();

    // Iterate over additonals
    for (childAdditionals::iterator i = myChildAdditionals.begin(); i != myChildAdditionals.end(); i++) {
        Position PositionOfChild = (*i)->getPositionInView();
        SUMOReal angleBetweenParentAndChild = myPosition.angleTo2D(PositionOfChild);
        SUMOReal distancieBetweenParentAndChild = myPosition.distanceTo2D(PositionOfChild);
        // Calculate middle point
        myConnectionMiddlePosition[*i] = Position(myPosition.x() + cos(angleBetweenParentAndChild) * distancieBetweenParentAndChild, myPosition.y());
    }

    // Iterate over eges
    for (childEdges::iterator i = myChildEdges.begin(); i != myChildEdges.end(); i++) {
        // clear position of lanes and rotations
        i->positionsOverLanes.clear();
        i->rotationsOverLanes.clear();
        // Calculate position and rotation of every lane
        for (int j = 0; j < (int)i->edge->getLanes().size(); j++) {
            i->positionsOverLanes.push_back(i->edge->getLanes().at(j)->getShape().positionAtOffset(i->edge->getLanes().at(j)->getShape().length() - 10));
            i->rotationsOverLanes.push_back(i->edge->getLanes().at(j)->getShape().rotationDegreeAtOffset(i->edge->getLanes().at(j)->getShape().length() - 10) * -1);
        }
        // Calculate middle position of lanes
        Position middlePoint((i->positionsOverLanes.front().x() + i->positionsOverLanes.back().x()) / 2, (i->positionsOverLanes.front().y() + i->positionsOverLanes.back().y()) / 2);
        // Set position of connection
        SUMOReal angleBetweenParentAndChild = myPosition.angleTo2D(middlePoint);
        SUMOReal distancieBetweenParentAndChild = myPosition.distanceTo2D(middlePoint);
        myConnectionMiddlePosition[i->edge] = Position(myPosition.x() + cos(angleBetweenParentAndChild) * distancieBetweenParentAndChild, myPosition.y());
    }

    // Iterate over lanes
    for (childLanes::iterator i = myChildLanes.begin(); i != myChildLanes.end(); i++) {
        // Calculate position and rotation of every lane
        i->positionOverLane = i->lane->getShape().positionAtOffset(i->lane->getShape().length() - 10);
        i->rotationOverLane = i->lane->getShape().rotationDegreeAtOffset(i->lane->getShape().length() - 10) * -1;
        // Set position of connection
        SUMOReal angleBetweenParentAndChild = myPosition.angleTo2D(i->positionOverLane);
        SUMOReal distancieBetweenParentAndChild = myPosition.distanceTo2D(i->positionOverLane);
        myConnectionMiddlePosition[i->lane] = Position(myPosition.x() + cos(angleBetweenParentAndChild) * distancieBetweenParentAndChild, myPosition.y());
    }
}


void
GNEAdditionalSet::drawConnections() const {
    // Iterate over list of additionals
    for (childAdditionals::const_iterator i = myChildAdditionals.begin(); i != myChildAdditionals.end(); i++) {
        // Add a draw matrix
        glPushMatrix();
        // traslate in the Z axis
        glTranslated(0, 0, getType() - 0.01);
        // Set color of the base
        GLHelper::setColor(RGBColor(255, 235, 0, 255));
        // Draw Line
        GLHelper::drawLine(myPosition, myConnectionMiddlePosition.at(*i));
        GLHelper::drawLine(myConnectionMiddlePosition.at(*i), (*i)->getPositionInView());
        // Pop draw matrix
        glPopMatrix();
    }
    // Iterate over edges
    for (childEdges::const_iterator i = myChildEdges.begin(); i != myChildEdges.end(); i++) {
        // Add a draw matrix
        glPushMatrix();
        // traslate in the Z axis
        glTranslated(0, 0, getType() - 0.01);
        // Set color of the base
        GLHelper::setColor(RGBColor(255, 235, 0, 255));
        // Calculate middle point between lanes
        /*** @todo ADD DISTANCE AS PARAMETER **/
        Position middlePoint((i->positionsOverLanes.front().x() + i->positionsOverLanes.back().x()) / 2, (i->positionsOverLanes.front().y() + i->positionsOverLanes.back().y()) / 2);
        // Draw Line
        GLHelper::drawLine(myPosition, myConnectionMiddlePosition.at(i->edge));
        GLHelper::drawLine(myConnectionMiddlePosition.at(i->edge), middlePoint);
        // Pop draw matrix
        glPopMatrix();
    }
    // Iterate over lanes
    for (childLanes::const_iterator i = myChildLanes.begin(); i != myChildLanes.end(); i++) {
        // Add a draw matrix
        glPushMatrix();
        // traslate in the Z axis
        glTranslated(0, 0, getType() - 0.01);
        // Set color of the base
        GLHelper::setColor(RGBColor(255, 235, 0, 255));
        // Draw Line
        GLHelper::drawLine(myPosition, myConnectionMiddlePosition.at(i->lane));
        GLHelper::drawLine(myConnectionMiddlePosition.at(i->lane), i->positionOverLane);
        // Pop draw matrix
        glPopMatrix();
    }
}


void
GNEAdditionalSet::writeAdditionalChildrens(OutputDevice& device, const std::string& currentDirectory) {
    for (childAdditionals::iterator i = myChildAdditionals.begin(); i != myChildAdditionals.end(); i++) {
        (*i)->writeAdditional(device, currentDirectory);
    }
}


int
GNEAdditionalSet::getNumberOfAdditionalChilds() const {
    return int(myChildAdditionals.size());
}


int
GNEAdditionalSet::getNumberOfEdgeChilds() const {
    return int(myChildEdges.size());
}


int
GNEAdditionalSet::getNumberOfLaneChilds() const {
    return int(myChildLanes.size());
}


std::vector<std::string>
GNEAdditionalSet::getAdditionalChildIds() const {
    // Declare and resize vector
    std::vector<std::string> vectorOfAdditionalsIds;
    vectorOfAdditionalsIds.resize(myChildAdditionals.size());
    // Save Ids
    for (int i = 0; i < (int)myChildAdditionals.size(); i++) {
        vectorOfAdditionalsIds[i] = myChildAdditionals.at(i)->getID();
    }
    return vectorOfAdditionalsIds;
}


std::vector<std::string>
GNEAdditionalSet::getEdgeChildIds() const {
    // Declare and resize vector
    std::vector<std::string> vectorOfEdgesIds;
    vectorOfEdgesIds.resize(myChildEdges.size());
    // Save Ids
    for (int i = 0; i < (int)myChildEdges.size(); i++) {
        vectorOfEdgesIds[i] = myChildEdges.at(i).edge->getID();
    }
    return vectorOfEdgesIds;
}


std::vector<std::string>
GNEAdditionalSet::getLaneChildIds() const {
    // Declare and resize vector
    std::vector<std::string> vectorOfLanesIds;
    vectorOfLanesIds.resize(myChildLanes.size());
    // Save Ids
    for (int i = 0; i < (int)myChildLanes.size(); i++) {
        vectorOfLanesIds[i] = myChildLanes.at(i).lane->getID();
    }
    return vectorOfLanesIds;
}


void
GNEAdditionalSet::setEdgeChilds(std::vector<GNEEdge*> edges) {
    // First remove all existent edges
    for (childEdges::iterator i = myChildEdges.begin(); i != myChildEdges.end(); i++) {
        i->edge->removeAdditionalGeometrySet(this);
    }
    // clear edge childs vector
    myChildEdges.clear();
    // Iterate over vector of new edges
    for (std::vector<GNEEdge*>::iterator i = edges.begin(); i != edges.end(); i++) {
        (*i)->addAdditionalSet(this);
        addEdgeChild(*i);
    }
    // Update geometry
    updateGeometry();
}


void
GNEAdditionalSet::setLaneChilds(std::vector<GNELane*> lanes) {
    // First remove all existent lanes
    for (childLanes::iterator i = myChildLanes.begin(); i != myChildLanes.end(); i++) {
        i->lane->removeAdditionalGeometrySet(this);
    }
    // clear lane childs vector
    myChildLanes.clear();
    // Iterate over vector of new lanes
    for (std::vector<GNELane*>::iterator i = lanes.begin(); i != lanes.end(); i++) {
        (*i)->addAdditionalSet(this);
        addLaneChild(*i);
    }
    // Update geometry
    updateGeometry();
}

/****************************************************************************/
