/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEHierarchicalElementParents.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// A abstract class for representation of Hierarchical Elements
/****************************************************************************/
#ifndef GNEHierarchicalElementParents_h
#define GNEHierarchicalElementParents_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/geom/Position.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;
class GNEDemandElement;
class GNEShape;
class GNEConnection;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEHierarchicalElementParents
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEHierarchicalElementParents {

public:

    /// @brief declare GNEChange_Children as friend class
    friend class GNEChange_Children;

    /**@brief Constructor used by elements that have another additionals as parent
     * @param[in] tag Type of xml tag that define the element (SUMO_TAG_BUS_STOP, SUMO_TAG_JUNCTION, etc...)
     * @param[in] edgeParents vector of edge parents
     * @param[in] laneParents vector of lane parents
     * @param[in] shapeParents vector of shape parents
     * @param[in] additionalParents vector of additional parents
     * @param[in] demandElementParents vector of demand element parents
     */
    GNEHierarchicalElementParents(GNEAttributeCarrier* AC,
                                  const std::vector<GNEEdge*>& edgeParents,
                                  const std::vector<GNELane*>& laneParents,
                                  const std::vector<GNEShape*>& shapeParents,
                                  const std::vector<GNEAdditional*>& additionalParents,
                                  const std::vector<GNEDemandElement*>& demandElementParents);

    /// @brief Destructor
    ~GNEHierarchicalElementParents();

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @name members and functions related to edge parents
    /// @{
    /// @brief add edge parent
    void addEdgeParent(GNEEdge* edge);

    /// @brief remove edge parent
    void removeEdgeParent(GNEEdge* edge);

    /// @brief get edge parents
    const std::vector<GNEEdge*>& getEdgeParents() const;

    /// @}

    /// @name members and functions related to lane parents
    /// @{
    /// @brief add lane parent
    void addLaneParent(GNELane* lane);

    /// @brief remove lane parent
    void removeLaneParent(GNELane* lane);

    /// @brief get lanes of VSS
    const std::vector<GNELane*>& getLaneParents() const;

    /// @}

    /// @name members and functions related to shape parents
    /// @{
    /// @brief add shape parent
    void addShapeParent(GNEShape* shape);

    /// @brief remove shape parent
    void removeShapeParent(GNEShape* shape);

    /// @brief get shapes of VSS
    const std::vector<GNEShape*>& getShapeParents() const;

    /// @}

    /// @name members and functions related to additional parents
    /// @{
    /// @brief add additional parent to this additional
    void addAdditionalParent(GNEAdditional* additional);

    /// @brief remove additional parent from this additional
    void removeAdditionalParent(GNEAdditional* additional);

    /// @brief return vector of additionals that have as Parent this edge (For example, Calibrators)
    const std::vector<GNEAdditional*>& getAdditionalParents() const;

    /// @}

    /// @name members and functions related to demand element parents
    /// @{
    /// @brief add demand element parent to this demand element
    void addDemandElementParent(GNEDemandElement* demandElement);

    /// @brief remove demand element parent from this demand element
    void removeDemandElementParent(GNEDemandElement* demandElement);

    /// @brief return vector of demand elements that have as Parent this edge (For example, Calibrators)
    const std::vector<GNEDemandElement*>& getDemandElementParents() const;

    /// @}

protected:
    /// @brief struct for pack all variables and functions relative to connections between hierarchical element and their children
    struct ParentConnections {
        /// @brief constructor
        ParentConnections(GNEHierarchicalElementParents* hierarchicalElement);

        /// @brief update Connection's geometry
        void update();

        /// @brief draw connections between Parent and childrens
        void draw(const GUIVisualizationSettings& s, const GUIGlObjectType parentType) const;

        /// @brief position and rotation of every symbol over lane
        std::vector<std::pair<Position, double> > symbolsPositionAndRotation;

        /// @brief Matrix with the Vertex's positions of connections between parents an their children
        std::vector<PositionVector> connectionPositions;

    private:
        /// @brief pointer to hierarchical element parent
        GNEHierarchicalElementParents* myHierarchicalElement;
    };

    /// @name members and functions relative to changing parents
    /// @{

    /// @brief change edge parents of a shape
    void changeEdgeParents(GNEShape* elementChild, const std::string& newEdgeIDs);

    /// @brief change edge parents of an additional
    void changeEdgeParents(GNEAdditional* elementChild, const std::string& newEdgeIDs);

    /// @brief change edge parents of a demandElement
    void changeEdgeParents(GNEDemandElement* elementChild, const std::string& newEdgeIDs);

    /// @brief change edge parents of a demandElement (GNEEdge version)
    void changeEdgeParents(GNEDemandElement* elementChild, const std::vector<GNEEdge*>& newEdges);

    /// @brief change edge parents of a shape
    void changeLaneParents(GNEShape* elementChild, const std::string& newLaneIDs);

    /// @brief change edge parents of an additional
    void changeLaneParents(GNEAdditional* elementChild, const std::string& newLaneIDs);

    /// @brief change edge parents of a demandElement
    void changeLaneParents(GNEDemandElement* elementChild, const std::string& newLaneIDs);

    /**@brief change additional parent of a shape
     * @throw exception if this shape doesn't have previously a defined Additional parent
     * @throw exception if shape with ID newAdditionalParentID doesn't exist
     */
    void changeAdditionalParent(GNEShape* shapeTobeChanged, const std::string& newAdditionalParentID, int additionalParentIndex);

    /**@brief change additional parent of an additional
     * @throw exception if this additional doesn't have previously a defined Additional parent
     * @throw exception if additional with ID newAdditionalParentID doesn't exist
     */
    void changeAdditionalParent(GNEAdditional* additionalTobeChanged, const std::string& newAdditionalParentID, int additionalParentIndex);

    /**@brief change additional parent of a demand element
     * @throw exception if this additional doesn't have previously a defined Additional parent
     * @throw exception if additional with ID newAdditionalParentID doesn't exist
     */
    void changeAdditionalParent(GNEDemandElement* demandElementTobeChanged, const std::string& newAdditionalParentID, int additionalParentIndex);

    /**@brief change first demand element parent of a shape
     * @throw exception if this demand element doesn't have previously a defined DemandElement parent
     * @throw exception if demand element with ID newDemandElementParentID doesn't exist
     */
    void changeDemandElementParent(GNEShape* shapeTobeChanged, const std::string& newDemandElementParentID, int demandElementParentIndex);

    /**@brief change first demand element parent of an additional
     * @throw exception if this demand element doesn't have previously a defined DemandElement parent
     * @throw exception if demand element with ID newDemandElementParentID doesn't exist
     */
    void changeDemandElementParent(GNEAdditional* additionalTobeChanged, const std::string& newDemandElementParentID, int demandElementParentIndex);

    /**@brief change first demand element parent of demandElement
     * @throw exception if this demand element doesn't have previously a defined DemandElement parent
     * @throw exception if demand element with ID newDemandElementParentID doesn't exist
     */
    void changeDemandElementParent(GNEDemandElement* demandElementTobeChanged, const std::string& newDemandElementParentID, int demandElementParentIndex);

    /// @}

    /// @brief variable ParentConnections
    ParentConnections myParentConnections;

private:
    /// @brief list of edge parents of this element
    std::vector<GNEEdge*> myEdgeParents;

    /// @brief list of lane parents of this element
    std::vector<GNELane*> myLaneParents;

    /// @brief list of shape parents of this element
    std::vector<GNEShape*> myShapeParents;

    /// @brief list of additional parents of this element
    std::vector<GNEAdditional*> myAdditionalParents;

    /// @brief list of demand elements parents of this element
    std::vector<GNEDemandElement*> myDemandElementParents;

    /// @brief pointer to AC (needed to avoid diamond problem)
    GNEAttributeCarrier* myAC;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalElementParents(const GNEHierarchicalElementParents&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalElementParents& operator=(const GNEHierarchicalElementParents&) = delete;
};

#endif
