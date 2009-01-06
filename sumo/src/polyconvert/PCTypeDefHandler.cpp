/****************************************************************************/
/// @file    PCTypeDefHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A handler for loading polygon type maps
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
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/RGBColor.h>
#include "PCTypeMap.h"
#include "PCTypeDefHandler.h"

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
PCTypeDefHandler::PCTypeDefHandler(OptionsCont &oc, PCTypeMap &con) throw()
        : SUMOSAXHandler("Detector-Defintion"),
        myOptions(oc),  myContainer(con)
{}


PCTypeDefHandler::~PCTypeDefHandler() throw()
{}


void
PCTypeDefHandler::myStartElement(SumoXMLTag element,
                                 const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    if (element==SUMO_TAG_POLYTYPE) {
        // get the id, report an error if not given or empty...
        string id;
        if (!attrs.setIDFromAttributes("polytype", id)) {
            return;
        }
        string type = attrs.getStringSecure(SUMO_ATTR_NAME, myOptions.getString("type"));
        string prefix = attrs.getStringSecure(SUMO_ATTR_PREFIX, myOptions.getString("prefix"));
        int layer = attrs.getIntSecure(SUMO_ATTR_LAYER, myOptions.getInt("layer"));
        string color = attrs.getStringSecure(SUMO_ATTR_COLOR, myOptions.getString("color"));
        bool discard = attrs.getBoolSecure(SUMO_ATTR_DISCARD, false);
        bool allowFill = attrs.getBoolSecure(SUMO_ATTR_FILL, true);
        if (!myContainer.add(id, type, color, prefix, layer, discard, allowFill)) {
            MsgHandler::getErrorInstance()->inform("Could not add polygon type '" + id + "' (probably the id is already used).");
        }
    }
}


/****************************************************************************/

