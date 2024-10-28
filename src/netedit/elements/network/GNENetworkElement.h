/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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

#include <netedit/elements/GNEHierarchicalElement.h>
#include <utils/gui/div/GUIGeometry.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/PositionVector.h>
#include <netedit/elements/GNEContour.h>
#include <netedit/GNEMoveElement.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNEAdditional;
class GNEDemandElement;


// ===========================================================================
// class definitions
// ===========================================================================

class GNENetworkElement : public GUIGlObject, public GNEHierarchicalElement, public GNEMoveElement {

public:
    /**@brief Constructor.
     * @param[in] net The net to inform about gui updates
     * @param[in] id of the element
     * @param[in] type type of GL object
     * @param[in] tag sumo xml tag of the element
     * @param[in] junctionParents vector of junction parents
     * @param[in] edgeParents vector of edge parents
     * @param[in] laneParents vector of lane parents
     * @param[in] additionalParents vector of additional parents
     * @param[in] demandElementParents vector of demand element parents
     * @param[in] genericDataParents vector of generic data parents
     */
    GNENetworkElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon,
                      const std::vector<GNEJunction*>& junctionParents,
                      const std::vector<GNEEdge*>& edgeParents,
                      const std::vector<GNELane*>& laneParents,
                      const std::vector<GNEAdditional*>& additionalParents,
                      const std::vector<GNEDemandElement*>& demandElementParents,
                      const std::vector<GNEGenericData*>& genericDataParents);

    /// @brief Destructor
    virtual ~GNENetworkElement();

    /**@brief get move operation
    * @note returned GNEMoveOperation can be nullptr
    */
    virtual GNEMoveOperation* getMoveOperation() = 0;

    /// @brief get GUIGlObject associated with this AttributeCarrier
    GUIGlObject* getGUIGlObject();

    /// @brief get GUIGlObject associated with this AttributeCarrier (constant)
    const GUIGlObject* getGUIGlObject() const;

    /// @brief check if current network element is valid to be written into XML (by default true, can be reimplemented in children)
    virtual bool isNetworkElementValid() const;

    /// @brief return a string with the current network element problem (by default empty, can be reimplemented in children)
    virtual std::string getNetworkElementProblem() const;

    /// @name Functions related with geometry of element
    /// @{

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;

    /// @}

    /// @name Function related with contourdrawing (can be implemented in children)
    /// @{

    /// @brief check if draw from contour (green)
    virtual bool checkDrawFromContour() const = 0;

    /// @brief check if draw from contour (magenta)
    virtual bool checkDrawToContour() const = 0;

    /// @brief check if draw related contour (cyan)
    virtual bool checkDrawRelatedContour() const = 0;

    /// @brief check if draw over contour (orange)
    virtual bool checkDrawOverContour() const = 0;

    /// @brief check if draw delete contour (pink/white)
    virtual bool checkDrawDeleteContour() const = 0;

    /// @brief check if draw select contour (blue)
    virtual bool checkDrawSelectContour() const = 0;

    /// @brief check if draw move contour (red)
    virtual bool checkDrawMoveContour() const = 0;

    /// @}

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

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    virtual GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) = 0;

    /// @brief Returns the boundary to which the view shall be centered in order to show the object
    virtual Boundary getCenteringBoundary() const = 0;

    /// @brief update centering boundary (implies change in RTREE)
    virtual void updateCenteringBoundary(const bool updateGrid) = 0;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;

    /// @brief check if element is locked
    bool isGLObjectLocked() const;

    /// @brief mark element as front element
    void markAsFrontElement();

    /// @brief delete element
    virtual void deleteGLObject() = 0;

    /// @brief select element
    void selectGLObject();

    /// @brief Returns the name of the object (default "")
    virtual const std::string getOptionalName() const;

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for getting the Attribute of an XML key in Position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    virtual PositionVector getAttributePositionVector(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
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

    /// @brief straigthen shape edited
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

    /// @brief get parameters map
    virtual const Parameterised::Map& getACParametersMap() const = 0;

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
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNENetworkElement(const GNENetworkElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNENetworkElement& operator=(const GNENetworkElement&) = delete;
};
