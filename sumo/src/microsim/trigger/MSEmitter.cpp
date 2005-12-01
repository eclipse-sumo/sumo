//---------------------------------------------------------------------------//
//                        MSEmitter.cpp -
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
// Revision 1.2  2005/12/01 07:37:35  dkrajzew
// introducing bus stops: eased building vehicles; vehicles may now have nested elements
//
// Revision 1.1  2005/11/09 06:35:03  dkrajzew
// Emitters reworked
//
// Revision 1.4  2005/10/06 13:39:21  dksumo
// using of a configuration file rechecked
//
// Revision 1.3  2005/09/20 06:11:17  dksumo
// floats and doubles replaced by SUMOReal; warnings removed
//
// Revision 1.2  2005/09/09 12:51:25  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.1  2005/08/01 13:31:00  dksumo
// triggers reworked and new added
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
#include <utils/helpers/SimpleCommand.h>
#include <microsim/MSLane.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include "MSEmitter.h"
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
MSEmitter::MSEmitter_FileTriggeredChild::MSEmitter_FileTriggeredChild(
            MSNet &net, const std::string &aXMLFilename,
            MSEmitter &parent)
    : MSEmitterChild(parent), MSTriggeredXMLReader(net, aXMLFilename), myHaveNext(false),
    myFlow(-1), myRunningID(0), myHaveInitialisedFlow(false)
{
    myBeginTime = net.getCurrentTimeStep();
}


MSEmitter::MSEmitter_FileTriggeredChild::~MSEmitter_FileTriggeredChild()
{
}


SUMOTime
MSEmitter::MSEmitter_FileTriggeredChild::execute()
{
    if(myParent.childCheckEmit(this)) {
        buildAndScheduleFlowVehicle();
        return (SUMOTime) computeOffset(myFlow);
    }
    return 1;
}


bool
MSEmitter::MSEmitter_FileTriggeredChild::processNextEntryReaderTriggered()
{
    if(myFlow>=0) {
        return true;
    }
    if(myParent.childCheckEmit(this)) {
        myHaveNext = false;
        return true;
    }
    return false;
}


void
MSEmitter::MSEmitter_FileTriggeredChild::buildAndScheduleFlowVehicle()
{
    string aVehicleId = myParent.getID() + "_" + toString(myRunningID++);
    MSVehicleType* aVehType = myVTypeDist.getOverallProb()>0
        ? myVTypeDist.get()
        : MSVehicleType::dict_Random();
    if(aVehType==0) {
        WRITE_WARNING(string("MSTriggeredSource ") + myParent.getID()+ string(": no valid vehicle type exists."));
        WRITE_WARNING("Continuing with next element.");
        return;// false;
    }
    // check and assign vehicle type
    MSRoute *aEmitRoute = myRouteDist.get();
    if(aEmitRoute==0) {
        WRITE_WARNING(string("MSTriggeredSource ") + myParent.getID()+ string(": no valid route exsists."));
        WRITE_WARNING("Continuing with next element.");
        return;// false;
    }

    MSVehicle *veh =
        MSNet::getInstance()->getVehicleControl().buildVehicle(
            aVehicleId, aEmitRoute, _offset+1, aVehType, 0, 0);
    myParent.schedule(this, veh, -1);
    myHaveNext = true;
}


