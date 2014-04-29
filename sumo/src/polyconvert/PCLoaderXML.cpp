/****************************************************************************/
/// @file    PCLoaderXML.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Christoph Sommer
/// @author  Michael Behrisch
/// @date    Thu, 02.11.2006
/// @version $Id$
///
// A reader for polygons and pois stored in XML-format
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
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/StdDefs.h>
#include <polyconvert/PCPolyContainer.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "PCLoaderXML.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static interface
// ---------------------------------------------------------------------------
void
PCLoaderXML::loadIfSet(OptionsCont& oc, PCPolyContainer& toFill,
                       PCTypeMap& tm) {
    if (!oc.isSet("xml-files")) {
        return;
    }
    PCLoaderXML handler(toFill, tm, oc);
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("xml");
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        if (!FileHelpers::isReadable(*file)) {
            throw ProcessError("Could not open xml-file '" + *file + "'.");
        }
        PROGRESS_BEGIN_MESSAGE("Parsing XML from '" + *file + "'");
        if (!XMLSubSys::runParser(handler, *file)) {
            throw ProcessError();
        }
        PROGRESS_DONE_MESSAGE();
    }
}



// ---------------------------------------------------------------------------
// handler methods
// ---------------------------------------------------------------------------
PCLoaderXML::PCLoaderXML(PCPolyContainer& toFill,
                         PCTypeMap& tm, OptionsCont& oc)
    : SUMOSAXHandler("xml-poi-definition"),
      myCont(toFill), myTypeMap(tm), myOptions(oc) {}


PCLoaderXML::~PCLoaderXML() {}


void
PCLoaderXML::myStartElement(int element,
                            const SUMOSAXAttributes& attrs) {
    if (element != SUMO_TAG_POI && element != SUMO_TAG_POLY) {
        return;
    }
    if (element == SUMO_TAG_POI) {
        bool ok = true;
        // get the id, report an error if not given or empty...
        std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
        SUMOReal x = attrs.get<SUMOReal>(SUMO_ATTR_X, id.c_str(), ok);
        SUMOReal y = attrs.get<SUMOReal>(SUMO_ATTR_Y, id.c_str(), ok);
        std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, myOptions.getString("type"));
        if (!ok) {
            return;
        }
        Position pos(x, y);
        if (!GeoConvHelper::getProcessing().x2cartesian(pos)) {
            WRITE_WARNING("Unable to project coordinates for POI '" + id + "'.");
        }
        // patch the values
        bool discard = myOptions.getBool("discard");
        SUMOReal layer = (SUMOReal)myOptions.getInt("layer");
        RGBColor color;
        if (myTypeMap.has(type)) {
            const PCTypeMap::TypeDef& def = myTypeMap.get(type);
            id = def.prefix + id;
            type = def.id;
            color = def.color;
            discard = def.discard;
            layer = (SUMOReal)def.layer;
        } else {
            id = myOptions.getString("prefix") + id;
            color = RGBColor::parseColor(myOptions.getString("color"));
        }
        layer = attrs.getOpt<SUMOReal>(SUMO_ATTR_LAYER, id.c_str(), ok, layer);
        if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
            color = attrs.get<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok);
        }
        SUMOReal angle = attrs.getOpt<SUMOReal>(SUMO_ATTR_ANGLE, id.c_str(), ok, Shape::DEFAULT_ANGLE);
        std::string imgFile = attrs.getOpt<std::string>(SUMO_ATTR_IMGFILE, id.c_str(), ok, Shape::DEFAULT_IMG_FILE);
        if (imgFile != "" && !FileHelpers::isAbsolute(imgFile)) {
            imgFile = FileHelpers::getConfigurationRelative(getFileName(), imgFile);
        }
        SUMOReal imgWidth = attrs.getOpt<SUMOReal>(SUMO_ATTR_WIDTH, id.c_str(), ok, Shape::DEFAULT_IMG_WIDTH);
        SUMOReal imgHeight = attrs.getOpt<SUMOReal>(SUMO_ATTR_HEIGHT, id.c_str(), ok, Shape::DEFAULT_IMG_HEIGHT);
        if (!ok) {
            return;
        }
        if (!discard) {
            bool ignorePrunning = false;
            if (OptionsCont::getOptions().isInStringVector("prune.keep-list", id)) {
                ignorePrunning = true;
            }
            PointOfInterest* poi = new PointOfInterest(id, type, color, pos, layer, angle, imgFile, imgWidth, imgHeight);
            myCont.insert(id, poi, (int)layer, ignorePrunning);
        }
    }
    if (element == SUMO_TAG_POLY) {
        bool discard = myOptions.getBool("discard");
        SUMOReal layer = (SUMOReal)myOptions.getInt("layer");
        bool ok = true;
        std::string id = attrs.getOpt<std::string>(SUMO_ATTR_ID, myCurrentID.c_str(), ok, "");
        std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, myCurrentID.c_str(), ok, myOptions.getString("type"));
        if (!ok) {
            return;
        }
        RGBColor color;
        if (myTypeMap.has(type)) {
            const PCTypeMap::TypeDef& def = myTypeMap.get(type);
            id = def.prefix + id;
            type = def.id;
            color = def.color;
            discard = def.discard;
            layer = (SUMOReal)def.layer;
        } else {
            id = myOptions.getString("prefix") + id;
            color = RGBColor::parseColor(myOptions.getString("color"));
        }
        layer = attrs.getOpt<SUMOReal>(SUMO_ATTR_LAYER, id.c_str(), ok, layer);
        if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
            color = attrs.get<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok);
        }
        SUMOReal angle = attrs.getOpt<SUMOReal>(SUMO_ATTR_ANGLE, id.c_str(), ok, Shape::DEFAULT_ANGLE);
        std::string imgFile = attrs.getOpt<std::string>(SUMO_ATTR_IMGFILE, id.c_str(), ok, Shape::DEFAULT_IMG_FILE);
        if (imgFile != "" && !FileHelpers::isAbsolute(imgFile)) {
            imgFile = FileHelpers::getConfigurationRelative(getFileName(), imgFile);
        }
        bool fill = attrs.getOpt<bool>(SUMO_ATTR_FILL, id.c_str(), ok, false);
        if (!ok) {
            return;
        }
        if (!discard) {
            bool ignorePrunning = false;
            if (OptionsCont::getOptions().isInStringVector("prune.keep-list", id)) {
                ignorePrunning = true;
            }
            myCurrentID = id;
            myCurrentType = type;
            myCurrentColor = color;
            myCurrentIgnorePrunning = ignorePrunning;
            myCurrentLayer = layer;
            PositionVector pshape = attrs.get<PositionVector>(SUMO_ATTR_SHAPE, myCurrentID.c_str(), ok);
            if (!ok) {
                return;
            }
            PositionVector shape;
            for (PositionVector::const_iterator i = pshape.begin(); i != pshape.end(); ++i) {
                Position pos((*i));
                if (!GeoConvHelper::getProcessing().x2cartesian(pos)) {
                    WRITE_WARNING("Unable to project coordinates for polygon '" + myCurrentID + "'.");
                }
                shape.push_back(pos);
            }
            Polygon* poly = new Polygon(myCurrentID, myCurrentType, myCurrentColor, shape, fill, layer, angle, imgFile);
            myCont.insert(myCurrentID, poly, (int)myCurrentLayer, myCurrentIgnorePrunning);
        }
    }
}


/****************************************************************************/

