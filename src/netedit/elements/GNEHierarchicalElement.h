/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEHierarchicalElementHelper.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2020
///
// A abstract class for representation of hierarchical elements
/****************************************************************************/
#pragma once

#include "GNEHierarchicalElementHelper.h"


// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEHierarchicalElements
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEHierarchicalElement {

public:

    /// @brief declare GNEChange_Children as friend class
    friend class GNEChange_Children;
    friend class GNEDemandElement;

    /**@brief Constructor
     * @param[in] AC Attribute carrier
     * @param[in] parentJunctions vector of parent junctions
     * @param[in] parentEdges vector of parent edges
     * @param[in] parentLanes vector of parent lanes
     * @param[in] parentAdditionals vector of parent additionals
     * @param[in] parentShapes vector of parent shapes
     * @param[in] parentTAZElements vector of parent TAZs
     * @param[in] parentDemandElements vector of parent demand elements
     * @param[in] parentGenericData vector of parent generic data elements
     * @param[in] childJunctions vector of child junctions
     * @param[in] childEdges vector of child edges
     * @param[in] childLanes vector of child lanes
     * @param[in] childAdditionals vector of child additional
     * @param[in] childShapes vector of child shapes
     * @param[in] childTAZElements vector of child TAZs
     * @param[in] childDemandElements vector of child demand elements
     * @param[in] childGenericDataElements vector of child generic data elements
     */
    GNEHierarchicalElement(const GNEAttributeCarrier* AC,
        const std::vector<GNEJunction*>& parentJunctions,
        const std::vector<GNEEdge*>& parentEdges,
        const std::vector<GNELane*>& parentLanes,
        const std::vector<GNEAdditional*>& parentAdditionals,
        const std::vector<GNEShape*>& parentShapes,
        const std::vector<GNETAZElement*>& parentTAZElements,
        const std::vector<GNEDemandElement*>& parentDemandElements,
        const std::vector<GNEGenericData*>& parentGenericDatas,
        const std::vector<GNEJunction*>& childJunctions,
        const std::vector<GNEEdge*>& childEdges,
        const std::vector<GNELane*>& childLanes,
        const std::vector<GNEAdditional*>& childAdditionals,
        const std::vector<GNEShape*>& childShapes,
        const std::vector<GNETAZElement*>& childTAZElements,
        const std::vector<GNEDemandElement*>& childDemandElements,
        const std::vector<GNEGenericData*>& childGenericDataElements);

    /// @brief Destructor
    ~GNEHierarchicalElement();

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @brief get hierarchicalcontainer with parents and children
    const GNEHierarchicalElementHelper::HierarchicalContainer &getHierarchicalContainer() const;

    /// @brief restore hierarchical container
    void restoreHierarchicalContainer(const GNEHierarchicalElementHelper::HierarchicalContainer &container);

    /// @name common get functions
    /// @{

    /// @brief get all parents and children
    std::vector<GNEHierarchicalElement*> getAllHierarchicalElements() const;

    /// @brief get parent edges
    const std::vector<GNEEdge*>& getParentEdges() const;

    /// @brief get parent lanes
    const std::vector<GNELane*>& getParentLanes() const;

    /// @brief get parent additionals
    const std::vector<GNEAdditional*>& getParentAdditionals() const;

    /// @brief get parent shapes
    const std::vector<GNEShape*>& getParentShapes() const;

    /// @brief get parent TAZElements
    const std::vector<GNETAZElement*>& getParentTAZElements() const;

    /// @brief get parent demand elements
    const std::vector<GNEDemandElement*>& getParentDemandElements() const;

    /// @brief get parent demand elements
    const std::vector<GNEGenericData*>& getParentGenericDatas() const;

    /// @brief get child edges
    const std::vector<GNEEdge*>& getChildEdges() const;

    /// @brief get child lanes
    const std::vector<GNELane*>& getChildLanes() const;

    /// @brief return child additionals
    const std::vector<GNEAdditional*>& getChildAdditionals() const;

    /// @brief get child shapes
    const std::vector<GNEShape*>& getChildShapes() const;

    /// @brief get child TAZElements
    const std::vector<GNETAZElement*>& getChildTAZElements() const;

    /// @brief return child demand elements
    const std::vector<GNEDemandElement*>& getChildDemandElements() const;

    /// @brief return child generic data elements
    const std::vector<GNEGenericData*>& getChildGenericDatas() const;
    /// @}
    
    /// @name common generic add/remove functions
    /// @{

    /// @brief add parent element
    template<typename T>
    void addParentElement(T* element) {
        // add parent element into container
        myHierarchicalContainer.addChildElement(myAC, element);
    }

    /// @brief remove parent elmeent
    template<typename T>
    void removeParentElement(T* element) {
        // remove parent element from container
        myHierarchicalContainer.removeChildElement(myAC, element);
    }

    /// @brief add child element
    template<typename T>
    void addChildElement(T* element) {
        // add child element into container
        myHierarchicalContainer.addChildElement(myAC, element);
        // update connections geometry
        myParentConnections.update();
        // Check if children must be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortChildDemandElements();
        }
    }

    /// @brief remove child element
    template<typename T>
    void removeChildElement(T* element) {
        // remove child element from container
        myHierarchicalContainer.removeChildElement(myAC, element);
        // update connections geometry
        myParentConnections.update();
        // Check if children must be sorted automatically
        if (myAC->getTagProperty().canAutomaticSortChildren()) {
            sortChildDemandElements();
        }
    }
    /// @}

    /// @name specific get functions
    /// @{

    /// @brief get front parent junction
    GNEJunction* getFirstParentJunction() const;

    /// @brief remove parent junction
    GNEJunction* getSecondParentJunction()const;

    /// @brief get middle (via) parent edges
    std::vector<GNEEdge*> getMiddleParentEdges() const;

    /// @brief if use edge/parent lanes as a list of consecutive elements, obtain a list of IDs of elements after insert a new element
    std::string getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const;

    /// @brief get child position calculated in ChildConnections
    const Position& getChildPosition(const GNELane* lane);

    /// @brief get child rotation calculated in ChildConnections
    double getChildRotation(const GNELane* lane);

    /// @brief get previous child demand element to the given demand element
    GNEDemandElement* getPreviousChildDemandElement(const GNEDemandElement* demandElement) const;

    /// @brief get next child demand element to the given demand element
    GNEDemandElement* getNextChildDemandElement(const GNEDemandElement* demandElement) const;
    /// @}

    /// @brief update front parent junction
    void updateFirstParentJunction(GNEJunction* junction);

    /// @brief update last parent junction
    void updateSecondParentJunction(GNEJunction* junction);

    /// @brief update child connections
    void updateChildConnections();

    /// @brief Draw connections between parent and children
    void drawChildConnections(const GUIVisualizationSettings& s, const GUIGlObjectType GLTypeParent, const double exaggeration) const;

    /// @brief Draw dotted connections between parent and children
    void drawChildDottedConnections(const GUIVisualizationSettings& s, const double exaggeration) const;

    /// @brief sort child additionals (used by Rerouters, VSS, TAZs...)
    void sortChildAdditionals();

    /// @brief check if children are overlapped (Used by Rerouters)
    bool checkChildAdditionalsOverlapping() const;

    /// @brief sort child demand elements
    void sortChildDemandElements();

    /// @brief check if childs demand elements are overlapped
    bool checkChildDemandElementsOverlapping() const;

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateParentAdditional();

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateParentDemandElement();

