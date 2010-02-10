/****************************************************************************/
/// @file    GeomConvHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2003
/// @version $Id$
///
// }
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
GeomConvHelper::parseShape(const std::string &shpdef) {
    StringTokenizer st(shpdef, " ");
    if (shpdef=="") {
        throw EmptyData();
    }
    Position2DVector shape;
    while (st.hasNext()) {
        StringTokenizer pos(st.next(), ",");
        SUMOReal x = TplConvert<char>::_2SUMOReal(pos.next().c_str());
        SUMOReal y = TplConvert<char>::_2SUMOReal(pos.next().c_str());
        shape.push_back(Position2D(x, y));
    }
    return shape;
}


Boundary
GeomConvHelper::parseBoundary(const std::string &def) {
    StringTokenizer st(def, ",");
    if (st.size()!=4) {
        throw InvalidArgument("Could not parse '" + def + "' as boundary.");
    }
    SUMOReal xmin = TplConvert<char>::_2SUMOReal(st.next().c_str());
    SUMOReal ymin = TplConvert<char>::_2SUMOReal(st.next().c_str());
    SUMOReal xmax = TplConvert<char>::_2SUMOReal(st.next().c_str());
    SUMOReal ymax = TplConvert<char>::_2SUMOReal(st.next().c_str());
    return Boundary(xmin, ymin, xmax, ymax);
}



/****************************************************************************/

