/****************************************************************************/
/// @file    PCNetProjectionLoader.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 02.11.2006
/// @version $Id$
///
// A reader for a SUMO network's projection description
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include <utils/xml/SUMOSAXReader.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "PCNetProjectionLoader.h"

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
PCNetProjectionLoader::loadIfSet(OptionsCont& oc,
                                 Position& netOffset, Boundary& origNetBoundary,
                                 Boundary& convNetBoundary,
                                 std::string& projParameter) {
    if (!oc.isSet("net")) {
        return;
    }
    // check file
    std::string file = oc.getString("net");
    if (!FileHelpers::exists(file)) {
        throw ProcessError("Could not open net-file '" + file + "'.");
    }
    // build handler and parser
    PCNetProjectionLoader handler(netOffset, origNetBoundary, convNetBoundary, projParameter);
    handler.setFileName(file);
    SUMOSAXReader* parser = XMLSubSys::getSAXReader(handler);
    PROGRESS_BEGIN_MESSAGE("Parsing network projection from '" + file + "'");
    if (!parser->parseFirst(file)) {
        delete parser;
        throw ProcessError("Can not read XML-file '" + handler.getFileName() + "'.");
    }
    // parse
    while (parser->parseNext() && !handler.hasReadAll());
    // clean up
    PROGRESS_DONE_MESSAGE();
    if (!handler.hasReadAll()) {
        throw ProcessError("Could not find projection parameter in net.");
    }
    delete parser;
}



// ---------------------------------------------------------------------------
// handler methods
// ---------------------------------------------------------------------------
PCNetProjectionLoader::PCNetProjectionLoader(Position& netOffset,
        Boundary& origNetBoundary, Boundary& convNetBoundary,
        std::string& projParameter)
    : SUMOSAXHandler("sumo-network"), myNetOffset(netOffset),
      myOrigNetBoundary(origNetBoundary), myConvNetBoundary(convNetBoundary),
      myProjParameter(projParameter),
      myFoundOffset(false), myFoundOrigNetBoundary(false),
      myFoundConvNetBoundary(false), myFoundProj(false) {}


PCNetProjectionLoader::~PCNetProjectionLoader() {}


void
PCNetProjectionLoader::myStartElement(int element,
                                      const SUMOSAXAttributes& attrs) {
    if (element != SUMO_TAG_LOCATION) {
        return;
    }
    bool ok = true;
    PositionVector tmp = attrs.getShapeReporting(SUMO_ATTR_NET_OFFSET, 0, ok, false);
    if (ok) {
        myNetOffset = tmp[0];
    }
    myOrigNetBoundary = attrs.getBoundaryReporting(SUMO_ATTR_ORIG_BOUNDARY, 0, ok);
    myConvNetBoundary = attrs.getBoundaryReporting(SUMO_ATTR_CONV_BOUNDARY, 0, ok);
    myProjParameter = attrs.getOptStringReporting(SUMO_ATTR_ORIG_PROJ, 0, ok, "");
    myFoundOffset = myFoundOrigNetBoundary = myFoundConvNetBoundary = myFoundProj = ok;
}


bool
PCNetProjectionLoader::hasReadAll() const {
    return myFoundOffset && myFoundOrigNetBoundary && myFoundConvNetBoundary && myFoundProj;
}


/****************************************************************************/

