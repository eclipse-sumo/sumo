/****************************************************************************/
/// @file    XMLSnippletStorage.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 27 Mar 2007
/// @version $Id: XMLSnippletStorage.cpp 3486 2007-02-19 13:38:00 +0100 (Mo, 19 Feb 2007) behrisch $
///
// Storage for XML-snipplets
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

#include <iostream>
#include "XMLSnippletStorage.h"
#include <cassert>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
XMLSnippletStorage::XMLSnippletStorage(XMLSnippletStorage *parent, 
                                       const std::string &name)
    : myParent(parent), myName(name)
{
}


XMLSnippletStorage::~XMLSnippletStorage()
{
    for(std::vector<XMLSnippletStorage*>::iterator i=myChildren.begin(); i!=myChildren.end(); ++i) {
        delete (*i);
    }
}


void 
XMLSnippletStorage::addAttribute(const std::string &name, const std::string &val)
{
    Attribute attr(name, val);
    myAttributes.push_back(attr);
}


void 
XMLSnippletStorage::addCharacters(const std::string &chars)
{
    myCharacters = chars;
}


XMLSnippletStorage *
XMLSnippletStorage::addChild(const std::string &name)
{
    XMLSnippletStorage *child = new XMLSnippletStorage(this, name);
    myChildren.push_back(child);
    return child;
}


XMLSnippletStorage *
XMLSnippletStorage::getParent()
{
    return myParent;
}


void 
XMLSnippletStorage::flush(std::ostream &strm, int level)
{
    size_t i;
    for(i=0; i<level; i++) {
        strm << "   ";
    }
    strm << "<" << myName;
    for(std::vector<Attribute>::iterator j=myAttributes.begin(); j!=myAttributes.end(); ++j) {
        strm << " " << (*j).first << "=\"" << (*j).second << "\"";
    }
    strm << ">" << endl;
    for(std::vector<XMLSnippletStorage*>::iterator k=myChildren.begin(); k!=myChildren.end(); ++k) {
        (*k)->flush(strm, level+1);
    }
    for(i=0; i<level; i++) {
        strm << "   ";
    }
    strm << "</" << myName << ">" << endl;
}


XMLSnippletStorage*
XMLSnippletStorage::duplicate(XMLSnippletStorage *parent)
{
    XMLSnippletStorage *ret = new XMLSnippletStorage(parent, myName);
    ret->addCharacters(myCharacters);
    for(std::vector<XMLSnippletStorage*>::iterator i=myChildren.begin(); i!=myChildren.end(); ++i) {
        ret->myChildren.push_back((*i)->duplicate(this));
    }
    return ret;
}


const std::string &
XMLSnippletStorage::getName() const
{
    return myName;
}



/****************************************************************************/

