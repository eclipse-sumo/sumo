/****************************************************************************/
/// @file    NLDetectorBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Clemens Honomichl
/// @author  Michael Behrisch
/// @author  Christian Roessel
/// @author  Jakob Erdmann
/// @date    Mon, 15 Apr 2002
/// @version $Id$
///
// Builds detectors for microsim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <iostream>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/output/MSInductLoop.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/output/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/output/MSVTypeProbe.h>
#include <microsim/output/MSRouteProbe.h>
#include <microsim/output/MSMeanData_Net.h>
#include <microsim/output/MSMeanData_Emissions.h>
#include <microsim/output/MSMeanData_Harmonoise.h>
#include <microsim/output/MSMeanData_Amitran.h>
#include <microsim/output/MSInstantInductLoop.h>
#include <microsim/MSGlobals.h>
#include <microsim/actions/Command_SaveTLCoupledDet.h>
#include <microsim/actions/Command_SaveTLCoupledLaneDet.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/TplConvert.h>
#include "NLDetectorBuilder.h"
#include <microsim/output/MSDetectorControl.h>

#include <mesosim/MEInductLoop.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * NLDetectorBuilder::E3DetectorDefinition-methods
 * ----------------------------------------------------------------------- */
NLDetectorBuilder::E3DetectorDefinition::E3DetectorDefinition(const std::string& id,
        const std::string& device, SUMOReal haltingSpeedThreshold,
        SUMOTime haltingTimeThreshold, SUMOTime splInterval,
        const std::string& vTypes)
    : myID(id), myDevice(device),
      myHaltingSpeedThreshold(haltingSpeedThreshold),
      myHaltingTimeThreshold(haltingTimeThreshold),
      mySampleInterval(splInterval),
      myVehicleTypes(vTypes) {}


NLDetectorBuilder::E3DetectorDefinition::~E3DetectorDefinition() {}


/* -------------------------------------------------------------------------
 * NLDetectorBuilder-methods
 * ----------------------------------------------------------------------- */
NLDetectorBuilder::NLDetectorBuilder(MSNet& net)
    : myNet(net), myE3Definition(0) {}


NLDetectorBuilder::~NLDetectorBuilder() {
    delete myE3Definition;
}


void
NLDetectorBuilder::buildInductLoop(const std::string& id,
                                   const std::string& lane, SUMOReal pos, SUMOTime splInterval,
                                   const std::string& device, bool friendlyPos,
                                   const std::string& vTypes) {
    checkSampleInterval(splInterval, SUMO_TAG_E1DETECTOR, id);
    // get and check the lane
    MSLane* clane = getLaneChecking(lane, SUMO_TAG_E1DETECTOR, id);
    // get and check the position
    pos = getPositionChecking(pos, clane, friendlyPos, id);
    // build the loop
    MSDetectorFileOutput* loop = createInductLoop(id, clane, pos, vTypes);
    // add the file output
    myNet.getDetectorControl().add(SUMO_TAG_INDUCTION_LOOP, loop, device, splInterval);
}


void
NLDetectorBuilder::buildInstantInductLoop(const std::string& id,
        const std::string& lane, SUMOReal pos,
        const std::string& device, bool friendlyPos,
        const std::string& vTypes) {
    // get and check the lane
    MSLane* clane = getLaneChecking(lane, SUMO_TAG_INSTANT_INDUCTION_LOOP, id);
    // get and check the position
    pos = getPositionChecking(pos, clane, friendlyPos, id);
    // build the loop
    MSDetectorFileOutput* loop = createInstantInductLoop(id, clane, pos, device, vTypes);
    // add the file output
    myNet.getDetectorControl().add(SUMO_TAG_INSTANT_INDUCTION_LOOP, loop);
}


