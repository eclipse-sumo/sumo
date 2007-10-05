/****************************************************************************/
/// @file    MSNet.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// object. Holds all necessary objects for
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

#ifdef _SPEEDCHECK
#include <ctime>
#endif

#include <iostream>
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include <cassert>
#include <vector>
#include <sstream>
#include <utils/common/UtilExceptions.h>
#include "MSNet.h"
#include "MSEdgeControl.h"
#include "MSJunctionControl.h"
#include "MSEmitControl.h"
#include "MSEventControl.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSJunctionLogic.h"
#include "MSLane.h"
#include "MSVehicleTransfer.h"
#include "MSRoute.h"
#include "MSRouteLoaderControl.h"
#include "traffic_lights/MSTLLogicControl.h"
#include "MSVehicleControl.h"
#include "trigger/MSTrigger.h"
#include "MSCORN.h"
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSVehicleTransfer.h>
#include "traffic_lights/MSTrafficLightLogic.h"
#include <microsim/output/MSDetectorHaltingContainerWrapper.h>
#include <microsim/output/MSTDDetectorInterface.h>
#include <microsim/output/MSDetectorOccupancyCorrection.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/shapes/ShapeContainer.h>
#include "output/meandata/MSMeanData_Net.h"
#include "output/MSXMLRawOut.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/SysUtils.h>
#include "trigger/MSTriggerControl.h"
#include "MSGlobals.h"
#include "MSSaveState.h"
#include "MSDebugHelper.h"
#include "MSRouteHandler.h"
#include "MSRouteLoader.h"
#include "MSBuildCells.h"
#include <utils/geom/GeoConvHelper.h>
#include <ctime>
#include "MSPerson.h"

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#endif


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
MSNet* MSNet::myInstance = 0;
SUMOReal MSNet::myDeltaT = 1;


// ===========================================================================
// member method definitions
// ===========================================================================
MSNet*
MSNet::getInstance(void)
{
    if (myInstance != 0) {
        return myInstance;
    }
    assert(false);
    return 0;
}


MSNet::MSNet(SUMOTime startTimeStep, MSVehicleControl *vc,
             SUMOReal tooSlowRTF, bool logExecTime, bool logStep)
        : myLogExecutionTime(logExecTime), myLogStepNumber(logStep),
          myTooSlowRTF(tooSlowRTF)
{
    MSCORN::init();
    MSVehicleTransfer::setInstance(new MSVehicleTransfer());
    myStep = startTimeStep;
    myEmitter = new MSEmitControl(*vc);
    myVehicleControl = vc;
    myDetectorControl = new MSDetectorControl();
    myEdges = 0;
    myJunctions = 0;
    myRouteLoaders = 0;
    myLogics = 0;
    myCellsBuilder = 0;
    myPersonControl = 0;
    myMSPhoneNet = 0;
    myTriggerControl = new MSTriggerControl();
    myShapeContainer = new ShapeContainer();
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gMesoNet = new MELoop();
    }
#endif
    myInstance = this;
}




