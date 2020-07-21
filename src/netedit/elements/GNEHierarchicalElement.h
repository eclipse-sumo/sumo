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

    /// @name get functions
    /// @{

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
    
    /// @name generic add/remove functions
    /// @{

    /// @brief add parent element
    template<typename T>
    void addParentElement(T* element);

    /// @brief remove parent elmeent
    template<typename T>
    void removeParentElement(T* element);

    /// @brief add child element
    template<typename T>
    void addChildElement(T* element);

    /// @brief remove child element
    template<typename T>
    void removeChildElement(T* element);

    /// @}

    /// @brief get front parent junction
    GNEJunction* getFirstParentJunction() const;

    /// @brief remove parent junction
    GNEJunction* getSecondParentJunction()const ;

    /// @brief update front parent junction
    void updateFirstParentJunction(GNEJunction* junction);

    /// @brief update last parent junction
    void updateSecondParentJunction(GNEJunction* junction);

    /// @brief get middle (via) parent edges
    std::vector<GNEEdge*> getMiddleParentEdges() const;

    /// @brief if use edge/parent lanes as a list of consecutive elements, obtain a list of IDs of elements after insert a new element
    std::string getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const;

    /* Children */


    /// @brief get child position calculated in ChildConnections
    const Position& getChildPosition(const GNELane* lane);

    /// @brief get child rotation calculated in ChildConnections
    double getChildRotation(const GNELane* lane);

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

    /// @brief return child demand elements by type
    const std::vector<GNEDemandElement*>& getChildDemandElementsByType(SumoXMLTag tag) const;

    /// @brief sort child demand elements
    void sortChildDemandElements();

    /// @brief check if childs demand elements are overlapped
    bool checkChildDemandElementsOverlapping() const;

    /// @brief get previous child demand element to the given demand element
    GNEDemandElement* getPreviousChildDemandElement(const GNEDemandElement* demandElement) const;

    /// @brief get next child demand element to the given demand element
    GNEDemandElement* getNextChildDemandElement(const GNEDemandElement* demandElement) const;

    /// @}

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateParentAdditional();

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateParentDemandElement();

protected:

    /* parent */

    /// @name members and functions relative to changing parents
    /// @{

    /// @brief replace the parent edges of a shape
    void replaceParentEdges(GNEShape* elementChild, const std::string& newEdgeIDs);

    /// @brief replace the parent edges of an additional
    void replaceParentEdges(GNEAdditional* elementChild, const std::string& newEdgeIDs);

    /// @brief replace the parent edges of a demandElement
    void replaceParentEdges(GNEDemandElement* elementChild, const std::string& newEdgeIDs);

    /// @brief replace the parent edges of a demandElement (GNEEdge version)
    void replaceParentEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newEdges);

    /// @brief replace the parent edges of a generic data (GNEEdge version)
    void replaceParentEdges(GNEGenericData* elementChild, const std::vector<GNEEdge*>& newEdges);

    /// @brief replace the first parent edge (used by demand elements)
    void replaceFirstParentEdge(GNEDemandElement* elementChild, GNEEdge* newFirstEdge);

    /// @brief replace the first parent edge (used by generic data elements)
    void replaceFirstParentEdge(GNEGenericData* elementChild, GNEEdge* newFirstEdge);

    /// @brief replace middle (via) parent edges
    void replaceMiddleParentEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newMiddleEdges, const bool updateChildReferences);

    /// @brief replace the last parent edge (used by demand elements)
    void replaceLastParentEdge(GNEDemandElement* elementChild, GNEEdge* newLastEdge);

    /// @brief replace the last parent edge (used by generic data elements)
    void replaceLastParentEdge(GNEGenericData* elementChild, GNEEdge* newLastEdge);

    /// @brief replace the parent edges of a shape
    void replaceParentLanes(GNEShape* elementChild, const std::string& newLaneIDs);

    /// @brief replace the parent edges of an additional
    void replaceParentLanes(GNEAdditional* elementChild, const std::string& newLaneIDs);

    /// @brief replace the parent edges of a demandElement
    void replaceParentLanes(GNEDemandElement* elementChild, const std::string& newLaneIDs);

    /**@brief replace the parent additional of a shape
     * @throw exception if this shape doesn't have previously a defined Additional parent
     * @throw exception if shape with ID newParentAdditionalID doesn't exist
     */
    void replaceParentAdditional(GNEShape* shapeTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex);

    /**@brief replace the parent additional of an additional
     * @throw exception if this additional doesn't have previously a defined Additional parent
     * @throw exception if additional with ID newParentAdditionalID doesn't exist
     */
    void replaceParentAdditional(GNEAdditional* additionalTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex);

    /**@brief replace the parent additional of a demand element
     * @throw exception if this additional doesn't have previously a defined Additional parent
     * @throw exception if additional with ID newParentAdditionalID doesn't exist
     */
    void replaceParentAdditional(GNEDemandElement* demandElementTobeChanged, const std::string& newParentAdditionalID, int additionalParentIndex);

    /// @brief replace the first parent TAZElement (used by generic data elements)
    void replaceFirstParentTAZElement(GNEGenericData* elementChild, GNETAZElement* newFirstTAZElement);

    /// @brief replace the last parent TAZElement (used by demand elements)
    void replaceLastParentTAZElement(GNEGenericData* elementChild, GNETAZElement* newLastTAZElement);

    /**@brief replace the parent demand element of a shape
     * @throw exception if this demand element doesn't have previously a defined DemandElement parent
     * @throw exception if demand element with ID newParentDemandElementID doesn't exist
     */
    void replaceParentDemandElement(GNEShape* shapeTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex);

    /**@brief replace the parent demand element of an additional
     * @throw exception if this demand element doesn't have previously a defined DemandElement parent
     * @throw exception if demand element with ID newParentDemandElementID doesn't exist
     */
    void replaceParentDemandElement(GNEAdditional* additionalTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex);

    /**@brief change first parent demand element of demandElement
     * @throw exception if this demand element doesn't have previously a defined DemandElement parent
     * @throw exception if demand element with ID newParentDemandElementID doesn't exist
     */
    void replaceParentDemandElement(GNEDemandElement* demandElementTobeChanged, const std::string& newParentDemandElementID, int demandElementParentIndex);

    /// @}

    /// @brief variable ParentConnections
    GNEGeometry::ParentConnections myParentConnections;

    /// @brief change child edges of an additional
    void changeChildEdges(GNEAdditional* elementChild, const std::string& newEdgeIDs);

    /// @brief change child edges of an additional
    void changeChildLanes(GNEAdditional* elementChild, const std::string& newEdgeIDs);

    /// @brief variable ChildConnections
    GNEHierarchicalElementHelper::ChildConnections myChildConnections;

private:
    /// @brief container with parents and children
    GNEHierarchicalElementHelper::Container myContainer;

    /// @brief vector with the demand elements children sorted by type and filtered (to avoid duplicated
    std::map<SumoXMLTag, std::vector<GNEDemandElement* >> myDemandElementsByType;

    /// @brief pointer to AC (needed to avoid diamond problem)
    const GNEAttributeCarrier* myAC;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalElement(const GNEHierarchicalElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalElement& operator=(const GNEHierarchicalElement&) = delete;
};

