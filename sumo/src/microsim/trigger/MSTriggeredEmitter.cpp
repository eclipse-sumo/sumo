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
// Revision 1.5  2005/10/17 08:58:24  dkrajzew
// trigger rework#1
//
// Revision 1.4  2005/10/07 11:37:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/23 13:16:41  dkrajzew
// debugging the building process
//
// Revision 1.2  2005/09/22 13:45:52  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
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
#include <utils/common/ToString.h>
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
int beginTime_;


MSTriggeredEmitter::MSTriggeredEmitter_FileTriggeredChild::MSTriggeredEmitter_FileTriggeredChild(
            MSNet &net, const std::string &aXMLFilename,
            MSTriggeredEmitter &parent)
    : MSTriggeredEmitterChild(parent), MSTriggeredXMLReader(net, aXMLFilename), myHaveNext(false)
{
}


MSTriggeredEmitter::MSTriggeredEmitter_FileTriggeredChild::~MSTriggeredEmitter_FileTriggeredChild()
{
}

/*
SUMOTime
MSTriggeredEmitter::MSTriggeredEmitter_FileTriggeredChild::execute()
{
    if(myParent.childCheckEmit(this)) {
        myHaveNext = false;
    }
    return 1;
}
*/

bool
MSTriggeredEmitter::MSTriggeredEmitter_FileTriggeredChild::processNext()
{
    if(myParent.childCheckEmit(this)) {
        myHaveNext = false;
        return true;
    }
    return false;
}


void
MSTriggeredEmitter::MSTriggeredEmitter_FileTriggeredChild::myStartElement(int element, const std::string &name,
                                   const Attributes &attrs)
{
    if(name=="routedistelem") {
        // parse route distribution
        // check if route exists
        string routeStr = getStringSecure(attrs, "routeid", "");
        MSRoute* route = MSRoute::dictionary( routeStr );
        if ( route == 0 ) {
            MsgHandler::getErrorInstance()->inform(
                string("MSTriggeredSource ") + myParent.getID()
                + string(": Route '") + routeStr + string("' does not exist."));
            throw ProcessError();
        }
        // check frequency
        SUMOReal freq = getFloatSecure(attrs, "frequency", -1);
        if(freq<0) {
            MsgHandler::getErrorInstance()->inform(
                string("MSTriggeredSource ") + myParent.getID()
                + string(": Attribute \"frequency\" has value < 0."));
            throw ProcessError();
        }
        // Attributes ok, add to routeDist
        myRouteDist.add(freq, route);
        return;
    }
    // vehicle-type distributions
    if(name=="vtype-dist") {
        SUMOReal prob = -1;
        try {
            prob = getFloatSecure(attrs, SUMO_ATTR_PROB, -1);
        } catch(NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + myParent.getID() + "' (" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + ").");
            return;
        }
        if(prob<=0) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + myParent.getID() + "' (" + toString(prob) + ").");
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

    /*
    if(name=="flow") {
        SUMOReal no = -1;
        try {
            no = getFloatSecure(attrs, SUMO_ATTR_NO, -1);
        } catch(NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + myParent.getID() + "' (" + getStringSecure(attrs, SUMO_ATTR_PROB, "") + ").");
            return;
        }
        if(no<=0) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + myParent.getID() + "' (" + toString(no) + ").");
            return;
        }
        myFlow = (SUMOReal) (no / 24.); // !!!
        UserCommand *us = new UserCommand(*this);
        mySentCommands.push_back(us);
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            us, (SUMOTime) getFrequency()+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        MSNet::getInstance()->getVehicleControl().newUnbuildVehicleBuild();
    }
    */


    // check whethe the correct tag is read
    if(name=="emit") {
        // check and assign id
        string aVehicleId = myParent.getID() + string( "_" ) + getStringSecure(attrs, "id", "")
            + "_" + toString(beginTime_);
        MSVehicle* veh = MSVehicle::dictionary( aVehicleId );
        if ( veh != 0 ) {
            WRITE_WARNING(string("MSTriggeredSource ") + myParent.getID()+ string(": Vehicle ") + aVehicleId+ string(" does already exist. "));
            WRITE_WARNING("Continuing with next element.");
            return;// false;
        }
        // check and assign vehicle type
        string emitType = getStringSecure(attrs, "vehtype", "");
        MSVehicleType* aVehType = MSVehicleType::dictionary( emitType );
        if ( aVehType == 0 ) {
            WRITE_WARNING(string("MSTriggeredSource ") + myParent.getID()+ string(": Vehicle type ") + emitType + string(" does not exist. "));
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
        SUMOReal aEmitSpeed = getFloatSecure(attrs, "speed", -1);
        veh =
            MSNet::getInstance()->getVehicleControl().buildVehicle(
                aVehicleId, aEmitRoute, aEmitTime,
                aVehType, 0, 0, RGBColor(1, 1, 1));
        myParent.schedule(this, veh, aEmitSpeed);
        myHaveNext = true;
        _offset = SUMOTime(aEmitTime);

        /* !!!!
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
        */
    }
}


