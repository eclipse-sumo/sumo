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

#include "GNEHierarchicalStructure.h"


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
    GNEHierarchicalElement* getHierarchicalElement() {
        return this;
    }

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @brief get hierarchicalcontainer with parents and children
    const GNEHierarchicalStructure& getHierarchicalContainer() const;

    /// @brief restore hierarchical container
    void restoreHierarchicalContainer(const GNEHierarchicalStructure& container);

    /// @name common get functions
    /// @{

    /// @brief get all parents and children
    std::vector<GNEHierarchicalElement*> getAllHierarchicalElements() const;

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

    /// @name common generic add/remove functions
    /// @{

    /// @brief add parent element
    template<typename T>
    void addParentElement(T* element) {
        myHierarchicalStructure.addParentElement(element);
    }

    /// @brief remove parent element
    template<typename T>
    void removeParentElement(T* element) {
        myHierarchicalStructure.removeParentElement(element);
    }

    /// @brief add child element
    template<typename T>
    void addChildElement(T* element) {
        myHierarchicalStructure.addChildElement(element);
    }

    /// @brief remove child element
    template<typename T>
    void removeChildElement(T* element) {
        myHierarchicalStructure.removeChildElement(element);
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

protected:
    /// @brief replace parents in the given edited element
    template<typename T, typename U>
    void replaceParentElements(T* editedElement, const std::vector<U>& newParents) {
        // remove edited elements from parents
        for (const auto& parent : myHierarchicalStructure.getParents<U>()) {
            parent->removeChildElement(editedElement);
        }
        // set new parents
        myHierarchicalStructure.setParents(newParents);
        // add edited elements in new parents
        for (const auto& parent : myHierarchicalStructure.getParents<U>()) {
            parent->addChildElement(editedElement);
        }
    }

    /// @brief replace children in the given edited element
    template<typename T, typename U>
    void replaceChildElements(T* editedElement, const GNEHierarchicalContainerChildren<U>& newChildren) {
        // remove edited elements from children
        for (const auto& child : myHierarchicalStructure.getChildren<U>()) {
            child->removeChildElement(editedElement);
        }
        // set new children
        myHierarchicalStructure.setChildren(newChildren);
        // add edited elements in new children
        for (const auto& child : myHierarchicalStructure.getChildren<U>()) {
            child->addChildElement(editedElement);
        }
    }

private:
    /// @brief hierarchical structure with parents and children
    GNEHierarchicalStructure myHierarchicalStructure;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalElement(const GNEHierarchicalElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalElement& operator=(const GNEHierarchicalElement&) = delete;
};
