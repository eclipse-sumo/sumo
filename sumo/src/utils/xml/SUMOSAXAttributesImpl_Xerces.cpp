/****************************************************************************/
/// @file    SUMOSAXAttributesImpl_Xerces.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Encapsulated Xerces-SAX-attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XercesVersion.hpp>
#include <xercesc/util/TransService.hpp>
#include <utils/common/RGBColor.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include "SUMOSAXAttributesImpl_Xerces.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// class definitions
// ===========================================================================
SUMOSAXAttributesImpl_Xerces::SUMOSAXAttributesImpl_Xerces(const XERCES_CPP_NAMESPACE::Attributes& attrs,
        const std::map<int, XMLCh*>& predefinedTags,
        const std::map<int, std::string>& predefinedTagsMML,
        const std::string& objectType) :
    SUMOSAXAttributes(objectType),
    myAttrs(attrs),
    myPredefinedTags(predefinedTags),
    myPredefinedTagsMML(predefinedTagsMML) { }


SUMOSAXAttributesImpl_Xerces::~SUMOSAXAttributesImpl_Xerces() {
}


bool
SUMOSAXAttributesImpl_Xerces::hasAttribute(int id) const {
    AttrMap::const_iterator i = myPredefinedTags.find(id);
    if (i == myPredefinedTags.end()) {
        return false;
    }
    return myAttrs.getIndex((*i).second) >= 0;
}


bool
SUMOSAXAttributesImpl_Xerces::getBool(int id) const {
    return TplConvert::_2bool(getAttributeValueSecure(id));
}


int
SUMOSAXAttributesImpl_Xerces::getInt(int id) const {
    return TplConvert::_2int(getAttributeValueSecure(id));
}


SUMOLong
SUMOSAXAttributesImpl_Xerces::getLong(int id) const {
    return TplConvert::_2long(getAttributeValueSecure(id));
}


std::string
SUMOSAXAttributesImpl_Xerces::getString(int id) const {
    const XMLCh* utf16 = getAttributeValueSecure(id);
#if _XERCES_VERSION < 30100
    char* t = XERCES_CPP_NAMESPACE::XMLString::transcode(utf16);
    std::string result(t);
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    return result;
#else
    if (XERCES_CPP_NAMESPACE::XMLString::stringLen(utf16) == 0) {
        // TranscodeToStr and debug_new interact badly in this case;
        return "";
    } else {
        XERCES_CPP_NAMESPACE::TranscodeToStr utf8(utf16, "UTF-8");
        return TplConvert::_2str(utf8.str(), (unsigned)utf8.length());
    }
#endif
}


std::string
SUMOSAXAttributesImpl_Xerces::getStringSecure(int id,
        const std::string& str) const {
    const XMLCh* utf16 = getAttributeValueSecure(id);
#if _XERCES_VERSION < 30100
    char* t = XERCES_CPP_NAMESPACE::XMLString::transcode(utf16);
    std::string result(TplConvert::_2strSec(t, str));
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    return result;
#else
    if (XERCES_CPP_NAMESPACE::XMLString::stringLen(utf16) == 0) {
        // TranscodeToStr and debug_new interact badly in this case;
        return "";
    } else {
        XERCES_CPP_NAMESPACE::TranscodeToStr utf8(utf16, "UTF-8");
        return TplConvert::_2strSec(utf8.str(), (unsigned)utf8.length(), str);
    }
#endif
}


SUMOReal
SUMOSAXAttributesImpl_Xerces::getFloat(int id) const {
    return TplConvert::_2SUMOReal(getAttributeValueSecure(id));
}


const XMLCh*
SUMOSAXAttributesImpl_Xerces::getAttributeValueSecure(int id) const {
    AttrMap::const_iterator i = myPredefinedTags.find(id);
    assert(i != myPredefinedTags.end());
    return myAttrs.getValue((*i).second);
}


SUMOReal
SUMOSAXAttributesImpl_Xerces::getFloat(const std::string& id) const {
    XMLCh* t = XERCES_CPP_NAMESPACE::XMLString::transcode(id.c_str());
    SUMOReal result = TplConvert::_2SUMOReal(myAttrs.getValue(t));
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    return result;
}


