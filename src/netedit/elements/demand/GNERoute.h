/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2016-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNERoute.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// A class for visualizing routes in Netedit
/****************************************************************************/
#pragma once
#include <config.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

#include "GNEDemandElement.h"
#include "GNERouteHandler.h"


// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;
class GNEConnection;
class GNEVehicle;

// ===========================================================================
// class definitions
// ===========================================================================

class GNERoute : public GNEDemandElement, public Parameterised {

public:

    /// @brief class used in GUIGLObjectPopupMenu for routes
    class GNERoutePopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GNERoutePopupMenu)

    public:
        /** @brief Constructor
         * @param[in] app The main window for instantiation of other windows
         * @param[in] parent The parent view for changing it
         * @param[in] o The object of interest
         * @param[in, out] additionalVisualizations Information which additional visualisations are enabled (per view)
         */
        GNERoutePopupMenu(GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject& o);

        /// @brief Destructor
        ~GNERoutePopupMenu();

        /// @brief Called to modify edge distance values along the route
        long onCmdApplyDistance(FXObject*, FXSelector, void*);

    protected:
        /// @brief default constructor needed by FOX
        GNERoutePopupMenu() {}
    };

    /// @brief default constructor
    GNERoute(SumoXMLTag tag, GNENet* net);

    /// @brief default constructor (used in calibrators)
    GNERoute(GNENet* net);

    /// @brief default constructor (used in copy vehicles)
    GNERoute(GNENet* net, const std::string& id, const GNEDemandElement* originalRoute);

    /// @brief default  constructor (used in copy embedded vehicles)
    GNERoute(GNENet* net, GNEVehicle* vehicleParent, const GNEDemandElement* originalRoute);

    /**@brief parameter constructor
     * @param[in] viewNet view in which this Route is placed
     * @param[in] id route ID
     * @param[in] vClass vehicle class
     * @param[in] edges route edges
     * @param[in] color route color
     * @param[in] repeat the number of times that the edges of this route shall be repeated
     * @param[in] cycleType the times will be shifted forward by 'cycleTime' on each repeat
     * @param[in] parameters generic parameters
     */
    GNERoute(GNENet* net, const std::string& id, SUMOVehicleClass vClass, const std::vector<GNEEdge*>& edges, const RGBColor& color,
             const int repeat, const SUMOTime cycleTime, const Parameterised::Map& parameters);

    /**@brief parameter constructor for embedded routes
     * @param[in] viewNet view in which this Route is placed
     * @param[in] vehicleParent vehicle parent of this embedded route
     * @param[in] edges route edges
     * @param[in] color route color
     * @param[in] repeat the number of times that the edges of this route shall be repeated
     * @param[in] cycleType the times will be shifted forward by 'cycleTime' on each repeat
     * @param[in] parameters generic parameters
     */
    GNERoute(GNENet* net, GNEDemandElement* vehicleParent, const std::vector<GNEEdge*>& edges, const RGBColor& color,
             const int repeat, const SUMOTime cycleTime, const Parameterised::Map& parameters);

    /// @brief destructor
    ~GNERoute();

    /**@brief get move operation
     * @note returned GNEMoveOperation can be nullptr
     */
    GNEMoveOperation* getMoveOperation();

    /**@brief write demand element element into a xml file
     * @param[in] device device in which write parameters of demand element element
     */
    void writeDemandElement(OutputDevice& device) const;

    /// @brief check if current demand element is valid to be written into XML (by default true, can be reimplemented in children)
    Problem isDemandElementValid() const;

    /// @brief return a string with the current demand element problem (by default empty, can be reimplemented in children)
    std::string getDemandElementProblem() const;

    /// @brief fix demand element problem (by default throw an exception, has to be reimplemented in children)
    void fixDemandElementProblem();

    /// @name members and functions relative to elements common to all demand elements
    /// @{
    /// @brief obtain VClass related with this demand element
    SUMOVehicleClass getVClass() const;

    /// @brief get color
    const RGBColor& getColor() const;

    /// @}

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns position of additional in view
    Position getPositionInView() const;
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

    /**@brief Returns the name of the parent object
     * @return This object's parent id
     */
    std::string getParentName() const;

    /// @brief return exaggeration associated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     * @return The boundary the object is within
     */
    Boundary getCenteringBoundary() const;

    /// @brief split geometry
    void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList);

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    /// @}

    /// @name inherited from GNEPathManager::PathElement
    /// @{

    /// @brief compute pathElement
    void computePathElement();

    /**@brief Draws partial object over lane
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment lane segment
     * @param[in] offsetFront front offset
     */
    void drawLanePartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const;

    /**@brief Draws partial object over junction
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment junction segment
     * @param[in] offsetFront front offset
     */
    void drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const;

    /// @brief get first path lane
    GNELane* getFirstPathLane() const;

    /// @brief get last path lane
    GNELane* getLastPathLane() const;
    /// @}

    /// @brief inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in Position format (used in person plans)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    Position getAttributePosition(SumoXMLAttr key) const;

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     * @param[in] net optionally the GNENet to inform about gui updates
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const;

    /** @brief check if a route is valid
     * @param[in] edges vector with the route's edges
     * @return an empty string if route is valid, or a string with the problem
     */
    static std::string isRouteValid(const std::vector<GNEEdge*>& edges);

    /// @brief create a copy of the given route
    static GNEDemandElement* copyRoute(const GNERoute* originalRoute);

protected:
    /// @brief route color
    RGBColor myColor;

    /// @brief repeat
    int myRepeat;

    /// @brief cycleTime
    SUMOTime myCycleTime;

    /// @brief SUMOVehicleClass (Only used for drawing)
    SUMOVehicleClass myVClass;

private:
    /// @brief draw route partial lane
    void drawRoutePartialLane(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                              const GNEPathManager::Segment* segment, const double offsetFront,
                              const GUIGeometry& geometry, const double exaggeration) const;

    /// @brief draw route partial junction
    void drawRoutePartialJunction(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                  const double offsetFront, const GUIGeometry& geometry, const double exaggeration) const;

    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief Invalidated copy constructor.
    GNERoute(GNERoute*) = delete;

    /// @brief Invalidated assignment operator.
    GNERoute& operator=(GNERoute*) = delete;
};
