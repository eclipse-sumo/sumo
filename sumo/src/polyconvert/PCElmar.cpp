/***************************************************************************
                          PCElmar.cpp
    A reader of polygons stored in elmar-format
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Mon, 05 Dec 2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Danilo Boyom
    email                : Danilot.Tete-Boyom@dlr.de
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
// Revision 1.5  2006/11/16 10:50:50  dkrajzew
// warnings removed
//
// Revision 1.4  2006/11/02 12:20:16  dkrajzew
// removed unneeded code
//
// Revision 1.3  2006/09/18 10:14:35  dkrajzew
// changed the way geocoordinates are processed
//
// Revision 1.2  2006/08/02 10:27:21  dkrajzew
// building under Linux patched
//
// Revision 1.1  2006/08/01 07:52:46  dkrajzew
// polyconvert added
//
// Revision 1.2  2006/03/27 07:36:34  dksumo
// some further work...
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
#include "PCElmar.h"
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
PCElmar::PCElmar(PCPolyContainer &toFill, const Boundary & /*netBoundary*/,
                 PCTypeMap &tm)
	: myCont(toFill), myTypeMap(tm)
{
}


PCElmar::~PCElmar()
{
}


void
PCElmar::loadElmar(OptionsCont &oc)
{
    RGBColor c = GfxConvHelper::parseColor(oc.getString("color"));
    std::string file = oc.getString("elmar");
	// load the polygons
	ifstream out(file.c_str());
	if(!out) {
		MsgHandler::getErrorInstance()->inform("Can not open elmar-file '" + file + "'.");
		throw ProcessError();
	}

	// Attributes of the Polygon
	std::string id;
    std::string name;
	std::string type;
	std::string ort;
	Position2DVector vec;

	std::string buff;
	std::string rest; // rest after doing substring
	std::string tab = "\t";
    int l = 0;


	while(out.good()) {
		getline(out,buff);

		if(buff.find("#") != string::npos) { // work on the line without #-symbol
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

		std::string xpos;
		std::string ypos;
		while(rest.find(tab)!=string::npos){ // now collecting the Positions
            xpos = rest.substr(0,rest.find(tab));
			rest = rest.substr(rest.find(tab)+1, rest.length());
            ypos = rest.substr(0,rest.find(tab));

            SUMOReal x = TplConvert<char>::_2SUMOReal(xpos.c_str());
			SUMOReal y = TplConvert<char>::_2SUMOReal(ypos.c_str());

            Position2D pos(x, y);
            GeoConvHelper::remap(pos);
            vec.push_back/*_noDoublePos*/(pos);
            rest = rest.substr(rest.find(tab)+1);
        }
        name = StringUtils::convertUmlaute(name);
		if(name=="noname"||myCont.contains(name)) {
		    name = name + "#" + toString(myCont.getEnumIDFor(name));
        }

        // check the polygon
        if(vec.size()==0) {
            MsgHandler::getWarningInstance()->inform("The polygon '" + id + "' is empty.");
            continue;
        }
        if(id=="") {
            MsgHandler::getErrorInstance()->inform("The name of a polygon is missing.");
            continue;
        }

        // patch the values
        bool fill = vec.getBegin()==vec.getEnd();
        bool discard = false;
        int layer = oc.getInt("layer");
        RGBColor color;
        if(myTypeMap.has(type)) {
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
        if(!discard) {
            Polygon2D *poly = new Polygon2D(name, type, color, vec, fill);
            myCont.insert(name, poly, layer);
        }
    	vec.clear();
	}

    out.close();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



