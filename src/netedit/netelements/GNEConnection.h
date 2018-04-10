/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEConnection.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// A class for represent connections between Lanes
/****************************************************************************/
#ifndef GNEConnection_h
#define GNEConnection_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNENetElement.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEEdge;


// ===========================================================================
// class definitions
// ===========================================================================

class GNEConnection : public GNENetElement {
public:
    /** Constructor
     * @param[in] from The edge the vehicles leave
     * @param[in] connection NBEdge::Connection in which the rest of parameters are defined
     * @param[in] uncontrolled if set to true, This connection will not be TLS-controlled despite its node being controlled.
    **/
    GNEConnection(GNELane* from, GNELane* to);

    /// @brief Destructor
    ~GNEConnection();

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved) and implemented in ALL childrens
    void updateGeometry();

    /// Returns the street's geometry
    Boundary getBoundary() const;

    /// @brief get the name of the edge the vehicles leave
    GNEEdge* getEdgeFrom() const;

    /// @brief get the name of the edge the vehicles may reach when leaving "from"
    GNEEdge* getEdgeTo() const;

    /// @briefthe get lane of the incoming lane
    GNELane* getLaneFrom() const;

    /// @briefthe get lane of the outgoing lane
    GNELane* getLaneTo() const;

    /// @briefthe get lane index of the incoming lane
    int getFromLaneIndex() const;

    /// @briefthe get lane index of the outgoing lane
    int getToLaneIndex() const;

    /// @brief get Edge::Connection
    NBEdge::Connection& getNBEdgeConnection() const;

    /// @brief get NBConnection
    NBConnection getNBConnection() const;

    /// @brief get LinkState
    LinkState getLinkState() const;

    /// @brief get Position vector calculated in updateGeometry()
    PositionVector getShape() const;

    /// @brief update internal ID of Connection
    void updateID();

    /// @brief recompute cached myLinkState
    void updateLinkState();

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

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     */
    Boundary getCenteringBoundary() const;

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
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

protected:
    /// @brief incoming lane of this connection
    GNELane* myFromLane;

    /// @brief outgoing lane of this connection
    GNELane* myToLane;

    /// @brief the shape of the connection
    PositionVector myShape;

    /// @name computed only once (for performance) in updateGeometry()
    /// @{
    /// @brief The rotations of the shape parts
    std::vector<double> myShapeRotations;

    /// @brief The lengths of the shape parts
    std::vector<double> myShapeLengths;

    /// @brief waiting position for internal junction
    PositionVector myInternalJunctionMarker;
    /// @}

    /// @brief Linkstate. @note cached because after 'undo' the connection needs to be drawn while the node logic (NBRequest) has not been recomputed
    LinkState myLinkState;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEConnection(const GNEConnection&) = delete;

    /// @brief Invalidated assignment operator.
    GNEConnection& operator=(const GNEConnection&) = delete;
};


#endif

/****************************************************************************/

