/****************************************************************************/
/// @file    GeomConvHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2003
/// @version $Id$
///
// Some helping functions for geometry parsing
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

#include <string>
#include <sstream>
#include <utils/geom/Position2DVector.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include "GeomConvHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
Position2DVector
GeomConvHelper::parseShapeReporting(const std::string &shpdef, const std::string &objecttype,
                                    const char *objectid, bool &ok, bool allowEmpty, bool report) throw() {
    if (shpdef=="") {
        if (!allowEmpty) {
            emitError(report, "Shape", objecttype, objectid, "the shape is empty");
            ok = false;
        }
        return Position2DVector();
    }
    StringTokenizer st(shpdef, " ");
    Position2DVector shape;
    while (st.hasNext()) {
        StringTokenizer pos(st.next(), ",");
        if (pos.size()!=2 && pos.size()!=3) {
            emitError(report, "Shape", objecttype, objectid, "the position is neither x,y nor x,y,z");
            ok = false;
            return Position2DVector();
        }
        try {
            SUMOReal x = TplConvert<char>::_2SUMOReal(pos.next().c_str());
            SUMOReal y = TplConvert<char>::_2SUMOReal(pos.next().c_str());
            if(pos.size()==2) {
                shape.push_back(Position2D(x, y));
            } else {
                SUMOReal z = TplConvert<char>::_2SUMOReal(pos.next().c_str());
                shape.push_back(Position2D(x, y, z));
            }
        } catch (NumberFormatException &) {
            emitError(report, "Shape", objecttype, objectid, "not numeric position entry");
            ok = false;
            return Position2DVector();
        } catch (EmptyData &) {
            emitError(report, "Shape", objecttype, objectid, "empty position entry");
            ok = false;
            return Position2DVector();
        }
    }
    return shape;
}


Boundary
GeomConvHelper::parseBoundaryReporting(const std::string &def, const std::string &objecttype,
                                       const char *objectid, bool &ok, bool report) throw() {
    StringTokenizer st(def, ",");
    if (st.size()!=4) {
        emitError(report, "Bounding box", objecttype, objectid, "mismatching entry number");
        ok = false;
        return Boundary();
    }
    try {
        SUMOReal xmin = TplConvert<char>::_2SUMOReal(st.next().c_str());
        SUMOReal ymin = TplConvert<char>::_2SUMOReal(st.next().c_str());
        SUMOReal xmax = TplConvert<char>::_2SUMOReal(st.next().c_str());
        SUMOReal ymax = TplConvert<char>::_2SUMOReal(st.next().c_str());
        return Boundary(xmin, ymin, xmax, ymax);
    } catch (NumberFormatException &) {
        emitError(report, "Shape", objecttype, objectid, "not numeric entry");
    } catch (EmptyData &) {
        emitError(report, "Shape", objecttype, objectid, "empty entry");
    }
    ok = false;
    return Boundary();
}


void
GeomConvHelper::emitError(bool report, const std::string &what, const std::string &objecttype,
                          const char *objectid, const std::string &desc) throw() {
    if (!report) {
        return;
    }
    std::ostringstream oss;
    oss << what << " of ";
    if (objectid==0) {
        oss << "a(n) ";
    }
    oss << objecttype;
    if (objectid!=0) {
        oss << " '" << objectid << "'";
    }
    oss << " is broken: " << desc << ".";
    MsgHandler::getErrorInstance()->inform(oss.str());
}



/****************************************************************************/

