/***************************************************************************
                          AttributesHandler.cpp
                          This class realises the access to the 
                            SAX2-Attributes
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
// $Log$
// Revision 1.3  2002/04/17 11:19:56  dkrajzew
// windows-carriage returns removed
//
// Revision 1.2  2002/04/16 06:52:01  dkrajzew
// documentation added; coding standard attachements added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sax2/Attributes.hpp>
#include <string>
#include <map>
#include "XMLConvert.h"
#include "AttributesHandler.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "AttributesHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:

