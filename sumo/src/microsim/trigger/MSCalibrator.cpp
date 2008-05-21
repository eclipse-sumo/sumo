/****************************************************************************/
/// @file    MSCalibrator.cpp
/// @author  Tino Morenz
/// @date    Wed, 24.10.2007
/// @version $Id: $
///
// A vehicle emitting device
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
#include "MSCalibrator.h"
#include <microsim/MSGlobals.h>

//TM
#include <netload/NLDetectorBuilder.h>
#include <microsim/output/MSInductLoop.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;



/* -------------------------------------------------------------------------
 * MSTriggeredReader::UserCommand-methods
 * ----------------------------------------------------------------------- */
MSCalibrator::MSCalibrator_FileTriggeredChild::MSCalibrator_FileTriggeredChild(
    MSNet &net, const std::string &aXMLFilename,
    MSCalibrator &parent, MSVehicleControl &vc) throw()
        : MSTriggeredXMLReader(net, aXMLFilename), MSCalibratorChild(parent, vc),
        myHaveNext(false), myFlow(-1), myHaveInitialisedFlow(false), myRunningID(0)
{
    myBeginTime = net.getCurrentTimeStep();
}


MSCalibrator::MSCalibrator_FileTriggeredChild::~MSCalibrator_FileTriggeredChild() throw()
{}


SUMOTime
MSCalibrator::MSCalibrator_FileTriggeredChild::execute(SUMOTime) throw(ProcessError)
{
    if (myParent.childCheckEmit(this)) {
        buildAndScheduleFlowVehicle();
        return (SUMOTime) computeOffset(myFlow);
    }
    return 1;
}



SUMOTime
MSCalibrator::execute(SUMOTime timestep) throw(ProcessError)
{

    if (timestep == 0) {
        return 1;
    }

    if (myNumVehicles == -1) {
        return 1;
    }



    SUMOReal vehPerInterval = myNumVehicles / (myInterval);
    int num_replacements = 0;
    /*
    	OutputDevice *dev = OutputDevice::getOutputDevice(myDebugFilesBaseName + "diff.csv");
    	OutputDevice *dev2 = OutputDevice::getOutputDevice(myDebugFilesBaseName + "replace.csv");
    	OutputDevice *dev3 = OutputDevice::getOutputDevice(myDebugFilesBaseName + "vehicle.xml");
    */

    //positive diff=too many vehicles, negative diff=not enough vehicles
    SUMOReal veh_cnt = myIL->getNVehContributed();
    SUMOReal diff =  veh_cnt - vehPerInterval + myToCalibrate;
    SUMOReal meanSpeed = myIL->getCurrentSpeed();

    if (diff > 0) {

        unsigned int vehNum = myDestLane->getVehicleNumber();

        //not enough vehicles to delete on the lane?
        if (vehNum < diff) {

            myToCalibrate = diff - vehNum;

            diff = vehNum;
        } else {
            myToCalibrate = diff - floor(diff);
        }

        for (int i=1; i <= diff; i++) {

            for (MSLane::VehCont::const_iterator it = myDestLane->getVehiclesSecure().begin();
                    it < myDestLane->getVehiclesSecure().end();
                    it++) {

                MSVehicle * veh = (*it);

                if (veh->getVehicleType().getID() == "BUS") {
                    continue;
                } else {
                    veh->leaveLaneAtLaneChange();
                    veh->onTripEnd();
                    MSNet::getInstance()->getMSPhoneNet()->removeVehicle(*veh, MSNet::getInstance()->getCurrentTimeStep());
                    myDestLane->removeVehicle(veh);
                    MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);

                    num_replacements++;

                    break;
                }
            }
//			WRITE_WARNING("Removing Vehicle " + veh->getID() + " at Timestep: " + toString(timestep) + "\n");
        }

        //make sure enough cars were deleted, only happens if BUSSES were in the set
        myToCalibrate+diff-num_replacements;
    } else if (diff < 0) {

        myToCalibrate = diff - ceil(diff);
        for (int i=-1; i >= diff; i--) {
//			WRITE_WARNING("Inserting Vehicle at Timestep: " + toString(timestep) + "\n");

            ((MSCalibrator_FileTriggeredChild*) myFileBasedCalibrator)->buildAndScheduleFlowVehicle(meanSpeed);
            childCheckEmit(myFileBasedCalibrator);
            num_replacements--;
        }
    } else if (diff == 0) {
        myToCalibrate = 0;
    }
    /*
    	if (myDebugLevel > 0) {
    		dev->getOStream() << timestep << "\t" << num_replacements << "\t" << diff << "\t" << myToCalibrate << "\t" << veh_cnt << "\t" << meanSpeed << endl;

    		dev2->getOStream() << num_replacements << endl;

    		myIL->writeXMLOutput(*dev3, timestep-1, timestep);
    	}

    //	WRITE_WARNING("execute2(" + toString(timestep) + "): Count: " + toString(myIL->getNVehContributed(myInterval)));
     */

    return 1;
}
///TM

