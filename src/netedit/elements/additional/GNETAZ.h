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
/// @file    GNETAZ.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
///
//
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/globjects/GUIPolygon.h>

#include "GNEAdditional.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEMoveElementShape;

// ===========================================================================
// class definitions
// ===========================================================================

class GNETAZ : public GNEAdditional, public TesselatedPolygon {

public:
    /// @brief needed to avoid diamond Problem between GUIPolygon and GNEAdditional
    using GNEAdditional::getID;

    /// @default GNETAZ Constructor
    GNETAZ(GNENet* net);

    /**@brief GNETAZ Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] filename file in which this element is stored
     * @param[in] shape TAZ shape
     * @param[in] center TAZ center
     * @param[in] fill flag for fill TAZ shape
     * @param[in] color TAZ color
     * @param[in] name TAZ's name
     * @param[in] parameters generic parameters
     */
    GNETAZ(const std::string& id, GNENet* net, const std::string& filename, const PositionVector& shape, const Position& TAZ,
           const bool fill, const RGBColor& color, const std::string& name, const Parameterised::Map& parameters);

    /// @brief GNETAZ Destructor
    ~GNETAZ();

    /// @brief methods to retrieve the elements linked to this TAZ
    /// @{

    /// @brief get GNEMoveElement associated with this TAZ
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this TAZ
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this TAZ (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /**@brief return index of a vertex of shape, or of a new vertex if position is over an shape's edge
     * @param pos position of new/existent vertex
     * @param snapToGrid enable or disable snapToActiveGrid
     * @return index of position vector
     */
    int getVertexIndex(Position pos, bool snapToGrid);

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

    /// @name Functions related with geometry of element
    /// @{

    /// @brief update pre-computed geometry information
    void updateGeometry() override;

    /// @brief Returns position of additional in view
    Position getPositionInView() const;

    /// @brief return exaggeration associated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;

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

    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

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

    /* @brief method for checking if the key and their corresponding attribute are valid
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

    /// @brief update TAZ Statistic
    void updateTAZStatistic();

protected:
    /// @brief move element shape
    GNEMoveElementShape* myMoveElementShape = nullptr;

    /// @brief TAZ center contour
    GNEContour myTAZCenterContour;

    /// @brief use edges whithin during creation (only in edit mode)
    bool myEdgesWithin = false;

private:
    /// @brief hint size of vertex
    static const double myHintSize;

    /// @brief squaredhint size of vertex
    static const double myHintSizeSquared;

    /// @brief Max source weight
    double myMaxWeightSource = 0;

    /// @brief Min source weight
    double myMinWeightSource = 0;

    /// @brief Average source weight
    double myAverageWeightSource = 0;

    /// @brief Max Sink weight
    double myMaxWeightSink = 0;

    /// @brief Min Sink weight
    double myMinWeightSink = 0;

    /// @brief Average Sink weight
    double myAverageWeightSink = 0;

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief Invalidated copy constructor.
    GNETAZ(const GNETAZ&) = delete;

    /// @brief Invalidated assignment operator.
    GNETAZ& operator=(const GNETAZ&) = delete;
};
