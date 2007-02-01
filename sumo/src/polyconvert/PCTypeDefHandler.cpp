/****************************************************************************/
/// @file    PCTypeDefHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id: $
///
// A handler for loading polygon type maps
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
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/gfx/RGBColor.h>
#include <utils/gfx/GfxConvHelper.h>
#include "PCTypeMap.h"
#include "PCTypeDefHandler.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
PCTypeDefHandler::PCTypeDefHandler(OptionsCont &oc, PCTypeMap &con)
        : SUMOSAXHandler("Detector-Defintion"),
        myOptions(oc),  myContainer(con)
{}


PCTypeDefHandler::~PCTypeDefHandler()
{}


void
PCTypeDefHandler::myStartElement(int /*element*/, const std::string &name,
                                 const Attributes &attrs)
{
    if (name=="polytype") {
        string id;
        try {
            id = getString(attrs, SUMO_ATTR_ID);
        } catch (EmptyData&) {
            MsgHandler::getErrorInstance()->inform("A polygon type without an id occured within '" + _file + ".");
            return;
        }
        string type = getStringSecure(attrs, "name", myOptions.getString("type"));
        string prefix = getStringSecure(attrs, "prefix", myOptions.getString("prefix"));
        int layer = getIntSecure(attrs, "layer", myOptions.getInt("layer"));
        string color = getStringSecure(attrs, SUMO_ATTR_COLOR, myOptions.getString("color"));
        bool discard = getBoolSecure(attrs, "discard", false);
        bool allowFill = getBoolSecure(attrs, "fill", true);
        if (!myContainer.add(id, type, color, prefix, layer, discard, allowFill)) {
            MsgHandler::getErrorInstance()->inform("Could not add polygon type '" + id + "' (probably the id is already used).");
        }
    }
}


void
PCTypeDefHandler::myCharacters(int /*element*/, const std::string&,
                               const std::string &)
{}


void
PCTypeDefHandler::myEndElement(int, const std::string&)
{}



/****************************************************************************/

