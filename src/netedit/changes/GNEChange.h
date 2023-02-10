/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
// The reification of a netedit editing operation (see command pattern)
// inherits from FXCommand and is used to for undo/redo
/****************************************************************************/
#pragma once
#include <config.h>

#include <netbuild/NBEdge.h>
#include <netbuild/NBNode.h>
#include <netedit/elements/GNEHierarchicalContainer.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <utils/foxtools/fxheader.h>
#include <utils/geom/PositionVector.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNEHierarchicalElement;
class GNEAttributeCarrier;
class GNEDataSet;
class GNEDataInterval;
class GNEMeanData;
class GNENet;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange
 * @brief the function-object for an editing operation (abstract base)
 */
class GNEChange : public FXObject {
    FXDECLARE_ABSTRACT(GNEChange)

public:
    /// @name friend class
    friend class GNEChangeGroup;
    friend class GNEUndoList;

    /**@brief Constructor
     * @param[in] supermode related with this change
     * @param[in] forward The direction of this change
     * @param[in] selectedElement flag to mark if element is selected
     */
    GNEChange(Supermode supermode, bool forward, const bool selectedElement);

    /**@brief Constructor
     * @param[in] supermode related with this change
     * @param[in] element hierarchical element
     * @param[in] forward The direction of this change
     * @param[in] selectedElement flag to mark if element is selected
     */
    GNEChange(Supermode supermode, GNEHierarchicalElement* element, bool forward, const bool selectedElement);

    /// @brief Destructor
    ~GNEChange();

    /// @brief undo action/operation
    virtual void undo() = 0;

    /// @brief redo action/operation
    virtual void redo() = 0;

    /// @brief return undoName
    virtual std::string undoName() const = 0;

    /// @brief return rendoName
    virtual std::string redoName() const = 0;

    /// @brief Return the size of the command group
    virtual int size() const;

    /// @brief get supermode
    Supermode getSupermode() const;

    /**
     * @brief Return TRUE if this command can be merged with previous undo
     * commands.  This is useful to combine e.g. multiple consecutive
     * single-character text changes into a single block change.
     * The default implementation returns FALSE.
     */
    bool canMerge() const;

    /**
     * @brief Called by the undo system to try and merge the new incoming command
     * with this command; should return TRUE if merging was possible.
     * The default implementation returns FALSE.
     */
    bool mergeWith(GNEChange* command);

protected:
    /// @brief FOX need this
    GNEChange();

    /// @brief restore container (only use in undo() function)
    void restoreHierarchicalContainers();

    /// @brief add given element into parents and children (only use in redo() function)
    template<typename T>
    void addElementInParentsAndChildren(T* element) {
        // add element in parents
        for (const auto& junction : myOriginalHierarchicalContainer.getParents<std::vector<GNEJunction*> >()) {
            junction->addChildElement(element);
        }
        for (const auto& edge : myOriginalHierarchicalContainer.getParents<std::vector<GNEEdge*> >()) {
            edge->addChildElement(element);
        }
        for (const auto& lane : myOriginalHierarchicalContainer.getParents<std::vector<GNELane*> >()) {
            lane->addChildElement(element);
        }
        for (const auto& additional : myOriginalHierarchicalContainer.getParents<std::vector<GNEAdditional*> >()) {
            additional->addChildElement(element);
        }
        for (const auto& demandElement : myOriginalHierarchicalContainer.getParents<std::vector<GNEDemandElement*> >()) {
            demandElement->addChildElement(element);
        }
        for (const auto& genericData : myOriginalHierarchicalContainer.getParents<std::vector<GNEGenericData*> >()) {
            genericData->addChildElement(element);
        }
        // add element in children
        for (const auto& junction : myOriginalHierarchicalContainer.getChildren<std::vector<GNEJunction*> >()) {
            junction->addParentElement(element);
        }
        for (const auto& edge : myOriginalHierarchicalContainer.getChildren<std::vector<GNEEdge*> >()) {
            edge->addParentElement(element);
        }
        for (const auto& lane : myOriginalHierarchicalContainer.getChildren<std::vector<GNELane*> >()) {
            lane->addParentElement(element);
        }
        for (const auto& additional : myOriginalHierarchicalContainer.getChildren<std::vector<GNEAdditional*> >()) {
            additional->addParentElement(element);
        }
        for (const auto& demandElement : myOriginalHierarchicalContainer.getChildren<std::vector<GNEDemandElement*> >()) {
            demandElement->addParentElement(element);
        }
        for (const auto& genericData : myOriginalHierarchicalContainer.getChildren<std::vector<GNEGenericData*> >()) {
            genericData->addParentElement(element);
        }
    }

    /// @brief remove given element from parents and children (only use in redo() function)
    template<typename T>
    void removeElementFromParentsAndChildren(T* element) {
        // Remove element from parents
        for (const auto& junction : myOriginalHierarchicalContainer.getParents<std::vector<GNEJunction*> >()) {
            junction->removeChildElement(element);
        }
        for (const auto& edge : myOriginalHierarchicalContainer.getParents<std::vector<GNEEdge*> >()) {
            edge->removeChildElement(element);
        }
        for (const auto& lane : myOriginalHierarchicalContainer.getParents<std::vector<GNELane*> >()) {
            lane->removeChildElement(element);
        }
        for (const auto& additional : myOriginalHierarchicalContainer.getParents<std::vector<GNEAdditional*> >()) {
            additional->removeChildElement(element);
        }
        for (const auto& demandElement : myOriginalHierarchicalContainer.getParents<std::vector<GNEDemandElement*> >()) {
            demandElement->removeChildElement(element);
        }
        for (const auto& genericData : myOriginalHierarchicalContainer.getParents<std::vector<GNEGenericData*> >()) {
            genericData->removeChildElement(element);
        }
        // Remove element from children
        for (const auto& junction : myOriginalHierarchicalContainer.getChildren<std::vector<GNEJunction*> >()) {
            junction->removeParentElement(element);
        }
        for (const auto& edge : myOriginalHierarchicalContainer.getChildren<std::vector<GNEEdge*> >()) {
            edge->removeParentElement(element);
        }
        for (const auto& lane : myOriginalHierarchicalContainer.getChildren<std::vector<GNELane*> >()) {
            lane->removeParentElement(element);
        }
        for (const auto& additional : myOriginalHierarchicalContainer.getChildren<std::vector<GNEAdditional*> >()) {
            additional->removeParentElement(element);
        }
        for (const auto& demandElement : myOriginalHierarchicalContainer.getChildren<std::vector<GNEDemandElement*> >()) {
            demandElement->removeParentElement(element);
        }
        for (const auto& genericData : myOriginalHierarchicalContainer.getChildren<std::vector<GNEGenericData*> >()) {
            genericData->removeParentElement(element);
        }
    }

    /// @brief supermode related with this change
    const Supermode mySupermode;

    /// @brief we group antagonistic commands (create junction/delete junction) and keep them apart by this flag
    bool myForward;

    /// @brief flag for check if element is selected
    const bool mySelectedElement;

    /// @brief Hierarchical container with parent and children
    const GNEHierarchicalContainer myOriginalHierarchicalContainer;

    /// @brief map with hierarchical container of all parent and children elements
    std::map<GNEHierarchicalElement*, GNEHierarchicalContainer> myHierarchicalContainers;

private:
    // @brief next GNEChange (can be access by GNEChangeGroup and GNEUndoList)
    GNEChange* next;

    /// @brief Invalidated copy constructor.
    GNEChange(const GNEChange&) = delete;

    /// @brief Invalidated assignment operator.
    GNEChange& operator=(const GNEChange&) = delete;
};
