/***************************************************************************
                          GenericSAX2Handler.cpp
                          A class extending the SAX-parser functionality
                             -------------------
    begin                : Mon, 15 Apr 2002
    copyright            : (C) 2002 by Daniel Krajzewicz
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
    Attention!!!
    As one of few, this module is under the
        Lesser GNU General Public Licence
    *********************************************************************
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 ***************************************************************************/
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.1  2002/10/16 14:54:04  dkrajzew
// initial commit for xml-related utility functions
//
// Revision 1.7  2002/06/21 10:50:24  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.6  2002/06/11 14:38:22  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/06/11 13:43:35  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/06/10 08:33:22  dkrajzew
// Parsing of strings into other data formats generelized; Options now recognize false numeric values; documentation added
//
// Revision 1.3  2002/04/17 11:19:57  dkrajzew
// windows-carriage returns removed
//
// Revision 1.2  2002/04/16 06:52:01  dkrajzew
// documentation added; coding standard attachements added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <stack>
#include <map>
#include <sax2/Attributes.hpp>
#include <sax2/DefaultHandler.hpp>
#include <utils/convert/TplConvert.h>
#include "GenericSAX2Handler.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
GenericSAX2Handler::GenericSAX2Handler()
   : DefaultHandler(), _errorOccured(false), _unknownOccured(false)
{
}


GenericSAX2Handler::GenericSAX2Handler(const Tag *tags, int tagNo)
   : DefaultHandler(), _errorOccured(false), _unknownOccured(false)
{
   for(int i=0; i<tagNo; i++) {
      addTag(tags[i].name, tags[i].value);
   }
}


GenericSAX2Handler::~GenericSAX2Handler() {
}


void
GenericSAX2Handler::addTag(const std::string &name, int id)
{
    _tagMap.insert(TagMap::value_type(name, id));
}


bool
GenericSAX2Handler::errorOccured() const {
   return _errorOccured;
}


bool
GenericSAX2Handler::unknownOccured() const {
   return _unknownOccured;
}


void
GenericSAX2Handler::startElement(const XMLCh* const uri,
                                 const XMLCh* const localname,
                                 const XMLCh* const qname,
                                 const Attributes& attrs) {
   string name = TplConvert<XMLCh>::_2str(qname);
   int element = convertTag(name);
   _tagTree.push(element);
   _characters = "";
   if(element<0)
      _unknownOccured = true;
   myStartElement(element, name, attrs);
}


void
GenericSAX2Handler::endElement(const XMLCh* const uri,
                               const XMLCh* const localname,
                               const XMLCh* const qname) {
   string name = TplConvert<XMLCh>::_2str(qname);
   int element = convertTag(name);
   if(element<0)
      _unknownOccured = true;
   // call user handler
   myCharacters(element, name, _characters);
   myEndElement(element, name);
   // update the tag tree
   if(_tagTree.size()==0)
      _errorOccured = true;
   else
      _tagTree.pop();
}


void
GenericSAX2Handler::characters(const XMLCh* const chars,
                               const unsigned int length) {
   _characters += TplConvert<XMLCh>::_2str(chars, length);
}


void
GenericSAX2Handler::ignorableWhitespace(const XMLCh* const chars,
                                        const unsigned int length) {
}


void
GenericSAX2Handler::resetDocument() {
}


void
GenericSAX2Handler::warning(const SAXParseException& exception) {
}


void
GenericSAX2Handler::error(const SAXParseException& exception) {
}


void
GenericSAX2Handler::fatalError(const SAXParseException& exception) {
}


void
GenericSAX2Handler::myStartElementDump(int element,
                                       const std::string &name,
                                       const Attributes &attrs) {
}


void
GenericSAX2Handler::myCharactersDump(int element,
                                     const std::string &name,
                                     const std::string &chars)
{
}


void
GenericSAX2Handler::myEndElementDump(int element,
                                     const std::string &name) {
}


int
GenericSAX2Handler::convertTag(const std::string &tag) const {
   TagMap::const_iterator i=_tagMap.find(tag);
   if(i==_tagMap.end())
      return -1; // !!! should it be reported (as error)
   return (*i).second;
}


string
GenericSAX2Handler::buildErrorMessage(const std::string &file,
                                      const string &type,
                                      const SAXParseException& exception) {
    ostringstream buf;
    buf << type << endl;
    buf << TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
    buf << " In file: " << file << endl;
    buf << " At line/column " << exception.getLineNumber()+1 
        << '/' << exception.getColumnNumber() << ")." << endl;
    return buf.str();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GenericSAX2Handler.icc"
//#endif

// Local Variables:
// mode:C++
//

