/****************************************************************************/
/// @file    GNEConnection.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// A class for represent connections between Lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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
    /**@brief Constructor.
     * @param[in] from The edge the vehicles leave
     * @param[in] fromLane index of the incoming lane
     * @param[in] to The edge the vehicles may reach when leaving "from"
     * @param[in] toLane index of the outgoing lane
     * @param[in] pass if set, vehicles which pass this (lane-2-lane) connection) will not wait
     * @param[in] keepClear if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection.
     * @param[in] contPos If set to a positive value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection.
     * @param[in] uncontrolled if set to true, This connection will not be TLS-controlled despite its node being controlled.
     */
    GNEConnection(GNEEdge *from, int fromLane, GNEEdge *to, int toLane, bool pass, bool keepClear, SUMOReal contPos, bool uncontrolled);

    /** Constructor
     * @param[in] from The edge the vehicles leave
     * @param[in] connection NBEdge::Connection in which the rest of parameters are defined
     * @param[in] uncontrolled if set to true, This connection will not be TLS-controlled despite its node being controlled.
    **/
    GNEConnection(GNEEdge *from, NBEdge::Connection connection, bool uncontrolled = false);

    /// @brief Destructor
    ~GNEConnection();

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved) and implemented in ALL childrens
    void updateGeometry();

    /// Returns the street's geometry
    Boundary getBoundary() const;

    /// @brief get the name of the edge the vehicles leave
    GNEEdge *getEdgeFrom() const;

    /// @brief get the name of the edge the vehicles may reach when leaving "from"
    GNEEdge *getEdgeTo() const;

    /// @briefthe get lane of the incoming lane
    GNELane* getLaneFrom() const;

    /// @briefthe get lane of the outgoing lane
    GNELane* getLaneTo() const;

    /// @briefthe get lane index of the incoming lane
    int getFromLaneIndex() const;

    /// @briefthe get lane index of the outgoing lane
    int getToLaneIndex() const;

    /// @brief get parameter pass
    bool getPass();

    /// @brief get parameter keepClear
    bool getKeepClear();

    /// @brief get parameter ContPos
    SUMOReal getContPos();

    /// @brief get parameter uncontrolled
    bool getUncontrolled();

    /// @brief get Edge::NBConnection
    const NBEdge::Connection &getNBEdgeConnection() const;

    /// @brief get Draw connection
    bool getDrawConnection() const;

    /// @brief get LinkState
    int getLinkState() const;

    /// @brief set parameter pass
    void setPass(bool pass);

    /// @brief set parameter keepClear
    void setKeepClear(bool keepClear);

    /// @brief set parameter ContPos
    void setContPos(SUMOReal contPos);

    /// @brief set parameter uncontrolled
    void setUncontrolled(bool uncontrolled);

    /// @brief enable or disable draw connection
    void setDrawConnection(bool drawConnection);

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
    /// @brief NBEdge::Connection associated with this connection
    NBEdge::Connection myConnection;

    /// @brief incoming edge of this connection
    GNEEdge *myFromEdge;

    /// @brief check if this connection is uncontrollec
    bool myUncontrolled;

    /// @brief the shape of the edge
    PositionVector myShape;

    /// @name computed only once (for performance) in updateGeometry()
    /// @{
    /// @brief The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// @brief The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;
    /// @}

    /// @brief Enable or disable draw connection
    /// @note by default is enabled
    bool myDrawConnection;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEConnection(const GNEConnection&);

    /// @brief Invalidated assignment operator.
    GNEConnection& operator=(const GNEConnection&);
};


#endif

/****************************************************************************/

