/****************************************************************************/
/// @file    PCElmarPoints.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 02.11.2006
/// @version $Id$
///
// A reader of pois stored in elmar-format
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

#include <string>
#include <map>
#include <fstream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/StdDefs.h>
#include <polyconvert/PCPolyContainer.h>
#include "PCElmarPoints.h"
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
PCElmarPoints::PCElmarPoints(PCPolyContainer &toFill,
                             PCTypeMap &tm)
        : myCont(toFill), myTypeMap(tm)
{}


PCElmarPoints::~PCElmarPoints()
{}


void
PCElmarPoints::load(OptionsCont &oc)
{
    std::string file = oc.getString("elmar-points");
    // load the pois
    ifstream out(file.c_str());
    if (!out) {
        throw ProcessError("Can not open elmar-file '" + file + "'.");
    }
    RGBColor c = RGBColor::parseColor(oc.getString("color"));

    // Attributes of the poi
    std::string name, desc, type, ort;

    std::string buff, rest; // rest after doing substring
    std::string xpos, ypos;
    std::string tab = "\t";
    int l = 0;


    while (out.good()) {
        getline(out,buff);

        // do not parse comment lines
        if (buff.find("#") != string::npos) {
            continue;
        }
        // ... and empty lines
        if (StringUtils::prune(buff)=="") {
            continue;
        }

        l++;
        name = buff.substr(0,buff.find(tab));
        rest = buff.substr(buff.find(tab)+1, buff.length());

        type = rest.substr(0,rest.find(tab));
        rest = rest.substr(rest.find(tab)+1, rest.length());

        desc = rest.substr(0,rest.find(tab));
        rest = rest.substr(rest.find(tab)+1, rest.length());

        xpos = rest.substr(0,rest.find(tab));
        rest = rest.substr(rest.find(tab)+1, rest.length());

        ypos = rest.substr(0,rest.find(tab));

        SUMOReal x = TplConvert<char>::_2SUMOReal(xpos.c_str());
        SUMOReal y = TplConvert<char>::_2SUMOReal(ypos.c_str());

        Position2D pos(x, y);
        GeoConvHelper::x2cartesian(pos);

        // check the poi
        if (name=="") {
            MsgHandler::getErrorInstance()->inform("The name of a poi is missing.");
            continue;
        }

        // patch the values
        bool discard = false;
        int layer = oc.getInt("layer");
        RGBColor color;
        if (myTypeMap.has(type)) {
            const PCTypeMap::TypeDef &def = myTypeMap.get(type);
            name = def.prefix + name;
            type = def.id;
            color = RGBColor::parseColor(def.color);
            discard = def.discard;
            layer = def.layer;
        } else {
            name = oc.getString("prefix") + name;
            type = oc.getString("type");
            color = c;
        }
        if (!discard) {
            bool ignorePrunning = false;
            if (OptionsCont::getOptions().isInStringVector("prune.ignore", name)) {
                ignorePrunning = true;
            }
            PointOfInterest *poi = new PointOfInterest(name, type, pos, color);
            myCont.insert(name, poi, layer, ignorePrunning);
        }
    }

    out.close();
}



/****************************************************************************/

