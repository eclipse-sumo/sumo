/****************************************************************************/
/// @file    NBDistrictCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// A container for all of the nets districts
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
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NBDistrict.h"
#include "NBDistrictCont.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NBDistrictCont::NBDistrictCont()
{}


NBDistrictCont::~NBDistrictCont()
{
    clear();
}


bool
NBDistrictCont::insert(NBDistrict *district)
{
    DistrictCont::iterator i = _districts.find(district->getID());
    if (i!=_districts.end()) return false;
    _districts.insert(DistrictCont::value_type(district->getID(), district));
    return true;
}


NBDistrict *
NBDistrictCont::retrieve(const string &id)
{
    DistrictCont::iterator i = _districts.find(id);
    if (i==_districts.end()) return 0;
    return (*i).second;
}


void
NBDistrictCont::writeXML(ostream &into)
{
    for (DistrictCont::iterator i=_districts.begin(); i!=_districts.end(); i++) {
        (*i).second->writeXML(into);
    }
    into << endl;
}


int NBDistrictCont::size()
{
    return _districts.size();
}


void
NBDistrictCont::clear()
{
    for (DistrictCont::iterator i=_districts.begin(); i!=_districts.end(); i++) {
        delete((*i).second);
    }
    _districts.clear();
}


void
NBDistrictCont::report()
{
    WRITE_MESSAGE("   " + toString<int>(size()) + " districts loaded.");
}


bool
NBDistrictCont::addSource(const std::string &dist, NBEdge *source,
                          SUMOReal weight)
{
    NBDistrict *o = retrieve(dist);
    if (o==0) {
        return false;
    }
    return o->addSource(source, weight);
}


bool
NBDistrictCont::addSink(const std::string &dist, NBEdge *destination,
                        SUMOReal weight)
{
    NBDistrict *o = retrieve(dist);
    if (o==0) {
        return false;
    }
    return o->addSink(destination, weight);
}


void
NBDistrictCont::removeFromSinksAndSources(NBEdge *e)
{
    for (DistrictCont::iterator i=_districts.begin(); i!=_districts.end(); i++) {
        (*i).second->removeFromSinksAndSources(e);
    }
}



/****************************************************************************/

