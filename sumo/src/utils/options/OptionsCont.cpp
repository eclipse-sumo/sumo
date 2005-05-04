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
// Revision 1.8  2005/05/04 09:28:01  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2004/12/16 12:18:39  dkrajzew
// debugging
//
// Revision 1.6  2004/11/23 10:36:02  dkrajzew
// debugging
//
// Revision 1.5  2004/07/02 09:41:39  dkrajzew
// debugging the repeated setting of a value
//
// Revision 1.4  2003/07/30 12:54:00  dkrajzew
// unneeded and deprecated methods and variables removed
//
// Revision 1.3  2003/06/24 08:13:51  dkrajzew
// added the possibiliy to clear the container
//
// Revision 1.2  2003/02/07 10:51:59  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:58:18  dkrajzew
// initial release for utilities that handle program options
//
// Revision 1.7  2002/07/11 05:52:11  dkrajzew
// Option_FileName introduced and used to allow the usage of relative path
//  names within the configuration files
//
// Revision 1.6  2002/06/11 15:58:25  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/05/14 04:45:49  dkrajzew
// Bresenham added; some minor changes; windows eol removed
//
// Revision 1.4  2002/04/26 10:08:38  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/04/17 11:21:52  dkrajzew
// Windows-carriage returns removed
//
// Revision 1.2  2002/04/16 12:28:26  dkrajzew
// Usage of SUMO_DATA removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:01  dkrajzew
// new version-free project name
//
// Revision 1.6  2002/04/09 12:20:37  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.5  2002/03/22 10:59:37  dkrajzew
// Memory leak tracing added; ostrstreams replaces by ostringstreams
//
// Revision 1.4  2002/03/20 08:39:17  dkrajzew
// comments updated
//
// Revision 1.3  2002/03/20 08:38:14  dkrajzew
// isDefault - method added
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <sstream>


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC       // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
OptionsCont::OptionsCont()
    : _addresses(), _values()
{
}

OptionsCont::~OptionsCont()
{
    clear();
}


void
OptionsCont::doRegister(const string &name1, Option *v)
{
    assert(v!=0);
    ItemAddressContType::iterator i = find(_addresses.begin(), _addresses.end(), v);
    if(i==_addresses.end()) {
        _addresses.push_back(v);
    }
    _values.insert(KnownContType::value_type(name1, v));
}


void
OptionsCont::doRegister(const string &name1, char abbr, Option *v)
{
    doRegister(name1, v);
    doRegister(convertChar(abbr), v);
}


void
OptionsCont::addSynonyme(const string &name1, const string &name2)
{
    KnownContType::iterator i1 = _values.find(name1);
    KnownContType::iterator i2 = _values.find(name2);
    if(i1==_values.end()&&i2==_values.end()) {
        throw InvalidArgument(
            "Neither the option '" + name1 + "' nor the option '"
            + name2 + "' is known yet");
    }
    if(i1!=_values.end()&&i2!=_values.end()) {
        if((*i1).second==(*i2).second) {
            return;
        }
        throw InvalidArgument(
            "Both options '" + name1 + "' and '" + name2
            + "' do exist and differ.");
    }
    if(i1==_values.end()&&i2!=_values.end()) {
        doRegister(name1, (*i2).second);
    }
    if(i1!=_values.end()&&i2==_values.end()) {
        doRegister(name2, (*i1).second);
    }
}


bool
OptionsCont::exists(const string &name) const
{
    KnownContType::const_iterator i = _values.find(name);
    return i!=_values.end();
}


bool
OptionsCont::isSet(const string &name) const
{
    KnownContType::const_iterator i = _values.find(name);
    if(i==_values.end()) {
        return false;
    }
    return (*i).second->isSet();
}


bool
OptionsCont::isDefault(const std::string &name) const
{
    KnownContType::const_iterator i = _values.find(name);
    if(i==_values.end()) {
        return false;
    }
    return (*i).second->isDefault();
}


Option *
OptionsCont::getSecure(const string &name) const
{
    KnownContType::const_iterator i = _values.find(name);
    if(i==_values.end()) {
        throw InvalidArgument("No option with the name '" + name
            + "' exists.");
    }
    return (*i).second;
}


string
OptionsCont::getString(const string &name) const
{
    Option *o = getSecure(name);
    return o->getString();
}