void
NLDetectorBuilder::buildE2Detector(const std::string& id,
                                   const std::string& lane, SUMOReal pos, SUMOReal length,
                                   bool cont, SUMOTime splInterval,
                                   const std::string& device,
                                   SUMOTime haltingTimeThreshold,
                                   SUMOReal haltingSpeedThreshold,
                                   SUMOReal jamDistThreshold, bool friendlyPos,
                                   const std::string& vTypes) {
    checkSampleInterval(splInterval, SUMO_TAG_E2DETECTOR, id);
    MSLane* clane = getLaneChecking(lane, SUMO_TAG_E2DETECTOR, id);
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput* det = 0;
    if (!cont) {
        convUncontE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED, clane, pos, length, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, vTypes);
        myNet.getDetectorControl().add(SUMO_TAG_LANE_AREA_DETECTOR, det, device, splInterval);
    } else {
        convContE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildMultiLaneE2Det(id, DU_USER_DEFINED, clane, pos, length, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, vTypes);
        myNet.getDetectorControl().add(SUMO_TAG_LANE_AREA_DETECTOR, det, device, splInterval);
    }
}


void
NLDetectorBuilder::buildE2Detector(const std::string& id,
                                   const std::string& lane, SUMOReal pos, SUMOReal length,
                                   bool cont,
                                   MSTLLogicControl::TLSLogicVariants& tlls,
                                   const std::string& device,
                                   SUMOTime haltingTimeThreshold,
                                   SUMOReal haltingSpeedThreshold,
                                   SUMOReal jamDistThreshold, bool friendlyPos,
                                   const std::string& vTypes) {
    if (tlls.getActive() == 0) {
        throw InvalidArgument("The detector '" + id + "' refers to the unknown lsa.");
    }
    MSLane* clane = getLaneChecking(lane, SUMO_TAG_E2DETECTOR, id);
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput* det = 0;
    if (!cont) {
        convUncontE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED, clane, pos, length, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, vTypes);
        myNet.getDetectorControl().add(SUMO_TAG_LANE_AREA_DETECTOR, det);
    } else {
        convContE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildMultiLaneE2Det(id, DU_USER_DEFINED, clane, pos, length, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, vTypes);
        myNet.getDetectorControl().add(SUMO_TAG_LANE_AREA_DETECTOR, det);
    }
    // add the file output
    new Command_SaveTLCoupledDet(tlls, det, myNet.getCurrentTimeStep(), OutputDevice::getDevice(device));
}


void
NLDetectorBuilder::buildE2Detector(const std::string& id,
                                   const std::string& lane, SUMOReal pos, SUMOReal length,
                                   bool cont,
                                   MSTLLogicControl::TLSLogicVariants& tlls,
                                   const std::string& tolane,
                                   const std::string& device,
                                   SUMOTime haltingTimeThreshold,
                                   SUMOReal haltingSpeedThreshold,
                                   SUMOReal jamDistThreshold, bool friendlyPos,
                                   const std::string& vTypes) {
    if (tlls.getActive() == 0) {
        throw InvalidArgument("The detector '" + id + "' refers to the unknown lsa.");
    }
    MSLane* clane = getLaneChecking(lane, SUMO_TAG_E2DETECTOR, id);
    MSLane* ctoLane = getLaneChecking(tolane, SUMO_TAG_E2DETECTOR, id);
    MSLink* link = MSLinkContHelper::getConnectingLink(*clane, *ctoLane);
    if (link == 0) {
        throw InvalidArgument(
            "The detector output can not be build as no connection between lanes '"
            + lane + "' and '" + tolane + "' exists.");
    }
    if (pos < 0) {
        pos = -pos;
    }
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput* det = 0;
    if (!cont) {
        convUncontE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED, clane, pos, length, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, vTypes);
        myNet.getDetectorControl().add(SUMO_TAG_LANE_AREA_DETECTOR, det);
    } else {
        convContE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildMultiLaneE2Det(id, DU_USER_DEFINED, clane, pos, length, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, vTypes);
        myNet.getDetectorControl().add(SUMO_TAG_LANE_AREA_DETECTOR, det);
    }
    // add the file output
    new Command_SaveTLCoupledLaneDet(tlls, det, myNet.getCurrentTimeStep(), OutputDevice::getDevice(device), link);
}


