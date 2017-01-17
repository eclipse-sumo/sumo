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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
    : ShapeHandler("xml-poi-definition", toFill),
      myTypeMap(tm), myOptions(oc) {}


PCLoaderXML::~PCLoaderXML() {}


void
PCLoaderXML::myStartElement(int element,
                            const SUMOSAXAttributes& attrs) {
    if (element != SUMO_TAG_POI && element != SUMO_TAG_POLY) {
        return;
    }
    bool ok = true;
    // get the id, report an error if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    std::string type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, myOptions.getString("type"));
    if (!ok) {
        return;
    }
    // patch the values
    bool discard = myOptions.getBool("discard");
    if (myTypeMap.has(type)) {
        const PCTypeMap::TypeDef& def = myTypeMap.get(type);
        discard = def.discard;
        setDefaults(def.prefix, def.color, def.layer, def.allowFill);
    } else {
        setDefaults(myOptions.getString("prefix"), RGBColor::parseColor(myOptions.getString("color")),
                    myOptions.getFloat("layer"), myOptions.getBool("fill"));
    }
    if (!discard) {
        if (element == SUMO_TAG_POI) {
            addPOI(attrs, myOptions.isInStringVector("prune.keep-list", id), true);
        }
        if (element == SUMO_TAG_POLY) {
            addPoly(attrs, myOptions.isInStringVector("prune.keep-list", id), true);
        }
    }
}


Position
PCLoaderXML::getLanePos(const std::string& poiID, const std::string& laneID, SUMOReal lanePos) {
    static_cast<PCPolyContainer&>(myShapeContainer).addLanePos(poiID, laneID, lanePos);
    return Position::INVALID;
}


/****************************************************************************/

