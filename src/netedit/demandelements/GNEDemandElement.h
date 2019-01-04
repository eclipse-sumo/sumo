/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDemandElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
/// @version $Id$
///
// A abstract class for demand elements
/****************************************************************************/
#ifndef GNEDemandElement_h
#define GNEDemandElement_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEAttributeCarrier.h>
#include <utils/gui/globjects/GUIGlObject.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDemandElement : public GUIGlObject, public GNEAttributeCarrier {

public:
    /**@brief Constructor.
     * @param[in] net The net to inform about gui updates
     * @param[in] id of the element
     * @param[in] type type of GL object
     * @param[in] tag sumo xml tag of the element
     */
    GNEDemandElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag);

    /// @brief Destructor
    ~GNEDemandElement();

    /**@brief update pre-computed geometry information
     * @note: must be called when geometry changes (i.e. lane moved) and implemented in ALL childrens
     */
    virtual void updateGeometry(bool updateGrid) = 0;

    /// @brief get Net in which this element is placed
    GNENet* getNet() const;

    /// @brief add demand child to this edge
    void addDemandParent(GNEDemandElement* demand);

    /// @brief remove demand child from this edge
    void removeDemandParent(GNEDemandElement* demand);

    /// @brief add demand child to this edge
    void addDemandChild(GNEDemandElement* demand);

    /// @brief remove demand child from this edge
    void removeDemandChild(GNEDemandElement* demand);

    /// @brief return vector of demands that have as Parameter this edge (For example, Rerouters)
    const std::vector<GNEDemandElement*>& getDemandParents() const;

    /// @brief return vector of demands that have as Parent this edge (For example, Calibrators)
    const std::vector<GNEDemandElement*>& getDemandChilds() const;

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

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform demand changes
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

    /// @brief list of Demand parents of this NetElement
    std::vector<GNEDemandElement*> myDemandParents;

    /// @brief list of Demand Childs of this NetElement
    std::vector<GNEDemandElement*> myDemandChilds;

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief method for check if mouse is over objects
    virtual void mouseOverObject(const GUIVisualizationSettings& s) const = 0;

    /// @brief Invalidated copy constructor.
    GNEDemandElement(const GNEDemandElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElement& operator=(const GNEDemandElement&) = delete;
};


#endif

/****************************************************************************/

