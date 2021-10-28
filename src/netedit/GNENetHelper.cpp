/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNENetHelper.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// Helper for GNENet
/****************************************************************************/

#include <netbuild/NBAlgorithms.h>
#include <netbuild/NBNetBuilder.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Shape.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/elements/additional/GNEPOI.h>
#include <netedit/elements/additional/GNEPoly.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/demand/GNEVehicleType.h>
#include <netedit/elements/network/GNEEdgeType.h>

#include "GNENetHelper.h"

// ---------------------------------------------------------------------------
// GNENetHelper::AttributeCarriers - methods
// ---------------------------------------------------------------------------

GNENetHelper::AttributeCarriers::AttributeCarriers(GNENet* net) :
    myNet(net),
    myAllowUndoShapes(true) {
    // fill additionals with tags
    auto additionalTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::ADDITIONALELEMENT | GNETagProperties::TagType::SYMBOL, false);
    for (const auto& additionalTag : additionalTags) {
        myAdditionals.insert(std::make_pair(additionalTag.first.getTag(), std::vector<GNEAdditional*>()));
    }
    // fill shapes with tags
    auto shapeTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::SHAPE, false);
    for (const auto& shapeTag : shapeTags) {
        myShapes.insert(std::make_pair(shapeTag.first.getTag(), std::vector<GNEShape*>()));
    }
    // fill TAZElements with tags
    auto TAZElementTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::TAZELEMENT, false);
    for (const auto& TAZElementTag : TAZElementTags) {
        myTAZElements.insert(std::make_pair(TAZElementTag.first.getTag(), std::vector<GNETAZElement*>()));
    }
    // fill demand elements with tags
    auto demandElementTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::DEMANDELEMENT, false);
    for (const auto& demandElementTag : demandElementTags) {
        myDemandElements.insert(std::make_pair(demandElementTag.first.getTag(), std::vector<GNEDemandElement*>()));
    }
    auto stopTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::STOP, false);
    for (const auto& stopTag : stopTags) {
        myDemandElements.insert(std::make_pair(stopTag.first.getTag(), std::vector<GNEDemandElement*>()));
    }
    // fill data elements with tags
    auto genericDataElementTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::GENERICDATA, false);
    for (const auto& genericDataElementTag : genericDataElementTags) {
        myGenericDatas.insert(std::make_pair(genericDataElementTag.first.getTag(), std::set<GNEGenericData*>()));
    }
}


GNENetHelper::AttributeCarriers::~AttributeCarriers() {
    // Drop EdgeTypes
    for (const auto& edgeType : myEdgeTypes) {
        edgeType.second->decRef("GNENetHelper::~GNENet");
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + edgeType.second->getTagStr() + " '" + edgeType.second->getID() + "' in AttributeCarriers destructor");
        delete edgeType.second;
    }
    // Drop Edges
    for (const auto& edge : myEdges) {
        edge.second->decRef("GNENetHelper::~GNENet");
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + edge.second->getTagStr() + " '" + edge.second->getID() + "' in AttributeCarriers destructor");
        delete edge.second;
    }
    // Drop myJunctions
    for (const auto& junction : myJunctions) {
        junction.second->decRef("GNENetHelper::~GNENet");
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + junction.second->getTagStr() + " '" + junction.second->getID() + "' in AttributeCarriers destructor");
        delete junction.second;
    }
    // Drop Additionals (Only used for additionals that were inserted without using GNEChange_Additional)
    for (const auto& additionalTag : myAdditionals) {
        for (const auto& additional : additionalTag.second) {
            // decrease reference manually (because it was increased manually in GNEAdditionalHandler)
            additional->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + additional->getTagStr() + " '" + additional->getID() + "' in AttributeCarriers destructor");
            delete additional;
        }
    }
    // Drop Shapes (Only used for shapes that were inserted without using GNEChange_Shape)
    for (const auto& shapeTag : myShapes) {
        for (const auto& shape : shapeTag.second) {
            // decrease reference manually (because it was increased manually in GNEShapeHandler)
            shape->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + shape->getTagStr() + " '" + shape->getID() + "' in AttributeCarriers destructor");
            delete shape;
        }
    }
    // Drop TAZElements (Only used for TAZElements that were inserted without using GNEChange_TAZElement)
    for (const auto& TAZElementTag : myTAZElements) {
        for (const auto& TAZElement : TAZElementTag.second) {
            // decrease reference manually (because it was increased manually in GNETAZElementHandler)
            TAZElement->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + TAZElement->getTagStr() + " '" + TAZElement->getID() + "' in AttributeCarriers destructor");
            delete TAZElement;
        }
    }
    // Drop demand elements (Only used for demand elements that were inserted without using GNEChange_DemandElement, for example the default VType")
    for (const auto& demandElementTag : myDemandElements) {
        for (const auto& demandElement : demandElementTag.second) {
            // decrease reference manually (because it was increased manually in GNERouteHandler)
            demandElement->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + demandElement->getTagStr() + " '" + demandElement->getID() + "' in AttributeCarriers destructor");
            delete demandElement;
        }
    }
}


