/****************************************************************************/
/// @file    SUMOSAXAttributes.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 30 Mar 2007
/// @version $Id$
///
// Encapsulated SAX-Attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/StringTokenizer.h>
#include <iostream>
#include <sstream>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
bool SUMOSAXAttributes::myHaveInformedAboutDeprecatedDivider = false;


// ===========================================================================
// method definitions
// ===========================================================================
bool
SUMOSAXAttributes::setIDFromAttributes(const char * objecttype,
                                       std::string &id, bool report) const throw() {
    id = "";
    if (hasAttribute(SUMO_ATTR_ID)) {
        id = getString(SUMO_ATTR_ID);
    }
    if (id=="") {
        if (report) {
            MsgHandler::getErrorInstance()->inform("Missing id of a '" + std::string(objecttype) + "'-object.");
        }
        return false;
    }
    return true;
}


int
SUMOSAXAttributes::getIntReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid,
                                   bool &ok, bool report) const throw() {
    if (!hasAttribute(attr)) {
        if (report) {
            emitUngivenError(getName(attr), objecttype, objectid);
        }
        ok = false;
        return -1;
    }
    try {
        return getInt(attr);
    } catch (NumberFormatException &) {
        if (report) {
            emitFormatError(getName(attr), "an int", objecttype, objectid);
        }
    } catch (EmptyData &) {
        if (report) {
            emitEmptyError(getName(attr), objecttype, objectid);
        }
    }
    ok = false;
    return -1;
}


int
SUMOSAXAttributes::getOptIntReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid,
                                      bool &ok, int defaultValue, bool report) const throw() {
    if (!hasAttribute(attr)) {
        return defaultValue;
    }
    try {
        return getInt(attr);
    } catch (NumberFormatException &) {
        if (report) {
            emitFormatError(getName(attr), "an int", objecttype, objectid);
        }
    } catch (EmptyData &) {
        if (report) {
            emitEmptyError(getName(attr), objecttype, objectid);
        }
    }
    ok = false;
    return -1;
}


SUMOReal
SUMOSAXAttributes::getSUMORealReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid,
                                        bool &ok, bool report) const throw() {
    if (!hasAttribute(attr)) {
        if (report) {
            emitUngivenError(getName(attr), objecttype, objectid);
        }
        ok = false;
        return -1;
    }
    try {
        return getFloat(attr);
    } catch (NumberFormatException &) {
        if (report) {
            emitFormatError(getName(attr), "a real number", objecttype, objectid);
        }
    } catch (EmptyData &) {
        if (report) {
            emitEmptyError(getName(attr), objecttype, objectid);
        }
    }
    ok = false;
    return (SUMOReal) -1;
}


SUMOReal
SUMOSAXAttributes::getOptSUMORealReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid,
        bool &ok, SUMOReal defaultValue, bool report) const throw() {
    if (!hasAttribute(attr)) {
        return defaultValue;
    }
    try {
        return getFloat(attr);
    } catch (NumberFormatException &) {
        if (report) {
            emitFormatError(getName(attr), "a real number", objecttype, objectid);
        }
    } catch (EmptyData &) {
        if (report) {
            emitEmptyError(getName(attr), objecttype, objectid);
        }
    }
    ok = false;
    return (SUMOReal) -1;
}


bool
SUMOSAXAttributes::getBoolReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid,
                                    bool &ok, bool report) const throw() {
    if (!hasAttribute(attr)) {
        if (report) {
            emitUngivenError(getName(attr), objecttype, objectid);
        }
        ok = false;
        return false;
    }
    try {
        return getBool(attr);
    } catch (BoolFormatException &) {
        if (report) {
            emitFormatError(getName(attr), "a boolean", objecttype, objectid);
        }
    } catch (EmptyData &) {
        if (report) {
            emitEmptyError(getName(attr), objecttype, objectid);
        }
    }
    ok = false;
    return false;
}


bool
SUMOSAXAttributes::getOptBoolReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid,
                                       bool &ok, bool defaultValue, bool report) const throw() {
    if (!hasAttribute(attr)) {
        return defaultValue;
    }
    try {
        return getBool(attr);
    } catch (BoolFormatException &) {
        if (report) {
            emitFormatError(getName(attr), "a boolean", objecttype, objectid);
        }
    } catch (EmptyData &) {
        if (report) {
            emitEmptyError(getName(attr), objecttype, objectid);
        }
    }
    ok = false;
    return false;
}


