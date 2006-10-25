#ifndef GLHelper_h
#define GLHelper_h
//---------------------------------------------------------------------------//
//                        GLHelper.h -
//  Some methods which help to draw certain geometrical objects in openGL
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
// Revision 1.11  2006/10/25 12:22:43  dkrajzew
// updated
//
// Revision 1.10  2006/08/01 07:34:28  dkrajzew
// API for drawing not filled circles
//
// Revision 1.7  2006/03/27 07:24:49  dksumo
// extracted drawing of lane geometries
//
// Revision 1.6  2006/03/08 13:16:13  dksumo
// some work on lane visualization
//
// Revision 1.5  2006/01/03 11:01:01  dksumo
// new visualization settings implemented
//
// Revision 1.4  2005/10/06 13:39:46  dksumo
// using of a configuration file rechecked
//
// Revision 1.3  2005/09/20 06:13:02  dksumo
// floats and doubles replaced by SUMOReal; warnings removed
//
// Revision 1.2  2005/09/09 12:54:48  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.1  2004/10/22 12:50:46  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.3  2004/07/02 09:46:28  dkrajzew
// some helper procedures for vss visualisation
//
// Revision 1.2  2003/10/02 14:55:58  dkrajzew
// visualisation of E2-detectors implemented
//
// Revision 1.1  2003/06/05 14:27:45  dkrajzew
// some helping functions added; Makefile added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <utility>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Line2D.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GLHelper {
public:
    /// Draws a filled polygon described by the list of points
    static void drawFilledPoly(const Position2DVector &v, bool close);

    static void drawBoxLine(const Position2D &beg, SUMOReal rot,
        SUMOReal visLength, SUMOReal width);

    static void drawBoxLine(const Position2D &beg1, const Position2D &beg2,
        SUMOReal rot, SUMOReal visLength, SUMOReal width);

    static void drawBoxLines(const Position2DVector &geom,
        const std::vector<SUMOReal> &rots, const std::vector<SUMOReal> &lengths,
        SUMOReal width);

    static void drawBoxLines(const Position2DVector &geom1,
        const Position2DVector &geom2,
        const std::vector<SUMOReal> &rots, const std::vector<SUMOReal> &lengths,
        SUMOReal width);

    static void drawStippleBoxLines(const Position2DVector &geom1,
        const std::vector<SUMOReal> &rots, const std::vector<SUMOReal> &lengths,
        SUMOReal width);

    static void drawBoxLines(const Position2DVector &geom, SUMOReal width);

    static void drawLine(const Position2D &beg, SUMOReal rot,
        SUMOReal visLength);

    static void drawLine(const Position2D &beg1, const Position2D &beg2,
        SUMOReal rot, SUMOReal visLength);

    static void drawFilledCircle(SUMOReal width, int steps=8);

    static void drawFilledCircle(SUMOReal width, int steps,
        SUMOReal beg, SUMOReal end);

    static void drawOutlineCircle(SUMOReal width, SUMOReal iwidth,
        int steps=8);

    static void drawOutlineCircle(SUMOReal width, SUMOReal iwidth,
        int steps, SUMOReal beg, SUMOReal end);

    static void drawTriangleAtEnd(const Line2D &l, float tLength,
        float tWidth);

private:
    static std::vector<std::pair<SUMOReal, SUMOReal> > myCircleCoords;

    static int myStippleTexture;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

