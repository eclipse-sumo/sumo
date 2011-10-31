/****************************************************************************/
/// @file    GeoConvHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-08-01
/// @version $Id$
///
// static methods for processing the coordinates conversion for the current net
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/options/OptionsCont.h>
#include "GeoConvHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
GeoConvHelper GeoConvHelper::myDefault("!", Position(), Boundary(), Boundary());
GeoConvHelper GeoConvHelper::myLoaded("!", Position(), Boundary(), Boundary());
GeoConvHelper GeoConvHelper::myOutput("!", Position(), Boundary(), Boundary());
int GeoConvHelper::myNumLoaded = 0;

// ===========================================================================
// method definitions
// ===========================================================================
GeoConvHelper::GeoConvHelper(const std::string& proj, const Position& offset,
                             const Boundary& orig, const Boundary& conv, int shift, bool inverse, bool baseFound):
    myProjString(proj),
#ifdef HAVE_PROJ
    myProjection(0),
#endif
    myOffset(offset),
    myProjectionMethod(NONE),
    myOrigBoundary(orig),
    myConvBoundary(conv),
    myGeoScale(pow(10, (double)-shift)),
    myUseInverseProjection(inverse),
    myBaseFound(baseFound),
    myBaseX(0),
    myBaseY(0) {
    if (proj == "!") {
        myProjectionMethod = NONE;
    } else if (proj == "-") {
        myProjectionMethod = SIMPLE;
    } else if (proj == "UTM") {
        myProjectionMethod = UTM;
    } else if (proj == "DHDN") {
        myProjectionMethod = DHDN;
#ifdef HAVE_PROJ
    } else {
        myProjectionMethod = PROJ;
        myProjection = pj_init_plus(proj.c_str());
        if (myProjection == 0) {
            // !!! check pj_errno
            throw ProcessError("Could not build projection!");
        }
#endif
    }
}


GeoConvHelper::~GeoConvHelper() {
#ifdef HAVE_PROJ
    if (myProjection != 0) {
        pj_free(myProjection);
    }
#endif
}


GeoConvHelper&
GeoConvHelper::operator=(const GeoConvHelper& orig) {
    myProjString = orig.myProjString;
    myOffset = orig.myOffset;
    myProjectionMethod = orig.myProjectionMethod;
    myOrigBoundary = orig.myOrigBoundary;
    myConvBoundary = orig.myConvBoundary;
    myGeoScale = orig.myGeoScale;
    myUseInverseProjection = orig.myUseInverseProjection;
    myBaseFound = orig.myBaseFound;
    myBaseX = orig.myBaseX;
    myBaseY = orig.myBaseY;
#ifdef HAVE_PROJ
    if (myProjection != 0) {
        pj_free(myProjection);
    }
    myProjection = (orig.myProjectionMethod == PROJ ? pj_init_plus(orig.myProjString.c_str()) : 0);
#endif
    return *this;
}


bool
GeoConvHelper::init(OptionsCont& oc) {
    std::string proj = "!"; // the default
    int shift = oc.getInt("proj.scale");
    Position offset = Position(oc.getFloat("offset.x"), oc.getFloat("offset.y"));
    bool inverse = oc.getBool("proj.inverse");
    bool baseFound = !oc.exists("offset.disable-normalization") ||
                     oc.getBool("offset.disable-normalization") ||
                     !oc.isDefault("offset.x") ||
                     !oc.isDefault("offset.y");

    if (oc.getBool("simple-projection")) {
        proj = "-";
    }

#ifdef HAVE_PROJ
    if (oc.getBool("proj.inverse") && oc.getString("proj") == "!") {
        WRITE_ERROR("Inverse projection works only with explicit proj parameters.");
        return false;
    }
    unsigned numProjections = oc.getBool("simple-projection") + oc.getBool("proj.utm") + oc.getBool("proj.dhdn") + (oc.getString("proj").length() > 1);
    if (numProjections > 1) {
        WRITE_ERROR("The projection method needs to be uniquely defined.");
        return false;
    }

    if (oc.getBool("proj.utm")) {
        proj = "UTM";
    } else if (oc.getBool("proj.dhdn")) {
        proj = "DHDN";
    } else {
        proj = oc.getString("proj");
    }
#endif
    myDefault = GeoConvHelper(proj, offset, Boundary(), Boundary(), shift, inverse, baseFound);
    return true;
}


