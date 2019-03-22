/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEHierarchicalElement.h
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// A abstract class for representation of Hierarchical Elements
/****************************************************************************/
#ifndef GNEHierarchicalElement_h
#define GNEHierarchicalElement_h

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
 * @class GNEHierarchicalElement
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEHierarchicalElement : public GNEAttributeCarrier {

public:
    /**@brief Default constructor
    * @param[in] tag Type of xml tag that define the element (SUMO_TAG_BUS_STOP, SUMO_TAG_JUNCTION, etc...)
    * @param[in] singleAdditionalParent pointer to single additional parent
    */
    GNEHierarchicalElement(SumoXMLTag tag);

    /**@brief Constructor used by Additionals that have another additional as parent
    * @param[in] tag Type of xml tag that define the element (SUMO_TAG_BUS_STOP, SUMO_TAG_JUNCTION, etc...)
    * @param[in] additionalParents vector of additional parents
    */
    GNEHierarchicalElement(SumoXMLTag tag, const std::vector<GNEAdditional*>& additionalParents);

    /**@brief Constructor
    * @param[in] tag Type of xml tag that define the element (SUMO_TAG_BUS_STOP, SUMO_TAG_JUNCTION, etc...)
    * @param[in] edgeChilds vector of edge childs
    */
    GNEHierarchicalElement(SumoXMLTag tag, const std::vector<GNEEdge*> &edgeChilds);

    /**@brief Constructor
    * @param[in] tag Type of xml tag that define the element (SUMO_TAG_BUS_STOP, SUMO_TAG_JUNCTION, etc...)
    * @param[in] laneChilds vector of lane childs
    */
    GNEHierarchicalElement(SumoXMLTag tag, const std::vector<GNELane*> &laneChilds);

    /// @brief Destructor
    ~GNEHierarchicalElement();

    /// @brief gererate a new ID for an element child
    virtual std::string generateChildID(SumoXMLTag childTag) = 0;

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    virtual void updateGeometry(bool updateGrid) = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @name members and functions relative to additional parents
    /// @{
    /// @brief add additional child to this edge
    void addAdditionalParent(GNEAdditional* additional);

    /// @brief remove additional child from this edge
    void removeAdditionalParent(GNEAdditional* additional);

    /// @brief return vector of additionals that have as Parameter this edge (For example, Rerouters)
    const std::vector<GNEAdditional*>& getAdditionalParents() const;

    /// @}

    /// @name members and functions relative to additional childs
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

    /// @name members and functions relative to demand element parents
    /// @{
    /// @brief add demand element child to this edge
    void addDemandElementParent(GNEDemandElement* demandElement);

    /// @brief remove demand element child from this edge
    void removeDemandElementParent(GNEDemandElement* demandElement);

    /// @brief return vector of demand element that have as Parameter this edge (For example, Routes)
    const std::vector<GNEDemandElement*>& getDemandElementParents() const;

    /// @}

    /// @name members and functions relative to demand element childs
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

    /// @name members and functions relative to edge childs
    /// @{

    /// @brief add edge child
    void addEdgeChild(GNEEdge* edge);

    /// @brief remove edge child
    void removeEdgeChild(GNEEdge* edge);

    /// @brief get edge chidls
    const std::vector<GNEEdge*>& getEdgeChilds() const;

    /// @}

    /// @name members and functions relative to edge childs
    /// @{

    /// @brief add lane child
    void addLaneChild(GNELane* lane);

    /// @brief remove lane child
    void removeLaneChild(GNELane* lane);

    /// @brief get lanes of VSS
    const std::vector<GNELane*>& getLaneChilds() const;

    /// @}

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateAdditionalParent();

    /// @brief update parent after add or remove a child (can be reimplemented, for example used for statistics)
    virtual void updateDemandElementParent();

protected:
  
    /// @brief struct for pack all variables and functions relative to connections between hierarchical element and their childs
    struct ChildConnections {
        /// @brief constructor
        ChildConnections(GNEHierarchicalElement* hierarchicalElement);

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
        GNEHierarchicalElement* myHierarchicalElement;
    };

    /// @brief list of additional parents of this NetElement
    std::vector<GNEAdditional*> myAdditionalParents;

    /// @brief vector with the additional childs
    std::vector<GNEAdditional*> myAdditionalChilds;

    /// @brief list of demand elements parents of this NetElement
    std::vector<GNEDemandElement*> myDemandElementParents;

    /// @brief vector with the demand elements childs
    std::vector<GNEDemandElement*> myDemandElementChilds;

    /// @brief vector with the edge childs of this additional
    std::vector<GNEEdge*> myEdgeChilds;

    /// @brief vector with the lane childs of this additional
    std::vector<GNELane*> myLaneChilds;

    /// @brief variable ChildConnections
    ChildConnections myChildConnections;

    /// @name members and functions relative to changing parents
    /// @{
    /**@brief change first additional parent of additional
    * @throw exception if this additional doesn't have previously a defined Additional parent
    * @throw exception if additional with ID newAdditionalParentID doesn't exist
    */
    void changeFirstAdditionalParent(GNEAdditional *additionalTobeChanged, const std::string& newAdditionalParentID);

    /**@brief change second additional parent of additional
    * @throw exception if this additional doesn't have previously a defined Additional parent
    * @throw exception if additional with ID newAdditionalParentID doesn't exist
    */
    void changeSecondAdditionalParent(GNEAdditional *additionalTobeChanged, const std::string& newAdditionalParentID);

    /**@brief change first demand element parent of demandElement
    * @throw exception if this demand element doesn't have previously a defined DemandElement parent
    * @throw exception if demand element with ID newDemandElementParentID doesn't exist
    */
    void changeDemandElementParent(GNEDemandElement *demandElementTobeChanged, const std::string& newDemandElementParentID);

    /// @}

private:
    /// @brief Invalidated copy constructor.
    GNEHierarchicalElement(const GNEHierarchicalElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalElement& operator=(const GNEHierarchicalElement&) = delete;
};

#endif
