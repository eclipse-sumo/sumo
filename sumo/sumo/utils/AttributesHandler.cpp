#include <sax2/Attributes.hpp>
#include <string>
#include <map>
#include "XMLConvert.h"
#include "AttributesHandler.h"

using namespace std;

AttributesHandler::AttributesHandler() {
}

AttributesHandler::~AttributesHandler() {
    for(TagMap::iterator i=_tags.begin(); i!=_tags.end(); i++) {
        delete (*i).second;
    }
    _tags.clear();
}

void AttributesHandler::add(int id, const std::string &name) {
   check(id);
   _tags.insert(TagMap::value_type(id, convert(name)));
}

int AttributesHandler::getInt(const Attributes &attrs, int id) const {
   TagMap::const_iterator i=_tags.find(id);
   return XMLConvert::_2int(attrs.getValue(0, (*i).second));
}

bool AttributesHandler::getBool(const Attributes &attrs, int id) const {
   TagMap::const_iterator i=_tags.find(id);
   return XMLConvert::_2bool(attrs.getValue(0, (*i).second));
}

std::string AttributesHandler::getString(const Attributes &attrs, int id) const {
   TagMap::const_iterator i=_tags.find(id);
   return XMLConvert::_2str(attrs.getValue(0, (*i).second));
}

long AttributesHandler::getLong(const Attributes &attrs, int id) const {
   TagMap::const_iterator i=_tags.find(id);
   return XMLConvert::_2long(attrs.getValue(0, (*i).second));
}

float AttributesHandler::getFloat(const Attributes &attrs, int id) const {
   TagMap::const_iterator i=_tags.find(id);
   return XMLConvert::_2float(attrs.getValue(0, (*i).second));
}

char *AttributesHandler::getCharP(const Attributes &attrs, int id) const {
   TagMap::const_iterator i=_tags.find(id);
   return XMLConvert::_2char(attrs.getValue(0, (*i).second));
}

void AttributesHandler::check(int id) const {
   if(_tags.find(id)!=_tags.end())
      throw exception();
}

unsigned short *AttributesHandler::convert(const std::string &name) const {
   size_t len = name.length();
   unsigned short *ret = new unsigned short[len+1];
   size_t i=0;
   for(; i<len; i++) {
      ret[i] = (unsigned short) name.at(i);
   }
   ret[i] = 0;
   return ret;
}