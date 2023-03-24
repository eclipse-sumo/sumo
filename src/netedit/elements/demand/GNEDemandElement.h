/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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

#include <netedit/GNEMoveElement.h>
#include <netedit/GNEPathManager.h>
#include <netedit/elements/GNEHierarchicalElement.h>
#include <utils/common/Parameterised.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/div/GUIGeometry.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/vehicle/SUMOVehicleParameter.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNENet;
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
 * @brief An Element which don't belong to GNENet but has influence in the simulation
 */
class GNEDemandElement : public GNEPathManager::PathElement, public GNEHierarchicalElement, public GNEMoveElement {

public:
    /// @brief friend declaration (needed for vTypes)
    friend class GNERouteHandler;

    /// @brief enum class for demandElement problems
    enum class Problem {
        OK,                     // There is no problem
        INVALID_ELEMENT,        // Element is invalid (for example, a route without edges)
        INVALID_PATH,           // Path (route, trip... ) is not valid (i.e is empty)
        DISCONNECTED_PLAN,      // Plan element (person, containers) is not connected with the previous or next plan
        INVALID_STOPPOSITION,   // StopPosition is invalid (only used in stops over edges or lanes
        STOP_DOWNSTREAM,        // Stops don't follow their route parent
        NO_PLANS                // Person or container doesn't have a plan
    };

    /**@brief Constructor
     * @param[in] id Gl-id of the demand element element (Must be unique)
     * @param[in] net pointer to GNEViewNet of this demand element element belongs
     * @param[in] type GUIGlObjectType of demand element
     * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_ROUTE, SUMO_TAG_VEHICLE, etc...)
     * @param[in] pathOptions path options
     * @param[in] junctionParents vector of junction parents
     * @param[in] edgeParents vector of edge parents
     * @param[in] laneParents vector of lane parents
     * @param[in] additionalParents vector of additional parents
     * @param[in] demandElementParents vector of demand element parents
     * @param[in] genericDataParents vector of generic data parents
     */
    GNEDemandElement(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, const int pathOptions,
                     const std::vector<GNEJunction*>& junctionParents,
                     const std::vector<GNEEdge*>& edgeParents,
                     const std::vector<GNELane*>& laneParents,
                     const std::vector<GNEAdditional*>& additionalParents,
                     const std::vector<GNEDemandElement*>& demandElementParents,
                     const std::vector<GNEGenericData*>& genericDataParents);

    /**@brief Constructor
     * @param[in] demandElementParent pointer to parent demand element pointer (used to generate an ID)
     * @param[in] net pointer to GNEViewNet of this demand element element belongs
     * @param[in] type GUIGlObjectType of demand element
     * @param[in] tag Type of xml tag that define the demand element element (SUMO_TAG_ROUTE, SUMO_TAG_VEHICLE, etc...)
     * @param[in] pathOptions path options
     * @param[in] junctionParents vector of junction parents
     * @param[in] edgeParents vector of edge parents
     * @param[in] laneParents vector of lane parents
     * @param[in] additionalParents vector of additional parents
     * @param[in] demandElementParents vector of demand element parents
     * @param[in] genericDataParents vector of generic data parents
     */
    GNEDemandElement(GNEDemandElement* demandElementParent, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, const int pathOptions,
                     const std::vector<GNEJunction*>& junctionParents,
                     const std::vector<GNEEdge*>& edgeParents,
                     const std::vector<GNELane*>& laneParents,
                     const std::vector<GNEAdditional*>& additionalParents,
                     const std::vector<GNEDemandElement*>& demandElementParents,
                     const std::vector<GNEGenericData*>& genericDataParents);

    /// @brief Destructor
    virtual ~GNEDemandElement();

    /**@brief get move operation
     * @note returned GNEMoveOperation can be nullptr
     */
    virtual GNEMoveOperation* getMoveOperation() = 0;