bool
SUMOSAXAttributesImpl_Xerces::hasAttribute(const std::string& id) const {
    XMLCh* t = XERCES_CPP_NAMESPACE::XMLString::transcode(id.c_str());
    bool result = myAttrs.getIndex(t) >= 0;
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    return result;
}


std::string
SUMOSAXAttributesImpl_Xerces::getStringSecure(const std::string& id,
        const std::string& str) const {
    XMLCh* t = XERCES_CPP_NAMESPACE::XMLString::transcode(id.c_str());
    std::string result = TplConvert::_2strSec(myAttrs.getValue(t), str);
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    return result;
}


SumoXMLEdgeFunc
SUMOSAXAttributesImpl_Xerces::getEdgeFunc(bool& ok) const {
    if (hasAttribute(SUMO_ATTR_FUNCTION)) {
        std::string funcString = getString(SUMO_ATTR_FUNCTION);
        if (SUMOXMLDefinitions::EdgeFunctions.hasString(funcString)) {
            return SUMOXMLDefinitions::EdgeFunctions.get(funcString);
        }
        ok = false;
    }
    return EDGEFUNC_NORMAL;
}


SumoXMLNodeType
SUMOSAXAttributesImpl_Xerces::getNodeType(bool& ok) const {
    if (hasAttribute(SUMO_ATTR_TYPE)) {
        std::string typeString = getString(SUMO_ATTR_TYPE);
        if (SUMOXMLDefinitions::NodeTypes.hasString(typeString)) {
            return SUMOXMLDefinitions::NodeTypes.get(typeString);
        }
        ok = false;
    }
    return NODETYPE_UNKNOWN;
}


RGBColor
SUMOSAXAttributesImpl_Xerces::getColor() const {
    return RGBColor::parseColor(getString(SUMO_ATTR_COLOR));
}


PositionVector
SUMOSAXAttributesImpl_Xerces::getShape(int attr) const {
    StringTokenizer st(getString(attr));
    PositionVector shape;
    while (st.hasNext()) {
        StringTokenizer pos(st.next(), ",");
        if (pos.size() != 2 && pos.size() != 3) {
            throw FormatException("shape format");
        }
        SUMOReal x = TplConvert::_2SUMOReal(pos.next().c_str());
        SUMOReal y = TplConvert::_2SUMOReal(pos.next().c_str());
        if (pos.size() == 2) {
            shape.push_back(Position(x, y));
        } else {
            SUMOReal z = TplConvert::_2SUMOReal(pos.next().c_str());
            shape.push_back(Position(x, y, z));
        }
    }
    return shape;
}


Boundary
SUMOSAXAttributesImpl_Xerces::getBoundary(int attr) const {
    std::string def = getString(attr);
    StringTokenizer st(def, ",");
    if (st.size() != 4) {
        throw FormatException("boundary format");
    }
    const SUMOReal xmin = TplConvert::_2SUMOReal(st.next().c_str());
    const SUMOReal ymin = TplConvert::_2SUMOReal(st.next().c_str());
    const SUMOReal xmax = TplConvert::_2SUMOReal(st.next().c_str());
    const SUMOReal ymax = TplConvert::_2SUMOReal(st.next().c_str());
    return Boundary(xmin, ymin, xmax, ymax);
}


std::vector<std::string>
SUMOSAXAttributesImpl_Xerces::getStringVector(int attr) const {
    std::string def = getString(attr);
    std::vector<std::string> ret;
    parseStringVector(def, ret);
    return ret;
}


std::string
SUMOSAXAttributesImpl_Xerces::getName(int attr) const {
    if (myPredefinedTagsMML.find(attr) == myPredefinedTagsMML.end()) {
        return "?";
    }
    return myPredefinedTagsMML.find(attr)->second;
}


void
SUMOSAXAttributesImpl_Xerces::serialize(std::ostream& os) const {
    for (int i = 0; i < (int)myAttrs.getLength(); ++i) {
        os << " " << TplConvert::_2str(myAttrs.getLocalName(i));
        os << "=\"" << TplConvert::_2str(myAttrs.getValue(i)) << "\"";
    }
}


/****************************************************************************/

