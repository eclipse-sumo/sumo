//---------------------------------------------------------------------------//
//                        Command_SaveTLSSwitches.cpp -
//  Writes the switch times of a tls into a file
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 06 Jul 2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.1  2006/07/06 12:22:06  dkrajzew
// tls switches added
//
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

#include "Command_SaveTLSSwitches.h"
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <microsim/MSLink.h>
#include <microsim/MSLane.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>

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
Command_SaveTLSSwitches::Command_SaveTLSSwitches(
            const MSTLLogicControl::TLSLogicVariants &logics,
            const std::string &file)
    : myLogics(logics)
{
    MSNet::getInstance()->getEndOfTimestepEvents().addEvent(this,
        0, MSEventControl::ADAPT_AFTER_EXECUTION);
    myFile.open(file.c_str());
    if(!myFile.good()) {
        MsgHandler::getErrorInstance()->inform("The file '" + file + "' to save the tl-states into could not be opened.");
        throw ProcessError();
    }
    myFile << "<tls-switches>" << endl;
}


Command_SaveTLSSwitches::~Command_SaveTLSSwitches()
{
    myFile << "</tls-switches>" << endl;
}


SUMOTime
Command_SaveTLSSwitches::execute(SUMOTime currentTime)
{
    MSTrafficLightLogic *light = myLogics.defaultTL;
    const MSTrafficLightLogic::LinkVectorVector &links = light->getLinks();
    const std::bitset<64> &allowedLinks = light->allowed();
    for(size_t i=0; i<links.size(); i++) {
        if(!allowedLinks.test(i)) {
            const MSTrafficLightLogic::LinkVector &currLinks = links[i];
            const MSTrafficLightLogic::LaneVector &currLanes = light->getLanesAt(i);
            for(int j=0; j<currLinks.size(); j++) {
                if(myPreviousLinkStates.find(currLinks[j])==myPreviousLinkStates.end()) {
                    continue;
                } else {
                    MSLink *link = currLinks[j];
                    SUMOTime lastOn = myPreviousLinkStates[link].first;
                    bool saved = myPreviousLinkStates[link].second;
                    if(!saved) {
                        myFile << "   <switch tls=\"" << light->getID()
                            << "\" subid=\"" << light->getSubID()
                            << "\" fromLane=\"" << currLanes[j]->getID()
                            << "\" toLane=\"" << link->getLane()->getID()
                            << "\" begin=\"" << lastOn
                            << "\" end=\"" << currentTime
                            << "\" duration=\"" << (currentTime-lastOn)
                            << "\"/>" << endl;
                        myPreviousLinkStates[link] = make_pair<SUMOTime, bool>(lastOn, true);
                    }
                }
            }
        } else {
            const MSTrafficLightLogic::LinkVector &currLinks = links[i];
            for(MSTrafficLightLogic::LinkVector::const_iterator j=currLinks.begin(); j!=currLinks.end(); j++) {
                if(myPreviousLinkStates.find(*j)!=myPreviousLinkStates.end()) {
                    if(!myPreviousLinkStates[*j].second) {
                        continue;
                    }
                }
                myPreviousLinkStates[*j] = make_pair<SUMOTime, bool>(currentTime, false);
            }
        }
    }
    return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
