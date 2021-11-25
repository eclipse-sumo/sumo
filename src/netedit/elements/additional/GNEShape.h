/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEShape.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
///
// Abstract class for Shapes uses in netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/GNEMoveElement.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/GNEHierarchicalElement.h>
#include <utils/gui/globjects/GUIGlObject.h>


// ===========================================================================
// class definitions
// ===========================================================================

class GNEShape : public GUIGlObject, public GNEHierarchicalElement, public GNEMoveElement {

public:
    /**@brief Constructor.
     * @param[in] id Gl-id of the shape element (Must be unique)
     * @param[in] net pointer to GNENet of this shape element belongs
     * @param[in] type GUIGlObjectType of shape
     * @param[in] tag Type of xml tag that define the shape element
     * @param[in] net The net to inform about gui updates
     * @param[in] tag sumo xml tag of the element
     * @param[in] parameters generic parameters
     * @param[in] movementBlocked if movement of POI is blocked
     * @param[in] junctionParents vector of junction parents
     * @param[in] edgeParents vector of edge parents
     * @param[in] laneParents vector of lane parents
     * @param[in] additionalParents vector of additional parents
     * @param[in] shapeParents vector of shape parents
     * @param[in] TAZElementParents vector of TAZElement parents
     * @param[in] demandElementParents vector of demand element parents
     * @param[in] genericDataParents vector of generic data parents
     * @param[in] parameters generic parameters
     */
    GNEShape(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag,
             const std::vector<GNEJunction*>& junctionParents,
             const std::vector<GNEEdge*>& edgeParents,
             const std::vector<GNELane*>& laneParents,
             const std::vector<GNEAdditional*>& additionalParents,
             const std::vector<GNEShape*>& shapeParents,
             const std::vector<GNETAZElement*>& TAZElementParents,
             const std::vector<GNEDemandElement*>& demandElementParents,
             const std::vector<GNEGenericData*>& genericDataParents);

    /// @brief Destructor
    ~GNEShape();

    /**@brief get move operation
    * @note returned GNEMoveOperation can be nullptr
    */
    virtual GNEMoveOperation* getMoveOperation() = 0;

    /// @brief remove geometry point in the clicked position
    virtual void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) = 0;

    /// @brief get ID (all shapes have one)
    const std::string& getID() const;

    /// @brief get GUIGlObject associated with this AttributeCarrier
    GUIGlObject* getGUIGlObject();

    /// @brief gererate a new ID for an element child
    virtual std::string generateChildID(SumoXMLTag childTag) = 0;

    /**@brief Sets a parameter
     * @param[in] key The parameter's name
     * @param[in] value The parameter's value
     */
    virtual void setParameter(const std::string& key, const std::string& value) = 0;

    /**@brief writte shape element into a xml file
     * @param[in] device device in which write parameters of shape element
     */
    virtual void writeShape(OutputDevice& device) = 0;

    /// @brief Returns the numerical id of the object
    virtual GUIGlID getGlID() const = 0;

    /// @name functions for edit geometry
    /// @{

    /**@brief update pre-computed geometry information
    * @note: must be called when geometry changes (i.e. lane moved) and implemented in ALL childrens
    */
    virtual void updateGeometry() = 0;

    /// @brief Returns position of shape in view
    virtual Position getPositionInView() const = 0;

    /// @brief return exaggeration asociated with this GLObject
    virtual double getExaggeration(const GUIVisualizationSettings& s) const = 0;

    /// @brief Returns the boundary to which the view shall be centered in order to show the object
    Boundary getCenteringBoundary() const;

    /// @brief update centering boundary (implies change in RTREE)
    virtual void updateCenteringBoundary(const bool updateGrid) = 0;

    /// @}

    /// @name inherited from GUIPolygon/GUIPointOfInterest
    /// @{

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    virtual GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) = 0;

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    virtual GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) = 0;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform shape changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;

    /* @brief method for enable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    void enableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for enable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    void disableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    virtual bool isAttributeEnabled(SumoXMLAttr key) const = 0;

    /* @brief method for check if the value for certain attribute is computed (for example, due a network recomputing)
     * @param[in] key The attribute key
     */
    bool isAttributeComputed(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

    /// @brief get parameters map
    virtual const std::map<std::string, std::string>& getACParametersMap() const = 0;

protected:
    /// @brief object boundary
    Boundary myBoundary;

    /// @brief replace shape parent lanes
    void replaceShapeParentLanes(const std::string& value);

    /// @brief shift lane index
    void shiftLaneIndex();

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief set move shape
    virtual void setMoveShape(const GNEMoveResult& moveResult) = 0;

    /// @brief commit move shape
    virtual void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) = 0;

    /// @brief method for enable or disable the attribute and nothing else (used in GNEChange_EnableAttribute)
    void toogleAttribute(SumoXMLAttr key, const bool value);

    /// @brief Invalidated copy constructor.
    GNEShape(const GNEShape&) = delete;

    /// @brief Invalidated assignment operator.
    GNEShape& operator=(const GNEShape&) = delete;
};