bool
MSCalibrator::MSCalibrator_FileTriggeredChild::processNextEntryReaderTriggered()
{
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
MSCalibrator::MSCalibrator_FileTriggeredChild::buildAndScheduleFlowVehicle(SUMOReal speed)
{
    SUMOVehicleParameter pars;
    pars.id = myParent.getID() + "_" + toString(myRunningID++);
    pars.depart = myOffset+1;
    pars.repetitionNumber = -1;
    pars.repetitionOffset = -1;
    MSVehicleType* aVehType = myVTypeDist.getOverallProb()>0
                              ? myVTypeDist.get()
                              : MSNet::getInstance()->getVehicleControl().getRandomVType();
    if (aVehType==0) {
        WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": no valid vehicle type exists.");
        WRITE_WARNING("Continuing with next element.");
        return;// false;
    }
    // check and assign vehicle type
    MSRoute *aEmitRoute = myRouteDist.get();
    if (aEmitRoute==0) {
        WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": no valid route exsists.");
        WRITE_WARNING("Continuing with next element.");
        return;// false;
    }

    MSVehicle *veh =
        MSNet::getInstance()->getVehicleControl().buildVehicle(pars, aEmitRoute, aVehType);
    myParent.schedule(this, veh, speed);
    myHaveNext = true;
}


