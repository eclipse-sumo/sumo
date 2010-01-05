/****************************************************************************/
/// @file    GeoConvHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-08-01
/// @version $Id$
///
// static methods for processing the coordinates conversion for the current net
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <cmath>
#include "GeoConvHelper.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/options/OptionsCont.h>

#ifdef HAVE_PROJ
#include <proj_api.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
std::string GeoConvHelper::myProjString = "!";
#ifdef HAVE_PROJ
projPJ GeoConvHelper::myProjection = 0;
#endif
Position2D GeoConvHelper::myOffset;
double GeoConvHelper::myGeoScale = 1.f;
GeoConvHelper::ProjectionMethod GeoConvHelper::myProjectionMethod = NONE;
bool GeoConvHelper::myUseInverseProjection = false;
bool GeoConvHelper::myBaseFound = false;
double GeoConvHelper::myBaseX = 0;
double GeoConvHelper::myBaseY = 0;
Boundary GeoConvHelper::myOrigBoundary;
Boundary GeoConvHelper::myConvBoundary;


// ===========================================================================
// method definitions
// ===========================================================================
void
GeoConvHelper::addProjectionOptions(OptionsCont &oc) {
    oc.addOptionSubTopic("Projection");

    oc.doRegister("proj.simple", new Option_Bool(false));
    oc.addDescription("proj.simple", "Projection", "Uses a simple method for projection");

    oc.doRegister("proj.shift", new Option_Integer(0));
    oc.addDescription("proj.shift", "Projection", "Number of places to shift decimal point to right in geo-coordinates");

#ifdef HAVE_PROJ
    oc.doRegister("proj.utm", new Option_Bool(false));
    oc.addDescription("proj.utm", "Projection", "Determine the UTM zone (for a universal transversal mercator projection based on the WGS84 ellipsoid)");

    oc.doRegister("proj.dhdn", new Option_Bool(false));
    oc.addDescription("proj.dhdn", "Projection", "Determine the DHDN zone (for a transversal mercator projection based on the bessel ellipsoid)");

    oc.doRegister("proj", new Option_String("!"));
    oc.addDescription("proj", "Projection", "Uses STR as proj.4 definition for projection");

    oc.doRegister("proj.inverse", new Option_Bool(false));
    oc.addDescription("proj.inverse", "Projection", "Inverses projection");
#endif
}


bool
GeoConvHelper::init(OptionsCont &oc) {
#ifdef HAVE_PROJ
    if (oc.getBool("proj.inverse") && oc.getString("proj") == "!") {
        MsgHandler::getErrorInstance()->inform("Inverse projection works only with explicit proj parameters.");
        return false;
    }
    unsigned numProjections = oc.getBool("proj.simple") + oc.getBool("proj.utm") + oc.getBool("proj.dhdn") + (oc.getString("proj").length() > 1);
    if (numProjections > 1) {
        MsgHandler::getErrorInstance()->inform("The projection method needs to be uniquely defined.");
        return false;
    }
#endif
    myOffset = Position2D(oc.getFloat("x-offset-to-apply"), oc.getFloat("y-offset-to-apply"));
    if (oc.getBool("proj.simple")) {
        return init("-", oc.getInt("proj.shift"));
    }
    bool ret = true;
#ifdef HAVE_PROJ
    if (oc.getBool("proj.utm")) {
        myProjectionMethod = UTM;
        ret = init(".", oc.getInt("proj.shift"));
    } else if (oc.getBool("proj.dhdn")) {
        myProjectionMethod = DHDN;
        ret = init(".", oc.getInt("proj.shift"));
    } else {
        ret = init(oc.getString("proj"), oc.getInt("proj.shift"), oc.getBool("proj.inverse"));
    }
#endif
    if (oc.exists("disable-normalize-node-positions") && oc.getBool("disable-normalize-node-positions")) {
        myBaseFound = true;
    }
    return ret;
}


bool
GeoConvHelper::init(const std::string &proj,
                    const int shift,
                    bool inverse) {
    myProjString = proj;
    myGeoScale = pow(10, (double)-shift);
    myUseInverseProjection = inverse;
    close();
    myBaseFound = false;
    myOrigBoundary.reset();
    myConvBoundary.reset();
    if (proj=="!") {
        myProjectionMethod = NONE;
        return true;
    }
    if (proj=="-") {
        myProjectionMethod = SIMPLE;
        return true;
    }
    if (proj==".") {
        return true;
    }
#ifdef HAVE_PROJ
    myProjection = pj_init_plus(proj.c_str());
    // !!! check pj_errno
    if (myProjection != 0) {
        myProjectionMethod = PROJ;
        return true;
    }
#endif
    return false;
}


