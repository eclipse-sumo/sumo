/****************************************************************************/
/// @file    GeoConvHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    2006-08-01
/// @version $Id$
///
// static methods for processing the coordinates conversion for the current net
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <climits>
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
GeoConvHelper GeoConvHelper::myProcessing("!", Position(), Boundary(), Boundary());
GeoConvHelper GeoConvHelper::myLoaded("!", Position(), Boundary(), Boundary());
GeoConvHelper GeoConvHelper::myFinal("!", Position(), Boundary(), Boundary());
int GeoConvHelper::myNumLoaded = 0;

// ===========================================================================
// method definitions
// ===========================================================================
GeoConvHelper::GeoConvHelper(const std::string& proj, const Position& offset,
                             const Boundary& orig, const Boundary& conv, int shift, bool inverse):
    myProjString(proj),
#ifdef HAVE_PROJ
    myProjection(0),
    myInverseProjection(0),
    myGeoProjection(0),
#endif
    myOffset(offset),
    myGeoScale(pow(10, (double) - shift)),
    myProjectionMethod(NONE),
    myUseInverseProjection(inverse),
    myOrigBoundary(orig),
    myConvBoundary(conv) {
    if (proj == "!") {
        myProjectionMethod = NONE;
    } else if (proj == "-") {
        myProjectionMethod = SIMPLE;
    } else if (proj == "UTM") {
        myProjectionMethod = UTM;
    } else if (proj == "DHDN") {
        myProjectionMethod = DHDN;
    } else if (proj == "DHDN_UTM") {
        myProjectionMethod = DHDN_UTM;
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
    if (myInverseProjection != 0) {
        pj_free(myInverseProjection);
    }
    if (myGeoProjection != 0) {
        pj_free(myInverseProjection);
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
#ifdef HAVE_PROJ
    if (myProjection != 0) {
        pj_free(myProjection);
        myProjection = 0;
    }
    if (myInverseProjection != 0) {
        pj_free(myInverseProjection);
        myInverseProjection = 0;
    }
    if (myGeoProjection != 0) {
        pj_free(myGeoProjection);
        myGeoProjection = 0;
    }
    if (orig.myProjection != 0) {
        myProjection = pj_init_plus(orig.myProjString.c_str());
    }
    if (orig.myInverseProjection != 0) {
        myInverseProjection = pj_init_plus(pj_get_def(orig.myInverseProjection, 0));
    }
    if (orig.myGeoProjection != 0) {
        myGeoProjection = pj_init_plus(pj_get_def(orig.myGeoProjection, 0));
    }
#endif
    return *this;
}


bool
GeoConvHelper::init(OptionsCont& oc) {
    std::string proj = "!"; // the default
    int shift = oc.getInt("proj.scale");
    Position offset = Position(oc.getFloat("offset.x"), oc.getFloat("offset.y"));
    bool inverse = oc.exists("proj.inverse") && oc.getBool("proj.inverse");

    if (oc.getBool("simple-projection")) {
        proj = "-";
    }

#ifdef HAVE_PROJ
    if (oc.getBool("proj.inverse") && oc.getString("proj") == "!") {
        WRITE_ERROR("Inverse projection works only with explicit proj parameters.");
        return false;
    }
    unsigned numProjections = oc.getBool("simple-projection") + oc.getBool("proj.utm") + oc.getBool("proj.dhdn") + oc.getBool("proj.dhdnutm") + (oc.getString("proj").length() > 1);
    if (numProjections > 1) {
        WRITE_ERROR("The projection method needs to be uniquely defined.");
        return false;
    }

    if (oc.getBool("proj.utm")) {
        proj = "UTM";
    } else if (oc.getBool("proj.dhdn")) {
        proj = "DHDN";
    } else if (oc.getBool("proj.dhdnutm")) {
        proj = "DHDN_UTM";
    } else {
        proj = oc.getString("proj");
    }
#endif
    myProcessing = GeoConvHelper(proj, offset, Boundary(), Boundary(), shift, inverse);
    myFinal = myProcessing;
    return true;
}


void
GeoConvHelper::init(const std::string& proj,
                    const Position& offset,
                    const Boundary& orig,
                    const Boundary& conv,
                    int shift) {
    myProcessing = GeoConvHelper(proj, offset, orig, conv, shift);
    myFinal = myProcessing;
}


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
    oc.addDescription("proj.dhdn", "Projection", "Determine the DHDN zone (for a transversal mercator projection based on the bessel ellipsoid, \"Gauss-Krueger\")");

    oc.doRegister("proj", new Option_String("!"));
    oc.addDescription("proj", "Projection", "Uses STR as proj.4 definition for projection");

    oc.doRegister("proj.inverse", new Option_Bool(false));
    oc.addDescription("proj.inverse", "Projection", "Inverses projection");

    oc.doRegister("proj.dhdnutm", new Option_Bool(false));
    oc.addDescription("proj.dhdnutm", "Projection", "Convert from Gauss-Krueger to UTM");
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
GeoConvHelper::cartesian2geo(Position& cartesian) const {
    cartesian.sub(getOffsetBase());
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
    if (includeInBoundary) {
        myOrigBoundary.add(from);
    }
    // init projection parameter on first use
#ifdef HAVE_PROJ
    if (myProjection == 0) {
        double x = from.x() * myGeoScale;
        switch (myProjectionMethod) {
            case DHDN_UTM: {
                int zone = (int)((x - 500000.) / 1000000.);
                if (zone < 1 || zone > 5) {
                    WRITE_WARNING("Attempt to initialize DHDN_UTM-projection on invalid longitude " + toString(x));
                    return false;
                }
                myProjString = "+proj=tmerc +lat_0=0 +lon_0=" + toString(3 * zone) +
                               " +k=1 +x_0=" + toString(zone * 1000000 + 500000) +
                               " +y_0=0 +ellps=bessel +datum=potsdam +units=m +no_defs";
                myInverseProjection = pj_init_plus(myProjString.c_str());
                myGeoProjection = pj_init_plus("+proj=latlong +datum=WGS84");
                //!!! check pj_errno
                x = ((x - 500000.) / 1000000.) * 3; // continues with UTM
            }
            case UTM: {
                int zone = (int)(x + 180) / 6 + 1;
                myProjString = "+proj=utm +zone=" + toString(zone) +
                               " +ellps=WGS84 +datum=WGS84 +units=m +no_defs";
                myProjection = pj_init_plus(myProjString.c_str());
                //!!! check pj_errno
            }
            break;
            case DHDN: {
                int zone = (int)(x / 3);
                if (zone < 1 || zone > 5) {
                    WRITE_WARNING("Attempt to initialize DHDN-projection on invalid longitude " + toString(x));
                    return false;
                }
                myProjString = "+proj=tmerc +lat_0=0 +lon_0=" + toString(3 * zone) +
                               " +k=1 +x_0=" + toString(zone * 1000000 + 500000) +
                               " +y_0=0 +ellps=bessel +datum=potsdam +units=m +no_defs";
                myProjection = pj_init_plus(myProjString.c_str());
                //!!! check pj_errno
            }
            break;
            default:
                break;
        }
    }
    if (myInverseProjection != 0) {
        double x = from.x();
        double y = from.y();
        if (pj_transform(myInverseProjection, myGeoProjection, 1, 1, &x, &y, NULL)) {
            WRITE_WARNING("Could not transform (" + toString(x) + "," + toString(y) + ")");
        }
        from.set(SUMOReal(x * RAD_TO_DEG), SUMOReal(y * RAD_TO_DEG));
    }
#endif
    // perform conversion
    bool ok = x2cartesian_const(from);
    if (ok) {
        if (includeInBoundary) {
            myConvBoundary.add(from);
        }
    }
    return ok;
}


bool
GeoConvHelper::x2cartesian_const(Position& from) const {
    double x = from.x() * myGeoScale;
    double y = from.y() * myGeoScale;
    if (myProjectionMethod == NONE) {
        from.add(myOffset);
    } else if (myUseInverseProjection) {
        cartesian2geo(from);
    } else {
        if (x > 180.1 || x < -180.1 || y > 90.1 || y < -90.1) {
            return false;
        }
#ifdef HAVE_PROJ
        if (myProjection != 0) {
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
            x *= 111320. * cos(DEG2RAD(ys));
            y *= 111136.;
            from.set((SUMOReal)x, (SUMOReal)y);
            //!!! recheck whether the axes are mirrored
            from.add(myOffset);
        }
    }
    if (x > std::numeric_limits<double>::max() ||
            y > std::numeric_limits<double>::max()) {
        return false;
    }
    if (myProjectionMethod != SIMPLE) {
        from.set((SUMOReal)x, (SUMOReal)y);
        from.add(myOffset);
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
    return myOffset;
}


const std::string&
GeoConvHelper::getProjString() const {
    return myProjString;
}


void
GeoConvHelper::computeFinal() {
    if (myNumLoaded == 0) {
        myFinal = myProcessing;
    } else  {
        myFinal = GeoConvHelper(
                      // prefer options over loaded location
                      myProcessing.usingGeoProjection() ? myProcessing.getProjString() : myLoaded.getProjString(),
                      // let offset and boundary lead back to the original coords of the loaded data
                      myProcessing.getOffset() + myLoaded.getOffset(),
                      myLoaded.getOrigBoundary(),
                      // the new boundary (updated during loading)
                      myProcessing.getConvBoundary());
    }
}


void
GeoConvHelper::setLoaded(const GeoConvHelper& loaded) {
    myNumLoaded++;
    if (myNumLoaded > 1) {
        WRITE_WARNING("Ignoring loaded location attribute nr. " + toString(myNumLoaded) + " for tracking of original location");
    } else {
        myLoaded = loaded;
    }
}


void
GeoConvHelper::resetLoaded() {
    myNumLoaded = 0;
}


/****************************************************************************/

