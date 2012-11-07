/****************************************************************************/
/// @file    SUMOSAXAttributesImpl_Binary.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Encapsulated Xerces-SAX-attributes
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

#include <cassert>
#include <sstream>
#include <utils/iodevices/BinaryFormatter.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include "SUMOSAXAttributesImpl_Binary.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// class definitions
// ===========================================================================
SUMOSAXAttributesImpl_Binary::SUMOSAXAttributesImpl_Binary(
        const std::map<int, std::string> &predefinedTagsMML,
        const std::string& objectType,
        BinaryInputDevice* in) : SUMOSAXAttributes(objectType), myAttrIds(predefinedTagsMML) {
    for (std::map<int, std::string>::const_iterator i = predefinedTagsMML.begin(); i != predefinedTagsMML.end(); ++i) {
        myAttrNames[i->second] = i->first;
    }
    int sizes[] = {1+1, 1+sizeof(int), 1+sizeof(SUMOReal), 0, 0, 1+sizeof(int), 1+sizeof(int)+1,
                   1+2*sizeof(SUMOReal), 1+3*sizeof(SUMOReal), 1+4*sizeof(SUMOReal), 1+3, 1+1, 1+1};
    while (in->peek() == BinaryFormatter::BF_XML_ATTRIBUTE) {
        int attr;
        *in >> attr;
        int type = in->peek();
        switch(type) {
            case BinaryFormatter::BF_BYTE:
                *in >> myBoolValues[attr];
                break;
            case BinaryFormatter::BF_INTEGER:
                *in >> myIntValues[attr];
                break;
            case BinaryFormatter::BF_FLOAT:
                *in >> myFloatValues[attr];
                break;
            case BinaryFormatter::BF_STRING:
                *in >> myStringValues[attr];
                break;
            case BinaryFormatter::BF_LIST: {
                std::ostringstream into; // !!! binary?
                int size;
                *in >> size;
                into << BinaryFormatter::BF_LIST << size;
                while (size > 0) {
                    int type = in->peek();
                    into << in->read(sizes[type]);
                    size--;
                }
                myStringValues[attr] = into.str();
                          }
            case BinaryFormatter::BF_EDGE:
            case BinaryFormatter::BF_LANE:
            case BinaryFormatter::BF_POSITION_2D:
            case BinaryFormatter::BF_POSITION_3D:
            case BinaryFormatter::BF_BOUNDARY:
            case BinaryFormatter::BF_COLOR:
            case BinaryFormatter::BF_NODE_TYPE:
            case BinaryFormatter::BF_EDGE_FUNCTION:
                myStringValues[attr] = in->read(sizes[type]);
                break;
            case BinaryFormatter::BF_ROUTE: {
                std::ostringstream into;
                int size;
                *in >> size;
                into << BinaryFormatter::BF_ROUTE << size;
                myStringValues[attr] = into.str();
                          }
            default:
                throw ProcessError("Invalid binary file");
        }
    }
}


SUMOSAXAttributesImpl_Binary::~SUMOSAXAttributesImpl_Binary() {
}


bool
SUMOSAXAttributesImpl_Binary::hasAttribute(int id) const {
    return myAttrs.find(id) != myAttrs.end();
}


bool
SUMOSAXAttributesImpl_Binary::getBool(int id) const throw(EmptyData, BoolFormatException) {
    const std::map<int, bool>::const_iterator i = myBoolValues.find(id);
    if (i == myBoolValues.end()) {
        throw EmptyData();
    }
    return i->second;
}


bool
SUMOSAXAttributesImpl_Binary::getBoolSecure(int id, bool val) const throw(EmptyData) {
    const std::map<int, bool>::const_iterator i = myBoolValues.find(id);
    if (i == myBoolValues.end()) {
        return val;
    }
    return i->second;
}


int
SUMOSAXAttributesImpl_Binary::getInt(int id) const throw(EmptyData, NumberFormatException) {
    const std::map<int, int>::const_iterator i = myIntValues.find(id);
    if (i == myIntValues.end()) {
        throw EmptyData();
    }
    return i->second;
}


int
SUMOSAXAttributesImpl_Binary::getIntSecure(int id,
        int def) const throw(EmptyData, NumberFormatException) {
    const std::map<int, int>::const_iterator i = myIntValues.find(id);
    if (i == myIntValues.end()) {
        return def;
    }
    return i->second;
}


SUMOLong
SUMOSAXAttributesImpl_Binary::getLong(int id) const throw(EmptyData, NumberFormatException) {
    throw NumberFormatException();
}


std::string
SUMOSAXAttributesImpl_Binary::getString(int id) const throw(EmptyData) {
    const std::map<int, std::string>::const_iterator i = myStringValues.find(id);
    if (i == myStringValues.end()) {
        throw EmptyData();
    }
    return i->second;
}


std::string
SUMOSAXAttributesImpl_Binary::getStringSecure(int id,
        const std::string& str) const throw(EmptyData) {
    const std::map<int, std::string>::const_iterator i = myStringValues.find(id);
    if (i == myStringValues.end()) {
        return str;
    }
    return i->second;
}


SUMOReal
SUMOSAXAttributesImpl_Binary::getFloat(int id) const throw(EmptyData, NumberFormatException) {
    const std::map<int, SUMOReal>::const_iterator i = myFloatValues.find(id);
    if (i == myFloatValues.end()) {
        throw EmptyData();
    }
    return i->second;
}


SUMOReal
SUMOSAXAttributesImpl_Binary::getFloatSecure(int id,
        SUMOReal def) const throw(EmptyData, NumberFormatException) {
    const std::map<int, SUMOReal>::const_iterator i = myFloatValues.find(id);
    if (i == myFloatValues.end()) {
        return def;
    }
    return i->second;
}


SUMOReal
SUMOSAXAttributesImpl_Binary::getFloat(const std::string& id) const throw(EmptyData, NumberFormatException) {
    const std::map<const std::string, int>::const_iterator i = myAttrNames.find(id);
    if (i == myAttrNames.end()) {
        throw EmptyData();
    }
    return getFloat(i->second);
}


bool
SUMOSAXAttributesImpl_Binary::hasAttribute(const std::string& id) const {
    const std::map<const std::string, int>::const_iterator i = myAttrNames.find(id);
    return i != myAttrNames.end() && hasAttribute(i->second);
}


std::string
SUMOSAXAttributesImpl_Binary::getStringSecure(const std::string& id,
        const std::string& str) const {
    const std::map<const std::string, int>::const_iterator i = myAttrNames.find(id);
    if (i == myAttrNames.end()) {
        return str;
    }
    return getStringSecure(i->second, str);
}


std::string
SUMOSAXAttributesImpl_Binary::getName(int attr) const {
    if (myAttrIds.find(attr) == myAttrIds.end()) {
        return "?";
    }
    return myAttrIds.find(attr)->second;
}


void
SUMOSAXAttributesImpl_Binary::serialize(std::ostream& os) const {
    for (std::set<int>::const_iterator i = myAttrs.begin(); i != myAttrs.end(); ++i) {
        os << " " << getName(*i);
        os << "=\"" << getStringSecure(*i, "?") << "\"";
    }
}


/****************************************************************************/