void
NLDetectorBuilder::convUncontE2PosLength(const std::string& id, MSLane* clane,
        SUMOReal& pos, SUMOReal& length,
        bool friendlyPos) {
    // get and check the position
    pos = getPositionChecking(pos, clane, friendlyPos, id);
    // check length
    if (length <= 0) {
        if (friendlyPos) {
            length = (SUMOReal) 0.1;
        } else {
            throw InvalidArgument("Invalid length for detector '" + id + "'.");
        }
    }
    if (length + pos > clane->getLength()) {
        if (friendlyPos) {
            length = clane->getLength() - pos;
        } else {
            throw InvalidArgument("The length of detector '" + id + "' reaches beyond the lane's '" + clane->getID() + "' length.");
        }
    }
}


void
NLDetectorBuilder::convContE2PosLength(const std::string& id, MSLane* clane,
                                       SUMOReal& pos, SUMOReal& /*length*/,
                                       bool friendlyPos) {
    // get and check the position
    pos = getPositionChecking(pos, clane, friendlyPos, id);
    // length will be kept as is
}


void
NLDetectorBuilder::beginE3Detector(const std::string& id,
                                   const std::string& device, SUMOTime splInterval,
                                   SUMOReal haltingSpeedThreshold,
                                   SUMOTime haltingTimeThreshold,
                                   const std::string& vTypes) {
    checkSampleInterval(splInterval, SUMO_TAG_E3DETECTOR, id);
    myE3Definition = new E3DetectorDefinition(id, device, haltingSpeedThreshold, haltingTimeThreshold, splInterval, vTypes);
}


void
NLDetectorBuilder::addE3Entry(const std::string& lane,
                              SUMOReal pos, bool friendlyPos) {
    if (myE3Definition == 0) {
        return;
    }
    MSLane* clane = getLaneChecking(lane, SUMO_TAG_E3DETECTOR, myE3Definition->myID);
    // get and check the position
    pos = getPositionChecking(pos, clane, friendlyPos, myE3Definition->myID);
    // build and save the entry
    myE3Definition->myEntries.push_back(MSCrossSection(clane, pos));
}


void
NLDetectorBuilder::addE3Exit(const std::string& lane,
                             SUMOReal pos, bool friendlyPos) {
    if (myE3Definition == 0) {
        return;
    }
    MSLane* clane = getLaneChecking(lane, SUMO_TAG_E3DETECTOR, myE3Definition->myID);
    // get and check the position
    pos = getPositionChecking(pos, clane, friendlyPos, myE3Definition->myID);
    // build and save the exit
    myE3Definition->myExits.push_back(MSCrossSection(clane, pos));
}


std::string
NLDetectorBuilder::getCurrentE3ID() const {
    if (myE3Definition == 0) {
        return "<unknown>";
    }
    return myE3Definition->myID;
}


void
NLDetectorBuilder::endE3Detector() {
    if (myE3Definition == 0) {
        return;
    }
    // If E3 own entry or exit detectors
    if (myE3Definition->myEntries.size() > 0 || myE3Definition->myExits.size() > 0) {
        // create E3 detector
        MSDetectorFileOutput* det = createE3Detector(myE3Definition->myID,
                                    myE3Definition->myEntries, myE3Definition->myExits,
                                    myE3Definition->myHaltingSpeedThreshold, myE3Definition->myHaltingTimeThreshold, myE3Definition->myVehicleTypes);
        // add to net
        myNet.getDetectorControl().add(SUMO_TAG_ENTRY_EXIT_DETECTOR, det, myE3Definition->myDevice, myE3Definition->mySampleInterval);
    } else
        WRITE_WARNING(toString(SUMO_TAG_E3DETECTOR) + " with id = '" + myE3Definition->myID + "' will not be created because is empty (no " + toString(SUMO_TAG_DET_ENTRY) + " or " + toString(SUMO_TAG_DET_EXIT) + " was defined)")

        // clean up
        delete myE3Definition;
    myE3Definition = 0;
}


void
NLDetectorBuilder::buildVTypeProbe(const std::string& id,
                                   const std::string& vtype, SUMOTime frequency,
                                   const std::string& device) {
    checkSampleInterval(frequency, SUMO_TAG_VTYPEPROBE, id);
    new MSVTypeProbe(id, vtype, OutputDevice::getDevice(device), frequency);
}