    /// @brief remove geometry point in the clicked position (Currently unused in shapes)
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);

    /// @brief get GUIGlObject associated with this AttributeCarrier
    GUIGlObject* getGUIGlObject();

    /// @brief get demand element geometry (stacked)
    const GUIGeometry& getDemandElementGeometry();

    /// @brief get previous child demand element to the given demand element
    GNEDemandElement* getPreviousChildDemandElement(const GNEDemandElement* demandElement) const;

    /// @brief get next child demand element to the given demand element
    GNEDemandElement* getNextChildDemandElement(const GNEDemandElement* demandElement) const;

    /// @brief update element stacked geometry (stacked)
    void updateDemandElementGeometry(const GNELane* lane, const double posOverLane);

    /// @brief update stack label
    void updateDemandElementStackLabel(const int stack);

    /// @brief update element spread geometry
    void updateDemandElementSpreadGeometry(const GNELane* lane, const double posOverLane);

    /// @name members and functions relative to elements common to all demand elements
    /// @{
    /// @brief obtain VClass related with this demand element
    virtual SUMOVehicleClass getVClass() const = 0;

    /// @brief get color
    virtual const RGBColor& getColor() const = 0;
    /// @}

    /// @name members and functions relative to write demand elements into XML
    /// @{
    /**@brief write demand element element into a xml file
     * @param[in] device device in which write parameters of demand element element
     */
    virtual void writeDemandElement(OutputDevice& device) const = 0;

    /// @brief check if current demand element is valid to be writed into XML (by default true, can be reimplemented in children)
    virtual Problem isDemandElementValid() const = 0;

    /// @brief return a string with the current demand element problem (by default empty, can be reimplemented in children)
    virtual std::string getDemandElementProblem() const = 0;

    /// @brief fix demand element problem (by default throw an exception, has to be reimplemented in children)
    virtual void fixDemandElementProblem() = 0;
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
    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /// @brief Returns position of demand element in view
    virtual Position getPositionInView() const = 0;

    /// @brief split geometry
    virtual void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList) = 0;
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
    virtual GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

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

    /// @brief check if element is locked
    bool isGLObjectLocked();

    /// @brief mark element as front element
    void markAsFrontElement();

    /// @brief delete element
    void deleteGLObject();

    /// @brief select element
    void selectGLObject();

    /// @brief update GLObject (geometry, ID, etc.)
    void updateGLObject();

    /// @}

    /// @name inherited from GNEPathManager::PathElement
    /// @{
    /// @brief compute pathElement
    virtual void computePathElement() = 0;

    /// @brief check if path element is selected
    bool isPathElementSelected() const;

    /**@brief Draws partial object (lane)
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] lane GNELane in which draw partial
     * @param[in] drawGeometry flag to enable/disable draw geometry (lines, boxLines, etc.)
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

    /* @brief method for getting the Attribute of an XML key in Position format (used in person plans)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    virtual Position getAttributePosition(SumoXMLAttr key) const = 0;

    /**@brief method for setting the attribute and letting the object perform demand element changes
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

    /// @brief get parameters map
    virtual const Parameterised::Map& getACParametersMap() const = 0;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    virtual std::string getPopUpID() const = 0;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    virtual std::string getHierarchyName() const = 0;
    /// @}

    /// @brief get personPlan start position
    const Position getBeginPosition(const double pedestrianDepartPos) const;

    /// @brief get invalid stops
    std::vector<GNEDemandElement*> getInvalidStops() const;

protected:
    /// @brief demand element geometry (also called "stacked geometry")
    GUIGeometry myDemandElementGeometry;

    /// @brief demand element spread geometry (Only used by vehicles and pedestrians)
    GUIGeometry mySpreadGeometry;

    /// @brief stacked label number
    int myStackedLabelNumber;

    /// @brief check if a new demand element ID is valid
    bool isValidDemandElementID(const std::string& newID) const;

    /// @name Only for person plans
    /// @{
    /// @brief check if person plan can be drawn
    bool drawPersonPlan() const;

    /// @brief check if container plan can be drawn
    bool drawContainerPlan() const;

    /// @brief draw person plan partial lane
    void drawPersonPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront,
                               const double personPlanWidth, const RGBColor& personPlanColor) const;

    /// @brief draw person plan partial junction
    void drawPersonPlanPartial(const bool drawPlan, const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* segment,
                               const double offsetFront, const double personPlanWidth, const RGBColor& personPlanColor) const;

    /// @brief check if person plan is valid
    Problem isPersonPlanValid() const;

    /// @brief get person plan problem
    std::string getPersonPlanProblem() const;

    /// @brief person plans arrival position radius
    static const double myPersonPlanArrivalPositionDiameter;

    /// @}

    /// @brief draw line between junctions
    void drawJunctionLine(const GNEDemandElement* element) const;

    /// @brief draw stack label
    void drawStackLabel(const std::string& element, const Position& position, const double rotation, const double width, const double length, const double exaggeration) const;

    /// @brief draw flow label
    void drawFlowLabel(const Position& position, const double rotation, const double width, const double length, const double exaggeration) const;

    /// @name replace parent elements
    /// @{

    /// @brief replace demand parent edges
    void replaceDemandParentEdges(const std::string& value);

    /// @brief replace demand parent lanes
    void replaceDemandParentLanes(const std::string& value);

    /// @brief replace the first parent junction
    void replaceFirstParentJunction(const std::string& value);

    /// @brief replace the last parent junction
    void replaceLastParentJunction(const std::string& value);

    /// @brief replace the first parent edge
    void replaceFirstParentEdge(const std::string& value);

    /// @brief replace the last parent edge
    void replaceLastParentEdge(const std::string& value);

    /// @brief replace additional parent
    void replaceAdditionalParent(SumoXMLTag tag, const std::string& value);

    /// @brief replace demand element parent
    void replaceDemandElementParent(SumoXMLTag tag, const std::string& value, const int parentIndex);

    /// @brief set VTypeDistribution parent
    void setVTypeDistributionParent(const std::string& value);

    /// @}

    /// @brief auxiliar struct used for calculate pathStopIndex
    struct EdgeStopIndex {

        /// @brief constructor
        EdgeStopIndex(GNEEdge* edge_, GNEDemandElement* stop) :
            edge(edge_),
            stops({stop}) {}

        /// @brief edge (obtained from segment)
        const GNEEdge* edge = nullptr;

        /// @brief list of stops placed in the edge
        std::vector<GNEDemandElement*> stops;

        /// @brief stopIndex (-1 menans out of route)
        int stopIndex = -1;

    private:
        /// @brief default constructor (disabled)
        EdgeStopIndex() {}
    };

    /// @brief get edgeStopIndex
    std::vector<EdgeStopIndex> getEdgeStopIndex() const;

    /// @brief set flow parameters (used in toggleAttribute(...) function of vehicles, persons and containers
    void setFlowParameters(SUMOVehicleParameter* vehicleParameters, const SumoXMLAttr attribute, const bool value);

    /// @brief adjust flow default attributes (called in vehicle/person/flow constructors)
    void adjustDefaultFlowAttributes(SUMOVehicleParameter* vehicleParameters);

    /// @brief build menu command route length
    void buildMenuCommandRouteLength(GUIGLObjectPopupMenu* ret) const;

private:
    /**@brief check restriction with the number of children
     * @throw ProcessError if itis called without be reimplemented in child class
     */
    virtual bool checkChildDemandElementRestriction() const;

    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief set move shape
    virtual void setMoveShape(const GNEMoveResult& moveResult) = 0;

    /// @brief commit move shape
    virtual void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) = 0;

    /// @brief Invalidated copy constructor.
    GNEDemandElement(const GNEDemandElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElement& operator=(const GNEDemandElement&) = delete;
};
