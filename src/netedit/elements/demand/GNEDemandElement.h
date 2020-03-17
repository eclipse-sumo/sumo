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
/// @file    GNEDemandElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
///
// A abstract class for demand elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEHierarchicalParentElements.h>
#include <netedit/elements/GNEHierarchicalChildElements.h>
#include <netedit/GNEGeometry.h>
#include <utils/common/Parameterised.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEAdditional;
class GNEDemandElement;
class GNENetworkElement;
class GNEGenericData;
class GNEEdge;
class GNELane;
class GNEJunction;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEDemandElement
 * @brief An Element which don't belongs to GNENet but has influency in the simulation
 */
class GNEDemandElement : public GUIGlObject, public GNEAttributeCarrier, public GNEHierarchicalParentElements, public GNEHierarchicalChildElements {

public:
    /// @brief struct for pack all variables related with Demand Element moving
    struct DemandElementMove {
        /// @brief boundary used during moving of elements (to avoid insertion in RTREE)
        Boundary movingGeometryBoundary;

        /// @brief value for saving first original position over lane before moving
        Position originalViewPosition;

        /// @brief value for saving first original position over lane before moving
        std::string firstOriginalLanePosition;

        /// @brief value for saving second original position over lane before moving
        std::string secondOriginalPosition;
    };


    /**@brief Constructor
     * @param[in] id Gl-id of the demand element element (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this demand element element belongs
     * @param[in] type GUIGlObjectType of demand element
     * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_ROUTE, SUMO_TAG_VEHICLE, etc...)
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
    GNEDemandElement(const std::string& id, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
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

    /**@brief Constructor
     * @param[in] demandElementParent pointer to parent demand element pointer (used to generate an ID)
     * @param[in] viewNet pointer to GNEViewNet of this demand element element belongs
     * @param[in] type GUIGlObjectType of demand element
     * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_ROUTE, SUMO_TAG_VEHICLE, etc...)
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
    GNEDemandElement(GNEDemandElement* demandElementParent, GNEViewNet* viewNet, GUIGlObjectType type, SumoXMLTag tag,
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
    virtual ~GNEDemandElement();

    /// @brief get ID
    const std::string& getID() const;

    /// @brief gererate a new ID for an element child
    std::string generateChildID(SumoXMLTag childTag);

    /// @name members and functions relative to demand element (stacked) geometries
    /// @{
    /// @brief get demand element geometry (stacked)
    const GNEGeometry::Geometry& getDemandElementGeometry();

    /// @brief get demand element segment geometry (stacked)
    const GNEGeometry::SegmentGeometry& getDemandElementSegmentGeometry() const;

    /// @brief update element stacked geometry (stacked)
    void updateDemandElementGeometry(const GNELane* lane, const double posOverLane);

    /// @brief update stack label
    void updateDemandElementStackLabel(const int stack);
    /// @}

    /// @name members and functions relative to demand element spread geometries
    /// @{
    /// @brief get demand element segment spread geometry
    const GNEGeometry::SegmentGeometry& getDemandElementSegmentSpreadGeometry() const;

    /// @brief update element spread geometry
    void updateDemandElementSpreadGeometry(const GNELane* lane, const double posOverLane);
    /// @}

    /// @name members and functions relative to elements common to all demand elements
    /// @{
    /// @brief obtain from edge of this demand element
    virtual GNEEdge* getFromEdge() const = 0;

    /// @brief obtain to edge of this demand element
    virtual GNEEdge* getToEdge() const = 0;

    /// @brief obtain VClass related with this demand element
    virtual SUMOVehicleClass getVClass() const = 0;

    /// @brief get color
    virtual const RGBColor& getColor() const = 0;
    /// @}

    /// @name members and functions relative to write demand elements into XML
    /// @{
    /**@brief writte demand element element into a xml file
     * @param[in] device device in which write parameters of demand element element
     */
    virtual void writeDemandElement(OutputDevice& device) const = 0;

    /// @brief check if current demand element is valid to be writed into XML (by default true, can be reimplemented in children)
    virtual bool isDemandElementValid() const;

