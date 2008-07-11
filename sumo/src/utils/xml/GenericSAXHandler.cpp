/****************************************************************************/
/// @file    GenericSAXHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A handler which converts occuring elements and attributes into enums
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include "GenericSAXHandler.h"
#include <utils/common/TplConvert.h>
#include <utils/common/TplConvertSec.h>
#include "SUMOSAXAttributesImpl_Xerces.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// class definitions
// ===========================================================================
GenericSAXHandler::GenericSAXHandler() throw()
{ }


GenericSAXHandler::GenericSAXHandler(GenericSAXHandler::Tag *tags,
                                     GenericSAXHandler::Attr *attrs) throw()
{
    int i = 0;
    while (tags[i].key != SUMO_TAG_NOTHING) {
        myTagMap.insert(TagMap::value_type(tags[i].name, tags[i].key));
        i++;
    }
    i = 0;
    while (attrs[i].key != SUMO_ATTR_NOTHING) {
        assert(myPredefinedTags.find(attrs[i].key)==myPredefinedTags.end());
        myPredefinedTags[attrs[i].key] = convert(attrs[i].name);
        myPredefinedTagsMML[attrs[i].key] = attrs[i].name;
        i++;
    }
}


GenericSAXHandler::~GenericSAXHandler() throw()
{
    for (AttrMap::iterator i1=myPredefinedTags.begin(); i1!=myPredefinedTags.end(); i1++) {
        delete[](*i1).second;
    }
}

/*
bool
GenericSAXHandler::hasAttribute(const Attributes &attrs, SumoXMLAttr id) throw()
{
    AttrMap::const_iterator i=myPredefinedTags.find(id);
    if (i==myPredefinedTags.end()) {
        return false;
    }
    return attrs.getIndex((*i).second)>=0;
}


bool
GenericSAXHandler::hasAttribute(const Attributes &attrs,
                                const XMLCh * const id) throw()
{
    return attrs.getIndex(id)>=0;
}


bool
GenericSAXHandler::getBool(const Attributes &attrs, SumoXMLAttr id) const throw(EmptyData, BoolFormatException)
{
    return TplConvert<XMLCh>::_2bool(getAttributeValueSecure(attrs, id));
}


bool
GenericSAXHandler::getBoolSecure(const Attributes &attrs, SumoXMLAttr id, bool val) const throw(EmptyData)
{
    return TplConvertSec<XMLCh>::_2boolSec(getAttributeValueSecure(attrs, id), val);
}


int
GenericSAXHandler::getInt(const Attributes &attrs, SumoXMLAttr id) const throw(EmptyData, NumberFormatException)
{
    return TplConvert<XMLCh>::_2int(getAttributeValueSecure(attrs, id));
}


int
GenericSAXHandler::getIntSecure(const Attributes &attrs, SumoXMLAttr id,
                                int def) const throw(EmptyData, NumberFormatException)
{
    return TplConvertSec<XMLCh>::_2intSec(getAttributeValueSecure(attrs, id), def);
}


std::string
GenericSAXHandler::getString(const Attributes &attrs, SumoXMLAttr id) const throw(EmptyData)
{
    return TplConvert<XMLCh>::_2str(getAttributeValueSecure(attrs, id));
}


std::string
GenericSAXHandler::getStringSecure(const Attributes &attrs, SumoXMLAttr id,
                                   const std::string &str) const throw(EmptyData)
{
    return TplConvertSec<XMLCh>::_2strSec(getAttributeValueSecure(attrs, id), str);
}


std::string
GenericSAXHandler::getStringSecure(const Attributes &attrs, const XMLCh * const id,
                                   const std::string &str) const throw(EmptyData)
{
    return TplConvertSec<XMLCh>::_2strSec(attrs.getValue(id), str);
}


SUMOReal
GenericSAXHandler::getFloat(const Attributes &attrs, SumoXMLAttr id) const throw(EmptyData, NumberFormatException)
{
    return TplConvert<XMLCh>::_2SUMOReal(getAttributeValueSecure(attrs, id));
}


SUMOReal
GenericSAXHandler::getFloatSecure(const Attributes &attrs, SumoXMLAttr id,
                                  SUMOReal def) const throw(EmptyData, NumberFormatException)
{
    return TplConvertSec<XMLCh>::_2SUMORealSec(getAttributeValueSecure(attrs, id), def);
}


SUMOReal
GenericSAXHandler::getFloat(const Attributes &attrs,
                            const XMLCh * const id) const throw(EmptyData, NumberFormatException)
{
    return TplConvert<XMLCh>::_2SUMOReal(attrs.getValue(id));
}


const XMLCh *
GenericSAXHandler::getAttributeValueSecure(const Attributes &attrs,
        SumoXMLAttr id) const throw()
{
    AttrMap::const_iterator i=myPredefinedTags.find(id);
    assert(i!=myPredefinedTags.end());
    return attrs.getValue((*i).second);
}
*/