void
GNENetHelper::AttributeCarriers::remapJunctionAndEdgeIds() {
    std::map<std::string, GNEEdge*> newEdgeMap;
    std::map<std::string, GNEJunction*> newJunctionMap;
    // fill newEdgeMap
    for (const auto& edge : myEdges) {
        edge.second->setMicrosimID(edge.second->getNBEdge()->getID());
        newEdgeMap[edge.second->getNBEdge()->getID()] = edge.second;
    }
    for (const auto& junction : myJunctions) {
        newJunctionMap[junction.second->getNBNode()->getID()] = junction.second;
        junction.second->setMicrosimID(junction.second->getNBNode()->getID());
    }
    myEdges = newEdgeMap;
    myJunctions = newJunctionMap;
}


bool 
GNENetHelper::AttributeCarriers::isNetworkElementAroundShape(GNEAttributeCarrier* AC, const PositionVector &shape) const {
    // check what tipe of AC
    if (AC->getTagProperty().getTag() == SUMO_TAG_JUNCTION) {
        // Junction
        const GNEJunction *junction = myJunctions.at(AC->getID());
        if (junction->getNBNode()->getShape().size() == 0) {
            return shape.around(junction->getNBNode()->getCenter());
        } else {
            return (shape.overlapsWith(junction->getNBNode()->getShape()));
        }
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_EDGE) {
        // Edge
        for (const auto &lane : myEdges.at(AC->getID())->getLanes()) {
            if (shape.overlapsWith(lane->getLaneShape())) {
                return true;
            }
        }
        return false;
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_LANE) {
        // Lane
        return shape.overlapsWith(myNet->retrieveLane(AC->getID())->getLaneShape());
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_CONNECTION) {
        // connection
        return shape.overlapsWith(dynamic_cast<GNEConnection*>(AC)->getConnectionShape());
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_CROSSING) {
        // crossing
        return shape.overlapsWith(dynamic_cast<GNECrossing*>(AC)->getCrossingShape());
    } else if (AC->getTagProperty().isAdditionalElement()) {
        // Additional
        const GNEAdditional *additional = myNet->retrieveAdditional(AC);
        if (additional->getAdditionalGeometry().getShape().size() <= 1) {
            return shape.around(additional->getPositionInView());
        } else {
            return shape.overlapsWith(additional->getAdditionalGeometry().getShape());
        }
    } else if (AC->getTagProperty().isShape()) {
        // shapes (Polys and POIs)
        if (AC->getTagProperty().getTag() == SUMO_TAG_POLY) {
            // Polygon
            return shape.overlapsWith(dynamic_cast<GNEPoly*>(AC)->getShape());
        } else {
            // POI
            return shape.around(dynamic_cast<GNEPoly*>(AC)->getPositionInView());
        }
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        // TAZ
        return shape.overlapsWith(dynamic_cast<GNETAZ*>(AC)->getTAZElementShape());
    } else {
        return false;
    }
}


GNEJunction*
GNENetHelper::AttributeCarriers::registerJunction(GNEJunction* junction) {
    // increase reference
    junction->incRef("GNENet::registerJunction");
    junction->setResponsible(false);
    myJunctions[junction->getMicrosimID()] = junction;
    // expand net boundary
    myNet->expandBoundary(junction->getCenteringBoundary());
    // add edge into grid
    myNet->addGLObjectIntoGrid(junction);
    // update geometry
    junction->updateGeometry();
    // add z in net boundary
    myNet->addZValueInBoundary(junction->getNBNode()->getPosition().z());
    return junction;
}


const std::map<std::string, GNEJunction*>&
GNENetHelper::AttributeCarriers::getJunctions() const {
    return myJunctions;
}


void
GNENetHelper::AttributeCarriers::clearJunctions() {
    myJunctions.clear();
}


