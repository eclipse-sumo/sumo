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
/// @file    GNEHierarchicalParentElements.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// A abstract class for representation of Hierarchical Elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/GNEGeometry.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/geom/Position.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;
class GNENetworkElement;
class GNEDemandElement;
class GNEShape;
class GNEGenericData;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEHierarchicalParentElements
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEHierarchicalParentElements {

public:

    /// @brief declare GNEChange_Children as friend class
    friend class GNEChange_Children;

    /**@brief Constructor used by elements that have another additionals as parent
     * @param[in] tag Type of xml tag that define the element (SUMO_TAG_BUS_STOP, SUMO_TAG_JUNCTION, etc...)
     * @param[in] parentEdges vector of parent edges
     * @param[in] parentLanes vector of parent lanes
     * @param[in] parentShapes vector of parent shapes
     * @param[in] parentAdditionals vector of parent additionals
     * @param[in] parentDemandElements vector of parent demand elements
     * @param[in] parentGenericData vector of parent generic data elements
     */
    GNEHierarchicalParentElements(GNEAttributeCarrier* AC,
                                  const std::vector<GNEEdge*>& parentEdges,
                                  const std::vector<GNELane*>& parentLanes,
                                  const std::vector<GNEShape*>& parentShapes,
                                  const std::vector<GNEAdditional*>& parentAdditionals,
                                  const std::vector<GNEDemandElement*>& parentDemandElements,
                                  const std::vector<GNEGenericData*>& parentGenericDatas);

    /// @brief Destructor
    ~GNEHierarchicalParentElements();

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @name members and functions related to parent edges
    /// @{
    /// @brief add parent edge
    void addParentEdge(GNEEdge* edge);

    /// @brief remove parent edge
    void removeParentEdge(GNEEdge* edge);

    /// @brief get parent edges
    const std::vector<GNEEdge*>& getParentEdges() const;

    /// @brief get middle (via) parent edges
    std::vector<GNEEdge*> getMiddleParentEdges() const;

    /// @brief get path edges
    const std::vector<GNEEdge*>& getPathEdges() const;

    /// @}

    /// @name members and functions related to parent lanes
    /// @{
    /// @brief add parent lane
    void addParentLane(GNELane* lane);

    /// @brief remove parent lane
    void removeParentLane(GNELane* lane);

    /// @brief get parent lanes
    const std::vector<GNELane*>& getParentLanes() const;

    /// @}

    /// @name members and functions related to parent shapes
    /// @{
    /// @brief add parent shape
    void addParentShape(GNEShape* shape);

    /// @brief remove parent shape
    void removeParentShape(GNEShape* shape);

    /// @brief get parent shapes
    const std::vector<GNEShape*>& getParentShapes() const;

    /// @}

    /// @name members and functions related to parent additionals
    /// @{
    /// @brief add parent additional
    void addParentAdditional(GNEAdditional* additional);

    /// @brief remove parent additional
    void removeParentAdditional(GNEAdditional* additional);

    /// @brief get parent additionals
    const std::vector<GNEAdditional*>& getParentAdditionals() const;

    /// @brief get number of parent additionals
    size_t getNumberOfParentAdditionals(GNETagProperties::TagType additionalType) const;

    /// @}

    /// @name members and functions related to parent demand elements
    /// @{
    /// @brief add parent demand element
    void addParentDemandElement(GNEDemandElement* demandElement);

    /// @brief remove parent demand element
    void removeParentDemandElement(GNEDemandElement* demandElement);

    /// @brief get parent demand elements
    const std::vector<GNEDemandElement*>& getParentDemandElements() const;

    /// @}

    /// @name members and functions related to parent generic data elements
    /// @{
    /// @brief add parent demand element
    void addParentGenericData(GNEGenericData* genericData);

    /// @brief remove parent demand element
    void removeParentGenericData(GNEGenericData* genericData);

    /// @brief get parent demand elements
    const std::vector<GNEGenericData*>& getParentGenericDatas() const;

    /// @}

    /// @brief if use edge/parent lanes as a list of consecutive elements, obtain a list of IDs of elements after insert a new element
    std::string getNewListOfParents(const GNENetworkElement* currentElement, const GNENetworkElement* newNextElement) const;

protected:
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

    /// @brief replace edge route Parents
    void replacePathEdges(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& routeEdges);

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

private:
    /// @brief vector of parent edges of this element
    std::vector<GNEEdge*> myParentEdges;

    /// @brief vector of edges used in Flow/Trips/PersonTrips
    std::vector<GNEEdge*> myRouteEdges;

    /// @brief vector of parent lanes of this element
    std::vector<GNELane*> myParentLanes;

    /// @brief vector of parent shapes of this element
    std::vector<GNEShape*> myParentShapes;

    /// @brief vector of parent additionals of this element
    std::vector<GNEAdditional*> myParentAdditionals;

    /// @brief vector of demand elements parents of this element
    std::vector<GNEDemandElement*> myParentDemandElements;

    /// @brief vector of generic datas parents of this element
    std::vector<GNEGenericData*> myParentGenericDatas;

    /// @brief pointer to AC (needed to avoid diamond problem)
    GNEAttributeCarrier* myAC;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalParentElements(const GNEHierarchicalParentElements&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalParentElements& operator=(const GNEHierarchicalParentElements&) = delete;
};