std::string
SUMOSAXAttributes::getStringReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid,
                                      bool &ok, bool report) const throw() {
    if (!hasAttribute(attr)) {
        if (report) {
            emitUngivenError(getName(attr), objecttype, objectid);
        }
        ok = false;
        return "";
    }
    try {
        std::string ret = getString(attr);
        if (ret=="") {
            throw EmptyData();
        }
        return ret;
    } catch (EmptyData &) {
        if (report) {
            emitEmptyError(getName(attr), objecttype, objectid);
        }
    }
    ok = false;
    return "";
}


std::string
SUMOSAXAttributes::getOptStringReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid,
        bool &ok, const std::string&defaultValue, bool report) const throw() {
    if (!hasAttribute(attr)) {
        return defaultValue;
    }
    try {
        return getString(attr);
    } catch (EmptyData &) {
        if (report) {
            emitEmptyError(getName(attr), objecttype, objectid);
        }
    }
    ok = false;
    return "";
}


SUMOTime
SUMOSAXAttributes::getSUMOTimeReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid,
                                        bool &ok, bool report) const throw() {
#ifdef HAVE_SUBSECOND_TIMESTEPS
    return getSUMORealReporting(attr, objecttype, objectid, ok, report);
#else
    return getIntReporting(attr, objecttype, objectid, ok, report);
#endif
}


SUMOTime
SUMOSAXAttributes::getOptSUMOTimeReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid,
        bool &ok, SUMOTime defaultValue, bool report) const throw() {
#ifdef HAVE_SUBSECOND_TIMESTEPS
    return getOptSUMORealReporting(attr, objecttype, objectid, ok, defaultValue, report);
#else
    return getOptIntReporting(attr, objecttype, objectid, ok, defaultValue, report);
#endif
}





void
SUMOSAXAttributes::emitUngivenError(const std::string &attrname, const char *objecttype, const char *objectid) const throw() {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' is missing in definition of ";
    if (objectid==0) {
        oss << "a ";
    }
    if (objecttype!=0) {
        oss << objecttype;
    } else {
        oss << "<unknown type>";
    }
    if (objectid!=0) {
        oss << " '" << objectid << "'";
    }
    oss << ".";
    MsgHandler::getErrorInstance()->inform(oss.str());
}


void
SUMOSAXAttributes::emitEmptyError(const std::string &attrname, const char *objecttype, const char *objectid) const throw() {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' in definition of ";
    if (objectid==0) {
        oss << "a ";
    }
    if (objecttype!=0) {
        oss << objecttype;
    } else {
        oss << "<unknown type>";
    }
    if (objectid!=0) {
        oss << " '" << objectid << "'";
    }
    oss << " is empty.";
    MsgHandler::getErrorInstance()->inform(oss.str());
}


void
SUMOSAXAttributes::emitFormatError(const std::string &attrname, const std::string &type, const char *objecttype, const char *objectid) const throw() {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' in definition of ";
    if (objectid==0) {
        oss << "a ";
    }
    if (objecttype!=0) {
        oss << objecttype;
    } else {
        oss << "<unknown type>";
    }
    if (objectid!=0) {
        oss << " '" << objectid << "'";
    }
    oss << " is not " << type << ".";
    MsgHandler::getErrorInstance()->inform(oss.str());
}


void
SUMOSAXAttributes::parseStringVector(const std::string &def, std::vector<std::string> &into) throw() {
    if (def.find(';')!=std::string::npos||def.find(',')!=std::string::npos) {
        if (!myHaveInformedAboutDeprecatedDivider) {
            MsgHandler::getWarningInstance()->inform("Please note that using ';' and ',' as XML list separators is deprecated.\n From 1.0 onwards, only ' ' will be accepted.");
            myHaveInformedAboutDeprecatedDivider = true;
        }
    }
    StringTokenizer st(def, ";, ", true);
    while (st.hasNext()) {
        into.push_back(st.next());
    }
}


/****************************************************************************/

