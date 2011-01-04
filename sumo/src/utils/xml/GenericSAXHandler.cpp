/****************************************************************************/
/// @file    GenericSAXHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A handler which converts occuring elements and attributes into enums
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/FileHelpers.h>
#include "SUMOSAXAttributesImpl_Xerces.h"
#include "XMLSubSys.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// class definitions
// ===========================================================================
GenericSAXHandler::GenericSAXHandler(GenericSAXHandler::Tag *tags,
                                     GenericSAXHandler::Attr *attrs, const std::string &file) throw()
        : myParentHandler(0), myParentIndicator(SUMO_TAG_NOTHING), myFileName(file) {
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


GenericSAXHandler::~GenericSAXHandler() throw() {
    for (AttrMap::iterator i1=myPredefinedTags.begin(); i1!=myPredefinedTags.end(); i1++) {
        delete[](*i1).second;
    }
}


void
GenericSAXHandler::setFileName(const std::string &name) throw() {
    myFileName = name;
}


const std::string &
GenericSAXHandler::getFileName() const throw() {
    return myFileName;
}


XMLCh*
GenericSAXHandler::convert(const std::string &name) const throw() {
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
                                const Attributes& attrs) {
    std::string name = TplConvert<XMLCh>::_2str(qname);
    SumoXMLTag element = convertTag(name);
    myCharactersVector.clear();
    SUMOSAXAttributesImpl_Xerces na(attrs, myPredefinedTags, myPredefinedTagsMML);
    if (element == SUMO_TAG_INCLUDE) {
        std::string file = na.getString(SUMO_ATTR_HREF);
        if (!FileHelpers::isAbsolute(file)) {
            file = FileHelpers::getConfigurationRelative(getFileName(), file);
        }
        XMLSubSys::runParser(*this, file);
    } else {
        myStartElement(element, na);
    }
}


void
GenericSAXHandler::endElement(const XMLCh* const /*uri*/,
                              const XMLCh* const /*localname*/,
                              const XMLCh* const qname) {
    std::string name = TplConvert<XMLCh>::_2str(qname);
    SumoXMLTag element = convertTag(name);
    // collect characters
    if (myCharactersVector.size()!=0) {
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
    if (element != SUMO_TAG_INCLUDE) {
        myEndElement(element);
        if (myParentHandler && myParentIndicator == element) {
            XMLSubSys::setHandler(*myParentHandler);
            myParentIndicator = SUMO_TAG_NOTHING;
            myParentHandler = 0;
        }
    }
}


void
GenericSAXHandler::registerParent(const SumoXMLTag tag, GenericSAXHandler* handler) {
    myParentHandler = handler;
    myParentIndicator = tag;
    XMLSubSys::setHandler(*this);
}


void
GenericSAXHandler::characters(const XMLCh* const chars,
                              const XERCES3_SIZE_t length) {
    myCharactersVector.push_back(TplConvert<XMLCh>::_2str(chars, static_cast<unsigned int>(length)));
}


SumoXMLTag
GenericSAXHandler::convertTag(const std::string &tag) const throw() {
    TagMap::const_iterator i=myTagMap.find(tag);
    if (i==myTagMap.end()) {
        return SUMO_TAG_NOTHING;
    }
    return (*i).second;
}


void
GenericSAXHandler::myStartElement(SumoXMLTag, const SUMOSAXAttributes &) throw(ProcessError) {}


void
GenericSAXHandler::myCharacters(SumoXMLTag, const std::string &) throw(ProcessError) {}


void
GenericSAXHandler::myEndElement(SumoXMLTag) throw(ProcessError) {}


/****************************************************************************/

