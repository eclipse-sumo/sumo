/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEShape.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
/// @version $Id$
///
// Abstract class for Shapes uses in netedit
/****************************************************************************/
#ifndef GNEShape_h
#define GNEShape_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


#include <netedit/GNEAttributeCarrier.h>


// ===========================================================================
// class definitions
// ===========================================================================

class GNEShape : public GNEAttributeCarrier {
public:
    /**@brief Constructor.
     * @param[in] net The net to inform about gui updates
     * @param[in] tag sumo xml tag of the element
     * @param[in] movementBlocked if movement of POI is blocked
     */
    GNEShape(GNENet* net, SumoXMLTag tag, bool movementBlocked);

    /// @brief Destructor
    ~GNEShape();

    /**@brief update pre-computed geometry information
    * @note: must be called when geometry changes (i.e. lane moved) and implemented in ALL childrens
    */
    virtual void updateGeometry(bool updateGrid) = 0;

    /**@brief writte shape element into a xml file
    * @param[in] device device in which write parameters of additional element
    */
    virtual void writeShape(OutputDevice& device) = 0;

    /// @brief Returns position of additional in view
    virtual Position getPositionInView() const = 0;

    /// @brief Returns the numerical id of the object
    virtual GUIGlID getGlID() const = 0;

    /// @brief get Net in which this element is placed
    GNENet* getNet() const;

    /// @brief return true if movement is blocked
    bool isMovementBlocked() const;

    /// @brief draw lock icon
    void draw(const Position& pos, double layer, double size = 0.5) const;

    /// @name functions for edit geometry
    /// @{
    /// @brief begin movement (used when user click over edge to start a movement, to avoid problems with problems with GL Tree)
    virtual void startGeometryMoving() = 0;

    /// @brief begin movement (used when user click over edge to start a movement, to avoid problems with problems with GL Tree)
    virtual void endGeometryMoving() = 0;
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

    /// @brief Returns the boundary to which the view shall be centered in order to show the object
    virtual Boundary getCenteringBoundary() const = 0;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /// @brief select attribute carrier using GUIGlobalSelection
    void selectAttributeCarrier(bool changeFlag = true);

    /// @brief unselect attribute carrier using GUIGlobalSelection
    void unselectAttributeCarrier(bool changeFlag = true);

    /// @brief check if attribute carrier is selected
    bool isAttributeCarrierSelected() const;

    /// @brief check if attribute carrier must be drawn using selecting color.
    bool drawUsingSelectColor() const;

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform additional changes
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

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

    /// @name This functions related with generic parameters has to be implemented in all GNEAttributeCarriers
    /// @{

    /// @brief return generic parameters in string format
    virtual std::string getGenericParametersStr() const = 0;

    /// @brief return generic parameters as vector of pairs format
    virtual std::vector<std::pair<std::string, std::string> > getGenericParameters() const = 0;

    /// @brief set generic parameters in string format
    virtual void setGenericParametersStr(const std::string& value) = 0;

    /// @}

protected:
    /// @brief the net to inform about updates
    GNENet* myNet;

    /// @brief boundary used during moving of elements
    Boundary myMovingGeometryBoundary;

    /// @brief flag to block movement
    bool myBlockMovement;

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEShape(const GNEShape&) = delete;

    /// @brief Invalidated assignment operator.
    GNEShape& operator=(const GNEShape&) = delete;
};


#endif

/****************************************************************************/