void
MSNet::closeBuilding(MSEdgeControl *edges, MSJunctionControl *junctions,
                     MSRouteLoaderControl *routeLoaders,
                     MSTLLogicControl *tlc, // ShapeContainer *sc,
                     const MSMeanData_Net_Cont &meanData,
                     vector<int> stateDumpTimes,
                     std::string stateDumpFiles)
{
    myEdges = edges;
    myJunctions = junctions;
    myRouteLoaders = routeLoaders;
    myLogics = tlc;
    myMSPhoneNet = new MSPhoneNet();
    // intialise outputs
    myMeanData = meanData;
    // tol
    if (OptionsCont::getOptions().isSet("ss2-cell-output")||OptionsCont::getOptions().isSet("ss2-sql-cell-output")) {
        // start old-data removal through MSEventControl
        Command* writeDate = new WrappingCommand< MSPhoneNet >(
                                 myMSPhoneNet, &MSPhoneNet::writeCellOutput);
        getEndOfTimestepEvents().addEvent(
            writeDate, (myStep)%300+300, MSEventControl::NO_CHANGE);
    }
    if (OptionsCont::getOptions().isSet("ss2-la-output")||OptionsCont::getOptions().isSet("ss2-sql-la-output")) {
        // start old-data removal through MSEventControl
        Command* writeDate = new WrappingCommand< MSPhoneNet >(
                                 myMSPhoneNet, &MSPhoneNet::writeLAOutput);
        getEndOfTimestepEvents().addEvent(
            writeDate, (myStep)%300+300, MSEventControl::NO_CHANGE);
    }
    //

    // we may add it before the network is loaded
    if (myEdges!=0) {
        myEdges->insertMeanData(myMeanData.size());
        // check whether the c2c is used
        if (MSGlobals::gUsingC2C) {
            // build the speed-up grid
            myCellsBuilder = new MSBuildCells(*this, GeoConvHelper::getConvBoundary());
            myCellsBuilder->build();
            // print some debug stuff if wished
            if (OptionsCont::getOptions().isSet("c2x.edge-near-info")) {
                myCellsBuilder->writeNearEdges(OutputDevice::getDeviceByOption("c2x.edge-near-info"));
            }
        }
    }
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gMesoNet->insertMeanData(myMeanData.size());
    }
#endif

    // save the time the network state shall be saved at
    myStateDumpTimes = stateDumpTimes;
    myStateDumpFiles = stateDumpFiles;

    // set requests/responses
    myJunctions->postloadInitContainer();
}


MSNet::~MSNet()
{
    // delete controls
    delete myJunctions;
    delete myDetectorControl;
    // delete mean data
    for (MSMeanData_Net_Cont::iterator i1=myMeanData.begin(); i1!=myMeanData.end(); ++i1) {
        delete *i1;
    }
    myMeanData.clear();
    delete myEdges;
    delete myEmitter;
    delete myLogics;
    delete myRouteLoaders;
    delete myVehicleControl;
    delete myMSPhoneNet;
    myMSPhoneNet = 0;
    delete myShapeContainer;
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        delete MSGlobals::gMesoNet;
    }
#endif
    delete myTriggerControl;
    delete myCellsBuilder;
    clearAll();
    GeoConvHelper::close();
    OutputDevice::closeAll();
}


bool
MSNet::simulate(SUMOTime start, SUMOTime stop)
{
    initialiseSimulation();
    // the simulation loop
    bool tooSlow = false;
    myStep = start;
    try {
        do {
            if (myLogStepNumber) {
                preSimStepOutput();
            }
            simulationStep(start, myStep);
            if (myLogStepNumber) {
                postSimStepOutput();
            }
            myStep += DELTA_T;
            if (myLogExecutionTime && myTooSlowRTF>0) {
                SUMOReal rtf = ((SUMOReal) 1000./ (SUMOReal) mySimStepDuration);
                if (rtf<myTooSlowRTF) {
                    tooSlow = true;
                }
            }
        } while (myStep<=stop && !myVehicleControl->haveAllVehiclesQuit() && !tooSlow);
        if (tooSlow) {
            WRITE_MESSAGE("Simulation End: The simulation got too slow.");
        } else if (myStep>stop) {
            WRITE_MESSAGE("Simulation End: The final simulation step has been reached.");
        } else {
            WRITE_MESSAGE("Simulation End: All vehicles have left the simulation.");
        }
    } catch (ProcessError &e) {
//!!!        WRITE_MESSAGE("Simulation End: An error occured (see log).");
        throw e;
    }
    // exit simulation loop
    closeSimulation(start, stop);
    return true;
}


