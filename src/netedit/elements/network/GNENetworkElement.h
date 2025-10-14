/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNENetworkElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
// A abstract class for network elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/GNEContour.h>
#include <netedit/elements/GNEHierarchicalElement.h>
#include <utils/gui/globjects/GUIGlObject.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEMoveOperation;

// ===========================================================================
// class definitions
// ===========================================================================

class GNENetworkElement : public GNEAttributeCarrier, public GUIGlObject, public GNEHierarchicalElement {

public:
    /**@brief Constructor.
     * @param[in] net The net to inform about gui updates
     * @param[in] id of the element
     * @param[in] tag sumo xml tag of the element
     */
    GNENetworkElement(GNENet* net, const std::string& id, SumoXMLTag tag);

    /// @brief Destructor
    virtual ~GNENetworkElement();

    /// @brief get GNEHierarchicalElement associated with this AttributeCarrier
    GNEHierarchicalElement* getHierarchicalElement();

    /// @brief get GUIGlObject associated with this AttributeCarrier
    GUIGlObject* getGUIGlObject() override;

    /// @brief get GUIGlObject associated with this AttributeCarrier (constant)
    const GUIGlObject* getGUIGlObject() const override;

    /// @brief check if current network element is valid to be written into XML (by default true, can be reimplemented in children)
    virtual bool isNetworkElementValid() const;

    /// @brief return a string with the current network element problem (by default empty, can be reimplemented in children)
    virtual std::string getNetworkElementProblem() const;

    /// @name inherited from GUIGlObject
    /// @{

    /**@brief Returns an own parameter window
    *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief check if element is locked
    bool isGLObjectLocked() const;

    /// @brief mark element as front element
    void markAsFrontElement();

    /// @brief select element
    void selectGLObject();

    /// @brief Returns the name of the object (default "")
    virtual const std::string getOptionalName() const;

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const override;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const override;

    /// @}

    /// @name functions related with shape editing
    /// @{

    /// @brief set shape edited
    void setShapeEdited(const bool value);

    /// @brief check if shape is being edited
    bool isShapeEdited() const;

    /// @brief get index geometry point under cursor of shape edited
    int getGeometryPointUnderCursorShapeEdited() const;

    /// @brief simplify shape edited
    void simplifyShapeEdited(GNEUndoList* undoList);

    /// @brief straighten shape edited
    void straigthenShapeEdited(GNEUndoList* undoList);

    /// @brief close shape edited
    void closeShapeEdited(GNEUndoList* undoList);

    /// @brief open shape edited
    void openShapeEdited(GNEUndoList* undoList);

    /// @brief set first geometry point shape edited
    void setFirstGeometryPointShapeEdited(const int index, GNEUndoList* undoList);

    /// @brief delete geometry point shape edited
    void deleteGeometryPointShapeEdited(const int index, GNEUndoList* undoList);

    /// @brief reset shape edited
    void resetShapeEdited(GNEUndoList* undoList);

    /// @}

    /// @brief set network element id
    void setNetworkElementID(const std::string& newID);

protected:
    /// @brief flag to check if element shape is being edited
    bool myShapeEdited;

    /// @brief network element contour
    GNEContour myNetworkElementContour;

    // @brief check if we're drawing using a boundary but element was already selected
    bool checkDrawingBoundarySelection() const;

    /// @brief get shape edited popup menu
    GUIGLObjectPopupMenu* getShapeEditedPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent, const PositionVector& shape);

    /**@brief return index of a vertex of shape, or of a new vertex if position is over an shape's edge
     * @param pos position of new/existent vertex
     * @return index of position vector
     */
    int getVertexIndex(const PositionVector& shape, const Position& pos);

private:
    /// @brief Invalidated copy constructor.
    GNENetworkElement(const GNENetworkElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNENetworkElement& operator=(const GNENetworkElement&) = delete;
};
