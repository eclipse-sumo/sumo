/****************************************************************************/
/// @file    GLHelper.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// Some methods which help to draw certain geometrical objects in openGL
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
#ifndef GLHelper_h
#define GLHelper_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utility>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Line2D.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class GLHelper
{
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


#endif

/****************************************************************************/

