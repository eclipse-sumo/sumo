/****************************************************************************/
/// @file    GeoConvHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-08-01
/// @version $Id$
///
// static methods for processing the coordinates conversion for the current net
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include "GeoConvHelper.h"
#include <utils/geom/GeomHelper.h>

#ifdef HAVE_PROJ
#include <proj_api.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
#ifdef HAVE_PROJ
projPJ GeoConvHelper::myProjection = 0;
#endif
Position2D GeoConvHelper::myOffset;
bool GeoConvHelper::myDisableProjection = true;
bool GeoConvHelper::myUseInverseProjection = true;
SUMOReal GeoConvHelper::myInitX;
SUMOReal GeoConvHelper::myInitY;
Boundary GeoConvHelper::myOrigBoundary;
Boundary GeoConvHelper::myConvBoundary;


// ===========================================================================
// method definitions
// ===========================================================================
bool
GeoConvHelper::init(const std::string &proj,
                    const Position2D &offset,
                    bool inverse)
{
    myUseInverseProjection = inverse;
#ifdef HAVE_PROJ
    pj_free(myProjection);
#endif
    myOffset = offset;
    myOrigBoundary.reset();
    myConvBoundary.reset();
    myDisableProjection = false;
    if (proj.length()==0||proj[0]=='!') {
        // use no projection
        myDisableProjection = true;
        return true;
    }
    if (proj[0]=='-') {
        // use a simple projection only
        myInitX = -1;
        myInitY = -1;
        return true;
    }
#ifdef HAVE_PROJ
    // use full projection
    myProjection = pj_init_plus(proj.c_str());
    return myProjection!=0;
#else
    // use a simple projection only
    myInitX = -1;
    myInitY = -1;
    return true;
#endif
}


bool
GeoConvHelper::init(const std::string &proj,
                    const Position2D &offset,
                    const Boundary &orig,
                    const Boundary &conv,
                    bool inverse)
{
    myUseInverseProjection = inverse;
    bool ret = init(proj, offset);
    myOrigBoundary.add(orig);
    myConvBoundary.add(conv);
    return ret;
}


void
GeoConvHelper::close()
{
#ifdef HAVE_PROJ
    pj_free(myProjection);
    myProjection = 0;
#endif
}


bool
GeoConvHelper::usingGeoProjection()
{
#ifdef HAVE_PROJ
    return myProjection!=0;
#else
    return false;
#endif
}


void
GeoConvHelper::cartesian2geo(Position2D &cartesian)
{
#ifdef HAVE_PROJ
    if (myDisableProjection) {
        cartesian.sub(myOffset);
        return;
    }
    projUV p;
    p.u = cartesian.x() - myOffset.x();
    p.v = cartesian.y() - myOffset.y();
    p = pj_inv(p, myProjection);
    p.u *= RAD_TO_DEG;
    p.v *= RAD_TO_DEG;
    cartesian.set((SUMOReal) p.u, (SUMOReal) p.v);
#else
    cartesian.sub(myOffset);
#endif
}


void
GeoConvHelper::x2cartesian(Position2D &from, bool includeInBoundary)
{
    myOrigBoundary.add(from);
    if (myDisableProjection) {
        from.add(myOffset);
        if (includeInBoundary) {
            myConvBoundary.add(from);
        }
        return;
    }
#ifdef HAVE_PROJ
    if (myProjection!=0) {
        projUV p;
        if (!myUseInverseProjection) {
            // small, tiny method to norm the values properly
            if (from.x()>360.&&from.y()>360.) {
                p.u = from.x() / 100000.0 * DEG_TO_RAD;
                p.v = from.y() / 100000.0 * DEG_TO_RAD;
            } else {
                p.u = from.x() * DEG_TO_RAD;
                p.v = from.y() * DEG_TO_RAD;
            }
            p = pj_fwd(p, myProjection);
        } else {
            p.u = from.x();
            p.v = from.y();
            p = pj_inv(p, myProjection);
            p.u *= 100000.0 * RAD_TO_DEG;
            p.v *= 100000.0 * RAD_TO_DEG;
        }
        from.set((SUMOReal) p.u + (SUMOReal) myOffset.x(), (SUMOReal) p.v + (SUMOReal) myOffset.y());
    } else {
#endif
        SUMOReal x = (SUMOReal)(from.x() / 100000.0);
        SUMOReal y = (SUMOReal)(from.y() / 100000.0);
        SUMOReal ys = y;
        if (myInitX==-1) {
            myInitX = x;
            myInitY = y;
        }
        x = (x-myInitX);
        y = (y-myInitY);
        SUMOReal u = (SUMOReal)(x * 111.320*1000.);
        SUMOReal v = (SUMOReal)(y * 111.136*1000.);
        u *= (SUMOReal) cos(ys*PI/180.0);
        /*!!! recheck whether the axes are mirrored
                p.v = (SUMOReal) (x * 111.320*1000.);
                SUMOReal y1 = (SUMOReal) (y * 111.136*1000.);
                p.u *= (SUMOReal) cos(ys*PI/180.0); // !!!
                */
        from.set(u + (SUMOReal) myOffset.x(), v + (SUMOReal) myOffset.y());
#ifdef HAVE_PROJ
    }
#endif
    if (includeInBoundary) {
        myConvBoundary.add(from);
    }
}


void
GeoConvHelper::includeInOriginal(SUMOReal x, SUMOReal y)
{
    myOrigBoundary.add(x, y);
}


void
GeoConvHelper::includeInOriginal(const Position2D &p)
{
    myOrigBoundary.add(p);
}


void
GeoConvHelper::includeInOriginal(const Boundary &b)
{
    myOrigBoundary.add(b);
}


void
GeoConvHelper::moveConvertedBy(SUMOReal x, SUMOReal y)
{
    myOffset.add(x, y);
    myConvBoundary.moveby(x, y);
}


const Boundary &
GeoConvHelper::getOrigBoundary()
{
    return myOrigBoundary;
}


const Boundary &
GeoConvHelper::getConvBoundary()
{
    return myConvBoundary;
}


const Position2D &
GeoConvHelper::getOffset()
{
    return myOffset;
}



/****************************************************************************/

