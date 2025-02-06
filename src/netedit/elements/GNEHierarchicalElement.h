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
/// @file    GNEHierarchicalElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// A abstract class for representation of hierarchical elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/div/GUIGeometry.h>
#include <netedit/elements/GNEAttributeCarrier.h>

#include "GNEHierarchicalStructureParents.h"
#include "GNEHierarchicalStructureChildren.h"

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEHierarchicalElements
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEHierarchicalElement : public GNEAttributeCarrier {

public:
    /// @brief declare GNEChange_Children as friend class
    friend class GNEChange_Children;
    friend class GNEDemandElement;

    /**@brief Constructor
     * @param[in] tag SUMO Tag assigned to this type of object
     * @param[in] net GNENet in which this AttributeCarrier is stored
     * @param[in] parentJunctions vector of parent junctions
     * @param[in] parentEdges vector of parent edges
     * @param[in] parentLanes vector of parent lanes
     * @param[in] parentAdditionals vector of parent additionals
     * @param[in] parentDemandElements vector of parent demand elements
     * @param[in] parentGenericData vector of parent generic data elements
     */
    GNEHierarchicalElement(GNENet* net, SumoXMLTag tag,
                           const std::vector<GNEJunction*>& parentJunctions,
                           const std::vector<GNEEdge*>& parentEdges,
                           const std::vector<GNELane*>& parentLanes,
                           const std::vector<GNEAdditional*>& parentAdditionals,
                           const std::vector<GNEDemandElement*>& parentDemandElements,
                           const std::vector<GNEGenericData*>& parentGenericDatas);

    /// @brief Destructor
    ~GNEHierarchicalElement();

    /// @brief get GNEHierarchicalElement associated with this AttributeCarrier
    GNEHierarchicalElement* getHierarchicalElement();

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @brief get parents(used in GNE_Change)
    const GNEHierarchicalStructureParents getParents() const;

    /// @brief clear hierarchical structure parents (used in GNE_Change)
    void clear();

    /// @name common get functions
    /// @{

    /// @brief get parent junctions
    const GNEHierarchicalContainerParents<GNEJunction*>& getParentJunctions() const;

    /// @brief get parent edges
    const GNEHierarchicalContainerParents<GNEEdge*>& getParentEdges() const;

    /// @brief get parent lanes
    const GNEHierarchicalContainerParents<GNELane*>& getParentLanes() const;

    /// @brief get parent additionals
    const GNEHierarchicalContainerParents<GNEAdditional*>& getParentAdditionals() const;

    /// @brief get parent stoppingPlaces (used by plans)
    const GNEHierarchicalContainerParents<GNEAdditional*> getParentStoppingPlaces() const;

    /// @brief get parent TAZs (used by plans)
    const GNEHierarchicalContainerParents<GNEAdditional*> getParentTAZs() const;

    /// @brief get parent demand elements
    const GNEHierarchicalContainerParents<GNEDemandElement*>& getParentDemandElements() const;

    /// @brief get parent demand elements
    const GNEHierarchicalContainerParents<GNEGenericData*>& getParentGenericDatas() const;

    /// @brief get child junctions
    const GNEHierarchicalContainerChildren<GNEJunction*>& getChildJunctions() const;

    /// @brief get child edges
    const GNEHierarchicalContainerChildren<GNEEdge*>& getChildEdges() const;

    /// @brief get child lanes
    const GNEHierarchicalContainerChildren<GNELane*>& getChildLanes() const;

    /// @brief return child additionals
    const GNEHierarchicalContainerChildren<GNEAdditional*>& getChildAdditionals() const;

    /// @brief return child TAZSourceSinks (Hash)
    const GNEHierarchicalContainerChildrenSet<GNETAZSourceSink*>& getChildTAZSourceSinks() const;

    /// @brief return child demand elements
    const GNEHierarchicalContainerChildren<GNEDemandElement*>& getChildDemandElements() const;

    /// @brief return child generic data elements
    const GNEHierarchicalContainerChildren<GNEGenericData*>& getChildGenericDatas() const;

    /// @}

    /// @brief add child (only used if we're creating elements without undo-redo)
    template<typename T>
    void addChildElement(T* element) {
        myHierarchicalStructureChildren.addChildElement(element);
    }

    /// @brief insert parent element (and update their child)
    /// @{

    /// @brief insert parent in the given junction
    template<typename T>
    static void insertParentInJunction(GNEJunction* junction, T* parent) {
        junction->myHierarchicalStructureParents.addParentElement(parent);
        parent->myHierarchicalStructureChildren.addChildElement(junction);
    }

    /// @brief insert parent in the given edge
    template<typename T>
    static void insertParentInEdge(GNEEdge* edge, T* parent) {
        edge->myHierarchicalStructureParents.addParentElement(parent);
        parent->myHierarchicalStructureChildren.addChildElement(edge);
    }

    /// @brief insert parent in the given lane
    template<typename T>
    static void insertParentInLane(GNELane* lane, T* parent) {
        lane->myHierarchicalStructureParents.addParentElement(parent);
        parent->myHierarchicalStructureChildren.addChildElement(lane);
    }

    /// @brief insert parent in the given additional
    template<typename T>
    static void insertParentInAdditional(GNEAdditional* additional, T* parent) {
        additional->myHierarchicalStructureParents.addParentElement(parent);
        parent->myHierarchicalStructureChildren.addChildElement(additional);
    }

    /// @brief insert parent in the given TAZ SourceSink
    template<typename T>
    static void insertParentInTAZSourceSink(GNETAZSourceSink* sourceSink, T* parent) {
        sourceSink->myHierarchicalStructureParents.addParentElement(parent);
        parent->myHierarchicalStructureChildren.addChildElement(sourceSink);
    }

    /// @brief insert parent in the given demand element
    template<typename T>
    static void insertParentInDemandElement(GNEDemandElement* demandElement, T* parent) {
        demandElement->myHierarchicalStructureParents.addParentElement(parent);
        parent->myHierarchicalStructureChildren.addChildElement(demandElement);
    }

    /// @brief insert parent in the given genericData
    template<typename T>
    static void insertParentInGenericData(GNEGenericData* genericData, T* parent) {
        genericData->myHierarchicalStructureParents.addParentElement(parent);
        parent->myHierarchicalStructureChildren.addChildElement(genericData);
    }

    /// @}

    /// @brief remove parent element (and update their child)
    /// @{

    /// @brief remove parent from the given junction
    template<typename T>
    static void removeParentFromJunction(GNEJunction* junction, T* parent) {
        junction->myHierarchicalStructureParents.removeParentElement(parent);
        parent->myHierarchicalStructureChildren.removeChildElement(junction);
    }

    /// @brief remove parent from the given edge
    template<typename T>
    static void removeParentFromEdge(GNEEdge* edge, T* parent) {
        edge->myHierarchicalStructureParents.removeParentElement(parent);
        parent->myHierarchicalStructureChildren.removeChildElement(edge);
    }

    /// @brief remove parent from the given lane
    template<typename T>
    static void removeParentFromLane(GNELane* lane, T* parent) {
        lane->myHierarchicalStructureParents.removeParentElement(parent);
        parent->myHierarchicalStructureChildren.removeChildElement(lane);
    }

    /// @brief remove parent from the given additional
    template<typename T>
    static void removeParentFromAdditional(GNEAdditional* additional, T* parent) {
        additional->myHierarchicalStructureParents.removeParentElement(parent);
        parent->myHierarchicalStructureChildren.removeChildElement(additional);
    }

    /// @brief remove parent from the given TAZ SourceSink
    template<typename T>
    static void removeParentFromTAZSourceSink(GNETAZSourceSink* sourceSink, T* parent) {
        sourceSink->myHierarchicalStructureParents.removeParentElement(parent);
        parent->myHierarchicalStructureChildren.removeChildElement(sourceSink);
    }

    /// @brief remove parent from the given demand element
    template<typename T>
    static void removeParentFromDemandElement(GNEDemandElement* demandElement, T* parent) {
        demandElement->myHierarchicalStructureParents.removeParentElement(parent);
        parent->myHierarchicalStructureChildren.removeChildElement(demandElement);
    }

    /// @brief remove parent from the given genericData
    template<typename T>
    static void removeParentFromGenericData(GNEGenericData* genericData, T* parent) {
        genericData->myHierarchicalStructureParents.removeParentElement(parent);
        parent->myHierarchicalStructureChildren.removeChildElement(genericData);
    }

    /// @brief insert child element (and update their parent)
    /// @{

    /// @brief insert child in the given junction
    template<typename T>
    static void insertChildInJunction(GNEJunction* junction, T* child) {
        junction->myHierarchicalStructureChildren.addChildElement(child);
        child->myHierarchicalStructureParents.addParentElement(junction);
    }

    /// @brief insert child in the given edge
    template<typename T>
    static void insertChildInEdge(GNEEdge* edge, T* child) {
        edge->myHierarchicalStructureChildren.addChildElement(child);
        child->myHierarchicalStructureParents.addParentElement(edge);
    }

    /// @brief insert child in the given lane
    template<typename T>
    static void insertChildInLane(GNELane* lane, T* child) {
        lane->myHierarchicalStructureChildren.addChildElement(child);
        child->myHierarchicalStructureParents.addParentElement(lane);
    }

    /// @brief insert child in the given additional
    template<typename T>
    static void insertChildInAdditional(GNEAdditional* additional, T* child) {
        additional->myHierarchicalStructureChildren.addChildElement(child);
        child->myHierarchicalStructureParents.addParentElement(additional);
    }

    /// @brief insert child in the given TAZ SourceSink
    template<typename T>
    static void insertChildInTAZSourceSink(GNETAZSourceSink* sourceSink, T* child) {
        sourceSink->myHierarchicalStructureChildren.addChildElement(child);
        child->myHierarchicalStructureParents.addParentElement(sourceSink);
    }

    /// @brief insert child in the given demand element
    template<typename T>
    static void insertChildInDemandElement(GNEDemandElement* demandElement, T* child) {
        demandElement->myHierarchicalStructureChildren.addChildElement(child);
        child->myHierarchicalStructureParents.addParentElement(demandElement);
    }

    /// @brief insert child in the given genericData
    template<typename T>
    static void insertChildInGenericData(GNEGenericData* genericData, T* child) {
        genericData->myHierarchicalStructureChildren.addChildElement(child);
        child->myHierarchicalStructureParents.addParentElement(genericData);
    }

    /// @}

    /// @brief remove child element (and update their parent)
    /// @{

    /// @brief remove child from the given junction
    template<typename T>
    static void removeChildFromJunction(GNEJunction* junction, T* child) {
        junction->myHierarchicalStructureChildren.removeChildElement(child);
        child->myHierarchicalStructureParents.removeParentElement(junction);
    }

    /// @brief remove child from the given edge
    template<typename T>
    static void removeChildFromEdge(GNEEdge* edge, T* child) {
        edge->myHierarchicalStructureChildren.removeChildElement(child);
        child->myHierarchicalStructureParents.removeParentElement(edge);
    }

    /// @brief remove child from the given lane
    template<typename T>
    static void removeChildFromLane(GNELane* lane, T* child) {
        lane->myHierarchicalStructureChildren.removeChildElement(child);
        child->myHierarchicalStructureParents.removeParentElement(lane);
    }

    /// @brief remove child from the given additional
    template<typename T>
    static void removeChildFromAdditional(GNEAdditional* additional, T* child) {
        additional->myHierarchicalStructureChildren.removeChildElement(child);
        child->myHierarchicalStructureParents.removeParentElement(additional);
    }

    /// @brief remove child from the given TAZ SourceSink
    template<typename T>
    static void removeChildFromTAZSourceSink(GNETAZSourceSink* sourceSink, T* child) {
        sourceSink->myHierarchicalStructureChildren.removeChildElement(child);
        child->myHierarchicalStructureParents.removeParentElement(sourceSink);
    }

    /// @brief remove child from the given demand element
    template<typename T>
    static void removeChildFromDemandElement(GNEDemandElement* demandElement, T* child) {
        demandElement->myHierarchicalStructureChildren.removeChildElement(child);
        child->myHierarchicalStructureParents.removeParentElement(demandElement);
    }

    /// @brief remove child from the given genericData
    template<typename T>
    static void removeChildFromGenericData(GNEGenericData* genericData, T* child) {
        genericData->myHierarchicalStructureChildren.removeChildElement(child);
        child->myHierarchicalStructureParents.removeParentElement(genericData);
    }

    /// @}

    /// @name specific get functions
    /// @{

    /// @brief if use edge/parent lanes as a list of consecutive elements, obtain a list of IDs of elements after insert a new element
    std::string getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const;

    /// @}

    /// @brief check if children are overlapped (Used by Rerouters)
    bool checkChildAdditionalsOverlapping() const;

    /// @brief check if child demand elements are overlapped
    bool checkChildDemandElementsOverlapping() const;

private:
    /// @brief hierarchical structure with parents
    GNEHierarchicalStructureParents myHierarchicalStructureParents;

    /// @brief hierarchical structure with  children
    GNEHierarchicalStructureChildren myHierarchicalStructureChildren;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalElement(const GNEHierarchicalElement&) = delete;
};