protected:
    /// @name replace parent elements
    /// @{

    /// @brief replace parent edges
    template<typename T>
    void replaceParentEdges(T* elementChild, const std::vector<GNEEdge*>& newParentEdges) {
        // remove elementChild from parents
        for (const auto& edge : myHierarchicalContainer.parentEdges) {
            edge->removeChildElement(elementChild);
        }
        // set new parents edges
        myHierarchicalContainer.parentEdges = newParentEdges;
        // add elementChild into new parents
        for (const auto& edge : myHierarchicalContainer.parentEdges) {
            edge->addChildElement(elementChild);
        }
    }

    /// @brief replace parent lanes
    template<typename T>
    void replaceParentLanes(T* elementChild, const std::vector<GNELane*>& newParentLanes) {
        // remove elementChild from parents
        for (const auto& lane : myHierarchicalContainer.parentLanes) {
            lane->removeChildElement(elementChild);
        }
        // set new parents lanes
        myHierarchicalContainer.parentLanes = newParentLanes;
        // add elementChild into new parents
        for (const auto& lane : myHierarchicalContainer.parentLanes) {
            lane->addChildElement(elementChild);
        }
    }

    /// @brief replace parent additionals
    template<typename T>
    void replaceParentAdditionals(T* elementChild, const std::vector<GNEAdditional*>& newParentAdditionals) {
        // remove elementChild from parents
        for (const auto& additional : myHierarchicalContainer.parentAdditionals) {
            additional->removeChildElement(elementChild);
        }
        // set new parents additionals
        myHierarchicalContainer.parentAdditionals = newParentAdditionals;
        // add elementChild into new parents
        for (const auto& additional : myHierarchicalContainer.parentAdditionals) {
            additional->addChildElement(elementChild);
        }
    }

    /// @brief replace parent demand elements
    template<typename T>
    void replaceParentDemandElements(T* elementChild, const std::vector<GNEDemandElement*>& newParentDemandElements) {
        // remove elementChild from parents
        for (const auto& demandElement : myHierarchicalContainer.parentDemandElements) {
            demandElement->removeChildElement(elementChild);
        }
        // set new parents demandElements
        myHierarchicalContainer.parentDemandElements = newParentDemandElements;
        // add elementChild into new parents
        for (const auto& demandElement : myHierarchicalContainer.parentDemandElements) {
            demandElement->addChildElement(elementChild);
        }
    }

    /// @brief replace parent TAZElements
    template<typename T>
    void replaceParentTAZElements(T* elementChild, const std::vector<GNETAZElement*>& newParentTAZElements) {
        // remove elementChild from parents
        for (const auto& TAZElement : myHierarchicalContainer.parentTAZElements) {
            TAZElement->removeChildElement(elementChild);
        }
        // set new parents TAZElements
        myHierarchicalContainer.parentTAZElements = newParentTAZElements;
        // add elementChild into new parents
        for (const auto& TAZElement : myHierarchicalContainer.parentTAZElements) {
            TAZElement->addChildElement(elementChild);
        }
    }
    /// @}

    /// @name replace child elements
    /// @{

    /// @brief replace child edges
    template<typename T>
    void replaceChildEdges(T* elementChild, const std::vector<GNEEdge*>& newChildEdges) {
        // remove elementChild from childs
        for (const auto& edge : myHierarchicalContainer.childEdges) {
            edge->removeChildElement(elementChild);
        }
        // set new childs edges
        myHierarchicalContainer.childEdges = newChildEdges;
        // add elementChild into new childs
        for (const auto& edge : myHierarchicalContainer.childEdges) {
            edge->addChildElement(elementChild);
        }
    }

    /// @brief replace child lanes
    template<typename T>
    void replaceChildLanes(T* elementChild, const std::vector<GNELane*>& newChildLanes) {
        // remove elementChild from childs
        for (const auto& lane : myHierarchicalContainer.childLanes) {
            lane->removeChildElement(elementChild);
        }
        // set new childs lanes
        myHierarchicalContainer.childLanes = newChildLanes;
        // add elementChild into new childs
        for (const auto& lane : myHierarchicalContainer.childLanes) {
            lane->addChildElement(elementChild);
        }
    }
    /// @}

    /// @brief variable ParentConnections
    GNEGeometry::ParentConnections myParentConnections;

    /// @brief variable ChildConnections
    GNEHierarchicalElementHelper::ChildConnections myChildConnections;

private:
    /// @brief hierarchical container with parents and children
    GNEHierarchicalElementHelper::HierarchicalContainer myHierarchicalContainer;

    /// @brief pointer to AC (needed to avoid diamond problem)
    const GNEAttributeCarrier* myAC;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalElement(const GNEHierarchicalElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalElement& operator=(const GNEHierarchicalElement&) = delete;
};

