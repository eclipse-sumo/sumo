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
/// @file    GNEChange.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// The reification of a NETEDIT editing operation (see command pattern)
// inherits from FXCommand and is used to for undo/redo
/****************************************************************************/
#pragma once
#include <config.h>

#include <fx.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBNode.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/geom/PositionVector.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAttributeCarrier;
class GNEAdditional;
class GNEDataSet;
class GNEDemandElement;
class GNEGenericData;
class GNEDataInterval;
class GNEEdge;
class GNELane;
class GNEShape;
class GNENet;
class GNEViewNet;
class GNEHierarchicalChildElements;
class GNEHierarchicalParentElements;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange
 * @brief the function-object for an editing operation (abstract base)
 */
class GNEChange : public FXCommand {
    FXDECLARE_ABSTRACT(GNEChange)

public:
    /**@brief Constructor
     * @param[in] forward The direction of this change
     */
    GNEChange(bool forward);

    /**@brief Constructor
     * @param[in] net GNENet net on which to apply changes
     * @param[in] forward The direction of this change
     */
    GNEChange(GNENet* net, bool forward);

    /**@brief Constructor
     * @param[in] net GNENet net on which to apply changes
     * @param[in] forward The direction of this change
     * @param[in] parents hierarchical parent elements
     * @param[in] children hierarchical children elements
     */
    GNEChange(GNENet* net, GNEHierarchicalParentElements* parents, GNEHierarchicalChildElements* children, bool forward);

    /// @brief Destructor
    ~GNEChange();

    /// @brief return actual size
    virtual FXuint size() const;

    /// @brief return undoName
    virtual FXString undoName() const;

    /// @brief return rendoName
    virtual FXString redoName() const;

    /// @brief undo action/operation
    virtual void undo();

    /// @brief redo action/operation
    virtual void redo();

protected:
    /// @brief add given edge into parents and children
    void addEdge(GNEEdge* edge);

    /// @brief remove given edge from parents and children
    void removeEdge(GNEEdge* edge);

    /// @brief add given lane into parents and children
    void addLane(GNELane* lane);

    /// @brief remove given lane from parents and children
    void removeLane(GNELane* lane);

    /// @brief add given shape into parents and children
    void addShape(GNEShape* shape);

    /// @brief remove given shape from parents and children
    void removeShape(GNEShape* shape);

    /// @brief add given additional into parents and children
    void addAdditional(GNEAdditional* additional);

    /// @brief remove given additional from parents and children
    void removeAdditional(GNEAdditional* additional);

    /// @brief add given demand element into parents and children
    void addDemandElement(GNEDemandElement* demandElement);

    /// @brief remove given demand element from parents and children
    void removeDemandElement(GNEDemandElement* demandElement);

    /// @brief add given generic data into parents and children
    void addGenericData(GNEGenericData* genericData);

    /// @brief remove given generic data from parents and children
    void removeGenericData(GNEGenericData* genericData);

    /**@brief the net to which operations shall be applied or which shall be
     * informed about gui updates
     * (we are not responsible for the pointer)
     */
    GNENet* myNet;

    /**@brief we group antagonistic commands (create junction/delete
     * junction) and keep them apart by this flag
     */
    bool myForward;

    /// @brief reference to vector of parent edges
    const std::vector<GNEEdge*>& myParentEdges;

    /// @brief reference to vector of parent lanes
    const std::vector<GNELane*>& myParentLanes;

    /// @brief reference to vector of parent shapes
    const std::vector<GNEShape*>& myParentShapes;

    /// @brief reference to vector of parent additionals
    const std::vector<GNEAdditional*>& myParentAdditionals;

    /// @brief reference to vector of parent demand elements
    const std::vector<GNEDemandElement*>& myParentDemandElements;

    /// @brief reference to vector of parent generic datas
    const std::vector<GNEGenericData*>& myParentGenericDatas;

    /// @brief reference to vector of child edges
    const std::vector<GNEEdge*>& myChildEdges;

    /// @brief reference to vector of child lanes
    const std::vector<GNELane*>& myChildLanes;

    /// @brief reference to vector of child shapes
    const std::vector<GNEShape*>& myChildShapes;

    /// @brief reference to vector of child additional
    const std::vector<GNEAdditional*>& myChildAdditionals;

    /// @brief reference to vector of child demand elements
    const std::vector<GNEDemandElement*>& myChildDemandElements;

    /// @brief reference to vector of child generic datas
    const std::vector<GNEGenericData*>& myChildGenericDatas;

private:
    /// @brief vector of empty edges
    static const std::vector<GNEEdge*> myEmptyEdges;

    /// @brief vector of empty lanes
    static const std::vector<GNELane*> myEmptyLanes;

    /// @brief vector of empty shapes
    static const std::vector<GNEShape*> myEmptyShapes;

    /// @brief vector of empty additionals
    static const std::vector<GNEAdditional*> myEmptyAdditionals;

    /// @brief vector of empty demand elements
    static const std::vector<GNEDemandElement*> myEmptyDemandElements;

    /// @brief vector of empty generic datas
    static const std::vector<GNEGenericData*> myEmptyGenericDatas;
};
