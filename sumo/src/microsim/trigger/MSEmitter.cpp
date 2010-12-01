/****************************************************************************/
/// @file    MSEmitter.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 21.07.2005
/// @version $Id$
///
// A vehicle emitting device
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/common/Command.h>
#include <utils/common/WrappingCommand.h>
#include <microsim/MSLane.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include "MSEmitter.h"
#include <microsim/MSGlobals.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



/* -------------------------------------------------------------------------
 * MSTriggeredReader::UserCommand-methods
 * ----------------------------------------------------------------------- */
MSEmitter::MSEmitter_FileTriggeredChild::MSEmitter_FileTriggeredChild(
    MSNet &net, const std::string &aXMLFilename,
    MSEmitter &parent, MSVehicleControl &vc) throw()
        : MSTriggeredXMLReader(net, aXMLFilename), MSEmitterChild(parent, vc),
        myHaveNext(false), myFlow(-1), myHaveInitialisedFlow(false), myRunningID(0) {
    myBeginTime = net.getCurrentTimeStep();
}


MSEmitter::MSEmitter_FileTriggeredChild::~MSEmitter_FileTriggeredChild() throw() {}


SUMOTime
MSEmitter::MSEmitter_FileTriggeredChild::execute(SUMOTime) throw(ProcessError) {
    if (myParent.childCheckEmit(this)) {
        buildAndScheduleFlowVehicle();
        return (SUMOTime) computeOffset(myFlow);
    }
    return 1;
}


bool
MSEmitter::MSEmitter_FileTriggeredChild::processNextEntryReaderTriggered() {
    if (myFlow>=0) {
        return true;
    }
    if (!myHaveNext) {
        return true;
    }
    if (myParent.childCheckEmit(this)) {
        myHaveNext = false;
        return true;
    }
    return false;
}


void
MSEmitter::MSEmitter_FileTriggeredChild::buildAndScheduleFlowVehicle() {
    SUMOVehicleParameter* pars = new SUMOVehicleParameter();
    pars->id = myParent.getID() + "_" + toString(myRunningID++);
    pars->depart = myOffset+1;
    pars->repetitionNumber = -1;
    pars->repetitionOffset = -1;
    MSVehicleType* aVehType = myVTypeDist.getOverallProb()>0
                              ? myVTypeDist.get()
                              : MSNet::getInstance()->getVehicleControl().getVType();
    if (aVehType==0) {
        WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": no valid vehicle type exists.");
        WRITE_WARNING("Continuing with next element.");
        return;// false;
    }
    // check and assign vehicle type
    const MSRoute *aEmitRoute = myRouteDist.get();
    if (aEmitRoute==0) {
        WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": no valid route exsists.");
        WRITE_WARNING("Continuing with next element.");
        return;// false;
    }

    MSVehicle *veh = static_cast<MSVehicle*>(
        MSNet::getInstance()->getVehicleControl().buildVehicle(pars, aEmitRoute, aVehType));
    myParent.schedule(this, veh, -1);
    myHaveNext = true;
}


