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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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
#include <utils/geoconv/GeoConvHelper.h>
#include <ctime>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member defintions
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


MSNet::MSNet(SUMOTime startTimeStep, SUMOTime /*stopTimeStep*/,
             SUMOReal tooSlowRTF, bool logExecTime)
        : myLogExecutionTime(logExecTime), myTooSlowRTF(tooSlowRTF)
{
    MSCORN::init();
    MSVehicleTransfer::setInstance(new MSVehicleTransfer());
    myStep = startTimeStep;
    myVehicleControl = new MSVehicleControl();
    myEmitter = new MSEmitControl(*myVehicleControl);
    myDetectorControl = new MSDetectorControl();
    myEdges = 0;
    myJunctions = 0;
    myRouteLoaders = 0;
    myLogics = 0;
    myCellsBuilder = 0;
    myTriggerControl = new MSTriggerControl();
    myShapeContainer = new ShapeContainer();
    myMSPhoneNet = new MSPhoneNet();

    myInstance = this;
}



MSNet::MSNet(SUMOTime startTimeStep, SUMOTime /*stopTimeStep*/,
             MSVehicleControl *vc,
             SUMOReal tooSlowRTF, bool logExecTime)
        : myLogExecutionTime(logExecTime), myTooSlowRTF(tooSlowRTF)
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
    myTriggerControl = new MSTriggerControl();
    myShapeContainer = new ShapeContainer();
    myMSPhoneNet = new MSPhoneNet();

    myInstance = this;
}




void
MSNet::closeBuilding(MSEdgeControl *edges, MSJunctionControl *junctions,
                     MSRouteLoaderControl *routeLoaders,
                     MSTLLogicControl *tlc, // ShapeContainer *sc,
                     std::vector<OutputDevice*> streams,
                     const MSMeanData_Net_Cont &meanData,
                     TimeVector stateDumpTimes,
                     std::string stateDumpFiles)
{
    myEdges = edges;
    myJunctions = junctions;
    myRouteLoaders = routeLoaders;
    myLogics = tlc;
    MSCORN::setTripDurationsOutput(streams[OS_TRIPDURATIONS]);
    MSCORN::setVehicleRouteOutput(streams[OS_VEHROUTE]);
    MSCORN::setVehicleDeviceTOSS2Output(streams[OS_DEVICE_TO_SS2]);
    MSCORN::setCellTOSS2Output(streams[OS_CELL_TO_SS2]);
    MSCORN::setLATOSS2Output(streams[OS_LA_TO_SS2]);
    MSCORN::setVehicleDeviceTOSS2SQLOutput(streams[OS_DEVICE_TO_SS2_SQL]);
    MSCORN::setCellTOSS2SQLOutput(streams[OS_CELL_TO_SS2_SQL]);
    MSCORN::setLATOSS2SQLOutput(streams[OS_LA_TO_SS2_SQL]);
    MSCORN::setCELLPHONEDUMPOutput(streams[OS_CELLPHONE_DUMP_TO]);
    //car2car
    MSCORN::setClusterInfoOutput(streams[OS_CLUSTER_INFO]);
    MSCORN::setEdgeNearInfoOutput(streams[OS_EDGE_NEAR]);
    MSCORN::setSavedInfoOutput(streams[OS_SAVED_INFO]);
    MSCORN::setSavedInfoOutputFreq(streams[OS_SAVED_INFO_FREQ]);
    MSCORN::setTransmittedInfoOutput(streams[OS_TRANS_INFO]);
    MSCORN::setVehicleInRangeOutput(streams[OS_VEH_IN_RANGE]);

    myOutputStreams = streams;
    myMeanData = meanData;

    // we may add it before the network is loaded
    if (myEdges!=0) {
        myEdges->insertMeanData(myMeanData.size());
        if (MSGlobals::gUsingC2C) {
            myCellsBuilder = new MSBuildCells(*this, myConvBoundary); //Danilo
            myCellsBuilder->build();
        }
    }

    // save the time the network state shall be saved at
    myStateDumpTimes = stateDumpTimes;
    myStateDumpFiles = stateDumpFiles;

    // set requests/responses
    MSJunction::postloadInitContainer(); // !!!
    GeoConvHelper::init(myOrigProj, myOffset);
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
    delete myTriggerControl;
    delete myCellsBuilder;
    // close outputs
    for (size_t i2=0; i2<OS_MAX&&i2<myOutputStreams.size(); i2++) {
        delete myOutputStreams[i2];
    }

    clearAll();
    GeoConvHelper::close();
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
            preSimStepOutput();
            simulationStep(start, myStep);
            postSimStepOutput();
            myStep++;
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
    } catch (ProcessError &) {
        WRITE_MESSAGE("Simulation End: An error occured (see log).");
    }
    // exit simulation loop
    closeSimulation(start, stop);
    return true;
}


