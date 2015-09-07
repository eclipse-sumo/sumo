/****************************************************************************/
/// @file    GNEEdge.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id: GNEEdge.h 4489 2015-04-28 12:58:04Z erdm_ja $
///
// A road/street connecting two junctions (netedit-version, adapted from GUIEdge)
// Basically a container for an NBEdge with drawing and editing capabilities
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <vector>
#include <string>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/geom/Position.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <netbuild/NBEdge.h>
#include "GNEAttributeCarrier.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEJunction;
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEEdge
 * @brief A road/street connecting two junctions (netedit-version)
 *
 * @see MSEdge
 */
class GNEEdge : public GUIGlObject, public GNEAttributeCarrier {

    friend class GNEChange_Lane;
    friend class GNEChange_Connection;

public:
    /// Definition of the lane's positions vector
    typedef std::vector<GNELane*> LaneVector;

    /** @brief Constructor.
     * @param[in] nbe The represented edge
     * @param[in] net The net to inform about gui updates
     * @param[in] loaded Whether the edge was loaded from a file
     */
    GNEEdge(NBEdge& nbe, GNENet* net, bool wasSplit = false, bool loaded = false) ;


    /// @brief Destructor.
    ~GNEEdge() ;

    /// Returns the street's geometry
    Boundary getBoundary() const;


    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    virtual GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app,
            GUISUMOAbstractView& parent) ;


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    virtual GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app,
            GUISUMOAbstractView& parent) ;


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const ;


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const ;
    //@}


    /** @brief update edge geometry after junction move */
    void updateJunctionPosition(GNEJunction* junction, const Position& origPos);


    /** @brief returns the internal NBEdge
     */
    NBEdge* getNBEdge() {
        return &myNBEdge;
    }


    /** @brief returns the source-junction */
    GNEJunction* getSource() const;

    /** @brief returns the destination-junction */
    GNEJunction* getDest() const;

    /** @brief change the edge geometry without registering undo/redo
     * It is up to the Edge to decide whether an new geometry node should be
     * generated or an existing node should be moved
     * @param[in] oldPos The origin of the mouse movement
     * @param[in] newPos The destination of the mouse movenent
     * @param[in] relative Whether newPos is absolute or relative
     * @return newPos if something was moved, oldPos if nothing was moved
     */
    Position moveGeometry(const Position& oldPos, const Position& newPos, bool relative=false);


    /** @brief change the edge geometry without registering undo/redo
     * @param[in] delta All inner points are moved by adding delta
     */
    void moveGeometry(const Position& delta);


    /** @brief deletes the closest geometry node within SNAP_RADIUS.
     * @return true if a node was deleted
     */
    bool deleteGeometry(const Position& pos, GNEUndoList* undoList);


    /** @brief makes pos the new geometry endpoint at the appropriate end
     */
    void setEndpoint(Position pos, GNEUndoList* undoList);


    /** @brief restores the endpoint to the junction position at the appropriate end
     */
    void resetEndpoint(const Position& pos, GNEUndoList* undoList);


    //@name inherited from GNEAttributeCarrier
    //@{
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    bool isValid(SumoXMLAttr key, const std::string& value);
    //@}

    /// @brief set responsibility for deleting internal strctures
    void setResponsible(bool newVal) {
        myAmResponsible = newVal;
    }

    /** @brief update edge geometry and inform the lanes
     * @param[in] geom The new geometry
     * @param[in] inner Whether geom is only the inner points
     */
    void setGeometry(PositionVector geom, bool inner);


    /** @brief update edge geometry and inform the lanes
     * let the lanes recompute their precomputed geometry information
     * (needed after computing junction shapes)
     */
    void updateLaneGeometries();


    /** @brief copy edge attributes from tpl */
    void copyTemplate(GNEEdge* tpl, GNEUndoList* undolist);


    /** returns GLIDs of all lanes */
    std::set<GUIGlID> getLaneGlIDs();

    /** returns a reference to the lane vector */
    const LaneVector& getLanes() {
        return myLanes;
    }

    /// @brief whether this edge was created from a split
    bool wasSplit() {
        return myWasSplit;
    }

    /* @brief compute a splitting position which keeps the resulting edges
     * straight unless the user clicked near a geometry point */
    Position getSplitPos(const Position& clickPos);


private:
    // the radius in which to register clicks for geometry nodes
    static const SUMOReal SNAP_RADIUS;

    /// the underlying NBEdge
    NBEdge& myNBEdge;

    ///@brief restore point for undo
    PositionVector myOrigShape;

    /// List of this edges lanes
    LaneVector myLanes;

    // the net to inform about updates
    GNENet* myNet;

    /// @brief whether we are responsible for deleting myNBNode
    bool myAmResponsible;

    /// @brief whether this edge was created from a split
    bool myWasSplit;

    /// @brief modification status of the connections
    std::string myConnectionStatus;

private:
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief invalidated copy constructor
    GNEEdge(const GNEEdge& s);

    /// @brief invalidated assignment operator
    GNEEdge& operator=(const GNEEdge& s);

    /** @brief changes the number of lanes.
     * When reducing the number of lanes, higher-numbered lanes are removed first.
     * When increasing the number of lanes, the last known attributes for a lane
     * with this number are restored. If none are found the attributes for the
     * leftmost lane are copied
     */
    void setNumLanes(unsigned int numLanes, GNEUndoList* undoList);

    /** @brief increase number of lanes by one use the given attributes and
     * restore the GNELane*/
    void addLane(GNELane* lane, const NBEdge::Lane& laneAttrs);

    /* decrease the number of lanes by one. argument is only used to increase
     * robustness (assertions) */
    void removeLane(GNELane* lane);

    /** @brief adds a connection */
    void addConnection(unsigned int fromLane, const std::string& toEdgeID, unsigned int toLane, bool mayPass);

    /** @brief removes a connection */
    void removeConnection(unsigned int fromLane, const std::string& toEdgeID, unsigned int toLane);
};


#endif

/****************************************************************************/

