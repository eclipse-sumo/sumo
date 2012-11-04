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
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XercesVersion.hpp>
#include <xercesc/util/TransService.hpp>
#include "SUMOSAXAttributesImpl_Binary.h"
#include <utils/common/TplConvert.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// class definitions
// ===========================================================================
SUMOSAXAttributesImpl_Binary::SUMOSAXAttributesImpl_Binary(const Attributes& attrs,
        const std::map<int, XMLCh*> &predefinedTags,
        const std::map<int, std::string> &predefinedTagsMML,
        const std::string& objectType) :
    SUMOSAXAttributes(objectType),
    myAttrs(attrs),
    myPredefinedTags(predefinedTags),
    myPredefinedTagsMML(predefinedTagsMML) { }


SUMOSAXAttributesImpl_Binary::~SUMOSAXAttributesImpl_Binary() {
}


bool
SUMOSAXAttributesImpl_Binary::hasAttribute(int id) const {
    AttrMap::const_iterator i = myPredefinedTags.find(id);
    if (i == myPredefinedTags.end()) {
        return false;
    }
    return myAttrs.getIndex((*i).second) >= 0;
}


bool
SUMOSAXAttributesImpl_Binary::getBool(int id) const throw(EmptyData, BoolFormatException) {
    return TplConvert::_2bool(getAttributeValueSecure(id));
}


bool
SUMOSAXAttributesImpl_Binary::getBoolSecure(int id, bool val) const throw(EmptyData) {
    return TplConvert::_2boolSec(getAttributeValueSecure(id), val);
}


int
SUMOSAXAttributesImpl_Binary::getInt(int id) const throw(EmptyData, NumberFormatException) {
    return TplConvert::_2int(getAttributeValueSecure(id));
}


int
SUMOSAXAttributesImpl_Binary::getIntSecure(int id,
        int def) const throw(EmptyData, NumberFormatException) {
    return TplConvert::_2intSec(getAttributeValueSecure(id), def);
}


SUMOLong
SUMOSAXAttributesImpl_Binary::getLong(int id) const throw(EmptyData, NumberFormatException) {
    return TplConvert::_2long(getAttributeValueSecure(id));
}


std::string
SUMOSAXAttributesImpl_Binary::getString(int id) const throw(EmptyData) {
    const XMLCh* utf16 = getAttributeValueSecure(id);
#if _XERCES_VERSION < 30100
    char* t = XMLString::transcode(utf16);
    std::string result(t);
    XMLString::release(&t);
    return result;
#else
    if (XMLString::stringLen(utf16) == 0) {
        // TranscodeToStr and debug_new interact badly in this case;
        return "";
    } else {
        TranscodeToStr utf8(utf16, "UTF-8");
        return TplConvert::_2str(utf8.str(), (unsigned)utf8.length());
    }
#endif
}


std::string
SUMOSAXAttributesImpl_Binary::getStringSecure(int id,
        const std::string& str) const throw(EmptyData) {
    const XMLCh* utf16 = getAttributeValueSecure(id);
#if _XERCES_VERSION < 30100
    char* t = XMLString::transcode(utf16);
    std::string result(TplConvert::_2strSec(t, str));
    XMLString::release(&t);
    return result;
#else
    if (XMLString::stringLen(utf16) == 0) {
        // TranscodeToStr and debug_new interact badly in this case;
        return "";
    } else {
        TranscodeToStr utf8(utf16, "UTF-8");
        return TplConvert::_2strSec(utf8.str(), (unsigned)utf8.length(), str);
    }
#endif
}


SUMOReal
SUMOSAXAttributesImpl_Binary::getFloat(int id) const throw(EmptyData, NumberFormatException) {
    return TplConvert::_2SUMOReal(getAttributeValueSecure(id));
}


SUMOReal
SUMOSAXAttributesImpl_Binary::getFloatSecure(int id,
        SUMOReal def) const throw(EmptyData, NumberFormatException) {
    return TplConvert::_2SUMORealSec(getAttributeValueSecure(id), def);
}


const XMLCh*
SUMOSAXAttributesImpl_Binary::getAttributeValueSecure(int id) const {
    AttrMap::const_iterator i = myPredefinedTags.find(id);
    assert(i != myPredefinedTags.end());
    return myAttrs.getValue((*i).second);
}


SUMOReal
SUMOSAXAttributesImpl_Binary::getFloat(const std::string& id) const throw(EmptyData, NumberFormatException) {
    XMLCh* t = XMLString::transcode(id.c_str());
    SUMOReal result = TplConvert::_2SUMOReal(myAttrs.getValue(t));
    XMLString::release(&t);
    return result;
}


bool
SUMOSAXAttributesImpl_Binary::hasAttribute(const std::string& id) const {
    XMLCh* t = XMLString::transcode(id.c_str());
    bool result = myAttrs.getIndex(t) >= 0;
    XMLString::release(&t);
    return result;
}


std::string
SUMOSAXAttributesImpl_Binary::getStringSecure(const std::string& id,
        const std::string& str) const {
    XMLCh* t = XMLString::transcode(id.c_str());
    std::string result = TplConvert::_2strSec(myAttrs.getValue(t), str);
    XMLString::release(&t);
    return result;
}


std::string
SUMOSAXAttributesImpl_Binary::getName(int attr) const {
    if (myPredefinedTagsMML.find(attr) == myPredefinedTagsMML.end()) {
        return "?";
    }
    return myPredefinedTagsMML.find(attr)->second;
}


void
SUMOSAXAttributesImpl_Binary::serialize(std::ostream& os) const {
    for (int i = 0; i < (int)myAttrs.getLength(); ++i) {
        os << " " << TplConvert::_2str(myAttrs.getLocalName(i));
        os << "=\"" << TplConvert::_2str(myAttrs.getValue(i)) << "\"";
    }
}


/****************************************************************************/

