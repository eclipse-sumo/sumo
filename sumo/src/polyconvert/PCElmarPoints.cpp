/***************************************************************************
                          PCElmarPoints.cpp
    A reader of pois stored in elmar-format
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Thu, 02.11.2006
    copyright            : (C) 2006 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2007/01/08 14:43:58  dkrajzew
// code beautifying; prliminary import for Visum points added
//
// Revision 1.2  2006/11/16 10:50:50  dkrajzew
// warnings removed
//
// Revision 1.1  2006/11/02 12:19:50  dkrajzew
// added parsing of Elmar's pointcollections
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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
#include "PCElmarPoints.h"
#include <utils/gfx/RGBColor.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/geoconv/GeoConvHelper.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method defintions
 * ======================================================================= */
PCElmarPoints::PCElmarPoints(PCPolyContainer &toFill,
			     const Boundary &/*netBoundary*/, PCTypeMap &tm)
    : myCont(toFill), myTypeMap(tm)
{
}


PCElmarPoints::~PCElmarPoints()
{
}


void
PCElmarPoints::load(OptionsCont &oc)
{
    RGBColor c = GfxConvHelper::parseColor(oc.getString("color"));
    std::string file = oc.getString("elmar-points");
    // load the pois
    ifstream out(file.c_str());
    if(!out) {
        MsgHandler::getErrorInstance()->inform("Can not open elmar-file '" + file + "'.");
        throw ProcessError();
    }

    // Attributes of the poi
    std::string name, desc, type, ort;
    Position2DVector vec;

    std::string buff, rest; // rest after doing substring
    std::string xpos, ypos;
    std::string tab = "\t";
    int l = 0;


    while(out.good()) {
        getline(out,buff);

        // do not parse comment lines
        if(buff.find("#") != string::npos) {
            continue;
        }
        // ... and empty lines
        if(StringUtils::prune(buff)=="") {
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
        GeoConvHelper::remap(pos);

        // check the poi
        if(name=="") {
            MsgHandler::getErrorInstance()->inform("The name of a poi is missing.");
            continue;
        }

        // patch the values
        bool discard = false;
        int layer = oc.getInt("layer");
        RGBColor color;
        if(myTypeMap.has(type)) {
            const PCTypeMap::TypeDef &def = myTypeMap.get(type);
            name = def.prefix + name;
            type = def.id;
            color = GfxConvHelper::parseColor(def.color);
            discard = def.discard;
            layer = def.layer;
        } else {
            name = oc.getString("prefix") + name;
            type = oc.getString("type");
            color = c;
        }
        if(!discard) {
            PointOfInterest *poi = new PointOfInterest(name, type, pos, color);
            myCont.insert(name, poi, layer);
        }
        vec.clear();
    }

    out.close();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



