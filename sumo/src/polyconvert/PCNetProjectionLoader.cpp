/****************************************************************************/
/// @file    PCNetProjectionLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 02.11.2006
/// @version $Id: PCNetProjectionLoader.cpp 7490 2009-07-23 14:24:32Z dkrajzew $
///
// A reader for a SUMO network's projection description
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
#include "PCNetProjectionLoader.h"
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
PCNetProjectionLoader::loadIfSet(OptionsCont &oc, 
                                 Position2D &netOffset, Boundary &origNetBoundary, 
                                 Boundary &convNetBoundary, 
                                 std::string &projParameter) throw(ProcessError) {
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
    XMLPScanToken token;
    XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader *parser = XMLSubSys::getSAXReader(handler);
    MsgHandler::getMessageInstance()->beginProcessMsg("Parsing network projection from '" + file + "'...");
    if (!parser->parseFirst(file.c_str(), token)) {
        delete parser;
        throw ProcessError("Can not read XML-file '" + handler.getFileName() + "'.");
    }
    // parse
    while (parser->parseNext(token) && !handler.hasReadAll());
    // clean up
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
    if(!handler.hasReadAll()) {
        throw ProcessError("Could not find projection parameter in net.");
    }
    delete parser;
}



// ---------------------------------------------------------------------------
// handler methods
// ---------------------------------------------------------------------------
PCNetProjectionLoader::PCNetProjectionLoader(Position2D &netOffset, 
        Boundary &origNetBoundary, Boundary &convNetBoundary, 
        std::string &projParameter) throw()
    : SUMOSAXHandler("sumo-network"), myNetOffset(netOffset), 
    myOrigNetBoundary(origNetBoundary), myConvNetBoundary(convNetBoundary), 
    myProjParameter(projParameter),
    myFoundOffset(false), myFoundOrigNetBoundary(false), 
    myFoundConvNetBoundary(false), myFoundProj(false)
{}


PCNetProjectionLoader::~PCNetProjectionLoader() throw() {}


void
PCNetProjectionLoader::myStartElement(SumoXMLTag element,
                            const SUMOSAXAttributes &attrs) throw(ProcessError) {
    if (element!=SUMO_TAG_LOCATION) {
        return;
    }
    myOrigNetBoundary = GeomConvHelper::parseBoundary(attrs.getStringSecure(SUMO_ATTR_ORIG_BOUNDARY, ""));
    myConvNetBoundary = GeomConvHelper::parseBoundary(attrs.getStringSecure(SUMO_ATTR_CONV_BOUNDARY, ""));
    myNetOffset = GeomConvHelper::parseShape(attrs.getStringSecure(SUMO_ATTR_NET_OFFSET, ""))[0];
    myProjParameter = attrs.getStringSecure(SUMO_ATTR_ORIG_PROJ, "");
    myFoundOffset = myFoundOrigNetBoundary = myFoundConvNetBoundary = myFoundProj = true;
}


void
PCNetProjectionLoader::myCharacters(SumoXMLTag element,
                          const std::string &chars) throw(ProcessError) {
    switch(element) {
    case SUMO_TAG_ORIG_BOUNDARY:
        myOrigNetBoundary = GeomConvHelper::parseBoundary(chars);
        myFoundOrigNetBoundary = true;
        break;
    case SUMO_TAG_CONV_BOUNDARY:
        myConvNetBoundary = GeomConvHelper::parseBoundary(chars);
        myFoundConvNetBoundary = true;
        break;
    case SUMO_TAG_NET_OFFSET:
        myNetOffset = GeomConvHelper::parseShape(chars)[0];
        myFoundOffset = true;
        break;
    case SUMO_TAG_ORIG_PROJ:
        myProjParameter = chars;
        myFoundProj = true;
        break;
    default:
        break;
    }
}


bool 
PCNetProjectionLoader::hasReadAll() const throw()
{
    return myFoundOffset&&myFoundOrigNetBoundary&&myFoundConvNetBoundary&&myFoundProj;
}


/****************************************************************************/

