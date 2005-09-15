//---------------------------------------------------------------------------//
//                        MSTriggeredEmitter.cpp -
//  Class that realises the setting of a lane's maximum speed triggered by
//      values read from a file
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 21.07.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.1  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/helpers/Command.h>
#include <microsim/MSLane.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/convert/ToString.h>
#include <microsim/MSEventControl.h>
#include "MSTriggeredEmitter.h"
#include <microsim/MSGlobals.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;



/* -------------------------------------------------------------------------
 * MSTriggeredReader::UserCommand-methods
 * ----------------------------------------------------------------------- */
MSTriggeredEmitter::UserCommand::UserCommand(MSTriggeredEmitter &parent)
    : _parent(parent)
{
}


MSTriggeredEmitter::UserCommand::~UserCommand( void )
{
}


SUMOTime
MSTriggeredEmitter::UserCommand::execute()
{
    if(_parent.wantsMe(this)) {
        return _parent.userEmit();
    }
    return 0;
}


/* =========================================================================
 * method definitions
 * ======================================================================= */
int beginTime_;

MSTriggeredEmitter::MSTriggeredEmitter(const std::string &id,
                                       MSNet &net,
                                       MSLane* destLane, double pos,
                                       const std::string &aXMLFilename)
    : MSTriggeredXMLReader(net, aXMLFilename), MSTrigger(id),
    myDestLane(destLane), myHaveNext(false), myAmOverriding(false),
    myPos(pos), myUserFlow(1), myUserMode(false), myLastUserEmit(0), myVehicle(0)
{
    assert(myPos>=0);
    beginTime_ = MSNet::getInstance()->getCurrentTimeStep();
}


MSTriggeredEmitter::~MSTriggeredEmitter()
{
}


bool
MSTriggeredEmitter::processNext()
{
    if(myVehicle==0) {
        return false;
    }
    // try to emit
#ifdef HAVE_MESOSIM
    if(MSGlobals::gUseMesoSim) {
        if ( ((MSEdge&) myDestLane->edge()).emit( *myVehicle,  MSNet::getInstance()->getCurrentTimeStep() ) ) {
            MSNet::getInstance()->getVehicleControl().vehiclesEmitted(1);
            myVehicle->onDepart();
            myHaveNext = false;
//            readNextEmitElement();
            myVehicle = 0;
            return true;
        }
    } else {
#endif
    if ( myDestLane->isEmissionSuccess( myVehicle ) ) {
        myVehicle->onDepart();
        MSNet::getInstance()->getVehicleControl().vehiclesEmitted(1);
        myHaveNext = false;
        myVehicle = 0;
        return true;
    }
#ifdef HAVE_MESOSIM
    }
#endif
    return false;
}


void
MSTriggeredEmitter::setUserFlow(float factor)
{
    // !!! the commands should be adapted to current flow imediatly
    myUserFlow = factor;
    //if(myUserMode&&myUserFlow!=0) {
        UserCommand *us = new UserCommand(*this);
        mySentCommands.push_back(us);
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            us, getFrequency()+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        myLastUserEmit = MSNet::getInstance()->getCurrentTimeStep();
    //}
}


void
MSTriggeredEmitter::setUserMode(bool val)
{
    if(!myUserMode&&val&&myUserFlow!=0) {
        UserCommand *us = new UserCommand(*this);
        mySentCommands.push_back(us);
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            us, getFrequency()+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        myLastUserEmit = MSNet::getInstance()->getCurrentTimeStep();
    }
    myUserMode = val;
    if(!myUserMode) {
        UserCommand *us = new UserCommand(*this);
        mySentCommands.push_back(us);
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            us, getFrequency()+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        myLastUserEmit = MSNet::getInstance()->getCurrentTimeStep();
    }
}


int
MSTriggeredEmitter::userEmit()
{
    /*
    if(myVehicle!=0) {
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(myVehicle);
    }
    */
    /*
    if((int) (myLastUserEmit+getFrequency())>MSNet::getInstance()->getCurrentTimeStep()) {
        return myLastUserEmit+getFrequency();
    }
    myLastUserEmit = MSNet::getInstance()->getCurrentTimeStep();
    /*
    if(myNoUserEvents!=0) {
        return false;
    }
    */
    string aVehicleId = getID() + string( "_user_" ) +  toString(MSNet::getInstance()->getCurrentTimeStep());
    MSRoute *aEmitRoute = myRouteDist.get();
    SUMOTime aEmitTime = MSNet::getInstance()->getCurrentTimeStep();
    myVehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(
        aVehicleId, aEmitRoute, aEmitTime,
        MSVehicleType::dict_Random(), 0, 0, RGBColor(1, 1, 1));
    if(!MSVehicle::dictionary(aVehicleId, myVehicle)) {
        // !!!
        throw 1;
    }
    myVehicle->moveSetState( MSVehicle::State( myPos, myDestLane->maxSpeed() ) );
    processNext();
    MSNet::getInstance()->getVehicleControl().newUnbuildVehicleBuild();
    return getFrequency();
}


bool
MSTriggeredEmitter::wantsMe(MSTriggeredEmitter::UserCommand *us)
{
    assert(mySentCommands.size()>0);
    std::vector<UserCommand*>::iterator i =
        find(mySentCommands.begin(), mySentCommands.end(), us);
    if(i!=mySentCommands.end()-1) {
        mySentCommands.erase(i);
        return false;
    }
    return true;
}


float
MSTriggeredEmitter::getFrequency() const
{

    return myUserMode
        ? 1. / (myUserFlow / 3600.)
        : 1. / (myFlow / 3600.);
}


