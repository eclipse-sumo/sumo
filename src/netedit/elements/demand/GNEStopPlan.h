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
/// @file    GNEStopPlan.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2023
///
// Representation of Stops in netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDemandElement.h"
#include "GNEDemandElementPlan.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEStopPlan : public GNEDemandElement, public GNEDemandElementPlan {

public:
    /// @brief default constructor
    GNEStopPlan(SumoXMLTag tag, GNENet* net);

    /**@brief constructor called in buildStopPlan
     * @param[in] tag stop tag
     * @param[in] planParameters plan parameters
     * @param[in] additionals from-to additionals
     * @param[in] endPos end position
     * @param[in] duration stop duration
     * @param[in] until stop until
     * @param[in] actType act type
     * @param[in] friendlyPos friendly pos
     * @param[in] parameterSet parameter sets
     */
    GNEStopPlan(SumoXMLTag tag, GNEDemandElement* personParent, const GNEPlanParents& planParameters,
                const double endPos, const SUMOTime duration, const SUMOTime until, const std::string& actType,
                bool friendlyPos, const int parameterSet);

    /// @brief destructor
    ~GNEStopPlan();

    /// @brief methods to retrieve the elements linked to this stopPlan
    /// @{

    /// @brief get GNEMoveElement associated with this stopPlan
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this stopPlan
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this stopPlan
    const Parameterised* getParameters() const override;

    /// @}

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
    void updateGeometry() override;

    /// @brief Returns position of demand element in view
    Position getPositionInView() const;
    /// @}

    /// @name inherited from GUIGlObject
    /// @{
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

    /// @name inherited from GNEPathElement
    /// @{

    /// @brief compute pathElement
    void computePathElement();

    /**@brief Draws partial object over lane
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment lane segment
     * @param[in] offsetFront front offset
     */
    void drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const;

    /**@brief Draws partial object over junction
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment junction segment
     * @param[in] offsetFront front offset
     */
    void drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const;

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

    /* @brief method for setting the attribute and letting the object perform demand element changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     * @param[in] net optionally the GNENet to inform about gui updates
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) override;

    /* @brief method for setting the attribute and letting the object perform demand element changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    bool isValid(SumoXMLAttr key, const std::string& value) override;

    /* @brief method for enable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    void enableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for disable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    void disableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const override;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const override;
    /// @}

protected:
    /// @brief variable used for draw contours
    GNEContour myStopContour;

    /// @brief variable used for draw sign contours
    GNEContour myStopSignContour;

    /// @brief sign position
    Position mySignPosition;

    /// @brief duration
    SUMOTime myDuration;

    /// @brief until
    SUMOTime myUntil;

    /// @brief act type
    std::string myActType;

    /// @brief friendly pos
    bool myFriendlyPos = false;

    /// @brief parameter set
    int myParametersSet = 0;

private:
    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief method for enable or disable the attribute and nothing else (used in GNEChange_ToggleAttribute)
    void toggleAttribute(SumoXMLAttr key, const bool value);

    /// @brief Invalidated copy constructor.
    GNEStopPlan(const GNEStopPlan&) = delete;

    /// @brief Invalidated assignment operator
    GNEStopPlan& operator=(const GNEStopPlan&) = delete;
};