void
MSNet::initialiseSimulation()
{
    if (OptionsCont::getOptions().isSet("emissions-output")) {
        MSCORN::setWished(MSCORN::CORN_OUT_EMISSIONS);
    }
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        MSCORN::setWished(MSCORN::CORN_OUT_TRIPDURATIONS);
    }
    if (OptionsCont::getOptions().isSet("vehroute-output")) {
        MSCORN::setWished(MSCORN::CORN_OUT_VEHROUTES);
    }

    //car2car
    if (OptionsCont::getOptions().isSet("c2x.cluster-info")) {
        MSCORN::setWished(MSCORN::CORN_OUT_CLUSTER_INFO);
    }
    if (OptionsCont::getOptions().isSet("c2x.saved-info")) {
        MSCORN::setWished(MSCORN::CORN_OUT_SAVED_INFO);
    }
    if (OptionsCont::getOptions().isSet("c2x.saved-info-freq")) {
        MSCORN::setWished(MSCORN::CORN_OUT_SAVED_INFO_FREQ);
    }
    if (OptionsCont::getOptions().isSet("c2x.transmitted-info")) {
        MSCORN::setWished(MSCORN::CORN_OUT_TRANS_INFO);
    }
    if (OptionsCont::getOptions().isSet("c2x.vehicle-in-range")) {
        MSCORN::setWished(MSCORN::CORN_OUT_VEH_IN_RANGE);
    }
}


void
MSNet::closeSimulation(SUMOTime start, SUMOTime stop)
{
    if (OptionsCont::getOptions().isSet("ss2-sql-output")) {
        OutputDevice::getDeviceByOption("ss2-sql-output") << ";\n";
    }
    if (myLogExecutionTime!=0&&mySimDuration!=0) {
        ostringstream msg;
        msg << "Performance: " << "\n"
        << " Duration: " << mySimDuration << "ms" << "\n"
        << " Real time factor: " << ((SUMOReal)(stop-start)*1000./(SUMOReal)mySimDuration) << "\n"
        << " UPS: " << ((SUMOReal) myVehiclesMoved / (SUMOReal) mySimDuration * 1000.) << "\n";
        WRITE_MESSAGE(msg.str());
    }
}


void
MSNet::simulationStep(SUMOTime /*start*/, SUMOTime step)
{
    myStep = step;
    debug_globaltime = step;
    // execute beginOfTimestepEvents
    if (myLogExecutionTime) {
        mySimStepBegin = SysUtils::getCurrentMillis();
    }
    myBeginOfTimestepEvents.execute(myStep);

    // load routes
    myEmitter->moveFrom(myRouteLoaders->loadNext(step));
    // emit Vehicles
    size_t emittedVehNo = myEmitter->emitVehicles(myStep);
    myVehicleControl->vehiclesEmitted(emittedVehNo);
    if (MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions(step);
    }
    MSVehicleTransfer::getInstance()->checkEmissions(myStep);

    if (myMSPhoneNet!=0) {
        myMSPhoneNet->setDynamicCalls(myStep);
    }

    if (MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions(step);
    }
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gMesoNet->simulate(step);
    } else {
#endif
        myJunctions->resetRequests();

        // move Vehicles
        // move vehicles which do not interact with the lane end
        myEdges->moveNonCritical();
        // precompute possible positions for vehicles that do interact with
        // their lane's end
        myEdges->moveCritical();

        // set information about which vehicles may drive at all
        myLogics->maskRedLinks();
        // check the right-of-way for all junctions
        myJunctions->setAllowed();
        // set information which vehicles should decelerate
        myLogics->maskYellowLinks();

        // move vehicles which do interact with their lane's end
        //  (it is now known whether they may drive
        myEdges->moveFirst();
        if (MSGlobals::gCheck4Accidents) {
            myEdges->detectCollisions(step);
        }

        MSUpdateEachTimestepContainer< DetectorContainer::UpdateHaltings >::getInstance()->updateAll();
        MSUpdateEachTimestepContainer< MSE3Collector >::getInstance()->updateAll();
        MSUpdateEachTimestepContainer< Detector::UpdateE2Detectors >::getInstance()->updateAll();
        MSUpdateEachTimestepContainer< Detector::UpdateOccupancyCorrections >::getInstance()->updateAll();

        // Vehicles change Lanes (maybe)
        myEdges->changeLanes();

        // check whether the tls shall be switched
        myLogics->check2Switch(myStep);

        if (MSGlobals::gCheck4Accidents) {
            myEdges->detectCollisions(step);
        }
#ifdef HAVE_MESOSIM
    }
#endif
    // Check if mean-lane-data is due
    writeOutput();
    // execute endOfTimestepEvents
    myEndOfTimestepEvents.execute(myStep);
    if (MSGlobals::gUsingC2C) {
        computeCar2Car();
    }

    // persons
    if (myPersonControl!=0) {
        if (myPersonControl->hasWaitingPersons(myStep)) {
            const MSPersonControl::PersonVector &persons = myPersonControl->getWaitingPersons(myStep);
            for (MSPersonControl::PersonVector::const_iterator i=persons.begin(); i!=persons.end(); ++i) {
                MSPerson *person = *i;
                if (person->endReached()) {
                    delete person;
                } else {
                    person->proceed(this, myStep);
                }
            }
        }
    }

    // check state dumps
    if (find(myStateDumpTimes.begin(), myStateDumpTimes.end(), myStep)!=myStateDumpTimes.end()) {
        string name = myStateDumpFiles + '_' + toString(myStep) + ".bin";
        ofstream strm(name.c_str(), fstream::out|fstream::binary);
        saveState(strm, (long) 0xffffffff);
    }

    if (myLogExecutionTime) {
        mySimStepEnd = SysUtils::getCurrentMillis();
        mySimStepDuration = mySimStepEnd - mySimStepBegin;
        mySimDuration += mySimStepDuration;
        myVehiclesMoved += myVehicleControl->getRunningVehicleNo();
    }
}

// Compute Car2Car-Communication
void
MSNet::computeCar2Car(void)
{
    if (myAllEdges.size()==0) {
        myAllEdges = myEdges->getMultiLaneEdges();
        const std::vector<MSEdge*> &add = myEdges->getSingleLaneEdges();
        copy(add.begin(), add.end(), back_inserter(myAllEdges));
    }
    myConnected.clear();
    myClusterHeaders.clear();

    for (std::vector<MSEdge*>::iterator i=myAllEdges.begin(); i!=myAllEdges.end(); ++i) {
        MSEdge *e = *i;
        const MSEdge::DictTypeVeh &eEquipped = e->getEquippedVehs();
        if (eEquipped.size()>0) {
            std::map<std::string, MSVehicle*>::const_iterator k = eEquipped.begin();
            // above all Equipped vehicle of this Edge
            for (; k!=eEquipped.end(); ++k) {
                // update own information
                // a) insert the current edge if the vehicle is standing for a long period
                // b) remove information older then a specified amount of time (MSGlobals::gLANRefuseOldInfosOffset)
                (*k).second->updateInfos(myStep);

                // go through the neighbors of this vehicle's edge
                const std::vector<MSEdge*> &neighborEdges = e->getNeighborEdges();
                for (std::vector<MSEdge*>::const_iterator l=neighborEdges.begin(); l!=neighborEdges.end(); ++l) {
                    const MSEdge::DictTypeVeh &nEquipped = (*l)->getEquippedVehs();
                    if (nEquipped.size()>0) {
                        std::map<std::string, MSVehicle*>::const_iterator m = nEquipped.begin();
                        // go through all vehicles on neighbor edge
                        for (; m!=nEquipped.end(); ++m) {
                            if ((*k).second != (*m).second) {
                                // update connection state
                                (*k).second->addVehNeighbors((*m).second, myStep);
                            }
                        }
                    }
                }

                // remove connections to vehicles which are no longer in range
                (*k).second->cleanUpConnections(myStep);

                // ...reset the cluster id
                (*k).second->setClusterId(-1);
                // for each vehicle with communication partners...
                if ((*k).second->getConnections().size()!=0) {
                    // ...add the vehicle to list of connected vehicles
                    myConnected.push_back((*k).second);
                }
            }
        }
    }

    // build the clusters
    {
        int clusterId = 1;
        std::vector<MSVehicle*>::iterator q1;//, q1, q2;
        for (q1=myConnected.begin(); q1!=myConnected.end(); q1++) {
            if ((*q1)->getClusterId()<0) {
                /*
                q = q1;
                for(q2=myConnected.begin(); q2!=connected.end(); q2++) {
                    int size1 = (*q1)->getConnections().size();
                    int size2 = (*q2)->getConnections().size();
                if((*q2)->getClusterId() < 0 && size1 < size2){
                q = q2;
                }
                }
                */
                myClusterHeaders.push_back(*q1);
                (*q1)->buildMyCluster(myStep, clusterId);
                clusterId++;
            }
        }
    }

    // send information
    {
        std::vector<MSVehicle*>::iterator q;
        for (q= myClusterHeaders.begin();q!=myClusterHeaders.end();q++){
            (*q)->sendInfos(myStep);
        }
    }

    // Rerouting?
    {
        std::vector<MSVehicle*>::iterator q1;
        for (q1 = myConnected.begin();q1!=myConnected.end();q1++){
            (*q1)->checkReroute(myStep);
        }
    }
}


