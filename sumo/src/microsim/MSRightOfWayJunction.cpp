/***************************************************************************
                          MSRightOfWayJunction.cpp  -  Usual right-of-way
                          junction.
                             -------------------
    begin                : Wed, 12 Dez 2001
    copyright            : (C) 2001 by Christian Roessel
    email                : roessel@zpr.uni-koeln.de
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
// Revision 1.9  2003/12/05 14:59:33  dkrajzew
// removed some unused lines
//
// Revision 1.8  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.7  2003/10/31 08:03:38  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
// Revision 1.6  2003/10/15 11:41:43  dkrajzew
// false usage of rand() patched
//
// Revision 1.5  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout;
//  this shall ease the search for further couts which must be redirected to
//  the messaaging subsystem
//
// Revision 1.4  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok);
//  detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.3  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:42:29  dkrajzew
// complete deletion within destructors implemented; clear-operator added
//  for container; global file include; junction extended by position
//  information (should be revalidated later)
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.3  2002/04/18 10:53:20  croessel
// In findCompetitor we now ignore lanes, that don't have a vehicle that
// is able to leave the lane.
//
// Revision 1.2  2002/04/11 15:25:56  croessel
// Changed float to double.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.3  2002/03/06 10:56:36  croessel
// Bugfix: myRespond will have always the correct size before being passed
//  to myLogic.
//
// Revision 2.2  2002/02/27 13:47:57  croessel
// Additional assert's because of parameter-passing-problems.
//
// Revision 2.1  2002/02/21 18:49:45  croessel
// Deadlock-killer implemented.
//
// Revision 2.0  2002/02/14 14:43:19  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/02/01 15:48:26  croessel
// Changed condition in moveFirstVehicles() again.
//
// Revision 1.4  2002/02/01 14:14:33  croessel
// Changed condition in moveFirstVehicles(). Now vehicles with a
// BrakeRequest only will also be moved.
//
// Revision 1.3  2002/02/01 11:52:28  croessel
// Removed function-adaptor findCompetitor from inside the class to the
// outside to please MSVC++.
//
// Revision 1.2  2002/02/01 11:40:34  croessel
// Changed return-type of some void methods used in for_each-loops to
// bool in order to please MSVC++.
//
// Revision 1.1  2001/12/13 15:54:49  croessel
// Initial commit. Has been MSJunction.cpp before.
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSRightOfWayJunction.h"
#include "MSLane.h"
#include "MSJunctionLogic.h"
#include <algorithm>
#include <cassert>
#include <cmath>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSRightOfWayJunction::MSRightOfWayJunction( string id,
                                            double x, double y,
                                            LaneCont incoming,
                                            LaneCont internal,
                                            MSJunctionLogic* logic)
    : MSLogicJunction( id, x, y, incoming, internal ),
    myLogic( logic )
{
}


bool
MSRightOfWayJunction::clearRequests()
{
    myRequest.reset();
    myInnerState.reset();
    return true;
}

//-------------------------------------------------------------------------//

MSRightOfWayJunction::~MSRightOfWayJunction()
{
}

//-------------------------------------------------------------------------//

bool
MSRightOfWayJunction::setAllowed()
{
#ifdef ABS_DEBUG
	if(MSNet::globaltime>MSNet::searchedtime&&myID==MSNet::searchedJunction) {
		DEBUG_OUT << "Request: " << myRequest << endl;
		DEBUG_OUT << "InnerSt: " << myInnerState<< endl;
	}
#endif
    // Get myRespond from logic and check for deadlocks.
    myLogic->respond( myRequest, myInnerState, myRespond );
    deadlockKiller();
#ifdef ABS_DEBUG
	if(MSNet::globaltime>MSNet::searchedtime&&myID==MSNet::searchedJunction) {
		DEBUG_OUT << "Respond: " << myRespond << endl;
	}
#endif
    return true;
}

//-------------------------------------------------------------------------//


void
MSRightOfWayJunction::deadlockKiller()
{
    if ( myRequest.none() ) {
        return;
    }

    // let's assume temporary, that deadlocks only occure on right-before-left
    //  junctions
    if ( myRespond.none() && myInnerState.none() ) {
#ifdef ABS_DEBUG
	if(MSNet::globaltime>MSNet::searchedtime&&myID==MSNet::searchedJunction) {
		DEBUG_OUT << "Killing deadlock" << endl;
	}
#endif

        // Handle deadlock: Create randomly a deadlock-free request out of
        // myRequest, i.e. a "single bit" request. Then again, send it
        // through myLogic (this is neccessary because we don't have a
        // mapping between requests and lanes.) !!! (we do now!!)
        vector< unsigned > trueRequests;
        trueRequests.reserve( myRespond.size() );
        for ( unsigned i = 0; i < myRequest.size(); ++i ) {

            if ( myRequest.test(i) ) {

                trueRequests.push_back( i );
                assert( trueRequests.size() <= myRespond.size() );
            }
        }
        // Choose randomly an index out of [0,trueRequests.size()];
        // !!! random choosing may choose one of less priorised lanes
        unsigned noLockIndex = static_cast< unsigned > ( floor (
           static_cast< double >( rand() ) /
           (static_cast< double >( RAND_MAX ) + 1.0) *
           static_cast< double >( trueRequests.size() ) ) );

        // Create deadlock-free request.
        std::bitset<64> noLockRequest(false);
        assert(trueRequests.size()>noLockIndex);
        noLockRequest.set( trueRequests[ noLockIndex ] );
        // Calculate respond with deadlock-free request.
        myLogic->respond( noLockRequest, myInnerState,  myRespond );
    }
    return;
}

//-------------------------------------------------------------------------//





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSRightOfWayJunction.icc"
//#endif

// Local Variables:
// mode:C++
// End:
