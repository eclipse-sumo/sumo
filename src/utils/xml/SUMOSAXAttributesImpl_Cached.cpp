/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    SUMOSAXAttributesImpl_Cached.cpp
/// @author  Jakob Erdmann
/// @date    Dec 2016
///
// Encapsulated xml-attributes that use a map from string-attr-names to string-attr-values as backend
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XercesVersion.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/TranscodingException.hpp>
#include <utils/common/RGBColor.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
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
    const std::vector<std::string>& predefinedTagsMML,
    const std::string& objectType) :
    SUMOSAXAttributes(objectType),
    myAttrs(attrs),
    myPredefinedTagsMML(predefinedTagsMML) { }


SUMOSAXAttributesImpl_Cached::~SUMOSAXAttributesImpl_Cached() { }


bool
SUMOSAXAttributesImpl_Cached::hasAttribute(int id) const {
    assert(id >= 0);
    assert(id < (int)myPredefinedTagsMML.size());
    return myAttrs.find(myPredefinedTagsMML[id]) != myAttrs.end();
}


std::string
SUMOSAXAttributesImpl_Cached::getString(int id, bool* isPresent) const {
    const auto it = myAttrs.find(myPredefinedTagsMML[id]);
    if (it != myAttrs.end()) {
        return it->second;
    }
    *isPresent = false;
    return "";
}


std::string
SUMOSAXAttributesImpl_Cached::getStringSecure(int id, const std::string& str) const {
    const std::string& result = getAttributeValueSecure(id);
    return result.size() == 0 ? str : result;
}


const std::string&
SUMOSAXAttributesImpl_Cached::getAttributeValueSecure(int id) const {
    assert(id >= 0);
    assert(id < (int)myPredefinedTagsMML.size());
    return myAttrs.find(myPredefinedTagsMML[id])->second;
}


double
SUMOSAXAttributesImpl_Cached::getFloat(const std::string& id) const {
    return StringUtils::toDouble(myAttrs.find(id)->second);
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


std::string
SUMOSAXAttributesImpl_Cached::getName(int attr) const {
    assert(attr >= 0);
    assert(attr < (int)myPredefinedTagsMML.size());
    return myPredefinedTagsMML[attr];
}


void
SUMOSAXAttributesImpl_Cached::serialize(std::ostream& os) const {
    for (std::map<std::string, std::string>::const_iterator it = myAttrs.begin(); it != myAttrs.end(); ++it) {
        os << " " << it->first;
        os << "=\"" << it->second << "\"";
    }
}

std::vector<std::string>
SUMOSAXAttributesImpl_Cached::getAttributeNames() const {
    std::vector<std::string> result;
    for (std::map<std::string, std::string>::const_iterator it = myAttrs.begin(); it != myAttrs.end(); ++it) {
        result.push_back(it->first);
    }
    return result;
}

SUMOSAXAttributes*
SUMOSAXAttributesImpl_Cached::clone() const {
    return new SUMOSAXAttributesImpl_Cached(myAttrs, myPredefinedTagsMML, getObjectType());
}


/****************************************************************************/
