//---------------------------------------------------------------------------//
//                        MSTrafficLightJunction.cpp -
//  A traffic light junction
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <bitset>
#include <map>
#include "MSNet.h"
#include "MSLane.h"
#include "MSLinkCont.h"
#include "MSLink.h"
#include "MSRightOfWayJunction.h"
#include "MSEventControl.h"
#include "MSJunctionLogic.h"
#include "MSTrafficLightLogic.h"
#include "MSTrafficLightJunction.h"

using namespace std;

MSTrafficLightJunction::MSTrafficLightJunction(const std::string &id,
                                               double x, double y,
                                               InLaneCont in,
                                               MSJunctionLogic* logic,
                                               MSTrafficLightLogic *tlLogic,
                                               size_t delay,
                                               size_t initStep,
                                               MSEventControl &ec)
    : MSRightOfWayJunction(id, x, y, in, logic),
    _tlLogic(tlLogic)
{
    ec.addEvent(new SwitchCommand(myInLanes, _tlLogic), delay,
        MSEventControl::ADAPT_AFTER_EXECUTION);
    //setLinkPriorities();
}


MSTrafficLightJunction::~MSTrafficLightJunction()
{
}


bool
MSTrafficLightJunction::setAllowed()
{
#ifdef ABS_DEBUG
	if(MSNet::globaltime>MSNet::searchedtime&&myID==MSNet::searchedJunction) {
		cout << "Step: " << _tlLogic->step() << endl;
		cout << "RequestPre:  " << myRequest << endl;
	}
#endif
    _tlLogic->applyPhase(myRequest);
#ifdef ABS_DEBUG
	if(MSNet::globaltime>MSNet::searchedtime&&myID==MSNet::searchedJunction) {
		cout << "RequestPost: " << myRequest << endl;
	}
#endif
    // Get myRespond from logic and check for deadlocks.
    myLogic->respond( myRequest, myRespond );
#ifdef ABS_DEBUG
	if(MSNet::globaltime>MSNet::searchedtime&&myID==MSNet::searchedJunction) {
		cout << "Respond:     " << myRespond << endl;
	}
#endif

    return true;
}



void
MSTrafficLightJunction::postloadInit()
{
    MSLogicJunction::postloadInit();
    _tlLogic->setLinkPriorities(myInLanes);
}


//-------------------------------------------------------------------------//

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSTrafficLightJunction.icc"
//#endif


// Local Variables:
// mode:C++
// End:

