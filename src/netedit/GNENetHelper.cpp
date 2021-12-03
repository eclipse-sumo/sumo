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
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNEPOI.h>
#include <netedit/elements/additional/GNEPoly.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/demand/GNEVehicleType.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/elements/network/GNEEdgeTemplate.h>
#include <netedit/elements/network/GNEEdgeType.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>

#include "GNENetHelper.h"

// ---------------------------------------------------------------------------
// GNENetHelper::AttributeCarriers - methods
// ---------------------------------------------------------------------------

GNENetHelper::AttributeCarriers::AttributeCarriers(GNENet* net) :
    myNet(net) {
    // fill additionals with tags
    auto additionalTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::ADDITIONALELEMENT | GNETagProperties::TagType::SYMBOL, false);
    for (const auto& additionalTag : additionalTags) {
        myAdditionals.insert(std::make_pair(additionalTag.first.getTag(), std::set<GNEAdditional*>()));
    }
    // fill shapes with tags
    auto shapeTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::SHAPE, false);
    for (const auto& shapeTag : shapeTags) {
        myShapes.insert(std::make_pair(shapeTag.first.getTag(), std::set<GNEShape*>()));
    }
    // fill TAZElements with tags
    auto TAZElementTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::TAZELEMENT, false);
    for (const auto& TAZElementTag : TAZElementTags) {
        myTAZElements.insert(std::make_pair(TAZElementTag.first.getTag(), std::set<GNETAZElement*>()));
    }
    // fill demand elements with tags
    auto demandElementTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::DEMANDELEMENT, false);
    for (const auto& demandElementTag : demandElementTags) {
        myDemandElements.insert(std::make_pair(demandElementTag.first.getTag(), std::set<GNEDemandElement*>()));
    }
    auto stopTags = GNEAttributeCarrier::getAllowedTagPropertiesByCategory(GNETagProperties::TagType::STOP, false);
    for (const auto& stopTag : stopTags) {
        myDemandElements.insert(std::make_pair(stopTag.first.getTag(), std::set<GNEDemandElement*>()));
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
            WRITE_DEBUG("Deleting unreferenced " + additional->getTagStr() + " in AttributeCarriers destructor");
            delete additional;
        }
    }
    // Drop Shapes (Only used for shapes that were inserted without using GNEChange_Shape)
    for (const auto& shapeTag : myShapes) {
        for (const auto& shape : shapeTag.second) {
            // decrease reference manually (because it was increased manually in GNEShapeHandler)
            shape->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + shape->getTagStr() + " in AttributeCarriers destructor");
            delete shape;
        }
    }
    // Drop TAZElements (Only used for TAZElements that were inserted without using GNEChange_TAZElement)
    for (const auto& TAZElementTag : myTAZElements) {
        for (const auto& TAZElement : TAZElementTag.second) {
            // decrease reference manually (because it was increased manually in GNETAZElementHandler)
            TAZElement->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + TAZElement->getTagStr() + " in AttributeCarriers destructor");
            delete TAZElement;
        }
    }
    // Drop demand elements (Only used for demand elements that were inserted without using GNEChange_DemandElement, for example the default VType")
    for (const auto& demandElementTag : myDemandElements) {
        for (const auto& demandElement : demandElementTag.second) {
            // decrease reference manually (because it was increased manually in GNERouteHandler)
            demandElement->decRef();
            // show extra information for tests (except for default IDs)
            if ((demandElement->getID() != DEFAULT_VTYPE_ID) && (demandElement->getID() != DEFAULT_BIKETYPE_ID) && (demandElement->getID() != DEFAULT_PEDTYPE_ID)) {
                WRITE_DEBUG("Deleting unreferenced " + demandElement->getTagStr() + " in AttributeCarriers destructor");
            }
            delete demandElement;
        }
    }
    // Drop dataSets (Only used for TAZElements that were inserted without using GNEChange_DataSets)
    for (const auto& dataSet : myDataSets) {
        // decrease reference manually (because it was increased manually in GNEDataHandler)
        dataSet->decRef();
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + dataSet->getTagStr() + " in AttributeCarriers destructor");
        delete dataSet;
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
GNENetHelper::AttributeCarriers::isNetworkElementAroundShape(GNEAttributeCarrier* AC, const PositionVector& shape) const {
    // check what tipe of AC
    if (AC->getTagProperty().getTag() == SUMO_TAG_JUNCTION) {
        // Junction
        const GNEJunction* junction = myJunctions.at(AC->getID());
        if (junction->getNBNode()->getShape().size() == 0) {
            return shape.around(junction->getNBNode()->getCenter());
        } else {
            return (shape.overlapsWith(junction->getNBNode()->getShape()));
        }
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_EDGE) {
        // Edge
        for (const auto& lane : myEdges.at(AC->getID())->getLanes()) {
            if (shape.overlapsWith(lane->getLaneShape())) {
                return true;
            }
        }
        return false;
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_LANE) {
        // Lane
        return shape.overlapsWith(retrieveLane(AC->getID())->getLaneShape());
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_CONNECTION) {
        // connection
        return shape.overlapsWith(dynamic_cast<GNEConnection*>(AC)->getConnectionShape());
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_CROSSING) {
        // crossing
        return shape.overlapsWith(dynamic_cast<GNECrossing*>(AC)->getCrossingShape());
    } else if (AC->getTagProperty().isAdditionalElement()) {
        // Additional
        const GNEAdditional* additional = retrieveAdditional(AC);
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
            return shape.around(dynamic_cast<GNEPOI*>(AC)->getPositionInView());
        }
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        // TAZ
        return shape.overlapsWith(dynamic_cast<GNETAZ*>(AC)->getTAZElementShape());
    } else {
        return false;
    }
}


GNEAttributeCarrier*
GNENetHelper::AttributeCarriers::retrieveAttributeCarrier(const GUIGlID id, bool hardFail) const {
    // obtain blocked GUIGlObject
    GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    // Make sure that object exists
    if (object != nullptr) {
        // unblock and try to parse to AtributeCarrier
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        GNEAttributeCarrier* ac = dynamic_cast<GNEAttributeCarrier*>(object);
        // If was sucesfully parsed, return it
        if (ac == nullptr) {
            throw ProcessError("GUIGlObject does not match the declared type");
        } else {
            return ac;
        }
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant GUIGlObject");
    } else {
        return nullptr;
    }
}


