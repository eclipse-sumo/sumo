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
/// @file    GNEGenericData.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data sets
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/elements/GNEHierarchicalElement.h>
#include <utils/gui/div/GUIGeometry.h>
#include <netedit/GNEPathManager.h>
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
class GNEGenericData : public GUIGlObject, public Parameterised, public GNEHierarchicalElement, public GNEPathManager::PathElement {

public:
    /**@brief Constructor
     * @param[in] tag generic data Tag (edgeData, laneData, etc.)
     * @param[in] GLType GUIGlObjectType associated to this Generic Data
     * @param[in] dataIntervalParent pointer to data interval parent
     * @param[in] parameters parameters map
     * @param[in] junctionParents vector of junction parents
     * @param[in] edgeParents vector of edge parents
     * @param[in] laneParents vector of lane parents
     * @param[in] additionalParents vector of additional parents
     * @param[in] shapeParents vector of shape parents
     * @param[in] TAZElementParents vector of TAZElement parents
     * @param[in] demandElementParents vector of demand element parents
     * @param[in] genericDataParents vector of generic data parents
     */
    GNEGenericData(const SumoXMLTag tag, const GUIGlObjectType type, GNEDataInterval* dataIntervalParent,
                   const std::map<std::string, std::string>& parameters,
                   const std::vector<GNEJunction*>& junctionParents,
                   const std::vector<GNEEdge*>& edgeParents,
                   const std::vector<GNELane*>& laneParents,
                   const std::vector<GNEAdditional*>& additionalParents,
                   const std::vector<GNEShape*>& shapeParents,
                   const std::vector<GNETAZElement*>& TAZElementParents,
                   const std::vector<GNEDemandElement*>& demandElementParents,
                   const std::vector<GNEGenericData*>& genericDataParents);

    /// @brief Destructor
    virtual ~GNEGenericData();

    /// @brief get generic data color
    virtual const RGBColor& getColor() const = 0;

    /// @brief check if current generic data is visible
    virtual bool isGenericDataVisible() const = 0;

    /// @brief get ID
    const std::string& getID() const;

    /// @brief get GUIGlObject associated with this AttributeCarrier
    GUIGlObject* getGUIGlObject();

    /// @brief get data interval parent
    GNEDataInterval* getDataIntervalParent() const;

    // @brief draw attribute
    void drawAttribute(const PositionVector& shape) const;

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

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
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;

    /// @brief return exaggeration asociated with this GLObject
    virtual double getExaggeration(const GUIVisualizationSettings& s) const = 0;

    //// @brief Returns the boundary to which the view shall be centered in order to show the object
    virtual Boundary getCenteringBoundary() const = 0;

    /// @}

    /// @name inherited from GNEPathManager::PathElement
    /// @{

    /// @brief compute pathElement
    virtual void computePathElement() = 0;

    /**@brief Draws partial object (lane)
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] lane GNELane in which draw partial
     * @param[in] segment PathManager segment (used for segment options)
     * @param[in] offsetFront extra front offset (used for drawing partial gl above other elements)
     */
    virtual void drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const = 0;

    /**@brief Draws partial object (junction)
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] fromLane from GNELane
     * @param[in] toLane to GNELane
     * @param[in] segment PathManager segment (used for segment options)
     * @param[in] offsetFront extra front offset (used for drawing partial gl above other elements)
     */
    virtual void drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment, const double offsetFront) const = 0;

    /// @brief get first path lane
    virtual GNELane* getFirstPathLane() const = 0;

    /// @brief get last path lane
    virtual GNELane* getLastPathLane() const = 0;

    /// @brief get path element depart lane pos
    double getPathElementDepartValue() const;

    /// @brief get path element depart position
    Position getPathElementDepartPos() const;

    /// @brief get path element arrival lane pos
    double getPathElementArrivalValue() const;

    /// @brief get path element arrival position
    Position getPathElementArrivalPos() const;

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

    /* @brief method for check if the value for certain attribute is computed (for example, due a network recomputing)
     * @param[in] key The attribute key
     */
    bool isAttributeComputed(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    virtual std::string getPopUpID() const = 0;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    virtual std::string getHierarchyName() const = 0;
    /// @}

    /// @brief get parameters map
    const std::map<std::string, std::string>& getACParametersMap() const;

protected:
    /// @brief dataInterval Parent
    GNEDataInterval* myDataIntervalParent;

    /// @brief draw filtered attribute
    void drawFilteredAttribute(const GUIVisualizationSettings& s, const PositionVector& laneShape, const std::string& attribute) const;

    /// @brief check if attribute is visible in inspect, delete or select mode
    bool isVisibleInspectDeleteSelect() const;

    /// @brief replace the first parent edge
    void replaceFirstParentEdge(const std::string& value);

    /// @brief replace the last parent edge
    void replaceLastParentEdge(const std::string& value);

    /// @brief replace the first parent TAZElement
    void replaceFirstParentTAZElement(SumoXMLTag tag, const std::string& value);

    /// @brief replace the second parent TAZElement
    void replaceSecondParentTAZElement(SumoXMLTag tag, const std::string& value);

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief method for enable or disable the attribute and nothing else (used in GNEChange_EnableAttribute)
    virtual void toogleAttribute(SumoXMLAttr key, const bool value) = 0;

    /// @brief Invalidated copy constructor.
    GNEGenericData(const GNEGenericData&) = delete;

    /// @brief Invalidated assignment operator.
    GNEGenericData& operator=(const GNEGenericData&) = delete;
};

/****************************************************************************/

