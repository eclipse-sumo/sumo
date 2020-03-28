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
/// @file    GNEHierarchicalChildElements.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
///
// A abstract class for representation of Hierarchical Elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/globjects/GUIGlObjectTypes.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;
class GNEDemandElement;
class GNEShape;
class GNEGenericData;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEHierarchicalChildElements
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEHierarchicalChildElements {

public:

    /// @brief declare GNEChange_Children as friend class
    friend class GNEChange_Children;

    /**@brief Parameter Constructor
     * @param[in] childEdges vector of child edges
     * @param[in] childLanes vector of child lanes
     * @param[in] childShapes vector of child shapes
     * @param[in] childAdditionals vector of child additional
     * @param[in] childDemandElements vector of child demand elements
     * @param[in] childGenericDataElements vector of child generic data elements
     */
    GNEHierarchicalChildElements(GNEAttributeCarrier* AC,
                                 const std::vector<GNEEdge*>& childEdges,
                                 const std::vector<GNELane*>& childLanes,
                                 const std::vector<GNEShape*>& childShapes,
                                 const std::vector<GNEAdditional*>& childAdditionals,
                                 const std::vector<GNEDemandElement*>& childDemandElements,
                                 const std::vector<GNEGenericData*>& childGenericDataElements);

    /// @brief Destructor
    ~GNEHierarchicalChildElements();

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

    /// @name members and functions related to child edges
    /// @{
    /// @brief add child edge
    void addChildEdge(GNEEdge* edge);

    /// @brief remove child edge
    void removeChildEdge(GNEEdge* edge);

    /// @brief get child edges
    const std::vector<GNEEdge*>& getChildEdges() const;

    /// @}

    /// @name members and functions related to child lanes
    /// @{
    /// @brief add child lane
    void addChildLane(GNELane* lane);

    /// @brief remove child lane
    void removeChildLane(GNELane* lane);

    /// @brief get child lanes
    const std::vector<GNELane*>& getChildLanes() const;

    /// @}

    /// @name members and functions related to child shapes
    /// @{
    /// @brief add child shape
    void addChildShape(GNEShape* shape);

    /// @brief remove child shape
    void removeChildShape(GNEShape* shape);

    /// @brief get child shapes
    const std::vector<GNEShape*>& getChildShapes() const;

    /// @}

    /// @name members and functions related to child additionals
    /// @{
    /// @brief add child additional
    void addChildAdditional(GNEAdditional* additional);

    /// @brief remove child additional
    void removeChildAdditional(GNEAdditional* additional);

    /// @brief return child additionals
    const std::vector<GNEAdditional*>& getChildAdditionals() const;

    /// @brief get number of child additionals
    size_t getNumberOfChildAdditionals(GNETagProperties::TagType additionalType) const;

    /// @brief sort child additionals (used by Rerouters, VSS, TAZs...)
    void sortChildAdditionals();

    /// @brief check if children are overlapped (Used by Rerouters)
    bool checkChildAdditionalsOverlapping() const;

    /// @}

    /// @name members and functions related to child demand elements
    /// @{
    /// @brief add child demand element
    void addChildDemandElement(GNEDemandElement* demandElement);

    /// @brief remove child demand element
    void removeChildDemandElement(GNEDemandElement* demandElement);

    /// @brief return child demand elements
    const std::vector<GNEDemandElement*>& getChildDemandElements() const;

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

    /// @name members and functions related to child generic data elements
    /// @{
    /// @brief add child generic data element
    void addChildGenericDataElement(GNEGenericData* genericDataElement);

    /// @brief remove child generic data element
    void removeChildGenericDataElement(GNEGenericData* genericDataElement);

    /// @brief return child generic data elements
    const std::vector<GNEGenericData*>& getChildGenericDataElements() const;

    /// @}

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateParentAdditional();

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateParentDemandElement();

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
        ChildConnections(GNEHierarchicalChildElements* hierarchicalElement);

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
        GNEHierarchicalChildElements* myHierarchicalElement;
    };

    /// @brief change child edges of an additional
    void changeChildEdges(GNEAdditional* elementChild, const std::string& newEdgeIDs);

    /// @brief change child edges of an additional
    void changeChildLanes(GNEAdditional* elementChild, const std::string& newEdgeIDs);

    /// @brief variable ChildConnections
    ChildConnections myChildConnections;

private:
    /// @brief vector with the child edges of this element
    std::vector<GNEEdge*> myChildEdges;

    /// @brief vector with the child lanes of this element
    std::vector<GNELane*> myChildLanes;

    /// @brief vector with the child lanes of this element
    std::vector<GNEShape*> myChildShapes;

    /// @brief vector with the child additional
    std::vector<GNEAdditional*> myChildAdditionals;

    /// @brief vector with the demand elements children
    std::vector<GNEDemandElement*> myChildDemandElements;

    /// @brief vector with the generic data elements children
    std::vector<GNEGenericData*> myChildGenericDataElements;

    /// @brief vector with the demand elements children sorted by type and filtered (to avoid duplicated
    std::map<SumoXMLTag, std::vector<GNEDemandElement* >> myDemandElementsByType;

    /// @brief pointer to AC (needed to avoid diamond problem)
    GNEAttributeCarrier* myAC;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalChildElements(const GNEHierarchicalChildElements&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalChildElements& operator=(const GNEHierarchicalChildElements&) = delete;
};

