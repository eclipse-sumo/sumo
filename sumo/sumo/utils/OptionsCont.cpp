/***************************************************************************
                          OptionsCont.h
			  A container for options. 
			  Allows the access of the values of the stored options
			  using different option names.
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2002/04/17 11:19:57  dkrajzew
// windows-carriage returns removed
//
// Revision 1.3  2002/04/16 12:25:37  dkrajzew
// Security assertion included into getPath
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.3  2002/03/20 08:20:00  dkrajzew
// isDefault - method added
//
// Revision 2.2  2002/03/11 10:24:01  traffic
// superflous collapse option removed.
//
// Revision 1.2  2002/03/11 10:07:52  traffic
// superflous collapse option removed.
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 2.1  2002/02/18 11:06:11  traffic
// "getValues()" now returns a string
//
// Revision 2.0  2002/02/14 14:43:27  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:19  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <map>
#include <string>
#include <exception>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include "Option.h"
#include "OptionsCont.h"
#include "UtilExceptions.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
OptionsCont::OptionsCont() :  
  _addresses(0), _values(), _path("") 
{
}


OptionsCont::OptionsCont(string path) :  
  _addresses(0), _values(), _path(path) 
{
   char *tmp = getenv(_path.c_str());
   if(tmp==0) {
      cout << "You must set the enviroment variable '" << path << "'!" << endl;
      throw InvalidArgument("Enviroment variable is not set");
   }
}

OptionsCont::~OptionsCont() {
    for(ItemAddressContType::iterator i=_addresses.begin(); i!=_addresses.end(); i++) {
      delete (*i);
    }
}

void OptionsCont::doRegister(string name1, Option *v) {
  assert(v!=0);
    ItemAddressContType::iterator i = find(_addresses.begin(), _addresses.end(), v);
    if(i==_addresses.end())
      _addresses.push_back(v);
    _values.insert(KnownContType::value_type(name1, v));
}

void OptionsCont::doRegister(string name1, char abbr, Option *v) {
    doRegister(name1, v);
    doRegister(convertChar(abbr), v);
}

void OptionsCont::doRegisterSystemPath(std::string name1, std::string value) {
  Option *o = new Option_String(getSystemPath(value));
  doRegister(name1, o);
}

void OptionsCont::doRegisterSystemPath(std::string name1, char abbr, std::string value) {
  Option *o = new Option_String(getSystemPath(value));
  doRegister(name1, o);
  doRegister(convertChar(abbr), o);
}

void OptionsCont::addSynonyme(string name1, string name2) {
    KnownContType::iterator i1 = _values.find(name1);
    KnownContType::iterator i2 = _values.find(name2);
    if(i1==_values.end()&&i2==_values.end())
      throw InvalidArgument("Neither the option '" + name1 + "' nor the option '" + name2 + "' is known yet");
    if(i1!=_values.end()&&i2!=_values.end()) {
      if((*i1).second==(*i2).second) 
        return;
      throw new InvalidArgument("Both options '" + name1 + "' and '" + name2 + "' do exist and differ.");
    }
    if(i1==_values.end()&&i2!=_values.end())
      doRegister(name1, (*i2).second);
    if(i1!=_values.end()&&i2==_values.end())
      doRegister(name2, (*i1).second);
}

bool OptionsCont::exists(string name) const {
  KnownContType::const_iterator i = _values.find(name);
  return i!=_values.end();
}

bool OptionsCont::isSet(string name) const {
  KnownContType::const_iterator i = _values.find(name);
  if(i==_values.end()) 
      return false;
  return (*i).second->isSet();
}

bool OptionsCont::isDefault(std::string name) const {
  KnownContType::const_iterator i = _values.find(name);
  if(i==_values.end()) 
      return false;
  return (*i).second->isDefault();
}

Option *OptionsCont::getSecure(string name) const {
  KnownContType::const_iterator i = _values.find(name);
  if(i==_values.end()) {
      cout << "No option with the name '" << name << "' exists." << endl;
      throw InvalidArgument("No option with the name '" + name + "' exists.");
  }
  return (*i).second;
}

string OptionsCont::getString(string name) const {
    Option *o = getSecure(name);
    return o->getString();
}

float OptionsCont::getFloat(string name) const {
    Option *o = getSecure(name);
    return o->getFloat();
}

int OptionsCont::getInt(string name) const {
    Option *o = getSecure(name);
    return o->getInt();
}

long OptionsCont::getLong(string name) const {
    Option *o = getSecure(name);
    return o->getLong();
}

bool OptionsCont::getBool(string name) const {
    Option *o = getSecure(name);
    return o->getBool();
}

bool OptionsCont::set(string name, string value, bool isDefault) {
    Option *o = getSecure(name);
    if(!o->set(value, isDefault)) {
      reportDoubleSetting(name);
      return false;
    }
    return true;
}

bool OptionsCont::set(string name, bool value, bool isDefault) {
    Option *o = getSecure(name);
    if(!o->isBool()) {
      cout << "The option '" << name << "' is not a boolean attribute and so needs a value." << endl;
      throw InvalidArgument("The option '" + name + "' is not a boolean attribute and so needs a value.");
    }
    if(!o->set(value, isDefault)) {
      reportDoubleSetting(name);
      return false;
    }
    return true;
}

vector<string> OptionsCont::getSynonymes(string name) const {
    Option *o = getSecure(name);
    vector<string> v(0);
    for(KnownContType::const_iterator i=_values.begin(); i!=_values.end(); i++) {
      if((*i).second==o&&name!=(*i).first)
        v.push_back((*i).first);
    }
    return v;
}

ostream& operator<<( ostream& os, const OptionsCont& oc) {
    vector<string> done;
    os << "Options set:" << endl;
    for(OptionsCont::KnownContType::const_iterator i=oc._values.begin(); i!=oc._values.end(); i++) {
      vector<string>::iterator j = find(done.begin(), done.end(), (*i).first);
      if(j==done.end()) {
        vector<string> synonymes = oc.getSynonymes((*i).first);
        os << (*i).first << " (";
        for(j=synonymes.begin(); j!=synonymes.end(); j++) {
          if(j!=synonymes.begin())
            os << ", ";
          os << (*j);
        }
        os << ")";
        if((*i).second->isSet()) {
          os << ": " << (*i).second->getValue() << endl;
        } else {
          os << ": <INVALID>" << endl;
        }
        done.push_back((*i).first);
        copy(synonymes.begin(), synonymes.end(), back_inserter(done));
      }
    }
    return os;
}

void OptionsCont::reportDoubleSetting(string arg) const {
    vector<string> synonymes = getSynonymes(arg);
    cout << "A value for the option '" << arg << "' was already set." << endl;
    cout << "  Possible synonymes: ";
    for(vector<string>::iterator i=synonymes.begin(); i!=synonymes.end(); ) {
      cout << (*i);
      i++;
      if(i!=synonymes.end())
        cout << ", ";
    }
    cout << endl;
}

string OptionsCont::convertChar(char abbr) const {
    char buf[2];
    buf[0] = abbr;
    buf[1] = 0;
    string s(buf);
    return s;
}

bool OptionsCont::isBool(string name) const {
    Option *o = getSecure(name);
    return o->isBool();
}

string OptionsCont::getPath() const {
    if(_path.length()==0)
        throw InvalidArgument("No enviroment variable given.");
  char *tmp = getenv(_path.c_str());
  if(tmp==0) return "";
  return string(tmp);
}

void OptionsCont::resetDefaults() {
    for(ItemAddressContType::iterator i=_addresses.begin(); i!=_addresses.end(); i++) {
      (*i)->_default = true;
    }
}

string OptionsCont::getSystemPath(string ext) const {
  return string(getPath() + string("/") + ext);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "OptionsCont.icc"
//#endif

// Local Variables:
// mode:C++
// 

