/****************************************************************************/
/// @file    MSPhoneLA.cpp
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id$
///
// A location area of a cellular network (GSM)
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

#include <cassert>
#include "MSPhoneLA.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSPhoneLA::MSPhoneLA(int pos_id, int d)
        : last_time(0), position_id(pos_id), dir(d),
        sum_changes(0), quality_id(0), intervall(0)
{}


MSPhoneLA::~MSPhoneLA()
{}


void
MSPhoneLA::addCall()
{
    assert(true);
    sum_changes++;
}


void
MSPhoneLA::addCall(const std::string &id)
{
    _Calls[id] = 1;
    ++sum_changes;
}


void
MSPhoneLA::remCall(const std::string &id)
{
    std::map<std::string, int>::iterator icalls = _Calls.find(id);
    assert(icalls!=_Calls.end());
    _Calls.erase(icalls);
}


bool
MSPhoneLA::hasCall(const std::string &id)
{
    return _Calls.find(id)!=_Calls.end();
}


bool
MSPhoneLA::operator ==(MSPhoneLA * other)
{
    if (position_id==other->position_id && dir==other->dir) {
        return true;
    } else {
        return false;
    }
}


bool
MSPhoneLA::operator !=(MSPhoneLA * other)
{
    if (position_id!=other->position_id || dir!=other->dir) {
        return true;
    } else {
        return false;
    }
}


void
MSPhoneLA::writeOutput(SUMOTime t)
{
    intervall = t - last_time;
    MSCORN::saveTOSS2_LA_ChangesData(t, position_id, dir, sum_changes, quality_id, intervall);
    last_time = t;
    sum_changes = 0;
}


//#include <microsim/MSNet.h>

void
MSPhoneLA::writeSQLOutput(SUMOTime t)
{
    intervall = t - last_time;
    MSCORN::saveTOSS2SQL_LA_ChangesData(t, position_id, dir, sum_changes, quality_id, intervall);
    last_time = t;
    sum_changes = 0;
}



/****************************************************************************/