void
GeoConvHelper::init(const std::string& proj,
                    const Position& offset,
                    const Boundary& orig,
                    const Boundary& conv) {
    myDefault = GeoConvHelper(proj, offset, orig, conv);
}


#ifdef HAVE_PROJ
void
GeoConvHelper::initProjection(double x, double y) {
    assert(myProjection == 0); // do not reinitialize

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
            WRITE_WARNING("Attempt to initialize DHDN-projection on invalid longitude " + toString(x));
            return;
        }
        myProjString = "+proj=tmerc +lat_0=0 +lon_0=" + toString(3*zone) +
                       " +k=1 +x_0=" + toString(zone * 1000000 + 500000) +
                       " +y_0=0 +ellps=bessel +datum=potsdam +units=m +no_defs";
        myProjection = pj_init_plus(myProjString.c_str());
        //!!! check pj_errno
    }
}
#endif


void
GeoConvHelper::addProjectionOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Projection");

    oc.doRegister("simple-projection", new Option_Bool(false));
    oc.addSynonyme("simple-projection", "proj.simple", true);
    oc.addDescription("simple-projection", "Projection", "Uses a simple method for projection");

    oc.doRegister("proj.scale", new Option_Integer(0));
    oc.addSynonyme("proj.scale", "proj.shift", true);
    oc.addDescription("proj.scale", "Projection", "Number of places to shift decimal point to right in geo-coordinates");

#ifdef HAVE_PROJ
    oc.doRegister("proj.utm", new Option_Bool(false));
    oc.addDescription("proj.utm", "Projection", "Determine the UTM zone (for a universal transversal mercator projection based on the WGS84 ellipsoid)");

    oc.doRegister("proj.dhdn", new Option_Bool(false));
    oc.addDescription("proj.dhdn", "Projection", "Determine the DHDN zone (for a transversal mercator projection based on the bessel ellipsoid)");

    oc.doRegister("proj", new Option_String("!"));
    oc.addDescription("proj", "Projection", "Uses STR as proj.4 definition for projection");

    oc.doRegister("proj.inverse", new Option_Bool(false));
    oc.addDescription("proj.inverse", "Projection", "Inverses projection");
#endif // HAVE_PROJ
}


bool
GeoConvHelper::usingGeoProjection() const {
    return myProjectionMethod != NONE;
}


bool
GeoConvHelper::usingInverseGeoProjection() const {
    return myUseInverseProjection;
}


void
GeoConvHelper::cartesian2geo(Position& cartesian) {
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
GeoConvHelper::x2cartesian(Position& from, bool includeInBoundary) {
    myOrigBoundary.add(from);
    double x = from.x();
    double y = from.y();
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
            initProjection(x, y);
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
            if (x > 100000 || y > 100000) {
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


const Boundary&
GeoConvHelper::getOrigBoundary() const {
    return myOrigBoundary;
}


const Boundary&
GeoConvHelper::getConvBoundary() const {
    return myConvBoundary;
}


const Position
GeoConvHelper::getOffset() const {
    return myOffset;
}


const Position
GeoConvHelper::getOffsetBase() const {
    return Position(myOffset.x()-myBaseX, myOffset.y()-myBaseY);
}


const std::string&
GeoConvHelper::getProjString() const {
    return myProjString;
}


const GeoConvHelper&
GeoConvHelper:: getOutputInstance() {
    if (myNumLoaded == 0) {
        return myDefault;
    } else if (myNumLoaded > 1) {
        WRITE_WARNING("Multiple location elements have been loaded. Check output location for correctness");
    }
    myOutput = GeoConvHelper(
                   // prefer options over loaded location
                   myDefault.usingGeoProjection() ? myDefault.getProjString() : myLoaded.getProjString(),
                   // let offset and boundary lead back to the original coords of the loaded data
                   myDefault.getOffset() + myLoaded.getOffset(),
                   myLoaded.getOrigBoundary(),
                   // the new boundary (updated during loading)
                   myDefault.getConvBoundary());
    return myOutput;
}

/****************************************************************************/

