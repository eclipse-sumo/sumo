/****************************************************************************/
/// @file    AttributesHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 15 Apr 2002
/// @version $Id: $
///
// This class realises the access to the
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <sax2/Attributes.hpp>
#include <string>
#include <map>
#include <utils/common/TplConvert.h>
#include <utils/common/TplConvertSec.h>
#include "AttributesHandler.h"

/*
#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG
*/

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
AttributesHandler::AttributesHandler()
{}


AttributesHandler::AttributesHandler(Attr *attrs, int noAttrs)
{
    for (int i=0; i<noAttrs; i++) {
        add(attrs[i].key, attrs[i].name);
    }
}


AttributesHandler::~AttributesHandler()
{
    for (AttrMap::iterator i1=myPredefinedTags.begin(); i1!=myPredefinedTags.end(); i1++) {
        delete(*i1).second;
    }
    for (StrAttrMap::iterator i2=myStrTags.begin(); i2!=myStrTags.end(); i2++) {
        delete(*i2).second;
    }
}


void
AttributesHandler::add(int id, const std::string &name)
{
    check(id);
    myPredefinedTags.insert(AttrMap::value_type(id, convert(name)));
}


bool
AttributesHandler::hasAttribute(const Attributes &attrs, int id)
{
    AttrMap::const_iterator i=myPredefinedTags.find(id);
    if (i==myPredefinedTags.end()) {
        return false;
    }
    return attrs.getIndex((*i).second)>=0;
}


bool
AttributesHandler::hasAttribute(const Attributes &attrs,
                                const std::string &id)
{
    return attrs.getIndex(getAttributeNameSecure(id))>=0;
}


int
AttributesHandler::getInt(const Attributes &attrs, int id) const
{
    return TplConvert<XMLCh>::_2int(getAttributeValueSecure(attrs, id));
}


int
AttributesHandler::getIntSecure(const Attributes &attrs, int id,
                                int def) const
{
    return TplConvertSec<XMLCh>::_2intSec(
               getAttributeValueSecure(attrs, id), def);
}


int
AttributesHandler::getInt(const Attributes &attrs, const std::string &id) const
{
    return TplConvert<XMLCh>::_2int(getAttributeValueSecure(attrs, id));
}


int
AttributesHandler::getIntSecure(const Attributes &attrs,
                                const std::string &id,
                                int def) const
{
    return TplConvertSec<XMLCh>::_2intSec(
               getAttributeValueSecure(attrs, id), def);
}


bool
AttributesHandler::getBool(const Attributes &attrs, int id) const
{
    return TplConvert<XMLCh>::_2bool(getAttributeValueSecure(attrs, id));
}


bool
AttributesHandler::getBoolSecure(const Attributes &attrs, int id, bool val) const
{
    return TplConvertSec<XMLCh>::_2boolSec(
               getAttributeValueSecure(attrs, id), val);
}


bool
AttributesHandler::getBool(const Attributes &attrs, const std::string &id) const
{
    return TplConvert<XMLCh>::_2bool(getAttributeValueSecure(attrs, id));
}


bool
AttributesHandler::getBoolSecure(const Attributes &attrs,
                                 const std::string &id, bool val) const
{
    return TplConvertSec<XMLCh>::_2boolSec(
               getAttributeValueSecure(attrs, id), val);
}


std::string
AttributesHandler::getString(const Attributes &attrs, int id) const
{
    return TplConvert<XMLCh>::_2str(getAttributeValueSecure(attrs, id));
}


std::string
AttributesHandler::getStringSecure(const Attributes &attrs, int id,
                                   const std::string &str) const
{
    return TplConvertSec<XMLCh>::_2strSec(
               getAttributeValueSecure(attrs, id), str);
}


std::string
AttributesHandler::getString(const Attributes &attrs,
                             const std::string &id) const
{
    return TplConvert<XMLCh>::_2str(getAttributeValueSecure(attrs, id));
}


std::string
AttributesHandler::getStringSecure(const Attributes &attrs,
                                   const std::string &id,
                                   const std::string &str) const
{
    return TplConvertSec<XMLCh>::_2strSec(
               getAttributeValueSecure(attrs, id), str);
}


long
AttributesHandler::getLong(const Attributes &attrs, int id) const
{
    return TplConvert<XMLCh>::_2long(getAttributeValueSecure(attrs, id));
}


long
AttributesHandler::getLongSecure(const Attributes &attrs, int id,
                                 long def) const
{
    return TplConvertSec<XMLCh>::_2longSec(
               getAttributeValueSecure(attrs, id), def);
}


SUMOReal
AttributesHandler::getFloat(const Attributes &attrs, int id) const
{
    return TplConvert<XMLCh>::_2SUMOReal(getAttributeValueSecure(attrs, id));
}


SUMOReal
AttributesHandler::getFloatSecure(const Attributes &attrs, int id,
                                  SUMOReal def) const
{
    return TplConvertSec<XMLCh>::_2SUMORealSec(
               getAttributeValueSecure(attrs, id), def);
}


SUMOReal
AttributesHandler::getFloat(const Attributes &attrs,
                            const std::string &id) const
{
    return TplConvert<XMLCh>::_2SUMOReal(getAttributeValueSecure(attrs, id));
}


SUMOReal
AttributesHandler::getFloatSecure(const Attributes &attrs,
                                  const std::string &id,
                                  SUMOReal def) const
{
    return TplConvertSec<XMLCh>::_2SUMORealSec(
               getAttributeValueSecure(attrs, id), def);
}


const XMLCh *const
AttributesHandler::getAttributeNameSecure(int id) const
{
    AttrMap::const_iterator i=myPredefinedTags.find(id);
    if (i==myPredefinedTags.end()) {
        throw EmptyData();
    }
    return (*i).second;
}


const XMLCh *const
AttributesHandler::getAttributeNameSecure(const std::string &id) const
{
    StrAttrMap::const_iterator i=myStrTags.find(id);
    if (i==myStrTags.end()) {
        myStrTags.insert(StrAttrMap::value_type(id, convert(id)));
        return myStrTags.find(id)->second;
    }
    return (*i).second;
}


const XMLCh *
AttributesHandler::getAttributeValueSecure(const Attributes &attrs,
        int id) const
{
    return attrs.getValue(getAttributeNameSecure(id));
}


const XMLCh *
AttributesHandler::getAttributeValueSecure(const Attributes &attrs,
        const std::string &id) const
{
    return attrs.getValue(getAttributeNameSecure(id));
}


char *
AttributesHandler::getCharP(const Attributes &attrs, int id) const
{
    AttrMap::const_iterator i=myPredefinedTags.find(id);
    return TplConvert<XMLCh>::_2charp(attrs.getValue(0, (*i).second));
}


void
AttributesHandler::check(int id) const
{
    if (myPredefinedTags.find(id)!=myPredefinedTags.end()) {
        throw exception();
    }
}


XMLCh*
AttributesHandler::convert(const std::string &name) const
{
    size_t len = name.length();
    XMLCh *ret = new XMLCh[len+1];
    size_t i=0;
    for (; i<len; i++) {
        ret[i] = (XMLCh) name.at(i);
    }
    ret[i] = 0;
    return ret;
}



/****************************************************************************/