bool
GeoConvHelper::init(const std::string &proj,
                    const Position2D &offset,
                    const Boundary &orig,
                    const Boundary &conv) {
    bool ret = init(proj);
    myOffset = offset;
    myOrigBoundary.add(orig);
    myConvBoundary.add(conv);
    return ret;
}


void
GeoConvHelper::close() {
#ifdef HAVE_PROJ
    if (myProjection != 0) {
        pj_free(myProjection);
    }
    myProjection = 0;
#endif
}


bool
GeoConvHelper::usingGeoProjection() {
    return myProjectionMethod != NONE;
}


bool
GeoConvHelper::usingInverseGeoProjection() {
    return myUseInverseProjection;
}


void
GeoConvHelper::cartesian2geo(Position2D &cartesian) {
    cartesian.sub(myOffset);
    if (myProjectionMethod == NONE) {
        return;
    }
#ifdef HAVE_PROJ
    projUV p;
    p.u = cartesian.x();
    p.v = cartesian.y();
    p = pj_inv(p, myProjection);
    //!!! check pj_errno
    p.u *= RAD_TO_DEG;
    p.v *= RAD_TO_DEG;
    cartesian.set((SUMOReal) p.u, (SUMOReal) p.v);
#endif
}


bool
GeoConvHelper::x2cartesian(Position2D &from, bool includeInBoundary, double x, double y) {
    myOrigBoundary.add(from);
    if (x == -1 && y == -1) {
        x = from.x();
        y = from.y();
    }
    if (myProjectionMethod == NONE) {
        from.add(myOffset);
    } else if (myUseInverseProjection) {
        cartesian2geo(from);
    } else {
        x *= myGeoScale;
        y *= myGeoScale;
        if (x > 180.1 || x < -180.1 || y > 90.1 || y < -90.1) {
            return false;
        }
#ifdef HAVE_PROJ
        if (myProjection==0) {
            if (myProjectionMethod == UTM) {
                int zone = (int)(x + 180) / 6 + 1;
                myProjString = "+proj=utm +zone=" + toString(zone) +
                               " +ellps=WGS84 +datum=WGS84 +units=m +no_defs";
                myProjection = pj_init_plus(myProjString.c_str());
                //!!! check pj_errno
            }
            if (myProjectionMethod == DHDN) {
                int zone = (int)(x / 3);
                if (zone < 1 || zone > 5) {
                    return false;
                }
                myProjString = "+proj=tmerc +lat_0=0 +lon_0=" + toString(3*zone) +
                               " +k=1 +x_0=" + toString(zone * 1000000 + 500000) +
                               " +y_0=0 +ellps=bessel +datum=potsdam +units=m +no_defs";
                myProjection = pj_init_plus(myProjString.c_str());
                //!!! check pj_errno
            }
        }
        if (myProjection!=0) {
            projUV p;
            p.u = x * DEG_TO_RAD;
            p.v = y * DEG_TO_RAD;
            p = pj_fwd(p, myProjection);
            //!!! check pj_errno
            x = p.u;
            y = p.v;
        }
#endif
        if (myProjectionMethod == SIMPLE) {
            double ys = y;
            if (!myBaseFound) {
                myBaseX = x;
                myBaseY = y;
                myBaseFound = true;
            }
            x -= myBaseX;
            y -= myBaseY;
            x *= 111320. * cos(ys*PI/180.0);
            y *= 111136.;
            from.set((SUMOReal)x, (SUMOReal)y);
            //!!! recheck whether the axes are mirrored
            from.add(myOffset);
        }
    }
    if (myProjectionMethod != SIMPLE) {
        if (!myBaseFound) {
            if (from.x() > 100000 || from.y() > 100000) {
                myBaseX = x;
                myBaseY = y;
            }
            myBaseFound = true;
        }
        if (myBaseFound) {
            x -= myBaseX;
            y -= myBaseY;
        }
        from.set((SUMOReal)x, (SUMOReal)y);
        from.add(myOffset);
    }
    if (includeInBoundary) {
        myConvBoundary.add(from);
    }
    return true;
}


void
GeoConvHelper::moveConvertedBy(SUMOReal x, SUMOReal y) {
    myOffset.add(x, y);
    myConvBoundary.moveby(x, y);
}


const Boundary &
GeoConvHelper::getOrigBoundary() {
    return myOrigBoundary;
}


const Boundary &
GeoConvHelper::getConvBoundary() {
    return myConvBoundary;
}


const Position2D
GeoConvHelper::getOffsetBase() {
    return Position2D(myOffset.x()-myBaseX, myOffset.y()-myBaseY);
}


const std::string &
GeoConvHelper::getProjString() {
    return myProjString;
}



/****************************************************************************/