void
MSNet::initialiseSimulation()
{
    // prepare the "netstate" output and print the first line
    if (myOutputStreams[OS_NETSTATE]!=0) {
        myOutputStreams[OS_NETSTATE]->getOStream()
        << setprecision(3) << setiosflags(ios::fixed)
        << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
        << "<sumo-netstate>" << endl;
    }
    // ... the same for the vehicle emission state
    if (myOutputStreams[OS_EMISSIONS]!=0) {
        myOutputStreams[OS_EMISSIONS]->getOStream()
        << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
        << "<emissions>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_EMISSIONS);
    }
    // ... the same for the vehicle trip durations
    if (myOutputStreams[OS_TRIPDURATIONS]!=0) {
        myOutputStreams[OS_TRIPDURATIONS]->getOStream()
        << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
        << "<tripinfos>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_TRIPDURATIONS);
    }
    // ... the same for the vehicle route information
    if (myOutputStreams[OS_VEHROUTE]!=0) {
        myOutputStreams[OS_VEHROUTE]->getOStream()
        << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl
        << "<vehicleroutes>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_VEHROUTES);
    }
    // ... the same for TrafficOnline-SS2 information
    if (myOutputStreams[OS_DEVICE_TO_SS2]!=0) {
        MSCORN::setWished(MSCORN::CORN_OUT_DEVICE_TO_SS2);
    }
    if (myOutputStreams[OS_CELL_TO_SS2]!=0) {
        MSCORN::setWished(MSCORN::CORN_OUT_CELL_TO_SS2);
    }
    if (myOutputStreams[OS_LA_TO_SS2]!=0) {
        MSCORN::setWished(MSCORN::CORN_OUT_LA_TO_SS2);
    }
    // ... the same for TrafficOnline-SS2-SQL information
    if (myOutputStreams[OS_DEVICE_TO_SS2_SQL]!=0) {
        MSCORN::setWished(MSCORN::CORN_OUT_DEVICE_TO_SS2_SQL);
    }
    if (myOutputStreams[OS_CELL_TO_SS2_SQL]!=0) {
        MSCORN::setWished(MSCORN::CORN_OUT_CELL_TO_SS2_SQL);
    }
    if (myOutputStreams[OS_LA_TO_SS2_SQL]!=0) {
        MSCORN::setWished(MSCORN::CORN_OUT_LA_TO_SS2_SQL);
    }
    // ... the same for TrafficOnline cellphone-dump
    if (myOutputStreams[OS_CELLPHONE_DUMP_TO] != 0) {
        MSCORN::setWished(MSCORN::CORN_OUT_CELLPHONE_DUMP_TO);
    }
    //car2car
    if (myOutputStreams[OS_CLUSTER_INFO]!=0) {
        myOutputStreams[OS_CLUSTER_INFO]->getOStream()
        << "<?xml version=\"1.0\" standalone=\"no\"?>\n" << endl
        << "<clusterInfos>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_CLUSTER_INFO);
    }
    if (myOutputStreams[OS_EDGE_NEAR]!=0) {
        myOutputStreams[OS_EDGE_NEAR]->getOStream()
        << "<?xml version=\"1.0\" standalone=\"no\"?>\n" << endl
        << "<edgeNears>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_EDGE_NEAR);
    }
    if (myOutputStreams[OS_SAVED_INFO]!=0) {
        myOutputStreams[OS_SAVED_INFO]->getOStream()
        << "<?xml version=\"1.0\" standalone=\"no\"?>\n" << endl
        << "<savedInfos>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_SAVED_INFO);
    }
    if (myOutputStreams[OS_SAVED_INFO_FREQ]!=0) {
        myOutputStreams[OS_SAVED_INFO_FREQ]->getOStream()
        << "<?xml version=\"1.0\" standalone=\"no\"?>\n" << endl
        << "<savedInfosFreq>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_SAVED_INFO_FREQ);
    }
    if (myOutputStreams[OS_TRANS_INFO]!=0) {
        myOutputStreams[OS_TRANS_INFO]->getOStream()
        << "<?xml version=\"1.0\" standalone=\"no\"?>\n" << endl
        << "<transmittedInfos>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_TRANS_INFO);
    }
    if (myOutputStreams[OS_VEH_IN_RANGE]!=0) {
        myOutputStreams[OS_VEH_IN_RANGE]->getOStream()
        << "<?xml version=\"1.0\" standalone=\"no\"?>\n" << endl
        << "<vehicleInRanges>" << endl;
        MSCORN::setWished(MSCORN::CORN_OUT_VEH_IN_RANGE);
    }
}


