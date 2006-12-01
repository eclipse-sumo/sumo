/***************************************************************************
                          MSPhoneCell.cpp  -
    A cell of a cellular network (GSM)
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
// Revision 1.8  2006/12/01 09:14:41  dkrajzew
// debugging cell phones
//
// Revision 1.7  2006/11/28 12:15:39  dkrajzew
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
#include "MSPhoneCell.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSPhoneCell::MSPhoneCell( int id )
    : Cell_Id( id ), Calls_In( 0 ), Calls_Out( 0 ),
    Dyn_Calls_In( 0 ), Dyn_Calls_Out( 0 ), Sum_Calls( 0 ),
    Intervall( 0 ), last_time( 0 )
{
}


MSPhoneCell::~MSPhoneCell()
{
}


int
MSPhoneCell::getIntervall()
{
    return Intervall;
}


void
MSPhoneCell::addCall(const std::string &id, CallType t)
{
    Call c;
    c.ct=t;
    //c.id=id;
    /* @debug
    if ( getCall( id ) ) {
        int i = 0;
    }
    */
    _Calls[id] = c;
    switch (t) {
    case STATICIN:
	    Calls_In++;
		break;
    case STATICOUT:
	    Calls_Out++;
        break;
    case DYNIN:
	    Dyn_Calls_In++;
        break;
    case DYNOUT:
	    Dyn_Calls_Out++;
        break;
    }
    ++Sum_Calls;
}


void
MSPhoneCell::remCall(const std::string &id )
{
    std::map<std::string, Call>::iterator icalls = _Calls.find(id);
    assert(icalls!=_Calls.end());
    /*
    if(icalls==_Calls.end()) {
        return;
    }
    */
    switch ( (*icalls).second.ct ) {
    case STATICIN:
        Calls_In--;
        break;
    case STATICOUT:
        Calls_Out--;
        break;
    case DYNIN:
        Dyn_Calls_In--;
        break;
    case DYNOUT:
        Dyn_Calls_Out--;
        break;
    }
    if(Sum_Calls>0) {
        --Sum_Calls;
    }
    _Calls.erase(icalls);
}


void
MSPhoneCell::setStatParams( int interval, int statcallcount)
{
    vexpectStatCount.push_back( std::make_pair( interval, statcallcount) );
}


void
MSPhoneCell::setDynParams( int interval, int count, float duration, float deviation )
{
    DynParam p;
    p.count = count;
    p.deviation = deviation;
    p.duration = duration;
    vexpectDuration.push_back( std::make_pair( interval, p ) );
}


void
MSPhoneCell::setnextexpectData(SUMOTime t)
{
    itCount = vexpectStatCount.begin();
    if( itCount !=vexpectStatCount.end() && ( t % 300 ) == 0 ){
        Intervall = itCount->first;
        statcallcount = itCount->second/*.first*/;
        /*verteile ein/aus erstmal 50/50*/
        Calls_In = statcallcount / 2;
        Calls_Out = statcallcount / 2;
        vexpectStatCount.erase(itCount);
    }
    itDuration = vexpectDuration.begin();
    if( itDuration != vexpectDuration.end() && ( t % 3600 ) == 0 ){
        dyncallcount = itDuration->second.count;
        dynduration = itDuration->second.duration;
        dyndeviation = itDuration->second.deviation;
        vexpectDuration.erase( itDuration );
    }
}


bool
MSPhoneCell::hasCall(const std::string &id )
{
    return _Calls.find(id)!=_Calls.end();
}


void
MSPhoneCell::writeOutput( SUMOTime t )
{
    Intervall = t - last_time;
    MSCORN::saveTOSS2_CellStateData( t, Cell_Id, Calls_In, Calls_Out, Dyn_Calls_In, Dyn_Calls_Out, Sum_Calls, Intervall );
    last_time = t;
}


void
MSPhoneCell::writeSQLOutput( SUMOTime t )
{
    Intervall = t - last_time;
    MSCORN::saveTOSS2SQL_CellStateData( t, Cell_Id, Calls_In, Calls_Out, Dyn_Calls_In, Dyn_Calls_Out, Sum_Calls, Intervall );
    last_time = t;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