void
MSEmitter::MSEmitter_FileTriggeredChild::myStartElement(SumoXMLTag element,
        const SUMOSAXAttributes &attrs) throw(ProcessError) {
    if (element==SUMO_TAG_ROUTEDISTELEM) {
        // check probability
        bool ok = true;
        SUMOReal prob = attrs.getSUMORealReporting(SUMO_ATTR_PROB, "emitter/routedistelem", myParent.getID().c_str(), ok);
        std::string routeStr = attrs.getStringReporting(SUMO_ATTR_ID, "emitter/routedistelem", myParent.getID().c_str(), ok);
        if (ok) {
            const MSRoute* route = MSRoute::dictionary(routeStr);
            if (route == 0) {
                throw ProcessError("MSTriggeredSource " + myParent.getID() + ": Route '" + routeStr + "' does not exist.");
            }
            if (prob<0) {
                throw ProcessError("MSTriggeredSource " + myParent.getID() + ": Attribute 'probability' for route '" + routeStr + "' is negative (must not).");
            }
            // atributes ok, add to routeDist
            myRouteDist.add(prob, route);
            return;
        }
        throw ProcessError();
    }

    if (element==SUMO_TAG_VTYPEDISTELEM) {
        bool ok = true;
        SUMOReal prob = attrs.getSUMORealReporting(SUMO_ATTR_PROB, "emitter/vtypedistelem", myParent.getID().c_str(), ok);
        std::string vtypeStr = attrs.getStringReporting(SUMO_ATTR_ID, "emitter/vtypedistelem", myParent.getID().c_str(), ok);
        if (ok) {
            MSVehicleType *vtype = MSNet::getInstance()->getVehicleControl().getVType(vtypeStr);
            if (vtype==0) {
                throw ProcessError("MSTriggeredSource " + myParent.getID() + ": Vehicle type '" + vtypeStr + "' does not exist.");
            }
            if (prob<0) {
                throw ProcessError("MSTriggeredSource " + myParent.getID() + ": Attribute 'probability' for vtype '" + vtypeStr + "' is negative (must not).");
            }
            myVTypeDist.add(prob, vtype);
            return;
        }
        throw ProcessError();
    }

    if (element==SUMO_TAG_FLOW) {
        bool ok = true;
        SUMOReal no = attrs.getSUMORealReporting(SUMO_ATTR_NO, "emitter/flow", myParent.getID().c_str(), ok);
        if (no<0) {
            throw ProcessError("MSTriggeredSource " + myParent.getID() + ": Negative flow in emitter '" + myParent.getID() + "'.");
        }
        SUMOTime end = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, "emitter/flow", myParent.getID().c_str(), ok, -1);
        if (!ok) {
            return;
        }
        myFlow = (SUMOReal) no;
        if (end==-1||end>=MSNet::getInstance()->getCurrentTimeStep()) {
            if (myFlow>0) {
                buildAndScheduleFlowVehicle();
                MSNet::getInstance()->getEmissionEvents().addEvent(
                    new WrappingCommand<MSEmitter::MSEmitter_FileTriggeredChild>(this, &MSEmitter::MSEmitter_FileTriggeredChild::execute),
                    (SUMOTime)(1. / (myFlow / 3600.))+MSNet::getInstance()->getCurrentTimeStep(),
                    MSEventControl::ADAPT_AFTER_EXECUTION);
                myHaveInitialisedFlow = true;
            }
        }
    }

    // check whethe the correct tag is read
    if (element==SUMO_TAG_EMIT) {
        bool ok = true;
        SUMOTime depart = attrs.getOptSUMOTimeReporting(SUMO_ATTR_TIME, "emit", myParent.getID().c_str(), ok, -1);
        SUMOReal departSpeed = attrs.getOptSUMORealReporting(SUMO_ATTR_SPEED, "emit", myParent.getID().c_str(), ok, -1);
        std::string id = attrs.getOptStringReporting(SUMO_ATTR_ID, "emit", myParent.getID().c_str(), ok, "");
        std::string type = attrs.getOptStringReporting(SUMO_ATTR_TYPE, "emit", myParent.getID().c_str(), ok, "");
        std::string route = attrs.getOptStringReporting(SUMO_ATTR_ROUTE, "emit", myParent.getID().c_str(), ok, "");
        if (!ok) {
            return;
        }
        if (depart<myBeginTime) {
            // do not process the vehicle if the emission time is before the simulation begin
            return;
        }
        SUMOVehicleParameter* pars = new SUMOVehicleParameter();
        pars->repetitionNumber = -1;
        pars->repetitionOffset = -1;
        pars->depart = depart;
        pars->departSpeed = departSpeed;
        // check and assign id
        pars->id = id;
        if (myVehicleControl.getVehicle(pars->id)!=0) {
            WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": Vehicle " + pars->id + " already exists.\n Generating a default id.");
            pars->id = "";
        }
        if (pars->id=="") {
            pars->id = myParent.getID() +  "_" + time2string(pars->depart) +  "_" + toString(myRunningID++);
            if (myVehicleControl.getVehicle(pars->id)!=0) {
                WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": Vehicle " + pars->id + " already exists.\n Continuing with next element.");
                delete pars;
                return;
            }
        }
        // check and assign vehicle type
        pars->vtypeid = type;
        MSVehicleType* aVehType = MSNet::getInstance()->getVehicleControl().getVType(pars->vtypeid);
        if (aVehType == 0) {
            if (myVTypeDist.getOverallProb()!=0) {
                aVehType = myVTypeDist.get();
            }
            if (aVehType==0) {
                aVehType = MSNet::getInstance()->getVehicleControl().getVType();
                if (aVehType==0) {
                    WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": no valid vehicle type exists.\n Continuing with next element.");
                    delete pars;
                    return;
                }
            }
        }
        // check and assign vehicle type
        pars->routeid = route;
        const MSRoute *aEmitRoute = MSRoute::dictionary(pars->routeid);
        if (aEmitRoute==0) {
            if (myRouteDist.getOverallProb()!=0) {
                aEmitRoute = myRouteDist.get();
            }
            if (aEmitRoute==0) {
                WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": no valid route exsists.");
                WRITE_WARNING("Continuing with next element.");
                delete pars;
                return;
            }
        }
        // build vehicle
        MSVehicle *veh = static_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().buildVehicle(pars, aEmitRoute, aVehType));
        myParent.schedule(this, veh, pars->departSpeed);
        myHaveNext = true;
        myOffset = SUMOTime(pars->depart);
    }
    // check whethe the correct tag is read
    if (element==SUMO_TAG_RESET) {
        myVTypeDist.clear();
        myRouteDist.clear();
    }
}


