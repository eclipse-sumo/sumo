//---------------------------------------------------------------------------//
//                        Command_SaveTLCoupledLaneDet.cpp -
//  Realises the output of a lane's detector values if the lane has green light
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
// Revision 1.1  2004/02/16 14:03:46  dkrajzew
// e2-link-dependent detectors added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "Action.h"
#include "Command_SaveTLCoupledLaneDet.h"
#include <microsim/MSNet.h>
#include <microsim/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSDetectorFileOutput.h>
#include <microsim/MSLinkCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
Command_SaveTLCoupledLaneDet::Command_SaveTLCoupledLaneDet(
        MSTrafficLightLogic *tll, MSDetectorFileOutput *dtf,
        unsigned int begin, const std::string &file
        , MSLink *link)
    : Command_SaveTLCoupledDet(tll, dtf, begin, file),
    myLink(link), myLastState(MSLink::LINKSTATE_TL_RED)
{
}


Command_SaveTLCoupledLaneDet::~Command_SaveTLCoupledLaneDet()
{
}


bool
Command_SaveTLCoupledLaneDet::execute()
{
    if(myLink->getState()==myLastState) {
        return true;
    }
    if(myLink->getState()==MSLink::LINKSTATE_TL_RED) {
        unsigned int end =
            MSNet::getInstance()->getCurrentTimeStep();
        myFile << "<interval start=\"" << myStartTime << "\" stop=\"" << end
            << "\" " << myDetector->getXMLOutput( end-myStartTime )
            << " />" << endl;
        myStartTime = end;
    }
    myLastState = myLink->getState();
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
