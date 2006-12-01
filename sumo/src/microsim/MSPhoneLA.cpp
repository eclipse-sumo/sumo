/***************************************************************************
                          MSPhoneLA.cpp  -
    A location area of a cellular network (GSM)
                             -------------------
    begin                : 2006
    copyright            : (C) 2006 by DLR http://www.dlr.de
    author               : Eric Nicolay
    email                : Eric.Nicolay@dlr.de
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
// Revision 1.7  2006/12/01 09:14:41  dkrajzew
// debugging cell phones
//
// Revision 1.6  2006/11/28 12:15:39  dkrajzew
// documented TOL-classes and made them faster
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <cassert>
#include "MSPhoneLA.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSPhoneLA::MSPhoneLA( int pos_id, int d )
    : last_time( 0 ), position_id( pos_id ), dir( d ),
    sum_changes( 0 ), quality_id( 0 ), intervall( 0 )
{
}


MSPhoneLA::~MSPhoneLA()
{
}


void
MSPhoneLA::addCall( )
{
    sum_changes++;
}


void
MSPhoneLA::addCall(const std::string &id)
{
    _Calls[id] = 1;
    ++sum_changes;
}


void
MSPhoneLA::remCall(const std::string &id )
{
    std::map<std::string, int>::iterator icalls = _Calls.find(id);
    assert(icalls!=_Calls.end());
    _Calls.erase(icalls);
}


bool
MSPhoneLA::hasCall(const std::string &id )
{
    return _Calls.find(id)!=_Calls.end();
}


bool
MSPhoneLA::operator ==(MSPhoneLA * other)
{
    if(position_id==other->position_id && dir==other->dir) {
        return true;
    } else {
        return false;
    }
}


bool
MSPhoneLA::operator !=(MSPhoneLA * other)
{
    if(position_id!=other->position_id || dir!=other->dir) {
        return true;
    } else {
        return false;
    }
}


void
MSPhoneLA::writeOutput( SUMOTime t )
{
    intervall = t - last_time;
    MSCORN::saveTOSS2_LA_ChangesData( t, position_id, dir, sum_changes, quality_id, intervall );
    last_time = t;
    sum_changes = 0;
}


#include <microsim/MSNet.h>

void
MSPhoneLA::writeSQLOutput( SUMOTime t )
{
    intervall = t - last_time;
    MSCORN::saveTOSS2SQL_LA_ChangesData( t, position_id, dir, sum_changes, quality_id, intervall );
    last_time = t;
	sum_changes = 0;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

