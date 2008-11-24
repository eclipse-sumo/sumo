/****************************************************************************/
/// @file    PCLoaderVisum.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 02.11.2006
/// @version $Id: PCLoaderVisum.cpp 4971 2008-01-25 14:11:05Z dkrajzew $
///
// A reader of pois and polygons stored in VISUM-format
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
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/importio/LineReader.h>
#include <utils/common/StdDefs.h>
#include <polyconvert/PCPolyContainer.h>
#include "PCLoaderVisum.h"
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
void
PCLoaderVisum::loadIfSet(OptionsCont &oc, PCPolyContainer &toFill,
                       PCTypeMap &tm) throw(ProcessError)
{
    if (!oc.isSet("visum-files")) {
        return;
    }
    // parse file(s)
    vector<string> files = oc.getStringVector("visum-files");
    for (vector<string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        // nodes
        if (!FileHelpers::exists(*file)) {
            throw ProcessError("Could not open visum-file '" + *file + "'.");
        }
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing from visum-file '" + *file + "'...");
        load(*file, oc, toFill, tm);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
}



void
PCLoaderVisum::load(const string &file, OptionsCont &oc, PCPolyContainer &toFill,
                       PCTypeMap &tm) throw(ProcessError)
{
    RGBColor c = RGBColor::parseColor(oc.getString("color"));
    map<string, string> typemap;
    // load the pois/polys
    LineReader lr(file);
    bool parsingCategories = false;
    bool parsingPOIs = false;
    Position2DVector vec;
    string polyType, lastID;
    bool first = true;
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
            polyType = "";
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
            string comment = st.next();
            string xpos = st.next();
            string ypos = st.next();
            // process read values
            SUMOReal x = TplConvert<char>::_2SUMOReal(xpos.c_str());
            SUMOReal y = TplConvert<char>::_2SUMOReal(ypos.c_str());
            Position2D pos(x, y);
            GeoConvHelper::x2cartesian(pos);
            string type = typemap[catid];
            // check the poi
            name = num;
            // patch the values
            bool discard = false;
            int layer = oc.getInt("layer");
            RGBColor color;
            if (tm.has(type)) {
                const PCTypeMap::TypeDef &def = tm.get(type);
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
                PointOfInterest *poi = new PointOfInterest(name, type, pos, color);
                toFill.insert(name, poi, layer);
            }
        }
        
        // poly
        if (polyType!="") {
            StringTokenizer st(line, ";");
            string id = st.next();
            string type;
            if (!first&&lastID!=id) {
                // we have parsed a polygon completely
                RGBColor color;
                int layer = oc.getInt("layer");
                bool discard = false;
                if (tm.has(polyType)) {
                    const PCTypeMap::TypeDef &def = tm.get(polyType);
                    id = def.prefix + id;
                    type = def.id;
                    color = RGBColor::parseColor(def.color);
                    discard = def.discard;
                    layer = def.layer;
                } else {
                    id = oc.getString("prefix") + id;
                    type = oc.getString("type");
                    color = c;
                }
                if (!discard) {
                    Polygon2D *poly = new Polygon2D(id, type, color, vec, false);
                    toFill.insert(id, poly, 1);
                }
                vec.clear();
            }
            lastID = id;
            first = false;
            // parse current poly
            string index = st.next();
            string xpos = st.next();
            string ypos = st.next();
            Position2D pos2D((SUMOReal) atof(xpos.c_str()), (SUMOReal) atof(ypos.c_str()));
            GeoConvHelper::x2cartesian(pos2D);
            vec.push_back(pos2D);
        }


        if (line.find("$POIKATEGORIEDEF:")==0) {
            // ok, got categories, begin parsing from next line
            parsingCategories = true;
        }
        if (line.find("$POI:")==0) {
            // ok, got pois, begin parsing from next line
            parsingPOIs = true;
        }


        if (line.find("$BEZIRKPOLY")!=string::npos) {
            polyType = "district";
        }
        if (line.find("$GEBIETPOLY")!=string::npos) {
            polyType = "area";
        }

    }
}



/****************************************************************************/

