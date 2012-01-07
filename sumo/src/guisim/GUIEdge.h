/****************************************************************************/
/// @file    GUIEdge.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A road/street connecting two junctions (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIEdge_h
#define GUIEdge_h


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
#include <microsim/MSEdge.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include "GUILaneWrapper.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIEdge
 * @brief A road/street connecting two junctions (gui-version)
 *
 * @see MSEdge
 */
class GUIEdge : public MSEdge, public GUIGlObject {
public:
    /** @brief Constructor.
     * @param[in] id The id of the edge
     * @param[in] numericalID The numerical id (index) of the edge
     * @see MSEdge
     */
    GUIEdge(const std::string& id, unsigned int numericalID, const std::string& streetName) ;


    /// @brief Destructor.
    ~GUIEdge() ;


    /** @brief Builds lane wrappers for this edge's lanes
     */
    void initGeometry() ;


    /* @brief Returns the gl-ids of all known edges
     * @param[in] includeInternal Whether to include ids of internal edges
     */
    static std::vector<GUIGlID> getIDs(bool includeInternal);

    /// Returns the street's geometry
    Boundary getBoundary() const;

    /// returns the enumerated lane (!!! why not private with a friend?)
    MSLane& getLane(size_t laneNo);



    /// returns the enumerated lane's geometry (!!! why not private with a friend?)
    GUILaneWrapper& getLaneGeometry(size_t laneNo) const;

    GUILaneWrapper& getLaneGeometry(const MSLane* lane) const;

    /** returns the position on the line given by the coordinates where "prev"
        is the length of the line and "wanted" the distance from the begin
        !!! should be within another class */
    static std::pair<SUMOReal, SUMOReal> getLaneOffsets(SUMOReal x1, SUMOReal y1,
            SUMOReal x2, SUMOReal y2, SUMOReal prev, SUMOReal wanted);

    static void fill(std::vector<GUIEdge*> &netsWrappers);



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


#ifdef HAVE_MESOSIM
    unsigned int getVehicleNo() const;
    SUMOReal getOccupancy() const;
    SUMOReal getMeanSpeed() const;
    SUMOReal getAllowedSpeed() const;
    SUMOReal getFlow() const;

    /// @brief sets the color according to the currente settings
    void setColor(const GUIVisualizationSettings& s) const;

    /// @brief gets the color value according to the current scheme index
    SUMOReal getColorValue(size_t activeScheme) const;

#endif

private:
    /// Definition of the lane's positions vector
    typedef std::vector<GUILaneWrapper*> LaneWrapperVector;

    /// List of the edge's lanes geometrical information
    LaneWrapperVector myLaneGeoms;

    /**
     * @class lane_wrapper_finder
     * @brief A class to find the matching lane wrapper
     */
    class lane_wrapper_finder {
    public:
        /** @brief constructor */
        explicit lane_wrapper_finder(const MSLane& lane) : myLane(lane) { }

        /** @brief the comparing function */
        bool operator()(const GUILaneWrapper* const wrapper) {
            return wrapper->forLane(myLane);
        }

    private:
        lane_wrapper_finder& operator=(const lane_wrapper_finder&); // just to avoid a compiler warning
    private:
        /// @brief The lane to search for
        const MSLane& myLane;

    };

private:
    /// @brief invalidated copy constructor
    GUIEdge(const GUIEdge& s);

    /// @brief invalidated assignment operator
    GUIEdge& operator=(const GUIEdge& s);


};


#endif

/****************************************************************************/

