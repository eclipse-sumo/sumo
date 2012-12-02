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
SUMOSAXAttributesImpl_Xerces::getBool(int id) const throw(EmptyData, BoolFormatException) {
    return TplConvert::_2bool(getAttributeValueSecure(id));
}


bool
SUMOSAXAttributesImpl_Xerces::getBoolSecure(int id, bool val) const throw(EmptyData) {
    return TplConvert::_2boolSec(getAttributeValueSecure(id), val);
}


int
SUMOSAXAttributesImpl_Xerces::getInt(int id) const {
    return TplConvert::_2int(getAttributeValueSecure(id));
}


int
SUMOSAXAttributesImpl_Xerces::getIntSecure(int id,
        int def) const {
    return TplConvert::_2intSec(getAttributeValueSecure(id), def);
}


SUMOLong
SUMOSAXAttributesImpl_Xerces::getLong(int id) const {
    return TplConvert::_2long(getAttributeValueSecure(id));
}


std::string
SUMOSAXAttributesImpl_Xerces::getString(int id) const throw(EmptyData) {
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
        const std::string& str) const throw(EmptyData) {
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


SUMOReal
SUMOSAXAttributesImpl_Xerces::getFloatSecure(int id,
        SUMOReal def) const {
    return TplConvert::_2SUMORealSec(getAttributeValueSecure(id), def);
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
SUMOSAXAttributesImpl_Xerces::getColorReporting(const char* objectid, bool& ok) const {
    try {
        return RGBColor::parseColor(getString(SUMO_ATTR_COLOR));
    } catch (NumberFormatException&) {
    } catch (EmptyData&) {
    }
    ok = false;
    emitFormatError("color", "a valid color", objectid);
    return RGBColor();
}


PositionVector
SUMOSAXAttributesImpl_Xerces::getShapeReporting(int attr, const char* objectid, bool& ok,
        bool allowEmpty) const {
    std::string shpdef = getOptStringReporting(attr, objectid, ok, "");
    if (shpdef == "") {
        if (!allowEmpty) {
            emitEmptyError(getName(attr), objectid);
            ok = false;
        }
        return PositionVector();
    }
    StringTokenizer st(shpdef, " ");
    PositionVector shape;
    while (st.hasNext()) {
        StringTokenizer pos(st.next(), ",");
        if (pos.size() != 2 && pos.size() != 3) {
            emitFormatError(getName(attr), "x,y or x,y,z", objectid);
            ok = false;
            return PositionVector();
        }
        try {
            SUMOReal x = TplConvert::_2SUMOReal(pos.next().c_str());
            SUMOReal y = TplConvert::_2SUMOReal(pos.next().c_str());
            if (pos.size() == 2) {
                shape.push_back(Position(x, y));
            } else {
                SUMOReal z = TplConvert::_2SUMOReal(pos.next().c_str());
                shape.push_back(Position(x, y, z));
            }
        } catch (NumberFormatException&) {
            emitFormatError(getName(attr), "all numeric position entries", objectid);
            ok = false;
            return PositionVector();
        } catch (EmptyData&) {
            emitFormatError(getName(attr), "all valid entries", objectid);
            ok = false;
            return PositionVector();
        }
    }
    return shape;
}


Boundary
SUMOSAXAttributesImpl_Xerces::getBoundaryReporting(int attr, const char* objectid, bool& ok) const {
    std::string def = getStringReporting(attr, objectid, ok);
    StringTokenizer st(def, ",");
    if (st.size() != 4) {
        emitFormatError(getName(attr), "a valid number of entries", objectid);
        ok = false;
        return Boundary();
    }
    try {
        const SUMOReal xmin = TplConvert::_2SUMOReal(st.next().c_str());
        const SUMOReal ymin = TplConvert::_2SUMOReal(st.next().c_str());
        const SUMOReal xmax = TplConvert::_2SUMOReal(st.next().c_str());
        const SUMOReal ymax = TplConvert::_2SUMOReal(st.next().c_str());
        return Boundary(xmin, ymin, xmax, ymax);
    } catch (NumberFormatException&) {
        emitFormatError(getName(attr), "all numeric entries", objectid);
    } catch (EmptyData&) {
        emitFormatError(getName(attr), "all valid entries", objectid);
    }
    ok = false;
    return Boundary();
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

