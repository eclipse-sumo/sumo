/****************************************************************************/
/// @file    GNEEdge.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A road/street connecting two junctions (netedit-version, adapted from GUIEdge)
// Basically a container for an NBEdge with drawing and editing capabilities
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
#ifndef GNEEdge_h
#define GNEEdge_h


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
class GNENet;
class GNEJunction;
class GNELane;
class GNEAdditional;
class GNEAdditionalSet;
class GNEConnection;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEEdge
 * @brief A road/street connecting two junctions (netedit-version)
 *
 * @see MSEdge
 */
class GNEEdge : public GNENetElement {

    /// @brief Friend class
    friend class GNEChange_Lane;
    friend class GNEChange_Connection;

public:
    /// @brief Definition of the lane's positions vector
    typedef std::vector<GNELane*> LaneVector;

    /// @brief Definition of the additionals vector
    typedef std::vector<GNEAdditional*> AdditionalVector;

    /// @brief Definition of the additionalSets vector
    typedef std::vector<GNEAdditionalSet*> AdditionalSetVector;

    /// @brief Definition of the connections map
    typedef std::map<int, GNEConnection*> connectionMap;

    /**@brief Constructor.
     * @param[in] nbe The represented edge
     * @param[in] net The net to inform about gui updates
     * @param[in] loaded Whether the edge was loaded from a file
     */
    GNEEdge(NBEdge& nbe, GNENet* net, bool wasSplit = false, bool loaded = false);

    /// @brief Destructor.
    ~GNEEdge();

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// Returns the street's geometry
    Boundary getBoundary() const;

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
    virtual GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /**@brief update edge geometry after junction move */
    void updateJunctionPosition(GNEJunction* junction, const Position& origPos);

    /// @brief returns the internal NBEdge
    NBEdge* getNBEdge();

    /// @brief returns the source-junction
    GNEJunction* getSource() const;

    /// @brief returns the destination-junction
    GNEJunction* getDest() const;

    /// @brief returns vector of GNEConnections of this edge
    std::vector<GNEConnection*> getGNEConnections() const;

    /**@brief change the edge geometry
     * It is up to the Edge to decide whether an new geometry node should be
     * generated or an existing node should be moved
     * @param[in] oldPos The origin of the mouse movement
     * @param[in] newPos The destination of the mouse movenent
     * @param[in] relative Whether newPos is absolute or relative
     * @return newPos if something was moved, oldPos if nothing was moved
     */
    Position moveGeometry(const Position& oldPos, const Position& newPos, bool relative = false);

    //// @brief manipulate the given geometry and return whether it was changed
    static bool changeGeometry(PositionVector& geom, const std::string& id, const Position& oldPos, const Position& newPos, bool relative = false, bool moveEndPoints = false);

    /**@brief change the edge geometry
     * @param[in] delta All inner points are moved by adding delta
     */
    void moveGeometry(const Position& delta);

    /**@brief deletes the closest geometry node within SNAP_RADIUS.
     * @return true if a node was deleted
     */
    bool deleteGeometry(const Position& pos, GNEUndoList* undoList);

    /// @brief makes pos the new geometry endpoint at the appropriate end
    void setEndpoint(Position pos, GNEUndoList* undoList);

    /// @brief restores the endpoint to the junction position at the appropriate end
    void resetEndpoint(const Position& pos, GNEUndoList* undoList);

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

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

    /// @brief set responsibility for deleting internal strctures
    void setResponsible(bool newVal);

    /**@brief update edge geometry and inform the lanes
     * @param[in] geom The new geometry
     * @param[in] inner Whether geom is only the inner points
     */
    void setGeometry(PositionVector geom, bool inner);

    /**@brief update edge geometry and inform the lanes
     * let the lanes recompute their precomputed geometry information
     * (needed after computing junction shapes)
     */
    void updateLaneGeometries();

    /// @brief copy edge attributes from tpl
    void copyTemplate(GNEEdge* tpl, GNEUndoList* undolist);

    /// @brief returns GLIDs of all lanes
    std::set<GUIGlID> getLaneGlIDs();

    /// @brief returns a reference to the lane vector
    const std::vector<GNELane*>& getLanes();

    /// @brief whether this edge was created from a split
    bool wasSplit();

    /* @brief compute a splitting position which keeps the resulting edges
     * straight unless the user clicked near a geometry point */
    Position getSplitPos(const Position& clickPos);

    /// @brief override to also set lane ids
    void setMicrosimID(const std::string& newID);

    /// @brief add additional to this edge
    bool addAdditional(GNEAdditional* additional);

    /// @brief remove additional from this edge
    bool removeAdditional(GNEAdditional* additional);

    /// @brief return list of additionals associated with this edge
    const std::vector<GNEAdditional*>& getAdditionals() const;

    /// @brief add GNEAdditionalSet to this edge
    bool addAdditionalSet(GNEAdditionalSet* additionalSet);

    /// @brief remove GNEAdditionalSet from this edge
    bool removeAdditionalSet(GNEAdditionalSet* additionalSet);

    /// @brief return list of additionalSets associated with this edge
    const std::vector<GNEAdditionalSet*>& getAdditionalSets();

    // the radius in which to register clicks for geometry nodes
    static const SUMOReal SNAP_RADIUS;

protected:
    /// @brief the underlying NBEdge
    NBEdge& myNBEdge;

    /// @brief restore point for undo
    PositionVector myOrigShape;

    /// @brief List of this edges lanes
    LaneVector myLanes;

    /// @brief whether we are responsible for deleting myNBNode
    bool myAmResponsible;

    /// @brief whether this edge was created from a split
    bool myWasSplit;

    /// @brief modification status of the connections
    std::string myConnectionStatus;

    /// @brief vector with the additonals vinculated with this edge
    AdditionalVector myAdditionals;

    /// @brief vector with the additonalSets vinculated with this edge
    AdditionalSetVector myAdditionalSets;

    /// @brief map with the connections vinculated with this edge
    connectionMap myConnections;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief invalidated copy constructor
    GNEEdge(const GNEEdge& s);

    /// @brief invalidated assignment operator
    GNEEdge& operator=(const GNEEdge& s);

    /**@brief changes the number of lanes.
     * When reducing the number of lanes, higher-numbered lanes are removed first.
     * When increasing the number of lanes, the last known attributes for a lane
     * with this number are restored. If none are found the attributes for the
     * leftmost lane are copied
     */
    void setNumLanes(int numLanes, GNEUndoList* undoList);

    /// @brief@brief increase number of lanes by one use the given attributes and restore the GNELane
    void addLane(GNELane* lane, const NBEdge::Lane& laneAttrs);

    /// @briefdecrease the number of lanes by one. argument is only used to increase robustness (assertions)
    void removeLane(GNELane* lane);

    /// @brief adds a connection
    void addConnection(int fromLane, const std::string& toEdgeID, int toLane, bool mayPass);

    /// @brief removes a connection
    void removeConnection(int fromLane, const std::string& toEdgeID, int toLane);
};


#endif

/****************************************************************************/