bool
MSTriggeredEmitter::inUserMode() const
{
    return myUserMode;
}


float
MSTriggeredEmitter::getUserFlow() const
{
    return myUserFlow;
}


void
MSTriggeredEmitter::myStartElement(int element, const std::string &name,
                                   const Attributes &attrs)
{
    if(name=="routedistelem") {
        // parse route distribution
        // check if route exists
        string routeStr = getStringSecure(attrs, "routeid", "");
        MSRoute* route = MSRoute::dictionary( routeStr );
        if ( route == 0 ) {
            MsgHandler::getErrorInstance()->inform(
                string("MSTriggeredSource ") + getID()
                + string(": Route '") + routeStr + string("' does not exist."));
            throw ProcessError();
        }
        // check frequency
        float freq = getFloatSecure(attrs, "frequency", -1);
        if(freq<0) {
            MsgHandler::getErrorInstance()->inform(
                string("MSTriggeredSource ") + getID()
                + string(": Attribute \"frequency\" has value < 0."));
            throw ProcessError();
        }
        // Attributes ok, add to routeDist
        myRouteDist.add(freq, route);
        return;
    }
    // vehicle-type distributions
    if(name=="vtype-dist") {
        float prob = -1;
        try {
            prob = getFloatSecure(attrs, SUMO_ATTR_PROB, -1);
        } catch(NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + getID() + "' (" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + ").");
            return;
        }
        if(prob<=0) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + getID() + "' (" + toString(prob) + ").");
            return;
        }
        // get the id
        string id;
        try {
            id = getString(attrs, SUMO_ATTR_ID);
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("Error in description: missing id of a vtype-object.");
            return;
        }
        MSVehicleType *vtype = MSVehicleType::dictionary(id);
        if(vtype==0) {
            MsgHandler::getErrorInstance()->inform("Error in description: unknown vtype-object '" + id + "'.");
            return;
        }
        myCurrentVTypeDist.add(prob, vtype);
    }

    if(name=="flow") {
        float no = -1;
        try {
            no = getFloatSecure(attrs, SUMO_ATTR_NO, -1);
        } catch(NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + getID() + "' (" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + ").");
            return;
        }
        if(no<=0) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + getID() + "' (" + toString(no) + ").");
            return;
        }
        myFlow = no / 24.; // !!!
        UserCommand *us = new UserCommand(*this);
        mySentCommands.push_back(us);
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            us, getFrequency()+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        MSNet::getInstance()->getVehicleControl().newUnbuildVehicleBuild();
    }


    // check whethe the correct tag is read
    if(name=="emit") {
        // check and assign id
        string aVehicleId = getID() + string( "_" ) + getStringSecure(attrs, "id", "")
            + "_" + toString(beginTime_);
        MSVehicle* veh = MSVehicle::dictionary( aVehicleId );
        if ( veh != 0 ) {
            WRITE_WARNING(string("MSTriggeredSource ") + getID()+ string(": Vehicle ") + aVehicleId+ string(" does already exist. "));
            WRITE_WARNING("Continuing with next element.");
            return;// false;
        }
        // check and assign vehicle type
        string emitType = getStringSecure(attrs, "vehtype", "");
        MSVehicleType* aVehType = MSVehicleType::dictionary( emitType );
        if ( aVehType == 0 ) {
            WRITE_WARNING(string("MSTriggeredSource ") + getID()+ string(": Vehicle type ") + emitType + string(" does not exist. "));
            WRITE_WARNING("Continuing with next element.");
            return;// false;
        }
        // check and assign vehicle type
        string emitRoute = getStringSecure(attrs, "route", "");
        MSRoute *aEmitRoute = MSRoute::dictionary( emitRoute );
        if(aEmitRoute==0) {
            aEmitRoute = myRouteDist.get();
        }
        // check and assign emission time
        int aEmitTime = getIntSecure(attrs, "time", -1);
        myEmitSpeed = getFloatSecure(attrs, "speed", -1);
        if(myEmitSpeed<0) {
            myEmitSpeed = 0;
        } else if( myEmitSpeed > myDestLane->maxSpeed() ) {
            myEmitSpeed = myDestLane->maxSpeed();
        }
        myVehicle = 0;
        if(!myUserMode) {
            myVehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(
                aVehicleId, aEmitRoute, aEmitTime,
                aVehType, 0, 0, RGBColor(1, 1, 1));
            if(!MSVehicle::dictionary(aVehicleId, myVehicle)) {
                // !!!
                throw 1;
            }
            myVehicle->moveSetState( MSVehicle::State( myPos, myEmitSpeed ) );
        }
        myHaveNext = true;
        _offset = SUMOTime(aEmitTime);
    }
}


/*
void
MSTriggeredEmitter::setOverriding(bool val)
{
    myAmOverriding = val;
    if(myAmOverriding) {
        myCurrentSpeed = mySpeedOverrideValue;
    } else {
        myCurrentSpeed = myLoadedSpeed;
    }
}


void
MSTriggeredEmitter::setOverridingValue(double val)
{
    mySpeedOverrideValue = val;
    if(myAmOverriding) {
        myCurrentSpeed = mySpeedOverrideValue;
        processNext();
    } else {
        myCurrentSpeed = myLoadedSpeed;
    }
}
*/

void
MSTriggeredEmitter::myCharacters(int , const std::string &,
                                 const std::string &)
{
}


void
MSTriggeredEmitter::myEndElement(int , const std::string &)
{
}


bool
MSTriggeredEmitter::nextRead()
{
    return myHaveNext;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


