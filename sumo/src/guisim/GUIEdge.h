#ifndef GUIEdge_h
#define GUIEdge_h
//---------------------------------------------------------------------------//
//                        GUIEdge.h -
//  An MSEdge extended by values needed for the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.12  2004/04/02 11:20:35  dkrajzew
// changes needed to visualise the selection status
//
// Revision 1.11  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.10  2004/01/26 06:59:37  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics; different detector visualistaion in dependence to his controller
//
// Revision 1.9  2003/11/11 08:13:23  dkrajzew
// consequent usage of Position2D instead of two doubles
//
// Revision 1.8  2003/09/22 12:38:51  dkrajzew
// detectors need const Lanes
//
// Revision 1.7  2003/09/05 14:58:50  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.6  2003/07/22 14:59:27  dkrajzew
// changes due to new detector handling
//
// Revision 1.5  2003/07/16 15:24:55  dkrajzew
// GUIGrid now handles the set of things to draw in another manner than GUIEdgeGrid did; Further things to draw implemented
//
// Revision 1.4  2003/03/17 14:09:10  dkrajzew
// Windows eol removed
//
// Revision 1.3  2003/03/12 16:52:06  dkrajzew
// centering of objects debuggt
//
// Revision 1.2  2003/02/07 10:39:17  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundery.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <gui/GUIGlObject.h>
#include "GUILaneWrapper.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSJunction;
class GUILane;
class GUIBasicLane;
class GUIGlObjectStorage;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This is the gui-version of the MSEgde
 */
class GUIEdge : public MSEdge, public GUIGlObject
{
public:
    /// constructor
    GUIEdge(std::string id, GUIGlObjectStorage &idStorage);

    /// destructor
    ~GUIEdge();

    /// sets the junctions allowing the retrieval of the edge position (!!! why not private with a friend?)
    void initJunctions(MSJunction *from, MSJunction *to,
        GUIGlObjectStorage &idStorage);

    /** Returns the list of all known junctions as their names */
    static std::vector<std::string> getNames();

    /** Returns the list of all known junctions as their ids */
    static std::vector<size_t> getIDs();

    /// Returns the street's geometry
    Boundery getBoundery() const;

    /// returns the id of the edge (!!! not already implemented in MSEdge?)
    std::string getID() const;

    /// returns the enumerated lane (!!! why not private with a friend?)
    MSLane &getLane(size_t laneNo);



    /// returns the enumerated lane's geometry (!!! why not private with a friend?)
    GUILaneWrapper &getLaneGeometry(size_t laneNo) const;

    GUILaneWrapper &getLaneGeometry(const MSLane *lane) const;

    /** returns the position on the line given by the coordinates where "prev"
        is the length of the line and "wanted" the distance from the begin
        !!! should be within another class */
    static std::pair<double, double> getLaneOffsets(double x1, double y1,
        double x2, double y2, double prev, double wanted);

    /** @brief Initialize the edge.
        A GUILaneChanger is build here */
    void initialize(
        AllowedLanesCont* allowed, MSLane* departLane, LaneCont* lanes,
        EdgeBasicFunction function);

    /// Returns the given position on the given lane
    Position2D getLanePosition(const MSLane &lane, double pos) const;

    static void fill(std::vector<GUIEdge*> &netsWrappers);


    /// Returns an own popup-menu
    virtual GUIGLObjectPopupMenu *getPopUpMenu(
        GUIApplicationWindow &app, GUISUMOAbstractView &parent)
        { throw 1; }

    /// Returns an own parameter window
    virtual GUIParameterTableWindow *getParameterWindow(
        GUIApplicationWindow &app, GUISUMOAbstractView &parent)
        { throw 1; }

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    bool active() const;

private:

    /// the origin and the destination junction
    MSJunction *_from, *_to;

    /// Definition of the lane's positions vector
    typedef std::vector<GUILaneWrapper*> LaneWrapperVector;

    /// List of the edge's lanes geometrical information
    LaneWrapperVector _laneGeoms;

    /**
     * A class to find the matching lane wrapper
     */
    class lane_wrapper_finder {
    public:
        /** constructor */
        explicit lane_wrapper_finder(const MSLane &lane) : myLane(lane) { }

        /** the comparing function */
        bool operator() (const GUILaneWrapper * const wrapper) {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

