/***************************************************************************
                          PCTypeDefHandler.cpp
    A handler for loading polygon type maps
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2006/11/14 13:04:17  dkrajzew
// warnings removed
//
// Revision 1.1  2006/08/01 07:52:46  dkrajzew
// polyconvert added
//
// Revision 1.5  2006/03/17 09:04:18  dksumo
// class-documentation added/patched
//
// Revision 1.4  2006/02/02 12:16:13  dksumo
// saved to recheck methods
//
// Revision 1.3  2006/01/26 11:24:17  dksumo
// debugging building under linux
//
// Revision 1.2  2006/01/17 14:09:11  dksumo
// routes output added; debugging
//
// Revision 1.1  2006/01/16 13:45:21  dksumo
// initial work on the dfrouter
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
PCTypeDefHandler::PCTypeDefHandler(OptionsCont &oc, PCTypeMap &con)
    : SUMOSAXHandler("Detector-Defintion"),
    myOptions(oc),  myContainer(con)
{
}


PCTypeDefHandler::~PCTypeDefHandler()
{
}


void
PCTypeDefHandler::myStartElement(int /*element*/, const std::string &name,
                                 const Attributes &attrs)
{
    if(name=="polytype") {
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
        if(!myContainer.add(id, type, color, prefix, layer, discard, allowFill)) {
            MsgHandler::getErrorInstance()->inform("Could not add polygon type '" + id + "' (probably the id is already used).");
        }
    }
}


void
PCTypeDefHandler::myCharacters(int /*element*/, const std::string&,
                               const std::string &)
{
}


void
PCTypeDefHandler::myEndElement(int, const std::string&)
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


