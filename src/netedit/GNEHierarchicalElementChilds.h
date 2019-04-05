/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEHierarchicalElementChilds.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// A abstract class for representation of Hierarchical Elements
/****************************************************************************/
#ifndef GNEHierarchicalElementChilds_h
#define GNEHierarchicalElementChilds_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <utils/gui/globjects/GUIGlObjectTypes.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;
class GNEDemandElement;
class GNEShape;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEHierarchicalElementChilds
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEHierarchicalElementChilds {

public:
    /**@brief Parameter Constructor
     * @param[in] edgeChilds vector of edge childs
     * @param[in] laneChilds vector of lane childs
     * @param[in] shapeChilds vector of shape childs
     * @param[in] additionalChilds vector of additional childs
     * @param[in] demandElementChilds vector of demand element childs
     */
    GNEHierarchicalElementChilds(GNEAttributeCarrier* AC,
        const std::vector<GNEEdge*> &edgeChilds,
        const std::vector<GNELane*> &laneChilds,
        const std::vector<GNEShape*> &shapeChilds,
        const std::vector<GNEAdditional*> &additionalChilds,
        const std::vector<GNEDemandElement*> &demandElementChilds);

    /// @brief Destructor
    ~GNEHierarchicalElementChilds();

    /// @brief gererate a new ID for an element child
    virtual std::string generateChildID(SumoXMLTag childTag) = 0;

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    virtual void updateGeometry(bool updateGrid) = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @name members and functions related to edge childs
    /// @{
    /// @brief add edge child
    void addEdgeChild(GNEEdge* edge);

    /// @brief remove edge child
    void removeEdgeChild(GNEEdge* edge);

    /// @brief get edge chidls
    const std::vector<GNEEdge*>& getEdgeChilds() const;

    /// @}

    /// @name members and functions related to lane childs
    /// @{
    /// @brief add lane child
    void addLaneChild(GNELane* lane);

    /// @brief remove lane child
    void removeLaneChild(GNELane* lane);

    /// @brief get lanes of VSS
    const std::vector<GNELane*>& getLaneChilds() const;

    /// @}

    /// @name members and functions related to shape childs
    /// @{
    /// @brief add shape child
    void addShapeChild(GNEShape* shape);

    /// @brief remove shape child
    void removeShapeChild(GNEShape* shape);

    /// @brief get shapes of VSS
    const std::vector<GNEShape*>& getShapeChilds() const;

    /// @}

    /// @name members and functions related to additional childs
    /// @{
    /// @brief add additional child to this additional
    void addAdditionalChild(GNEAdditional* additional);

    /// @brief remove additional child from this additional
    void removeAdditionalChild(GNEAdditional* additional);

    /// @brief return vector of additionals that have as Parent this edge (For example, Calibrators)
    const std::vector<GNEAdditional*>& getAdditionalChilds() const;

    /// @brief sort childs (used by Rerouters, VSS, TAZs...)
    void sortAdditionalChilds();

    /// @brief check if childs are overlapped (Used by Rerouters)
    bool checkAdditionalChildsOverlapping() const;
    
    /// @}

    /// @name members and functions related to demand element childs
    /// @{
    /// @brief add demand element child to this demand element
    void addDemandElementChild(GNEDemandElement* demandElement);

    /// @brief remove demand element child from this demand element
    void removeDemandElementChild(GNEDemandElement* demandElement);

    /// @brief return vector of demand elements that have as Parent this edge (For example, Calibrators)
    const std::vector<GNEDemandElement*>& getDemandElementChilds() const;

    /// @brief sort childs (used by Rerouters, VSS, TAZs...)
    void sortDemandElementChilds();

    /// @brief check if childs are overlapped (Used by Rerouters)
    bool checkDemandElementChildsOverlapping() const;
    
    /// @}

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateAdditionalParent();

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateDemandElementParent();

protected:
  
    /// @brief struct for pack all variables and functions relative to connections between hierarchical element and their childs
    struct ChildConnections {
        /// @brief constructor
        ChildConnections(GNEHierarchicalElementChilds* hierarchicalElement);

        /// @brief update Connection's geometry
        void update();

        /// @brief draw connections between Parent and childrens
        void draw(GUIGlObjectType parentType) const;

        /// @brief position and rotation of every symbol over lane
        std::vector<std::pair<Position, double> > symbolsPositionAndRotation;

        /// @brief Matrix with the Vertex's positions of connections between parents an their childs
        std::vector<PositionVector> connectionPositions;

    private:
        /// @brief pointer to hierarchical element parent
        GNEHierarchicalElementChilds* myHierarchicalElement;
    };

    /// @brief vector with the edge childs of this element
    std::vector<GNEEdge*> myEdgeChilds;

    /// @brief vector with the lane childs of this element
    std::vector<GNELane*> myLaneChilds;

    /// @brief vector with the lane childs of this element
    std::vector<GNEShape*> myShapeChilds;

    /// @brief vector with the additional childs
    std::vector<GNEAdditional*> myAdditionalChilds;

    /// @brief vector with the demand elements childs
    std::vector<GNEDemandElement*> myDemandElementChilds;

    /// @brief variable ChildConnections
    ChildConnections myChildConnections;

private:
    /// @brief pointer to AC (needed to avoid diamond problem)
    GNEAttributeCarrier* myAC;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalElementChilds(const GNEHierarchicalElementChilds&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalElementChilds& operator=(const GNEHierarchicalElementChilds&) = delete;
};

#endif
