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
#include "SUMOSAXAttributesImpl_Xerces.h"
#include <utils/common/TplConvert.h>
#include <utils/common/TplConvertSec.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// class definitions
// ===========================================================================
SUMOSAXAttributesImpl_Xerces::SUMOSAXAttributesImpl_Xerces(const Attributes& attrs,
        const std::map<int, XMLCh*> &predefinedTags,
        const std::map<int, std::string> &predefinedTagsMML,
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
    return TplConvert<XMLCh>::_2bool(getAttributeValueSecure(id));
}


bool
SUMOSAXAttributesImpl_Xerces::getBoolSecure(int id, bool val) const throw(EmptyData) {
    return TplConvertSec<XMLCh>::_2boolSec(getAttributeValueSecure(id), val);
}


int
SUMOSAXAttributesImpl_Xerces::getInt(int id) const throw(EmptyData, NumberFormatException) {
    return TplConvert<XMLCh>::_2int(getAttributeValueSecure(id));
}


int
SUMOSAXAttributesImpl_Xerces::getIntSecure(int id,
        int def) const throw(EmptyData, NumberFormatException) {
    return TplConvertSec<XMLCh>::_2intSec(getAttributeValueSecure(id), def);
}


long
SUMOSAXAttributesImpl_Xerces::getLong(int id) const throw(EmptyData, NumberFormatException) {
    return TplConvert<XMLCh>::_2long(getAttributeValueSecure(id));
}


std::string
SUMOSAXAttributesImpl_Xerces::getString(int id) const throw(EmptyData) {
    const XMLCh* utf16 = getAttributeValueSecure(id);
#if _XERCES_VERSION < 30000
    return TplConvert<XMLCh>::_2str(utf16);
#else
    if (XMLString::stringLen(utf16) == 0) {
        // TranscodeToStr and debug_new interact badly in this case;
        return "";
    } else {
        TranscodeToStr utf8(utf16, "UTF-8");
        return TplConvert<XMLByte>::_2str(utf8.str(), (unsigned)utf8.length());
    }
#endif
}


std::string
SUMOSAXAttributesImpl_Xerces::getStringSecure(int id,
        const std::string& str) const throw(EmptyData) {
    const XMLCh* utf16 = getAttributeValueSecure(id);
#if _XERCES_VERSION < 30000
    return TplConvertSec<XMLCh>::_2strSec(utf16, str);
#else
    if (XMLString::stringLen(utf16) == 0) {
        // TranscodeToStr and debug_new interact badly in this case;
        return "";
    } else {
        TranscodeToStr utf8(utf16, "UTF-8");
        return TplConvertSec<XMLByte>::_2strSec(utf8.str(), (int)utf8.length(), str);
    }
#endif
}


SUMOReal
SUMOSAXAttributesImpl_Xerces::getFloat(int id) const throw(EmptyData, NumberFormatException) {
    return TplConvert<XMLCh>::_2SUMOReal(getAttributeValueSecure(id));
}


SUMOReal
SUMOSAXAttributesImpl_Xerces::getFloatSecure(int id,
        SUMOReal def) const throw(EmptyData, NumberFormatException) {
    return TplConvertSec<XMLCh>::_2SUMORealSec(getAttributeValueSecure(id), def);
}


const XMLCh*
SUMOSAXAttributesImpl_Xerces::getAttributeValueSecure(int id) const {
    AttrMap::const_iterator i = myPredefinedTags.find(id);
    assert(i != myPredefinedTags.end());
    return myAttrs.getValue((*i).second);
}


SUMOReal
SUMOSAXAttributesImpl_Xerces::getFloat(const std::string& id) const throw(EmptyData, NumberFormatException) {
    XMLCh* t = XMLString::transcode(id.c_str());
    SUMOReal result = TplConvert<XMLCh>::_2SUMOReal(myAttrs.getValue(t));
    XMLString::release(&t);
    return result;
}


bool
SUMOSAXAttributesImpl_Xerces::hasAttribute(const std::string& id) const {
    XMLCh* t = XMLString::transcode(id.c_str());
    bool result = myAttrs.getIndex(t) >= 0;
    XMLString::release(&t);
    return result;
}


std::string
SUMOSAXAttributesImpl_Xerces::getStringSecure(const std::string& id,
        const std::string& str) const {
    XMLCh* t = XMLString::transcode(id.c_str());
    std::string result = TplConvertSec<XMLCh>::_2strSec(myAttrs.getValue(t), str);
    XMLString::release(&t);
    return result;
}


std::string
SUMOSAXAttributesImpl_Xerces::getName(int attr) const {
    if (myPredefinedTagsMML.find(attr) == myPredefinedTagsMML.end()) {
        return "?";
    }
    return myPredefinedTagsMML.find(attr)->second;
}


/****************************************************************************/