void
MSNet::closeSimulation(SUMOTime start, SUMOTime stop)
{
    // print the last line of the "netstate" output
    if (myOutputStreams[OS_NETSTATE]!=0) {
        myOutputStreams[OS_NETSTATE]->getOStream() << "</sumo-netstate>" << endl;
    }
    // ... the same for the vehicle emission state
    if (myOutputStreams[OS_EMISSIONS]!=0) {
        myOutputStreams[OS_EMISSIONS]->getOStream() << "</emissions>" << endl;
    }
    // ... the same for the vehicle trip information
    if (myOutputStreams[OS_TRIPDURATIONS]!=0) {
        myOutputStreams[OS_TRIPDURATIONS]->getOStream() << "</tripinfos>" << endl;
    }
    // ... the same for the vehicle trip information
    if (myOutputStreams[OS_VEHROUTE]!=0) {
        myOutputStreams[OS_VEHROUTE]->getOStream() << "</vehicleroutes>" << endl;
    }
    // ... the same for the OS_CELL_TO_SS2_SQL
    if (myOutputStreams[OS_CELL_TO_SS2_SQL]!=0) {
        myOutputStreams[OS_CELL_TO_SS2_SQL]->getOStream() << ";" << endl;
    }
    if (myOutputStreams[OS_DEVICE_TO_SS2_SQL]!=0) {
        myOutputStreams[OS_DEVICE_TO_SS2_SQL]->getOStream() << ";" << endl;
    }
    if (myOutputStreams[OS_LA_TO_SS2_SQL]!=0) {
        myOutputStreams[OS_LA_TO_SS2_SQL]->getOStream() << ";" << endl;
    }
    //car2car
    if (myOutputStreams[OS_CLUSTER_INFO]!=0) {
        myOutputStreams[OS_CLUSTER_INFO]->getOStream() << "</clusterInfos>" << endl;
    }
    if (myOutputStreams[OS_EDGE_NEAR]!=0) {
        myOutputStreams[OS_EDGE_NEAR]->getOStream() << "</edgeNears>" << endl;
    }
    if (myOutputStreams[OS_SAVED_INFO]!=0) {
        myOutputStreams[OS_SAVED_INFO]->getOStream() << "</savedInfos>" << endl;
    }
    if (myOutputStreams[OS_SAVED_INFO_FREQ]!=0) {
        myOutputStreams[OS_SAVED_INFO_FREQ]->getOStream() << "</savedInfosFreq>" << endl;
    }
    if (myOutputStreams[OS_TRANS_INFO]!=0) {
        myOutputStreams[OS_TRANS_INFO]->getOStream() << "</transmittedInfos>" << endl;
    }
    if (myOutputStreams[OS_VEH_IN_RANGE]!=0) {
        myOutputStreams[OS_VEH_IN_RANGE]->getOStream() << "</vehicleInRanges>" << endl;
    }
    if (myLogExecutionTime!=0&&mySimDuration!=0) {
        ostringstream msg;
        msg << "Performance: " << endl
            << " Duration: " << mySimDuration << "ms" << endl
            << " Real time factor: " << ((SUMOReal)(stop-start)*1000./(SUMOReal)mySimDuration) << endl
            << " UPS: " << ((SUMOReal) myVehiclesMoved / (SUMOReal) mySimDuration * 1000.) << endl;
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

    if (MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions(step);
    }
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

    MSUpdateEachTimestepContainer<
    DetectorContainer::UpdateHaltings >::getInstance()->updateAll();
    MSUpdateEachTimestepContainer<
    DetectorContainer::UpdateE3Haltings >::getInstance()->updateAll();
    MSUpdateEachTimestepContainer<
    Detector::UpdateE2Detectors >::getInstance()->updateAll();
    MSUpdateEachTimestepContainer<
    Detector::UpdateOccupancyCorrections >::getInstance()->updateAll();

    // Vehicles change Lanes (maybe)
    myEdges->changeLanes();

    // check whether the tls shall be switched
    myLogics->check2Switch(myStep);

    if (MSGlobals::gCheck4Accidents) {
        myEdges->detectCollisions(step);
    }
    // Check if mean-lane-data is due
    writeOutput();
    // execute endOfTimestepEvents
    myEndOfTimestepEvents.execute(myStep);
    if (MSGlobals::gUsingC2C) {
        computeCar2Car();
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
    MSCORN::saveSavedInformationData(myStep,"","","",-1,-1,0);
    MSCORN::saveClusterInfoData(myStep,0,"",0,0);
    MSCORN::saveTransmittedInformationData(myStep,"","","",-1,-1,0);
    MSCORN::saveVehicleInRangeData(myStep, "", "",-1,-1,-1,-1,0);

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
    //close XML-tags
    MSCORN::saveClusterInfoData(myStep,0,"",0,1);
    MSCORN::saveTransmittedInformationData(myStep,"","","",-1,-1,1);
    MSCORN::saveVehicleInRangeData(myStep, "", "",-1,-1,-1,-1,1);
    MSCORN::saveSavedInformationData(myStep,"","","",-1,-1,1);
}


void
MSNet::clearAll()
{
    // clear container
    MSEdge::clear();
    MSJunction::clear();
    MSJunctionControl::clear();
    MSJunctionLogic::clear();
    MSLane::clear();
    MSVehicleType::clear();
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
    if (myOutputStreams[OS_NETSTATE]!=0) {
        MSXMLRawOut::write(myOutputStreams[OS_NETSTATE], *myEdges, myStep, 3);
    }
    // emission output
    if (myOutputStreams[OS_EMISSIONS]!=0) {
        myOutputStreams[OS_EMISSIONS]->getOStream()
        << "    <emission-state time=\"" << myStep << "\" "
        << "loaded=\"" << myVehicleControl->getLoadedVehicleNo() << "\" "
        << "emitted=\"" << myVehicleControl->getEmittedVehicleNo() << "\" "
        << "running=\"" << myVehicleControl->getRunningVehicleNo() << "\" "
        << "waiting=\"" << myEmitter->getWaitingVehicleNo() << "\" "
        << "ended=\"" << myVehicleControl->getEndedVehicleNo() << "\" "
        << "meanWaitingTime=\"" << myVehicleControl->getMeanWaitingTime() << "\" "
        << "meanTravelTime=\"" << myVehicleControl->getMeanTravelTime() << "\" ";
        if (myLogExecutionTime) {
            myOutputStreams[OS_EMISSIONS]->getOStream()
            << "duration=\"" << mySimStepDuration << "\" ";
        }
        myOutputStreams[OS_EMISSIONS]->getOStream()
        << "/>" << endl;
    }
    if (myOutputStreams[OS_CELL_TO_SS2_SQL] != 0
            || myOutputStreams[OS_LA_TO_SS2_SQL] != 0
            || myOutputStreams[OS_DEVICE_TO_SS2_SQL] != 0
            || myOutputStreams[OS_LA_TO_SS2] != 0
            || myOutputStreams[OS_CELL_TO_SS2] != 0
            || myOutputStreams[OS_DEVICE_TO_SS2] != 0) {
        myMSPhoneNet->writeOutput(myStep);
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
    /*
    if((what&(long) SAVESTATE_EDGES)!=0) myEdges->saveState(os);
    if((what&(long) SAVESTATE_EMITTER)!=0) myEmitter->saveState(os);
    if((what&(long) SAVESTATE_LOGICS)!=0) myLogics->saveState(os);
    if((what&(long) SAVESTATE_ROUTES)!=0) myRouteLoaders->saveState(os);
    if((what&(long) SAVESTATE_VEHICLES)!=0) myVehicleControl->saveState(os);
    */
}


void
MSNet::loadState(BinaryInputDevice &bis, long what)
{
    myVehicleControl->loadState(bis, what);
    myEdges->loadState(bis, what);
    /*
    if((what&(long) SAVESTATE_EDGES)!=0) myEdges->saveState(os);
    if((what&(long) SAVESTATE_EMITTER)!=0) myEmitter->saveState(os);
    if((what&(long) SAVESTATE_LOGICS)!=0) myLogics->saveState(os);
    if((what&(long) SAVESTATE_ROUTES)!=0) myRouteLoaders->saveState(os);
    if((what&(long) SAVESTATE_VEHICLES)!=0) myVehicleControl->saveState(os);
    */
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



void
MSNet::setOffset(const Position2D &p)
{
    myOffset = p;
}


void
MSNet::setOrigBoundary(const Boundary &p)
{
    myOrigBoundary = p;
}


void
MSNet::setConvBoundary(const Boundary &p)
{
    myConvBoundary = p;
}


void
MSNet::setOrigProj(const std::string &proj)
{
    myOrigProj = proj;
}



const Position2D &
MSNet::getOffset() const
{
    return myOffset;
}


const Boundary &
MSNet::getOrigBoundary() const
{
    return myOrigBoundary;
}


const Boundary &
MSNet::getConvBoundary() const
{
    return myConvBoundary;
}


const std::string &
MSNet::getOrigProj() const
{
    return myOrigProj;
}



/****************************************************************************/

