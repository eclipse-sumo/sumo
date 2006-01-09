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
// Revision 1.7  2006/01/09 13:36:09  dkrajzew
// further visualization options added
//
// Revision 1.6  2005/10/07 11:44:40  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/23 06:07:25  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:18:45  dkrajzew
// LARGE CODE RECHECK
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

    static void drawLine(const Position2D &beg, SUMOReal rot,
        SUMOReal visLength);

    static void drawFilledCircle(SUMOReal width, int steps=8);

    static void drawFilledCircle(SUMOReal width, int steps,
        SUMOReal beg, SUMOReal end);

    static void drawTriangleAtEnd(const Line2D &l, float tLength,
        float tWidth);

private:
    static std::vector<std::pair<SUMOReal, SUMOReal> > myCircleCoords;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

