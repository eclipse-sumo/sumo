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
/// @file    SUMOSAXAttributesImpl_Xerces.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// Encapsulated Xerces-SAX-attributes
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <utils/common/RGBColor.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include "XMLSubSys.h"
#include "SUMOSAXAttributesImpl_Xerces.h"
#include "SUMOSAXAttributesImpl_Cached.h"


// ===========================================================================
// class definitions
// ===========================================================================
SUMOSAXAttributesImpl_Xerces::SUMOSAXAttributesImpl_Xerces(const XERCES_CPP_NAMESPACE::Attributes& attrs,
        const std::vector<XMLCh*>& predefinedTags,
        const std::vector<std::string>& predefinedTagsMML,
        const std::string& objectType) :
    SUMOSAXAttributes(objectType),
    myAttrs(attrs),
    myPredefinedTags(predefinedTags),
    myPredefinedTagsMML(predefinedTagsMML) { }


SUMOSAXAttributesImpl_Xerces::~SUMOSAXAttributesImpl_Xerces() {
}


bool
SUMOSAXAttributesImpl_Xerces::hasAttribute(int id) const {
    assert(id >= 0);
    assert(id < (int)myPredefinedTags.size());
    return myAttrs.getIndex(myPredefinedTags[id]) >= 0;
}


std::string
SUMOSAXAttributesImpl_Xerces::getString(int id, bool* isPresent) const {
    const XMLCh* const xString = getAttributeValueSecure(id);
    if (xString != nullptr) {
        return StringUtils::transcode(getAttributeValueSecure(id));
    }
    *isPresent = false;
    return "";
}


std::string
SUMOSAXAttributesImpl_Xerces::getStringSecure(int id, const std::string& str) const {
    const XMLCh* utf16 = getAttributeValueSecure(id);
    if (XERCES_CPP_NAMESPACE::XMLString::stringLen(utf16) == 0) {
        // TranscodeToStr and debug_new interact badly in this case;
        return str;
    } else {
        return getString(id);
    }
}


const XMLCh*
SUMOSAXAttributesImpl_Xerces::getAttributeValueSecure(int id) const {
    assert(id >= 0);
    assert(id < (int)myPredefinedTags.size());
    return myAttrs.getValue(myPredefinedTags[id]);
}


double
SUMOSAXAttributesImpl_Xerces::getFloat(const std::string& id) const {
    XMLCh* t = XERCES_CPP_NAMESPACE::XMLString::transcode(id.c_str());
    const std::string utf8 = StringUtils::transcode(myAttrs.getValue(t));
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    return StringUtils::toDouble(utf8);
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
    const XMLCh* v = myAttrs.getValue(t);
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    if (v == nullptr) {
        return str;
    } else {
        return StringUtils::transcode(v);
    }
}


std::string
SUMOSAXAttributesImpl_Xerces::getName(int attr) const {
    assert(attr >= 0);
    assert(attr < (int)myPredefinedTagsMML.size());
    return myPredefinedTagsMML[attr];
}


void
SUMOSAXAttributesImpl_Xerces::serialize(std::ostream& os) const {
    for (int i = 0; i < (int)myAttrs.getLength(); ++i) {
        os << " " << StringUtils::transcode(myAttrs.getLocalName(i));
        os << "=\"" << StringUtils::transcode(myAttrs.getValue(i)) << "\"";
    }
}


std::vector<std::string>
SUMOSAXAttributesImpl_Xerces::getAttributeNames() const {
    std::vector<std::string> result;
    for (int i = 0; i < (int)myAttrs.getLength(); ++i) {
        result.push_back(StringUtils::transcode(myAttrs.getLocalName(i)));
    }
    return result;
}


SUMOSAXAttributes*
SUMOSAXAttributesImpl_Xerces::clone() const {
    std::map<std::string, std::string> attrs;
    for (int i = 0; i < (int)myAttrs.getLength(); ++i) {
        attrs[StringUtils::transcode(myAttrs.getLocalName(i))] = StringUtils::transcode(myAttrs.getValue(i));
    }
    return new SUMOSAXAttributesImpl_Cached(attrs, myPredefinedTagsMML, getObjectType());
}


/****************************************************************************/
