/****************************************************************************/
/// @file    GenericSAXHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: GenericSAXHandler.h 3712 2007-03-28 14:23:50 +0200 (Mi, 28 Mrz 2007) dkrajzew $
///
// A combination between a GenericSAXHandler and an GenericSAXHandler
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

#include <cassert>
#include "GenericSAXHandler.h"
#include <utils/common/TplConvert.h>
#include <utils/common/TplConvertSec.h>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// class definitions
// ===========================================================================
GenericSAXHandler::GenericSAXHandler()
{ }


GenericSAXHandler::GenericSAXHandler(
        GenericSAXHandler::Tag *tags, 
        GenericSAXHandler::Attr *attrs)
        : _errorOccured(false), _unknownOccured(false)
{ 
    int i = 0;
    while (tags[i].key != SUMO_TAG_NOTHING) {
        _tagMap.insert(TagMap::value_type(tags[i].name, tags[i].key));
        i++;
    }
    i = 0;
    while (attrs[i].key != SUMO_ATTR_NOTHING) {
        assert(myPredefinedTags.find(attrs[i].key)==myPredefinedTags.end());
        myPredefinedTags.insert(AttrMap::value_type(attrs[i].key, convert(attrs[i].name)));
        i++;
    }
}


GenericSAXHandler::~GenericSAXHandler()
{ 
    for (AttrMap::iterator i1=myPredefinedTags.begin(); i1!=myPredefinedTags.end(); i1++) {
        delete[] (*i1).second;
    }
}


bool
GenericSAXHandler::hasAttribute(const Attributes &attrs, SumoXMLAttr id)
{
    AttrMap::const_iterator i=myPredefinedTags.find(id);
    if (i==myPredefinedTags.end()) {
        return false;
    }
    return attrs.getIndex((*i).second)>=0;
}


bool
GenericSAXHandler::hasAttribute(const Attributes &attrs,
                                const XMLCh * const id)
{
    return attrs.getIndex(id)>=0;
}


bool
GenericSAXHandler::getBool(const Attributes &attrs, SumoXMLAttr id) const
{
    return TplConvert<XMLCh>::_2bool(getAttributeValueSecure(attrs, id));
}


bool
GenericSAXHandler::getBoolSecure(const Attributes &attrs, SumoXMLAttr id, bool val) const
{
    return TplConvertSec<XMLCh>::_2boolSec(
               getAttributeValueSecure(attrs, id), val);
}


int
GenericSAXHandler::getInt(const Attributes &attrs, SumoXMLAttr id) const
{
    return TplConvert<XMLCh>::_2int(getAttributeValueSecure(attrs, id));
}


int
GenericSAXHandler::getIntSecure(const Attributes &attrs, SumoXMLAttr id,
                                int def) const
{
    return TplConvertSec<XMLCh>::_2intSec(
               getAttributeValueSecure(attrs, id), def);
}


std::string
GenericSAXHandler::getString(const Attributes &attrs, SumoXMLAttr id) const
{
    return TplConvert<XMLCh>::_2str(getAttributeValueSecure(attrs, id));
}


std::string
GenericSAXHandler::getStringSecure(const Attributes &attrs, SumoXMLAttr id,
                                   const std::string &str) const
{
    return TplConvertSec<XMLCh>::_2strSec(
               getAttributeValueSecure(attrs, id), str);
}


SUMOReal
GenericSAXHandler::getFloat(const Attributes &attrs, SumoXMLAttr id) const
{
    return TplConvert<XMLCh>::_2SUMOReal(getAttributeValueSecure(attrs, id));
}


SUMOReal
GenericSAXHandler::getFloatSecure(const Attributes &attrs, SumoXMLAttr id,
                                  SUMOReal def) const
{
    return TplConvertSec<XMLCh>::_2SUMORealSec(
               getAttributeValueSecure(attrs, id), def);
}


SUMOReal
GenericSAXHandler::getFloat(const Attributes &attrs,
                            const XMLCh * const id) const
{
    return TplConvert<XMLCh>::_2SUMOReal(attrs.getValue(id));
}


const XMLCh *
GenericSAXHandler::getAttributeValueSecure(const Attributes &attrs,
        SumoXMLAttr id) const
{
    AttrMap::const_iterator i=myPredefinedTags.find(id);
    assert(i!=myPredefinedTags.end());
    return attrs.getValue((*i).second);
}


XMLCh*
GenericSAXHandler::convert(const std::string &name) const
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



bool
GenericSAXHandler::errorOccured() const
{
    return _errorOccured;
}


bool
GenericSAXHandler::unknownOccured() const
{
    return _unknownOccured;
}


void
GenericSAXHandler::startElement(const XMLCh* const /*uri*/,
                                 const XMLCh* const /*localname*/,
                                 const XMLCh* const qname,
                                 const Attributes& attrs)
{
    string name = TplConvert<XMLCh>::_2str(qname);
    int element = convertTag(name);
    _tagTree.push(element);
    //_characters = "";
    myCharactersVector.clear();
    if (element<0) {
        _unknownOccured = true;
    }
    myStartElement(element, name, attrs);
}


void
GenericSAXHandler::endElement(const XMLCh* const /*uri*/,
                               const XMLCh* const /*localname*/,
                               const XMLCh* const qname)
{
    string name = TplConvert<XMLCh>::_2str(qname);
    int element = convertTag(name);
    if (element<0) {
        _unknownOccured = true;
    }
    // call user handler
    // collect characters
    size_t len = 0;
    size_t i;
    for (i=0; i<myCharactersVector.size(); ++i) {
        len += myCharactersVector[i].length();
    }
    char *buf = new char[len+1];
    int pos = 0;
    for (i=0; i<myCharactersVector.size(); ++i) {
        memcpy((unsigned char*) buf+pos, (unsigned char*) myCharactersVector[i].c_str(),
               sizeof(char)*myCharactersVector[i].length());
        pos += myCharactersVector[i].length();
    }
    buf[pos] = 0;

    myCharacters(element, name, buf);
    delete[] buf;
    myEndElement(element, name);
    // update the tag tree
    if (_tagTree.size()==0) {
        _errorOccured = true;
    } else {
        _tagTree.pop();
    }
}


void
GenericSAXHandler::characters(const XMLCh* const chars,
                               const unsigned int length)
{
    myCharactersVector.push_back(TplConvert<XMLCh>::_2str(chars, length));
}


int
GenericSAXHandler::convertTag(const std::string &tag) const
{
    TagMap::const_iterator i=_tagMap.find(tag);
    if (i==_tagMap.end()) {
        return -1; // !!! should it be reported (as error)
    }
    return (*i).second;
}



/****************************************************************************/

