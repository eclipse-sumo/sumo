/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEMeanData.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// Class for representing MeanData
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/elements/GNEHierarchicalElement.h>

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEMeanData
 * @brief An Element which don't belong to GNENet but has influence in the simulation
 */
class GNEMeanData : public GUIGlObject, public GNEHierarchicalElement, public Parameterised {

public:
    /**@brief Constructor
     * @param[in] tag generic data Tag (edgeData, laneData, etc.)
     * @param[in] GLType GUIGlObjectType associated to this Mean Data
     * @param[in] dataIntervalParent pointer to data interval parent
     * @param[in] parameters parameters map
     * @param[in] junctionParents vector of junction parents
     * @param[in] edgeParents vector of edge parents
     * @param[in] laneParents vector of lane parents
     * @param[in] additionalParents vector of additional parents
     * @param[in] demandElementParents vector of demand element parents
     * @param[in] genericDataParents vector of generic data parents
     */
    GNEMeanData(const SumoXMLTag tag, FXIcon* icon, const GUIGlObjectType type, GNEDataInterval* dataIntervalParent,
                   const Parameterised::Map& parameters,
                   const std::vector<GNEJunction*>& junctionParents,
                   const std::vector<GNEEdge*>& edgeParents,
                   const std::vector<GNELane*>& laneParents,
                   const std::vector<GNEAdditional*>& additionalParents,
                   const std::vector<GNEDemandElement*>& demandElementParents,
                   const std::vector<GNEMeanData*>& genericDataParents);

    /// @brief Destructor
    ~GNEMeanData();

    /// @brief get GUIGlObject associated with this AttributeCarrier
    GUIGlObject* getGUIGlObject();

    /// @brief update pre-computed geometry information
    void updateGeometry() {}

    /// @brief Returns element position in view
    Position getPositionInView() const {}

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;

    /// @brief delete element
    void deleteGLObject();

    /// @brief select element
    void selectGLObject();

    /// @brief update GLObject (geometry, ID, etc.)
    void updateGLObject();

    /// @brief return exaggeration associated with this GLObject
    virtual double getExaggeration(const GUIVisualizationSettings& s) const = 0;

    //// @brief Returns the boundary to which the view shall be centered in order to show the object
    virtual Boundary getCenteringBoundary() const = 0;

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    virtual double getAttributeDouble(SumoXMLAttr key) const = 0;

    /**@brief method for setting the attribute and letting the object perform data set changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /**@brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    virtual std::string getPopUpID() const = 0;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    virtual std::string getHierarchyName() const = 0;
    /// @}

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEMeanData(const GNEMeanData&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMeanData& operator=(const GNEMeanData&) = delete;
};

/****************************************************************************/

