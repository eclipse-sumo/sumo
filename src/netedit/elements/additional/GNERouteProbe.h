/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNERouteProbe.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2016
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEAdditional.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNERouteProbe : public GNEAdditional, public Parameterised {

public:
    /// @brief Constructor
    GNERouteProbe(GNENet* net);

    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] filename file in which this element is stored
     * @param[in] edge edge in which this routeProbe is placed
     * @param[in] period The period in which to report the distribution
     * @oaran[in] name Route Probe Name
     * @param[in] outputFilename The file for generated output
     * @param[in] begin The time at which to start generating output
     * @param[in] vTypes list of vehicle types to be affected
     * @param[in] parameters generic parameters
     */
    GNERouteProbe(const std::string& id, GNENet* net, const std::string& filename, GNEEdge* edge, const SUMOTime period,
                  const std::string& name, const std::string& outputFilename, SUMOTime begin, const std::vector<std::string>& vehicleTypes,
                  const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNERouteProbe();

    /// @brief methods to retrieve the elements linked to this routeProbe
    /// @{

    /// @brief get GNEMoveElement associated with this routeProbe
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this routeProbe
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this routeProbe (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /// @name members and functions relative to write additionals into XML
    /// @{

    /**@brief write additional element into a xml file
    * @param[in] device device in which write parameters of additional element
    */
    void writeAdditional(OutputDevice& device) const;

    /// @brief check if current additional is valid to be written into XML (must be reimplemented in all detector children)
    bool isAdditionalValid() const;

    /// @brief return a string with the current additional problem (must be reimplemented in all detector children)
    std::string getAdditionalProblem() const;

    /// @brief fix additional problem (must be reimplemented in all detector children)
    void fixAdditionalProblem();

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const override;

    /// @}

    /**@brief get move operation
    * @note returned GNEMoveOperation can be nullptr
    */
    GNEMoveOperation* getMoveOperation();

    /// @name Functions related with geometry of element
    /// @{

    /// @brief update pre-computed geometry information
    void updateGeometry() override;

    /// @brief Returns position of additional in view
    Position getPositionInView() const;

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid);

    /// @brief split geometry
    void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList);

    /// @}

    /// @name inherited from GUIGlObject
    /// @{

    /// @brief Returns the name of the parent object
    /// @return This object's parent id
    std::string getParentName() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in double format
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    Position getAttributePosition(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in positionVector format
     * @param[in] key The attribute key
     * @return positionVector with the value associated to key
     */
    PositionVector getAttributePositionVector(SumoXMLAttr key) const override;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) override;

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value) override;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const override;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const override;

    /// @}

protected:
    /// @brief RouteProbe period
    SUMOTime myPeriod = SUMOTime_MAX_PERIOD;

    /// @brief output filename
    std::string myOutputFilename;

    /// @brief begin of rerouter
    SUMOTime myBegin = 0;

    /// @brief vehicle types
    std::vector<std::string> myVehicleTypes;

    /// @brief route probe logo offset
    Position myRouteProbeLogoOffset;

    /// @brief symbol base contour
    GNEContour mySymbolBaseContour;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief Invalidated copy constructor.
    GNERouteProbe(const GNERouteProbe&) = delete;

    /// @brief Invalidated assignment operator.
    GNERouteProbe& operator=(const GNERouteProbe&) = delete;
};
