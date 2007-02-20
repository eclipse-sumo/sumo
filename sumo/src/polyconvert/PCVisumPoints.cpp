/****************************************************************************/
/// @file    PCVisumPoints.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 02.11.2006
/// @version $Id$
///
// A reader of pois stored in visum-format
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

#include <string>
#include <map>
#include <fstream>
#include <utils/common/StringTokenizer.h>
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
#include "PCVisumPoints.h"
#include <utils/gfx/RGBColor.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/geoconv/GeoConvHelper.h>

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
PCVisumPoints::PCVisumPoints(PCPolyContainer &toFill,
                             const Boundary &/*netBoundary*/, PCTypeMap &tm)
        : myCont(toFill), myTypeMap(tm)
{}


PCVisumPoints::~PCVisumPoints()
{}


void
PCVisumPoints::load(OptionsCont &oc)
{
    RGBColor c = GfxConvHelper::parseColor(oc.getString("color"));
    std::string file = oc.getString("visum-points");
    map<string, string> typemap;
    // load the pois

    LineReader lr(file);
    bool parsingCategories = false;
    bool parsingEdgeAssignments = false;
    bool parsingPOIs = false;
    while (lr.hasMore()) {
        string line = lr.readLine();
        // do not parse empty lines
        if (line.length()==0) {
            continue;
        }
        // do not parse comment lines
        if (line[0]=='*') {
            continue;
        }

        if (line[0]=='$') {
            // reset parsing on new entry type
            parsingCategories = false;
            parsingPOIs = false;
        }

        if (parsingCategories) {
            // parse the category
            StringTokenizer st(line, ";");
            string catid = st.next();
            string catname = st.next();
            typemap[catid] = catname;
        }
        if (parsingPOIs) {
            // parse the poi
            // $POI:Nr;CATID;CODE;NAME;Kommentar;XKoord;YKoord;
            StringTokenizer st(line, ";");
            string num = st.next();
            string catid = st.next();
            string code = st.next();
            string name = st.next();
            string xpos = st.next();
            string ypos = st.next();
            // process read values
            SUMOReal x = TplConvert<char>::_2SUMOReal(xpos.c_str());
            SUMOReal y = TplConvert<char>::_2SUMOReal(ypos.c_str());
            Position2D pos(x, y);
            GeoConvHelper::x2cartesian(pos);
            string type = typemap[catid];
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
                color = GfxConvHelper::parseColor(def.color);
                discard = def.discard;
                layer = def.layer;
            } else {
                name = oc.getString("prefix") + name;
                type = oc.getString("type");
                color = c;
            }
            if (!discard) {
                PointOfInterest *poi = new PointOfInterest(name, type, pos, color);
                myCont.insert(name, poi, layer);
            }
        }

        if (line.find("$POIKATEGORIEDEF:")==0) {
            // ok, got categories, begin parsing from next line
            parsingCategories = true;
        }
        if (line.find("$POI:")==0) {
            // ok, got pois, begin parsing from next line
            parsingPOIs = true;
        }
        if (line.find("$POI:")==0) {
            // ok, got pois, begin parsing from next line
            parsingPOIs = true;
        }
    }
}



/****************************************************************************/

