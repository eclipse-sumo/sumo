/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNENetElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// A abstract class for net elements
/****************************************************************************/
#ifndef GNENetElement_h
#define GNENetElement_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEHierarchicalElementParents.h>
#include <netedit/GNEHierarchicalElementChildren.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/PositionVector.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;
class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================

class GNENetElement : public GUIGlObject, public GNEAttributeCarrier, public GNEHierarchicalElementParents, public GNEHierarchicalElementChildren {

public:
    /// @brief struct for pack all variables related with geometry of elemement
    struct NetElementGeometry {
        /// @brief constructor
        NetElementGeometry();

        /// @brief reset geometry
        void clearGeometry();

        /// @brief calculate shape rotations and lengths
        void calculateShapeRotationsAndLengths();

        /// @brief The shape of the netElement element
        PositionVector shape;

        /// @brief The rotations of the single shape parts
        std::vector<double> shapeRotations;

        /// @brief The lengths of the single shape parts
        std::vector<double> shapeLengths;
    };

    /**@brief Constructor.
     * @param[in] net The net to inform about gui updates
     * @param[in] id of the element
     * @param[in] type type of GL object
     * @param[in] tag sumo xml tag of the element
     */
    GNENetElement(GNENet* net, const std::string& id, GUIGlObjectType type, SumoXMLTag tag);

    /// @brief Destructor
    ~GNENetElement();

    /// @brief gererate a new ID for an element child
    virtual std::string generateChildID(SumoXMLTag childTag) = 0;

    /// @name Functions related with geometry of element
    /// @{
    /// @brief get NetElementGeometry
    const NetElementGeometry& getGeometry() const;

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of hierarchical element in view
    virtual Position getPositionInView() const = 0;
    /// @}

    /// @brief get Net in which this element is placed
    GNENet* getNet() const;

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

    /* @brief method for enabling attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note only certain attributes can be enabled, and can produce the disabling of other attributes
     */
    void enableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

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

    /// @brief netElement geometry
    NetElementGeometry myGeometry;

    /// @brief boundary used during moving of elements
    Boundary myMovingGeometryBoundary;

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief method for enabling the attribute and nothing else (used in GNEChange_EnableAttribute)
    void setEnabledAttribute(const int enabledAttributes);

    /// @brief Invalidated copy constructor.
    GNENetElement(const GNENetElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNENetElement& operator=(const GNENetElement&) = delete;
};


#endif

/****************************************************************************/