void
MSEmitter::MSEmitter_FileTriggeredChild::myStartElement(int element, const std::string &name,
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
        myVTypeDist.add(prob, vtype);
    }

    if(name=="flow") {
        // get the flow information
        SUMOReal no = -1;
        try {
            no = getFloatSecure(attrs, SUMO_ATTR_NO, -1);
        } catch(NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("Non-numeric flow in emitter '" + myParent.getID() + "' (" + getStringSecure(attrs, SUMO_ATTR_NO, "") + ").");
            return;
        }
        if(no<0) {
            MsgHandler::getErrorInstance()->inform("Negative flow in emitter '" + myParent.getID() + "' (" + getStringSecure(attrs, SUMO_ATTR_NO, "") + ").");
            return;
        }
        // get the end of this def
        SUMOTime end = -1;
        try {
            end = (SUMOTime) getFloatSecure(attrs, SUMO_ATTR_END, -1);
        } catch(NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("Non-numeric flow end in emitter '" + myParent.getID() + "' (" + getStringSecure(attrs, SUMO_ATTR_NO, "") + ").");
            return;
        }

        myFlow = (SUMOReal) no;

        if(end==-1||end>=MSNet::getInstance()->getCurrentTimeStep()) {
            if(myFlow>0) {
                buildAndScheduleFlowVehicle();
                MSEventControl::getBeginOfTimestepEvents()->addEvent(
								     new SimpleCommand<MSEmitter::MSEmitter_FileTriggeredChild>(this, &MSEmitter::MSEmitter_FileTriggeredChild::execute),
                    (SUMOTime) (1. / (myFlow / 3600.))+MSNet::getInstance()->getCurrentTimeStep(),
                    MSEventControl::ADAPT_AFTER_EXECUTION);
                myHaveInitialisedFlow = true;
            }
        }
    }

    // check whethe the correct tag is read
    if(name=="emit") {
        // check and assign emission time
        int aEmitTime = getIntSecure(attrs, "time", -1);
        if(aEmitTime<myBeginTime) {
            // do not process the vehicle if the emission time is before the simulation begin
            return;
        }
        // check and assign id
        string aVehicleId = myParent.getID() + string( "_" ) + getStringSecure(attrs, "id", "");
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
            if(myVTypeDist.getOverallProb()!=0) {
                aVehType = myVTypeDist.get();
            }
            if(aVehType==0) {
                WRITE_WARNING(string("MSTriggeredSource ") + myParent.getID()+ string(": no valid vehicle type exists."));
                WRITE_WARNING("Continuing with next element.");
                return;// false;
            }
        }
        // check and assign vehicle type
        string emitRoute = getStringSecure(attrs, "route", "");
        MSRoute *aEmitRoute = MSRoute::dictionary( emitRoute );
        if(aEmitRoute==0) {
            if(myRouteDist.getOverallProb()!=0) {
                aEmitRoute = myRouteDist.get();
            }
            if(aEmitRoute==0) {
                WRITE_WARNING(string("MSTriggeredSource ") + myParent.getID()+ string(": no valid route exsists."));
                WRITE_WARNING("Continuing with next element.");
                return;// false;
            }
        }
        // build vehicle
        SUMOReal aEmitSpeed = getFloatSecure(attrs, "speed", -1);
        veh =
            MSNet::getInstance()->getVehicleControl().buildVehicle(
                aVehicleId, aEmitRoute, aEmitTime,
                aVehType, 0, 0);
        myParent.schedule(this, veh, aEmitSpeed);
        myHaveNext = true;
        _offset = SUMOTime(aEmitTime);
    }
    // check whethe the correct tag is read
    if(name=="reset") {
        myVTypeDist.clear();
        myRouteDist.clear();
    }
}


void
MSEmitter::MSEmitter_FileTriggeredChild::myCharacters(
        int , const std::string &, const std::string &)
{
}


void
MSEmitter::MSEmitter_FileTriggeredChild::myEndElement(
        int , const std::string &)
{
}


bool
MSEmitter::MSEmitter_FileTriggeredChild::nextRead()
{
    return myHaveNext;
}


SUMOReal
MSEmitter::MSEmitter_FileTriggeredChild::getLoadedFlow() const
{
    return myFlow;
}


void
MSEmitter::MSEmitter_FileTriggeredChild::inputEndReached()
{
    if(myFlow>0&&!myHaveInitialisedFlow) {
        buildAndScheduleFlowVehicle();
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
							     new SimpleCommand<MSEmitter::MSEmitter_FileTriggeredChild>(this, &MSEmitter::MSEmitter_FileTriggeredChild::execute),
            (SUMOTime) (1. / (myFlow / 3600.))+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        myHaveInitialisedFlow = true;
    }
}


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSEmitter::MSEmitter(const std::string &id,
                                       MSNet &net,
                                       MSLane* destLane, SUMOReal pos,
                                       const std::string &aXMLFilename)
    : MSTrigger(id), myNet(net),
    myDestLane(destLane), myPos((SUMOReal) pos)
{
    assert(myPos>=0);
    myActiveChild =
        new MSEmitter_FileTriggeredChild(net, aXMLFilename, *this);
    myFileBasedEmitter = myActiveChild;
}


MSEmitter::~MSEmitter()
{
    {
        delete myFileBasedEmitter;
    }
    {
        std::map<MSEmitterChild*, std::pair<MSVehicle*, SUMOReal> >::iterator i;
        for(i=myToEmit.begin(); i!=myToEmit.end(); ++i) {
            delete (*i).second.first;
        }
    }
}


bool
MSEmitter::childCheckEmit(MSEmitterChild *child)
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
#ifdef HAVE_MESOSIM
    if(MSGlobals::gUseMesoSim) {
        if ( ((MSEdge&) myDestLane->edge()).emit( *veh,  MSNet::getInstance()->getCurrentTimeStep() ) ) {
            MSNet::getInstance()->getVehicleControl().vehiclesEmitted(1);
            veh->onDepart();
            // insert vehicle into the dictionary
            if(!MSVehicle::dictionary(veh->id(), veh)) {
                // !!!
                throw 1;
            }
            // erase the child information
            myToEmit.erase(myToEmit.find(child));
            return true;
        }
    } else {
#endif
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
#ifdef HAVE_MESOSIM
    }
#endif
    return false;
}


void
MSEmitter::schedule(MSEmitterChild *child,
                             MSVehicle *v, SUMOReal speed)
{
    myToEmit[child] = make_pair(v, speed);
}


size_t
MSEmitter::getActiveChildIndex() const
{
    return
        myFileBasedEmitter==myActiveChild ? 0 : 1;
}


void
MSEmitter::setActiveChild(MSEmitterChild *c)
{
    myActiveChild = c;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