void
MSCalibrator::MSCalibrator_FileTriggeredChild::myStartElement(SumoXMLTag element,
        const SUMOSAXAttributes &attrs) throw(ProcessError)
{
    if (element==SUMO_TAG_ROUTEDISTELEM) {
        // parse route distributino
        // check if route exists
        string routeStr = attrs.getStringSecure(SUMO_ATTR_ID, "");
        MSRoute* route = MSRoute::dictionary(routeStr);
        if (route == 0) {
            throw ProcessError(
                "MSTriggeredSource " + myParent.getID() + ": Route '" + routeStr + "' does not exist.");

        }
        // check frequency
        SUMOReal freq = attrs.getFloatSecure(SUMO_ATTR_PROB, -1);
        if (freq<0) {
            throw ProcessError(
                "MSTriggeredSource " + myParent.getID() + ": Attribute \"probability\" is negative (must not).");

        }
        // Attributes ok, add to routeDist
        myRouteDist.add(freq, route);
        return;
    }
    // vehicle-type distributions
    if (element==SUMO_TAG_VTYPEDISTELEM) {
        // get the id, report an error if not given or empty...
        string id;
        if(!attrs.setIDFromAttribues("vtypedistelem", id)) {
            return;
        }
        SUMOReal prob = -1;
        try {
            prob = attrs.getFloatSecure(SUMO_ATTR_PROB, -1);
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + myParent.getID() + "' (" + attrs.getStringSecure(SUMO_ATTR_PROB, "") + ").");
            return;
        }
        if (prob<=0) {
            MsgHandler::getErrorInstance()->inform("False probability while parsing calibrator '" + myParent.getID() + "' (" + toString(prob) + ").");
            return;
        }
        MSVehicleType *vtype = MSNet::getInstance()->getVehicleControl().getVType(id);
        if (vtype==0) {
            MsgHandler::getErrorInstance()->inform("Unknown vtype-object '" + id + "'.");
            return;
        }
        myVTypeDist.add(prob, vtype);
    }

    if (element==SUMO_TAG_FLOW) {
        // get the flow information
        SUMOReal no = -1;
        try {
            no = attrs.getFloatSecure(SUMO_ATTR_NO, -1);
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("Non-numeric flow in calibrator '" + myParent.getID() + "' (" + attrs.getStringSecure(SUMO_ATTR_NO, "") + ").");
            return;
        }
        if (no<0) {
            MsgHandler::getErrorInstance()->inform("Negative flow in calibrator '" + myParent.getID() + "' (" + attrs.getStringSecure(SUMO_ATTR_NO, "") + ").");
            return;
        }
        // get the end of this def
        SUMOTime end = -1;
        try {
            end = (SUMOTime) attrs.getFloatSecure(SUMO_ATTR_END, -1);
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("Non-numeric flow end in calibrator '" + myParent.getID() + "' (" + attrs.getStringSecure(SUMO_ATTR_NO, "") + ").");
            return;
        }

        myFlow = (SUMOReal) no;

        if (end==-1||end>=MSNet::getInstance()->getCurrentTimeStep()) {
            if (myFlow>0) {
                buildAndScheduleFlowVehicle();
                MSNet::getInstance()->getEmissionEvents().addEvent(
                    new WrappingCommand<MSCalibrator::MSCalibrator_FileTriggeredChild>(this, &MSCalibrator::MSCalibrator_FileTriggeredChild::execute),
                    (SUMOTime)(1. / (myFlow / 3600.))+MSNet::getInstance()->getCurrentTimeStep(),
                    MSEventControl::ADAPT_AFTER_EXECUTION);
                myHaveInitialisedFlow = true;
            }
        }
    }

    // check whethe the correct tag is read
    if (element==SUMO_TAG_EMIT) {
        SUMOVehicleParameter pars;
        pars.repetitionNumber = -1;
        pars.repetitionOffset = -1;
        // check and assign emission time
        pars.depart = attrs.getIntSecure(SUMO_ATTR_TIME, -1);
        if (pars.depart<myBeginTime) {
            // do not process the vehicle if the emission time is before the simulation begin
            return;
        }
        // check and assign id
        pars.id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (myVehicleControl.getVehicle(pars.id)!=0) {
            WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": Vehicle " + pars.id + " already exists.\n Generating a default id.");
            pars.id = "";
        }
        if (pars.id=="") {
            pars.id = myParent.getID() +  "_" + toString(pars.depart) +  "_" + toString(myRunningID++);
            if (myVehicleControl.getVehicle(pars.id)!=0) {
                WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": Vehicle " + pars.id + " already exists.\n Continuing with next element.");
                return;
            }
        }
        // check and assign vehicle type
        pars.vtypeid = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
        MSVehicleType* aVehType = MSNet::getInstance()->getVehicleControl().getVType(pars.vtypeid);
        if (aVehType == 0) {
            if (myVTypeDist.getOverallProb()!=0) {
                aVehType = myVTypeDist.get();
            }
            if (aVehType==0) {
                aVehType = MSNet::getInstance()->getVehicleControl().getRandomVType();
                if (aVehType==0) {
                    WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": no valid vehicle type exists.\n Continuing with next element.");
                    return;
                }
            }
        }
        // check and assign vehicle type
        pars.routeid = attrs.getStringSecure(SUMO_ATTR_ROUTE, "");
        MSRoute *aEmitRoute = MSRoute::dictionary(pars.routeid);
        if (aEmitRoute==0) {
            if (myRouteDist.getOverallProb()!=0) {
                aEmitRoute = myRouteDist.get();
            }
            if (aEmitRoute==0) {
                WRITE_WARNING("MSTriggeredSource " + myParent.getID()+ ": no valid route exsists.");
                WRITE_WARNING("Continuing with next element.");
                return;
            }
        }
        // build vehicle
        pars.departSpeed = attrs.getFloatSecure(SUMO_ATTR_SPEED, -1);
        MSVehicle *veh =
            MSNet::getInstance()->getVehicleControl().buildVehicle(pars, aEmitRoute, aVehType);
        myParent.schedule(this, veh, pars.departSpeed);
        myHaveNext = true;
        myOffset = SUMOTime(pars.depart);
    }
    // check whethe the correct tag is read
    if (element==SUMO_TAG_RESET) {
        myVTypeDist.clear();
        myRouteDist.clear();
    }
#if 0
#ifdef TM_CALIB
    if (element==SUMO_TAG_CALIB) {

        WRITE_WARNING("FOUND calib Tag!!!");
        /*
        		  MSNet::getInstance()->getEmissionEvents().addEvent(
                            new WrappingCommand<MSCalibrator::MSCalibrator_FileTriggeredChild>(this, &MSCalibrator::MSCalibrator_FileTriggeredChild::execute2),
                            //MSNet::getInstance()->getCurrentTimeStep() + 5,
        					10,
                            MSEventControl::ADAPT_AFTER_EXECUTION);
        */

    }
#endif //TM_CALIB
#endif //0
}


bool
MSCalibrator::MSCalibrator_FileTriggeredChild::nextRead()
{
    return myHaveNext;
}


SUMOReal
MSCalibrator::MSCalibrator_FileTriggeredChild::getLoadedFlow() const
{
    return myFlow;
}


