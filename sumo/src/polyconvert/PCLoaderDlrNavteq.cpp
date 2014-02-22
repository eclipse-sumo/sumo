/****************************************************************************/
/// @file    PCLoaderDlrNavteq.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Michael Behrisch
/// @date    Thu, 02.11.2006
/// @version $Id$
///
// A reader of pois and polygons stored in DLR-Navteq (Elmar)-format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/importio/LineReader.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/StdDefs.h>
#include <polyconvert/PCPolyContainer.h>
#include "PCLoaderDlrNavteq.h"
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
PCLoaderDlrNavteq::loadIfSet(OptionsCont& oc, PCPolyContainer& toFill,
                             PCTypeMap& tm) {
    if (oc.isSet("dlr-navteq-poly-files")) {
        loadPolyFiles(oc, toFill, tm);
    }
    if (oc.isSet("dlr-navteq-poi-files")) {
        loadPOIFiles(oc, toFill, tm);
    }
}


void
PCLoaderDlrNavteq::loadPOIFiles(OptionsCont& oc, PCPolyContainer& toFill,
                                PCTypeMap& tm) {
    std::vector<std::string> files = oc.getStringVector("dlr-navteq-poi-files");
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        if (!FileHelpers::exists(*file)) {
            throw ProcessError("Could not open dlr-navteq-poi-file '" + *file + "'.");
        }
        PROGRESS_BEGIN_MESSAGE("Parsing pois from dlr-navteq-poi-file '" + *file + "'");
        loadPOIFile(*file, oc, toFill, tm);
        PROGRESS_DONE_MESSAGE();
    }
}


void
PCLoaderDlrNavteq::loadPolyFiles(OptionsCont& oc, PCPolyContainer& toFill,
                                 PCTypeMap& tm) {
    std::vector<std::string> files = oc.getStringVector("dlr-navteq-poly-files");
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        if (!FileHelpers::exists(*file)) {
            throw ProcessError("Could not open dlr-navteq-poly-file '" + *file + "'.");
        }
        PROGRESS_BEGIN_MESSAGE("Parsing pois from dlr-navteq-poly-file '" + *file + "'");
        loadPolyFile(*file, oc, toFill, tm);
        PROGRESS_DONE_MESSAGE();
    }
}


void
PCLoaderDlrNavteq::loadPOIFile(const std::string& file,
                               OptionsCont& oc, PCPolyContainer& toFill,
                               PCTypeMap& tm) {
    // get the defaults
    RGBColor c = RGBColor::parseColor(oc.getString("color"));
    // parse
    int l = 0;
    LineReader lr(file);
    while (lr.hasMore()) {
        std::string line = lr.readLine();
        ++l;
        // skip invalid/empty lines
        if (line.length() == 0 || line.find("#") != std::string::npos) {
            continue;
        }
        if (StringUtils::prune(line) == "") {
            continue;
        }
        // parse the poi
        std::istringstream stream(line);
        // attributes of the poi
        std::string name, skip, type, desc;
        std::getline(stream, name, '\t');
        std::getline(stream, skip, '\t');
        std::getline(stream, type, '\t');
        std::getline(stream, desc, '\t');
        if (stream.fail()) {
            throw ProcessError("Invalid dlr-navteq-poi in line " + toString(l) + ":\n" + line);
        }
        double x, y;
        stream >> x;
        if (stream.fail()) {
            throw ProcessError("Invalid x coordinate for POI '" + name + "'.");
        }
        stream >> y;
        if (stream.fail()) {
            throw ProcessError("Invalid y coordinate for POI '" + name + "'.");
        }
        Position pos(x, y);
        // check the poi
        if (name == "") {
            throw ProcessError("The name of a POI is missing.");
        }
        if (!GeoConvHelper::getProcessing().x2cartesian(pos, true)) {
            throw ProcessError("Unable to project coordinates for POI '" + name + "'.");
        }

        // patch the values
        bool discard = oc.getBool("discard");
        int layer = oc.getInt("layer");
        RGBColor color;
        if (tm.has(type)) {
            const PCTypeMap::TypeDef& def = tm.get(type);
            name = def.prefix + name;
            type = def.id;
            color = def.color;
            discard = def.discard;
            layer = def.layer;
        } else {
            name = oc.getString("prefix") + name;
            type = oc.getString("type");
            color = c;
        }
        if (!discard) {
            bool ignorePrunning = false;
            if (OptionsCont::getOptions().isInStringVector("prune.keep-list", name)) {
                ignorePrunning = true;
            }
            PointOfInterest* poi = new PointOfInterest(name, type, color, pos, (SUMOReal)layer);
            toFill.insert(name, poi, layer, ignorePrunning);
        }
    }
}


void
PCLoaderDlrNavteq::loadPolyFile(const std::string& file,
                                OptionsCont& oc, PCPolyContainer& toFill,
                                PCTypeMap& tm) {
    // get the defaults
    RGBColor c = RGBColor::parseColor(oc.getString("color"));
    // attributes of the poly
    // parse
    int l = 0;
    LineReader lr(file);
    while (lr.hasMore()) {
        std::string line = lr.readLine();
        ++l;
        // skip invalid/empty lines
        if (line.length() == 0 || line.find("#") != std::string::npos) {
            continue;
        }
        if (StringUtils::prune(line) == "") {
            continue;
        }
        // parse the poi
        StringTokenizer st(line, "\t");
        std::vector<std::string> values = st.getVector();
        if (values.size() < 6 || values.size() % 2 != 0) {
            throw ProcessError("Invalid dlr-navteq-polygon - line: '" + line + "'.");
        }
        std::string id = values[0];
        std::string ort = values[1];
        std::string type = values[2];
        std::string name = values[3];
        PositionVector vec;
        size_t index = 4;
        // now collect the positions
        while (values.size() > index) {
            std::string xpos = values[index];
            std::string ypos = values[index + 1];
            index += 2;
            SUMOReal x = TplConvert::_2SUMOReal(xpos.c_str());
            SUMOReal y = TplConvert::_2SUMOReal(ypos.c_str());
            Position pos(x, y);
            if (!GeoConvHelper::getProcessing().x2cartesian(pos)) {
                WRITE_WARNING("Unable to project coordinates for polygon '" + id + "'.");
            }
            vec.push_back(pos);
        }

        name = StringUtils::convertUmlaute(name);
        if (name == "noname" || toFill.containsPolygon(name)) {
            name = name + "#" + toString(toFill.getEnumIDFor(name));
        }

        // check the polygon
        if (vec.size() == 0) {
            WRITE_WARNING("The polygon '" + id + "' is empty.");
            continue;
        }
        if (id == "") {
            WRITE_WARNING("The name of a polygon is missing; it will be discarded.");
            continue;
        }

        // patch the values
        bool fill = vec.front() == vec.back();
        bool discard = oc.getBool("discard");
        int layer = oc.getInt("layer");
        RGBColor color;
        if (tm.has(type)) {
            const PCTypeMap::TypeDef& def = tm.get(type);
            name = def.prefix + name;
            type = def.id;
            color = def.color;
            fill = fill && def.allowFill;
            discard = def.discard;
            layer = def.layer;
        } else {
            name = oc.getString("prefix") + name;
            type = oc.getString("type");
            color = c;
        }
        if (!discard) {
            Polygon* poly = new Polygon(name, type, color, vec, fill, (SUMOReal)layer);
            toFill.insert(name, poly, layer);
        }
        vec.clear();
    }
}





/****************************************************************************/

