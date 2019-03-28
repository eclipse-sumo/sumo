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
 * @class GNEHierarchicalElementParents
 * @brief An special type of Attribute carrier that owns hierarchical elements
 */
class GNEHierarchicalElementParents {

public:
    /// @brief Default constructor
    GNEHierarchicalElementParents(GNEAttributeCarrier* AC);

    /**@brief Constructor used by elements that have another additionals as parent
     * @param[in] tag Type of xml tag that define the element (SUMO_TAG_BUS_STOP, SUMO_TAG_JUNCTION, etc...)
     * @param[in] additionalParents vector of additional parents
     * @param[in] demandElementParents vector of demand element parents
     */
    GNEHierarchicalElementParents(GNEAttributeCarrier* AC,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEDemandElement*>& demandElementParents);

    /// @brief Destructor
    ~GNEHierarchicalElementParents();

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

    /// @name members and functions relative to demand element parents
    /// @{
    /// @brief add demand element child to this edge
    void addDemandElementParent(GNEDemandElement* demandElement);

    /// @brief remove demand element child from this edge
    void removeDemandElementParent(GNEDemandElement* demandElement);

    /// @brief return vector of demand element that have as Parameter this edge (For example, Routes)
    const std::vector<GNEDemandElement*>& getDemandElementParents() const;

    /// @}

protected:
  
    /// @brief struct for pack all variables and functions relative to connections between hierarchical element and their childs
    struct ParentConnections {
        /// @brief constructor
        ParentConnections(GNEHierarchicalElementParents* hierarchicalElement);

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
        GNEHierarchicalElementParents* myHierarchicalElement;
    };

    /// @brief list of additional parents of this NetElement
    std::vector<GNEAdditional*> myAdditionalParents;

    /// @brief list of demand elements parents of this NetElement
    std::vector<GNEDemandElement*> myDemandElementParents;

    /// @brief variable ParentConnections
    ParentConnections myParentConnections;

    /// @name members and functions relative to changing parents
    /// @{
    /**@brief change additional parent of an additional
     * @throw exception if this additional doesn't have previously a defined Additional parent
     * @throw exception if additional with ID newAdditionalParentID doesn't exist
     */
    void changeAdditionalParent(GNEAdditional *additionalTobeChanged, const std::string& newAdditionalParentID, int additionalParentIndex);

    /**@brief change first demand element parent of demandElement
     * @throw exception if this demand element doesn't have previously a defined DemandElement parent
     * @throw exception if demand element with ID newDemandElementParentID doesn't exist
     */
    void changeDemandElementParent(GNEDemandElement *demandElementTobeChanged, const std::string& newDemandElementParentID, int demandElementParentIndex);

    /// @}

private:
    /// @brief pointer to AC (needed to avoid “diamond problem”)
    GNEAttributeCarrier* myAC;

    /// @brief Invalidated copy constructor.
    GNEHierarchicalElementParents(const GNEHierarchicalElementParents&) = delete;

    /// @brief Invalidated assignment operator.
    GNEHierarchicalElementParents& operator=(const GNEHierarchicalElementParents&) = delete;
};

#endif
