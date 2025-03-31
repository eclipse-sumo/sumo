/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNEAccess.h>
#include <netedit/elements/additional/GNEBusStop.h>
#include <netedit/elements/additional/GNECalibrator.h>
#include <netedit/elements/additional/GNECalibratorFlow.h>
#include <netedit/elements/additional/GNEChargingStation.h>
#include <netedit/elements/additional/GNEClosingLaneReroute.h>
#include <netedit/elements/additional/GNEClosingReroute.h>
#include <netedit/elements/additional/GNEContainerStop.h>
#include <netedit/elements/additional/GNEDestProbReroute.h>
#include <netedit/elements/additional/GNEEntryExitDetector.h>
#include <netedit/elements/additional/GNEInductionLoopDetector.h>
#include <netedit/elements/additional/GNEInstantInductionLoopDetector.h>
#include <netedit/elements/additional/GNELaneAreaDetector.h>
#include <netedit/elements/additional/GNEMultiEntryExitDetector.h>
#include <netedit/elements/additional/GNEOverheadWire.h>
#include <netedit/elements/additional/GNEPOI.h>
#include <netedit/elements/additional/GNEParkingArea.h>
#include <netedit/elements/additional/GNEParkingAreaReroute.h>
#include <netedit/elements/additional/GNEParkingSpace.h>
#include <netedit/elements/additional/GNEPoly.h>
#include <netedit/elements/additional/GNERerouter.h>
#include <netedit/elements/additional/GNERerouterInterval.h>
#include <netedit/elements/additional/GNERerouterSymbol.h>
#include <netedit/elements/additional/GNERouteProbReroute.h>
#include <netedit/elements/additional/GNERouteProbe.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/additional/GNETAZSourceSink.h>
#include <netedit/elements/additional/GNETAZSourceSink.h>
#include <netedit/elements/additional/GNETractionSubstation.h>
#include <netedit/elements/additional/GNEVaporizer.h>
#include <netedit/elements/additional/GNEVariableSpeedSign.h>
#include <netedit/elements/additional/GNEVariableSpeedSignStep.h>
#include <netedit/elements/additional/GNEVariableSpeedSignSymbol.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <netedit/elements/data/GNEMeanData.h>
#include <netedit/elements/demand/GNEContainer.h>
#include <netedit/elements/demand/GNEPerson.h>
#include <netedit/elements/demand/GNEPersonTrip.h>
#include <netedit/elements/demand/GNERide.h>
#include <netedit/elements/demand/GNERoute.h>
#include <netedit/elements/demand/GNEStop.h>
#include <netedit/elements/demand/GNEStopPlan.h>
#include <netedit/elements/demand/GNETranship.h>
#include <netedit/elements/demand/GNETransport.h>
#include <netedit/elements/demand/GNEVType.h>
#include <netedit/elements/demand/GNEVTypeDistribution.h>
#include <netedit/elements/demand/GNEVehicle.h>
#include <netedit/elements/demand/GNEWalk.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/elements/network/GNEEdgeTemplate.h>
#include <netedit/elements/network/GNEEdgeType.h>
#include <netedit/elements/network/GNEInternalLane.h>
#include <netedit/elements/network/GNEWalkingArea.h>
#include <netedit/frames/GNEDemandSelector.h>
#include <netedit/frames/GNEElementTree.h>
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
#include <utils/gui/globjects/GUISaveDialog.h>
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
    auto additionalTagProperties = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::ADDITIONALELEMENT |
                                   GNETagProperties::TagType::SHAPE | GNETagProperties::TagType::TAZELEMENT | GNETagProperties::TagType::WIRE);
    for (const auto& additionalTagProperty : additionalTagProperties) {
        myAdditionals.insert(std::make_pair(additionalTagProperty->getTag(), std::unordered_map<const GUIGlObject*, GNEAdditional*>()));
        if (additionalTagProperty->hasAttribute(SUMO_ATTR_ID) || (additionalTagProperty->getTag() == SUMO_TAG_VAPORIZER)) {
            myAdditionalIDs.insert(std::make_pair(additionalTagProperty->getTag(), std::map<const std::string, GNEAdditional*>()));
        }
    }
    myTAZSourceSinks.insert(std::make_pair(SUMO_TAG_TAZSOURCE, std::unordered_map<const GNEAttributeCarrier*, GNETAZSourceSink*>()));
    myTAZSourceSinks.insert(std::make_pair(SUMO_TAG_TAZSINK, std::unordered_map<const GNEAttributeCarrier*, GNETAZSourceSink*>()));
    // fill demand elements with tags
    auto demandElementTagProperties = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::DEMANDELEMENT);
    for (const auto& demandElementTagProperty : demandElementTagProperties) {
        myDemandElements.insert(std::make_pair(demandElementTagProperty->getTag(), std::unordered_map<const GUIGlObject*, GNEDemandElement*>()));
        if (demandElementTagProperty->hasAttribute(SUMO_ATTR_ID)) {
            myDemandElementIDs.insert(std::make_pair(demandElementTagProperty->getTag(), std::map<const std::string, GNEDemandElement*>()));
        }
    }
    auto stopTagProperties = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::STOP_VEHICLE);
    for (const auto& stopTagProperty : stopTagProperties) {
        myDemandElements.insert(std::make_pair(stopTagProperty->getTag(), std::unordered_map<const GUIGlObject*, GNEDemandElement*>()));
    }
    // fill data elements with tags
    auto genericDataElementTagProperties = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::GENERICDATA);
    for (const auto& genericDataElementTagProperty : genericDataElementTagProperties) {
        myGenericDatas.insert(std::make_pair(genericDataElementTagProperty->getTag(), std::unordered_map<const GUIGlObject*, GNEGenericData*>()));
    }
    // fill meanDatas with tags
    auto meanDataTagProperties = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::MEANDATA);
    for (const auto& meanDataTagProperty : meanDataTagProperties) {
        myMeanDatas.insert(std::make_pair(meanDataTagProperty->getTag(), std::map<const std::string, GNEMeanData*>()));
    }
}


