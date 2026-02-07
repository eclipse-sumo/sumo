/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
#include <netedit/GNEViewNet.h>
#include <netedit/elements/GNEHierarchicalStructureParents.h>
#include <netedit/elements/additional/GNETAZSourceSink.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/elements/demand/GNEDemandElement.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNELane.h>
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

    /// @brief return redoName
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

    /// @brief add given element in parents and children
    template<typename T>
    void addElementInParentsAndChildren(T* element) {
        // add element in children
        for (const auto& junction : myParents.get<GNEJunction*>()) {
            GNEHierarchicalElement::insertChild(junction, element);
        }
        for (const auto& edge : myParents.get<GNEEdge*>()) {
            GNEHierarchicalElement::insertChild(edge, element);
        }
        for (const auto& lane : myParents.get<GNELane*>()) {
            GNEHierarchicalElement::insertChild(lane, element);
        }
        for (const auto& additional : myParents.get<GNEAdditional*>()) {
            GNEHierarchicalElement::insertChild(additional, element);
        }
        for (const auto& sourceSink : myParents.get<GNETAZSourceSink*>()) {
            GNEHierarchicalElement::insertChild(sourceSink, element);
        }
        for (const auto& demandElement : myParents.get<GNEDemandElement*>()) {
            GNEHierarchicalElement::insertChild(demandElement, element);
        }
        for (const auto& genericData : myParents.get<GNEGenericData*>()) {
            GNEHierarchicalElement::insertChild(genericData, element);
        }
    }

    /// @brief remove given element from parents and children
    template<typename T>
    void removeElementFromParentsAndChildren(T* element) {
        // Remove element from parents
        for (const auto& junction : myParents.get<GNEJunction*>()) {
            GNEHierarchicalElement::removeChild(junction, element);
        }
        for (const auto& edge : myParents.get<GNEEdge*>()) {
            GNEHierarchicalElement::removeChild(edge, element);
        }
        for (const auto& lane : myParents.get<GNELane*>()) {
            GNEHierarchicalElement::removeChild(lane, element);
        }
        for (const auto& additional : myParents.get<GNEAdditional*>()) {
            GNEHierarchicalElement::removeChild(additional, element);
        }
        for (const auto& sourceSink : myParents.get<GNETAZSourceSink*>()) {
            GNEHierarchicalElement::removeChild(sourceSink, element);
        }
        for (const auto& demandElement : myParents.get<GNEDemandElement*>()) {
            GNEHierarchicalElement::removeChild(demandElement, element);
        }
        for (const auto& genericData : myParents.get<GNEGenericData*>()) {
            GNEHierarchicalElement::removeChild(genericData, element);
        }
    }

    /// @brief supermode related with this change
    const Supermode mySupermode;

    /// @brief Hierarchical container with parents
    const GNEHierarchicalStructureParents myParents;

    /// @brief we group antagonistic commands (create junction/delete junction) and keep them apart by this flag
    bool myForward;

    /// @brief flag for check if element is selected
    const bool mySelectedElement;

private:
    // @brief next GNEChange (can be access by GNEChangeGroup and GNEUndoList)
    GNEChange* next;

    /// @brief Invalidated copy constructor.
    GNEChange(const GNEChange&) = delete;

    /// @brief Invalidated assignment operator.
    GNEChange& operator=(const GNEChange&) = delete;
};
