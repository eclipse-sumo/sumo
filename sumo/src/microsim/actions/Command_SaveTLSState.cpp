//---------------------------------------------------------------------------//
//                        Command_SaveTLSState.cpp -
//  Writes the state of the tls to a file
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
// Revision 1.3  2004/11/23 10:18:24  dkrajzew
// new detectors usage applied
//
// Revision 1.2  2004/02/16 14:02:57  dkrajzew
// e2-link-dependent detectors added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "Action.h"
#include "Command_SaveTLSState.h"
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
Command_SaveTLSState::Command_SaveTLSState(MSTrafficLightLogic *logic,
                                           const std::string &file)
    : myLogic(logic)
{
    myExecTime = MSEventControl::getBeginOfTimestepEvents()->addEvent(this,
        0, MSEventControl::ADAPT_AFTER_EXECUTION);
    myFile.open(file.c_str());
    if(!myFile.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The file '") + file
            + string("'to save the tl-states into could not be opened."));
        throw ProcessError();
    }
    myFile << "<sumo-output>" << endl;
}


Command_SaveTLSState::~Command_SaveTLSState()
{
    myFile << "</sumo-output>" << endl;
}


MSNet::Time
Command_SaveTLSState::execute()
{
    myFile << "   <tlsstate time=\"" << myExecTime++
        << "\" state=\"" << myLogic->buildStateList() << "\"/>" << endl;
    return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