float
OptionsCont::getFloat(const string &name) const
{
    Option *o = getSecure(name);
    return o->getFloat();
}


int
OptionsCont::getInt(const string &name) const
{
    Option *o = getSecure(name);
    return o->getInt();
}


long
OptionsCont::getLong(const string &name) const
{
    Option *o = getSecure(name);
    return o->getLong();
}


bool
OptionsCont::getBool(const string &name) const
{
    Option *o = getSecure(name);
    return o->getBool();
}

const IntVector &
OptionsCont::getIntVector(const std::string &name) const
{
    Option *o = getSecure(name);
    return o->getIntVector();
}


bool
OptionsCont::set(const string &name, const string &value, bool isDefault)
{
    Option *o = getSecure(name);
    if(!o->set(value, isDefault)) {
        reportDoubleSetting(name);
        return false;
    }
    return true;
}

bool
OptionsCont::set(const string &name, bool value, bool isDefault)
{
    Option *o = getSecure(name);
    if(!o->isBool()) {
        throw InvalidArgument("The option '" + name
            + "' is not a boolean attribute and requires an argument.");
    }
    if(!o->set(value, isDefault)) {
        reportDoubleSetting(name);
        return false;
    }
    return true;
}


vector<string>
OptionsCont::getSynonymes(const string &name) const
{
    Option *o = getSecure(name);
    vector<string> v(0);
    for(KnownContType::const_iterator i=_values.begin(); i!=_values.end(); i++) {
        if((*i).second==o&&name!=(*i).first) {
            v.push_back((*i).first);
        }
    }
    return v;
}


ostream&
operator<<( ostream& os, const OptionsCont& oc)
{
    vector<string> done;
    os << "Options set:" << endl;
    for(OptionsCont::KnownContType::const_iterator i=oc._values.begin();
            i!=oc._values.end(); i++) {
        vector<string>::iterator j = find(done.begin(), done.end(), (*i).first);
        if(j==done.end()) {
            vector<string> synonymes = oc.getSynonymes((*i).first);
            if(synonymes.size()!=0) {
                os << (*i).first << " (";
                for(j=synonymes.begin(); j!=synonymes.end(); j++) {
                    if(j!=synonymes.begin()) {
                        os << ", ";
                    }
                    os << (*j);
                }
                os << ")";
            } else {
                os << (*i).first;
            }
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


bool
OptionsCont::isFileName(const std::string &name) const
{
    Option *o = getSecure(name);
    return o->isFileName();
}


bool
OptionsCont::isUsableFileList(const std::string &name) const
{
    Option *o = getSecure(name);
    // check whether the option is set
    //  return false i not
    if(!o->isSet()) {
        return false;
    }
    // check whether the list of files is valid
    if(!FileHelpers::checkFileList(o->getString())) {
        throw InvalidArgument("The option '" + name
            + "' should not contain delimiters only.");
    }
    return true;
}


void
OptionsCont::reportDoubleSetting(const string &arg) const
{
    vector<string> synonymes = getSynonymes(arg);
    MsgHandler::getErrorInstance()->inform(
        "A value for the option '" + arg + "' was already set.");
    ostringstream s;
    s << "  Possible synonymes: ";
    for(vector<string>::iterator i=synonymes.begin(); i!=synonymes.end(); ) {
        s << (*i);
        i++;
        if(i!=synonymes.end()) {
            s << ", ";
        }
    }
    MsgHandler::getErrorInstance()->inform(string(s.str()));
}


string
OptionsCont::convertChar(char abbr) const
{
    char buf[2];
    buf[0] = abbr;
    buf[1] = 0;
    string s(buf);
    return s;
}


bool
OptionsCont::isBool(const string &name) const
{
    Option *o = getSecure(name);
    return o->isBool();
}


void
OptionsCont::resetWritable()
{
    for(ItemAddressContType::iterator i=_addresses.begin(); i!=_addresses.end(); i++) {
        (*i)->_writeable = true;
    }
}


bool
OptionsCont::isWriteable(const std::string &name)
{
    Option *o = getSecure(name);
    return o->isWriteable();
}


void
OptionsCont::clear()
{
    for(ItemAddressContType::iterator i=_addresses.begin();
            i!=_addresses.end(); i++) {
        delete (*i);
    }
    _addresses.clear();
    _values.clear();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
//

