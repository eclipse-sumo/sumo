/****************************************************************************/
/// @file    PCLoaderXML.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 02.11.2006
/// @version $Id$
///
// A reader for polygons and pois stored in XML-format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/StdDefs.h>
#include <polyconvert/PCPolyContainer.h>
#include "PCLoaderXML.h"
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>

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
// ---------------------------------------------------------------------------
// static interface
// ---------------------------------------------------------------------------
void
PCLoaderXML::loadIfSet(OptionsCont &oc, PCPolyContainer &toFill,
                       PCTypeMap &tm) throw(ProcessError) {
    if (!oc.isSet("xml")) {
        return;
    }
    PCLoaderXML handler(toFill, tm, oc);
    // parse file(s)
    vector<string> files = oc.getStringVector("xml");
    for (vector<string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        if (!FileHelpers::exists(*file)) {
            throw ProcessError("Could not open osm-file '" + *file + "'.");
        }
        handler.setFileName(*file);
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing XML from '" + *file + "'...");
        if (!XMLSubSys::runParser(handler, *file)) {
            throw ProcessError();
        }
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
}



// ---------------------------------------------------------------------------
// handler methods
// ---------------------------------------------------------------------------
PCLoaderXML::PCLoaderXML(PCPolyContainer &toFill,
                         PCTypeMap &tm, OptionsCont &oc) throw()
        : SUMOSAXHandler("xml-poi-definition"),
        myCont(toFill), myTypeMap(tm), myOptions(oc) {}


PCLoaderXML::~PCLoaderXML() throw() {}


void
PCLoaderXML::myStartElement(SumoXMLTag element,
                            const SUMOSAXAttributes &attrs) throw(ProcessError) {
    if (element!=SUMO_TAG_POI && element!=SUMO_TAG_POLY) {
        return;
    }
    if (element==SUMO_TAG_POI) {
        // get the id, report an error if not given or empty...
        string id;
        if (!attrs.setIDFromAttributes("poi", id)) {
            return;
        }
        string type = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
        SUMOReal x = attrs.getFloatSecure(SUMO_ATTR_X, -1);
        SUMOReal y = attrs.getFloatSecure(SUMO_ATTR_Y, -1);
        Position2D pos(y, x); // !!! reverse!
        GeoConvHelper::x2cartesian(pos);
        // patch the values
        bool discard = false;
        int layer = myOptions.getInt("layer");
        RGBColor color;
        if (myTypeMap.has(type)) {
            const PCTypeMap::TypeDef &def = myTypeMap.get(type);
            id = def.prefix + id;
            type = def.id;
            color = RGBColor::parseColor(def.color);
            discard = def.discard;
            layer = def.layer;
        } else {
            id = myOptions.getString("prefix") + id;
            type = myOptions.getString("type");
            color = RGBColor::parseColor(myOptions.getString("color"));
        }
        if (!discard) {
            bool ignorePrunning = false;
            if (OptionsCont::getOptions().isInStringVector("prune.ignore", id)) {
                ignorePrunning = true;
            }
            PointOfInterest *poi = new PointOfInterest(id, type, pos, color);
            if (!myCont.insert(id, poi, layer, ignorePrunning)) {
                MsgHandler::getErrorInstance()->inform("POI '" + id + "' could not been added.");
                delete poi;
            }
        }
    }
    if (element==SUMO_TAG_POLY) {
        bool discard = false;
        int layer = myOptions.getInt("layer");
        string id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        string type = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
        RGBColor color;
        if (myTypeMap.has(type)) {
            const PCTypeMap::TypeDef &def = myTypeMap.get(type);
            id = def.prefix + id;
            type = def.id;
            color = RGBColor::parseColor(def.color);
            discard = def.discard;
            layer = def.layer;
        } else {
            id = myOptions.getString("prefix") + id;
            type = myOptions.getString("type");
            color = RGBColor::parseColor(myOptions.getString("color"));
        }
        if (!discard) {
            bool ignorePrunning = false;
            if (OptionsCont::getOptions().isInStringVector("prune.ignore", id)) {
                ignorePrunning = true;
            }
            myCurrentID = id;
            myCurrentType = type;
            myCurrentColor = color;
            myCurrentIgnorePrunning = ignorePrunning;
            myCurrentLayer = layer;
        }
    }
}


void
PCLoaderXML::myCharacters(SumoXMLTag element,
                          const std::string &chars) throw(ProcessError) {
    if (element==SUMO_TAG_POLY) {
        Position2DVector pshape = GeomConvHelper::parseShape(chars);
        const Position2DVector::ContType &cont = pshape.getCont();
        Position2DVector shape;
        for (Position2DVector::ContType::const_iterator i=cont.begin(); i!=cont.end(); ++i) {
            Position2D pos((*i));
            GeoConvHelper::x2cartesian(pos);
            shape.push_back(pos);
        }
        Polygon2D *poly = new Polygon2D(myCurrentID, myCurrentType, myCurrentColor, shape, false);
        if (!myCont.insert(myCurrentID, poly, myCurrentLayer, myCurrentIgnorePrunning)) {
            MsgHandler::getErrorInstance()->inform("Polygon '" + myCurrentID + "' could not been added.");
            delete poly;
        }
    }
}



/****************************************************************************/