void
GNENetHelper::AttributeCarriers::updateJunctionID(GNEJunction* junction, const std::string& newID) {
    if (myJunctions.count(junction->getID()) == 0) {
        throw ProcessError(junction->getTagStr() + " with ID='" + junction->getID() + "' doesn't exist in AttributeCarriers.junction");
    } else if (myJunctions.count(newID) != 0) {
        throw ProcessError("There is another " + junction->getTagStr() + " with new ID='" + newID + "' in myJunctions");
    } else {
        // remove junction from container
        myJunctions.erase(junction->getNBNode()->getID());
        // rename in NetBuilder
        myNet->getNetBuilder()->getNodeCont().rename(junction->getNBNode(), newID);
        // update microsim ID
        junction->setMicrosimID(newID);
        // add it into myJunctions again
        myJunctions[junction->getID()] = junction;
        // build crossings
        junction->getNBNode()->buildCrossings();
        // net has to be saved
        myNet->requireSaveNet(true);
    }
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedJunctions() const {
    int counter = 0;
    for (const auto &junction : myJunctions) {
        if (junction.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


void
GNENetHelper::AttributeCarriers::insertCrossing(GNECrossing* crossing) {
    if (myCrossings.insert(crossing).second == false) {
        throw ProcessError(crossing->getTagStr() + " with ID='" + crossing->getID() + "' already exist");
    } else {
        ;
    }
}


void 
GNENetHelper::AttributeCarriers::deleteCrossing(GNECrossing* crossing) {
    const auto finder = myCrossings.find(crossing);
    if (finder == myCrossings.end()) {
        throw ProcessError(crossing->getTagStr() + " with ID='" + crossing->getID() + "' wasn't previously inserted");
    } else {
        myCrossings.erase(finder);
    }
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedCrossings() const {
    int counter = 0;
    for (const auto &crossing : myCrossings) {
        if (crossing->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


GNEEdgeType*
GNENetHelper::AttributeCarriers::registerEdgeType(GNEEdgeType* edgeType) {
    // increase reference
    edgeType->incRef("GNENet::registerEdgeType");
    // add it in container
    myEdgeTypes[edgeType->getMicrosimID()] = edgeType;
    return edgeType;
}


const std::map<std::string, GNEEdgeType*>&
GNENetHelper::AttributeCarriers::getEdgeTypes() const {
    return myEdgeTypes;
}


void GNENetHelper::AttributeCarriers::clearEdgeTypes() {
    myEdgeTypes.clear();
}


void
GNENetHelper::AttributeCarriers::updateEdgeTypeID(GNEEdgeType* edgeType, const std::string& newID) {
    if (myEdgeTypes.count(edgeType->getID()) == 0) {
        throw ProcessError(edgeType->getTagStr() + " with ID='" + edgeType->getID() + "' doesn't exist in AttributeCarriers.edgeType");
    } else if (myEdgeTypes.count(newID) != 0) {
        throw ProcessError("There is another " + edgeType->getTagStr() + " with new ID='" + newID + "' in myEdgeTypes");
    } else {
        // remove edgeType from container
        myEdgeTypes.erase(edgeType->getID());
        // rename in typeCont
        myNet->getNetBuilder()->getTypeCont().updateEdgeTypeID(edgeType->getID(), newID);
        // update microsim ID
        edgeType->setMicrosimID(newID);
        // add it into myEdgeTypes again
        myEdgeTypes[edgeType->getID()] = edgeType;
        // net has to be saved
        myNet->requireSaveNet(true);
    }
}


GNEEdge*
GNENetHelper::AttributeCarriers::registerEdge(GNEEdge* edge) {
    edge->incRef("GNENet::registerEdge");
    edge->setResponsible(false);
    // add edge to internal container of GNENet
    myEdges[edge->getMicrosimID()] = edge;
    // expand edge boundary
    myNet->expandBoundary(edge->getCenteringBoundary());
    // add edge into grid
    myNet->addGLObjectIntoGrid(edge);
    // insert all lanes
    for (const auto &lane : edge->getLanes()) {
        insertLane(lane);
    }
    // Add references into GNEJunctions
    edge->getFromJunction()->addOutgoingGNEEdge(edge);
    edge->getToJunction()->addIncomingGNEEdge(edge);
    return edge;
}


const std::map<std::string, GNEEdge*>&
GNENetHelper::AttributeCarriers::getEdges() const {
    return myEdges;
}


void GNENetHelper::AttributeCarriers::clearEdges() {
    myEdges.clear();
}


void
GNENetHelper::AttributeCarriers::updateEdgeID(GNEEdge *edge, const std::string& newID) {
    if (myEdges.count(edge->getID()) == 0) {
        throw ProcessError(edge->getTagStr() + " with ID='" + edge->getID() + "' doesn't exist in AttributeCarriers.edge");
    } else if (myEdges.count(newID) != 0) {
        throw ProcessError("There is another " + edge->getTagStr() + " with new ID='" + newID + "' in myEdges");
    } else {
        // remove edge from container
        myEdges.erase(edge->getNBEdge()->getID());
        // rename in NetBuilder
        myNet->getNetBuilder()->getEdgeCont().rename(edge->getNBEdge(), newID);
        // update microsim ID
        edge->setMicrosimID(newID);
        // add it into myEdges again
        myEdges[edge->getID()] = edge;
        // rename all connections related to this edge
        for (const auto& lane : edge->getLanes()) {
            lane->updateConnectionIDs();
        }
        // net has to be saved
        myNet->requireSaveNet(true);
    }
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedEdges() const {
    int counter = 0;
    for (const auto &edge : myEdges) {
        if (edge.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


void
GNENetHelper::AttributeCarriers::insertLane(GNELane* lane) {
    if (myLanes.insert(lane).second == false) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' already exist");
    }
}


void 
GNENetHelper::AttributeCarriers::deleteLane(GNELane* lane) {
    const auto finder = myLanes.find(lane);
    if (finder == myLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' wasn't previously inserted");
    } else {
        myLanes.erase(finder);
    }
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedLanes() const {
    int counter = 0;
    for (const auto &lane : myLanes) {
        if (lane->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


void
GNENetHelper::AttributeCarriers::insertConnection(GNEConnection* connection) {
    if (myConnections.insert(connection).second == false) {
        throw ProcessError(connection->getTagStr() + " with ID='" + connection->getID() + "' already exist");
    } else {
        ;
    }
}


void 
GNENetHelper::AttributeCarriers::deleteConnection(GNEConnection* connection) {
    const auto finder = myConnections.find(connection);
    if (finder == myConnections.end()) {
        throw ProcessError(connection->getTagStr() + " with ID='" + connection->getID() + "' wasn't previously inserted");
    } else {
        myConnections.erase(finder);
    }
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedConnections() const {
    int counter = 0;
    for (const auto &connection : myConnections) {
        if (connection->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


const std::map<SumoXMLTag, std::vector<GNEAdditional*> >&
GNENetHelper::AttributeCarriers::getAdditionals() const {
    return myAdditionals;
}


void
GNENetHelper::AttributeCarriers::clearAdditionals() {
    // clear elements in grid
    for (const auto& additionalsTags : myAdditionals) {
        for (const auto& additional : additionalsTags.second) {
            myNet->removeGLObjectFromGrid(additional);
        }
    }
    // iterate over myAdditionals and clear all additionals
    for (auto& additionals : myAdditionals) {
        additionals.second.clear();
    }
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedAdditionals() const {
    int counter = 0;
    for (const auto& additionalsTags : myAdditionals) {
        for (const auto& additional : additionalsTags.second) {
            if (additional->isAttributeCarrierSelected()) {
                counter++;
            }
            // also check children (Entry/Exit, spaces...)
            for (const auto& additionalChild : additional->getChildAdditionals()) {
                if (additionalChild->isAttributeCarrierSelected()) {
                    counter++;
                }
                // also check grandchildren (rerouters)
                for (const auto& additionalGrandChild : additionalChild->getChildAdditionals()) {
                    if (additionalGrandChild->isAttributeCarrierSelected()) {
                        counter++;
                    }
                }

            }
        }
    }
    return counter;
}


const std::map<SumoXMLTag, std::vector<GNEShape*> >&
GNENetHelper::AttributeCarriers::getShapes() const {
    return myShapes;
}


void
GNENetHelper::AttributeCarriers::clearShapes() {
    // clear elements in grid
    for (const auto& shapesTags : myShapes) {
        for (const auto& shape : shapesTags.second) {
            myNet->removeGLObjectFromGrid(shape);
        }
    }
    // iterate over myShapes and clear all shapes
    for (auto& shapes : myShapes) {
        shapes.second.clear();
    }
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedPolygons() const {
    int counter = 0;
    for (const auto &poly : myShapes.at(SUMO_TAG_POLY)) {
        if (poly->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedPOIs() const {
    int counter = 0;
    for (const auto &POI : myShapes.at(SUMO_TAG_POI)) {
        if (POI->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto &POILane : myShapes.at(GNE_TAG_POILANE)) {
        if (POILane->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto &POIGEO : myShapes.at(GNE_TAG_POIGEO)) {
        if (POIGEO->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


const std::map<SumoXMLTag, std::vector<GNETAZElement*> >&
GNENetHelper::AttributeCarriers::getTAZElements() const {
    return myTAZElements;
}


void
GNENetHelper::AttributeCarriers::clearTAZElements() {
    // clear elements in grid
    for (const auto& TAZElementsTags : myTAZElements) {
        for (const auto& TAZElement : TAZElementsTags.second) {
            myNet->removeGLObjectFromGrid(TAZElement);
        }
    }
    // iterate over myTAZElements and clear all TAZElements
    for (auto& TAZElements : myTAZElements) {
        TAZElements.second.clear();
    }
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedTAZs() const {
    int counter = 0;
    for (const auto &poly : myTAZElements.at(SUMO_TAG_TAZ)) {
        if (poly->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


bool
GNENetHelper::AttributeCarriers::TAZElementExist(const GNETAZElement* TAZElement) const {
    // first check that TAZElement pointer is valid
    if (TAZElement) {
        // get vector with this TAZ element type
        const auto &TAZElementTag = myTAZElements.at(TAZElement->getTagProperty().getTag());
        // find demanElement in TAZElementTag
        return std::find(TAZElementTag.begin(), TAZElementTag.end(), TAZElement) != TAZElementTag.end();
    } else {
        throw ProcessError("Invalid TAZElement pointer");
    }
}


const std::map<SumoXMLTag, std::vector<GNEDemandElement*> >&
GNENetHelper::AttributeCarriers::getDemandElements() const {
    return myDemandElements;
}


GNEDemandElement* 
GNENetHelper::AttributeCarriers::getDefaultVType() const {
    for (const auto &vType : myDemandElements.at(SUMO_TAG_VTYPE)) {
        if (vType->getID() == DEFAULT_VTYPE_ID) {
            return vType;
        }
    }
    throw ProcessError("Default vType doesn't exist");
}


GNEDemandElement* 
GNENetHelper::AttributeCarriers::getDefaultPType() const {
    for (const auto &pType : myDemandElements.at(SUMO_TAG_PTYPE)) {
        if (pType->getID() == DEFAULT_PEDTYPE_ID) {
            return pType;
        }
    }
    throw ProcessError("Default pType doesn't exist");
}


void
GNENetHelper::AttributeCarriers::clearDemandElements() {
    // clear elements in grid
    for (const auto& demandElementsTags : myDemandElements) {
        for (const auto& demandElement : demandElementsTags.second) {
            myNet->removeGLObjectFromGrid(demandElement);
        }
    }
    // iterate over myDemandElements and clear all demand elemnts
    for (auto& demandElements : myDemandElements) {
        demandElements.second.clear();
    }
}


void
GNENetHelper::AttributeCarriers::addDefaultVTypes() {
    // Create default vehicle Type (it has to be created here due myViewNet was previously nullptr)
    GNEVehicleType* defaultVehicleType = new GNEVehicleType(myNet, DEFAULT_VTYPE_ID, SVC_PASSENGER, SUMO_TAG_VTYPE);
    myDemandElements.at(defaultVehicleType->getTagProperty().getTag()).push_back(defaultVehicleType);
    defaultVehicleType->incRef("GNENet::DEFAULT_VEHTYPE");

    // Create default Bike Type (it has to be created here due myViewNet was previously nullptr)
    GNEVehicleType* defaultBikeType = new GNEVehicleType(myNet, DEFAULT_BIKETYPE_ID, SVC_BICYCLE, SUMO_TAG_VTYPE);
    myDemandElements.at(defaultBikeType->getTagProperty().getTag()).push_back(defaultBikeType);
    defaultBikeType->incRef("GNENet::DEFAULT_BIKETYPE_ID");

    // Create default person Type (it has to be created here due myViewNet was previously nullptr)
    GNEVehicleType* defaultPersonType = new GNEVehicleType(myNet, DEFAULT_PEDTYPE_ID, SVC_PEDESTRIAN, SUMO_TAG_PTYPE);
    myDemandElements.at(defaultPersonType->getTagProperty().getTag()).push_back(defaultPersonType);
    defaultPersonType->incRef("GNENet::DEFAULT_PEDTYPE_ID");
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedRoutes() const {
    int counter = 0;
    // iterate over routes
    for (const auto &route : myDemandElements.at(SUMO_TAG_ROUTE)) {
        if (route->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    // iterate over vehicles with embedded routes
    for (const auto &vehicle : myDemandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
        if (vehicle->getChildDemandElements().front()->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto &flow : myDemandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
        if (flow->getChildDemandElements().front()->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedVehicles() const {
    int counter = 0;
    // iterate over all vehicles and flows
    for (const auto &vehicle : myDemandElements.at(SUMO_TAG_VEHICLE)) {
        if (vehicle->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto &trip : myDemandElements.at(SUMO_TAG_TRIP)) {
        if (trip->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto &vehicle : myDemandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
        if (vehicle->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto &flow : myDemandElements.at(SUMO_TAG_FLOW)) {
        if (flow->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto &flow : myDemandElements.at(GNE_TAG_FLOW_ROUTE)) {
        if (flow->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto &flow : myDemandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
        if (flow->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedPersons() const {
    int counter = 0;
    // iterate over all persons
    for (const auto &person : myDemandElements.at(SUMO_TAG_PERSON)) {
        if (person->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto &personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        if (personFlow->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedPersonTrips() const {
    int counter = 0;
    // iterate over all person plans
    for (const auto &person : myDemandElements.at(SUMO_TAG_PERSON)) {
        for (const auto &personPlan : person->getChildDemandElements()) {
            if (personPlan->getTagProperty().isPersonTrip() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto &personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto &personPlan : personFlow->getChildDemandElements()) {
            if (personPlan->getTagProperty().isPersonTrip() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    return counter;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedWalks() const {
    int counter = 0;
    // iterate over all person plans
    for (const auto &person : myDemandElements.at(SUMO_TAG_PERSON)) {
        for (const auto &personPlan : person->getChildDemandElements()) {
            if (personPlan->getTagProperty().isWalk() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto &personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto &personPlan : personFlow->getChildDemandElements()) {
            if (personPlan->getTagProperty().isWalk() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    return counter;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedRides() const {
    int counter = 0;
    // iterate over all person plans
    for (const auto &person : myDemandElements.at(SUMO_TAG_PERSON)) {
        for (const auto &personPlan : person->getChildDemandElements()) {
            if (personPlan->getTagProperty().isRide() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto &personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto &personPlan : personFlow->getChildDemandElements()) {
            if (personPlan->getTagProperty().isRide() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    return counter;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedContainers() const {
    int counter = 0;
    // iterate over all containers
    for (const auto &container : myDemandElements.at(SUMO_TAG_CONTAINER)) {
        if (container->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto &containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        if (containerFlow->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedTransport() const {
    int counter = 0;
    // iterate over all container plans
    for (const auto &container : myDemandElements.at(SUMO_TAG_CONTAINER)) {
        for (const auto &containerPlan : container->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isTransportPlan() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto &containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto &containerPlan : containerFlow->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isTransportPlan() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    return counter;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedTranships() const {
    int counter = 0;
    // iterate over all container plans
    for (const auto &container : myDemandElements.at(SUMO_TAG_CONTAINER)) {
        for (const auto &containerPlan : container->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isTranshipPlan() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto &containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto &containerPlan : containerFlow->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isTranshipPlan() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedStops() const {
    int counter = 0;
    // iterate over routes
    for (const auto &route : myDemandElements.at(SUMO_TAG_ROUTE)) {
        if (route->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    // vehicles
    for (const auto &trip : myDemandElements.at(SUMO_TAG_TRIP)) {
        for (const auto &stop : trip->getChildDemandElements()) {
            if (stop->getTagProperty().isStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto &vehicle : myDemandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
        for (const auto &stop : vehicle->getChildDemandElements().front()->getChildDemandElements()) {
            if (stop->getTagProperty().isStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto &flow : myDemandElements.at(SUMO_TAG_FLOW)) {
        for (const auto &stop : flow->getChildDemandElements()) {
            if (stop->getTagProperty().isStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto &flow : myDemandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
        for (const auto &stop : flow->getChildDemandElements().front()->getChildDemandElements()) {
            if (stop->getTagProperty().isStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    // persons
    for (const auto &person : myDemandElements.at(SUMO_TAG_PERSON)) {
        for (const auto &personPlan : person->getChildDemandElements()) {
            if (personPlan->getTagProperty().isStopPerson() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto &personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto &personPlan : personFlow->getChildDemandElements()) {
            if (personPlan->getTagProperty().isStopPerson() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    // containers
    for (const auto &container : myDemandElements.at(SUMO_TAG_CONTAINER)) {
        for (const auto &containerPlan : container->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isStopContainer() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto &containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto &containerPlan : containerFlow->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isStopContainer() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    return counter;
}


const std::set<GNEDataSet*>&
GNENetHelper::AttributeCarriers::getDataSets() const {
    return myDataSets;
}


const std::set<GNEDataInterval*>&
GNENetHelper::AttributeCarriers::getDataIntervals() const {
    return myDataIntervals;
}


const std::map<SumoXMLTag, std::set<GNEGenericData*> >&
GNENetHelper::AttributeCarriers::getGenericDatas() const {
    return myGenericDatas;
}


std::vector<GNEGenericData*>
GNENetHelper::AttributeCarriers::retrieveGenericDatas(const SumoXMLTag genericDataTag, const double begin, const double end) {
    // declare generic data vector
    std::vector<GNEGenericData*> genericDatas;
    // iterate over all data sets
    for (const auto& genericData : myGenericDatas.at(genericDataTag)) {
        // check interval
        if ((genericData->getDataIntervalParent()->getAttributeDouble(SUMO_ATTR_BEGIN) >= begin) && 
            (genericData->getDataIntervalParent()->getAttributeDouble(SUMO_ATTR_END) <= end)) {
            genericDatas.push_back(genericData);
        }
    }
    return genericDatas;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedEdgeDatas() const {
    int counter = 0;
    // iterate over all edgeDatas
    for (const auto& genericData : myGenericDatas.at(SUMO_TAG_MEANDATA_EDGE)) {
        if (genericData->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int 
GNENetHelper::AttributeCarriers::getNumberOfSelectedEdgeRelDatas() const {
    int counter = 0;
    // iterate over all edgeDatas
    for (const auto& genericData : myGenericDatas.at(SUMO_TAG_EDGEREL)) {
        if (genericData->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedEdgeTAZRel() const {
    int counter = 0;
    // iterate over all edgeDatas
    for (const auto& genericData : myGenericDatas.at(SUMO_TAG_TAZREL)) {
        if (genericData->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


void 
GNENetHelper::AttributeCarriers::insertDataInterval(GNEDataInterval* dataInterval) {
    if (myDataIntervals.insert(dataInterval).second == false) {
        throw ProcessError(dataInterval->getTagStr() + " with ID='" + dataInterval->getID() + "' already exist");
    }
}


void 
GNENetHelper::AttributeCarriers::deleteDataInterval(GNEDataInterval* dataInterval) {
    const auto finder = myDataIntervals.find(dataInterval);
    if (finder == myDataIntervals.end()) {
        throw ProcessError(dataInterval->getTagStr() + " with ID='" + dataInterval->getID() + "' wasn't previously inserted");
    } else {
        myDataIntervals.erase(finder);
    }
}


void 
GNENetHelper::AttributeCarriers::insertGenericData(GNEGenericData* genericData) {
    if (myGenericDatas.at(genericData->getTagProperty().getTag()).insert(genericData).second == false) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' already exist");
    } else {
        ;
    }
}


void 
GNENetHelper::AttributeCarriers::deleteGenericData(GNEGenericData* genericData) {
    const auto finder = myGenericDatas.at(genericData->getTagProperty().getTag()).find(genericData);
    if (finder == myGenericDatas.at(genericData->getTagProperty().getTag()).end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' wasn't previously inserted");
    } else {
        myGenericDatas.at(genericData->getTagProperty().getTag()).erase(finder);
    }
}


void
GNENetHelper::AttributeCarriers::insertJunction(GNEJunction* junction) {
    myNet->getNetBuilder()->getNodeCont().insert(junction->getNBNode());
    registerJunction(junction);
}


void
GNENetHelper::AttributeCarriers::deleteSingleJunction(GNEJunction* junction) {
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(junction);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(junction);
    // Remove from grid and container
    myNet->removeGLObjectFromGrid(junction);
    myJunctions.erase(junction->getMicrosimID());
    myNet->getNetBuilder()->getNodeCont().extract(junction->getNBNode());
    junction->decRef("GNENet::deleteSingleJunction");
    junction->setResponsible(true);
}


bool
GNENetHelper::AttributeCarriers::edgeTypeExist(const GNEEdgeType* edgeType) const {
    return (myEdgeTypes.count(edgeType->getID()) > 0);
}


void
GNENetHelper::AttributeCarriers::insertEdgeType(GNEEdgeType* edgeType) {
    // insert in myEdgeTypes
    myEdgeTypes[edgeType->getMicrosimID()] = edgeType;
    // update edge selector
    if (myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->refreshEdgeTypeSelector();
    }
}


void
GNENetHelper::AttributeCarriers::deleteEdgeType(GNEEdgeType* edgeType) {
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(edgeType);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(edgeType);
    // remove from edge types
    myEdgeTypes.erase(edgeType->getMicrosimID());
    // check if this is the selected edge type in edgeSelector
    if (myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->getEdgeTypeSelected() == edgeType) {
        myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->clearEdgeTypeSelected();
        myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->refreshEdgeTypeSelector();
    }
    // update edge selector
    if (myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->refreshEdgeTypeSelector();
    }
}


void
GNENetHelper::AttributeCarriers::insertEdge(GNEEdge* edge) {
    NBEdge* nbe = edge->getNBEdge();
    myNet->getNetBuilder()->getEdgeCont().insert(nbe); // should we ignore pruning double edges?
    // if this edge was previouls extracted from the edgeContainer we have to rewire the nodes
    nbe->getFromNode()->addOutgoingEdge(nbe);
    nbe->getToNode()->addIncomingEdge(nbe);
    // register edge
    registerEdge(edge);
}


void
GNENetHelper::AttributeCarriers::deleteSingleEdge(GNEEdge* edge) {
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(edge);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(edge);
    // remove edge from visual grid and container
    myNet->removeGLObjectFromGrid(edge);
    myEdges.erase(edge->getMicrosimID());
    // remove all lanes
    for (const auto &lane : edge->getLanes()) {
        deleteLane(lane);
    }
    // extract edge of district container
    myNet->getNetBuilder()->getEdgeCont().extract(myNet->getNetBuilder()->getDistrictCont(), edge->getNBEdge());
    edge->decRef("GNENet::deleteSingleEdge");
    edge->setResponsible(true);
    // Remove refrences from GNEJunctions
    edge->getFromJunction()->removeOutgoingGNEEdge(edge);
    edge->getToJunction()->removeIncomingGNEEdge(edge);
}


bool
GNENetHelper::AttributeCarriers::additionalExist(const GNEAdditional* additional) const {
    // first check that additional pointer is valid
    if (additional) {
        // get vector with this additional element type
        const auto &additionalElementTag = myAdditionals.at(additional->getTagProperty().getTag());
        // find demanElement in additionalElementTag
        return std::find(additionalElementTag.begin(), additionalElementTag.end(), additional) != additionalElementTag.end();
    } else {
        throw ProcessError("Invalid additional pointer");
    }
}


void
GNENetHelper::AttributeCarriers::insertAdditional(GNEAdditional* additional) {
    // check if previously was inserted
    if (additionalExist(additional)) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' already exist");
    }
    // insert additional    
    myAdditionals.at(additional->getTagProperty().getTag()).push_back(additional);
    // add element in grid
    if (additional->getTagProperty().isPlacedInRTree()) {
        myNet->addGLObjectIntoGrid(additional);
    }
    // update geometry after insertion of additionals if myUpdateGeometryEnabled is enabled
    if (myNet->isUpdateGeometryEnabled()) {
        additional->updateGeometry();
    }
    // additionals has to be saved
    myNet->requireSaveAdditionals(true);
}


void
GNENetHelper::AttributeCarriers::deleteAdditional(GNEAdditional* additional) {
    // get vector with this additional type
    auto &additionalTag = myAdditionals.at(additional->getTagProperty().getTag());
    // find demanElement in additionalTag
    auto itFind = std::find(additionalTag.begin(), additionalTag.end(), additional);
    // check if additional was previously inserted
    if (itFind == additionalTag.end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(additional);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(additional);
    // remove from container
    additionalTag.erase(itFind);
    // remove element from grid
    if (additional->getTagProperty().isPlacedInRTree()) {
        myNet->removeGLObjectFromGrid(additional);
    }
    // delete path element
    myNet->getPathManager()->removePath(additional);
    // additionals has to be saved
    myNet->requireSaveAdditionals(true);
}


bool
GNENetHelper::AttributeCarriers::shapeExist(const GNEShape* shape) const {
    // first check that shape pointer is valid
    if (shape) {
        // get vector with this shape element type
        const auto &shapeElementTag = myShapes.at(shape->getTagProperty().getTag());
        // find demanElement in shapeElementTag
        return std::find(shapeElementTag.begin(), shapeElementTag.end(), shape) != shapeElementTag.end();
    } else {
        throw ProcessError("Invalid shape pointer");
    }
}


void
GNENetHelper::AttributeCarriers::insertShape(GNEShape* shape) {
    // Check if shape element exists before insertion
    if (shapeExist(shape)) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' already exist");
    }
    // insert shape
    myShapes.at(shape->getTagProperty().getTag()).push_back(shape);
    // add element in grid
    myNet->addGLObjectIntoGrid(shape);
    // update geometry after insertion of shapes if myUpdateGeometryEnabled is enabled
    if (myNet->isUpdateGeometryEnabled()) {
        shape->updateGeometry();
    }
    // shapes has to be saved
    myNet->requireSaveAdditionals(true);
}


void
GNENetHelper::AttributeCarriers::deleteShape(GNEShape* shape) {
    // get vector with this shape type
    auto &shapeTag = myShapes.at(shape->getTagProperty().getTag());
    // find demanElement in shapeTag
    auto itFind = std::find(shapeTag.begin(), shapeTag.end(), shape);
    // check if shape was previously inserted
    if (itFind == shapeTag.end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(shape);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(shape);
    // remove it from container
    shapeTag.erase(itFind);
    // remove element from grid
    myNet->removeGLObjectFromGrid(shape);
    // shapes has to be saved
    myNet->requireSaveAdditionals(true);
}


void
GNENetHelper::AttributeCarriers::insertTAZElement(GNETAZElement* TAZElement) {
    // Check if TAZElement element exists before insertion
    if (TAZElementExist(TAZElement)) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' already exist");
    }
    // insert TAZElement
    myTAZElements.at(TAZElement->getTagProperty().getTag()).push_back(TAZElement);
    // add element in grid
    myNet->addGLObjectIntoGrid(TAZElement);
    // update geometry after insertion of TAZElements if myUpdateGeometryEnabled is enabled
    if (myNet->isUpdateGeometryEnabled()) {
        TAZElement->updateGeometry();
    }
    // TAZElements has to be saved
    myNet->requireSaveAdditionals(true);
}


void
GNENetHelper::AttributeCarriers::deleteTAZElement(GNETAZElement* TAZElement) {
    // get vector with this TAZElement type
    auto &TAZElementTag = myTAZElements.at(TAZElement->getTagProperty().getTag());
    // find demanElement in TAZElementTag
    auto itFind = std::find(TAZElementTag.begin(), TAZElementTag.end(), TAZElement);
    // check if TAZElement was previously inserted
    if (itFind == TAZElementTag.end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(TAZElement);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(TAZElement);
    // remove it from container
    TAZElementTag.erase(itFind);
    // remove element from grid
    myNet->removeGLObjectFromGrid(TAZElement);
    // TAZElements has to be saved
    myNet->requireSaveAdditionals(true);
}


bool
GNENetHelper::AttributeCarriers::demandElementExist(GNEDemandElement* demandElement) const {
    // first check that demandElement pointer is valid
    if (demandElement) {
        // get vector with this demand element type
        const auto &demandElementTag = myDemandElements.at(demandElement->getTagProperty().getTag());
        // find demanElement in demandElementTag
        return std::find(demandElementTag.begin(), demandElementTag.end(), demandElement) != demandElementTag.end();
    } else {
        throw ProcessError("Invalid demandElement pointer");
    }
}


void
GNENetHelper::AttributeCarriers::insertDemandElement(GNEDemandElement* demandElement) {
    // Check if demandElement element exists before insertion
    if (demandElementExist(demandElement)) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' already exist");
    }
    // insert in demandElements container
    myDemandElements.at(demandElement->getTagProperty().getTag()).push_back(demandElement);
    // add element in grid
    myNet->addGLObjectIntoGrid(demandElement);
    // update geometry after insertion of demandElements if myUpdateGeometryEnabled is enabled
    if (myNet->isUpdateGeometryEnabled()) {
        demandElement->updateGeometry();
    }
    // demandElements has to be saved
    myNet->requireSaveDemandElements(true);
}


void
GNENetHelper::AttributeCarriers::deleteDemandElement(GNEDemandElement* demandElement) {
    // get vector with this demand element type
    auto &demandElementTag = myDemandElements.at(demandElement->getTagProperty().getTag());
    // find demanElement in demandElementTag
    auto itFind = std::find(demandElementTag.begin(), demandElementTag.end(), demandElement);
    // check if demandElement was previously inserted
    if (itFind == demandElementTag.end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(demandElement);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(demandElement);
    // erase it from container
    demandElementTag.erase(itFind);
    // remove element from grid
    myNet->removeGLObjectFromGrid(demandElement);
    // delete path element
    myNet->getPathManager()->removePath(demandElement);
    // demandElements has to be saved
    myNet->requireSaveDemandElements(true);
}


bool
GNENetHelper::AttributeCarriers::dataSetExist(GNEDataSet* dataSet) const {
    // first check that dataSet pointer is valid
    if (dataSet) {
        if (myDataSets.find(dataSet) != myDataSets.end()) {
            return true;
        } else {
            return false;
        }
    } else {
        throw ProcessError("Invalid dataSet pointer");
    }
}


void
GNENetHelper::AttributeCarriers::insertDataSet(GNEDataSet* dataSet) {
    // Check if dataSet element exists before insertion
    if (myDataSets.insert(dataSet).second == false) {
        throw ProcessError(dataSet->getTagStr() + " with ID='" + dataSet->getID() + "' already exist");
    }
    // dataSets has to be saved
    myNet->requireSaveDataElements(true);
    // update interval toolbar
    myNet->getViewNet()->getIntervalBar().updateIntervalBar();
}


void
GNENetHelper::AttributeCarriers::deleteDataSet(GNEDataSet* dataSet) {
    // find dataSet
    const auto itFind = myDataSets.find(dataSet);
    // first check that dataSet pointer is valid
    if (itFind == myDataSets.end()) {
        throw ProcessError(dataSet->getTagStr() + " with ID='" + dataSet->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(dataSet);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(dataSet);
    // obtain demand element and erase it from container
    myDataSets.erase(itFind);
    // dataSets has to be saved
    myNet->requireSaveDataElements(true);
    // update interval toolbar
    myNet->getViewNet()->getIntervalBar().updateIntervalBar();
}

// ---------------------------------------------------------------------------
// GNENetHelper::GNEChange_ReplaceEdgeInTLS - methods
// ---------------------------------------------------------------------------

GNENetHelper::GNEChange_ReplaceEdgeInTLS::GNEChange_ReplaceEdgeInTLS(NBTrafficLightLogicCont& tllcont, NBEdge* replaced, NBEdge* by) :
    GNEChange(Supermode::NETWORK, true, false),
    myTllcont(tllcont),
    myReplaced(replaced),
    myBy(by) {
}


GNENetHelper::GNEChange_ReplaceEdgeInTLS::~GNEChange_ReplaceEdgeInTLS() {}


void
GNENetHelper::GNEChange_ReplaceEdgeInTLS::undo() {
    // assuming this is only used for replacing incoming connections (GNENet::replaceIncomingEdge)
    myTllcont.replaceRemoved(myBy, -1, myReplaced, -1, true);
}


void
GNENetHelper::GNEChange_ReplaceEdgeInTLS::redo() {
    // assuming this is only used for replacing incoming connections (GNENet::replaceIncomingEdge)
    myTllcont.replaceRemoved(myReplaced, -1, myBy, -1, true);
}


std::string
GNENetHelper::GNEChange_ReplaceEdgeInTLS::undoName() const {
    return "Redo replace in TLS";
}


std::string
GNENetHelper::GNEChange_ReplaceEdgeInTLS::redoName() const {
    return "Undo replace in TLS";
}


bool
GNENetHelper::GNEChange_ReplaceEdgeInTLS::trueChange() {
    return myReplaced != myBy;
}

/****************************************************************************/