    /// @brief return a string with the current demand element problem (by default empty, can be reimplemented in children)
    virtual std::string getDemandElementProblem() const;

    /// @brief fix demand element problem (by default throw an exception, has to be reimplemented in children)
    virtual void fixDemandElementProblem();
    /// @}

    /**@brief open DemandElement Dialog
     * @note: if demand element needs an demand element dialog, this function has to be implemented in childrens (see GNERerouter and GNEVariableSpeedSign)
     * @throw invalid argument if demand element doesn't have an demand element Dialog
     */
    virtual void openDemandElementDialog();

    /**@brief get begin time of demand element
     * @note: used by demand elements of type "Vehicle", and it has to be implemented as children
     * @throw invalid argument if demand element doesn't has a begin time
     */
    virtual std::string getBegin() const;

    /// @name Functions related with geometry of element
    /// @{
    /// @brief begin geometry movement
    virtual void startGeometryMoving() = 0;

    /// @brief end geometry movement
    virtual void endGeometryMoving() = 0;

    /**@brief change the position of the element geometry without saving in undoList
     * @param[in] offset Position used for calculate new position of geometry without updating RTree
     */
    virtual void moveGeometry(const Position& offset) = 0;

    /**@brief commit geometry changes in the attributes of an element after use of moveGeometry(...)
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void commitGeometryMoving(GNEUndoList* undoList) = 0;

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief update dotted contour
    virtual void updateDottedContour() = 0;

    /// @brief partial update pre-computed geometry information
    virtual void updatePartialGeometry(const GNEEdge* edge) = 0;

    /// @brief compute path
    virtual void computePath() = 0;

    /// @brief invalidate path
    virtual void invalidatePath() = 0;

    /// @brief Returns position of demand element in view
    virtual Position getPositionInView() const = 0;

    /// @brief split geometry
    virtual void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) = 0;
    /// @}

    /// @brief Returns a pointer to GNEViewNet in which demand element element is located
    GNEViewNet* getViewNet() const;

    /// @name inherited from GUIGlObject
    /// @{

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    virtual GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     * @return The boundary the object is within
     */
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
    virtual void selectAttributeCarrier(bool changeFlag = true) = 0;

    /// @brief unselect attribute carrier using GUIGlobalSelection
    virtual void unselectAttributeCarrier(bool changeFlag = true) = 0;

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

    /**@brief method for setting the attribute and letting the object perform demand element changes
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

    /// @brief get first allowed vehicle lane
    GNELane* getFirstAllowedVehicleLane() const;

    /// @brief get first allowed vehicle lane
    GNELane* getLastAllowedVehicleLane() const;

protected:
    /// @brief The GNEViewNet this demand element element belongs
    GNEViewNet* myViewNet;

    /// @brief demand element geometry (also called "stacked geometry")
    GNEGeometry::Geometry myDemandElementGeometry;

    /// @brief demand element segment geometry (also called "stacked geometry")
    GNEGeometry::SegmentGeometry myDemandElementSegmentGeometry;

    /// @brief demand element spread geometry (Only used by vehicles and pedestrians)
    GNEGeometry::Geometry mySpreadGeometry;

    /// @brief demand element spread segment geometry (Only used by vehicles and pedestrians)
    GNEGeometry::SegmentGeometry mySpreadSegmentGeometry;

    /// @brief stacked label number
    int myStackedLabelNumber;

    /// @name Functions relative to change values in setAttribute(...)
    /// @{

    /// @brief check if a new demand element ID is valid
    bool isValidDemandElementID(const std::string& newID) const;

    /// @}

    /// @brief calculate personPlan start and end positions over lanes
    void calculatePersonPlanLaneStartEndPos(double& startPos, double& endPos) const;

    /// @brief calculate personPlan start and end positions
    void calculatePersonPlanPositionStartEndPos(Position& startPos, Position& endPos) const;

private:
    /**@brief check restriction with the number of children
     * @throw ProcessError if itis called without be reimplemented in child class
     */
    virtual bool checkChildDemandElementRestriction() const;

    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEDemandElement(const GNEDemandElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElement& operator=(const GNEDemandElement&) = delete;
};
