/****************************************************************************/
/// @file    ODDistrict.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A district
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
#include <vector>
#include <string>
#include <utility>
#include <utils/common/UtilExceptions.h>
#include <utils/common/Named.h>
#include <utils/common/MsgHandler.h>
#include "ODDistrict.h"

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
ODDistrict::ODDistrict(const std::string &id)
        : Named(id)
{}

ODDistrict::~ODDistrict()
{}


void
ODDistrict::addSource(const std::string &id, SUMOReal weight)
{
    mySources.add(weight, id);
}


void
ODDistrict::addSink(const std::string &id, SUMOReal weight)
{
    mySinks.add(weight, id);
}


std::string
ODDistrict::getRandomSource() const
{
    if (mySources.getOverallProb()==0) {
        MsgHandler::getErrorInstance()->inform("There is no source for district '" + getID() + "'.");
        throw ProcessError();
    }
    return mySources.get();
}


std::string
ODDistrict::getRandomSink() const
{
    if (mySinks.getOverallProb()==0) {
        MsgHandler::getErrorInstance()->inform("There is no sink for district '" + getID() + "'.");
        throw ProcessError();
    }
    return mySinks.get();
}


void
ODDistrict::setColor(SUMOReal val)
{
    myColor = val;
}


SUMOReal
ODDistrict::getColor()  const
{
    return myColor;
}



/****************************************************************************/

