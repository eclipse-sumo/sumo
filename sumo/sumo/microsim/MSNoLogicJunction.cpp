/***************************************************************************
                          MSNoLogicJunction.cpp  
                             -------------------
    begin                : Thu, 06 Jun 2002
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
// Revision 1.3  2002/08/06 15:50:16  roessel
// SetFirstVehiclesRequests implemented.
//
// Revision 1.2  2002/06/18 10:59:53  croessel
// Removed some ^M.
//
// Revision 1.1  2002/06/07 14:45:17  dkrajzew
// Added MSNoLogicJunction ,,pro forma,,. The code may compile but the
// class has no real functionality... 
//
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSNoLogicJunction.h"
#include "MSLane.h"
//#include "MSJunctionLogic.h"
#include <algorithm>
#include <cassert>
#include <cmath>
 
using namespace std;

//-------------------------------------------------------------------------//

MSNoLogicJunction::MSNoLogicJunction( string id, 
				      InLaneCont* in) :
    MSJunction( id ),
    myInLanes( in )
{
}


bool
MSNoLogicJunction::clearRequests()
{
    return true;
}

//-------------------------------------------------------------------------//

MSNoLogicJunction::~MSNoLogicJunction()
{
}

//-------------------------------------------------------------------------//

bool
MSNoLogicJunction::setFirstVehiclesRequests()
{
    for ( InLaneCont::iterator it = myInLanes->begin();
          it != myInLanes->end(); ++it ) {
        ( *it )->setRequest();
    }
    return true;
}

//-------------------------------------------------------------------------//

bool
MSNoLogicJunction::moveFirstVehicles()
{
    moveVehicles();
    return true;
}

//-------------------------------------------------------------------------//

bool
MSNoLogicJunction::vehicles2targetLane()
{
    for ( InLaneCont::iterator it = myInLanes->begin();
          it != myInLanes->end(); ++it ) {
        ( *it )->integrateNewVehicle();
    }
    return true;    
}

//-------------------------------------------------------------------------//

void
MSNoLogicJunction::moveVehicles()
{
    for ( InLaneCont::iterator inLaneIt = myInLanes->begin();
          inLaneIt != myInLanes->end(); ++inLaneIt ) {

        if( ! ( *inLaneIt )->empty() ) {
            
            ( *inLaneIt )->moveFirst( true );
        }
    }
}

//-------------------------------------------------------------------------//

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSNoLogicJunction.icc"
//#endif

// Local Variables:
// mode:C++
// End:




