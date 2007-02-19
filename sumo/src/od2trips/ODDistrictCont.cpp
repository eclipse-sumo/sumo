/****************************************************************************/
/// @file    ODDistrictCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The container for districts
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
#include "ODDistrict.h"
#include "ODDistrictCont.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/helpers/NamedObjectCont.h>

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
ODDistrictCont::ODDistrictCont()
        : myHadColorized(false)
{}


ODDistrictCont::~ODDistrictCont()
{}


std::string
ODDistrictCont::getRandomSourceFromDistrict(const std::string &name) const
{
    ODDistrict *district = get(name);
    if (district==0) {
        MsgHandler::getErrorInstance()->inform("There is no district '" + name + "'.");
        throw ProcessError();
    }
    return district->getRandomSource();
}


std::string
ODDistrictCont::getRandomSinkFromDistrict(const std::string &name) const
{
    ODDistrict *district = get(name);
    if (district==0) {
        MsgHandler::getErrorInstance()->inform("There is no district '" + name + "'.");
        throw ProcessError();
    }
    return district->getRandomSink();
}


void
ODDistrictCont::colorize()
{
    const std::vector<ODDistrict*> &v = getTempVector();
    for (size_t i=0; i!=v.size(); i++) {
        v[i]->setColor((SUMOReal) i / (SUMOReal) v.size());
    }
}


SUMOReal
ODDistrictCont::getDistrictColor(const std::string &name) const
{
    ODDistrict *district = get(name);
    if (district==0) {
        MsgHandler::getErrorInstance()->inform("There is no district '" + name + "'.");
        throw ProcessError();
    }
    return district->getColor();
}



/****************************************************************************/

