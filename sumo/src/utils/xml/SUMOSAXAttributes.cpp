/****************************************************************************/
/// @file    SUMOSAXAttributes.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 30 Mar 2007
/// @version $Id$
///
// Encapsulated SAX-Attributes
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
const std::string SUMOSAXAttributes::ENCODING = " encoding=\"UTF-8\"";


// ===========================================================================
// method definitions
// ===========================================================================
SUMOSAXAttributes::SUMOSAXAttributes(const std::string& objectType):
    myObjectType(objectType) {}


int
SUMOSAXAttributes::getIntReporting(int attr, const char* objectid,
                                   bool& ok, bool report) const {
    if (!hasAttribute(attr)) {
        if (report) {
            emitUngivenError(getName(attr), objectid);
        }
        ok = false;
        return -1;
    }
    try {
        return getInt(attr);
    } catch (NumberFormatException&) {
        if (report) {
            emitFormatError(getName(attr), "an int", objectid);
        }
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return -1;
}


int
SUMOSAXAttributes::getOptIntReporting(int attr, const char* objectid,
                                      bool& ok, int defaultValue, bool report) const {
    if (!hasAttribute(attr)) {
        return defaultValue;
    }
    try {
        return getInt(attr);
    } catch (NumberFormatException&) {
        if (report) {
            emitFormatError(getName(attr), "an int", objectid);
        }
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return -1;
}


SUMOLong
SUMOSAXAttributes::getLongReporting(int attr, const char* objectid,
                                    bool& ok, bool report) const {
    if (!hasAttribute(attr)) {
        if (report) {
            emitUngivenError(getName(attr), objectid);
        }
        ok = false;
        return -1;
    }
    try {
        return getLong(attr);
    } catch (NumberFormatException&) {
        if (report) {
            emitFormatError(getName(attr), "an int", objectid);
        }
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return -1;
}


SUMOReal
SUMOSAXAttributes::getSUMORealReporting(int attr, const char* objectid,
                                        bool& ok, bool report) const {
    if (!hasAttribute(attr)) {
        if (report) {
            emitUngivenError(getName(attr), objectid);
        }
        ok = false;
        return -1;
    }
    try {
        return getFloat(attr);
    } catch (NumberFormatException&) {
        if (report) {
            emitFormatError(getName(attr), "a real number", objectid);
        }
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return (SUMOReal) - 1;
}


SUMOReal
SUMOSAXAttributes::getOptSUMORealReporting(int attr, const char* objectid,
        bool& ok, SUMOReal defaultValue, bool report) const {
    if (!hasAttribute(attr)) {
        return defaultValue;
    }
    try {
        return getFloat(attr);
    } catch (NumberFormatException&) {
        if (report) {
            emitFormatError(getName(attr), "a real number", objectid);
        }
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return (SUMOReal) - 1;
}


bool
SUMOSAXAttributes::getBoolReporting(int attr, const char* objectid,
                                    bool& ok, bool report) const {
    if (!hasAttribute(attr)) {
        if (report) {
            emitUngivenError(getName(attr), objectid);
        }
        ok = false;
        return false;
    }
    try {
        return getBool(attr);
    } catch (BoolFormatException&) {
        if (report) {
            emitFormatError(getName(attr), "a boolean", objectid);
        }
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return false;
}


bool
SUMOSAXAttributes::getOptBoolReporting(int attr, const char* objectid,
                                       bool& ok, bool defaultValue, bool report) const {
    if (!hasAttribute(attr)) {
        return defaultValue;
    }
    try {
        return getBool(attr);
    } catch (BoolFormatException&) {
        if (report) {
            emitFormatError(getName(attr), "a boolean", objectid);
        }
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return false;
}


std::string
SUMOSAXAttributes::getStringReporting(int attr, const char* objectid,
                                      bool& ok, bool report) const {
    if (!hasAttribute(attr)) {
        if (report) {
            emitUngivenError(getName(attr), objectid);
        }
        ok = false;
        return "";
    }
    try {
        std::string ret = getString(attr);
        if (ret == "") {
            throw EmptyData();
        }
        return ret;
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return "";
}


std::string
SUMOSAXAttributes::getOptStringReporting(int attr, const char* objectid,
        bool& ok, const std::string& defaultValue, bool report) const {
    if (!hasAttribute(attr)) {
        return defaultValue;
    }
    try {
        return getString(attr);
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return "";
}


SUMOTime
SUMOSAXAttributes::getSUMOTimeReporting(int attr, const char* objectid,
                                        bool& ok, bool report) const {
#ifdef HAVE_SUBSECOND_TIMESTEPS
    if (!hasAttribute(attr)) {
        if (report) {
            emitUngivenError(getName(attr), objectid);
        }
        ok = false;
        return -1;
    }
    try {
        return (SUMOTime)(getFloat(attr) * 1000.);
    } catch (NumberFormatException&) {
        if (report) {
            emitFormatError(getName(attr), "a time value", objectid);
        }
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return (SUMOTime) - 1;
#else
    return getIntReporting(attr, objectid, ok, report);
#endif
}


SUMOTime
SUMOSAXAttributes::getOptSUMOTimeReporting(int attr, const char* objectid,
        bool& ok, SUMOTime defaultValue, bool report) const {
#ifdef HAVE_SUBSECOND_TIMESTEPS
    if (!hasAttribute(attr)) {
        return defaultValue;
    }
    try {
        return (SUMOTime)(getFloat(attr) * 1000.);
    } catch (NumberFormatException&) {
        if (report) {
            emitFormatError(getName(attr), "a real number", objectid);
        }
    } catch (EmptyData&) {
        if (report) {
            emitEmptyError(getName(attr), objectid);
        }
    }
    ok = false;
    return (SUMOTime) - 1;
#else
    return getOptIntReporting(attr, objectid, ok, defaultValue, report);
#endif
}





void
SUMOSAXAttributes::emitUngivenError(const std::string& attrname, const char* objectid) const {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' is missing in definition of ";
    if (objectid == 0) {
        oss << "a ";
    }
    oss << myObjectType;
    if (objectid != 0) {
        oss << " '" << objectid << "'";
    }
    oss << ".";
    WRITE_ERROR(oss.str());
}


void
SUMOSAXAttributes::emitEmptyError(const std::string& attrname, const char* objectid) const {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' in definition of ";
    if (objectid == 0) {
        oss << "a ";
    }
    oss << myObjectType;
    if (objectid != 0) {
        oss << " '" << objectid << "'";
    }
    oss << " is empty.";
    WRITE_ERROR(oss.str());
}


void
SUMOSAXAttributes::emitFormatError(const std::string& attrname, const std::string& type, const char* objectid) const {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' in definition of ";
    if (objectid == 0) {
        oss << "a ";
    }
    oss << myObjectType;
    if (objectid != 0) {
        oss << " '" << objectid << "'";
    }
    oss << " is not " << type << ".";
    WRITE_ERROR(oss.str());
}


void
SUMOSAXAttributes::parseStringVector(const std::string& def, std::vector<std::string>& into) {
    if (def.find(';') != std::string::npos || def.find(',') != std::string::npos) {
        if (!myHaveInformedAboutDeprecatedDivider) {
            WRITE_WARNING("Please note that using ';' and ',' as XML list separators is deprecated.\n From 1.0 onwards, only ' ' will be accepted.");
            myHaveInformedAboutDeprecatedDivider = true;
        }
    }
    StringTokenizer st(def, ";, ", true);
    while (st.hasNext()) {
        into.push_back(st.next());
    }
}


/****************************************************************************/

