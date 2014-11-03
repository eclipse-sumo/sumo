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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2007-2014 DLR (http://www.dlr.de/) and contributors
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
#include <iostream>
#include <sstream>
#include <utils/common/MsgHandler.h>
#include <utils/common/RGBColor.h>
#include <utils/common/StringTokenizer.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include "SUMOSAXAttributes.h"

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
        return TIME2STEPS(getFloat(attr));
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
    return get<int>(attr, objectid, ok, report);
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
    return getOpt<int>(attr, objectid, ok, defaultValue, report);
#endif
}





void
SUMOSAXAttributes::emitUngivenError(const std::string& attrname, const char* objectid) const {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' is missing in definition of ";
    if (objectid == 0 || objectid[0] == 0) {
        oss << "a " << myObjectType;
    } else {
        oss << myObjectType << " '" << objectid << "'";
    }
    oss << ".";
    WRITE_ERROR(oss.str());
}


void
SUMOSAXAttributes::emitEmptyError(const std::string& attrname, const char* objectid) const {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' in definition of ";
    if (objectid == 0 || objectid[0] == 0) {
        oss << "a " << myObjectType;
    } else {
        oss << myObjectType << " '" << objectid << "'";
    }
    oss << " is empty.";
    WRITE_ERROR(oss.str());
}


void
SUMOSAXAttributes::emitFormatError(const std::string& attrname, const std::string& type, const char* objectid) const {
    std::ostringstream oss;
    oss << "Attribute '" << attrname << "' in definition of ";
    if (objectid == 0 || objectid[0] == 0) {
        oss << "a " << myObjectType;
    } else {
        oss << myObjectType << " '" << objectid << "'";
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


void
SUMOSAXAttributes::parseStringSet(const std::string& def, std::set<std::string>& into) {
    if (def.find(';') != std::string::npos || def.find(',') != std::string::npos) {
        if (!myHaveInformedAboutDeprecatedDivider) {
            WRITE_WARNING("Please note that using ';' and ',' as XML list separators is deprecated.\n From 1.0 onwards, only ' ' will be accepted.");
            myHaveInformedAboutDeprecatedDivider = true;
        }
    }
    StringTokenizer st(def, ";, ", true);
    while (st.hasNext()) {
        into.insert(st.next());
    }
}


template<> const int invalid_return<int>::value = -1;
template<> const std::string invalid_return<int>::type = "int";
template<>
int SUMOSAXAttributes::getInternal(const int attr) const {
    return getInt(attr);
}


template<> const SUMOLong invalid_return<SUMOLong>::value = -1;
template<> const std::string invalid_return<SUMOLong>::type = "long";
template<>
SUMOLong SUMOSAXAttributes::getInternal(const int attr) const {
    return getLong(attr);
}


template<> const SUMOReal invalid_return<SUMOReal>::value = -1;
template<> const std::string invalid_return<SUMOReal>::type = "float";
template<>
SUMOReal SUMOSAXAttributes::getInternal(const int attr) const {
    return getFloat(attr);
}


template<> const bool invalid_return<bool>::value = false;
template<> const std::string invalid_return<bool>::type = "bool";
template<>
bool SUMOSAXAttributes::getInternal(const int attr) const {
    return getBool(attr);
}


template<> const std::string invalid_return<std::string>::value = "";
template<> const std::string invalid_return<std::string>::type = "string";
template<>
std::string SUMOSAXAttributes::getInternal(const int attr) const {
    const std::string ret = getString(attr);
    if (ret == "") {
        throw EmptyData();
    }
    return ret;
}


template<> const RGBColor invalid_return<RGBColor>::value = RGBColor();
template<> const std::string invalid_return<RGBColor>::type = "color";
template<>
RGBColor SUMOSAXAttributes::getInternal(const int /* attr */) const {
    return getColor();
}


template<> const PositionVector invalid_return<PositionVector>::value = PositionVector();
template<> const std::string invalid_return<PositionVector>::type = "PositionVector";
template<>
PositionVector SUMOSAXAttributes::getInternal(const int attr) const {
    return getShape(attr);
}


template<> const Boundary invalid_return<Boundary>::value = Boundary();
template<> const std::string invalid_return<Boundary>::type = "Boundary";
template<>
Boundary SUMOSAXAttributes::getInternal(const int attr) const {
    return getBoundary(attr);
}


/****************************************************************************/