void
MSNet::clearAll()
{
    // clear container
    MSEdge::clear();
    MSLane::clear();
    MSRoute::clear();
    delete MSVehicleTransfer::getInstance();
}


unsigned
MSNet::getNDumpIntervalls(void)
{
    return myMeanData.size();
}


SUMOTime
MSNet::getCurrentTimeStep() const
{
    return myStep;
}


MSVehicleControl &
MSNet::getVehicleControl() const
{
    return *myVehicleControl;
}


void
MSNet::writeOutput()
{
    // netstate output.
    if (OptionsCont::getOptions().isSet("netstate-dump")) {
        MSXMLRawOut::write(OutputDevice::getDeviceByOption("netstate-dump"), *myEdges, myStep, 3);
    }
    // emission output
    if (OptionsCont::getOptions().isSet("emissions-output")) {
        OutputDevice::getDeviceByOption("emissions-output")
        << "    <emission-state time=\"" << myStep << "\" "
        << "loaded=\"" << myVehicleControl->getLoadedVehicleNo() << "\" "
        << "emitted=\"" << myVehicleControl->getEmittedVehicleNo() << "\" "
        << "running=\"" << myVehicleControl->getRunningVehicleNo() << "\" "
        << "waiting=\"" << myEmitter->getWaitingVehicleNo() << "\" "
        << "ended=\"" << myVehicleControl->getEndedVehicleNo() << "\" "
        << "meanWaitingTime=\"" << myVehicleControl->getMeanWaitingTime() << "\" "
        << "meanTravelTime=\"" << myVehicleControl->getMeanTravelTime() << "\" ";
        if (myLogExecutionTime) {
            OutputDevice::getDeviceByOption("emissions-output")
            << "duration=\"" << mySimStepDuration << "\" ";
        }
        OutputDevice::getDeviceByOption("emissions-output") << "/>\n";
    }
}


bool
MSNet::haveAllVehiclesQuit()
{
    return myVehicleControl->haveAllVehiclesQuit();
}


void
MSNet::addMeanData(MSMeanData_Net *newMeanData)
{
    myMeanData.push_back(newMeanData);
    // we may add it before the network is loaded
    if (myEdges!=0) {
        myEdges->insertMeanData(1);
    }
}


size_t
MSNet::getMeanDataSize() const
{
    return myMeanData.size();
}


MSEdgeControl &
MSNet::getEdgeControl()
{
    return *myEdges;
}


MSDetectorControl &
MSNet::getDetectorControl()
{
    return *myDetectorControl;
}


MSTriggerControl &
MSNet::getTriggerControl()
{
    return *myTriggerControl;
}


MSTLLogicControl &
MSNet::getTLSControl()
{
    return *myLogics;
}