XMLCh*
GenericSAXHandler::convert(const std::string &name) const throw()
{
    size_t len = name.length();
    XMLCh *ret = new XMLCh[len+1];
    size_t i=0;
    for (; i<len; i++) {
        ret[i] = (XMLCh) name[i];
    }
    ret[i] = 0;
    return ret;
}


void
GenericSAXHandler::startElement(const XMLCh* const /*uri*/,
                                const XMLCh* const /*localname*/,
                                const XMLCh* const qname,
                                const Attributes& attrs)
{
    string name = TplConvert<XMLCh>::_2str(qname);
    SumoXMLTag element = convertTag(name);
    //myTagTree.push(element);
    //_characters = "";
    myCharactersVector.clear();
    SUMOSAXAttributesImpl_Xerces na(attrs, myPredefinedTags, myPredefinedTagsMML);
    myStartElement(element, na);
}


void
GenericSAXHandler::endElement(const XMLCh* const /*uri*/,
                              const XMLCh* const /*localname*/,
                              const XMLCh* const qname)
{
    string name = TplConvert<XMLCh>::_2str(qname);
    SumoXMLTag element = convertTag(name);
    // collect characters
    if(myCharactersVector.size()!=0) {
        size_t len = 0;
        unsigned i;
        for (i=0; i<myCharactersVector.size(); ++i) {
            len += myCharactersVector[i].length();
        }
        char *buf = new char[len+1];
        size_t pos = 0;
        for (i=0; i<myCharactersVector.size(); ++i) {
            memcpy((unsigned char*) buf+pos, (unsigned char*) myCharactersVector[i].c_str(),
                   sizeof(char)*myCharactersVector[i].length());
            pos += myCharactersVector[i].length();
        }
        buf[pos] = 0;

        // call user handler
        try {
            myCharacters(element, buf);
        } catch (std::runtime_error &) {
            delete[] buf;
            throw;
        }
        delete[] buf;
    }
    myEndElement(element);
}


void
GenericSAXHandler::characters(const XMLCh* const chars,
                              const unsigned int length)
{
    myCharactersVector.push_back(TplConvert<XMLCh>::_2str(chars, length));
}


SumoXMLTag
GenericSAXHandler::convertTag(const std::string &tag) const throw()
{
    TagMap::const_iterator i=myTagMap.find(tag);
    if (i==myTagMap.end()) {
        return SUMO_TAG_NOTHING;
    }
    return (*i).second;
}


void
GenericSAXHandler::myStartElement(SumoXMLTag, const SUMOSAXAttributes &) throw(ProcessError)
{}


void
GenericSAXHandler::myCharacters(SumoXMLTag, const std::string &) throw(ProcessError)
{}


void
GenericSAXHandler::myEndElement(SumoXMLTag) throw(ProcessError)
{}


/****************************************************************************/

