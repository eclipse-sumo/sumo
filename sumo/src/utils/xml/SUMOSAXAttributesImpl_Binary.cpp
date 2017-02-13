/****************************************************************************/
/// @file    SUMOSAXAttributesImpl_Binary.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Encapsulated xml-attributes that are retrieved from the sumo-binary-xml format (already typed)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/RGBColor.h>
#include <utils/common/TplConvert.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include <utils/iodevices/BinaryFormatter.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include "SUMOSAXAttributesImpl_Binary.h"
#include "SUMOSAXAttributesImpl_Cached.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// class definitions
// ===========================================================================
SUMOSAXAttributesImpl_Binary::SUMOSAXAttributesImpl_Binary(
    const std::map<int, std::string>& predefinedTagsMML,
    const std::string& objectType,
    BinaryInputDevice* in, const char version) : SUMOSAXAttributes(objectType), myAttrIds(predefinedTagsMML) {
    while (in->peek() == BinaryFormatter::BF_XML_ATTRIBUTE) {
        int attr;
        unsigned char attrByte;
        *in >> attrByte;
        attr = attrByte;
        if (version > 1) {
            in->putback(BinaryFormatter::BF_BYTE);
            *in >> attrByte;
            attr += 256 * attrByte;
        }
        int type = in->peek();
        switch (type) {
            case BinaryFormatter::BF_BYTE:
                *in >> myCharValues[attr];
                break;
            case BinaryFormatter::BF_INTEGER:
                *in >> myIntValues[attr];
                break;
            case BinaryFormatter::BF_FLOAT:
            case BinaryFormatter::BF_SCALED2INT:
                *in >> myFloatValues[attr];
                break;
            case BinaryFormatter::BF_STRING:
                *in >> myStringValues[attr];
                break;
            case BinaryFormatter::BF_LIST: {
                int size;
                *in >> size;
                while (size > 0) {
                    const int type = in->peek();
                    if (type != BinaryFormatter::BF_POSITION_2D && type != BinaryFormatter::BF_POSITION_3D &&
                            type != BinaryFormatter::BF_SCALED2INT_POSITION_2D &&
                            type != BinaryFormatter::BF_SCALED2INT_POSITION_3D) {
                        throw ProcessError("Invalid binary file, only supporting position vectors.");
                    }
                    size--;
                    Position p;
                    *in >> p;
                    myPositionVectors[attr].push_back(p);
                }
                break;
            }
            case BinaryFormatter::BF_EDGE:
                *in >> myIntValues[attr];
                break;
            case BinaryFormatter::BF_LANE:
                *in >> myIntValues[attr];
                in->putback(BinaryFormatter::BF_BYTE);
                *in >> myCharValues[attr];
                break;
            case BinaryFormatter::BF_POSITION_2D:
            case BinaryFormatter::BF_POSITION_3D:
            case BinaryFormatter::BF_SCALED2INT_POSITION_2D:
            case BinaryFormatter::BF_SCALED2INT_POSITION_3D: {
                Position p;
                *in >> p;
                myPositionVectors[attr].push_back(p);
                break;
            }
            case BinaryFormatter::BF_BOUNDARY: {
                Position p;
                *in >> p;
                myPositionVectors[attr].push_back(p);
                in->putback(BinaryFormatter::BF_POSITION_2D);
                *in >> p;
                myPositionVectors[attr].push_back(p);
                break;
            }
            case BinaryFormatter::BF_COLOR:
                *in >> myIntValues[attr];
                break;
            case BinaryFormatter::BF_NODE_TYPE:
                *in >> myCharValues[attr];
                break;
            case BinaryFormatter::BF_EDGE_FUNCTION:
                *in >> myCharValues[attr];
                break;
            case BinaryFormatter::BF_ROUTE: {
                std::ostringstream into(std::ios::binary);
                int size;
                *in >> size;
                FileHelpers::writeByte(into, BinaryFormatter::BF_ROUTE);
                FileHelpers::writeInt(into, size);
                if (size > 0) {
                    int intsToRead = size - 1;
                    int bitsOrEntry;
                    in->putback(BinaryFormatter::BF_INTEGER);
                    *in >> bitsOrEntry;
                    FileHelpers::writeInt(into, bitsOrEntry);
                    if (bitsOrEntry < 0) {
                        intsToRead = (-bitsOrEntry * (size - 1) - 1) / sizeof(int) / 8 + 2;
                    }
                    while (intsToRead > 0) {
                        in->putback(BinaryFormatter::BF_INTEGER);
                        *in >> bitsOrEntry;
                        FileHelpers::writeInt(into, bitsOrEntry);
                        intsToRead--;
                    }
                }
                myStringValues[attr] = into.str();
                break;
            }
            default:
                throw ProcessError("Binary file is invalid, attribute type is unknown.");
        }
        myAttrs.insert(attr);
    }
}


SUMOSAXAttributesImpl_Binary::~SUMOSAXAttributesImpl_Binary() {
}


bool
SUMOSAXAttributesImpl_Binary::hasAttribute(int id) const {
    return myAttrs.find(id) != myAttrs.end();
}


bool
SUMOSAXAttributesImpl_Binary::getBool(int id) const {
    const std::map<int, char>::const_iterator i = myCharValues.find(id);
    if (i == myCharValues.end()) {
        throw EmptyData();
    }
    return i->second != 0;
}


int
SUMOSAXAttributesImpl_Binary::getInt(int id) const {
    const std::map<int, int>::const_iterator i = myIntValues.find(id);
    if (i == myIntValues.end()) {
        throw EmptyData();
    }
    return i->second;
}


long long int
SUMOSAXAttributesImpl_Binary::getLong(int /* id */) const {
    throw NumberFormatException();
}


std::string
SUMOSAXAttributesImpl_Binary::getString(int id) const {
    const std::map<int, std::string>::const_iterator i = myStringValues.find(id);
    if (i == myStringValues.end()) {
        throw EmptyData();
    }
    return i->second;
}


std::string
SUMOSAXAttributesImpl_Binary::getStringSecure(int id,
        const std::string& str) const {
    const std::map<int, std::string>::const_iterator i = myStringValues.find(id);
    if (i == myStringValues.end()) {
        return str;
    }
    return i->second;
}


SUMOReal
SUMOSAXAttributesImpl_Binary::getFloat(int id) const {
    const std::map<int, SUMOReal>::const_iterator i = myFloatValues.find(id);
    if (i == myFloatValues.end()) {
        return TplConvert::_2SUMOReal(getString(id).c_str());
    }
    return i->second;
}


SUMOReal
SUMOSAXAttributesImpl_Binary::getFloat(const std::string& /* id */) const {
    throw ProcessError("not implemented for binary data");
}


bool
SUMOSAXAttributesImpl_Binary::hasAttribute(const std::string& /* id */) const {
    throw ProcessError("not implemented for binary data");
}


std::string
SUMOSAXAttributesImpl_Binary::getStringSecure(const std::string& /* id */,
        const std::string& /* str */) const {
    throw ProcessError("not implemented for binary data");
}


SumoXMLEdgeFunc
SUMOSAXAttributesImpl_Binary::getEdgeFunc(bool& ok) const {
    const std::map<int, char>::const_iterator i = myCharValues.find(SUMO_ATTR_FUNCTION);
    if (i != myCharValues.end()) {
        const char func = i->second;
        if (func < (char)SUMOXMLDefinitions::EdgeFunctions.size()) {
            return (SumoXMLEdgeFunc)func;
        }
        ok = false;
    }
    return EDGEFUNC_NORMAL;
}


SumoXMLNodeType
SUMOSAXAttributesImpl_Binary::getNodeType(bool& ok) const {
    const std::map<int, char>::const_iterator i = myCharValues.find(SUMO_ATTR_TYPE);
    if (i != myCharValues.end()) {
        const char type = i->second;
        if (type < (char)SUMOXMLDefinitions::NodeTypes.size()) {
            return (SumoXMLNodeType)type;
        }
        ok = false;
    }
    return NODETYPE_UNKNOWN;
}


RGBColor
SUMOSAXAttributesImpl_Binary::getColor() const {
    const std::map<int, int>::const_iterator i = myIntValues.find(SUMO_ATTR_COLOR);
    if (i == myIntValues.end()) {
        throw EmptyData();
    }
    const int val = i->second;
    return RGBColor(val & 0xff, (val >> 8) & 0xff, (val >> 16) & 0xff, (val >> 24) & 0xff);
}


PositionVector
SUMOSAXAttributesImpl_Binary::getShape(int attr) const {
    const std::map<int, PositionVector>::const_iterator i = myPositionVectors.find(attr);
    if (i == myPositionVectors.end() || i->second.size() == 0) {
        throw EmptyData();
    }
    return i->second;
}


Boundary
SUMOSAXAttributesImpl_Binary::getBoundary(int attr) const {
    const std::map<int, PositionVector>::const_iterator i = myPositionVectors.find(attr);
    if (i == myPositionVectors.end() || i->second.size() == 0) {
        throw EmptyData();
    }
    if (i->second.size() != 2) {
        throw FormatException("boundary format");
    }
    return Boundary(i->second[0].x(), i->second[0].y(), i->second[1].x(), i->second[1].y());
}


std::vector<std::string>
SUMOSAXAttributesImpl_Binary::getStringVector(int attr) const {
    std::string def = getString(attr);
    std::vector<std::string> ret;
    parseStringVector(def, ret);
    return ret;
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


SUMOSAXAttributes*
SUMOSAXAttributesImpl_Binary::clone() const {
    std::map<std::string, std::string> attrs;
    for (std::map<int, char>::const_iterator it = myCharValues.begin(); it != myCharValues.end(); ++it) {
        const std::string attrName = myAttrIds.find(it->first)->second;
        attrs[attrName] = toString(it->second);
    }
    for (std::map<int, int>::const_iterator it = myIntValues.begin(); it != myIntValues.end(); ++it) {
        const std::string attrName = myAttrIds.find(it->first)->second;
        attrs[attrName] = toString(it->second);
    }
    for (std::map<int, SUMOReal>::const_iterator it = myFloatValues.begin(); it != myFloatValues.end(); ++it) {
        const std::string attrName = myAttrIds.find(it->first)->second;
        attrs[attrName] = toString(it->second);
    }
    for (std::map<int, std::string>::const_iterator it = myStringValues.begin(); it != myStringValues.end(); ++it) {
        const std::string attrName = myAttrIds.find(it->first)->second;
        attrs[attrName] = it->second;
    }
    for (std::map<int, PositionVector>::const_iterator it = myPositionVectors.begin(); it != myPositionVectors.end(); ++it) {
        const std::string attrName = myAttrIds.find(it->first)->second;
        attrs[attrName] = toString(it->second);
    }
    return new SUMOSAXAttributesImpl_Cached(attrs, myAttrIds, getObjectType());
}

/****************************************************************************/

