/****************************************************************************/
/// @file    Command_SaveTLSState.cpp
/// @author  Daniel Krajzewicz
/// @date    15 Feb 2004
/// @version $Id: $
///
// Writes the state of the tls to a file
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

#include "Command_SaveTLSState.h"
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
Command_SaveTLSState::Command_SaveTLSState(
    const MSTLLogicControl::TLSLogicVariants &logics,
    const std::string &file)
        : myLogics(logics)
{
    MSNet::getInstance()->getEndOfTimestepEvents().addEvent(this,
            0, MSEventControl::ADAPT_AFTER_EXECUTION);
    myFile.open(file.c_str());
    if (!myFile.good()) {
        MsgHandler::getErrorInstance()->inform("The file '" + file + "' to save the tl-states into could not be opened.");
        throw ProcessError();
    }
    myFile << "<tls-states>" << endl;
}


Command_SaveTLSState::~Command_SaveTLSState()
{
    myFile << "</tls-states>" << endl;
}


SUMOTime
Command_SaveTLSState::execute(SUMOTime currentTime)
{
    for (std::map<std::string, MSTrafficLightLogic*>::const_iterator i=myLogics.ltVariants.begin(); i!=myLogics.ltVariants.end(); ++i) {
        string subid = (*i).second->getSubID();
        myFile << "   <tlsstate time=\"" << currentTime
        << "\" id=\"" << (*i).second->getID()
        << "\" subid=\"" << subid << "\">"
        << (*i).second->buildStateList() << "</tlsstate>" << endl;
    }
    return 1;
}



/****************************************************************************/

