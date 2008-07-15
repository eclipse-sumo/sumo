/****************************************************************************/
/// @file    SUMOSAXAttributes.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 30 Mar 2007
/// @version $Id$
///
// Encapsulated SAX-Attributes
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
#include "SUMOSAXAttributes.h"
#include <utils/common/MsgHandler.h>
#include <iostream>

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
bool 
SUMOSAXAttributes::setIDFromAttributes(const char * objecttype, 
                                      std::string &id, bool report) const throw()
{
    id = "";
    if(hasAttribute(SUMO_ATTR_ID)) {
        id = getString(SUMO_ATTR_ID);
    }
    if(id=="") {
        if(report) {
            MsgHandler::getErrorInstance()->inform("Missing id of a '" + string(objecttype) + "'-object.");
        }
        return false;
    }
    return true;
}


int 
SUMOSAXAttributes::getIntReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid, 
                          bool &ok, bool report) const throw()
{
    if(!hasAttribute(attr)) {
        if(report) {
            if(objectid!=0) {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' is missing in definition of " + string(objecttype) + " '" + string(objectid) + "'.");
            } else {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' is missing in definition of a(n) " + string(objecttype) + ".");
            }
        }
        ok = false;
        return -1;
    }
    try {
        return getInt(attr);
    } catch(NumberFormatException &) {
        if(report) {
            if(objectid!=0) {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' in definition of " + string(objecttype) + " '" + string(objectid) + "' is not an int.");
            } else {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' in definition of a(n) " + string(objecttype) + " is not an int.");
            }
        }
    } catch(EmptyData &) {
        if(report) {
            if(objectid!=0) {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' in definition of " + string(objecttype) + " '" + string(objectid) + "' is empty.");
            } else {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' in definition of a(n) " + string(objecttype) + " is empty.");
            }
        }
    }
    ok = false;
    return -1;
}


SUMOReal 
SUMOSAXAttributes::getSUMORealReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid, 
                          bool &ok, bool report) const throw()
{
    if(!hasAttribute(attr)) {
        if(report) {
            if(objectid!=0) {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' is missing in definition of " + string(objecttype) + " '" + string(objectid) + "'.");
            } else {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' is missing in definition of a " + string(objecttype) + ".");
            }
        }
        ok = false;
        return -1;
    }
    try {
        return getFloat(attr);
    } catch(NumberFormatException &) {
        if(report) {
            if(objectid!=0) {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' in definition of " + string(objecttype) + " '" + string(objectid) + "' is not a real number.");
            } else {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' in definition of " + string(objecttype) + " is not a real number.");
            }
        }
    } catch(EmptyData &) {
        if(report) {
            if(objectid!=0) {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' in definition of " + string(objecttype) + " '" + string(objectid) + "' is empty.");
            } else {
                MsgHandler::getErrorInstance()->inform("Attribute '" + getName(attr) + "' in definition of " + string(objecttype) + " is empty.");
            }
        }
    }
    ok = false;
    return (SUMOReal) -1;
}




/****************************************************************************/

