//---------------------------------------------------------------------------//
//                        Command_SaveTLCoupledDet.cpp -
//  Realises the output of a tls values on each switch
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 15 Feb 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.6  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 11:07:14  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:07:09  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/11/23 10:18:24  dkrajzew
// new detectors usage applied
//
// Revision 1.2  2004/02/16 14:02:57  dkrajzew
// e2-link-dependent detectors added
//
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

#include "Action.h"
#include "Command_SaveTLCoupledDet.h"
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
Command_SaveTLCoupledDet::Command_SaveTLCoupledDet(
            MSTrafficLightLogic * const tll,
            MSDetectorFileOutput *dtf, unsigned int begin,
            OutputDevice *device)
    : myLogic(tll), myDetector(dtf), myDevice(device),
    myStartTime(begin)
{
    tll->addSwitchAction(this);
}


Command_SaveTLCoupledDet::~Command_SaveTLCoupledDet()
{
}


bool
Command_SaveTLCoupledDet::execute()
{
    SUMOTime end =
        MSNet::getInstance()->getCurrentTimeStep();
    myDetector->writeXMLOutput( *myDevice, myStartTime, end );
    myStartTime = end;
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