GNENetHelper::AttributeCarriers::~AttributeCarriers() {
    // Drop EdgeTypes
    for (const auto& edgeType : myEdgeTypes) {
        edgeType.second->decRef("GNENetHelper::~GNENet");
        delete edgeType.second;
    }
    // Drop Edges
    for (const auto& edge : myEdges) {
        edge.second->decRef("GNENetHelper::~GNENet");
        delete edge.second;
    }
    // Drop myJunctions
    for (const auto& junction : myJunctions) {
        junction.second->decRef("GNENetHelper::~GNENet");
        delete junction.second;
    }
    // Drop Additionals (Only used for additionals that were inserted without using GNEChange_Additional)
    for (const auto& additionalTag : myAdditionals) {
        for (const auto& additional : additionalTag.second) {
            // decrease reference manually (because it was increased manually in GNEAdditionalHandler)
            additional.second->decRef();
            delete additional.second;
        }
    }
    // Drop demand elements (Only used for demand elements that were inserted without using GNEChange_DemandElement, for example the default VType")
    for (const auto& demandElementTag : myDemandElements) {
        for (const auto& demandElement : demandElementTag.second) {
            // decrease reference manually (because it was increased manually in GNERouteHandler)
            demandElement.second->decRef();
            delete demandElement.second;
        }
    }
    // Drop dataSets (Only used for TAZElements that were inserted without using GNEChange_DataSets)
    for (const auto& dataSet : myDataSets) {
        // decrease reference manually (because it was increased manually in GNEDataHandler)
        dataSet.second->decRef();
        delete dataSet.second;
    }
    // Drop MeanDatas (Only used for meanDatas that were inserted without using GNEChange_MeanData)
    for (const auto& meanDataTag : myMeanDatas) {
        for (const auto& meanData : meanDataTag.second) {
            // decrease reference manually (because it was increased manually in GNEMeanDataHandler)
            meanData.second->decRef();
            delete meanData.second;
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
GNENetHelper::AttributeCarriers::isNetworkElementAroundTriangle(GNEAttributeCarrier* AC, const Triangle& triangle) const {
    // check what type of AC
    if (AC->getTagProperty()->getTag() == SUMO_TAG_JUNCTION) {
        // Junction
        const GNEJunction* junction = myJunctions.at(AC->getID());
        if (junction->getNBNode()->getShape().size() == 0) {
            return triangle.isPositionWithin(junction->getNBNode()->getCenter());
        } else {
            return (triangle.intersectWithShape(junction->getNBNode()->getShape()));
        }
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_EDGE) {
        // Edge
        for (const auto& lane : myEdges.at(AC->getID())->getChildLanes()) {
            if (triangle.intersectWithShape(lane->getLaneShape())) {
                return true;
            }
        }
        return false;
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_LANE) {
        // Lane
        return triangle.intersectWithShape(retrieveLane(AC->getID())->getLaneShape());
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_CONNECTION) {
        // connection
        return triangle.intersectWithShape(myConnections.at(AC->getGUIGlObject())->getConnectionShape());
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_CROSSING) {
        // crossing
        return triangle.intersectWithShape(myCrossings.at(AC->getGUIGlObject())->getCrossingShape());
    } else if (AC->getTagProperty()->isAdditionalElement()) {
        // Additional (including shapes and TAZs
        const GNEAdditional* additional = retrieveAdditional(AC->getGUIGlObject());
        if (additional->getAdditionalGeometry().getShape().size() <= 1) {
            return triangle.isPositionWithin(additional->getPositionInView());
        } else {
            return triangle.intersectWithShape(additional->getAdditionalGeometry().getShape());
        }
    } else {
        return false;
    }
}


int
GNENetHelper::AttributeCarriers::getNumberOfNetworkElements() const {
    return myNumberOfNetworkElements;
}


int
GNENetHelper::AttributeCarriers::getNumberOfDemandElements() const {
    return myNumberOfDemandElements;
}


int
GNENetHelper::AttributeCarriers::getNumberOfDataElements() const {
    return myNumberOfDataElements;
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
    std::vector<GNEAttributeCarrier*> ACs;
    // first check if we have to return all ACs
    if (tag == SUMO_TAG_NOTHING) {
        const auto rootTagProperty = myNet->getTagPropertiesDatabase()->getTagProperty(SUMO_TAG_ROOTFILE, true);
        ACs.reserve(myNumberOfNetworkElements + myNumberOfDemandElements + myNumberOfDataElements);
        retrieveAttributeCarriersRecursively(rootTagProperty, ACs);
    } else {
        const auto tagProperty = myNet->getTagPropertiesDatabase()->getTagProperty(tag, true);
        // reserve space
        if (tagProperty->getSupermode() == Supermode::NETWORK) {
            ACs.reserve(myNumberOfNetworkElements);
        } else if (tagProperty->getSupermode() == Supermode::DEMAND) {
            ACs.reserve(myNumberOfDemandElements);
        } else if (tagProperty->getSupermode() == Supermode::DATA) {
            ACs.reserve(myNumberOfDataElements + myNumberOfMeanDataElements);
        }
        retrieveAttributeCarriersRecursively(tagProperty, ACs);
    }
    return ACs;
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
            if (!onlySelected || crossing.second->isAttributeCarrierSelected()) {
                result.push_back(crossing.second);
            }
        }
        for (const auto& edge : myEdges) {
            if (!onlySelected || edge.second->isAttributeCarrierSelected()) {
                result.push_back(edge.second);
            }
        }
        for (const auto& lane : myLanes) {
            if (!onlySelected || lane.second->isAttributeCarrierSelected()) {
                result.push_back(lane.second);
            }
        }
        for (const auto& connection : myConnections) {
            if (!onlySelected || connection.second->isAttributeCarrierSelected()) {
                result.push_back(connection.second);
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
            if (!onlySelected || dataSet.second->isAttributeCarrierSelected()) {
                result.push_back(dataSet.second);
            }
        }
        for (const auto& dataInterval : myDataIntervals) {
            if (!onlySelected || dataInterval.second->isAttributeCarrierSelected()) {
                result.push_back(dataInterval.second);
            }
        }
        for (const auto& genericDataSet : myGenericDatas) {
            for (const auto& genericData : genericDataSet.second) {
                if (!onlySelected || genericData.second->isAttributeCarrierSelected()) {
                    result.push_back(genericData.second);
                }
            }
        }
        for (const auto& meanDataSet : myMeanDatas) {
            for (const auto& meanData : meanDataSet.second) {
                if (!onlySelected || meanData.second->isAttributeCarrierSelected()) {
                    result.push_back(meanData.second);
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
            if (ignoreCurrentSupermode) {
                result.push_back(AC);
            } else if (editModes.isCurrentSupermodeNetwork() && (AC->getTagProperty()->isNetworkElement() ||
                       AC->getTagProperty()->isAdditionalElement())) {
                result.push_back(AC);
            } else if (editModes.isCurrentSupermodeDemand() && AC->getTagProperty()->isDemandElement()) {
                result.push_back(AC);
            } else if (editModes.isCurrentSupermodeData() && AC->getTagProperty()->isDataElement()) {
                result.push_back(AC);
            }
        }
    }
    return result;
}


GNEJunction*
GNENetHelper::AttributeCarriers::retrieveJunction(const std::string& id, bool hardFail) const {
    auto it = myJunctions.find(id);
    if (it != myJunctions.end()) {
        return it->second;
    }
    if (hardFail) {
        // If junction wasn't found, throw exception
        throw UnknownElement("Attempted to retrieve non-existant junction " + id);
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
    myNumberOfNetworkElements++;
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
    auto junctionCopy = myJunctions;
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
GNENetHelper::AttributeCarriers::retrieveCrossing(const GUIGlObject* glObject, bool hardFail) const {
    auto it = myCrossings.find(glObject);
    if (it != myCrossings.end()) {
        return it->second;
    }
    if (hardFail) {
        // If junction wasn't found, throw exception
        throw UnknownElement("Attempted to retrieve non-existant crossing " + glObject->getMicrosimID());
    } else {
        return nullptr;
    }
}


const std::unordered_map<const GUIGlObject*, GNECrossing*>&
GNENetHelper::AttributeCarriers::getCrossings() const {
    return myCrossings;
}


std::vector<GNECrossing*>
GNENetHelper::AttributeCarriers::getSelectedCrossings() const {
    std::vector<GNECrossing*> result;
    // iterate over crossings
    for (const auto& crossing : myCrossings) {
        if (crossing.second->isAttributeCarrierSelected()) {
            result.push_back(crossing.second);
        }
    }
    return result;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedCrossings() const {
    int counter = 0;
    for (const auto& crossing : myCrossings) {
        if (crossing.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


GNEWalkingArea*
GNENetHelper::AttributeCarriers::retrieveWalkingArea(const GUIGlObject* glObject, bool hardFail) const {
    auto it = myWalkingAreas.find(glObject);
    if (it != myWalkingAreas.end()) {
        return it->second;
    }
    if (hardFail) {
        // If junction wasn't found, throw exception
        throw UnknownElement("Attempted to retrieve non-existant walkingArea " + glObject->getMicrosimID());
    } else {
        return nullptr;
    }
}


const std::unordered_map<const GUIGlObject*, GNEWalkingArea*>&
GNENetHelper::AttributeCarriers::getWalkingAreas() const {
    return myWalkingAreas;
}


std::vector<GNEWalkingArea*>
GNENetHelper::AttributeCarriers::getSelectedWalkingAreas() const {
    std::vector<GNEWalkingArea*> result;
    // iterate over walkingAreas
    for (const auto& walkingArea : myWalkingAreas) {
        if (walkingArea.second->isAttributeCarrierSelected()) {
            result.push_back(walkingArea.second);
        }
    }
    return result;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedWalkingAreas() const {
    int counter = 0;
    for (const auto& walkingArea : myWalkingAreas) {
        if (walkingArea.second->isAttributeCarrierSelected()) {
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
        throw UnknownElement("Attempted to retrieve non-existant EdgeType " + id);
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
    auto it = myEdges.find(id);
    if (it != myEdges.end()) {
        return it->second;
    }
    if (hardFail) {
        // If edge wasn't found, throw exception
        throw UnknownElement("Attempted to retrieve non-existant edge " + id);
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
    // iterate over outgoing edges of from edge and check to junction
    for (const auto& edgeTo : from->getGNEOutgoingEdges()) {
        if (edgeTo->getToJunction() == to) {
            edges.push_back(edgeTo);
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
    myNumberOfNetworkElements++;
    // insert all lanes
    for (const auto& lane : edge->getChildLanes()) {
        insertLane(lane);
    }
    // Add references into GNEJunctions
    edge->getFromJunction()->addOutgoingGNEEdge(edge);
    edge->getToJunction()->addIncomingGNEEdge(edge);
    // update boundaries of both junctions (to remove it from Grid)
    edge->getFromJunction()->updateCenteringBoundary(true);
    edge->getToJunction()->updateCenteringBoundary(true);
    // update edge boundary
    edge->updateCenteringBoundary(false);
    // expand edge boundary
    myNet->expandBoundary(edge->getCenteringBoundary());
    // finally add edge into grid
    myNet->addGLObjectIntoGrid(edge);
    return edge;
}


void
GNENetHelper::AttributeCarriers::clearEdges() {
    myEdges.clear();
}


void
GNENetHelper::AttributeCarriers::addPrefixToEdges(const std::string& prefix) {
    // make a copy of edges
    auto edgeCopy = myEdges;
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


std::string
GNENetHelper::AttributeCarriers::generateEdgeID() const {
    // get edge prefix
    const std::string edgePrefix = OptionsCont::getOptions().getString("prefix") + OptionsCont::getOptions().getString("edge-prefix");
    // generate new ID
    while (myEdges.count(edgePrefix + toString(myNet->getEdgeIDCounter())) != 0) {
        myNet->getEdgeIDCounter()++;
    }
    return edgePrefix + toString(myNet->getEdgeIDCounter());
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
        for (const auto& lane : edge->getChildLanes()) {
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
        for (auto laneIt : edge->getChildLanes()) {
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
                    myNet->getEdgesAndNumberOfLanes().at(edge_id) != (int)edge->getChildLanes().size()) {
                return edge->getChildLanes().at(lane->getIndex() + 1);
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
GNENetHelper::AttributeCarriers::retrieveLane(const GUIGlObject* glObject, bool hardFail) const {
    auto it = myLanes.find(glObject);
    if (it != myLanes.end()) {
        return it->second;
    }
    if (hardFail) {
        // If junction wasn't found, throw exception
        throw UnknownElement("Attempted to retrieve non-existant lane " + glObject->getMicrosimID());
    } else {
        return nullptr;
    }
}


const std::unordered_map<const GUIGlObject*, GNELane*>&
GNENetHelper::AttributeCarriers::getLanes() const {
    return myLanes;
}


std::vector<GNELane*>
GNENetHelper::AttributeCarriers::getSelectedLanes() const {
    std::vector<GNELane*> result;
    // returns lanes depending of selection
    for (const auto& lane : myLanes) {
        if (lane.second->isAttributeCarrierSelected()) {
            result.push_back(lane.second);
        }
    }
    return result;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedLanes() const {
    int counter = 0;
    for (const auto& lane : myLanes) {
        if (lane.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


GNEConnection*
GNENetHelper::AttributeCarriers::retrieveConnection(const std::string& id, bool hardFail) const {
    // iterate over connections
    for (const auto& connection : myConnections) {
        if (connection.second->getID() == id) {
            return connection.second;
        }
    }
    if (hardFail) {
        // If POI wasn't found, throw exception
        throw UnknownElement("Attempted to retrieve non-existant connection " + id);
    } else {
        return nullptr;
    }
}


GNEConnection*
GNENetHelper::AttributeCarriers::retrieveConnection(const GUIGlObject* glObject, bool hardFail) const {
    auto it = myConnections.find(glObject);
    if (it != myConnections.end()) {
        return it->second;
    }
    if (hardFail) {
        // If POI wasn't found, throw exception
        throw UnknownElement("Attempted to retrieve non-existant connection " + glObject->getMicrosimID());
    } else {
        return nullptr;
    }
}


const std::unordered_map<const GUIGlObject*, GNEConnection*>&
GNENetHelper::AttributeCarriers::getConnections() const {
    return myConnections;
}


std::vector<GNEConnection*>
GNENetHelper::AttributeCarriers::getSelectedConnections() const {
    std::vector<GNEConnection*> result;
    // returns connections depending of selection
    for (const auto& connection : myConnections) {
        if (connection.second->isAttributeCarrierSelected()) {
            result.push_back(connection.second);
        }
    }
    return result;
}


int
GNENetHelper::AttributeCarriers::getNumberOfSelectedConnections() const {
    int counter = 0;
    for (const auto& connection : myConnections) {
        if (connection.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


GNEInternalLane*
GNENetHelper::AttributeCarriers::retrieveInternalLane(const GUIGlObject* glObject, bool hardFail) const {
    auto it = myInternalLanes.find(glObject);
    if (it != myInternalLanes.end()) {
        return it->second;
    }
    if (hardFail) {
        // If POI wasn't found, throw exception
        throw UnknownElement("Attempted to retrieve non-existant internalLane " + glObject->getMicrosimID());
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNENetHelper::AttributeCarriers::retrieveAdditional(SumoXMLTag type, const std::string& id, bool hardFail) const {
    auto it = myAdditionalIDs.at(type).find(id);
    if (it != myAdditionalIDs.at(type).end()) {
        return it->second;
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
        auto it = myAdditionalIDs.at(type).find(id);
        if (it != myAdditionalIDs.at(type).end()) {
            return it->second;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant additional (string)");
    } else {
        return nullptr;
    }
}


GNEAdditional*
GNENetHelper::AttributeCarriers::retrieveAdditional(const GUIGlObject* glObject, bool hardFail) const {
    // iterate over all additionals
    for (const auto& additionalTag : myAdditionals) {
        auto it = additionalTag.second.find(glObject);
        if (it != additionalTag.second.end()) {
            return it->second;
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
        if (interval->getTagProperty()->getTag() == SUMO_TAG_INTERVAL) {
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


const std::unordered_map<SumoXMLTag, std::unordered_map<const GUIGlObject*, GNEAdditional*>, std::hash<int> >&
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
            if (additional.second->getTagProperty()->isShapeElement() && additional.second->isAttributeCarrierSelected()) {
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
    for (auto& additionals : myAdditionalIDs) {
        additionals.second.clear();
    }
}


void
GNENetHelper::AttributeCarriers::updateAdditionalID(GNEAdditional* additional, const std::string& newID) {
    const auto tag = additional->getTagProperty()->getTag();
    const auto it = myAdditionalIDs.at(tag).find(additional->getID());
    if (it == myAdditionalIDs.at(tag).end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' doesn't exist in AttributeCarriers.additionals");
    } else {
        // remove from container, set new Id, and insert it again
        myAdditionalIDs.at(tag).erase(it);
        // set microsim ID
        additional->setMicrosimID(newID);
        myAdditionalIDs.at(tag)[newID] = additional;
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
           getNumberOfSelectedTAZs() -
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
GNENetHelper::AttributeCarriers::getNumberOfSelectedWires() const {
    int counter = 0;
    for (const auto& additionalsTags : myAdditionals) {
        for (const auto& additional : additionalsTags.second) {
            if (additional.second->isAttributeCarrierSelected() && additional.second->getTagProperty()->isWireElement()) {
                counter++;
            }
        }
    }
    return counter;
}


GNETAZSourceSink*
GNENetHelper::AttributeCarriers::retrieveTAZSourceSink(const GNEAttributeCarrier* sourceSink, bool hardFail) const {
    // iterate over all demand elements
    for (const auto& TAZSourceSinkTag : myTAZSourceSinks) {
        auto it = TAZSourceSinkTag.second.find(sourceSink);
        if (it != TAZSourceSinkTag.second.end()) {
            return it->second;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant sourceSink (glObject)");
    } else {
        return nullptr;
    }
}


const std::unordered_map<SumoXMLTag, std::unordered_map<const GNEAttributeCarrier*, GNETAZSourceSink*>, std::hash<int> >&
GNENetHelper::AttributeCarriers::getTAZSourceSinks() const {
    return myTAZSourceSinks;
}


int
GNENetHelper::AttributeCarriers::getNumberOfTAZSourceSinks() const {
    int counter = 0;
    for (const auto& sourceSinksTag : myTAZSourceSinks) {
        counter += (int)sourceSinksTag.second.size();
    }
    return counter;
}


void
GNENetHelper::AttributeCarriers::clearTAZSourceSinks() {
    // iterate over myTAZSourceSinks and clear all sourceSinks
    for (auto& sourceSinksTags : myTAZSourceSinks) {
        sourceSinksTags.second.clear();
    }
}


GNEDemandElement*
GNENetHelper::AttributeCarriers::retrieveDemandElement(SumoXMLTag type, const std::string& id, bool hardFail) const {
    auto it = myDemandElementIDs.at(type).find(id);
    if (it != myDemandElementIDs.at(type).end()) {
        return it->second;
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
        auto it = myDemandElementIDs.at(type).find(id);
        if (it != myDemandElementIDs.at(type).end()) {
            return it->second;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant demand element (string)");
    } else {
        return nullptr;
    }
}


GNEDemandElement*
GNENetHelper::AttributeCarriers::retrieveDemandElement(const GUIGlObject* glObject, bool hardFail) const {
    // iterate over all demand elements
    for (const auto& demandElementTag : myDemandElements) {
        auto it = demandElementTag.second.find(glObject);
        if (it != demandElementTag.second.end()) {
            return it->second;
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


const std::unordered_map<SumoXMLTag, std::unordered_map<const GUIGlObject*, GNEDemandElement*>, std::hash<int> >&
GNENetHelper::AttributeCarriers::getDemandElements() const {
    return myDemandElements;
}


std::string
GNENetHelper::AttributeCarriers::generateDemandElementID(SumoXMLTag tag) const {
    // obtain option container
    const auto& neteditOptions = OptionsCont::getOptions();
    // get tag property
    const auto tagProperty = myNet->getTagPropertiesDatabase()->getTagProperty(tag, true);
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
    } else if (tagProperty->isVehicle() && !tagProperty->isFlow()) {
        prefix = neteditOptions.getString("vehicle-prefix");
    } else if (tagProperty->isPerson()) {
        if (tagProperty->isFlow()) {
            prefix = neteditOptions.getString("personflow-prefix");
        } else {
            prefix = neteditOptions.getString("person-prefix");
        }
    } else if (tagProperty->isContainer()) {
        if (tagProperty->isFlow()) {
            prefix = neteditOptions.getString("containerflow-prefix");
        } else {
            prefix = neteditOptions.getString("container-prefix");
        }
    } else if (tagProperty->isFlow()) {
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
    auto it = myDemandElementIDs.at(SUMO_TAG_VTYPE).find(DEFAULT_VTYPE_ID);
    if (it != myDemandElementIDs.at(SUMO_TAG_VTYPE).end()) {
        return it->second;
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
    for (auto& demandElements : myDemandElementIDs) {
        demandElements.second.clear();
    }
}


void
GNENetHelper::AttributeCarriers::updateDemandElementID(GNEDemandElement* demandElement, const std::string& newID) {
    const auto tag = demandElement->getTagProperty()->getTag();
    const auto it = myDemandElementIDs.at(tag).find(demandElement->getID());
    if (it == myDemandElementIDs.at(tag).end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' doesn't exist in AttributeCarriers.demandElements");
    } else {
        // remove from container, set new Id, and insert it again
        myDemandElementIDs.at(tag).erase(it);
        // set microsim ID
        demandElement->setMicrosimID(newID);
        myDemandElementIDs.at(tag)[newID] = demandElement;
    }
}


void
GNENetHelper::AttributeCarriers::addDefaultVTypes() {
    // Create default vehicle Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultVehicleType = new GNEVType(DEFAULT_VTYPE_ID, myNet, SVC_PASSENGER);
    myDemandElements.at(defaultVehicleType->getTagProperty()->getTag()).insert(std::make_pair(defaultVehicleType->getGUIGlObject(), defaultVehicleType));
    myDemandElementIDs.at(defaultVehicleType->getTagProperty()->getTag()).insert(std::make_pair(defaultVehicleType->getID(), defaultVehicleType));
    defaultVehicleType->incRef("GNENet::DEFAULT_VEHTYPE");

    // Create default Bike Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultBikeType = new GNEVType(DEFAULT_BIKETYPE_ID, myNet, SVC_BICYCLE);
    myDemandElements.at(defaultBikeType->getTagProperty()->getTag()).insert(std::make_pair(defaultBikeType->getGUIGlObject(), defaultBikeType));
    myDemandElementIDs.at(defaultBikeType->getTagProperty()->getTag()).insert(std::make_pair(defaultBikeType->getID(), defaultBikeType));
    defaultBikeType->parametersSet |= VTYPEPARS_VEHICLECLASS_SET;
    defaultBikeType->incRef("GNENet::DEFAULT_BIKETYPE_ID");

    // Create default taxi Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultTaxiType = new GNEVType(DEFAULT_TAXITYPE_ID, myNet, SVC_TAXI);
    myDemandElements.at(defaultTaxiType->getTagProperty()->getTag()).insert(std::make_pair(defaultTaxiType->getGUIGlObject(), defaultTaxiType));
    myDemandElementIDs.at(defaultTaxiType->getTagProperty()->getTag()).insert(std::make_pair(defaultTaxiType->getID(), defaultTaxiType));
    defaultTaxiType->parametersSet |= VTYPEPARS_VEHICLECLASS_SET;
    defaultTaxiType->incRef("GNENet::DEFAULT_TAXITYPE_ID");

    // Create default rail Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultRailType = new GNEVType(DEFAULT_RAILTYPE_ID, myNet, SVC_RAIL);
    myDemandElements.at(defaultRailType->getTagProperty()->getTag()).insert(std::make_pair(defaultRailType->getGUIGlObject(), defaultRailType));
    myDemandElementIDs.at(defaultRailType->getTagProperty()->getTag()).insert(std::make_pair(defaultRailType->getID(), defaultRailType));
    defaultRailType->parametersSet |= VTYPEPARS_VEHICLECLASS_SET;
    defaultRailType->incRef("GNENet::DEFAULT_RAILTYPE_ID");

    // Create default person Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultPersonType = new GNEVType(DEFAULT_PEDTYPE_ID, myNet, SVC_PEDESTRIAN);
    myDemandElements.at(defaultPersonType->getTagProperty()->getTag()).insert(std::make_pair(defaultPersonType->getGUIGlObject(), defaultPersonType));
    myDemandElementIDs.at(defaultPersonType->getTagProperty()->getTag()).insert(std::make_pair(defaultPersonType->getID(), defaultPersonType));
    defaultPersonType->parametersSet |= VTYPEPARS_VEHICLECLASS_SET;
    defaultPersonType->incRef("GNENet::DEFAULT_PEDTYPE_ID");

    // Create default container Type (it has to be created here due myViewNet was previously nullptr)
    GNEVType* defaultContainerType = new GNEVType(DEFAULT_CONTAINERTYPE_ID, myNet, SVC_IGNORING);
    myDemandElements.at(defaultContainerType->getTagProperty()->getTag()).insert(std::make_pair(defaultContainerType->getGUIGlObject(), defaultContainerType));
    myDemandElementIDs.at(defaultContainerType->getTagProperty()->getTag()).insert(std::make_pair(defaultContainerType->getID(), defaultContainerType));
    defaultContainerType->parametersSet |= VTYPEPARS_VEHICLECLASS_SET;
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
            if (personPlan->getTagProperty()->isPlanPersonTrip() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow.second->getChildDemandElements()) {
            if (personPlan->getTagProperty()->isPlanPersonTrip() && personPlan->isAttributeCarrierSelected()) {
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
            if (personPlan->getTagProperty()->isPlanWalk() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow.second->getChildDemandElements()) {
            if (personPlan->getTagProperty()->isPlanWalk() && personPlan->isAttributeCarrierSelected()) {
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
            if (personPlan->getTagProperty()->isPlanRide() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow.second->getChildDemandElements()) {
            if (personPlan->getTagProperty()->isPlanRide() && personPlan->isAttributeCarrierSelected()) {
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
            if (containerPlan->getTagProperty()->isPlanTransport() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto& containerPlan : containerFlow.second->getChildDemandElements()) {
            if (containerPlan->getTagProperty()->isPlanTransport() && containerPlan->isAttributeCarrierSelected()) {
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
            if (containerPlan->getTagProperty()->isPlanTranship() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto& containerPlan : containerFlow.second->getChildDemandElements()) {
            if (containerPlan->getTagProperty()->isPlanTranship() && containerPlan->isAttributeCarrierSelected()) {
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
            if (stop->getTagProperty()->isVehicleStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& vehicle : myDemandElements.at(GNE_TAG_VEHICLE_WITHROUTE)) {
        for (const auto& stop : vehicle.second->getChildDemandElements().front()->getChildDemandElements()) {
            if (stop->getTagProperty()->isVehicleStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& flow : myDemandElements.at(SUMO_TAG_FLOW)) {
        for (const auto& stop : flow.second->getChildDemandElements()) {
            if (stop->getTagProperty()->isVehicleStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& flow : myDemandElements.at(GNE_TAG_FLOW_WITHROUTE)) {
        for (const auto& stop : flow.second->getChildDemandElements().front()->getChildDemandElements()) {
            if (stop->getTagProperty()->isVehicleStop() && stop->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    // persons
    for (const auto& person : myDemandElements.at(SUMO_TAG_PERSON)) {
        for (const auto& personPlan : person.second->getChildDemandElements()) {
            if (personPlan->getTagProperty()->isPlanStopPerson() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& personFlow : myDemandElements.at(SUMO_TAG_PERSONFLOW)) {
        for (const auto& personPlan : personFlow.second->getChildDemandElements()) {
            if (personPlan->getTagProperty()->isPlanStopPerson() && personPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    // containers
    for (const auto& container : myDemandElements.at(SUMO_TAG_CONTAINER)) {
        for (const auto& containerPlan : container.second->getChildDemandElements()) {
            if (containerPlan->getTagProperty()->isPlanStopContainer() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    for (const auto& containerFlow : myDemandElements.at(SUMO_TAG_CONTAINERFLOW)) {
        for (const auto& containerPlan : containerFlow.second->getChildDemandElements()) {
            if (containerPlan->getTagProperty()->isPlanStopContainer() && containerPlan->isAttributeCarrierSelected()) {
                counter++;
            }
        }
    }
    return counter;
}


GNEDataSet*
GNENetHelper::AttributeCarriers::retrieveDataSet(const std::string& id, bool hardFail) const {
    for (const auto& dataSet : myDataSets) {
        if (dataSet.second->getID() == id) {
            return dataSet.second;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant data set");
    } else {
        return nullptr;
    }
}


const std::map<const std::string, GNEDataSet*>&
GNENetHelper::AttributeCarriers::getDataSets() const {
    return myDataSets;
}


std::string
GNENetHelper::AttributeCarriers::generateDataSetID() const {
    // get prefix
    const auto prefix = OptionsCont::getOptions().getString("dataSet-prefix");
    int counter = 0;
    while (retrieveDataSet(prefix + "_" + toString(counter), false) != nullptr) {
        counter++;
    }
    return (prefix + "_" + toString(counter));
}


GNEDataInterval*
GNENetHelper::AttributeCarriers::retrieveDataInterval(const GNEAttributeCarrier* AC, bool hardFail) const {
    if (myDataIntervals.count(AC)) {
        return myDataIntervals.at(AC);
    } else if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant data interval");
    } else {
        return nullptr;
    }
}


const std::unordered_map<const GNEAttributeCarrier*, GNEDataInterval*>&
GNENetHelper::AttributeCarriers::getDataIntervals() const {
    return myDataIntervals;
}


void
GNENetHelper::AttributeCarriers::insertDataInterval(const GNEAttributeCarrier* AC, GNEDataInterval* dataInterval) {
    if (myDataIntervals.count(AC) > 0) {
        throw ProcessError(dataInterval->getTagStr() + " with ID='" + dataInterval->getID() + "' already exist");
    } else {
        myDataIntervals[AC] = dataInterval;
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
    // remove it from inspected elements and GNEElementTree
    myNet->getViewNet()->getInspectedElements().uninspectAC(dataInterval);
    dataInterval->unmarkForDrawingFront();
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(dataInterval);
    // mark interval toolbar for update
    myNet->getViewNet()->getIntervalBar().markForUpdate();
}


GNEGenericData*
GNENetHelper::AttributeCarriers::retrieveGenericData(const GUIGlObject* glObject, bool hardFail) const {
    // iterate over all genericDatas
    for (const auto& genericDataTag : myGenericDatas) {
        auto it = genericDataTag.second.find(glObject);
        if (it != genericDataTag.second.end()) {
            return it->second;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant generic (glObject)");
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
            if (genericData.second->isAttributeCarrierSelected()) {
                result.push_back(genericData.second);
            }
        }
    }
    return result;
}


const std::unordered_map<SumoXMLTag, std::unordered_map<const GUIGlObject*, GNEGenericData*>, std::hash<int> >&
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
        if ((genericData.second->getDataIntervalParent()->getAttributeDouble(SUMO_ATTR_BEGIN) >= begin) &&
                (genericData.second->getDataIntervalParent()->getAttributeDouble(SUMO_ATTR_END) <= end)) {
            genericDatas.push_back(genericData.second);
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
        if (genericData.second->isAttributeCarrierSelected()) {
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
        if (genericData.second->isAttributeCarrierSelected()) {
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
        if (genericData.second->isAttributeCarrierSelected()) {
            counter++;
        }
    }
    return counter;
}


void
GNENetHelper::AttributeCarriers::insertGenericData(GNEGenericData* genericData) {
    if (myGenericDatas.at(genericData->getTagProperty()->getTag()).count(genericData->getGUIGlObject()) > 0) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' already exist");
    } else {
        myGenericDatas.at(genericData->getTagProperty()->getTag()).insert(std::make_pair(genericData->getGUIGlObject(), genericData));
    }
    // mark interval toolbar for update
    myNet->getViewNet()->getIntervalBar().markForUpdate();
}


void
GNENetHelper::AttributeCarriers::deleteGenericData(GNEGenericData* genericData) {
    const auto finder = myGenericDatas.at(genericData->getTagProperty()->getTag()).find(genericData);
    if (finder == myGenericDatas.at(genericData->getTagProperty()->getTag()).end()) {
        throw ProcessError(genericData->getTagStr() + " with ID='" + genericData->getID() + "' wasn't previously inserted");
    } else {
        myGenericDatas.at(genericData->getTagProperty()->getTag()).erase(finder);
    }
    // remove it from inspected elements and GNEElementTree
    myNet->getViewNet()->getInspectedElements().uninspectAC(genericData);
    genericData->unmarkForDrawingFront();
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(genericData);
    // delete path element
    myNet->getDataPathManager()->removePath(genericData);
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
        if ((interval.second->getAttributeDouble(SUMO_ATTR_BEGIN) >= begin) && (interval.second->getAttributeDouble(SUMO_ATTR_END) <= end)) {
            // iterate over generic datas
            for (const auto& genericData : interval.second->getGenericDataChildren()) {
                if (genericDataTag.empty() || (genericData->getTagProperty()->getTagStr() == genericDataTag)) {
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
            dataSets.push_back(dataSet.second);
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
            if (genericDataTag.empty() || (genericData->getTagProperty()->getTagStr() == genericDataTag)) {
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
        if (meanData.second->getID() == id) {
            return meanData.second;
        }
    }
    if (hardFail) {
        throw ProcessError("Attempted to retrieve non-existant meanData (string)");
    } else {
        return nullptr;
    }
}


const std::unordered_map<SumoXMLTag, std::map<const std::string, GNEMeanData*>, std::hash<int> >&
GNENetHelper::AttributeCarriers::getMeanDatas() const {
    return myMeanDatas;
}


int
GNENetHelper::AttributeCarriers::getNumberOfMeanDatas() const {
    return myNumberOfMeanDataElements;
}


void
GNENetHelper::AttributeCarriers::clearMeanDatas() {
    // iterate over myMeanDatas and clear all meanDatas
    for (auto& meanDatas : myMeanDatas) {
        meanDatas.second.clear();
    }
}


void
GNENetHelper::AttributeCarriers::updateMeanDataID(GNEMeanData* meanData, const std::string& newID) {
    const auto tag = meanData->getTagProperty()->getTag();
    const auto it = myMeanDatas.at(tag).find(meanData->getID());
    if (it == myMeanDatas.at(tag).end()) {
        throw ProcessError(meanData->getTagStr() + " with ID='" + meanData->getID() + "' doesn't exist in AttributeCarriers.meanDatas");
    } else {
        // remove from container, set new Id, and insert it again
        myMeanDatas.at(tag).erase(it);
        myMeanDatas.at(tag)[newID] = meanData;
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
    myNet->getViewNet()->getInspectedElements().uninspectAC(junction);
    junction->unmarkForDrawingFront();
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(junction);
    // Remove from grid and container
    myNet->removeGLObjectFromGrid(junction);
    myJunctions.erase(junction->getMicrosimID());
    myNumberOfNetworkElements--;
    myNet->getNetBuilder()->getNodeCont().extract(junction->getNBNode());
    junction->decRef("GNENet::deleteSingleJunction");
    junction->setResponsible(true);
}


void
GNENetHelper::AttributeCarriers::insertEdgeType(GNEEdgeType* edgeType) {
    // get pointer to create edge frame
    const auto& createEdgeFrame = myNet->getViewNet()->getViewParent()->getCreateEdgeFrame();
    // insert in myEdgeTypes
    myEdgeTypes[edgeType->getMicrosimID()] = edgeType;
    myNumberOfNetworkElements++;
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
    myNet->getViewNet()->getInspectedElements().uninspectAC(edgeType);
    edgeType->unmarkForDrawingFront();
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(edgeType);
    // remove from edge types
    myEdgeTypes.erase(edgeType->getMicrosimID());
    myNumberOfNetworkElements--;
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
    myNet->getViewNet()->getInspectedElements().uninspectAC(edge);
    edge->unmarkForDrawingFront();
    myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(edge);
    // remove edge from visual grid and container
    myNet->removeGLObjectFromGrid(edge);
    myEdges.erase(edge->getMicrosimID());
    myNumberOfNetworkElements--;
    // remove all lanes
    for (const auto& lane : edge->getChildLanes()) {
        deleteLane(lane);
    }
    // extract edge of district container
    myNet->getNetBuilder()->getEdgeCont().extract(myNet->getNetBuilder()->getDistrictCont(), edge->getNBEdge());
    edge->decRef("GNENet::deleteSingleEdge");
    edge->setResponsible(true);
    // Remove refrences from GNEJunctions
    edge->getFromJunction()->removeOutgoingGNEEdge(edge);
    edge->getToJunction()->removeIncomingGNEEdge(edge);
    // update boundaries of both junctions (to remove it from Grid)
    edge->getFromJunction()->updateCenteringBoundary(true);
    edge->getToJunction()->updateCenteringBoundary(true);
    // get template editor
    GNEInspectorFrame::TemplateEditor* templateEditor = myNet->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // check if we have to remove template
    if (templateEditor->getEdgeTemplate() && (templateEditor->getEdgeTemplate()->getID() == edge->getID())) {
        templateEditor->setEdgeTemplate(nullptr);
    }
}


void
GNENetHelper::AttributeCarriers::insertLane(GNELane* lane) {
    if (myLanes.count(lane->getGUIGlObject()) > 0) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' already exist");
    } else {
        myLanes[lane->getGUIGlObject()] = lane;
        myNumberOfNetworkElements++;
    }
}


void
GNENetHelper::AttributeCarriers::deleteLane(GNELane* lane) {
    const auto finder = myLanes.find(lane->getGUIGlObject());
    if (finder == myLanes.end()) {
        throw ProcessError(lane->getTagStr() + " with ID='" + lane->getID() + "' wasn't previously inserted");
    } else {
        myLanes.erase(finder);
        myNumberOfNetworkElements--;
        // remove it from inspected elements and GNEElementTree
        myNet->getViewNet()->getInspectedElements().uninspectAC(lane);
        lane->unmarkForDrawingFront();
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(lane);
    }
}


void
GNENetHelper::AttributeCarriers::insertCrossing(GNECrossing* crossing) {
    if (myCrossings.count(crossing->getGUIGlObject()) > 0) {
        throw ProcessError(crossing->getTagStr() + " with ID='" + crossing->getID() + "' already exist");
    } else {
        myCrossings[crossing->getGUIGlObject()] = crossing;
        myNumberOfNetworkElements++;
    }
}


void
GNENetHelper::AttributeCarriers::deleteCrossing(GNECrossing* crossing) {
    const auto finder = myCrossings.find(crossing->getGUIGlObject());
    if (finder == myCrossings.end()) {
        throw ProcessError(crossing->getTagStr() + " with ID='" + crossing->getID() + "' wasn't previously inserted");
    } else {
        myCrossings.erase(finder);
        myNumberOfNetworkElements--;
        // remove it from inspected elements and GNEElementTree
        if (myNet->getViewNet()) {
            myNet->getViewNet()->getInspectedElements().uninspectAC(crossing);
            crossing->unmarkForDrawingFront();
            myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(crossing);
        }
    }
}


void
GNENetHelper::AttributeCarriers::insertWalkingArea(GNEWalkingArea* walkingArea) {
    if (myWalkingAreas.count(walkingArea->getGUIGlObject()) > 0) {
        throw ProcessError(walkingArea->getTagStr() + " with ID='" + walkingArea->getID() + "' already exist");
    } else {
        myWalkingAreas[walkingArea->getGUIGlObject()] = walkingArea;
        myNumberOfNetworkElements++;
    }
}


void
GNENetHelper::AttributeCarriers::deleteWalkingArea(GNEWalkingArea* walkingArea) {
    const auto finder = myWalkingAreas.find(walkingArea->getGUIGlObject());
    if (finder == myWalkingAreas.end()) {
        throw ProcessError(walkingArea->getTagStr() + " with ID='" + walkingArea->getID() + "' wasn't previously inserted");
    } else {
        myWalkingAreas.erase(finder);
        myNumberOfNetworkElements--;
        // remove it from inspected elements and GNEElementTree
        myNet->getViewNet()->getInspectedElements().uninspectAC(walkingArea);
        walkingArea->unmarkForDrawingFront();
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(walkingArea);
    }
}


void
GNENetHelper::AttributeCarriers::insertConnection(GNEConnection* connection) {
    if (myConnections.count(connection->getGUIGlObject()) > 0) {
        throw ProcessError(connection->getTagStr() + " with ID='" + connection->getID() + "' already exist");
    } else {
        myConnections[connection->getGUIGlObject()] = connection;
        myNumberOfNetworkElements++;
    }
}


void
GNENetHelper::AttributeCarriers::deleteConnection(GNEConnection* connection) {
    const auto finder = myConnections.find(connection->getGUIGlObject());
    if (finder == myConnections.end()) {
        throw ProcessError(connection->getTagStr() + " with ID='" + connection->getID() + "' wasn't previously inserted");
    } else {
        myConnections.erase(finder);
        myNumberOfNetworkElements--;
        // remove it from inspected elements and GNEElementTree
        myNet->getViewNet()->getInspectedElements().uninspectAC(connection);
        connection->unmarkForDrawingFront();
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(connection);
    }
}


void
GNENetHelper::AttributeCarriers::insertInternalLane(GNEInternalLane* internalLane) {
    if (myInternalLanes.count(internalLane->getGUIGlObject()) > 0) {
        throw ProcessError(internalLane->getTagStr() + " with ID='" + internalLane->getID() + "' already exist");
    } else {
        myInternalLanes[internalLane->getGUIGlObject()] = internalLane;
        myNumberOfNetworkElements++;
    }
}


void
GNENetHelper::AttributeCarriers::deleteInternalLane(GNEInternalLane* internalLane) {
    const auto finder = myInternalLanes.find(internalLane->getGUIGlObject());
    if (finder == myInternalLanes.end()) {
        throw ProcessError(internalLane->getTagStr() + " with ID='" + internalLane->getID() + "' wasn't previously inserted");
    } else {
        myInternalLanes.erase(finder);
        myNumberOfNetworkElements--;
    }
}


void
GNENetHelper::AttributeCarriers::insertAdditional(GNEAdditional* additional) {
    const auto tag = additional->getTagProperty()->getTag();
    if (myAdditionals.at(tag).count(additional) > 0) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' already exist");
    } else {
        // insert in both containers
        myAdditionals.at(tag)[additional->getGUIGlObject()] = additional;
        if (additional->getTagProperty()->hasAttribute(SUMO_ATTR_ID)) {
            myAdditionalIDs.at(tag)[additional->getID()] = additional;
        }
        myNumberOfNetworkElements++;
        // add element in grid
        if (additional->getTagProperty()->isPlacedInRTree()) {
            myNet->addGLObjectIntoGrid(additional);
        }
        // update geometry after insertion of additionals if myUpdateGeometryEnabled is enabled
        if (myNet->isUpdateGeometryEnabled()) {
            additional->updateGeometry();
        }
        // additionals has to be saved
        myNet->getSavingStatus()->requireSaveAdditionals();
    }
}


void
GNENetHelper::AttributeCarriers::deleteAdditional(GNEAdditional* additional) {
    const auto tag = additional->getTagProperty()->getTag();
    // find demanElement in additionalTag
    auto itFind = myAdditionals.at(tag).find(additional->getGUIGlObject());
    // check if additional was previously inserted
    if (itFind == myAdditionals.at(tag).end()) {
        throw ProcessError(additional->getTagStr() + " with ID='" + additional->getID() + "' wasn't previously inserted");
    } else {
        // remove from both container
        myAdditionals.at(tag).erase(itFind);
        if (additional->getTagProperty()->hasAttribute(SUMO_ATTR_ID)) {
            myAdditionalIDs.at(tag).erase(myAdditionalIDs.at(tag).find(additional->getID()));
        }
        myNumberOfNetworkElements--;
        // remove it from inspected elements and GNEElementTree
        myNet->getViewNet()->getInspectedElements().uninspectAC(additional);
        additional->unmarkForDrawingFront();
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(additional);
        // remove element from grid
        if (additional->getTagProperty()->isPlacedInRTree()) {
            myNet->removeGLObjectFromGrid(additional);
        }
        // delete path element
        myNet->getNetworkPathManager()->removePath(additional);
        // additionals has to be saved
        myNet->getSavingStatus()->requireSaveAdditionals();
    }
}




void
GNENetHelper::AttributeCarriers::insertTAZSourceSink(GNETAZSourceSink* sourceSink) {
    const auto sourceSinkTag = sourceSink->getTagProperty()->getTag();
    if (myTAZSourceSinks.at(sourceSinkTag).count(sourceSink) > 0) {
        throw ProcessError(sourceSink->getTagStr() + " with ID='" + sourceSink->getID() + "' already exist");
    } else {
        myTAZSourceSinks.at(sourceSinkTag)[sourceSink] = sourceSink;
        myNumberOfNetworkElements++;
        // additionals has to be saved
        myNet->getSavingStatus()->requireSaveAdditionals();
    }
}


void
GNENetHelper::AttributeCarriers::deleteTAZSourceSink(GNETAZSourceSink* sourceSink) {
    const auto tag = sourceSink->getTagProperty()->getTag();
    // find demanElement in additionalTag
    auto itFind = myTAZSourceSinks.at(tag).find(sourceSink);
    // check if sourceSink was previously inserted
    if (itFind == myTAZSourceSinks.at(tag).end()) {
        throw ProcessError(sourceSink->getTagStr() + " with ID='" + sourceSink->getID() + "' wasn't previously inserted");
    } else {
        // remove from both container
        myTAZSourceSinks.at(tag).erase(itFind);
        myNumberOfNetworkElements--;
        // remove it from inspected elements and GNEElementTree
        myNet->getViewNet()->getInspectedElements().uninspectAC(sourceSink);
        sourceSink->unmarkForDrawingFront();
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(sourceSink);
        // additionals has to be saved
        myNet->getSavingStatus()->requireSaveAdditionals();
    }
}


void
GNENetHelper::AttributeCarriers::insertDemandElement(GNEDemandElement* demandElement) {
    const auto tag = demandElement->getTagProperty()->getTag();
    if (myDemandElements.at(tag).count(demandElement) > 0) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' already exist");
    } else {
        myDemandElements.at(tag)[demandElement->getGUIGlObject()] = demandElement;
        myNumberOfDemandElements++;
        if (demandElement->getTagProperty()->hasAttribute(SUMO_ATTR_ID)) {
            myDemandElementIDs.at(tag)[demandElement->getID()] = demandElement;
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
}


void
GNENetHelper::AttributeCarriers::deleteDemandElement(GNEDemandElement* demandElement, const bool updateFrames) {
    const auto tag = demandElement->getTagProperty()->getTag();
    auto viewParent = myNet->getViewNet()->getViewParent();
    // find demanElement in demandElementTag
    auto itFind = myDemandElements.at(tag).find(demandElement->getGUIGlObject());
    // check if demandElement was previously inserted
    if (itFind == myDemandElements.at(tag).end()) {
        throw ProcessError(demandElement->getTagStr() + " with ID='" + demandElement->getID() + "' wasn't previously inserted");
    } else {
        // erase it from container
        myDemandElements.at(tag).erase(itFind);
        if (demandElement->getTagProperty()->hasAttribute(SUMO_ATTR_ID)) {
            myDemandElementIDs.at(tag).erase(myDemandElementIDs.at(tag).find(demandElement->getID()));
        }
        myNumberOfDemandElements--;
        // remove element from grid
        myNet->removeGLObjectFromGrid(demandElement);
        // remove it from inspected elements and GNEElementTree
        myNet->getViewNet()->getInspectedElements().uninspectAC(demandElement);
        demandElement->unmarkForDrawingFront();
        viewParent->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(demandElement);
        viewParent->getPersonPlanFrame()->getPersonHierarchy()->removeCurrentEditedAttributeCarrier(demandElement);
        viewParent->getContainerPlanFrame()->getContainerHierarchy()->removeCurrentEditedAttributeCarrier(demandElement);
        if (viewParent->getRouteDistributionFrame()->getDistributionSelector()->getCurrentDistribution() == demandElement) {
            viewParent->getRouteDistributionFrame()->getDistributionSelector()->setDistribution(nullptr);
        }
        if (viewParent->getTypeDistributionFrame()->getDistributionSelector()->getCurrentDistribution() == demandElement) {
            viewParent->getTypeDistributionFrame()->getDistributionSelector()->setDistribution(nullptr);
        }
        // if is the last inserted route, remove it from GNEViewNet
        if (myNet->getViewNet()->getLastCreatedRoute() == demandElement) {
            myNet->getViewNet()->setLastCreatedRoute(nullptr);
        }
        // delete path element
        myNet->getDemandPathManager()->removePath(demandElement);
        // check if update demand elements frames
        if (updateFrames) {
            updateDemandElementFrames(demandElement->getTagProperty());
        }
        // demandElements has to be saved
        myNet->getSavingStatus()->requireSaveDemandElements();
    }
}


void
GNENetHelper::AttributeCarriers::insertDataSet(GNEDataSet* dataSet) {
    if (myDataSets.count(dataSet->getID()) > 0) {
        throw ProcessError(dataSet->getTagStr() + " with ID='" + dataSet->getID() + "' already exist");
    } else {
        myDataSets[dataSet->getID()] = dataSet;
        myNumberOfDataElements++;
        // dataSets has to be saved
        myNet->getSavingStatus()->requireSaveDataElements();
        // mark interval toolbar for update
        myNet->getViewNet()->getIntervalBar().markForUpdate();
    }
}


void
GNENetHelper::AttributeCarriers::deleteDataSet(GNEDataSet* dataSet) {
    const auto finder = myDataSets.find(dataSet->getID());
    if (finder == myDataSets.end()) {
        throw ProcessError(dataSet->getTagStr() + " with ID='" + dataSet->getID() + "' wasn't previously inserted");
    } else {
        myDataSets.erase(finder);
        myNumberOfDataElements--;
        // remove it from inspected elements and GNEElementTree
        myNet->getViewNet()->getInspectedElements().uninspectAC(dataSet);
        dataSet->unmarkForDrawingFront();
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(dataSet);
        // dataSets has to be saved
        myNet->getSavingStatus()->requireSaveDataElements();
        // mark interval toolbar for update
        myNet->getViewNet()->getIntervalBar().markForUpdate();
    }
}


void
GNENetHelper::AttributeCarriers::insertMeanData(GNEMeanData* meanData) {
    if (myMeanDatas.at(meanData->getTagProperty()->getTag()).count(meanData->getID()) > 0) {
        throw ProcessError(meanData->getTagStr() + " with ID='" + meanData->getID() + "' already exist");
    } else {
        myMeanDatas.at(meanData->getTagProperty()->getTag()).insert(std::make_pair(meanData->getID(), meanData));
        myNumberOfMeanDataElements++;
        // meanDatas has to be saved
        myNet->getSavingStatus()->requireSaveMeanDatas();
    }
}


void
GNENetHelper::AttributeCarriers::deleteMeanData(GNEMeanData* meanData) {
    // find demanElement in meanDataTag
    auto itFind = myMeanDatas.at(meanData->getTagProperty()->getTag()).find(meanData->getID());
    // check if meanData was previously inserted
    if (itFind == myMeanDatas.at(meanData->getTagProperty()->getTag()).end()) {
        throw ProcessError(meanData->getTagStr() + " with ID='" + meanData->getID() + "' wasn't previously inserted");
    } else {
        // remove from container
        myMeanDatas.at(meanData->getTagProperty()->getTag()).erase(itFind);
        myNumberOfMeanDataElements--;
        // remove it from inspected elements and GNEElementTree
        myNet->getViewNet()->getInspectedElements().uninspectAC(meanData);
        meanData->unmarkForDrawingFront();
        myNet->getViewNet()->getViewParent()->getInspectorFrame()->getHierarchicalElementTree()->removeCurrentEditedAttributeCarrier(meanData);
        // remove element from grid
        if (meanData->getTagProperty()->isPlacedInRTree()) {
            myNet->removeGLObjectFromGrid(meanData);
        }
        // meanDatas has to be saved
        myNet->getSavingStatus()->requireSaveMeanDatas();
    }
}


void
GNENetHelper::AttributeCarriers::updateDemandElementFrames(const GNETagProperties* tagProperty) {
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        // continue depending of demand mode
        switch (myNet->getViewNet()->getEditModes().demandEditMode) {
            case DemandEditMode::DEMAND_VEHICLE:
                if (tagProperty->isType()) {
                    myNet->getViewNet()->getViewParent()->getVehicleFrame()->getTypeSelector()->refreshDemandElementSelector();
                }
                break;
            case DemandEditMode::DEMAND_TYPE:
                if (tagProperty->isType()) {
                    myNet->getViewNet()->getViewParent()->getTypeFrame()->getTypeSelector()->refreshTypeSelector(true);
                }
                break;
            case DemandEditMode::DEMAND_TYPEDISTRIBUTION:
                if (tagProperty->isType()) {
                    myNet->getViewNet()->getViewParent()->getTypeDistributionFrame()->getDistributionSelector()->refreshDistributionSelector();
                }
                break;
            case DemandEditMode::DEMAND_ROUTEDISTRIBUTION:
                if (tagProperty->isRoute()) {
                    myNet->getViewNet()->getViewParent()->getRouteDistributionFrame()->getDistributionSelector()->refreshDistributionSelector();
                }
                break;
            case DemandEditMode::DEMAND_PERSON:
                if (tagProperty->isType()) {
                    myNet->getViewNet()->getViewParent()->getPersonFrame()->getTypeSelector()->refreshDemandElementSelector();
                }
                break;
            case DemandEditMode::DEMAND_PERSONPLAN:
                if (tagProperty->isPerson()) {
                    myNet->getViewNet()->getViewParent()->getPersonPlanFrame()->getPersonSelector()->refreshDemandElementSelector();
                }
                break;
            case DemandEditMode::DEMAND_CONTAINER:
                if (tagProperty->isType()) {
                    myNet->getViewNet()->getViewParent()->getContainerFrame()->getTypeSelector()->refreshDemandElementSelector();
                }
                break;
            case DemandEditMode::DEMAND_CONTAINERPLAN:
                if (tagProperty->isContainer()) {
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


void
GNENetHelper::AttributeCarriers::retrieveAttributeCarriersRecursively(const GNETagProperties* tag, std::vector<GNEAttributeCarrier*>& ACs) {
    // fill network elements
    if (tag->getTag() == SUMO_TAG_JUNCTION) {
        for (const auto& junction : myJunctions) {
            ACs.push_back(junction.second);
        }
    }
    if (tag->getTag() == SUMO_TAG_EDGE) {
        for (const auto& edge : myEdges) {
            ACs.push_back(edge.second);
        }
    }
    if (tag->getTag() == SUMO_TAG_LANE) {
        for (const auto& lane : myLanes) {
            ACs.push_back(lane.second);
        }
    }
    if (tag->getTag() == SUMO_TAG_CONNECTION) {
        for (const auto& connection : myConnections) {
            ACs.push_back(connection.second);
        }
    }
    if (tag->getTag() == SUMO_TAG_CROSSING) {
        for (const auto& crossing : myCrossings) {
            ACs.push_back(crossing.second);
        }
    }
    if (tag->getTag() == SUMO_TAG_WALKINGAREA) {
        for (const auto& walkingArea : myWalkingAreas) {
            ACs.push_back(walkingArea.second);
        }
    }
    // fill additional elements
    if (tag->isAdditionalElement()) {
        for (const auto& additionalTag : myAdditionals) {
            if (additionalTag.first == tag->getTag()) {
                for (const auto& additional : additionalTag.second) {
                    ACs.push_back(additional.second);
                }
            }
        }
    }
    // fill demand elements
    if (tag->isDemandElement()) {
        for (const auto& demandElementTag : myDemandElements) {
            if (demandElementTag.first == tag->getTag()) {
                for (const auto& demandElemet : demandElementTag.second) {
                    ACs.push_back(demandElemet.second);
                }
            }
        }
    }
    // fill data elements
    if (tag->isDataElement()) {
        if (tag->getTag() == SUMO_TAG_DATASET) {
            for (const auto& dataSet : myDataSets) {
                ACs.push_back(dataSet.second);
            }
        }
        if (tag->getTag() == SUMO_TAG_DATAINTERVAL) {
            for (const auto& dataInterval : myDataIntervals) {
                ACs.push_back(dataInterval.second);
            }
        }
        for (const auto& genericDataTag : myGenericDatas) {
            if (genericDataTag.first == tag->getTag()) {
                for (const auto& genericData : genericDataTag.second) {
                    ACs.push_back(genericData.second);
                }
            }
        }
        for (const auto& meanDataTag : myMeanDatas) {
            if (meanDataTag.first == tag->getTag()) {
                for (const auto& meanData : meanDataTag.second) {
                    ACs.push_back(meanData.second);
                }
            }
        }
    }
    // iterate over children
    for (const auto child : tag->getTagChildren()) {
        retrieveAttributeCarriersRecursively(child, ACs);
    }
}

// ---------------------------------------------------------------------------
// GNENetHelper::GNETagSelector - methods
// ---------------------------------------------------------------------------

GNENetHelper::ACTemplate::ACTemplate(GNENet* net) :
    myNet(net) {
}


void
GNENetHelper::ACTemplate::buildTemplates() {
    // network
    myTemplates[SUMO_TAG_CROSSING] = new GNECrossing(myNet);
    // additionals
    myTemplates[SUMO_TAG_BUS_STOP] = GNEBusStop::buildBusStop(myNet);
    myTemplates[SUMO_TAG_TRAIN_STOP] = GNEBusStop::buildTrainStop(myNet);
    myTemplates[SUMO_TAG_ACCESS] = new GNEAccess(myNet);
    myTemplates[SUMO_TAG_CONTAINER_STOP] = new GNEContainerStop(myNet);
    myTemplates[SUMO_TAG_CHARGING_STATION] = new GNEChargingStation(myNet);
    myTemplates[SUMO_TAG_PARKING_AREA] = new GNEParkingArea(myNet);
    myTemplates[SUMO_TAG_PARKING_SPACE] = new GNEParkingSpace(myNet);
    myTemplates[SUMO_TAG_INDUCTION_LOOP] = new GNEInductionLoopDetector(myNet);
    myTemplates[SUMO_TAG_LANE_AREA_DETECTOR] = new GNELaneAreaDetector(SUMO_TAG_LANE_AREA_DETECTOR, myNet);
    myTemplates[GNE_TAG_MULTI_LANE_AREA_DETECTOR] = new GNELaneAreaDetector(GNE_TAG_MULTI_LANE_AREA_DETECTOR, myNet);
    myTemplates[SUMO_TAG_ENTRY_EXIT_DETECTOR] = new GNEMultiEntryExitDetector(myNet);
    myTemplates[SUMO_TAG_DET_ENTRY] = new GNEEntryExitDetector(SUMO_TAG_DET_ENTRY, myNet);
    myTemplates[SUMO_TAG_DET_EXIT] = new GNEEntryExitDetector(SUMO_TAG_DET_EXIT, myNet);
    myTemplates[SUMO_TAG_INSTANT_INDUCTION_LOOP] = new GNEInstantInductionLoopDetector(myNet);
    myTemplates[SUMO_TAG_VSS] = new GNEVariableSpeedSign(myNet);
    myTemplates[SUMO_TAG_STEP] = new GNEVariableSpeedSignStep(myNet);
    myTemplates[SUMO_TAG_CALIBRATOR] = new GNECalibrator(SUMO_TAG_CALIBRATOR, myNet);
    myTemplates[GNE_TAG_CALIBRATOR_LANE] = new GNECalibrator(GNE_TAG_CALIBRATOR_LANE, myNet);
    myTemplates[GNE_TAG_CALIBRATOR_FLOW] = new GNECalibratorFlow(myNet);
    myTemplates[SUMO_TAG_REROUTER] = new GNERerouter(myNet);
    myTemplates[SUMO_TAG_INTERVAL] = new GNERerouterInterval(myNet);
    myTemplates[SUMO_TAG_CLOSING_REROUTE] = new GNEClosingReroute(myNet);
    myTemplates[SUMO_TAG_CLOSING_LANE_REROUTE] = new GNEClosingLaneReroute(myNet);
    myTemplates[SUMO_TAG_DEST_PROB_REROUTE] = new GNEDestProbReroute(myNet);
    myTemplates[SUMO_TAG_PARKING_AREA_REROUTE] = new GNEParkingAreaReroute(myNet);
    myTemplates[SUMO_TAG_ROUTE_PROB_REROUTE] = new GNERouteProbReroute(myNet);
    myTemplates[SUMO_TAG_ROUTEPROBE] = new GNERouteProbe(myNet);
    myTemplates[SUMO_TAG_VAPORIZER] = new GNEVaporizer(myNet);
    // symbols
    myTemplates[GNE_TAG_REROUTER_SYMBOL] = new GNERerouterSymbol(myNet);
    myTemplates[GNE_TAG_VSS_SYMBOL] = new GNEVariableSpeedSignSymbol(myNet);
    // shapes
    myTemplates[SUMO_TAG_POLY] = new GNEPoly(SUMO_TAG_POLY, myNet);
    myTemplates[SUMO_TAG_POI] = new GNEPOI(SUMO_TAG_POI, myNet);
    myTemplates[GNE_TAG_POILANE] = new GNEPOI(GNE_TAG_POILANE, myNet);
    myTemplates[GNE_TAG_POIGEO] = new GNEPOI(GNE_TAG_POIGEO, myNet);
    // TAZs
    myTemplates[SUMO_TAG_TAZ] = new GNETAZ(myNet);
    myTemplates[SUMO_TAG_TAZSOURCE] = new GNETAZSourceSink(SUMO_TAG_TAZSOURCE, myNet);
    myTemplates[SUMO_TAG_TAZSINK] = new GNETAZSourceSink(SUMO_TAG_TAZSINK, myNet);
    // wires
    myTemplates[SUMO_TAG_TRACTION_SUBSTATION] = new GNETractionSubstation(myNet);
    myTemplates[SUMO_TAG_OVERHEAD_WIRE_SECTION] = new GNEOverheadWire(myNet);
    //myTemplates[SUMO_TAG_OVERHEAD_WIRE_CLAMP] = nullptr;   // TMP
    // JuPedSim elements
    myTemplates[GNE_TAG_JPS_WALKABLEAREA] = new GNEPoly(GNE_TAG_JPS_WALKABLEAREA, myNet);
    myTemplates[GNE_TAG_JPS_OBSTACLE] = new GNEPoly(GNE_TAG_JPS_OBSTACLE, myNet);
    // vTypes
    myTemplates[SUMO_TAG_VTYPE] = new GNEVType(myNet);
    myTemplates[SUMO_TAG_VTYPE_DISTRIBUTION] = new GNEVTypeDistribution(myNet);
    // routes
    const auto routes = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::ROUTE);
    for (const auto route : routes) {
        myTemplates[route->getTag()] = new GNERoute(route->getTag(), myNet);
    }
    // vehicles
    const auto vehicles = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::VEHICLE);
    for (const auto vehicle : vehicles) {
        myTemplates[vehicle->getTag()] = new GNEVehicle(vehicle->getTag(), myNet);
    }
    // persons
    const auto persons = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::PERSON);
    for (const auto person : persons) {
        myTemplates[person->getTag()] = new GNEPerson(person->getTag(), myNet);
    }
    // container
    const auto containers = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::CONTAINER);
    for (const auto container : containers) {
        myTemplates[container->getTag()] = new GNEContainer(container->getTag(), myNet);
    }
    // stops and waypoints
    const auto stopAndWaypoints = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::STOP_VEHICLE);
    for (const auto stopAndWaypoint : stopAndWaypoints) {
        myTemplates[stopAndWaypoint->getTag()] = new GNEStop(stopAndWaypoint->getTag(), myNet);
    }
    // personTrip
    const auto personTrips = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::PERSONTRIP);
    for (const auto personTrip : personTrips) {
        myTemplates[personTrip->getTag()] = new GNEPersonTrip(personTrip->getTag(), myNet);
    }
    // walk
    const auto walks = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::WALK);
    for (const auto walk : walks) {
        myTemplates[walk->getTag()] = new GNEWalk(walk->getTag(), myNet);
    }
    // ride
    const auto rides = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::RIDE);
    for (const auto ride : rides) {
        myTemplates[ride->getTag()] = new GNERide(ride->getTag(), myNet);
    }
    // stop person
    const auto stopPersons = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::STOP_PERSON);
    for (const auto stopPerson : stopPersons) {
        myTemplates[stopPerson->getTag()] = new GNEStopPlan(stopPerson->getTag(), myNet);
    }
    // transport
    const auto transports = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::TRANSPORT);
    for (const auto transport : transports) {
        myTemplates[transport->getTag()] = new GNETransport(transport->getTag(), myNet);
    }
    // tranship
    const auto tranships = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::TRANSHIP);
    for (const auto tranship : tranships) {
        myTemplates[tranship->getTag()] = new GNETranship(tranship->getTag(), myNet);
    }
    // stop container
    const auto stopContainers = myNet->getTagPropertiesDatabase()->getTagPropertiesByType(GNETagProperties::TagType::STOP_CONTAINER);
    for (const auto stopContainer : stopContainers) {
        myTemplates[stopContainer->getTag()] = new GNEStopPlan(stopContainer->getTag(), myNet);
    }
    // reset all to their default values
    for (const auto& AC : myTemplates) {
        AC.second->resetDefaultValues(false);
    }
}


GNENetHelper::ACTemplate::~ACTemplate() {
    for (auto& AC : myTemplates) {
        delete AC.second;
    }
}


std::map<SumoXMLTag, GNEAttributeCarrier*>
GNENetHelper::ACTemplate::getACTemplates() const {
    return myTemplates;
}


GNEAttributeCarrier*
GNENetHelper::ACTemplate::getTemplateAC(const SumoXMLTag tag) const {
    if (myTemplates.count(tag) > 0) {
        return myTemplates.at(tag);
    } else {
        return nullptr;
    }
}


GNEAttributeCarrier*
GNENetHelper::ACTemplate::getTemplateAC(const std::string& selectorText) const {
    for (const auto& templateAC : myTemplates) {
        if (templateAC.second->getTagProperty()->getSelectorText() == selectorText) {
            return templateAC.second;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// GNENetHelper::SavingFilesHandler - methods
// ---------------------------------------------------------------------------

GNENetHelper::SavingFilesHandler::SavingFilesHandler(GNENet* net) :
    myNet(net) {
}


void
GNENetHelper::SavingFilesHandler::updateNeteditConfig() {
    auto& neteditOptions = OptionsCont::getOptions();
    // get files
    const auto additionalFiles = parsingSavingFiles(myAdditionalElementsSavingFiles);
    const auto demandElementFiles = parsingSavingFiles(myDemandElementsSavingFiles);
    const auto dataElementFiles = parsingSavingFiles(myDataElementsSavingFiles);
    const auto meanDataElementFiles = parsingSavingFiles(myMeanDataElementsSavingFiles);
    // additionals
    neteditOptions.resetWritable();
    if (additionalFiles.size() > 0) {
        neteditOptions.set("additional-files", additionalFiles);
    } else {
        neteditOptions.resetDefault("additional-files");
    }
    // route files
    neteditOptions.resetWritable();
    if (demandElementFiles.size() > 0) {
        neteditOptions.set("route-files", demandElementFiles);
    } else {
        neteditOptions.resetDefault("route-files");
    }
    // data files
    neteditOptions.resetWritable();
    if (dataElementFiles.size() > 0) {
        neteditOptions.set("data-files", dataElementFiles);
    } else {
        neteditOptions.resetDefault("data-files");
    }
    // meanData files
    neteditOptions.resetWritable();
    if (meanDataElementFiles.size() > 0) {
        neteditOptions.set("meandata-files", meanDataElementFiles);
    } else {
        neteditOptions.resetDefault("meandata-files");
    }
}


void
GNENetHelper::SavingFilesHandler::addAdditionalFilename(const GNEAttributeCarrier* additionalElement) {
    if ((additionalElement->getFilename().size() > 0) && !existAdditionalFilename(additionalElement->getFilename())) {
        if (myAdditionalElementsSavingFiles.empty()) {
            updateAdditionalEmptyFilenames(additionalElement->getFilename());
        } else {
            myAdditionalElementsSavingFiles.push_back(additionalElement->getFilename());
        }
    }
}


void
GNENetHelper::SavingFilesHandler::updateAdditionalEmptyFilenames(const std::string& file) {
    for (const auto& additionalTag : myNet->getAttributeCarriers()->getAdditionals()) {
        for (const auto& additional : additionalTag.second) {
            additional.second->changeDefaultFilename(file);
        }
    }
    // update all templates
    for (auto& templateAC : myNet->getACTemplates()->getACTemplates()) {
        if (templateAC.second->getTagProperty()->isAdditionalElement() && templateAC.second->getFilename().empty()) {
            templateAC.second->changeDefaultFilename(file);
        }
    }
    // add it to current files
    if (!existAdditionalFilename(file)) {
        myAdditionalElementsSavingFiles.push_back(file);
    }
}


const std::vector<std::string>&
GNENetHelper::SavingFilesHandler::getAdditionalFilenames() const {
    return myAdditionalElementsSavingFiles;
}


GNENetHelper::SavingFilesHandler::ACsbyFilename
GNENetHelper::SavingFilesHandler::getAdditionalsByFilename() {
    ACsbyFilename additionalsbyFilenames;
    for (const auto& additionalTag : myNet->getAttributeCarriers()->getAdditionals()) {
        for (const auto& additional : additionalTag.second) {
            additionalsbyFilenames[additional.second->getFilename()].insert(additional.second);
        }
    }
    // special case for routes (due calibrators)
    for (const auto& route : myNet->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
        if (std::find(myAdditionalElementsSavingFiles.begin(), myAdditionalElementsSavingFiles.end(), route.second->getFilename()) != myAdditionalElementsSavingFiles.end()) {
            additionalsbyFilenames[route.second->getFilename()].insert(route.second);
        }
    }
    // clear empty saving files
    auto it = myAdditionalElementsSavingFiles.begin();
    while (it != myAdditionalElementsSavingFiles.end()) {
        if (it->empty() || (additionalsbyFilenames.find(*it) == additionalsbyFilenames.end())) {
            it = myAdditionalElementsSavingFiles.erase(it);
        } else {
            it++;
        }
    }
    return additionalsbyFilenames;
}


bool
GNENetHelper::SavingFilesHandler::existAdditionalFilename(const std::string& file) const {
    const auto it = std::find(myAdditionalElementsSavingFiles.begin(), myAdditionalElementsSavingFiles.end(), file);
    return it != myAdditionalElementsSavingFiles.end();
}


void
GNENetHelper::SavingFilesHandler::addDemandFilename(const GNEAttributeCarrier* demandElement) {
    if ((demandElement->getFilename().size() > 0) && !existDemandFilename(demandElement->getFilename())) {
        if (myDemandElementsSavingFiles.empty()) {
            updateDemandEmptyFilenames(demandElement->getFilename());
        } else {
            myDemandElementsSavingFiles.push_back(demandElement->getFilename());
        }
    }
}


void
GNENetHelper::SavingFilesHandler::updateDemandEmptyFilenames(const std::string& file) {
    for (const auto& demandTag : myNet->getAttributeCarriers()->getDemandElements()) {
        for (const auto& demand : demandTag.second) {
            demand.second->changeDefaultFilename(file);
        }
    }
    // update all templates
    for (auto& templateAC : myNet->getACTemplates()->getACTemplates()) {
        if (templateAC.second->getTagProperty()->isDemandElement() && templateAC.second->getFilename().empty()) {
            templateAC.second->changeDefaultFilename(file);
        }
    }
    // add it to current files
    if (!existDemandFilename(file)) {
        myDemandElementsSavingFiles.push_back(file);
    }
}


const std::vector<std::string>&
GNENetHelper::SavingFilesHandler::getDemandFilenames() const {
    return myDemandElementsSavingFiles;
}


GNENetHelper::SavingFilesHandler::ACsbyFilename
GNENetHelper::SavingFilesHandler::getDemandsByFilename() {
    ACsbyFilename demandsbyFilenames;
    for (const auto& demandTag : myNet->getAttributeCarriers()->getDemandElements()) {
        for (const auto& demand : demandTag.second) {
            if (std::find(myAdditionalElementsSavingFiles.begin(), myAdditionalElementsSavingFiles.end(), demand.second->getFilename()) == myAdditionalElementsSavingFiles.end()) {
                demandsbyFilenames[demand.second->getFilename()].insert(demand.second);
            }
        }
    }
    // clear empty saving files
    auto it = myDemandElementsSavingFiles.begin();
    while (it != myDemandElementsSavingFiles.end()) {
        if (it->empty() || (demandsbyFilenames.find(*it) == demandsbyFilenames.end())) {
            it = myDemandElementsSavingFiles.erase(it);
        } else {
            it++;
        }
    }
    return demandsbyFilenames;
}


bool
GNENetHelper::SavingFilesHandler::existDemandFilename(const std::string& file) const {
    const auto it = std::find(myDemandElementsSavingFiles.begin(), myDemandElementsSavingFiles.end(), file);
    return it != myDemandElementsSavingFiles.end();
}


void
GNENetHelper::SavingFilesHandler::addDataFilename(const GNEAttributeCarrier* dataElement) {
    if ((dataElement->getFilename().size() > 0) && !existDataFilename(dataElement->getFilename())) {
        if (myDataElementsSavingFiles.empty()) {
            updateDataEmptyFilenames(dataElement->getFilename());
        } else {
            myDataElementsSavingFiles.push_back(dataElement->getFilename());
        }
    }
}


void
GNENetHelper::SavingFilesHandler::updateDataEmptyFilenames(const std::string& file) {
    if (file.size() > 0) {
        for (const auto& dataSet : myNet->getAttributeCarriers()->getDataSets()) {
            dataSet.second->changeDefaultFilename(file);
        }
        // add it to current files
        if (!existDataFilename(file)) {
            myDataElementsSavingFiles.push_back(file);
        }
    }
}


const std::vector<std::string>&
GNENetHelper::SavingFilesHandler::getDataFilenames() const {
    return myDataElementsSavingFiles;
}


GNENetHelper::SavingFilesHandler::ACsbyFilename
GNENetHelper::SavingFilesHandler::getDatasByFilename() {
    ACsbyFilename datasbyFilenames;
    for (const auto& dataSet : myNet->getAttributeCarriers()->getDataSets()) {
        datasbyFilenames[dataSet.second->getFilename()].insert(dataSet.second);
    }
    // clear empty saving files
    auto it = myDataElementsSavingFiles.begin();
    while (it != myDataElementsSavingFiles.end()) {
        if (it->empty() || (datasbyFilenames.find(*it) == datasbyFilenames.end())) {
            it = myDataElementsSavingFiles.erase(it);
        } else {
            it++;
        }
    }
    return datasbyFilenames;
}


bool
GNENetHelper::SavingFilesHandler::existDataFilename(const std::string& file) const {
    const auto it = std::find(myDataElementsSavingFiles.begin(), myDataElementsSavingFiles.end(), file);
    return it != myDataElementsSavingFiles.end();
}


void
GNENetHelper::SavingFilesHandler::addMeanDataFilename(const GNEAttributeCarrier* meanDataElement) {
    if ((meanDataElement->getFilename().size() > 0) && !existMeanDataFilename(meanDataElement->getFilename())) {
        if (myMeanDataElementsSavingFiles.empty()) {
            updateMeanDataEmptyFilenames(meanDataElement->getFilename());
        } else {
            myMeanDataElementsSavingFiles.push_back(meanDataElement->getFilename());
        }
    }
}


void
GNENetHelper::SavingFilesHandler::updateMeanDataEmptyFilenames(const std::string& file) {
    for (const auto& meanDataTag : myNet->getAttributeCarriers()->getMeanDatas()) {
        for (const auto& meanData : meanDataTag.second) {
            meanData.second->changeDefaultFilename(file);
        }
    }
    // update all templates
    for (auto& templateAC : myNet->getACTemplates()->getACTemplates()) {
        if (templateAC.second->getTagProperty()->isMeanData() && templateAC.second->getFilename().empty()) {
            templateAC.second->changeDefaultFilename(file);
        }
    }
    // add it to current files
    if (!existMeanDataFilename(file)) {
        myMeanDataElementsSavingFiles.push_back(file);
    }
}


const std::vector<std::string>&
GNENetHelper::SavingFilesHandler::getMeanDataFilenames() const {
    return myMeanDataElementsSavingFiles;
}


GNENetHelper::SavingFilesHandler::ACsbyFilename
GNENetHelper::SavingFilesHandler::getMeanDatasByFilename() {
    ACsbyFilename meanDatasbyFilenames;
    for (const auto& meanDataTag : myNet->getAttributeCarriers()->getMeanDatas()) {
        for (const auto& meanData : meanDataTag.second) {
            meanDatasbyFilenames[meanData.second->getFilename()].insert(meanData.second);
        }
    }
    // clear empty saving files
    auto it = myMeanDataElementsSavingFiles.begin();
    while (it != myMeanDataElementsSavingFiles.end()) {
        if (it->empty() || (meanDatasbyFilenames.find(*it) == meanDatasbyFilenames.end())) {
            it = myMeanDataElementsSavingFiles.erase(it);
        } else {
            it++;
        }
    }
    return meanDatasbyFilenames;
}


bool
GNENetHelper::SavingFilesHandler::existMeanDataFilename(const std::string& file) const {
    const auto it = std::find(myMeanDataElementsSavingFiles.begin(), myMeanDataElementsSavingFiles.end(), file);
    return it != myMeanDataElementsSavingFiles.end();
}


std::string
GNENetHelper::SavingFilesHandler::parsingSavingFiles(const std::vector<std::string>& savingFiles) const {
    std::string savingFileNames;
    // group all saving files in a single string separated with comma
    for (const auto& savingFile : savingFiles) {
        savingFileNames.append(savingFile + ",");
    }
    // remove last ','
    if (savingFileNames.size() > 0) {
        savingFileNames.pop_back();
    }
    return savingFileNames;
}

// ---------------------------------------------------------------------------
// GNENetHelper::SavingStatus - methods
// ---------------------------------------------------------------------------

GNENetHelper::SavingStatus::SavingStatus(GNENet* net) :
    myNet(net) {
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


GNENetHelper::SavingStatus::AskSaving
GNENetHelper::SavingStatus::askSaveNetwork() const {
    // Check if there are non saved network elements
    if (myNetworkSaved) {
        return AskSaving::CONTINUE;
    } else {
        // open question box
        const std::string header = TL("Confirm close Network");
        const std::string contentsA = TL("You have unsaved changes in the network.");
        const std::string contentsB = TL("Do you wish to close and discard all changes?");
        const auto answer = GUISaveDialog::question(myNet->getApp(), header.c_str(), "%s", (contentsA + "\n" + contentsB).c_str());
        // continue depending of answer
        if (answer == GUISaveDialog::CLICKED_DISCARD) {
            return AskSaving::DISCARD;
        } else if (answer == GUISaveDialog::CLICKED_SAVE) {
            return AskSaving::SAVE;
        } else {
            return AskSaving::ABORT;
        }
    }
}


GNENetHelper::SavingStatus::AskSaving
GNENetHelper::SavingStatus::askSaveAdditionalElements() const {
    // Check if there are non saved additional elements
    if (myAdditionalSaved) {
        return AskSaving::CONTINUE;
    } else {
        // open question box
        const std::string header = TL("Save additional elements before close");
        const std::string contentsA = TL("You have unsaved additional elements.");
        const std::string contentsB = TL("Do you wish to close and discard all changes?");
        const auto answer = GUISaveDialog::question(myNet->getApp(), header.c_str(), "%s", (contentsA + "\n" + contentsB).c_str());
        // continue depending of answer
        if (answer == GUISaveDialog::CLICKED_DISCARD) {
            return AskSaving::DISCARD;
        } else if (answer == GUISaveDialog::CLICKED_SAVE) {
            return AskSaving::SAVE;
        } else {
            return AskSaving::ABORT;
        }
    }
}


GNENetHelper::SavingStatus::AskSaving
GNENetHelper::SavingStatus::askSaveDemandElements() const {
    // Check if there are non saved demand elements
    if (myDemandElementSaved) {
        return AskSaving::CONTINUE;
    } else {
        // open question box
        const std::string header = TL("Save demand elements before close");
        const std::string contentsA = TL("You have unsaved demand elements.");
        const std::string contentsB = TL("Do you wish to close and discard all changes?");
        const auto answer = GUISaveDialog::question(myNet->getApp(), header.c_str(), "%s", (contentsA + "\n" + contentsB).c_str());
        // continue depending of answer
        if (answer == GUISaveDialog::CLICKED_DISCARD) {
            return AskSaving::DISCARD;
        } else if (answer == GUISaveDialog::CLICKED_SAVE) {
            return AskSaving::SAVE;
        } else {
            return AskSaving::ABORT;
        }
    }
}


GNENetHelper::SavingStatus::AskSaving
GNENetHelper::SavingStatus::askSaveDataElements() const {
    // Check if there are non saved data elements
    if (myDataElementSaved) {
        return AskSaving::CONTINUE;
    } else {
        // open question box
        const std::string header = TL("Save data elements before close");
        const std::string contentsA = TL("You have unsaved data elements.");
        const std::string contentsB = TL("Do you wish to close and discard all changes?");
        const auto answer = GUISaveDialog::question(myNet->getApp(), header.c_str(), "%s", (contentsA + "\n" + contentsB).c_str());
        // continue depending of answer
        if (answer == GUISaveDialog::CLICKED_DISCARD) {
            return AskSaving::DISCARD;
        } else if (answer == GUISaveDialog::CLICKED_SAVE) {
            return AskSaving::SAVE;
        } else {
            return AskSaving::ABORT;
        }
    }
}


GNENetHelper::SavingStatus::AskSaving
GNENetHelper::SavingStatus::askSaveMeanDataElements() const {
    // Check if there are non saved mean data elements
    if (myMeanDataElementSaved) {
        return AskSaving::CONTINUE;
    } else {
        // open question box
        const std::string header = TL("Save meanData elements before close");
        const std::string contentsA = TL("You have unsaved meanData elements.");
        const std::string contentsB = TL("Do you wish to close and discard all changes?");
        const auto answer = GUISaveDialog::question(myNet->getApp(), header.c_str(), "%s", (contentsA + "\n" + contentsB).c_str());
        // continue depending of answer
        if (answer == GUISaveDialog::CLICKED_DISCARD) {
            return AskSaving::DISCARD;
        } else if (answer == GUISaveDialog::CLICKED_SAVE) {
            return AskSaving::SAVE;
        } else {
            return AskSaving::ABORT;
        }
    }
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