void
NLDetectorBuilder::buildRouteProbe(const std::string& id, const std::string& edge,
                                   SUMOTime frequency, SUMOTime begin,
                                   const std::string& device,
                                   const std::string& vTypes) {
    checkSampleInterval(frequency, SUMO_TAG_ROUTEPROBE, id);
    MSEdge* e = getEdgeChecking(edge, SUMO_TAG_ROUTEPROBE, id);
    MSRouteProbe* probe = new MSRouteProbe(id, e, id + "_" + toString(begin), id + "_" + toString(begin - frequency), vTypes);
    // add the file output
    myNet.getDetectorControl().add(SUMO_TAG_ROUTEPROBE, probe, device, frequency, begin);
}


// -------------------
MSE2Collector*
NLDetectorBuilder::buildSingleLaneE2Det(const std::string& id,
                                        DetectorUsage usage,
                                        MSLane* lane, SUMOReal pos, SUMOReal length,
                                        SUMOTime haltingTimeThreshold,
                                        SUMOReal haltingSpeedThreshold,
                                        SUMOReal jamDistThreshold,
                                        const std::string& vTypes) {
    return createSingleLaneE2Detector(id, usage, lane, pos,
                                      length, haltingTimeThreshold, haltingSpeedThreshold,
                                      jamDistThreshold, vTypes);
}


MSDetectorFileOutput*
NLDetectorBuilder::buildMultiLaneE2Det(const std::string& id, DetectorUsage usage,
                                       MSLane* lane, SUMOReal pos, SUMOReal length,
                                       SUMOTime haltingTimeThreshold,
                                       SUMOReal haltingSpeedThreshold,
                                       SUMOReal jamDistThreshold,
                                       const std::string& vTypes) {
    MSDetectorFileOutput* ret = createMultiLaneE2Detector(id, usage,
                                lane, pos, haltingTimeThreshold, haltingSpeedThreshold,
                                jamDistThreshold, vTypes);
    static_cast<MS_E2_ZS_CollectorOverLanes*>(ret)->init(lane, length);
    return ret;
}


MSDetectorFileOutput*
NLDetectorBuilder::createInductLoop(const std::string& id,
                                    MSLane* lane, SUMOReal pos,
                                    const std::string& vTypes, bool) {
    if (MSGlobals::gUseMesoSim) {
        return new MEInductLoop(id, MSGlobals::gMesoNet->getSegmentForEdge(lane->getEdge(), pos), pos, vTypes);
    }
    return new MSInductLoop(id, lane, pos, vTypes);
}


MSDetectorFileOutput*
NLDetectorBuilder::createInstantInductLoop(const std::string& id,
        MSLane* lane, SUMOReal pos, const std::string& od,
        const std::string& vTypes) {
    return new MSInstantInductLoop(id, OutputDevice::getDevice(od), lane, pos, vTypes);
}


MSE2Collector*
NLDetectorBuilder::createSingleLaneE2Detector(const std::string& id,
        DetectorUsage usage, MSLane* lane, SUMOReal pos, SUMOReal length,
        SUMOTime haltingTimeThreshold, SUMOReal haltingSpeedThreshold, SUMOReal jamDistThreshold,
        const std::string& vTypes) {
    return new MSE2Collector(id, usage, lane, pos, length, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, vTypes);
}


MSDetectorFileOutput*
NLDetectorBuilder::createMultiLaneE2Detector(const std::string& id,
        DetectorUsage usage, MSLane* lane, SUMOReal pos,
        SUMOTime haltingTimeThreshold, SUMOReal haltingSpeedThreshold, SUMOReal jamDistThreshold,
        const std::string& vTypes) {
    return new MS_E2_ZS_CollectorOverLanes(id, usage, lane, pos, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, vTypes);
}


MSDetectorFileOutput*
NLDetectorBuilder::createE3Detector(const std::string& id,
                                    const CrossSectionVector& entries,
                                    const CrossSectionVector& exits,
                                    SUMOReal haltingSpeedThreshold,
                                    SUMOTime haltingTimeThreshold,
                                    const std::string& vTypes) {
    return new MSE3Collector(id, entries, exits, haltingSpeedThreshold, haltingTimeThreshold, vTypes);
}


