//---------------------------------------------------------------------------//
//                        MSLaneSpeedTrigger.cpp -
//  Class that realises the setting of a lane's maximum speed triggered by
//      values read from a file
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
// Revision 1.5  2006/01/17 14:10:56  dkrajzew
// debugging
//
// Revision 1.4  2005/11/09 06:37:52  dkrajzew
// trigger reworked
//
// Revision 1.3  2005/10/07 11:37:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.2  2005/09/22 13:45:52  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.1  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/26 08:11:49  dksumo
// level3 warnings patched; debugging
//
// Revision 1.3.2.1  2005/04/15 09:48:19  dksumo
// using a single SUMOTime type for time values; level3 warnings removed
//
// Revision 1.3  2005/02/01 09:49:24  dksumo
// got rid of MSNet::Time
//
// Revision 1.2  2005/01/06 10:48:06  dksumo
// 0.8.2.1 patches
//
// Revision 1.1  2004/10/22 12:49:25  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.7  2004/07/02 09:56:40  dkrajzew
// debugging while implementing the vss visualisation
//
// Revision 1.6  2003/09/23 14:18:15  dkrajzew
// hierarchy refactored; user-friendly implementation
//
// Revision 1.5  2003/09/22 14:56:06  dkrajzew
// base debugging
//
// Revision 1.4  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.3  2003/06/18 11:12:51  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
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

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/helpers/Command.h>
#include <microsim/MSLane.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include "MSLaneSpeedTrigger.h"
#include <utils/common/XMLHelpers.h>
#include <utils/common/TplConvert.h>
#include <microsim/MSEventControl.h>

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
MSLaneSpeedTrigger::MSLaneSpeedTrigger(const std::string &id,
                                       MSNet &net,
                                       const std::vector<MSLane*> &destLanes,
                                       const std::string &aXMLFilename)
    : MSTrigger(id), SUMOSAXHandler("speed_limits", aXMLFilename),
    myDestLanes(destLanes), /*myHaveNext(false), */myAmOverriding(false)
/*    ,
    myNextOffset(0)*/
{
    myCurrentSpeed = destLanes[0]->maxSpeed();
    // read in the trigger description
    SAX2XMLReader* triggerParser = 0;
    try {
        triggerParser = XMLHelpers::getSAXReader(*this);
        triggerParser->parse(aXMLFilename.c_str());
    } catch (SAXException &e) {
        MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(e.getMessage()));
        throw ProcessError();
    } catch (XMLException &e) {
        MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(e.getMessage()));
        throw ProcessError();
    }
    // set it to the right value
        // assert there is at least one
    if(myLoadedSpeeds.size()==0) {
        myLoadedSpeeds.push_back(make_pair(100000, myCurrentSpeed));
    }
        // set the process to the begin
    myCurrentEntry = myLoadedSpeeds.begin();
        // pass previous time steps
    while((*myCurrentEntry).first<net.getCurrentTimeStep()&&myCurrentEntry!=myLoadedSpeeds.end()) {
        processCommand(true);
    }

    // add the processing to the event handler
    MSEventControl::getBeginOfTimestepEvents()->addEvent(
        new MyCommand(this), (*myCurrentEntry).first,
            MSEventControl::NO_CHANGE);
    delete triggerParser;
}


MSLaneSpeedTrigger::~MSLaneSpeedTrigger()
{
}


SUMOTime
MSLaneSpeedTrigger::processCommand(bool move2next)
{
    std::vector<MSLane*>::iterator i;
    for(i=myDestLanes.begin(); i!=myDestLanes.end(); ++i) {
        (*i)->myMaxSpeed = getCurrentSpeed();
    }
    if(move2next&&myCurrentEntry!=myLoadedSpeeds.end()) {
        ++myCurrentEntry;
    }
    if(myCurrentEntry!=myLoadedSpeeds.end()) {
        return ((*myCurrentEntry).first)-((*(myCurrentEntry-1)).first);
    } else {
        return 0;
    }
}


void
MSLaneSpeedTrigger::myStartElement(int element, const std::string &,
                                   const Attributes &attrs)
{
    // check whethe the correct tag is read
    if(element!=SUMO_TAG_STEP) {
        return;
    }
    // extract the values
    try {
        int next = getIntSecure(attrs, SUMO_ATTR_TIME, -1);
        SUMOReal speed = getFloatSecure(attrs, SUMO_ATTR_SPEED, -1.0);
        // check the values
        if(next<0) {
            MsgHandler::getErrorInstance()->inform("Wrong time in MSLaneSpeedTrigger in file '" + _file + "'.");
            return;
        }
        if(speed<0) {
            MsgHandler::getErrorInstance()->inform("Wrong speed in MSLaneSpeedTrigger in file '" + _file + "'.");
            return;
        }
        // set the values for the next step as they are valid
        myLoadedSpeeds.push_back(make_pair(next, speed));
    } catch(NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Could not initialise vss '" + getID() + "'.");
        throw ProcessError();
    }
}


SUMOReal
MSLaneSpeedTrigger::getDefaultSpeed() const
{
    return myDefaultSpeed;
}


void
MSLaneSpeedTrigger::setOverriding(bool val)
{
    myAmOverriding = val;
    processCommand(false);
}


void
MSLaneSpeedTrigger::setOverridingValue(SUMOReal val)
{
    mySpeedOverrideValue = val;
    processCommand(false);
}


SUMOReal
MSLaneSpeedTrigger::getLoadedSpeed()
{
    if(myCurrentEntry!=myLoadedSpeeds.end()) {
        return (*myCurrentEntry).second;
    } else {
        return (*(myCurrentEntry-1)).second;
    }
}


SUMOReal
MSLaneSpeedTrigger::getCurrentSpeed() const
{
    if(myAmOverriding) {
        return mySpeedOverrideValue;
    } else {
        // ok, maybe the first shall not yet be the valid one
        if(myCurrentEntry==myLoadedSpeeds.begin()&&(*myCurrentEntry).first>MSNet::getInstance()->getCurrentTimeStep()) {
            return myDefaultSpeed;
        }
        // try the loaded
        if(myCurrentEntry!=myLoadedSpeeds.end()&&(*myCurrentEntry).first<=MSNet::getInstance()->getCurrentTimeStep()) {
            return (*myCurrentEntry).second;
        } else {
            return (*(myCurrentEntry-1)).second;
        }
    }
}


void
MSLaneSpeedTrigger::myCharacters(int , const std::string &,
                                 const std::string &)
{
}


void
MSLaneSpeedTrigger::myEndElement(int , const std::string &)
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


