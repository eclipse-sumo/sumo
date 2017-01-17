/****************************************************************************/
/// @file    GeomHelper.h
/// @author  Daniel Krajzewicz
/// @author  Friedemann Wesner
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Some static methods performing geometrical operations
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GeomHelper_h
#define GeomHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cmath>
#include "Position.h"
#include "PositionVector.h"
#include <utils/common/UtilExceptions.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define DEG2RAD(x) static_cast<SUMOReal>((x) * M_PI / 180.)
#define RAD2DEG(x) static_cast<SUMOReal>((x) * 180. / M_PI)


// ===========================================================================
// class definitions
// ===========================================================================
/** @class GeomHelper
 * @brief Some static methods performing geometrical operations
 */
class GeomHelper {

public:
    /// @brief a value to signify offsets outside the range of [0, Line.length()]
    static const SUMOReal INVALID_OFFSET;

    /** @brief Returns the positions the given circle is crossed by the given line
     * @param[in] c The center position of the circle
     * @param[in] radius The radius of the circle
     * @param[in] p1 The begin of the line
     * @param[in] p2 The end of the line
     * @param[filled] into The list of crossing positions (0-1 along the line's length)
     * @see http://blog.csharphelper.com/2010/03/28/determine-where-a-line-intersects-a-circle-in-c.aspx
     * @see http://gamedev.stackexchange.com/questions/18333/circle-line-collision-detection-problem (jazzdawg)
     */
    static void findLineCircleIntersections(const Position& c, SUMOReal radius, const Position& p1, const Position& p2,
                                            std::vector<SUMOReal>& into);


    /** @brief Returns the angle between two vectors on a plane
       The angle is from vector 1 to vector 2, positive anticlockwise
       The result is between -pi and pi
    */
    static SUMOReal angle2D(const Position& p1, const Position& p2);

    static SUMOReal nearest_offset_on_line_to_point2D(
        const Position& lineStart, const Position& lineEnd,
        const Position& p, bool perpendicular = true);

    static Position crossPoint(const Boundary& b,
                               const PositionVector& v);

    /** @brief Returns the distance of second angle from first angle counter-clockwise
     * @param[in] angle1 The first angle
     * @param[in] angle2 The second angle
     * @return Angle (counter-clockwise) starting from first to second angle
     */
    static SUMOReal getCCWAngleDiff(SUMOReal angle1, SUMOReal angle2);


    /** @brief Returns the distance of second angle from first angle clockwise
     * @param[in] angle1 The first angle
     * @param[in] angle2 The second angle
     * @return Angle (clockwise) starting from first to second angle
     */
    static SUMOReal getCWAngleDiff(SUMOReal angle1, SUMOReal angle2);


    /** @brief Returns the minimum distance (clockwise/counter-clockwise) between both angles
     * @param[in] angle1 The first angle
     * @param[in] angle2 The second angle
     * @return The minimum distance between both angles
     */
    static SUMOReal getMinAngleDiff(SUMOReal angle1, SUMOReal angle2);


    /** @brief Returns the difference of the second angle to the first angle in radiants
     *
     * The results are always between -pi and pi.
     * Positive values denote that the second angle is counter clockwise closer, negative values mean
     * it is clockwise closer.
     * @param[in] angle1 The first angle
     * @param[in] angle2 The second angle
     * @return angle starting from first to second angle
     */
    static SUMOReal angleDiff(const SUMOReal angle1, const SUMOReal angle2);


    /** Converts an angle from mathematical radians where 0 is to the right and positive angles
     *  are counterclockwise to navigational degrees where 0 is up and positive means clockwise.
     *  The result is always in the range [0, 360).
     * @param[in] angle The angle in radians to convert
     * @return the angle in degrees
     */
    static SUMOReal naviDegree(const SUMOReal angle);

    /** Converts an angle from mathematical radians where 0 is to the right and positive angles
     *  are counterclockwise to the legacy degrees used in sumo where 0 is down and positive means clockwise.
     *  If positive is true the result is in the range [0, 360), otherwise in the range [-180, 180).
     * @param[in] angle The angle in radians to convert
     * @return the angle in degrees
     */
    static SUMOReal legacyDegree(const SUMOReal angle, const bool positive = false);

};


#endif

/****************************************************************************/
