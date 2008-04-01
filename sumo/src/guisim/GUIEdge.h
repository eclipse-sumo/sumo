/****************************************************************************/
/// @file    GUIEdge.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A road/street connecting two junctions (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundary.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include "GUILaneWrapper.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h> 


// ===========================================================================
// class declarations
// ===========================================================================
class MSJunction;
class GUILane;
class GUIBasicLane;
class GUIGlObjectStorage;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIEdge
 * @brief A road/street connecting two junctions (gui-version)
 *
 * @see MSEdge
 */
class GUIEdge : public MSEdge, public GUIGlObject
{
public:
    /** @brief Constructor.
     * @param[in] id The id of the edge
     * @param[in] numericalID The numerical id (index) of the edge
     * @param[in] idStorage The gl-objects storage to retrieve the gl-id for this edge from
     * @see MSEdge
     */
    GUIEdge(const std::string &id, size_t numericalID,
            GUIGlObjectStorage &idStorage) throw();


    /// @brief Destructor.
    ~GUIEdge() throw();


    /// sets the junctions allowing the retrieval of the edge position (!!! why not private with a friend?)
    void initGeometry(GUIGlObjectStorage &idStorage);

    /** Returns the list of all known junctions as their names */
    static std::vector<std::string> getNames();

    /** Returns the list of all known junctions as their ids */
    static std::vector<GLuint> getIDs();

    /// Returns the street's geometry
    Boundary getBoundary() const;

    /// returns the enumerated lane (!!! why not private with a friend?)
    MSLane &getLane(size_t laneNo);



    /// returns the enumerated lane's geometry (!!! why not private with a friend?)
    GUILaneWrapper &getLaneGeometry(size_t laneNo) const;

    GUILaneWrapper &getLaneGeometry(const MSLane *lane) const;

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
    virtual GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
            GUISUMOAbstractView &parent) throw();


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    virtual GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent) throw();


    /** @brief Returns the id of the object as known to microsim
     *
     * @return The id of the edge
     * @see GUIGlObject::microsimID
     */
    const std::string &microsimID() const throw();

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const throw();

    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const throw();
    //@}

#ifdef HAVE_MESOSIM
    size_t getVehicleNo() const;
    SUMOReal getDensity() const;
    SUMOReal getRouteSpread() const;
    SUMOReal getMeanSpeed() const;
    SUMOReal getAllowedSpeed() const;
    SUMOReal getFlow() const;
#endif

private:
    /// Definition of the lane's positions vector
    typedef std::vector<GUILaneWrapper*> LaneWrapperVector;

    /// List of the edge's lanes geometrical information
    LaneWrapperVector myLaneGeoms;

    /**
     * A class to find the matching lane wrapper
     */
    class lane_wrapper_finder
    {
    public:
        /** constructor */
        explicit lane_wrapper_finder(const MSLane &lane) : myLane(lane) { }

        /** the comparing function */
        bool operator()(const GUILaneWrapper * const wrapper) {
            return wrapper->forLane(myLane);
        }

    private:
        /// The time to search for
        const MSLane &myLane;

    };


private:
    /// invalidated copy constructor
    GUIEdge(const GUIEdge &s);

    /// invalidated assignment operator
    GUIEdge &operator=(const GUIEdge &s);

};


#endif

/****************************************************************************/

