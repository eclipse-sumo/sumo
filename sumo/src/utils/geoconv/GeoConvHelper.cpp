/****************************************************************************/
/// @file    GeoConvHelper.cpp
/// @author  unknown_author
/// @date    unknown_date
/// @version $Id: $
///
// missing_desc
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

#include <map>
#include <proj_api.h>
#include "GeoConvHelper.h"
#include <utils/geom/GeomHelper.h>

projPJ GeoConvHelper::myProjection = 0;
Position2D GeoConvHelper::myOffset;
bool GeoConvHelper::myDisableProjection;
SUMOReal GeoConvHelper::myInitX;
SUMOReal GeoConvHelper::myInitY;

bool
GeoConvHelper::init(const std::string &proj,
                    const Position2D &offset)
{
    pj_free(myProjection);
    if (proj.length()==0||proj[0]=='!') {
        // use no projection
        myDisableProjection = true;
        return true;
    }
    if (proj[0]=='-') {
        // use a simple projection only
        myDisableProjection = false;
        myInitX = -1;
        myInitY = -1;
        return true;
    }
    // use full projection
    myDisableProjection = false;
    myProjection = pj_init_plus("+proj=utm +zone=33 +ellps=bessel +units=m");
    myOffset = offset;
    return myProjection!=0;
}


bool
GeoConvHelper::initialised()
{
    return myProjection!=0;
}


void
GeoConvHelper::close()
{
    pj_free(myProjection);
    myProjection = 0;
}


void
GeoConvHelper::cartesian2geo(Position2D &cartesian)
{
    if (myDisableProjection) {
        return;
    }
    projUV p;
    p.u = cartesian.x() - myOffset.x();
    p.v = cartesian.y() - myOffset.y();
    p = pj_inv(p, myProjection);
    p.u *= RAD_TO_DEG;
    p.v *= RAD_TO_DEG;
    cartesian.set((SUMOReal) p.u, (SUMOReal) p.v);
}


void
GeoConvHelper::remap(Position2D &from)
{
    if (myDisableProjection) {
        return;
    }
    projUV p;
    if (myProjection!=0) {
        p.u = from.x() / 100000.0 * DEG_TO_RAD;
        p.v = from.y() / 100000.0 * DEG_TO_RAD;
        p = pj_fwd(p, myProjection);
        from.set((SUMOReal) p.u + (SUMOReal) myOffset.x(), (SUMOReal) p.v + (SUMOReal) myOffset.y());
    } else {
        SUMOReal x = (SUMOReal)(from.x() / 100000.0);
        SUMOReal y = (SUMOReal)(from.y() / 100000.0);
        SUMOReal ys = y;
        if (myInitX==-1) {
            myInitX = x;
            myInitY = y;
        }
        x = (x-myInitX);
        y = (y-myInitY);
        p.u = (SUMOReal)(x * 111.320*1000.);
        p.v = (SUMOReal)(y * 111.136*1000.);
        p.u *= (SUMOReal) cos(ys*PI/180.0);
        /*!!! recheck whether the axes are mirrored
                p.v = (SUMOReal) (x * 111.320*1000.);
                SUMOReal y1 = (SUMOReal) (y * 111.136*1000.);
                p.u *= (SUMOReal) cos(ys*PI/180.0); // !!!
                */
        from.set((SUMOReal) p.u + (SUMOReal) myOffset.x(), (SUMOReal) p.v + (SUMOReal) myOffset.y());
    }
}



/****************************************************************************/

