/****************************************************************************/
/// @file    GNEConnection.h
/// @author  Pablo Álvarez López
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
     * @param[in] edgeFrom The  edge the vehicles leave
     * @param[in] edgeTo The edge the vehicles may reach when leaving "from"
     * @param[in] fromLane the incoming lane
     * @param[in] toLane the outgoing lane
     * @param[in] pass if set, vehicles which pass this (lane-2-lane) connection) will not wait
     * @param[in] keepClear if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection.
     * @param[in] contPos If set to a positive value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection.
     * @param[in] uncontrolled if set to true, This connection will not be TLS-controlled despite its node being controlled.
     */
    GNEConnection(GNEEdge *edgeFrom, GNEEdge *edgeTo, GNELane *fromLane, GNELane *toLane, bool pass, bool keepClear, SUMOReal contPos, bool uncontrolled);

    /// @brief Destructor
    ~GNEConnection();

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved) and implemented in ALL childrens
    void updateGeometry();

    /// @brief get the name of the edge the vehicles leave
    /// @note this parameter cannot be changed
    GNEEdge *getEdgeFrom();

    /// @brief get the name of the edge the vehicles may reach when leaving "from"
    GNEEdge *getEdgeTo();

    /// @brief the get incoming lane
    GNELane *getFromLane();

    /// @briefthe get outgoing lane
    GNELane *getToLane();

    /// @briefthe get lane index of the incoming lane
    int getFromLaneIndex();

    /// @briefthe get lane index of the outgoing lane
    int getToLaneIndex();

    /// @brief get parameter pass
    bool getPass();

    /// @brief get parameter keepClear
    bool getKeepClear();

    /// @brief get parameter ContPos
    SUMOReal getContPos();

    /// @briefif get parameter uncontrolled
    bool getUncontrolled();

    /// @brief set parameter pass
    void setPass(bool pass);

    /// @brief set parameter keepClear
    void setKeepClear(bool keepClear);

    /// @brief set parameter ContPos
    void setContPos(SUMOReal contPos);

    /// @briefif set parameter uncontrolled
    void setUncontrolled(bool uncontrolled);

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
    /// @brief The name of the edge the vehicles leave
    GNEEdge *myEdgeFrom;

    /// @brief The name of the edge the vehicles may reach when leaving "from"
    GNEEdge *myEdgeTo;

    /// @brief the incoming lane
    GNELane *myFromLane;

    /// @brief the outgoing lane
    GNELane *myToLane;

    /// @brief if set, vehicles which pass this (lane-2-lane) connection) will not wait
    bool myPass;

    /// @brief if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection.
    bool myKeepClear;

    /// @brief if set to 0, no internal junction will be built for this connection. If set to a positive value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection.
    SUMOReal myContPos;

    /// @brief if set to true, This connection will not be TLS-controlled despite its node being controlled.
    bool myUncontrolled;

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

