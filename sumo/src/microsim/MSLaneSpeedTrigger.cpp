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
// Revision 1.11  2005/05/04 08:28:15  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.10  2005/02/01 10:10:41  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.9  2004/12/16 12:25:26  dkrajzew
// started a better vss handling
//
// Revision 1.8  2004/11/23 10:20:10  dkrajzew
// new detectors and tls usage applied; debugging
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
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/MsgHandler.h>
#include <helpers/Command.h>
#include <microsim/MSLane.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include "MSLaneSpeedTrigger.h"


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
    : MSTriggeredXMLReader(net, aXMLFilename), MSTrigger(id),
    myDestLanes(destLanes), myHaveNext(false), myAmOverriding(false)
{
}


MSLaneSpeedTrigger::~MSLaneSpeedTrigger()
{
}


void
MSLaneSpeedTrigger::init(MSNet &net)
{
    MSTriggeredXMLReader::init(net);
}


void
MSLaneSpeedTrigger::processNext()
{
    std::vector<MSLane*>::iterator i;
    for(i=myDestLanes.begin(); i!=myDestLanes.end(); ++i) {
        (*i)->myMaxSpeed = myCurrentSpeed;
    }
    myHaveNext = false;
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
        long next = getLongSecure(attrs, SUMO_ATTR_TIME, -1);
        double speed = getFloatSecure(attrs, SUMO_ATTR_SPEED, -1.0);
        // check the values
        if(next<0) {
            MsgHandler::getErrorInstance()->inform(
                string("Wrong time in MSLaneSpeedTrigger in file '")
                + _file
                + string("'."));
            return;
        }
        if(speed<0) {
            MsgHandler::getErrorInstance()->inform(
                string("Wrong speed in MSLaneSpeedTrigger in file '")
                + _file
                + string("'."));
            return;
        }
        // set the values for the next step as they are valid
        myCurrentSpeed = speed;
        _offset = SUMOTime(next);
        myHaveNext = true;
        myLoadedSpeed = myCurrentSpeed;
        if(myAmOverriding) {
            myCurrentSpeed = mySpeedOverrideValue;
        }
    } catch(NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            string("Could not initialise vss '") + getID()
            + string("'."));
        throw ProcessError();
    }
}


double
MSLaneSpeedTrigger::getDefaultSpeed() const
{
    return myDefaultSpeed;
}


void
MSLaneSpeedTrigger::setOverriding(bool val)
{
    myAmOverriding = val;
    if(myAmOverriding) {
        myCurrentSpeed = mySpeedOverrideValue;
    } else {
        myCurrentSpeed = myLoadedSpeed;
    }
}


void
MSLaneSpeedTrigger::setOverridingValue(double val)
{
    mySpeedOverrideValue = val;
    if(myAmOverriding) {
        myCurrentSpeed = mySpeedOverrideValue;
        processNext();
    } else {
        myCurrentSpeed = myLoadedSpeed;
    }
}


double
MSLaneSpeedTrigger::getLoadedSpeed()
{
    return myLoadedSpeed;
}


double
MSLaneSpeedTrigger::getCurrentSpeed() const
{
    return (*(myDestLanes.begin()))->maxSpeed();
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


bool
MSLaneSpeedTrigger::nextRead()
{
    return myHaveNext;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


