/****************************************************************************/
/// @file    PCElmar.cpp
/// @author  Danilot Teta Boyom
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// A reader of polygons stored in elmar-format
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
#include <utils/importio/LineReader.h>
#include <utils/common/StdDefs.h>
#include <utils/gfx/GfxConvHelper.h>
#include <polyconvert/PCPolyContainer.h>
#include "PCElmar.h"
#include <utils/gfx/RGBColor.h>
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
// method defintions
// ===========================================================================
PCElmar::PCElmar(PCPolyContainer &toFill,
                 PCTypeMap &tm)
        : myCont(toFill), myTypeMap(tm)
{}


PCElmar::~PCElmar()
{}


void
PCElmar::load(OptionsCont &oc)
{
    RGBColor c = GfxConvHelper::parseColor(oc.getString("color"));
    std::string file = oc.getString("elmar");
    // load the polygons
    ifstream out(file.c_str());
    if (!out) {
        throw ProcessError("Can not open elmar-file '" + file + "'.");
    }

    // Attributes of the polygon
    std::string id, name, type, ort;
    Position2DVector vec;

    std::string buff, rest; // rest after doing substring
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
        id = buff.substr(0,buff.find(tab));
        rest = buff.substr(buff.find(tab)+1, buff.length());

        ort = rest.substr(0,rest.find(tab));
        rest = rest.substr(rest.find(tab)+1, rest.length());

        type = rest.substr(0,rest.find(tab));
        rest = rest.substr(rest.find(tab)+1, rest.length());

        name = rest.substr(0,rest.find(tab));
        rest = rest.substr(rest.find(tab)+1, rest.length());

        std::string xpos, ypos;
        // now collect the positions
        while (rest.find(tab)!=string::npos) {
            xpos = rest.substr(0,rest.find(tab));
            rest = rest.substr(rest.find(tab)+1, rest.length());
            ypos = rest.substr(0,rest.find(tab));

            SUMOReal x = TplConvert<char>::_2SUMOReal(xpos.c_str());
            SUMOReal y = TplConvert<char>::_2SUMOReal(ypos.c_str());

            Position2D pos(x, y);
            GeoConvHelper::x2cartesian(pos);
            vec.push_back(pos);
            rest = rest.substr(rest.find(tab)+1);
        }

        name = StringUtils::convertUmlaute(name);
        if (name=="noname"||myCont.contains(name)) {
            name = name + "#" + toString(myCont.getEnumIDFor(name));
        }

        // check the polygon
        if (vec.size()==0) {
            MsgHandler::getWarningInstance()->inform("The polygon '" + id + "' is empty.");
            continue;
        }
        if (id=="") {
            MsgHandler::getErrorInstance()->inform("The name of a polygon is missing.");
            continue;
        }

        // patch the values
        bool fill = vec.getBegin()==vec.getEnd();
        bool discard = false;
        int layer = oc.getInt("layer");
        RGBColor color;
        if (myTypeMap.has(type)) {
            const PCTypeMap::TypeDef &def = myTypeMap.get(type);
            name = def.prefix + name;
            type = def.id;
            color = GfxConvHelper::parseColor(def.color);
            fill = fill && def.allowFill;
            discard = def.discard;
            layer = def.layer;
        } else {
            name = oc.getString("prefix") + name;
            type = oc.getString("type");
            color = c;
        }
        if (!discard) {
            Polygon2D *poly = new Polygon2D(name, type, color, vec, fill);
            myCont.insert(name, poly, layer);
        }
        vec.clear();
    }
    out.close();
}



/****************************************************************************/