bool
MSEmitter::MSEmitter_FileTriggeredChild::nextRead() {
    return myHaveNext;
}


SUMOReal
MSEmitter::MSEmitter_FileTriggeredChild::getLoadedFlow() const {
    return myFlow;
}


void
MSEmitter::MSEmitter_FileTriggeredChild::inputEndReached() {
    if (myFlow>0&&!myHaveInitialisedFlow) {
        buildAndScheduleFlowVehicle();
        MSNet::getInstance()->getEmissionEvents().addEvent(
            new WrappingCommand<MSEmitter::MSEmitter_FileTriggeredChild>(this, &MSEmitter::MSEmitter_FileTriggeredChild::execute),
            (SUMOTime)(1. / (myFlow / 3600.))+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        myHaveInitialisedFlow = true;
    }
}


// ===========================================================================
// method definitions
// ===========================================================================
MSEmitter::MSEmitter(const std::string &id,
                     MSNet &net,
                     MSLane* destLane, SUMOReal pos,
                     const std::string &aXMLFilename) throw()
        : MSTrigger(id), myNet(net),
        myDestLane(destLane), myPos((SUMOReal) pos) {
    assert(myPos>=0);
    myActiveChild =
        new MSEmitter_FileTriggeredChild(net, aXMLFilename, *this, net.getVehicleControl());
    myFileBasedEmitter = myActiveChild;
}


MSEmitter::~MSEmitter() throw() {
    delete myFileBasedEmitter;
    std::map<MSEmitterChild*, std::pair<MSVehicle*, SUMOReal> >::iterator i;
    for (i=myToEmit.begin(); i!=myToEmit.end(); ++i) {
        delete(*i).second.first;
    }
}


bool
MSEmitter::childCheckEmit(MSEmitterChild *child) {
    if (myToEmit.find(child)==myToEmit.end()) {
        // should not happen - a child is calling and should have a vehicle added
        throw 1;
    }
    if (child!=myActiveChild||myDestLane->getEdge().isVaporizing()) {
        // remove the vehicle previously inserted by the child
        delete myToEmit[child].first;
        // erase the child information
        myToEmit.erase(myToEmit.find(child));
        // inform child to process the next one (the current was not used)
        return true;
    }
    // get the vehicle and the speed the child has read/generated
    MSVehicle *veh = myToEmit[child].first;
    if (veh->getParameter().depart > MSNet::getInstance()->getCurrentTimeStep()) {
        return false;
    }
    SUMOReal speed = myToEmit[child].second;
    // !!! add warning if speed to high or negative
    // check whether the speed shall be patched
    SUMOReal pos = myPos;
    if (speed<0) {
        speed = MIN2(myDestLane->getMaxSpeed(), veh->getMaxSpeed());
    } else {
        speed = MIN3(myDestLane->getMaxSpeed(), veh->getMaxSpeed(), speed);
    }
    // try to emit
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        if (myDestLane->getEdge().emit(*veh,  myNet.getCurrentTimeStep())) {
            veh->onDepart();
            // insert vehicle into the dictionary
            if (!myNet.getVehicleControl().addVehicle(veh->getID(), veh)) {
                // !!!
                throw 1;
            }
            // erase the child information
            myToEmit.erase(myToEmit.find(child));
            return true;
        }
    } else {
#endif
        if (myDestLane->isEmissionSuccess(veh, speed, pos, true)) {
            veh->onDepart();
            // insert vehicle into the dictionary
            if (!myNet.getVehicleControl().addVehicle(veh->getID(), veh)) {
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
                    MSVehicle *v, SUMOReal speed) {
    myToEmit[child] = std::make_pair(v, speed);
}


size_t
MSEmitter::getActiveChildIndex() const {
    return
        myFileBasedEmitter==myActiveChild ? 0 : 1;
}


void
MSEmitter::setActiveChild(MSEmitterChild *c) {
    myActiveChild = c;
}


/****************************************************************************/

