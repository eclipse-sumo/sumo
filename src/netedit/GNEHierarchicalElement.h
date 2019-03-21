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

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditionalElement;
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
    * @param[in] singleAdditionalParent pointer to single additional parent
    */
    GNEHierarchicalElement(SumoXMLTag tag, GNEAdditional* singleAdditionalParent);

    /**@brief Constructor
    * @param[in] tag Type of xml tag that define the element (SUMO_TAG_BUS_STOP, SUMO_TAG_JUNCTION, etc...)
    * @param[in] firstAdditionalParent pointer to first additional parent
    * @param[in] secondAdditionalParent pointer to second additional parent
    */
    GNEHierarchicalElement(SumoXMLTag tag, GNEAdditional* firstAdditionalParent, GNEAdditional* secondAdditionalParent);

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

    // @brief get first additional parent
    GNEAdditional* getFirstAdditionalParent() const;

    // @brief get second additional parent
    GNEAdditional* getSecondAdditionalParent() const;

    /// @brief gererate a new ID for an additional child
    std::string generateAdditionalChildID(GNEAdditional *additionalParent, SumoXMLTag childTag);

    /// @name members and functions relative to additional's childs
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

    /// @brief add edge child
    void addEdgeChild(GNEEdge* edge);

    /// @brief remove edge child
    void removeEdgeChild(GNEEdge* edge);

    /// @brief get edge chidls
    const std::vector<GNEEdge*>& getEdgeChilds() const;

    /// @brief add lane child
    void addLaneChild(GNELane* lane);

    /// @brief remove lane child
    void removeLaneChild(GNELane* lane);

    /// @brief get lanes of VSS
    const std::vector<GNELane*>& getLaneChilds() const;

    /// @}

protected:
  
    /// @brief struct for pack all variables and functions relative to connections between Additionals and their childs
    struct ChildConnections {
        /// @brief constructor
        ChildConnections(GNEHierarchicalElement* additional);

        /// @brief update Connection's geometry
        void update();

        /// @brief draw connections between Parent and childrens
        void draw() const;

        /// @brief position and rotation of every symbol over lane
        std::vector<std::pair<Position, double> > symbolsPositionAndRotation;

        /// @brief Matrix with the Vertex's positions of connections between parents an their childs
        std::vector<PositionVector> connectionPositions;

    private:
        /// @brief pointer to additional parent
        GNEHierarchicalElement* myAdditional;
    };

    /// @brief pointer to first Additional parent
    GNEAdditional* myFirstAdditionalParent;

    /// @brief pointer to second Additional parent
    GNEAdditional* mySecondAdditionalParent;

    /// @brief vector with the Additional childs
    std::vector<GNEAdditional*> myAdditionalChilds;

    /// @brief vector with the edge childs of this additional
    std::vector<GNEEdge*> myEdgeChilds;

    /// @brief vector with the lane childs of this additional
    std::vector<GNELane*> myLaneChilds;

    /// @brief variable ChildConnections
    ChildConnections myChildConnections;

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
    /// @}

private:
    /// @brief Invalidated copy constructor.
    GNEHierarchicalElement(const GNEHierarchicalElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalElement& operator=(const GNEHierarchicalElement&) = delete;
};

#endif
