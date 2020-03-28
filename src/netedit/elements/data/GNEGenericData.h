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
/// @file    GNEGenericData.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data sets
/****************************************************************************/
#pragma once


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/elements/GNEHierarchicalParentElements.h>
#include <netedit/elements/GNEHierarchicalChildElements.h>
#include <netedit/GNEGeometry.h>
#include <utils/common/Parameterised.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <netbuild/NBVehicle.h>
#include <netbuild/NBEdge.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEDataInterval;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEGenericData
 * @brief An Element which don't belongs to GNENet but has influency in the simulation
 */
class GNEGenericData : public GUIGlObject, public GNEAttributeCarrier, public Parameterised, public GNEHierarchicalParentElements, public GNEHierarchicalChildElements {

public:
    /**@brief Constructor
     * @param[in] tag generic data Tag (edgeData, laneData, etc.)
     * @param[in] GLType GUIGlObjectType associated to this Generic Data
     * @param[in] dataIntervalParent pointer to data interval parent
     * @param[in] parameters parameters map
     * @param[in] edgeParents vector of edge parents
     * @param[in] laneParents vector of lane parents
     * @param[in] shapeParents vector of shape parents
     * @param[in] additionalParents vector of additional parents
     * @param[in] demandElementParents vector of demand element parents
     * @param[in] genericDataParents vector of generic data parents
     * @param[in] edgeChildren vector of edge children
     * @param[in] laneChildren vector of lane children
     * @param[in] shapeChildren vector of shape children
     * @param[in] additionalChildren vector of additional children
     * @param[in] demandElementChildren vector of demandElement children
     * @param[in] genericDataChildren vector of genericData children
     */
    GNEGenericData(const SumoXMLTag tag, const GUIGlObjectType type, GNEDataInterval* dataIntervalParent,
        const std::map<std::string, std::string>& parameters,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren);

    /// @brief Destructor
    virtual ~GNEGenericData();

    /// @brief get ID
    const std::string& getID() const;

    /// @brief get data interval parent
    GNEDataInterval* getDataIntervalParent() const;

    /// @brief get generic data color
    const RGBColor &getColor() const;

    /// @brief check if current generic data is visible
    bool isGenericDataVisible() const;

    // @brief draw attribute
    void drawAttribute(const PositionVector &shape) const;

    /// @brief Returns a pointer to GNEViewNet in which data set element is located
    GNEViewNet* getViewNet() const;

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief update dotted contour
    virtual void updateDottedContour() = 0;

    /// @brief Returns element position in view
    virtual Position getPositionInView() const = 0;

    /// @name members and functions relative to write data sets into XML
    /// @{
    /**@brief writte data set element into a xml file
     * @param[in] device device in which write parameters of data set element
     */
    virtual void writeGenericData(OutputDevice& device) const = 0;

    /// @brief check if current data set is valid to be writed into XML (by default true, can be reimplemented in children)
    virtual bool isGenericDataValid() const;

    /// @brief return a string with the current data set problem (by default empty, can be reimplemented in children)
    virtual std::string getGenericDataProblem() const;

    /// @brief fix data set problem (by default throw an exception, has to be reimplemented in children)
    virtual void fixGenericDataProblem();
    /// @}

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
    void drawGL(const GUIVisualizationSettings& s) const;

    //// @brief Returns the boundary to which the view shall be centered in order to show the object
    virtual Boundary getCenteringBoundary() const = 0;
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
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;

    /* @brief method for enable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    virtual void enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) = 0;

    /* @brief method for disable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    virtual void disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) = 0;

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    virtual bool isAttributeEnabled(SumoXMLAttr key) const = 0;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    virtual std::string getPopUpID() const = 0;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    virtual std::string getHierarchyName() const = 0;
    /// @}

protected:
    /// @brief dataInterval Parent
    GNEDataInterval* myDataIntervalParent;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief method for enabling the attribute and nothing else (used in GNEChange_EnableAttribute)
    virtual void setEnabledAttribute(const int enabledAttributes) = 0;

    /// @brief check if current generic data is visible (must be implemented in all generic data childs)
    virtual bool isVisible() const = 0;

    /// @brief get specific generic data color
    virtual const RGBColor& getSpecificColor() const = 0;

    /// @brief gererate a new ID for an element child (currently unused)
    std::string generateChildID(SumoXMLTag childTag);

    /// @brief Invalidated copy constructor.
    GNEGenericData(const GNEGenericData&) = delete;

    /// @brief Invalidated assignment operator.
    GNEGenericData& operator=(const GNEGenericData&) = delete;
};

/****************************************************************************/