SUMOReal
NLDetectorBuilder::getPositionChecking(SUMOReal pos, MSLane* lane, bool friendlyPos,
                                       const std::string& detid) {
    // check whether it is given from the end
    if (pos < 0) {
        pos += lane->getLength();
    }
    // check whether it is on the lane
    if (pos > lane->getLength()) {
        if (friendlyPos) {
            pos = lane->getLength();
        } else {
            throw InvalidArgument("The position of detector '" + detid + "' lies beyond the lane's '" + lane->getID() + "' end.");
        }
    }
    if (pos < 0) {
        if (friendlyPos) {
            pos = 0.;
        } else {
            throw InvalidArgument("The position of detector '" + detid + "' lies before the lane's '" + lane->getID() + "' begin.");
        }
    }
    return pos;
}


void
NLDetectorBuilder::createEdgeLaneMeanData(const std::string& id, SUMOTime frequency,
        SUMOTime begin, SUMOTime end, const std::string& type,
        const bool useLanes, const bool withEmpty, const bool printDefaults,
        const bool withInternal, const bool trackVehicles,
        const SUMOReal maxTravelTime, const SUMOReal minSamples,
        const SUMOReal haltSpeed, const std::string& vTypes,
        const std::string& device) {
    if (begin < 0) {
        throw InvalidArgument("Negative begin time for meandata dump '" + id + "'.");
    }
    if (end < 0) {
        end = SUMOTime_MAX;
    }
    if (end <= begin) {
        throw InvalidArgument("End before or at begin for meandata dump '" + id + "'.");
    }
    MSMeanData* det = 0;
    if (type == "" || type == "performance" || type == "traffic") {
        det = new MSMeanData_Net(id, begin, end, useLanes, withEmpty,
                                 printDefaults, withInternal, trackVehicles, maxTravelTime, minSamples, haltSpeed, vTypes);
    } else if (type == "emissions" || type == "hbefa") {
        if (type == "hbefa") {
            WRITE_WARNING("The netstate type 'hbefa' is deprecated. Please use the type 'emissions' instead.");
        }
        det = new MSMeanData_Emissions(id, begin, end, useLanes, withEmpty,
                                       printDefaults, withInternal, trackVehicles, maxTravelTime, minSamples, vTypes);
    } else if (type == "harmonoise") {
        det = new MSMeanData_Harmonoise(id, begin, end, useLanes, withEmpty,
                                        printDefaults, withInternal, trackVehicles, maxTravelTime, minSamples, vTypes);
    } else if (type == "amitran") {
        det = new MSMeanData_Amitran(id, begin, end, useLanes, withEmpty,
                                     printDefaults, withInternal, trackVehicles, maxTravelTime, minSamples, haltSpeed, vTypes);
    } else {
        throw InvalidArgument("Invalid type '" + type + "' for meandata dump '" + id + "'.");
    }
    if (det != 0) {
        if (frequency < 0) {
            frequency = end - begin;
        }
        MSNet::getInstance()->getDetectorControl().add(det, device, frequency, begin);
    }
}




// ------ Value checking/adapting methods ------
MSEdge*
NLDetectorBuilder::getEdgeChecking(const std::string& edgeID, SumoXMLTag type,
                                   const std::string& detid) {
    // get and check the lane
    MSEdge* edge = MSEdge::dictionary(edgeID);
    if (edge == 0) {
        throw InvalidArgument("The lane with the id '" + edgeID + "' is not known (while building " + toString(type) + " '" + detid + "').");
    }
    return edge;
}


MSLane*
NLDetectorBuilder::getLaneChecking(const std::string& laneID, SumoXMLTag type,
                                   const std::string& detid) {
    // get and check the lane
    MSLane* lane = MSLane::dictionary(laneID);
    if (lane == 0) {
        throw InvalidArgument("The lane with the id '" + laneID + "' is not known (while building " + toString(type) + " '" + detid + "').");
    }
    return lane;
}


void
NLDetectorBuilder::checkSampleInterval(SUMOTime splInterval, SumoXMLTag type, const std::string& id) {
    if (splInterval < 0) {
        throw InvalidArgument("Negative sampling frequency (in " + toString(type) + " '" + id + "').");
    }
    if (splInterval == 0) {
        throw InvalidArgument("Sampling frequency must not be zero (in " + toString(type) + " '" + id + "').");
    }
}


/****************************************************************************/

