#include <string>
#include <helpers/Command.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <utils/common/SErrorHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "MSLaneSpeedTrigger.h"

using namespace std;

MSLaneSpeedTrigger::MSLaneSpeedTrigger(const std::string &id,
                                       MSNet &net, MSLane &destLane, 
                                       const std::string &aXMLFilename)
    : MSTrigger(id),
    MSTriggeredXMLReader(net, aXMLFilename), _destLane(destLane)
{
}

MSLaneSpeedTrigger::~MSLaneSpeedTrigger()
{
}


void 
MSLaneSpeedTrigger::init(MSNet &net)
{
    MSTriggeredXMLReader::init(net);
    _destLane.myMaxSpeed = _currentSpeed;
}


void  
MSLaneSpeedTrigger::processNext()
{
    _destLane.myMaxSpeed = _currentSpeed;
}


void 
MSLaneSpeedTrigger::myStartElement(int element, const std::string &name,
                                   const Attributes &attrs)
{
    // check whethe the correct tag is read
    if(element!=SUMO_TAG_STEP) {
        return;
    }
    // extract the values
    long next = getLongSecure(attrs, SUMO_ATTR_TIME, -1);
    double speed = getFloatSecure(attrs, SUMO_ATTR_SPEED, -1.0);
    // check the values
    if(next<0) {
        SErrorHandler::add(
            string("Wrong time in MSLaneSpeedTrigger in file '")
            + _filename 
            + string("'."));
        return;
    }
    if(speed<0) {
        SErrorHandler::add(
            string("Wrong speed in MSLaneSpeedTrigger in file '")
            + _filename 
            + string("'."));
        return;
    }
    // set the values for the next step as they are valid
    _currentSpeed = speed;
    _offset = MSNet::Time(next) - _offset;
}


void 
MSLaneSpeedTrigger::myCharacters(int element, const std::string &name,
                                 const std::string &chars)
{
}


void
MSLaneSpeedTrigger::myEndElement(int element, const std::string &name)
{
}