void
MSCalibrator::MSCalibrator_FileTriggeredChild::inputEndReached()
{
    if (myFlow>0&&!myHaveInitialisedFlow) {
        buildAndScheduleFlowVehicle();
        MSNet::getInstance()->getEmissionEvents().addEvent(
            new WrappingCommand<MSCalibrator::MSCalibrator_FileTriggeredChild>(this, &MSCalibrator::MSCalibrator_FileTriggeredChild::execute),
            (SUMOTime)(1. / (myFlow / 3600.))+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        myHaveInitialisedFlow = true;
    }
}


// ===========================================================================
// method definitions
// ===========================================================================
MSCalibrator::MSCalibrator(const std::string &id,
                           MSNet &net,
                           MSLane* destLane, SUMOReal pos,
                           const std::string &aXMLFilename) throw()
        : MSTrigger(id), myNet(net),
        myDestLane(destLane), myPos((SUMOReal) pos), myDb(net),
        myDebugLevel(0), myDebugFilesBaseName("x:\\temp\\dbg_")
{
    assert(myPos>=0);

    myToCalibrate = 0;

    //myInterval=atof(getenv("TM_INTERV")?getenv("TM_INTERV"):"1"); //1
    //myNumVehicles=atof(getenv("TM_NUMVEH")?getenv("TM_NUMVEH"):"0"); //14.0
    myInterval = 60;
    myNumVehicles= -1;



    std::string ilId = "Calib_InductLoopOn_" + myDestLane->getID();
    myIL = myDb.createInductLoop(ilId, myDestLane, myPos);

    MSNet::getInstance()->getEmissionEvents().addEvent(
        new WrappingCommand<MSCalibrator>(this, &MSCalibrator::execute),
        //MSNet::getInstance()->getCurrentTimeStep() + 5,
        0,
        MSEventControl::ADAPT_AFTER_EXECUTION);


    //TODO clean up in destructor!!!
    MSCalibrator::calibratorMap[id]=this;

    myActiveChild =
        new MSCalibrator_FileTriggeredChild(net, aXMLFilename, *this, net.getVehicleControl());
    myFileBasedCalibrator = myActiveChild;
}


MSCalibrator::~MSCalibrator() throw()
{
    {
        delete myFileBasedCalibrator;
    }
    {
        std::map<MSCalibratorChild*, std::pair<MSVehicle*, SUMOReal> >::iterator i;
        for (i=myToEmit.begin(); i!=myToEmit.end(); ++i) {
            delete(*i).second.first;
        }
    }
}


bool
MSCalibrator::childCheckEmit(MSCalibratorChild *child)
{
    if (myToEmit.find(child)==myToEmit.end()) {
        // should not happen - a child is calling and should have a vehicle added
        throw 1;
    }
    if (child!=myActiveChild||myDestLane->getEdge()->isVaporizing()) {
        // check whether this is due to vaporization
        if(myDestLane->getEdge()->isVaporizing()) {
            myToEmit[child].first->setWasVaporized(true);
        }
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
    //TM
    SUMOReal pos = myPos+1;
    if (speed<0) {
        speed = MIN2(myDestLane->maxSpeed(), veh->getMaxSpeed());
    }
    // try to emit
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        if (myDestLane->getEdge()->emit(*veh,  myNet.getCurrentTimeStep())) {
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
        if (myDestLane->isEmissionSuccess(veh, speed, pos, false)) {
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
MSCalibrator::schedule(MSCalibratorChild *child,
                       MSVehicle *v, SUMOReal speed)
{
    myToEmit[child] = make_pair(v, speed);
}


size_t
MSCalibrator::getActiveChildIndex() const
{
    return
        myFileBasedCalibrator==myActiveChild ? 0 : 1;
}


void
MSCalibrator::setActiveChild(MSCalibratorChild *c)
{
    myActiveChild = c;
}

std::map<std::string, MSCalibrator*> MSCalibrator::calibratorMap;

void
MSCalibrator::updateCalibrator(std::string name, int time, SUMOReal count)
{

    std::map<std::string, MSCalibrator*>::iterator it = MSCalibrator::calibratorMap.find(name);

    if (it==MSCalibrator::calibratorMap.end()) {
        WRITE_ERROR("WRONG UPDATE COMMAND, CALIBRATOR NOT FOUND: " + name);
        return;
    }

    MSCalibrator * calibrator = (*it).second;

    calibrator->myInterval = time;
    calibrator->myNumVehicles = count;

    //DOME
    int i=0;


}


/****************************************************************************/

