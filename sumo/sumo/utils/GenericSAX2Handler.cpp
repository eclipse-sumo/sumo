#include <string>
#include <stack>
#include <map>
#include <sax2/Attributes.hpp>
#include <sax2/DefaultHandler.hpp>
#include "GenericSAX2Handler.h"
#include "XMLConvert.h"

using namespace std;

GenericSAX2Handler::GenericSAX2Handler() 
   : DefaultHandler(), _errorOccured(false), _unknownOccured(false)
{
}

GenericSAX2Handler::GenericSAX2Handler(const Tag *tags, int tagNo) 
   : DefaultHandler(), _errorOccured(false), _unknownOccured(false)
{
   for(int i=0; i<tagNo; i++) {
      _tagMap.insert(TagMap::value_type(tags[i].name, tags[i].value));
   }
}

GenericSAX2Handler::~GenericSAX2Handler() {
}

bool GenericSAX2Handler::errorOccured() const {
   return _errorOccured;
}

bool GenericSAX2Handler::unknownOccured() const {
   return _unknownOccured;
}

void GenericSAX2Handler::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs) {
   string name = XMLConvert::_2str(qname);
   int element = convertTag(name);
   _tagTree.push(element);
   _characters = "";
   if(element<0)
      _unknownOccured = true;
   myStartElement(element, name, attrs);
}

void GenericSAX2Handler::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname) {
   string name = XMLConvert::_2str(qname);
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

void GenericSAX2Handler::characters(const XMLCh* const chars, const unsigned int length) {
   _characters += XMLConvert::_2str(chars, length);
}

void GenericSAX2Handler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {
}

void GenericSAX2Handler::resetDocument() {
}

void GenericSAX2Handler::warning(const SAXParseException& exception) {
}

void GenericSAX2Handler::error(const SAXParseException& exception) {
}

void GenericSAX2Handler::fatalError(const SAXParseException& exception) {
}

void GenericSAX2Handler::myStartElementDump(int element, const std::string &name, const Attributes &attrs) {
}

void GenericSAX2Handler::myCharactersDump(int element, const char *chars) {
}

void GenericSAX2Handler::myEndElementDump(int element, const std::string &name) {
}

int GenericSAX2Handler::convertTag(const std::string &tag) const {
   TagMap::const_iterator i=_tagMap.find(tag);
   if(i==_tagMap.end()) 
      return -1; // !!! should it be reported (as error)
   return (*i).second;
}

