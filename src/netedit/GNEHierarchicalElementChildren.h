/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEHierarchicalElementChildren.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// A abstract class for representation of Hierarchical Elements
/****************************************************************************/
#ifndef GNEHierarchicalElementChildren_h
#define GNEHierarchicalElementChildren_h

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
 * @class GNEHierarchicalElementChildren
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEHierarchicalElementChildren {

public:

    /// @brief declare GNEChange_Children as friend class
    friend class GNEChange_Children;

    /**@brief Parameter Constructor
     * @param[in] edgeChildren vector of edge children
     * @param[in] laneChildren vector of lane children
     * @param[in] shapeChildren vector of shape children
     * @param[in] additionalChildren vector of additional children
     * @param[in] demandElementChildren vector of demand element children
     */
    GNEHierarchicalElementChildren(GNEAttributeCarrier* AC,
                                   const std::vector<GNEEdge*>& edgeChildren,
                                   const std::vector<GNELane*>& laneChildren,
                                   const std::vector<GNEShape*>& shapeChildren,
                                   const std::vector<GNEAdditional*>& additionalChildren,
                                   const std::vector<GNEDemandElement*>& demandElementChildren);

    /// @brief Destructor
    ~GNEHierarchicalElementChildren();

    /// @brief get child position calculated in ChildConnections
    const Position& getChildPosition(const GNELane* lane);

    /// @brief get child rotation calculated in ChildConnections
    double getChildRotation(const GNELane* lane);

    /// @brief update child connections
    void updateChildConnections();

    // Draw connections between parent and children
    void drawChildConnections(const GUIVisualizationSettings& s, const GUIGlObjectType GLTypeParent) const;

    /// @brief gererate a new ID for an element child
    virtual std::string generateChildID(SumoXMLTag childTag) = 0;

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @name members and functions related to edge children
    /// @{
    /// @brief add edge child
    void addEdgeChild(GNEEdge* edge);

    /// @brief remove edge child
    void removeEdgeChild(GNEEdge* edge);

    /// @brief get edge chidls
    const std::vector<GNEEdge*>& getEdgeChildren() const;

    /// @}

    /// @name members and functions related to lane children
    /// @{
    /// @brief add lane child
    void addLaneChild(GNELane* lane);

    /// @brief remove lane child
    void removeLaneChild(GNELane* lane);

    /// @brief get lanes of VSS
    const std::vector<GNELane*>& getLaneChildren() const;

    /// @}

    /// @name members and functions related to shape children
    /// @{
    /// @brief add shape child
    void addShapeChild(GNEShape* shape);

    /// @brief remove shape child
    void removeShapeChild(GNEShape* shape);

    /// @brief get shapes of VSS
    const std::vector<GNEShape*>& getShapeChildren() const;

    /// @}

    /// @name members and functions related to additional children
    /// @{
    /// @brief add additional child to this additional
    void addAdditionalChild(GNEAdditional* additional);

    /// @brief remove additional child from this additional
    void removeAdditionalChild(GNEAdditional* additional);

    /// @brief return vector of additionals that have as Parent this edge (For example, Calibrators)
    const std::vector<GNEAdditional*>& getAdditionalChildren() const;

    /// @brief sort children (used by Rerouters, VSS, TAZs...)
    void sortAdditionalChildren();

    /// @brief check if children are overlapped (Used by Rerouters)
    bool checkAdditionalChildrenOverlapping() const;

    /// @}

    /// @name members and functions related to demand element children
    /// @{
    /// @brief add demand element child to this demand element
    void addDemandElementChild(GNEDemandElement* demandElement);

    /// @brief remove demand element child from this demand element
    void removeDemandElementChild(GNEDemandElement* demandElement);

    /// @brief return vector of demand elements that have as Parent this edge (For example, Calibrators)
    const std::vector<GNEDemandElement*>& getDemandElementChildren() const;

    /// @brief return vector of demand elements that have as Parent this edge (For example, Calibrators)
    const std::set<GNEDemandElement*>& getSortedDemandElementChildrenByType(SumoXMLTag tag) const;

    /// @brief sort children (used by Rerouters, VSS, TAZs...)
    void sortDemandElementChildren();

    /// @brief check if children are overlapped (Used by Rerouters)
    bool checkDemandElementChildrenOverlapping() const;

    /// @brief get previous demand element to the given demand element
    GNEDemandElement* getPreviousemandElement(const GNEDemandElement* demandElement) const;

    /// @brief get next demand element to the given demand element
    GNEDemandElement* getNextDemandElement(const GNEDemandElement* demandElement) const;

    /// @}

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateAdditionalParent();

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateDemandElementParent();

protected:

    /// @brief struct for pack all variables and functions relative to connections between hierarchical element and their children
    struct ChildConnections {

    private:
        /// @brief connection geometry
        struct ConnectionGeometry {
            /// @brief default constructor
            ConnectionGeometry();

            /// @brief parameter constructor
            ConnectionGeometry(GNELane* _lane, Position _pos, double _rot);

            /// @brief lane
            GNELane* lane;

            /// @brief position
            Position pos;

            /// @brief rotation
            double rot;
        };

    public:
        /// @brief constructor
        ChildConnections(GNEHierarchicalElementChildren* hierarchicalElement);

        /// @brief update Connection's geometry
        void update();

        /// @brief draw connections between Parent and childrens
        void draw(const GUIVisualizationSettings& s, const GUIGlObjectType parentType) const;

        /// @brief position and rotation of every symbol over lane
        std::vector<ConnectionGeometry> symbolsPositionAndRotation;

        /// @brief Matrix with the Vertex's positions of connections between parents an their children
        std::vector<PositionVector> connectionPositions;

    private:
        /// @brief pointer to hierarchical element parent
        GNEHierarchicalElementChildren* myHierarchicalElement;
    };

    /// @brief change edge children of an additional
    void changeEdgeChildren(GNEAdditional* elementChild, const std::string& newEdgeIDs);

    /// @brief change edge children of an additional
    void changeLaneChildren(GNEAdditional* elementChild, const std::string& newEdgeIDs);

    /// @brief variable ChildConnections
    ChildConnections myChildConnections;

private:
    /// @brief vector with the edge children of this element
    std::vector<GNEEdge*> myEdgeChildren;

    /// @brief vector with the lane children of this element
    std::vector<GNELane*> myLaneChildren;

    /// @brief vector with the lane children of this element
    std::vector<GNEShape*> myShapeChildren;

    /// @brief vector with the additional children
    std::vector<GNEAdditional*> myAdditionalChildren;

    /// @brief vector with the demand elements children
    std::vector<GNEDemandElement*> myDemandElementChildren;

    /// @brief vector with the demand elements children sorted by type and filtered (to avoid duplicated
    std::map<SumoXMLTag, std::set<GNEDemandElement*> > mySortedDemandElementChildrenByType;

    /// @brief pointer to AC (needed to avoid diamond problem)
    GNEAttributeCarrier* myAC;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalElementChildren(const GNEHierarchicalElementChildren&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalElementChildren& operator=(const GNEHierarchicalElementChildren&) = delete;
};

#endif