std::vector<GNEAttributeCarrier*>
GNENetHelper::AttributeCarriers::retrieveAttributeCarriers(SumoXMLTag tag) {
    std::vector<GNEAttributeCarrier*> result;
    if ((tag == SUMO_TAG_NOTHING) || (tag == SUMO_TAG_JUNCTION)) {
        for (const auto& junction : myJunctions) {
            result.push_back(junction.second);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (tag == SUMO_TAG_EDGE)) {
        for (const auto& edge : myEdges) {
            result.push_back(edge.second);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (tag == SUMO_TAG_LANE)) {
        for (const auto& lane : myLanes) {
            result.push_back(lane);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (tag == SUMO_TAG_CONNECTION)) {
        for (const auto& connection : myConnections) {
            result.push_back(connection);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (tag == SUMO_TAG_CROSSING)) {
        for (const auto& crossing : myCrossings) {
            result.push_back(crossing);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (GNEAttributeCarrier::getTagProperties(tag).isAdditionalElement())) {
        for (const auto& additional : myAdditionals.at(tag)) {
            result.push_back(additional);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (GNEAttributeCarrier::getTagProperties(tag).isShape())) {
        for (const auto& shape : myShapes.at(tag)) {
            result.push_back(shape);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (GNEAttributeCarrier::getTagProperties(tag).isTAZElement())) {
        for (const auto& TAZElement : myTAZElements.at(tag)) {
            result.push_back(TAZElement);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (GNEAttributeCarrier::getTagProperties(tag).isDemandElement())) {
        for (const auto& demandElemet : myDemandElements.at(tag)) {
            result.push_back(demandElemet);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (tag == SUMO_TAG_DATASET)) {
        for (const auto& dataSet : myDataSets) {
            result.push_back(dataSet);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (tag == SUMO_TAG_DATAINTERVAL)) {
        for (const auto& dataInterval : myDataIntervals) {
            result.push_back(dataInterval);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (GNEAttributeCarrier::getTagProperties(tag).isGenericData())) {
        for (const auto& genericData : myGenericDatas.at(tag)) {
            result.push_back(genericData);
        }
    }
    return result;
}


std::vector<GNEAttributeCarrier*>
GNENetHelper::AttributeCarriers::retrieveAttributeCarriers(Supermode supermode, const bool onlySelected) {
    std::vector<GNEAttributeCarrier*> result;
    // continue depending of supermode
    if (supermode == Supermode::NETWORK) {
        // network
        for (const auto& junction : myJunctions) {
            if (!onlySelected || junction.second->isAttributeCarrierSelected()) {
                result.push_back(junction.second);
            }
        }
        for (const auto& crossing : myCrossings) {
            if (!onlySelected || crossing->isAttributeCarrierSelected()) {
                result.push_back(crossing);
            }
        }
        for (const auto& edge : myEdges) {
            if (!onlySelected || edge.second->isAttributeCarrierSelected()) {
                result.push_back(edge.second);
            }
        }
        for (const auto& lane : myLanes) {
            if (!onlySelected || lane->isAttributeCarrierSelected()) {
                result.push_back(lane);
            }
        }
        for (const auto& connection : myConnections) {
            if (!onlySelected || connection->isAttributeCarrierSelected()) {
                result.push_back(connection);
            }
        }
        for (const auto& additionalSet : myAdditionals) {
            for (const auto& additional : additionalSet.second) {
                if (!onlySelected || additional->isAttributeCarrierSelected()) {
                    result.push_back(additional);
                }
            }
        }
        for (const auto& shapeSet : myShapes) {
            for (const auto& shape : shapeSet.second) {
                if (!onlySelected || shape->isAttributeCarrierSelected()) {
                    result.push_back(shape);
                }
            }
        }
        for (const auto& TAZSet : myTAZElements) {
            for (const auto& TAZElement : TAZSet.second) {
                if (!onlySelected || TAZElement->isAttributeCarrierSelected()) {
                    result.push_back(TAZElement);
                }
            }
        }
    } else if (supermode == Supermode::DEMAND) {
        for (const auto& demandElementSet : myDemandElements) {
            for (const auto& demandElement : demandElementSet.second) {
                if (!onlySelected || demandElement->isAttributeCarrierSelected()) {
                    result.push_back(demandElement);
                }
            }
        }
    } else if (supermode == Supermode::DATA) {
        for (const auto& dataSet : myDataSets) {
            if (!onlySelected || dataSet->isAttributeCarrierSelected()) {
                result.push_back(dataSet);
            }
        }
        for (const auto& dataInterval : myDataIntervals) {
            if (!onlySelected || dataInterval->isAttributeCarrierSelected()) {
                result.push_back(dataInterval);
            }
        }
        for (const auto& genericDataSet : myGenericDatas) {
            for (const auto& genericData : genericDataSet.second) {
                if (!onlySelected || genericData->isAttributeCarrierSelected()) {
                    result.push_back(genericData);
                }
            }
        }
    }
    return result;
}


std::vector<GNEAttributeCarrier*>
GNENetHelper::AttributeCarriers::getSelectedAttributeCarriers(const bool ignoreCurrentSupermode) {
    // get modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // declare vector to save result
    std::vector<GNEAttributeCarrier*> result;
    result.reserve(gSelected.getSelected().size());
    // iterate over all elements of global selection
    for (const auto& glID : gSelected.getSelected()) {
        // obtain AC
        GNEAttributeCarrier* AC = retrieveAttributeCarrier(glID, false);
        // check if attribute carrier exist and is selected
        if (AC && AC->isAttributeCarrierSelected()) {
            bool insert = false;
            if (ignoreCurrentSupermode) {
                insert = true;
            } else if (editModes.isCurrentSupermodeNetwork() && (AC->getTagProperty().isNetworkElement() ||
                       AC->getTagProperty().isAdditionalElement() || AC->getTagProperty().isShape() || AC->getTagProperty().isTAZElement())) {
                insert = true;
            } else if (editModes.isCurrentSupermodeDemand() && AC->getTagProperty().isDemandElement()) {
                insert = true;
            } else if (editModes.isCurrentSupermodeData() && AC->getTagProperty().isDataElement()) {
                insert = true;
            }
            if (insert) {
                result.push_back(AC);
            }
        }
    }
    return result;
}


GNEJunction*
GNENetHelper::AttributeCarriers::retrieveJunction(const std::string& id, bool hardFail) const {
    if (myJunctions.count(id)) {
        return myJunctions.at(id);
    } else if (hardFail) {
        // If junction wasn't found, throw exception
        throw UnknownElement("Junction " + id);
    } else {
        return nullptr;
    }
}


const std::map<std::string, GNEJunction*>&
GNENetHelper::AttributeCarriers::getJunctions() const {
    return myJunctions;
}


std::vector<GNEJunction*>
GNENetHelper::AttributeCarriers::getSelectedJunctions() const {
    std::vector<GNEJunction*> result;
    // returns junctions depending of selection
    for (const auto& junction : myJunctions) {
        if (junction.second->isAttributeCarrierSelected()) {
            result.push_back(junction.second);
        }
    }
    return result;
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
    for (const auto& junction : myJunctions) {
        if (junction.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


GNECrossing*
GNENetHelper::AttributeCarriers::retrieveCrossing(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast crossing
    GNECrossing* crossing = dynamic_cast<GNECrossing*>(AC);
    if (crossing && (myCrossings.count(crossing) > 0)) {
        return crossing;
    } else if (hardFail) {
        throw UnknownElement("Crossing " + AC->getID());
    } else {
        return nullptr;
    }
}


const std::set<GNECrossing*>&
GNENetHelper::AttributeCarriers::getCrossings() const {
    return myCrossings;
}


std::vector<GNECrossing*>
GNENetHelper::AttributeCarriers::getSelectedCrossings() const {
    std::vector<GNECrossing*> result;
    // iterate over crossings
    for (const auto& crossing : myCrossings) {
        if (crossing->isAttributeCarrierSelected()) {
            result.push_back(crossing);
        }
    }
    return result;
}


void
GNENetHelper::AttributeCarriers::insertCrossing(GNECrossing* crossing) {
    if (myCrossings.insert(crossing).second == false) {
        throw ProcessError(crossing->getTagStr() + " with ID='" + crossing->getID() + "' already exist");
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
    for (const auto& crossing : myCrossings) {
        if (crossing->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


GNEEdgeType*
GNENetHelper::AttributeCarriers::retrieveEdgeType(const std::string& id, bool hardFail) const {
    if (myEdgeTypes.count(id) > 0) {
        return myEdgeTypes.at(id);
    } else if (hardFail) {
        // If edge wasn't found, throw exception
        throw UnknownElement("EdgeType " + id);
    } else {
        return nullptr;
    }
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


std::string
GNENetHelper::AttributeCarriers::generateEdgeTypeID() const {
    int counter = 0;
    while (myEdgeTypes.count("edgeType_" + toString(counter)) != 0) {
        counter++;
    }
    return ("edgeType_" + toString(counter));
}


GNEEdge*
GNENetHelper::AttributeCarriers::retrieveEdge(const std::string& id, bool hardFail) const {
    if (myEdges.count(id) > 0) {
        return myEdges.at(id);
    } else if (hardFail) {
        // If edge wasn't found, throw exception
        throw UnknownElement("Edge " + id);
    } else {
        return nullptr;
    }
}


GNEEdge*
GNENetHelper::AttributeCarriers::retrieveEdge(GNEJunction* from, GNEJunction* to, bool hardFail) const {
    if ((from == nullptr) || (to == nullptr)) {
        throw UnknownElement("Junctions cannot be nullptr");
    }
    // iterate over Junctions
    for (const auto& edge : myEdges) {
        if ((edge.second->getFromJunction() == from) && (edge.second->getToJunction() == to)) {
            return edge.second;
        }
    }
    // if edge wasn't found, throw exception or return nullptr
    if (hardFail) {
        throw UnknownElement("Edge with from='" + from->getID() + "' and to='" + to->getID() + "'");
    } else {
        return nullptr;
    }
}


const std::map<std::string, GNEEdge*>&
GNENetHelper::AttributeCarriers::getEdges() const {
    return myEdges;
}


std::vector<GNEEdge*>
GNENetHelper::AttributeCarriers::getSelectedEdges() const {
    std::vector<GNEEdge*> result;
    // returns edges depending of selection
    for (const auto& edge : myEdges) {
        if (edge.second->isAttributeCarrierSelected()) {
            result.push_back(edge.second);
        }
    }
    return result;
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
    for (const auto& lane : edge->getLanes()) {
        insertLane(lane);
    }
    // Add references into GNEJunctions
    edge->getFromJunction()->addOutgoingGNEEdge(edge);
    edge->getToJunction()->addIncomingGNEEdge(edge);
    return edge;
}


void GNENetHelper::AttributeCarriers::clearEdges() {
    myEdges.clear();
}


void
GNENetHelper::AttributeCarriers::updateEdgeID(GNEEdge* edge, const std::string& newID) {
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
    for (const auto& edge : myEdges) {
        if (edge.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


GNELane*
GNENetHelper::AttributeCarriers::retrieveLane(const std::string& id, bool hardFail, bool checkVolatileChange) const {
    const std::string edge_id = SUMOXMLDefinitions::getEdgeIDFromLane(id);
    const GNEEdge* edge = retrieveEdge(edge_id, false);
    if (edge != nullptr) {
        GNELane* lane = nullptr;
        // search  lane in lane's edges
        for (auto laneIt : edge->getLanes()) {
            if (laneIt->getID() == id) {
                lane = laneIt;
            }
        }
        // throw exception or return nullptr if lane wasn't found
        if (lane == nullptr) {
            if (hardFail) {
                // Throw exception if hardFail is enabled
                throw UnknownElement(toString(SUMO_TAG_LANE) + " " + id);
            }
        } else {
            // check if the recomputing with volatile option has changed the number of lanes (needed for additionals and demand elements)
            if (checkVolatileChange && (myNet->getEdgesAndNumberOfLanes().count(edge_id) == 1) &&
                    myNet->getEdgesAndNumberOfLanes().at(edge_id) != (int)edge->getLanes().size()) {
                return edge->getLanes().at(lane->getIndex() + 1);
            }
            return lane;
        }
    } else if (hardFail) {
        // Throw exception if hardFail is enabled
        throw UnknownElement(toString(SUMO_TAG_EDGE) + " " + edge_id);
    }
    return nullptr;
}


GNELane*
GNENetHelper::AttributeCarriers::retrieveLane(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast lane
    GNELane* lane = dynamic_cast<GNELane*>(AC);
    if (lane && (myLanes.count(lane) > 0)) {
        return lane;
    } else if (hardFail) {
        throw UnknownElement("Lane " + AC->getID());
    } else {
        return nullptr;
    }
}


const std::set<GNELane*>&
GNENetHelper::AttributeCarriers::getLanes() const {
    return myLanes;
}


std::vector<GNELane*>
GNENetHelper::AttributeCarriers::getSelectedLanes() const {
    std::vector<GNELane*> result;
    // returns lanes depending of selection
    for (const auto& lane : myLanes) {
        if (lane->isAttributeCarrierSelected()) {
            result.push_back(lane);
        }
    }
    return result;
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
    for (const auto& lane : myLanes) {
        if (lane->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


GNEConnection*
GNENetHelper::AttributeCarriers::retrieveConnection(const std::string& id, bool hardFail) const {
    // iterate over connections
    for (const auto& connection : myConnections) {
        if (connection->getID() == id) {
            return connection;
        }
    }
    if (hardFail) {
        // If POI wasn't found, throw exception
        throw UnknownElement("Connection " + id);
    } else {
        return nullptr;
    }
}


GNEConnection*
GNENetHelper::AttributeCarriers::retrieveConnection(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast connection
    GNEConnection* connection = dynamic_cast<GNEConnection*>(AC);
    if (connection && (myConnections.count(connection) > 0)) {
        return connection;
    } else if (hardFail) {
        throw UnknownElement("Connection " + AC->getID());
    } else {
        return nullptr;
    }
}


const std::set<GNEConnection*>&
GNENetHelper::AttributeCarriers::getConnections() const {
    return myConnections;
}


std::vector<GNEConnection*>
GNENetHelper::AttributeCarriers::getSelectedConnections() const {
    std::vector<GNEConnection*> result;
    // returns connections depending of selection
    for (const auto& connection : myConnections) {
        if (connection->isAttributeCarrierSelected()) {
            result.push_back(connection);
        }
    }
    return result;
}


void
GNENetHelper::AttributeCarriers::insertConnection(GNEConnection* connection) {
    if (myConnections.insert(connection).second == false) {
        throw ProcessError(connection->getTagStr() + " with ID='" + connection->getID() + "' already exist");
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
    for (const auto& connection : myConnections) {
        if (connection->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


GNEAdditional*
GNENetHelper::AttributeCarriers::retrieveAdditional(SumoXMLTag type, const std::string& id, bool hardFail) const {
    for (const auto& additional : myAdditionals.at(type)) {
        if (additional->getID() == id) {
            return additional;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant additional (string)");
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNENetHelper::AttributeCarriers::retrieveAdditional(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast additional
    GNEAdditional* additional = dynamic_cast<GNEAdditional*>(AC);
    if (additional && (myAdditionals.at(AC->getTagProperty().getTag()).count(additional) > 0)) {
        return additional;
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant additional (AttributeCarrier)");
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNENetHelper::AttributeCarriers::retrieveRerouterInterval(const std::string& rerouterID, const SUMOTime begin, const SUMOTime end) const {
    // first retrieve rerouter
    const GNEAdditional* rerouter = retrieveAdditional(SUMO_TAG_REROUTER, rerouterID);
    // parse begin and end
    const std::string beginStr = time2string(begin);
    const std::string endStr = time2string(end);
    // now iterate over all children and check begin and end
    for (const auto& interval : rerouter->getChildAdditionals()) {
        // check tag (to avoid symbols)
        if (interval->getTagProperty().getTag() == SUMO_TAG_INTERVAL) {
            // check begin and end
            if ((interval->getAttribute(SUMO_ATTR_BEGIN) == beginStr) &&
                    (interval->getAttribute(SUMO_ATTR_END) == endStr)) {
                return interval;
            }
        }
    }
    // throw exception
    throw ProcessError("Attempted to retrieve non-existant rerouter interval");
}


const std::map<SumoXMLTag, std::set<GNEAdditional*> >&
GNENetHelper::AttributeCarriers::getAdditionals() const {
    return myAdditionals;
}


std::vector<GNEAdditional*>
GNENetHelper::AttributeCarriers::getSelectedAdditionals() const {
    std::vector<GNEAdditional*> result;
    // returns additionals depending of selection
    for (const auto& additionalsTags : myAdditionals) {
        for (const auto& additional : additionalsTags.second) {
            if (additional->isAttributeCarrierSelected()) {
                result.push_back(additional);
            }
        }
    }
    return result;
}


int
GNENetHelper::AttributeCarriers::getNumberOfAdditionals() const {
    int counter = 0;
    for (const auto& additionalsTag : myAdditionals) {
        counter += (int)additionalsTag.second.size();
    }
    return counter;
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
        }
    }
    return counter;
}


std::string
GNENetHelper::AttributeCarriers::generateAdditionalID(SumoXMLTag type) const {
    int counter = 0;
    while (retrieveAdditional(type, toString(type) + "_" + toString(counter), false) != nullptr) {
        counter++;
    }
    return (toString(type) + "_" + toString(counter));
}


GNEShape*
GNENetHelper::AttributeCarriers::retrieveShape(SumoXMLTag type, const std::string& id, bool hardFail) const {
    for (const auto& shape : myShapes.at(type)) {
        if (shape->getID() == id) {
            return shape;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant shape");
    } else {
        return nullptr;
    }
}


GNEShape*
GNENetHelper::AttributeCarriers::retrieveShape(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast shape
    GNEShape* shape = dynamic_cast<GNEShape*>(AC);
    if (shape && (myShapes.at(AC->getTagProperty().getTag()).count(shape) > 0)) {
        return shape;
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant shape");
    } else {
        return nullptr;
    }
}


std::vector<GNEShape*>
GNENetHelper::AttributeCarriers::getSelectedShapes() {
    std::vector<GNEShape*> result;
    // return all polygons and POIs
    for (const auto& shapeTag : myShapes) {
        for (const auto& shape : shapeTag.second) {
            if (shape->isAttributeCarrierSelected()) {
                result.push_back(shape);
            }
        }
    }
    return result;
}


const std::map<SumoXMLTag, std::set<GNEShape*> >&
GNENetHelper::AttributeCarriers::getShapes() const {
    return myShapes;
}


std::string
GNENetHelper::AttributeCarriers::generateShapeID(SumoXMLTag tag) const {
    int counter = 0;
    // generate tag depending of shape tag
    if (tag == SUMO_TAG_POLY) {
        // Polys and TAZs share namespace
        while ((retrieveShape(SUMO_TAG_POLY, toString(tag) + "_" + toString(counter), false) != nullptr) ||
                (retrieveTAZElement(SUMO_TAG_TAZ, toString(tag) + "_" + toString(counter), false) != nullptr)) {
            counter++;
        }
        return (toString(tag) + "_" + toString(counter));
    } else {
        const std::string POI = toString(SUMO_TAG_POI);
        while ((retrieveShape(SUMO_TAG_POI, POI + "_" + toString(counter), false) != nullptr) ||
                (retrieveShape(GNE_TAG_POILANE, POI + "_" + toString(counter), false) != nullptr) ||
                (retrieveShape(GNE_TAG_POIGEO, POI + "_" + toString(counter), false) != nullptr)) {
            counter++;
        }
        return (toString(tag) + "_" + toString(counter));
    }
}


int
GNENetHelper::AttributeCarriers::getNumberOfShapes() const {
    int counter = 0;
    for (const auto& shapeTag : myShapes) {
        counter += (int)shapeTag.second.size();
    }
    return counter;
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
    for (const auto& poly : myShapes.at(SUMO_TAG_POLY)) {
        if (poly->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedPOIs() const {
    int counter = 0;
    for (const auto& POI : myShapes.at(SUMO_TAG_POI)) {
        if (POI->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& POILane : myShapes.at(GNE_TAG_POILANE)) {
        if (POILane->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& POIGEO : myShapes.at(GNE_TAG_POIGEO)) {
        if (POIGEO->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


GNETAZElement*
GNENetHelper::AttributeCarriers::retrieveTAZElement(SumoXMLTag type, const std::string& id, bool hardFail) const {
    for (const auto& TAZElement : myTAZElements.at(type)) {
        if (TAZElement->getID() == id) {
            return TAZElement;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant TAZElement");
    } else {
        return nullptr;
    }
}


GNETAZElement*
GNENetHelper::AttributeCarriers::retrieveTAZElement(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast TAZElement
    GNETAZElement* TAZElement = dynamic_cast<GNETAZElement*>(AC);
    if (TAZElement && (myTAZElements.at(AC->getTagProperty().getTag()).count(TAZElement) > 0)) {
        return TAZElement;
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant TAZElement");
    } else {
        return nullptr;
    }
}


std::vector<GNETAZElement*>
GNENetHelper::AttributeCarriers::getSelectedTAZElements() const {
    std::vector<GNETAZElement*> result;
    // returns TAZElements depending of selection
    for (const auto& TAZElementTags : myTAZElements) {
        for (const auto& TAZElement : TAZElementTags.second) {
            if (TAZElement->isAttributeCarrierSelected()) {
                result.push_back(TAZElement);
            }
        }
    }
    return result;
}


const std::map<SumoXMLTag, std::set<GNETAZElement*> >&
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
GNENetHelper::AttributeCarriers::getNumberOfTAZElements() const {
    int counter = 0;
    for (const auto& TAZElementsTag : myTAZElements) {
        counter += (int)TAZElementsTag.second.size();
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedTAZs() const {
    int counter = 0;
    for (const auto& poly : myTAZElements.at(SUMO_TAG_TAZ)) {
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
        const auto& TAZElementTag = myTAZElements.at(TAZElement->getTagProperty().getTag());
        // find demanElement in TAZElementTag
        return std::find(TAZElementTag.begin(), TAZElementTag.end(), TAZElement) != TAZElementTag.end();
    } else {
        throw ProcessError("Invalid TAZElement pointer");
    }
}


std::string
GNENetHelper::AttributeCarriers::generateTAZElementID(SumoXMLTag tag) const {
    int counter = 0;
    // generate tag depending of shape tag
    if (tag == SUMO_TAG_TAZ) {
        // Polys and TAZs share namespace
        while ((retrieveShape(SUMO_TAG_POLY, toString(tag) + "_" + toString(counter), false) != nullptr) ||
                (retrieveTAZElement(SUMO_TAG_TAZ, toString(tag) + "_" + toString(counter), false) != nullptr)) {
            counter++;
        }
        return (toString(tag) + "_" + toString(counter));
    } else {
        while (retrieveTAZElement(tag, toString(tag) + "_" + toString(counter)) != nullptr) {
            counter++;
        }
        return (toString(tag) + "_" + toString(counter));
    }
}


GNEDemandElement*
GNENetHelper::AttributeCarriers::retrieveDemandElement(SumoXMLTag type, const std::string& id, bool hardFail) const {
    for (const auto& demandElement : myDemandElements.at(type)) {
        if (demandElement->getID() == id) {
            return demandElement;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant demand element (string)");
    } else {
        return nullptr;
    }
}


GNEDemandElement*
GNENetHelper::AttributeCarriers::retrieveDemandElement(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast demandElement
    GNEDemandElement* demandElement = dynamic_cast<GNEDemandElement*>(AC);
    if (demandElement && (myDemandElements.at(AC->getTagProperty().getTag()).count(demandElement) > 0)) {
        return demandElement;
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant demand element (AttributeCarrier)");
    } else {
        return nullptr;
    }
}


std::vector<GNEDemandElement*>
GNENetHelper::AttributeCarriers::getSelectedDemandElements() const {
    std::vector<GNEDemandElement*> result;
    // returns demand elements depending of selection
    for (const auto& demandElementTag : myDemandElements) {
        for (const auto& demandElement : demandElementTag.second) {
            if (demandElement->isAttributeCarrierSelected()) {
                result.push_back(demandElement);
            }
        }
    }
    return result;
}


const std::map<SumoXMLTag, std::set<GNEDemandElement*> >&
GNENetHelper::AttributeCarriers::getDemandElements() const {
    return myDemandElements;
}


int
GNENetHelper::AttributeCarriers::getNumberOfDemandElements() const {
    int counter = 0;
    for (const auto& demandElementTag : myDemandElements) {
        counter += (int)demandElementTag.second.size();
    }
    return counter;
}


std::string
GNENetHelper::AttributeCarriers::generateDemandElementID(SumoXMLTag tag) const {
    // declare flags
    const bool isVehicle = ((tag == SUMO_TAG_VEHICLE) || (tag == SUMO_TAG_TRIP) || (tag == GNE_TAG_VEHICLE_WITHROUTE));
    const bool isFlow = ((tag == GNE_TAG_FLOW_ROUTE) || (tag == SUMO_TAG_FLOW) || (tag == GNE_TAG_FLOW_WITHROUTE));
    const bool isPerson = ((tag == SUMO_TAG_PERSON) || (tag == SUMO_TAG_PERSONFLOW));
    // declare counter
    int counter = 0;
    if (isVehicle || isFlow) {
        // get vehicle tag in string format
        const std::string tagStr = isVehicle ? toString(SUMO_TAG_VEHICLE) : toString(SUMO_TAG_FLOW);
        // special case for vehicles (Vehicles, Flows, Trips and routeFlows share nameSpaces)
        while ((retrieveDemandElement(SUMO_TAG_VEHICLE, tagStr + "_" + toString(counter), false) != nullptr) ||
                (retrieveDemandElement(SUMO_TAG_TRIP, tagStr + "_" + toString(counter), false) != nullptr) ||
                (retrieveDemandElement(GNE_TAG_VEHICLE_WITHROUTE, tagStr + "_" + toString(counter), false) != nullptr) ||
                (retrieveDemandElement(GNE_TAG_FLOW_ROUTE, tagStr + "_" + toString(counter), false) != nullptr) ||
                (retrieveDemandElement(SUMO_TAG_FLOW, tagStr + "_" + toString(counter), false) != nullptr) ||
                (retrieveDemandElement(GNE_TAG_FLOW_WITHROUTE, tagStr + "_" + toString(counter), false) != nullptr)) {
            counter++;
        }
        // return new vehicle ID
        return (tagStr + "_" + toString(counter));
    } else if (isPerson) {
        // get person tag in string format
        const std::string tagStr = toString(tag);
        // special case for persons (person and personFlows share nameSpaces)
        while ((retrieveDemandElement(SUMO_TAG_PERSON, tagStr + "_" + toString(counter), false) != nullptr) ||
                (retrieveDemandElement(SUMO_TAG_PERSONFLOW, tagStr + "_" + toString(counter), false) != nullptr)) {
            counter++;
        }
        // return new person ID
        return (tagStr + "_" + toString(counter));
    } else {
        // get tag in string format
        const std::string tagStr = toString(tag);
        while (retrieveDemandElement(tag, tagStr + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
        // return new element ID
        return (tagStr + "_" + toString(counter));
    }
}


GNEDemandElement*
GNENetHelper::AttributeCarriers::getDefaultVType() const {
    for (const auto& vType : myDemandElements.at(SUMO_TAG_VTYPE)) {
        if (vType->getID() == DEFAULT_VTYPE_ID) {
            return vType;
        }
    }
    throw ProcessError("Default vType doesn't exist");
}


GNEDemandElement*
GNENetHelper::AttributeCarriers::getDefaultPType() const {
    for (const auto& pType : myDemandElements.at(SUMO_TAG_PTYPE)) {
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
    myDemandElements.at(defaultVehicleType->getTagProperty().getTag()).insert(defaultVehicleType);
    defaultVehicleType->incRef("GNENet::DEFAULT_VEHTYPE");

    // Create default Bike Type (it has to be created here due myViewNet was previously nullptr)
    GNEVehicleType* defaultBikeType = new GNEVehicleType(myNet, DEFAULT_BIKETYPE_ID, SVC_BICYCLE, SUMO_TAG_VTYPE);
    myDemandElements.at(defaultBikeType->getTagProperty().getTag()).insert(defaultBikeType);
    defaultBikeType->incRef("GNENet::DEFAULT_BIKETYPE_ID");

    // Create default person Type (it has to be created here due myViewNet was previously nullptr)
    GNEVehicleType* defaultPersonType = new GNEVehicleType(myNet, DEFAULT_PEDTYPE_ID, SVC_PEDESTRIAN, SUMO_TAG_PTYPE);
    myDemandElements.at(defaultPersonType->getTagProperty().getTag()).insert(defaultPersonType);
    defaultPersonType->incRef("GNENet::DEFAULT_PEDTYPE_ID");
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedDemandElements() const {
    int counter = 0;
    for (const auto& demandElementsTags : myDemandElements) {
        for (const auto& demandElement : demandElementsTags.second) {
            if (demandElement->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedRoutes() const {
    int counter = 0;
    // iterate over routes
    for (const auto& route : myDemandElements.at(SUMO_TAG_ROUTE)) {
        if (route->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    // iterate over vehicles with embedded routes
    for (const auto& vehicle : myDemandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
        if (vehicle->getChildDemandElements().front()->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& flow : myDemandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
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
    for (const auto& vehicle : myDemandElements.at(SUMO_TAG_VEHICLE)) {
        if (vehicle->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& trip : myDemandElements.at(SUMO_TAG_TRIP)) {
        if (trip->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& vehicle : myDemandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
        if (vehicle->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& flow : myDemandElements.at(SUMO_TAG_FLOW)) {
        if (flow->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& flow : myDemandElements.at(GNE_TAG_FLOW_ROUTE)) {
        if (flow->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& flow : myDemandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
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
    for (const auto& person : myDemandElements.at(SUMO_TAG_PERSON)) {
        if (person->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
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
    for (const auto& person : myDemandElements.at(SUMO_TAG_PERSON)) {
        for (const auto& personPlan : person->getChildDemandElements()) {
            if (personPlan->getTagProperty().isPersonTrip() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow->getChildDemandElements()) {
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
    for (const auto& person : myDemandElements.at(SUMO_TAG_PERSON)) {
        for (const auto& personPlan : person->getChildDemandElements()) {
            if (personPlan->getTagProperty().isWalk() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow->getChildDemandElements()) {
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
    for (const auto& person : myDemandElements.at(SUMO_TAG_PERSON)) {
        for (const auto& personPlan : person->getChildDemandElements()) {
            if (personPlan->getTagProperty().isRide() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow->getChildDemandElements()) {
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
    for (const auto& container : myDemandElements.at(SUMO_TAG_CONTAINER)) {
        if (container->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
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
    for (const auto& container : myDemandElements.at(SUMO_TAG_CONTAINER)) {
        for (const auto& containerPlan : container->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isTransportPlan() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto& containerPlan : containerFlow->getChildDemandElements()) {
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
    for (const auto& container : myDemandElements.at(SUMO_TAG_CONTAINER)) {
        for (const auto& containerPlan : container->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isTranshipPlan() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto& containerPlan : containerFlow->getChildDemandElements()) {
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
    for (const auto& route : myDemandElements.at(SUMO_TAG_ROUTE)) {
        if (route->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    // vehicles
    for (const auto& trip : myDemandElements.at(SUMO_TAG_TRIP)) {
        for (const auto& stop : trip->getChildDemandElements()) {
            if (stop->getTagProperty().isStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& vehicle : myDemandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
        for (const auto& stop : vehicle->getChildDemandElements().front()->getChildDemandElements()) {
            if (stop->getTagProperty().isStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& flow : myDemandElements.at(SUMO_TAG_FLOW)) {
        for (const auto& stop : flow->getChildDemandElements()) {
            if (stop->getTagProperty().isStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& flow : myDemandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
        for (const auto& stop : flow->getChildDemandElements().front()->getChildDemandElements()) {
            if (stop->getTagProperty().isStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    // persons
    for (const auto& person : myDemandElements.at(SUMO_TAG_PERSON)) {
        for (const auto& personPlan : person->getChildDemandElements()) {
            if (personPlan->getTagProperty().isStopPerson() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow->getChildDemandElements()) {
            if (personPlan->getTagProperty().isStopPerson() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    // containers
    for (const auto& container : myDemandElements.at(SUMO_TAG_CONTAINER)) {
        for (const auto& containerPlan : container->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isStopContainer() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto& containerPlan : containerFlow->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isStopContainer() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    return counter;
}


GNEDataSet*
GNENetHelper::AttributeCarriers::retrieveDataSet(const std::string& id, bool hardFail) const {
    for (const auto& dataSet : myDataSets) {
        if (dataSet->getID() == id) {
            return dataSet;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant data set");
    } else {
        return nullptr;
    }
}


GNEDataSet*
GNENetHelper::AttributeCarriers::retrieveDataSet(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast dataSet
    GNEDataSet* dataSet = dynamic_cast<GNEDataSet*>(AC);
    if (dataSet && (myDataSets.count(dataSet) > 0)) {
        return dataSet;
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant data set");
    } else {
        return nullptr;
    }
}


const std::set<GNEDataSet*>&
GNENetHelper::AttributeCarriers::getDataSets() const {
    return myDataSets;
}


std::string
GNENetHelper::AttributeCarriers::generateDataSetID(const std::string& prefix) const {
    const std::string dataSetTagStr = toString(SUMO_TAG_DATASET);
    int counter = 0;
    while (retrieveDataSet(prefix + dataSetTagStr + "_" + toString(counter), false) != nullptr) {
        counter++;
    }
    return (prefix + dataSetTagStr + "_" + toString(counter));
}


GNEDataInterval*
GNENetHelper::AttributeCarriers::retrieveDataInterval(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast dataInterval
    GNEDataInterval* dataInterval = dynamic_cast<GNEDataInterval*>(AC);
    if (dataInterval && (myDataIntervals.count(dataInterval) > 0)) {
        return dataInterval;
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant data interval");
    } else {
        return nullptr;
    }
}


const std::set<GNEDataInterval*>&
GNENetHelper::AttributeCarriers::getDataIntervals() const {
    return myDataIntervals;
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


GNEGenericData*
GNENetHelper::AttributeCarriers::retrieveGenericData(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast genericData
    GNEGenericData* genericData = dynamic_cast<GNEGenericData*>(AC);
    if (genericData && (myGenericDatas.at(AC->getTagProperty().getTag()).count(genericData) > 0)) {
        return genericData;
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant data set");
    } else {
        return nullptr;
    }
}


std::vector<GNEGenericData*>
GNENetHelper::AttributeCarriers::getSelectedGenericDatas() const {
    std::vector<GNEGenericData*> result;
    // returns generic datas depending of selection
    for (const auto& genericDataTag : myGenericDatas) {
        for (const auto& genericData : genericDataTag.second) {
            if (genericData->isAttributeCarrierSelected()) {
                result.push_back(genericData);
            }
        }
    }
    return result;
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
GNENetHelper::AttributeCarriers::insertGenericData(GNEGenericData* genericData) {
    if (myGenericDatas.at(genericData->getTagProperty().getTag()).insert(genericData).second == false) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' already exist");
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


std::set<std::string>
GNENetHelper::AttributeCarriers::retrieveGenericDataParameters(const std::string& genericDataTag, const double begin, const double end) const {
    // declare solution
    std::set<std::string> attributesSolution;
    // declare generic data vector
    std::vector<GNEGenericData*> genericDatas;
    // iterate over all data sets
    for (const auto& interval : myDataIntervals) {
        // check interval
        if ((interval->getAttributeDouble(SUMO_ATTR_BEGIN) >= begin) && (interval->getAttributeDouble(SUMO_ATTR_END) <= end)) {
            // iterate over generic datas
            for (const auto& genericData : interval->getGenericDataChildren()) {
                if (genericDataTag.empty() || (genericData->getTagProperty().getTagStr() == genericDataTag)) {
                    genericDatas.push_back(genericData);
                }
            }
        }
    }
    // iterate over generic datas
    for (const auto& genericData : genericDatas) {
        for (const auto& attribute : genericData->getParametersMap()) {
            attributesSolution.insert(attribute.first);
        }
    }
    return attributesSolution;
}


std::set<std::string>
GNENetHelper::AttributeCarriers::retrieveGenericDataParameters(const std::string& dataSetID, const std::string& genericDataTag,
        const std::string& beginStr, const std::string& endStr) const {
    // declare solution
    std::set<std::string> attributesSolution;
    // vector of data sets and intervals
    std::vector<GNEDataSet*> dataSets;
    std::vector<GNEDataInterval*> dataIntervals;
    // get dataSet
    GNEDataSet* retrievedDataSet = retrieveDataSet(dataSetID, false);
    // if dataSetID is empty, return all parameters
    if (dataSetID.empty()) {
        // add all data sets
        dataSets.reserve(myDataSets.size());
        for (const auto& dataSet : myDataSets) {
            dataSets.push_back(dataSet);
        }
    } else if (retrievedDataSet) {
        dataSets.push_back(retrievedDataSet);
    } else {
        return attributesSolution;
    }
    // now continue with data intervals
    int numberOfIntervals = 0;
    for (const auto& dataSet : dataSets) {
        numberOfIntervals += (int)dataSet->getDataIntervalChildren().size();
    }
    // resize dataIntervals
    dataIntervals.reserve(numberOfIntervals);
    // add intervals
    for (const auto& dataSet : dataSets) {
        for (const auto& dataInterval : dataSet->getDataIntervalChildren()) {
            // continue depending of begin and end
            if (beginStr.empty() && endStr.empty()) {
                dataIntervals.push_back(dataInterval.second);
            } else if (endStr.empty()) {
                // parse begin
                const double begin = GNEAttributeCarrier::parse<double>(beginStr);
                if (dataInterval.second->getAttributeDouble(SUMO_ATTR_BEGIN) >= begin) {
                    dataIntervals.push_back(dataInterval.second);
                }
            } else if (beginStr.empty()) {
                // parse end
                const double end = GNEAttributeCarrier::parse<double>(endStr);
                if (dataInterval.second->getAttributeDouble(SUMO_ATTR_END) <= end) {
                    dataIntervals.push_back(dataInterval.second);
                }
            } else {
                // parse both begin end
                const double begin = GNEAttributeCarrier::parse<double>(beginStr);
                const double end = GNEAttributeCarrier::parse<double>(endStr);
                if ((dataInterval.second->getAttributeDouble(SUMO_ATTR_BEGIN) >= begin) &&
                        (dataInterval.second->getAttributeDouble(SUMO_ATTR_END) <= end)) {
                    dataIntervals.push_back(dataInterval.second);
                }
            }
        }
    }
    // finally iterate over intervals and get attributes
    for (const auto& dataInterval : dataIntervals) {
        for (const auto& genericData : dataInterval->getGenericDataChildren()) {
            // check generic data tag
            if (genericDataTag.empty() || (genericData->getTagProperty().getTagStr() == genericDataTag)) {
                for (const auto& attribute : genericData->getParametersMap()) {
                    attributesSolution.insert(attribute.first);
                }
            }
        }
    }
    return attributesSolution;
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
    // get pointer to create edge frame
    const auto& createEdgeFrame = myNet->getViewNet()->getViewParent()->getCreateEdgeFrame();
    // insert in myEdgeTypes
    myEdgeTypes[edgeType->getMicrosimID()] = edgeType;
    // update edge selector
    if (myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->shown()) {
        myNet->getViewNet()->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->refreshEdgeTypeSelector();
    }
    // set current edge type inspected
    createEdgeFrame->getEdgeTypeSelector()->setCurrentEdgeType(edgeType);
}


void
GNENetHelper::AttributeCarriers::deleteEdgeType(GNEEdgeType* edgeType) {
    // get pointer to create edge frame
    const auto& createEdgeFrame = myNet->getViewNet()->getViewParent()->getCreateEdgeFrame();
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(edgeType);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(edgeType);
    // remove from edge types
    myEdgeTypes.erase(edgeType->getMicrosimID());
    // check if this is the selected edge type in edgeSelector
    if (createEdgeFrame->getEdgeTypeSelector()->getEdgeTypeSelected() == edgeType) {
        createEdgeFrame->getEdgeTypeSelector()->clearEdgeTypeSelected();
    }
    // update edge selector
    createEdgeFrame->getEdgeTypeSelector()->refreshEdgeTypeSelector();
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
    for (const auto& lane : edge->getLanes()) {
        deleteLane(lane);
    }
    // extract edge of district container
    myNet->getNetBuilder()->getEdgeCont().extract(myNet->getNetBuilder()->getDistrictCont(), edge->getNBEdge());
    edge->decRef("GNENet::deleteSingleEdge");
    edge->setResponsible(true);
    // Remove refrences from GNEJunctions
    edge->getFromJunction()->removeOutgoingGNEEdge(edge);
    edge->getToJunction()->removeIncomingGNEEdge(edge);
    // get template editor
    GNEInspectorFrame::TemplateEditor* templateEditor = myNet->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // check if we have to remove template
    if (templateEditor->getEdgeTemplate() && (templateEditor->getEdgeTemplate()->getID() == edge->getID())) {
        templateEditor->setEdgeTemplate(nullptr);
    }
}


bool
GNENetHelper::AttributeCarriers::additionalExist(const GNEAdditional* additional) const {
    // first check that additional pointer is valid
    if (additional) {
        // get vector with this additional element type
        const auto& additionalElementTag = myAdditionals.at(additional->getTagProperty().getTag());
        // find demanElement in additionalElementTag
        return std::find(additionalElementTag.begin(), additionalElementTag.end(), additional) != additionalElementTag.end();
    } else {
        throw ProcessError("Invalid additional pointer");
    }
}


void
GNENetHelper::AttributeCarriers::insertAdditional(GNEAdditional* additional) {
    // insert additional
    if (myAdditionals.at(additional->getTagProperty().getTag()).insert(additional).second == false) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' already exist");
    }
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
    // find demanElement in additionalTag
    auto itFind = myAdditionals.at(additional->getTagProperty().getTag()).find(additional);
    // check if additional was previously inserted
    if (itFind == myAdditionals.at(additional->getTagProperty().getTag()).end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(additional);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(additional);
    // remove from container
    myAdditionals.at(additional->getTagProperty().getTag()).erase(itFind);
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
        const auto& shapeElementTag = myShapes.at(shape->getTagProperty().getTag());
        // find demanElement in shapeElementTag
        return std::find(shapeElementTag.begin(), shapeElementTag.end(), shape) != shapeElementTag.end();
    } else {
        throw ProcessError("Invalid shape pointer");
    }
}


void
GNENetHelper::AttributeCarriers::insertShape(GNEShape* shape) {
    // insert shape
    if (myShapes.at(shape->getTagProperty().getTag()).insert(shape).second == false) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' already exist");
    }
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
    // find demanElement in shapeTag
    auto itFind = myShapes.at(shape->getTagProperty().getTag()).find(shape);
    // check if shape was previously inserted
    if (itFind == myShapes.at(shape->getTagProperty().getTag()).end()) {
        throw ProcessError(shape->getTagStr() + " with ID='" + shape->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(shape);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(shape);
    // remove it from container
    myShapes.at(shape->getTagProperty().getTag()).erase(itFind);
    // remove element from grid
    myNet->removeGLObjectFromGrid(shape);
    // shapes has to be saved
    myNet->requireSaveAdditionals(true);
}


void
GNENetHelper::AttributeCarriers::insertTAZElement(GNETAZElement* TAZElement) {
    // insert TAZElement
    if (myTAZElements.at(TAZElement->getTagProperty().getTag()).insert(TAZElement).second == false) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' already exist");
    }
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
    // find demanElement in TAZElementTag
    auto itFind = myTAZElements.at(TAZElement->getTagProperty().getTag()).find(TAZElement);
    // check if myTAZElements.at(TAZElement->getTagProperty().getTag()) was previously inserted
    if (itFind == myTAZElements.at(TAZElement->getTagProperty().getTag()).end()) {
        throw ProcessError(TAZElement->getTagStr() + " with ID='" + TAZElement->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(TAZElement);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(TAZElement);
    // remove it from container
    myTAZElements.at(TAZElement->getTagProperty().getTag()).erase(itFind);
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
        const auto& demandElementTag = myDemandElements.at(demandElement->getTagProperty().getTag());
        // find demanElement in demandElementTag
        return std::find(demandElementTag.begin(), demandElementTag.end(), demandElement) != demandElementTag.end();
    } else {
        throw ProcessError("Invalid demandElement pointer");
    }
}


void
GNENetHelper::AttributeCarriers::insertDemandElement(GNEDemandElement* demandElement) {
    // insert in demandElements container
    if (myDemandElements.at(demandElement->getTagProperty().getTag()).insert(demandElement).second == false) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' already exist");
    }
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
    // find demanElement in demandElementTag
    auto itFind = myDemandElements.at(demandElement->getTagProperty().getTag()).find(demandElement);
    // check if demandElement was previously inserted
    if (itFind == myDemandElements.at(demandElement->getTagProperty().getTag()).end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and HierarchicalElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(demandElement);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(demandElement);
    // erase it from container
    myDemandElements.at(demandElement->getTagProperty().getTag()).erase(itFind);
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
