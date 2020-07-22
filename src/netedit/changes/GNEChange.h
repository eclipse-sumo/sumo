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
#include <netedit/elements/GNEHierarchicalElementHelper.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEShape.h>
#include <netedit/elements/additional/GNETAZElement.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/data/GNEEdgeData.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/geom/PositionVector.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNEHierarchicalElement;
class GNEAttributeCarrier;
class GNEAdditional;
class GNEDataSet;
class GNEDemandElement;
class GNEGenericData;
class GNEDataInterval;
class GNEEdge;
class GNELane;
class GNEShape;
class GNETAZElement;
class GNENet;
class GNEViewNet;

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
     * @param[in] selectedElement flag to mark if element is selected
     */
    GNEChange(bool forward, const bool selectedElement);

    /**@brief Constructor
     * @param[in] element hierarchical element
     * @param[in] forward The direction of this change
     * @param[in] selectedElement flag to mark if element is selected
     */
    GNEChange(GNEHierarchicalElement* element, bool forward, const bool selectedElement);

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
    /// @brief add given element into parents and children
    template<typename T>
    void addElementInParentsAndChildren(T* element) {
        // add element in parents
        for (const auto& edge : myContainer.parentEdges) {
            edge->addChildElement(element);
        }
        for (const auto& lane : myContainer.parentLanes) {
            lane->addChildElement(element);
        }
        for (const auto& additional : myContainer.parentAdditionals) {
            additional->addChildElement(element);
        }
        for (const auto& shape : myContainer.parentShapes) {
            shape->addChildElement(element);
        }
        for (const auto& TAZElement : myContainer.parentTAZElements) {
            TAZElement->addChildElement(element);
        }
        for (const auto& demandElement : myContainer.parentDemandElements) {
            demandElement->addChildElement(element);
        }
        for (const auto& genericData : myContainer.parentGenericDatas) {
            genericData->addChildElement(element);
        }
        // add element in children
        for (const auto& edge : myContainer.childEdges) {
            edge->addParentElement(element);
        }
        for (const auto& lane : myContainer.childLanes) {
            lane->addParentElement(element);
        }
        for (const auto& additional : myContainer.childAdditionals) {
            additional->addParentElement(element);
        }
        for (const auto& shape : myContainer.childShapes) {
            shape->addParentElement(element);
        }
        for (const auto& TAZElement : myContainer.childTAZElements) {
            TAZElement->addParentElement(element);
        }
        for (const auto& demandElement : myContainer.childDemandElements) {
            demandElement->addParentElement(element);
        }
        for (const auto& genericData : myContainer.childGenericDatas) {
            genericData->addParentElement(element);
        }
    }

    template<typename T>
    void removeElementFromParentsAndChildren(T* element) {
        // Remove element from parents
        for (const auto& edge : myContainer.parentEdges) {
            edge->removeChildElement(element);
        }
        for (const auto& lane : myContainer.parentLanes) {
            lane->removeChildElement(element);
        }
        for (const auto& additional : myContainer.parentAdditionals) {
            additional->removeChildElement(element);
        }
        for (const auto& shape : myContainer.parentShapes) {
            shape->removeChildElement(element);
        }
        for (const auto& TAZElement : myContainer.parentTAZElements) {
            TAZElement->removeChildElement(element);
        }
        for (const auto& demandElement : myContainer.parentDemandElements) {
            demandElement->removeChildElement(element);
        }
        for (const auto& genericData : myContainer.parentGenericDatas) {
            genericData->removeChildElement(element);
        }
        // Remove element from children
        for (const auto& edge : myContainer.childEdges) {
            edge->removeParentElement(element);
        }
        for (const auto& lane : myContainer.childLanes) {
            lane->removeParentElement(element);
        }
        for (const auto& additional : myContainer.childAdditionals) {
            additional->removeParentElement(element);
        }
        for (const auto& shape : myContainer.childShapes) {
            shape->removeParentElement(element);
        }
        for (const auto& TAZElement : myContainer.childTAZElements) {
            TAZElement->removeParentElement(element);
        }
        for (const auto& demandElement : myContainer.childDemandElements) {
            demandElement->removeParentElement(element);
        }
        for (const auto& genericData : myContainer.childGenericDatas) {
            genericData->removeParentElement(element);
        }
    }

    /**@brief we group antagonistic commands (create junction/delete
     * junction) and keep them apart by this flag
     */
    bool myForward;

    /// @brief flag for check if element is selected
    const bool mySelectedElement;
    
    /// @brief container with parent and children
    GNEHierarchicalElementHelper::Container myContainer;
};
