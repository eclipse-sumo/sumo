/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOSAXAttributesImpl_Cached.cpp
/// @author  Jakob Erdmann
/// @date    Dec 2016
/// @version $Id$
///
// Encapsulated xml-attributes that use a map from string-attr-names to string-attr-values as backend
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
#include <xercesc/util/TranscodingException.hpp>
#include <utils/common/RGBColor.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringBijection.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include "SUMOSAXAttributesImpl_Cached.h"
#include "SUMOSAXAttributesImpl_Cached.h"


// ===========================================================================
// class definitions
// ===========================================================================
SUMOSAXAttributesImpl_Cached::SUMOSAXAttributesImpl_Cached(
    const std::map<std::string, std::string>& attrs,
    const std::map<int, std::string>& predefinedTagsMML,
    const std::string& objectType) :
    SUMOSAXAttributes(objectType),
    myAttrs(attrs),
    myPredefinedTagsMML(predefinedTagsMML) { }


SUMOSAXAttributesImpl_Cached::~SUMOSAXAttributesImpl_Cached() {
}


bool
SUMOSAXAttributesImpl_Cached::hasAttribute(int id) const {
    std::map<int, std::string>::const_iterator i = myPredefinedTagsMML.find(id);
    if (i == myPredefinedTagsMML.end()) {
        return false;
    }
    return myAttrs.find((*i).second) != myAttrs.end();
}


bool
SUMOSAXAttributesImpl_Cached::getBool(int id) const {
    return TplConvert::_2bool(getAttributeValueSecure(id));
}


int
SUMOSAXAttributesImpl_Cached::getInt(int id) const {
    return TplConvert::_2int(getAttributeValueSecure(id));
}


long long int
SUMOSAXAttributesImpl_Cached::getLong(int id) const {
    return TplConvert::_2long(getAttributeValueSecure(id));
}


std::string
SUMOSAXAttributesImpl_Cached::getString(int id) const {
    return getAttributeValueSecure(id);
}


std::string
SUMOSAXAttributesImpl_Cached::getStringSecure(int id,
        const std::string& str) const {
    std::string result = getAttributeValueSecure(id);
    return result.size() == 0 ? str : result;
}


double
SUMOSAXAttributesImpl_Cached::getFloat(int id) const {
    return TplConvert::_2double(getAttributeValueSecure(id));
}


const char*
SUMOSAXAttributesImpl_Cached::getAttributeValueSecure(int id) const {
    std::map<int, std::string>::const_iterator i = myPredefinedTagsMML.find(id);
    assert(i != myPredefinedTagsMML.end());
    return myAttrs.find((*i).second)->second.c_str();
}


double
SUMOSAXAttributesImpl_Cached::getFloat(const std::string& id) const {
    return TplConvert::_2double(myAttrs.find(id)->second.c_str());
}


bool
SUMOSAXAttributesImpl_Cached::hasAttribute(const std::string& id) const {
    return myAttrs.find(id) != myAttrs.end();
}


std::string
SUMOSAXAttributesImpl_Cached::getStringSecure(const std::string& id,
        const std::string& str) const {
    std::map<std::string, std::string>::const_iterator it = myAttrs.find(id);
    if (it != myAttrs.end() && it->second != "") {
        return it->second;
    } else {
        return str;
    }
}


SumoXMLEdgeFunc
SUMOSAXAttributesImpl_Cached::getEdgeFunc(bool& ok) const {
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
SUMOSAXAttributesImpl_Cached::getNodeType(bool& ok) const {
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
SUMOSAXAttributesImpl_Cached::getColor() const {
    return RGBColor::parseColor(getString(SUMO_ATTR_COLOR));
}


PositionVector
SUMOSAXAttributesImpl_Cached::getShape(int attr) const {
    StringTokenizer st(getString(attr));
    PositionVector shape;
    while (st.hasNext()) {
        StringTokenizer pos(st.next(), ",");
        if (pos.size() != 2 && pos.size() != 3) {
            throw FormatException("shape format");
        }
        double x = TplConvert::_2double(pos.next().c_str());
        double y = TplConvert::_2double(pos.next().c_str());
        if (pos.size() == 2) {
            shape.push_back(Position(x, y));
        } else {
            double z = TplConvert::_2double(pos.next().c_str());
            shape.push_back(Position(x, y, z));
        }
    }
    return shape;
}


Boundary
SUMOSAXAttributesImpl_Cached::getBoundary(int attr) const {
    std::string def = getString(attr);
    StringTokenizer st(def, ",");
    if (st.size() != 4) {
        throw FormatException("boundary format");
    }
    const double xmin = TplConvert::_2double(st.next().c_str());
    const double ymin = TplConvert::_2double(st.next().c_str());
    const double xmax = TplConvert::_2double(st.next().c_str());
    const double ymax = TplConvert::_2double(st.next().c_str());
    return Boundary(xmin, ymin, xmax, ymax);
}


std::vector<std::string>
SUMOSAXAttributesImpl_Cached::getStringVector(int attr) const {
    std::string def = getString(attr);
    std::vector<std::string> ret;
    parseStringVector(def, ret);
    return ret;
}


std::string
SUMOSAXAttributesImpl_Cached::getName(int attr) const {
    if (myPredefinedTagsMML.find(attr) == myPredefinedTagsMML.end()) {
        return "?";
    }
    return myPredefinedTagsMML.find(attr)->second;
}


void
SUMOSAXAttributesImpl_Cached::serialize(std::ostream& os) const {
    for (std::map<std::string, std::string>::const_iterator it = myAttrs.begin(); it != myAttrs.end(); ++it) {
        os << " " << it->first;
        os << "=\"" << it->second << "\"";
    }
}


SUMOSAXAttributes*
SUMOSAXAttributesImpl_Cached::clone() const {
    return new SUMOSAXAttributesImpl_Cached(myAttrs, myPredefinedTagsMML, getObjectType());
}

/****************************************************************************/

