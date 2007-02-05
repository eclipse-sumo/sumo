/****************************************************************************/
/// @file    GenericSAX2Handler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 15 Apr 2002
/// @version $Id$
///
// A class extending the SAX-parser functionality
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

#include <string>
#include <stack>
#include <map>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <utils/common/TplConvert.h>
#include "GenericSAX2Handler.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
GenericSAX2Handler::GenericSAX2Handler()
        : DefaultHandler(), _errorOccured(false), _unknownOccured(false)
{}


GenericSAX2Handler::GenericSAX2Handler(const Tag *tags, int tagNo)
        : DefaultHandler(), _errorOccured(false), _unknownOccured(false)
{
    for (int i=0; i<tagNo; i++) {
        addTag(tags[i].name, tags[i].value);
    }
}


GenericSAX2Handler::~GenericSAX2Handler()
{}


void
GenericSAX2Handler::addTag(const std::string &name, int id)
{
    _tagMap.insert(TagMap::value_type(name, id));
}


bool
GenericSAX2Handler::errorOccured() const
{
    return _errorOccured;
}


bool
GenericSAX2Handler::unknownOccured() const
{
    return _unknownOccured;
}


void
GenericSAX2Handler::startElement(const XMLCh* const /*uri*/,
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
GenericSAX2Handler::endElement(const XMLCh* const /*uri*/,
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
GenericSAX2Handler::characters(const XMLCh* const chars,
                               const unsigned int length)
{
    myCharactersVector.push_back(TplConvert<XMLCh>::_2str(chars, length));
}


void
GenericSAX2Handler::ignorableWhitespace(const XMLCh* const /*chars*/,
                                        const unsigned int /*length*/)
{}


void
GenericSAX2Handler::resetDocument()
{}


void
GenericSAX2Handler::warning(const SAXParseException&)
{}


void
GenericSAX2Handler::error(const SAXParseException&)
{}


void
GenericSAX2Handler::fatalError(const SAXParseException&)
{}


void
GenericSAX2Handler::myStartElementDump(int /*element*/,
                                       const std::string &/*name*/,
                                       const Attributes &/*attrs*/)
{}


void
GenericSAX2Handler::myCharactersDump(int /*element*/,
                                     const std::string &/*name*/,
                                     const std::string &/*chars*/)
{}


void
GenericSAX2Handler::myEndElementDump(int /*element*/,
                                     const std::string &/*name*/)
{}


int
GenericSAX2Handler::convertTag(const std::string &tag) const
{
    TagMap::const_iterator i=_tagMap.find(tag);
    if (i==_tagMap.end()) {
        return -1; // !!! should it be reported (as error)
    }
    return (*i).second;
}


string
GenericSAX2Handler::buildErrorMessage(const std::string &file,
                                      const string &type,
                                      const SAXParseException& exception)
{
    ostringstream buf;
    buf << type << endl;
    buf << TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
    buf << " In file: " << file << endl;
    buf << " At line/column " << exception.getLineNumber()+1
    << '/' << exception.getColumnNumber() << ")." << endl;
    return buf.str();
}



/****************************************************************************/

