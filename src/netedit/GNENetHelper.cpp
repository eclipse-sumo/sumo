/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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

#include <netbuild/NBNetBuilder.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/data/GNEMeanData.h>
#include <netedit/elements/demand/GNEVType.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/elements/network/GNEEdgeTemplate.h>
#include <netedit/elements/network/GNEEdgeType.h>
#include <netedit/elements/network/GNEWalkingArea.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/demand/GNEContainerFrame.h>
#include <netedit/frames/demand/GNEContainerPlanFrame.h>
#include <netedit/frames/demand/GNEPersonFrame.h>
#include <netedit/frames/demand/GNEPersonPlanFrame.h>
#include <netedit/frames/demand/GNERouteDistributionFrame.h>
#include <netedit/frames/demand/GNEStopFrame.h>
#include <netedit/frames/demand/GNETypeDistributionFrame.h>
#include <netedit/frames/demand/GNETypeFrame.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNENetHelper.h"

// ---------------------------------------------------------------------------
// GNENetHelper::AttributeCarriers - methods
// ---------------------------------------------------------------------------

GNENetHelper::AttributeCarriers::AttributeCarriers(GNENet* net) :
    myNet(net),
    myStopIndex(0) {
    // fill additionals with tags
    auto additionalTags = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::ADDITIONALELEMENT |
                          GNETagProperties::TagType::SHAPE | GNETagProperties::TagType::TAZELEMENT | GNETagProperties::TagType::WIRE);
    for (const auto& additionalTag : additionalTags) {
        myAdditionals.insert(std::make_pair(additionalTag.getTag(), std::map<const GUIGlObject*, GNEAdditional*>()));
    }
    // fill demand elements with tags
    auto demandElementTags = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::DEMANDELEMENT);
    for (const auto& demandElementTag : demandElementTags) {
        myDemandElements.insert(std::make_pair(demandElementTag.getTag(), std::map<const GUIGlObject*, GNEDemandElement*>()));
    }
    auto stopTags = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::VEHICLESTOP);
    for (const auto& stopTag : stopTags) {
        myDemandElements.insert(std::make_pair(stopTag.getTag(), std::map<const GUIGlObject*, GNEDemandElement*>()));
    }
    // fill data elements with tags
    auto genericDataElementTags = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::GENERICDATA);
    for (const auto& genericDataElementTag : genericDataElementTags) {
        myGenericDatas.insert(std::make_pair(genericDataElementTag.getTag(), std::set<GNEGenericData*>()));
    }
    // fill meanDatas with tags
    auto meanDataTags = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::MEANDATA);
    for (const auto& meanDataTag : meanDataTags) {
        myMeanDatas.insert(std::make_pair(meanDataTag.getTag(), std::set<GNEMeanData*>()));
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
            additional.second->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + additional.second->getTagStr() + " in AttributeCarriers destructor");
            delete additional.second;
        }
    }
    // Drop demand elements (Only used for demand elements that were inserted without using GNEChange_DemandElement, for example the default VType")
    for (const auto& demandElementTag : myDemandElements) {
        for (const auto& demandElement : demandElementTag.second) {
            // decrease reference manually (because it was increased manually in GNERouteHandler)
            demandElement.second->decRef();
            // show extra information for tests
            if (demandElement.second->getTagProperty().isType()) {
                // special case for default VTypes
                if (DEFAULT_VTYPES.count(demandElement.second->getID()) == 0) {
                    WRITE_DEBUG("Deleting unreferenced " + demandElement.second->getTagStr() + " in AttributeCarriers destructor");
                }
            } else {
                WRITE_DEBUG("Deleting unreferenced " + demandElement.second->getTagStr() + " in AttributeCarriers destructor");
            }
            delete demandElement.second;
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
    // Drop MeanDatas (Only used for meanDatas that were inserted without using GNEChange_MeanData)
    for (const auto& meanDataTag : myMeanDatas) {
        for (const auto& meanData : meanDataTag.second) {
            // decrease reference manually (because it was increased manually in GNEMeanDataHandler)
            meanData->decRef();
            // show extra information for tests
            WRITE_DEBUG("Deleting unreferenced " + meanData->getTagStr() + " in AttributeCarriers destructor");
            delete meanData;
        }
    }
}


void
GNENetHelper::AttributeCarriers::remapJunctionAndEdgeIds() {
    std::map<std::string, GNEEdge*> newEdgeMap;
    std::map<std::string, GNEJunction*> newJunctionMap;
    // fill newEdgeMap
    for (const auto& edge : myEdges) {
        edge.second->setEdgeID(edge.second->getNBEdge()->getID());
        newEdgeMap[edge.second->getNBEdge()->getID()] = edge.second;
    }
    for (const auto& junction : myJunctions) {
        newJunctionMap[junction.second->getNBNode()->getID()] = junction.second;
        junction.second->setNetworkElementID(junction.second->getNBNode()->getID());
    }
    myEdges = newEdgeMap;
    myJunctions = newJunctionMap;
}


bool
GNENetHelper::AttributeCarriers::isNetworkElementAroundShape(GNEAttributeCarrier* AC, const PositionVector& shape) const {
    // check what type of AC
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
        // Additional (including shapes and TAZs
        const GNEAdditional* additional = retrieveAdditional(AC);
        if (additional->getAdditionalGeometry().getShape().size() <= 1) {
            return shape.around(additional->getPositionInView());
        } else {
            return shape.overlapsWith(additional->getAdditionalGeometry().getShape());
        }
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
        // unblock and try to parse to AttributeCarrier
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        GNEAttributeCarrier* ac = dynamic_cast<GNEAttributeCarrier*>(object);
        // If was successfully parsed, return it
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
    } else if ((tag == SUMO_TAG_NOTHING) || (tag == SUMO_TAG_WALKINGAREA)) {
        for (const auto& walkingArea : myWalkingAreas) {
            result.push_back(walkingArea);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (GNEAttributeCarrier::getTagProperty(tag).isAdditionalElement())) {
        for (const auto& additional : myAdditionals.at(tag)) {
            result.push_back(additional.second);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (GNEAttributeCarrier::getTagProperty(tag).isDemandElement())) {
        for (const auto& demandElemet : myDemandElements.at(tag)) {
            result.push_back(demandElemet.second);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (tag == SUMO_TAG_DATASET)) {
        for (const auto& dataSet : myDataSets) {
            result.push_back(dataSet);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (tag == SUMO_TAG_DATAINTERVAL)) {
        for (const auto& dataInterval : myDataIntervals) {
            result.push_back(dataInterval);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (GNEAttributeCarrier::getTagProperty(tag).isGenericData())) {
        for (const auto& genericData : myGenericDatas.at(tag)) {
            result.push_back(genericData);
        }
    } else if ((tag == SUMO_TAG_NOTHING) || (GNEAttributeCarrier::getTagProperty(tag).isMeanData())) {
        for (const auto& meanData : myMeanDatas.at(tag)) {
            result.push_back(meanData);
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
                if (!onlySelected || additional.second->isAttributeCarrierSelected()) {
                    result.push_back(additional.second);
                }
            }
        }
    } else if (supermode == Supermode::DEMAND) {
        for (const auto& demandElementSet : myDemandElements) {
            for (const auto& demandElement : demandElementSet.second) {
                if (!onlySelected || demandElement.second->isAttributeCarrierSelected()) {
                    result.push_back(demandElement.second);
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
        for (const auto& meanDataSet : myMeanDatas) {
            for (const auto& meanData : meanDataSet.second) {
                if (!onlySelected || meanData->isAttributeCarrierSelected()) {
                    result.push_back(meanData);
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
            } else if (editModes.isCurrentSupermodeNetwork() && (AC->getTagProperty().isNetworkElement() || AC->getTagProperty().isAdditionalElement())) {
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
GNENetHelper::AttributeCarriers::addPrefixToJunctions(const std::string& prefix) {
    // make a copy of junctions
    std::map<std::string, GNEJunction*> junctionCopy = myJunctions;
    // clear junctions
    myJunctions.clear();
    // fill junctions again
    for (const auto& junction : junctionCopy) {
        // update microsim ID
        junction.second->setNetworkElementID(prefix + junction.first);
        // insert in myJunctions again
        myJunctions[prefix + junction.first] = junction.second;
    }
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
        junction->setNetworkElementID(newID);
        // add it into myJunctions again
        myJunctions[junction->getID()] = junction;
        // build crossings
        junction->getNBNode()->buildCrossings();
        // net has to be saved
        myNet->getSavingStatus()->requireSaveNetwork();
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
        if (AC) {
            throw UnknownElement("Crossing " + AC->getID());
        } else {
            throw UnknownElement("Crossing");
        }
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


GNEWalkingArea*
GNENetHelper::AttributeCarriers::retrieveWalkingArea(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast walkingArea
    GNEWalkingArea* walkingArea = dynamic_cast<GNEWalkingArea*>(AC);
    if (walkingArea && (myWalkingAreas.count(walkingArea) > 0)) {
        return walkingArea;
    } else if (hardFail) {
        if (AC) {
            throw UnknownElement("WalkingArea " + AC->getID());
        } else {
            throw UnknownElement("WalkingArea");
        }
    } else {
        return nullptr;
    }
}


const std::set<GNEWalkingArea*>&
GNENetHelper::AttributeCarriers::getWalkingAreas() const {
    return myWalkingAreas;
}


std::vector<GNEWalkingArea*>
GNENetHelper::AttributeCarriers::getSelectedWalkingAreas() const {
    std::vector<GNEWalkingArea*> result;
    // iterate over walkingAreas
    for (const auto& walkingArea : myWalkingAreas) {
        if (walkingArea->isAttributeCarrierSelected()) {
            result.push_back(walkingArea);
        }
    }
    return result;
}


void
GNENetHelper::AttributeCarriers::insertWalkingArea(GNEWalkingArea* walkingArea) {
    if (myWalkingAreas.insert(walkingArea).second == false) {
        throw ProcessError(walkingArea->getTagStr() + " with ID='" + walkingArea->getID() + "' already exist");
    }
}


void
GNENetHelper::AttributeCarriers::deleteWalkingArea(GNEWalkingArea* walkingArea) {
    const auto finder = myWalkingAreas.find(walkingArea);
    if (finder == myWalkingAreas.end()) {
        throw ProcessError(walkingArea->getTagStr() + " with ID='" + walkingArea->getID() + "' wasn't previously inserted");
    } else {
        myWalkingAreas.erase(finder);
    }
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedWalkingAreas() const {
    int counter = 0;
    for (const auto& walkingArea : myWalkingAreas) {
        if (walkingArea->isAttributeCarrierSelected()) {
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
        edgeType->setNetworkElementID(newID);
        // add it into myEdgeTypes again
        myEdgeTypes[edgeType->getID()] = edgeType;
        // net has to be saved
        myNet->getSavingStatus()->requireSaveNetwork();
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


std::vector<GNEEdge*>
GNENetHelper::AttributeCarriers::retrieveEdges(GNEJunction* from, GNEJunction* to) const {
    if ((from == nullptr) || (to == nullptr)) {
        throw UnknownElement("Junctions cannot be nullptr");
    }
    std::vector<GNEEdge*> edges;
    // iterate over Junctions
    for (const auto& edge : myEdges) {
        if ((edge.second->getFromJunction() == from) && (edge.second->getToJunction() == to)) {
            edges.push_back(edge.second);
        }
    }
    return edges;
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


void
GNENetHelper::AttributeCarriers::clearEdges() {
    myEdges.clear();
}


void
GNENetHelper::AttributeCarriers::addPrefixToEdges(const std::string& prefix) {
    // make a copy of edges
    std::map<std::string, GNEEdge*> edgeCopy = myEdges;
    // clear edges
    myEdges.clear();
    // fill edges again
    for (const auto& edge : edgeCopy) {
        // update microsim ID
        edge.second->setNetworkElementID(prefix + edge.first);
        // insert in myEdges again
        myEdges[prefix + edge.first] = edge.second;
    }
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
        edge->setEdgeID(newID);
        // add it into myEdges again
        myEdges[edge->getID()] = edge;
        // rename all connections related to this edge
        for (const auto& lane : edge->getLanes()) {
            lane->updateConnectionIDs();
        }
        // net has to be saved
        myNet->getSavingStatus()->requireSaveNetwork();
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
        if (AC) {
            throw UnknownElement("Lane " + AC->getID());
        } else {
            throw UnknownElement("Lane");
        }
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
        if (AC) {
            throw UnknownElement("Connection " + AC->getID());
        } else {
            throw UnknownElement("Connection");
        }
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
        if (additional.second->getID() == id) {
            return additional.second;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant additional (string)");
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNENetHelper::AttributeCarriers::retrieveAdditionals(const std::vector<SumoXMLTag> types, const std::string& id, bool hardFail) const {
    for (const auto& type : types) {
        for (const auto& additional : myAdditionals.at(type)) {
            if (additional.second->getID() == id) {
                return additional.second;
            }
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
GNENetHelper::AttributeCarriers::retrieveAdditionalGL(const GUIGlObject* glObject, bool hardFail) const {
    // iterate over all additionals
    for (const auto &additionalTag : myAdditionals){
        for (const auto &additionalPair : additionalTag.second) {
            if (additionalPair.first == glObject) {
                return additionalPair.second;
            }
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant additional (glObject)");
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


const std::map<SumoXMLTag, std::map<const GUIGlObject*, GNEAdditional*> >&
GNENetHelper::AttributeCarriers::getAdditionals() const {
    return myAdditionals;
}


std::vector<GNEAdditional*>
GNENetHelper::AttributeCarriers::getSelectedAdditionals() const {
    std::vector<GNEAdditional*> result;
    // returns additionals depending of selection
    for (const auto& additionalsTags : myAdditionals) {
        for (const auto& additional : additionalsTags.second) {
            if (additional.second->isAttributeCarrierSelected()) {
                result.push_back(additional.second);
            }
        }
    }
    return result;
}


std::vector<GNEAdditional*>
GNENetHelper::AttributeCarriers::getSelectedShapes() const {
    std::vector<GNEAdditional*> result;
    // returns additionals depending of selection
    for (const auto& additionalsTags : myAdditionals) {
        for (const auto& additional : additionalsTags.second) {
            if (additional.second->getTagProperty().isShapeElement() && additional.second->isAttributeCarrierSelected()) {
                result.push_back(additional.second);
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
            myNet->removeGLObjectFromGrid(additional.second);
        }
    }
    // iterate over myAdditionals and clear all additionals
    for (auto& additionals : myAdditionals) {
        additionals.second.clear();
    }
}


std::string
GNENetHelper::AttributeCarriers::generateAdditionalID(SumoXMLTag tag) const {
    // obtain option container
    const auto& neteditOptions = OptionsCont::getOptions();
    // get prefix
    std::string prefix;
    if (tag == SUMO_TAG_BUS_STOP) {
        prefix = neteditOptions.getString("busStop-prefix");
    } else if (tag == SUMO_TAG_TRAIN_STOP) {
        prefix = neteditOptions.getString("trainStop-prefix");
    } else if (tag == SUMO_TAG_CONTAINER_STOP) {
        prefix = neteditOptions.getString("containerStop-prefix");
    } else if (tag == SUMO_TAG_CHARGING_STATION) {
        prefix = neteditOptions.getString("chargingStation-prefix");
    } else if (tag == SUMO_TAG_PARKING_AREA) {
        prefix = neteditOptions.getString("parkingArea-prefix");
    } else if (tag == SUMO_TAG_INDUCTION_LOOP) {
        prefix = neteditOptions.getString("e1Detector-prefix");
    } else if ((tag == SUMO_TAG_LANE_AREA_DETECTOR) || (tag == GNE_TAG_MULTI_LANE_AREA_DETECTOR)) {
        prefix = neteditOptions.getString("e2Detector-prefix");
    } else if (tag == SUMO_TAG_ENTRY_EXIT_DETECTOR) {
        prefix = neteditOptions.getString("e3Detector-prefix");
    } else if (tag == SUMO_TAG_INSTANT_INDUCTION_LOOP) {
        prefix = neteditOptions.getString("e1InstantDetector-prefix");
    } else if (tag == SUMO_TAG_REROUTER) {
        prefix = neteditOptions.getString("rerouter-prefix");
    } else if ((tag == SUMO_TAG_CALIBRATOR) || (tag == GNE_TAG_CALIBRATOR_LANE)) {
        prefix = neteditOptions.getString("calibrator-prefix");
    } else if (tag == SUMO_TAG_ROUTEPROBE) {
        prefix = neteditOptions.getString("routeProbe-prefix");
    } else if (tag == SUMO_TAG_VSS) {
        prefix = neteditOptions.getString("vss-prefix");
    } else if (tag == SUMO_TAG_TRACTION_SUBSTATION) {
        prefix = neteditOptions.getString("tractionSubstation-prefix");
    } else if (tag == SUMO_TAG_OVERHEAD_WIRE_SECTION) {
        prefix = neteditOptions.getString("overheadWire-prefix");
    } else if (tag == SUMO_TAG_POLY) {
        prefix = neteditOptions.getString("polygon-prefix");
    } else if ((tag == SUMO_TAG_POI) || (tag == GNE_TAG_POILANE) || (tag == GNE_TAG_POIGEO)) {
        prefix = neteditOptions.getString("poi-prefix");
    } else if (tag == SUMO_TAG_TAZ) {
        prefix = toString(SUMO_TAG_TAZ);
    } else if (tag == GNE_TAG_JPS_WALKABLEAREA) {
        prefix = neteditOptions.getString("jps.walkableArea-prefix");
    } else if (tag == GNE_TAG_JPS_OBSTACLE) {
        prefix = neteditOptions.getString("jps.obstacle-prefix");
    }
    int counter = 0;
    // check namespaces
    if (std::find(NamespaceIDs::busStops.begin(), NamespaceIDs::busStops.end(), tag) != NamespaceIDs::busStops.end()) {
        while (retrieveAdditionals(NamespaceIDs::busStops, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    } else if (std::find(NamespaceIDs::calibrators.begin(), NamespaceIDs::calibrators.end(), tag) != NamespaceIDs::calibrators.end()) {
        while (retrieveAdditionals(NamespaceIDs::calibrators, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    } else if (std::find(NamespaceIDs::polygons.begin(), NamespaceIDs::polygons.end(), tag) != NamespaceIDs::polygons.end()) {
        while (retrieveAdditionals(NamespaceIDs::polygons, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    } else if (std::find(NamespaceIDs::POIs.begin(), NamespaceIDs::POIs.end(), tag) != NamespaceIDs::POIs.end()) {
        while (retrieveAdditionals(NamespaceIDs::POIs, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    } else if (std::find(NamespaceIDs::laneAreaDetectors.begin(), NamespaceIDs::laneAreaDetectors.end(), tag) != NamespaceIDs::laneAreaDetectors.end()) {
        while (retrieveAdditionals(NamespaceIDs::laneAreaDetectors, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    } else {
        while (retrieveAdditional(tag, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    }
    // return new element ID
    return (prefix + "_" + toString(counter));
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedAdditionals() const {
    int counter = 0;
    for (const auto& additionalsTags : myAdditionals) {
        for (const auto& additional : additionalsTags.second) {
            if (additional.second->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedPureAdditionals() const {
    return getNumberOfSelectedAdditionals() -
           // shapes
           getNumberOfSelectedPolygons() - getNumberOfSelectedPOIs() -
           // JuPedSims
           getNumberOfSelectedJpsWalkableAreas() - getNumberOfSelectedJpsObstacles() -
           // TAZ
           getNumberOfSelectedTAZs() - getNumberOfSelectedTAZSources() - getNumberOfSelectedTAZSinks() -
           // wires
           getNumberOfSelectedWires();
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedPolygons() const {
    int counter = 0;
    for (const auto& poly : myAdditionals.at(SUMO_TAG_POLY)) {
        if (poly.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedJpsWalkableAreas() const {
    int counter = 0;
    for (const auto& walkableArea : myAdditionals.at(GNE_TAG_JPS_WALKABLEAREA)) {
        if (walkableArea.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedJpsObstacles() const {
    int counter = 0;
    for (const auto& obstacle : myAdditionals.at(GNE_TAG_JPS_OBSTACLE)) {
        if (obstacle.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedPOIs() const {
    int counter = 0;
    for (const auto& POI : myAdditionals.at(SUMO_TAG_POI)) {
        if (POI.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& POILane : myAdditionals.at(GNE_TAG_POILANE)) {
        if (POILane.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& POIGEO : myAdditionals.at(GNE_TAG_POIGEO)) {
        if (POIGEO.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedTAZs() const {
    int counter = 0;
    for (const auto& TAZ : myAdditionals.at(SUMO_TAG_TAZ)) {
        if (TAZ.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedTAZSources() const {
    int counter = 0;
    for (const auto& TAZSource : myAdditionals.at(SUMO_TAG_TAZSOURCE)) {
        if (TAZSource.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedTAZSinks() const {
    int counter = 0;
    for (const auto& TAZSink : myAdditionals.at(SUMO_TAG_TAZSINK)) {
        if (TAZSink.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedWires() const {
    int counter = 0;
    for (const auto& additionalsTags : myAdditionals) {
        for (const auto& additional : additionalsTags.second) {
            if (additional.second->isAttributeCarrierSelected() && additional.second->getTagProperty().isWireElement()) {
                counter++;
            }
        }
    }
    return counter;
}


GNEDemandElement*
GNENetHelper::AttributeCarriers::retrieveDemandElement(SumoXMLTag type, const std::string& id, bool hardFail) const {
    for (const auto& demandElement : myDemandElements.at(type)) {
        if (demandElement.second->getID() == id) {
            return demandElement.second;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant demand element (string)");
    } else {
        return nullptr;
    }
}


GNEDemandElement*
GNENetHelper::AttributeCarriers::retrieveDemandElements(std::vector<SumoXMLTag> types, const std::string& id, bool hardFail) const {
    for (const auto& type : types) {
        for (const auto& demandElement : myDemandElements.at(type)) {
            if (demandElement.second->getID() == id) {
                return demandElement.second;
            }
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


GNEDemandElement*
GNENetHelper::AttributeCarriers::retrieveDemandElementGL(const GUIGlObject* glObject, bool hardFail) const {
    // iterate over all demand elements
    for (const auto &demandElementTag : myDemandElements){
        for (const auto &demandElementPair : demandElementTag.second) {
            if (demandElementPair.first == glObject) {
                return demandElementPair.second;
            }
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant demandElement (glObject)");
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
            if (demandElement.second->isAttributeCarrierSelected()) {
                result.push_back(demandElement.second);
            }
        }
    }
    return result;
}


const std::map<SumoXMLTag, std::map<const GUIGlObject*, GNEDemandElement*> >&
GNENetHelper::AttributeCarriers::getDemandElements() const {
    return myDemandElements;
}


int
GNENetHelper::AttributeCarriers::getNumberOfDemandElements() const {
    int counter = 0;
    for (const auto& demandElementTag : myDemandElements) {
        if (demandElementTag.first == SUMO_TAG_VTYPE) {
            // iterate over vehicle types to avoid default vTypes
            for (const auto& vType : demandElementTag.second) {
                if (vType.second->getAttribute(GNE_ATTR_DEFAULT_VTYPE) != GNEAttributeCarrier::True) {
                    counter++;
                }
            }
        } else {
            counter += (int)demandElementTag.second.size();
        }
    }
    return counter;
}


std::string
GNENetHelper::AttributeCarriers::generateDemandElementID(SumoXMLTag tag) const {
    // obtain option container
    const auto& neteditOptions = OptionsCont::getOptions();
    // get tag property
    const auto tagProperty = GNEAttributeCarrier::getTagProperty(tag);
    // get prefix
    std::string prefix;
    if (tag == SUMO_TAG_ROUTE) {
        prefix = neteditOptions.getString("route-prefix");
    } else if (tag == SUMO_TAG_ROUTE_DISTRIBUTION) {
        prefix = neteditOptions.getString("routeDistribution-prefix");
    } else if (tag == SUMO_TAG_VTYPE) {
        prefix = neteditOptions.getString("vType-prefix");
    } else if (tag == SUMO_TAG_VTYPE_DISTRIBUTION) {
        prefix = neteditOptions.getString("vTypeDistribution-prefix");
    } else if ((tag == SUMO_TAG_TRIP) || (tag == GNE_TAG_TRIP_JUNCTIONS) || (tag == GNE_TAG_TRIP_TAZS)) {
        prefix = neteditOptions.getString("trip-prefix");
    } else if (tagProperty.isVehicle() && !tagProperty.isFlow()) {
        prefix = neteditOptions.getString("vehicle-prefix");
    } else if (tagProperty.isPerson()) {
        if (tagProperty.isFlow()) {
            prefix = neteditOptions.getString("personflow-prefix");
        } else {
            prefix = neteditOptions.getString("person-prefix");
        }
    } else if (tagProperty.isContainer()) {
        if (tagProperty.isFlow()) {
            prefix = neteditOptions.getString("containerflow-prefix");
        } else {
            prefix = neteditOptions.getString("container-prefix");
        }
    } else if (tagProperty.isFlow()) {
        prefix = neteditOptions.getString("flow-prefix");
    }
    // declare counter
    int counter = 0;
    if (std::find(NamespaceIDs::types.begin(), NamespaceIDs::types.end(), tag) != NamespaceIDs::types.end()) {
        while (retrieveDemandElements(NamespaceIDs::types, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    } else if (std::find(NamespaceIDs::routes.begin(), NamespaceIDs::routes.end(), tag) != NamespaceIDs::routes.end()) {
        while (retrieveDemandElements(NamespaceIDs::routes, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    } else if (std::find(NamespaceIDs::persons.begin(), NamespaceIDs::persons.end(), tag) != NamespaceIDs::persons.end()) {
        while (retrieveDemandElements(NamespaceIDs::persons, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    } else if (std::find(NamespaceIDs::containers.begin(), NamespaceIDs::containers.end(), tag) != NamespaceIDs::containers.end()) {
        while (retrieveDemandElements(NamespaceIDs::containers, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    } else if (std::find(NamespaceIDs::vehicles.begin(), NamespaceIDs::vehicles.end(), tag) != NamespaceIDs::vehicles.end()) {
        while (retrieveDemandElements(NamespaceIDs::vehicles, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    } else {
        while (retrieveDemandElement(tag, prefix + "_" + toString(counter), false) != nullptr) {
            counter++;
        }
    }
    // return new element ID
    return (prefix + "_" + toString(counter));

}


GNEDemandElement*
GNENetHelper::AttributeCarriers::getDefaultType() const {
    for (const auto& vType : myDemandElements.at(SUMO_TAG_VTYPE)) {
        if (vType.second->getID() == DEFAULT_VTYPE_ID) {
            return vType.second;
        }
    }
    throw ProcessError(TL("Default vType doesn't exist"));
}


void
GNENetHelper::AttributeCarriers::clearDemandElements() {
    // clear elements in grid
    for (const auto& demandElementsTags : myDemandElements) {
        for (const auto& demandElement : demandElementsTags.second) {
            myNet->removeGLObjectFromGrid(demandElement.second);
        }
    }
    // iterate over myDemandElements and clear all demand elements
    for (auto& demandElements : myDemandElements) {
        demandElements.second.clear();
    }
}


void
GNENetHelper::AttributeCarriers::addDefaultVTypes() {
    // Create default vehicle Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultVehicleType = new GNEVType(myNet, DEFAULT_VTYPE_ID, SVC_PASSENGER);
    myDemandElements.at(defaultVehicleType->getTagProperty().getTag()).insert(std::make_pair(defaultVehicleType->getGUIGlObject(), defaultVehicleType));
    defaultVehicleType->incRef("GNENet::DEFAULT_VEHTYPE");

    // Create default Bike Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultBikeType = new GNEVType(myNet, DEFAULT_BIKETYPE_ID, SVC_BICYCLE);
    myDemandElements.at(defaultBikeType->getTagProperty().getTag()).insert(std::make_pair(defaultBikeType->getGUIGlObject(), defaultBikeType));
    defaultBikeType->incRef("GNENet::DEFAULT_BIKETYPE_ID");

    // Create default taxi Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultTaxiType = new GNEVType(myNet, DEFAULT_TAXITYPE_ID, SVC_TAXI);
    myDemandElements.at(defaultTaxiType->getTagProperty().getTag()).insert(std::make_pair(defaultTaxiType->getGUIGlObject(), defaultTaxiType));
    defaultTaxiType->incRef("GNENet::DEFAULT_TAXITYPE_ID");

    // Create default rail Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultRailType = new GNEVType(myNet, DEFAULT_RAILTYPE_ID, SVC_RAIL);
    myDemandElements.at(defaultRailType->getTagProperty().getTag()).insert(std::make_pair(defaultRailType->getGUIGlObject(), defaultRailType));
    defaultRailType->incRef("GNENet::DEFAULT_RAILTYPE_ID");

    // Create default person Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultPersonType = new GNEVType(myNet, DEFAULT_PEDTYPE_ID, SVC_PEDESTRIAN);
    myDemandElements.at(defaultPersonType->getTagProperty().getTag()).insert(std::make_pair(defaultPersonType->getGUIGlObject(), defaultPersonType));
    defaultPersonType->incRef("GNENet::DEFAULT_PEDTYPE_ID");

    // Create default container Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultContainerType = new GNEVType(myNet, DEFAULT_CONTAINERTYPE_ID, SVC_IGNORING);
    myDemandElements.at(defaultContainerType->getTagProperty().getTag()).insert(std::make_pair(defaultContainerType->getGUIGlObject(), defaultContainerType));
    defaultContainerType->incRef("GNENet::DEFAULT_CONTAINERTYPE_ID");
}


int
GNENetHelper::AttributeCarriers::getStopIndex() {
    return myStopIndex++;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedDemandElements() const {
    int counter = 0;
    for (const auto& demandElementsTags : myDemandElements) {
        for (const auto& demandElement : demandElementsTags.second) {
            if (demandElement.second->isAttributeCarrierSelected()) {
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
        if (route.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    // iterate over vehicles with embedded routes
    for (const auto& vehicle : myDemandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
        if (vehicle.second->getChildDemandElements().front()->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& flow : myDemandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
        if (flow.second->getChildDemandElements().front()->isAttributeCarrierSelected()) {
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
        if (vehicle.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& trip : myDemandElements.at(SUMO_TAG_TRIP)) {
        if (trip.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& vehicle : myDemandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
        if (vehicle.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& flow : myDemandElements.at(SUMO_TAG_FLOW)) {
        if (flow.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& flow : myDemandElements.at(GNE_TAG_FLOW_ROUTE)) {
        if (flow.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& flow : myDemandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
        if (flow.second->isAttributeCarrierSelected()) {
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
        if (person.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        if (personFlow.second->isAttributeCarrierSelected()) {
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
        for (const auto& personPlan : person.second->getChildDemandElements()) {
            if (personPlan->getTagProperty().isPersonTrip() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow.second->getChildDemandElements()) {
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
        for (const auto& personPlan : person.second->getChildDemandElements()) {
            if (personPlan->getTagProperty().isPlanWalk() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow.second->getChildDemandElements()) {
            if (personPlan->getTagProperty().isPlanWalk() && personPlan->isAttributeCarrierSelected()) {
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
        for (const auto& personPlan : person.second->getChildDemandElements()) {
            if (personPlan->getTagProperty().isPlanRide() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow.second->getChildDemandElements()) {
            if (personPlan->getTagProperty().isPlanRide() && personPlan->isAttributeCarrierSelected()) {
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
        if (container.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        if (containerFlow.second->isAttributeCarrierSelected()) {
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
        for (const auto& containerPlan : container.second->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isPlanTransport() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto& containerPlan : containerFlow.second->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isPlanTransport() && containerPlan->isAttributeCarrierSelected()) {
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
        for (const auto& containerPlan : container.second->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isPlanTranship() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto& containerPlan : containerFlow.second->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isPlanTranship() && containerPlan->isAttributeCarrierSelected()) {
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
        if (route.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    // vehicles
    for (const auto& trip : myDemandElements.at(SUMO_TAG_TRIP)) {
        for (const auto& stop : trip.second->getChildDemandElements()) {
            if (stop->getTagProperty().isVehicleStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& vehicle : myDemandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
        for (const auto& stop : vehicle.second->getChildDemandElements().front()->getChildDemandElements()) {
            if (stop->getTagProperty().isVehicleStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& flow : myDemandElements.at(SUMO_TAG_FLOW)) {
        for (const auto& stop : flow.second->getChildDemandElements()) {
            if (stop->getTagProperty().isVehicleStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& flow : myDemandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
        for (const auto& stop : flow.second->getChildDemandElements().front()->getChildDemandElements()) {
            if (stop->getTagProperty().isVehicleStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    // persons
    for (const auto& person : myDemandElements.at(SUMO_TAG_PERSON)) {
        for (const auto& personPlan : person.second->getChildDemandElements()) {
            if (personPlan->getTagProperty().isPlanStopPerson() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow.second->getChildDemandElements()) {
            if (personPlan->getTagProperty().isPlanStopPerson() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    // containers
    for (const auto& container : myDemandElements.at(SUMO_TAG_CONTAINER)) {
        for (const auto& containerPlan : container.second->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isPlanStopContainer() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto& containerPlan : containerFlow.second->getChildDemandElements()) {
            if (containerPlan->getTagProperty().isPlanStopContainer() && containerPlan->isAttributeCarrierSelected()) {
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
    // mark interval toolbar for update
    myNet->getViewNet()->getIntervalBar().markForUpdate();
}


void
GNENetHelper::AttributeCarriers::deleteDataInterval(GNEDataInterval* dataInterval) {
    const auto finder = myDataIntervals.find(dataInterval);
    if (finder == myDataIntervals.end()) {
        throw ProcessError(dataInterval->getTagStr() + " with ID='" + dataInterval->getID() + "' wasn't previously inserted");
    } else {
        myDataIntervals.erase(finder);
    }
    // mark interval toolbar for update
    myNet->getViewNet()->getIntervalBar().markForUpdate();
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
GNENetHelper::AttributeCarriers::getNumberOfGenericDatas() const {
    int counter = 0;
    // iterate over all generic datas
    for (const auto& genericDataTag : myGenericDatas) {
        counter += (int)genericDataTag.second.size();
    }
    return counter;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedEdgeDatas() const {
    int counter = 0;
    // iterate over all edgeDatas
    for (const auto& genericData : myGenericDatas.at(GNE_TAG_EDGEREL_SINGLE)) {
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
    // mark interval toolbar for update
    myNet->getViewNet()->getIntervalBar().markForUpdate();
}


void
GNENetHelper::AttributeCarriers::deleteGenericData(GNEGenericData* genericData) {
    const auto finder = myGenericDatas.at(genericData->getTagProperty().getTag()).find(genericData);
    if (finder == myGenericDatas.at(genericData->getTagProperty().getTag()).end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' wasn't previously inserted");
    } else {
        myGenericDatas.at(genericData->getTagProperty().getTag()).erase(finder);
    }
    // mark interval toolbar for update
    myNet->getViewNet()->getIntervalBar().markForUpdate();
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


GNEMeanData*
GNENetHelper::AttributeCarriers::retrieveMeanData(SumoXMLTag type, const std::string& id, bool hardFail) const {
    for (const auto& meanData : myMeanDatas.at(type)) {
        if (meanData->getID() == id) {
            return meanData;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant meanData (string)");
    } else {
        return nullptr;
    }
}


GNEMeanData*
GNENetHelper::AttributeCarriers::retrieveMeanData(GNEAttributeCarrier* AC, bool hardFail) const {
    // cast meanData
    GNEMeanData* meanData = dynamic_cast<GNEMeanData*>(AC);
    if (meanData && (myMeanDatas.at(AC->getTagProperty().getTag()).count(meanData) > 0)) {
        return meanData;
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant meanData (AttributeCarrier)");
    } else {
        return nullptr;
    }
}


const std::map<SumoXMLTag, std::set<GNEMeanData*> >&
GNENetHelper::AttributeCarriers::getMeanDatas() const {
    return myMeanDatas;
}


int
GNENetHelper::AttributeCarriers::getNumberOfMeanDatas() const {
    int counter = 0;
    for (const auto& meanDatasTag : myMeanDatas) {
        counter += (int)meanDatasTag.second.size();
    }
    return counter;
}


void
GNENetHelper::AttributeCarriers::clearMeanDatas() {
    // clear elements in grid
    for (const auto& meanDatasTags : myMeanDatas) {
        for (const auto& meanData : meanDatasTags.second) {
            myNet->removeGLObjectFromGrid(meanData);
        }
    }
    // iterate over myMeanDatas and clear all meanDatas
    for (auto& meanDatas : myMeanDatas) {
        meanDatas.second.clear();
    }
}


std::string
GNENetHelper::AttributeCarriers::generateMeanDataID(SumoXMLTag tag) const {
    // obtain option container
    const auto& neteditOptions = OptionsCont::getOptions();
    // get prefix
    std::string prefix;
    if (tag == SUMO_TAG_MEANDATA_EDGE) {
        prefix = neteditOptions.getString("meanDataEdge-prefix");
    } else if (tag == SUMO_TAG_MEANDATA_LANE) {
        prefix = neteditOptions.getString("meanDataLane-prefix");
    }
    int counter = 0;
    while (retrieveMeanData(tag, prefix + "_" + toString(counter), false) != nullptr) {
        counter++;
    }
    return (prefix + "_" + toString(counter));
}


void
GNENetHelper::AttributeCarriers::insertJunction(GNEJunction* junction) {
    myNet->getNetBuilder()->getNodeCont().insert(junction->getNBNode());
    registerJunction(junction);
}


void
GNENetHelper::AttributeCarriers::deleteSingleJunction(GNEJunction* junction) {
    // remove it from inspected elements and GNEElementTree
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
    // remove it from inspected elements and GNEElementTree
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
    // remove it from inspected elements and GNEElementTree
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
        // search in additional elements
        for (auto additionalPair : myAdditionals.at(additional->getTagProperty().getTag())) {
            if (additionalPair.second == additional) {
                return true;
            }
        }
        return false;
    } else {
        throw ProcessError("Invalid additional pointer");
    }
}


void
GNENetHelper::AttributeCarriers::insertAdditional(GNEAdditional* additional) {
    // insert additional
    auto pair = std::make_pair(additional->getGUIGlObject(), additional);
    if (myAdditionals.at(additional->getTagProperty().getTag()).insert(pair).second == false) {
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
    myNet->getSavingStatus()->requireSaveAdditionals();
}


void
GNENetHelper::AttributeCarriers::deleteAdditional(GNEAdditional* additional) {
    // find demanElement in additionalTag
    auto itFind = myAdditionals.at(additional->getTagProperty().getTag()).find(additional);
    // check if additional was previously inserted
    if (itFind == myAdditionals.at(additional->getTagProperty().getTag()).end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and GNEElementTree
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
    myNet->getSavingStatus()->requireSaveAdditionals();
}


bool
GNENetHelper::AttributeCarriers::demandElementExist(GNEDemandElement* demandElement) const {
    // first check that demandElement pointer is valid
    if (demandElement) {
        // search in demand elements
        for (auto demandElementPair : myDemandElements.at(demandElement->getTagProperty().getTag())) {
            if (demandElementPair.second == demandElement) {
                return true;
            }
        }
        return false;
    } else {
        throw ProcessError("Invalid demandElement pointer");
    }
}


void
GNENetHelper::AttributeCarriers::insertDemandElement(GNEDemandElement* demandElement) {
    // insert in demandElements container
    auto pair = std::make_pair(demandElement->getGUIGlObject(), demandElement);
    if (myDemandElements.at(demandElement->getTagProperty().getTag()).insert(pair).second == false) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' already exist");
    }
    // add element in grid
    myNet->addGLObjectIntoGrid(demandElement);
    // update geometry after insertion of demandElements if myUpdateGeometryEnabled is enabled
    if (myNet->isUpdateGeometryEnabled()) {
        demandElement->updateGeometry();
    }
    // compute path element
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        demandElement->computePathElement();
    }
    // update demand elements frames
    updateDemandElementFrames(demandElement->getTagProperty());
    // demandElements has to be saved
    myNet->getSavingStatus()->requireSaveDemandElements();
}


void
GNENetHelper::AttributeCarriers::deleteDemandElement(GNEDemandElement* demandElement, const bool updateFrames) {
    // find demanElement in demandElementTag
    auto itFind = myDemandElements.at(demandElement->getTagProperty().getTag()).find(demandElement);
    // check if demandElement was previously inserted
    if (itFind == myDemandElements.at(demandElement->getTagProperty().getTag()).end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and GNEElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(demandElement);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(demandElement);
    myNet->getViewNet()->getViewParent()->getPersonPlanFrame()->getPersonHierarchy()->removeCurrentEditedAttributeCarrier(demandElement);
    myNet->getViewNet()->getViewParent()->getContainerPlanFrame()->getContainerHierarchy()->removeCurrentEditedAttributeCarrier(demandElement);
    // if is the last inserted route, remove it from GNEViewNet
    if (myNet->getViewNet()->getLastCreatedRoute() == demandElement) {
        myNet->getViewNet()->setLastCreatedRoute(nullptr);
    }
    // erase it from container
    myDemandElements.at(demandElement->getTagProperty().getTag()).erase(itFind);
    // remove element from grid
    myNet->removeGLObjectFromGrid(demandElement);
    // delete path element
    myNet->getPathManager()->removePath(demandElement);
    // check if update demand elements frames
    if (updateFrames) {
        updateDemandElementFrames(demandElement->getTagProperty());
    }
    // demandElements has to be saved
    myNet->getSavingStatus()->requireSaveDemandElements();
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
    myNet->getSavingStatus()->requireSaveDataElements();
    // mark interval toolbar for update
    myNet->getViewNet()->getIntervalBar().markForUpdate();
}


void
GNENetHelper::AttributeCarriers::deleteDataSet(GNEDataSet* dataSet) {
    // find dataSet
    const auto itFind = myDataSets.find(dataSet);
    // first check that dataSet pointer is valid
    if (itFind == myDataSets.end()) {
        throw ProcessError(dataSet->getTagStr() + " with ID='" + dataSet->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and GNEElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(dataSet);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(dataSet);
    // obtain demand element and erase it from container
    myDataSets.erase(itFind);
    // dataSets has to be saved
    myNet->getSavingStatus()->requireSaveDataElements();
    // mark interval toolbar for update
    myNet->getViewNet()->getIntervalBar().markForUpdate();
}


bool
GNENetHelper::AttributeCarriers::meanDataExist(const GNEMeanData* meanData) const {
    // first check that meanData pointer is valid
    if (meanData) {
        // get vector with this meanData element type
        const auto& meanDataElementTag = myMeanDatas.at(meanData->getTagProperty().getTag());
        // find demanElement in meanDataElementTag
        return std::find(meanDataElementTag.begin(), meanDataElementTag.end(), meanData) != meanDataElementTag.end();
    } else {
        throw ProcessError(TL("Invalid meanData pointer"));
    }
}


void
GNENetHelper::AttributeCarriers::insertMeanData(GNEMeanData* meanData) {
    // insert meanData
    if (myMeanDatas.at(meanData->getTagProperty().getTag()).insert(meanData).second == false) {
        throw ProcessError(meanData->getTagStr() + " with ID='" + meanData->getID() + "' already exist");
    }
    // add element in grid
    if (meanData->getTagProperty().isPlacedInRTree()) {
        myNet->addGLObjectIntoGrid(meanData);
    }
    // update geometry after insertion of meanDatas if myUpdateGeometryEnabled is enabled
    if (myNet->isUpdateGeometryEnabled()) {
        meanData->updateGeometry();
    }
    // meanDatas has to be saved
    myNet->getSavingStatus()->requireSaveMeanDatas();
}


void
GNENetHelper::AttributeCarriers::deleteMeanData(GNEMeanData* meanData) {
    // find demanElement in meanDataTag
    auto itFind = myMeanDatas.at(meanData->getTagProperty().getTag()).find(meanData);
    // check if meanData was previously inserted
    if (itFind == myMeanDatas.at(meanData->getTagProperty().getTag()).end()) {
        throw ProcessError(meanData->getTagStr() + " with ID='" + meanData->getID() + "' wasn't previously inserted");
    }
    // remove it from inspected elements and GNEElementTree
    myNet->getViewNet()->removeFromAttributeCarrierInspected(meanData);
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(meanData);
    // remove from container
    myMeanDatas.at(meanData->getTagProperty().getTag()).erase(itFind);
    // remove element from grid
    if (meanData->getTagProperty().isPlacedInRTree()) {
        myNet->removeGLObjectFromGrid(meanData);
    }
    // meanDatas has to be saved
    myNet->getSavingStatus()->requireSaveMeanDatas();
}


void
GNENetHelper::AttributeCarriers::updateDemandElementFrames(const GNETagProperties& tagProperty) {
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        // continue depending of demand mode
        switch (myNet->getViewNet()->getEditModes().demandEditMode) {
            case DemandEditMode::DEMAND_VEHICLE:
                if (tagProperty.isType()) {
                    myNet->getViewNet()->getViewParent()->getVehicleFrame()->getTypeSelector()->refreshDemandElementSelector();
                }
                break;
            case DemandEditMode::DEMAND_TYPE:
                if (tagProperty.isType()) {
                    myNet->getViewNet()->getViewParent()->getTypeFrame()->getTypeSelector()->refreshTypeSelector(true);
                }
                break;
            case DemandEditMode::DEMAND_TYPEDISTRIBUTION:
                if (tagProperty.isType()) {
                    myNet->getViewNet()->getViewParent()->getTypeDistributionFrame()->getDistributionSelector()->refreshDistributionSelector();
                }
                break;
            case DemandEditMode::DEMAND_ROUTEDISTRIBUTION:
                if (tagProperty.isRoute()) {
                    myNet->getViewNet()->getViewParent()->getRouteDistributionFrame()->getDistributionSelector()->refreshDistributionSelector();
                }
                break;
            case DemandEditMode::DEMAND_PERSON:
                if (tagProperty.isType()) {
                    myNet->getViewNet()->getViewParent()->getPersonFrame()->getTypeSelector()->refreshDemandElementSelector();
                }
                break;
            case DemandEditMode::DEMAND_PERSONPLAN:
                if (tagProperty.isPerson()) {
                    myNet->getViewNet()->getViewParent()->getPersonPlanFrame()->getPersonSelector()->refreshDemandElementSelector();
                }
                break;
            case DemandEditMode::DEMAND_CONTAINER:
                if (tagProperty.isType()) {
                    myNet->getViewNet()->getViewParent()->getContainerFrame()->getTypeSelector()->refreshDemandElementSelector();
                }
                break;
            case DemandEditMode::DEMAND_CONTAINERPLAN:
                if (tagProperty.isContainer()) {
                    myNet->getViewNet()->getViewParent()->getContainerPlanFrame()->getContainerSelector()->refreshDemandElementSelector();
                }
                break;
            case DemandEditMode::DEMAND_STOP:
                myNet->getViewNet()->getViewParent()->getStopFrame()->getStopParentSelector()->refreshDemandElementSelector();
                break;
            default:
                // nothing to update
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// GNENetHelper::SavingStatus - methods
// ---------------------------------------------------------------------------

GNENetHelper::SavingStatus::SavingStatus() {
}


void
GNENetHelper::SavingStatus::requireSaveSumoConfig() {
    mySumoConfigSaved = false;
}


void
GNENetHelper::SavingStatus::SumoConfigSaved() {
    mySumoConfigSaved = true;
}


bool
GNENetHelper::SavingStatus::isSumoConfigSaved() const {
    return mySumoConfigSaved;
}



void
GNENetHelper::SavingStatus::requireSaveNeteditConfig() {
    myNeteditConfigSaved = false;
}


void
GNENetHelper::SavingStatus::neteditConfigSaved() {
    myNeteditConfigSaved = true;
}


bool
GNENetHelper::SavingStatus::isNeteditConfigSaved() const {
    return myNeteditConfigSaved;
}


void
GNENetHelper::SavingStatus::requireSaveNetwork() {
    myNetworkSaved = false;
    // implies requiere save netedit config and sumo config
    myNeteditConfigSaved = false;
    mySumoConfigSaved = false;
}


void
GNENetHelper::SavingStatus::networkSaved() {
    myNetworkSaved = true;
}


bool
GNENetHelper::SavingStatus::isNetworkSaved() const {
    return myNetworkSaved;
}


void
GNENetHelper::SavingStatus::requireSaveTLS() {
    myTLSSaved = false;
}


void
GNENetHelper::SavingStatus::TLSSaved() {
    myTLSSaved = true;
}


bool
GNENetHelper::SavingStatus::isTLSSaved() const {
    return myTLSSaved;
}


void
GNENetHelper::SavingStatus::requireSaveEdgeType() {
    myEdgeTypeSaved = false;
}


void
GNENetHelper::SavingStatus::edgeTypeSaved() {
    myEdgeTypeSaved = true;
}


bool
GNENetHelper::SavingStatus::isEdgeTypeSaved() const {
    return myEdgeTypeSaved;
}


void
GNENetHelper::SavingStatus::requireSaveAdditionals() {
    myAdditionalSaved = false;
    // implies requiere save netedit config and sumo config
    myNeteditConfigSaved = false;
    mySumoConfigSaved = false;
}


void
GNENetHelper::SavingStatus::additionalsSaved() {
    myAdditionalSaved = true;
}


bool
GNENetHelper::SavingStatus::isAdditionalsSaved() const {
    return myAdditionalSaved;
}


void
GNENetHelper::SavingStatus::requireSaveDemandElements() {
    myDemandElementSaved = false;
    // implies requiere save netedit config and sumo config
    myNeteditConfigSaved = false;
    mySumoConfigSaved = false;
}


void
GNENetHelper::SavingStatus::demandElementsSaved() {
    myDemandElementSaved = true;
}


bool
GNENetHelper::SavingStatus::isDemandElementsSaved() const {
    return myDemandElementSaved;
}


void
GNENetHelper::SavingStatus::requireSaveDataElements() {
    myDataElementSaved = false;
    // implies requiere save netedit config and sumo config
    myNeteditConfigSaved = false;
    mySumoConfigSaved = false;
}


void
GNENetHelper::SavingStatus::dataElementsSaved() {
    myDataElementSaved = true;
}


bool
GNENetHelper::SavingStatus::isDataElementsSaved() const {
    return myDataElementSaved;
}


void
GNENetHelper::SavingStatus::requireSaveMeanDatas() {
    myMeanDataElementSaved = false;
    // implies requiere save netedit config and sumo config
    myNeteditConfigSaved = false;
    mySumoConfigSaved = false;
}


void
GNENetHelper::SavingStatus::meanDatasSaved() {
    myMeanDataElementSaved = true;
}


bool
GNENetHelper::SavingStatus::isMeanDatasSaved() const {
    return myMeanDataElementSaved;
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
    return TL("Redo replace in TLS");
}


std::string
GNENetHelper::GNEChange_ReplaceEdgeInTLS::redoName() const {
    return TL("Undo replace in TLS");
}


bool
GNENetHelper::GNEChange_ReplaceEdgeInTLS::trueChange() {
    return myReplaced != myBy;
}

/****************************************************************************/
