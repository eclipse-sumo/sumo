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
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
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
class GUIEdge : public MSEdge
{
public:
    /// constructor
    GUIEdge(std::string id);

    /// destructor
    ~GUIEdge();

    /// sets the junctions allowing the retrieval of the edge position (!!! why not private with a friend?)
    void initJunctions(MSJunction *from, MSJunction *to,
        GUIGlObjectStorage &idStorage);

    /// returns the names of all edges
    static std::vector<std::string> getNames();

    /// returns the x-position of the edge's end
    double toXPos() const;

    /// returns the x-position of the edge's begin
    double fromXPos() const;

    /// returns the y-position of the edge's end
    double toYPos() const;

    /// returns the y-position of the edge's begin
    double fromYPos() const;

    /// returns the id of the edge (!!! not already implemented in MSEdge?)
    std::string getID() const;

    /// returns the enumerated lane (!!! why not private with a friend?)
    MSLane &getLane(size_t laneNo);



    /// returns the enumerated lane's geometry (!!! why not private with a friend?)
    GUILaneWrapper &getLaneGeometry(size_t laneNo);

    GUILaneWrapper &getLaneGeometry(MSLane *lane);

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
        bool operator() (GUILaneWrapper *wrapper) {
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
//#ifndef DISABLE_INLINE
//#include "GUIEdge.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

