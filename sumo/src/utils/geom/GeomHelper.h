#ifndef GeomHelper_h
#define GeomHelper_h
//---------------------------------------------------------------------------//
//                        GeomHelper.h -
//  Some geometrical helpers
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
// Revision 1.9  2003/11/18 14:21:20  dkrajzew
// computation of junction-inlanes geometry added
//
// Revision 1.8  2003/09/05 15:27:38  dkrajzew
// changes from adding internal lanes and further work on node geometry
//
// Revision 1.7  2003/08/14 14:04:21  dkrajzew
// extrapolation of a lines point added
//
// Revision 1.6  2003/07/16 15:38:04  dkrajzew
// some work on computation and handling of geometry information
//
// Revision 1.5  2003/07/07 08:47:42  dkrajzew
// added the possibility to compute the normal of a vector at 90deg to the vector
//
// Revision 1.4  2003/05/20 09:50:22  dkrajzew
// further work and debugging
//
// Revision 1.3  2003/04/07 12:22:30  dkrajzew
// first steps towards a junctions geometry
//
// Revision 1.2  2003/02/07 10:50:20  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "Position2D.h"
#include "Position2DVector.h"



#define PI 3.1415926535897932384626433832795

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GeomHelper {
public:
    static bool intersects(double x1b, double y1b, double x1e, double y1e,
        double x2b, double y2b, double x2e, double y2e);

    static bool intersects(const Position2D &p11, const Position2D &p12,
        const Position2D &p21, const Position2D &p22);

    /// Returns the distance between both points
    static double distance(const Position2D &p1, const Position2D &p2);
/*
    static Position2DVector::const_iterator
        find_intersecting_line(const Position2D &p1, const Position2D &p2,
        const Position2DVector &poly, Position2DVector::const_iterator beg);
*/
    /*
    static Position2D intersection_position(const Position2D &p1,
        const Position2D &p2, const Position2DVector &poly,
        Position2DVector::const_iterator at);
        */
    static Position2D intersection_position(const Position2D &p11,
        const Position2D &p12, const Position2D &p21, const Position2D &p22);

//    static bool isWithin(const Position2DVector &poly, const Position2D &p);
    static double Angle2D(double x1, double y1, double x2, double y2);
/*
    static Position2D position_at_length_position(
        const Position2DVector &poly, double pos);
        */
/*
    static Position2D position_at_length_position(const Position2D &p1,
        const Position2D &p2, double pos);
*/

    static Position2D interpolate(const Position2D &p1,
        const Position2D &p2, double length);

    static Position2D extrapolate_first(const Position2D &p1,
        const Position2D &p2, double length);

    static Position2D extrapolate_second(const Position2D &p1,
        const Position2D &p2, double length);

    static double nearest_position_on_line_to_point(
        const Position2D &l1, const Position2D &l2,
        const Position2D &p);

    /** by Damian Coventry */
    static double Magnitude(const Position2D &Point1,
        const Position2D &Point2 );
    /** by Damian Coventry */
    static double DistancePointLine(const Position2D &Point,
        const Position2D &LineStart, const Position2D &LineEnd);

    static Position2D transfer_to_side(Position2D &p,
        const Position2D &lineBeg, const Position2D &lineEnd,
        double amount);


    static Position2D crossPoint(const Boundery &b,
        const Position2DVector &v);

    static std::pair<double, double> getNormal90D_CW(double x1, double y1,
        double x2, double y2, double length, double wanted_offset);

    static std::pair<double, double> getNormal90D_CW(const Position2D &beg,
        const Position2D &end, double length, double wanted_offset);

    static std::pair<double, double> getNormal90D_CW(const Position2D &beg,
        const Position2D &end, double wanted_offset);

/*
    static void sortAsPolyCWByAngle(Position2DVector &p);

    class as_poly_cw_sorter {
    public:
        /// constructor
        explicit as_poly_cw_sorter(Position2D center);

    public:
        /// comparing operation
        int operator() (const Position2D &p1, const Position2D &p2) const;

    private:
        /// the edge to compute the relative angle of
        Position2D _center;

    };
*/


};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GeomHelper.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