MSJunctionControl &
MSNet::getJunctionControl()
{
    return *myJunctions;
}


long
MSNet::getSimStepDurationInMillis() const
{
    return mySimStepDuration;
}


void
MSNet::saveState(std::ostream &os, long what)
{
    myVehicleControl->saveState(os, what);
    myEdges->saveState(os, what);
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gMesoNet->saveState(os, what);
//        if((what&(long) SAVESTATE_EDGES)!=0) myEdges->saveState(os);
    } else {
#endif
        /*
        if((what&(long) SAVESTATE_EDGES)!=0) myEdges->saveState(os);
        if((what&(long) SAVESTATE_EMITTER)!=0) myEmitter->saveState(os);
        if((what&(long) SAVESTATE_LOGICS)!=0) myLogics->saveState(os);
        if((what&(long) SAVESTATE_ROUTES)!=0) myRouteLoaders->saveState(os);
        if((what&(long) SAVESTATE_VEHICLES)!=0) myVehicleControl->saveState(os);
        */
#ifdef HAVE_MESOSIM
    }
#endif
}


void
MSNet::loadState(BinaryInputDevice &bis, long what)
{
    myVehicleControl->loadState(bis, what);
    myEdges->loadState(bis, what);
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MSGlobals::gMesoNet->loadState(bis, what, *myVehicleControl);
//        if((what&(long) SAVESTATE_EDGES)!=0) myEdges->saveState(os);
    } else {
#endif
        /*
        if((what&(long) SAVESTATE_EDGES)!=0) myEdges->saveState(os);
        if((what&(long) SAVESTATE_EMITTER)!=0) myEmitter->saveState(os);
        if((what&(long) SAVESTATE_LOGICS)!=0) myLogics->saveState(os);
        if((what&(long) SAVESTATE_ROUTES)!=0) myRouteLoaders->saveState(os);
        if((what&(long) SAVESTATE_VEHICLES)!=0) myVehicleControl->saveState(os);
        */
#ifdef HAVE_MESOSIM
    }
#endif
}


MSRouteLoader *
MSNet::buildRouteLoader(const std::string &file, int incDUABase, int incDUAStage)
{
    // return a new build route loader
    //  the handler is
    //  a) not adding the vehicles directly
    //  b) not using colors
    // (overridden in GUINet)
    return new MSRouteLoader(*this, new MSRouteHandler(file, *myVehicleControl, false, false, incDUABase, incDUAStage));
}


SUMOReal
MSNet::getTooSlowRTF() const
{
    return myTooSlowRTF;
}


MSPersonControl &
MSNet::getPersonControl() const
{
    if (myPersonControl==0) {
        myPersonControl = new MSPersonControl();
    }
    return *myPersonControl;
}


void 
MSNet::preSimStepOutput() const
{
    cout << std::setprecision(OUTPUT_ACCURACY);
    cout << "Step #" << myStep;
    if (!myLogExecutionTime) {
        cout << (char) 13;
    }
}


void 
MSNet::postSimStepOutput() const
{
    if (myLogExecutionTime) {
        if (mySimStepDuration!=0) {
            ostringstream oss;
            oss.setf(std::ios::fixed , std::ios::floatfield);    // use decimal format
            oss.setf(std::ios::showpoint);    // print decimal point
            oss << std::setprecision(OUTPUT_ACCURACY);
            oss << " (" << mySimStepDuration << "ms ~= "
            << (1000./ (SUMOReal) mySimStepDuration) << "*RT, ~"
            << ((SUMOReal) myVehicleControl->getRunningVehicleNo()/(SUMOReal) mySimStepDuration*1000.)
            << "UPS, vehicles"
            << " TOT " << myVehicleControl->getEmittedVehicleNo()
            << " ACT " << myVehicleControl->getRunningVehicleNo()
            << ")       "
            << (char) 13;
            string msg = oss.str().substr(0, 79);
            cout << msg;
        } else {
            cout << " (0ms; no further information available)          \r";
        }
    }
}


/****************************************************************************/