void
MSTriggeredEmitter::MSTriggeredEmitter_FileTriggeredChild::myCharacters(int , const std::string &,
                                 const std::string &)
{
}


void
MSTriggeredEmitter::MSTriggeredEmitter_FileTriggeredChild::myEndElement(int , const std::string &)
{
}


bool
MSTriggeredEmitter::MSTriggeredEmitter_FileTriggeredChild::nextRead()
{
    return myHaveNext;
}


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSTriggeredEmitter::MSTriggeredEmitter(const std::string &id,
                                       MSNet &net,
                                       MSLane* destLane, SUMOReal pos,
                                       const std::string &aXMLFilename)
    : MSTrigger(id), myNet(net),
    myDestLane(destLane), myAmOverriding(false),
    myPos((SUMOReal) pos), myUserFlow(1), myUserMode(false), myLastUserEmit(0)
{
    assert(myPos>=0);
    beginTime_ = MSNet::getInstance()->getCurrentTimeStep();
    myActiveChild =
        new MSTriggeredEmitter_FileTriggeredChild(net, aXMLFilename, *this);
    myFileBasedEmitter = myActiveChild;
}


MSTriggeredEmitter::~MSTriggeredEmitter()
{
    {
        delete myFileBasedEmitter;
        /*
        for(std::vector<MSTriggeredEmitterChild*>::iterator i=myChildren.begin(); i!=myChildren.end(); ++i) {
            delete *i;
        }
        */
    }
    {
        std::map<MSTriggeredEmitterChild*, std::pair<MSVehicle*, SUMOReal> >::iterator i;
        for(i=myToEmit.begin(); i!=myToEmit.end(); ++i) {
            delete (*i).second.first;
        }
    }
}


bool
MSTriggeredEmitter::childCheckEmit(MSTriggeredEmitterChild *child)
{
    if(myToEmit.find(child)==myToEmit.end()) {
        // should not happen - a child is calling and should have a vehicle added
        throw 1;
    }
    if(child!=myActiveChild) {
        // remove the vehicle previously inserted by the child
        delete myToEmit[child].first;
        // erase the child information
        myToEmit.erase(myToEmit.find(child));
        // inform child to process the next one (the current was not used)
        return true;
    }
    // get the vehicle and the speed the child has read/generated
    MSVehicle *veh = myToEmit[child].first;
    SUMOReal speed = myToEmit[child].second;
    // check whether the speed shall be patched
    if(speed>=0) {
        veh->moveSetState( MSVehicle::State( myPos, speed ) );
    } else {
        veh->moveSetState( MSVehicle::State( myPos, myDestLane->maxSpeed() ) );
    }
    // try to emit
    if ( myDestLane->isEmissionSuccess( veh ) ) {
        veh->onDepart();
        MSNet::getInstance()->getVehicleControl().vehiclesEmitted(1);
        // insert vehicle into the dictionary
        if(!MSVehicle::dictionary(veh->id(), veh)) {
            // !!!
            throw 1;
        }
        // erase the child information
        myToEmit.erase(myToEmit.find(child));
        return true;
    }
    return false;
}

    /*

void
MSTriggeredEmitter::setUserFlow(SUMOReal factor)
{
    // !!! the commands should be adapted to current flow imediatly
    myUserFlow = factor;
    //if(myUserMode&&myUserFlow!=0) {
        UserCommand *us = new UserCommand(*this);
        mySentCommands.push_back(us);
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            us, (SUMOTime) getFrequency()+MSNet::getInstance()->getCurrentTimeStep(),
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
            us, (SUMOTime) getFrequency()+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        myLastUserEmit = MSNet::getInstance()->getCurrentTimeStep();
    }
    myUserMode = val;
    if(!myUserMode) {
        UserCommand *us = new UserCommand(*this);
        mySentCommands.push_back(us);
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            us, (SUMOTime) getFrequency()+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        myLastUserEmit = MSNet::getInstance()->getCurrentTimeStep();
    }
}


int
MSTriggeredEmitter::userEmit()
{
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
    return (SUMOTime) getFrequency();
}

/*
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
*/
/*
SUMOReal
MSTriggeredEmitter::getFrequency() const
{
    throw 1;
    return myUserMode
        ? (SUMOReal) (1. / (myUserFlow / 3600.))
        : (SUMOReal) (1. / (myFlow / 3600.));
}


bool
MSTriggeredEmitter::inUserMode() const
{
    return myUserMode;
}


SUMOReal
MSTriggeredEmitter::getUserFlow() const
{
    return myUserFlow;
}
*/

void
MSTriggeredEmitter::schedule(MSTriggeredEmitterChild *child,
                             MSVehicle *v, SUMOReal speed)
{
    myToEmit[child] = make_pair(v, speed);
}


size_t
MSTriggeredEmitter::getActiveChildIndex() const
{
    return
        myFileBasedEmitter==myActiveChild ? 0 : 1;
}


void
MSTriggeredEmitter::setActiveChild(MSTriggeredEmitterChild *c)
{
    myActiveChild = c;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


