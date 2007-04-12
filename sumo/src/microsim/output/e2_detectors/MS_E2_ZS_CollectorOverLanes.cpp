/****************************************************************************/
/// @file    MS_E2_ZS_CollectorOverLanes.cpp
/// @author  Daniel Krajzewicz
/// @date    Oct 2003
/// @version $Id$
///
// A detector which joins E2Collectors over consecutive lanes (backward)
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

#include "MS_E2_ZS_CollectorOverLanes.h"
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSEdgeContinuations.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member variable definitions
// ===========================================================================
std::string
MS_E2_ZS_CollectorOverLanes::xmlHeaderM(
    "<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
    "<!--\n"
    "- densityMean [veh/km]\n"
    "- maxJamLengthInVehiclesMean [veh]\n"
    "- maxJamLengthInMetersMean [m]\n"
    "- jamLengthSumInVehiclesMean [veh]\n"
    "- jamLengthSumInMetersMean [m]\n"
    "- queueLengthAheadOfTrafficLightsInVehiclesMean [veh]\n"
    "- queueLengthAheadOfTrafficLightsInMetersMean [m]\n"
    "- nE2VehiclesMean [veh]\n"
    "- occupancyDegreeMean [0,1]\n"
    "- spaceMeanSpeedMean [m/s]\n"
    "- currentHaltingDurationSumPerVehicleMean [s]\n"
    "- nStartedHalts [n]\n"
    //"- haltingDurationSum [s]\n"
    "- haltingDurationMean [s]\n"
    "-->\n\n");

std::string MS_E2_ZS_CollectorOverLanes::infoEndM = "</detector>";


// ===========================================================================
// method definitions
// ===========================================================================
MS_E2_ZS_CollectorOverLanes::MS_E2_ZS_CollectorOverLanes(
    std::string id, DetectorUsage usage, MSLane* lane,
    SUMOReal startPos,
    SUMOReal haltingTimeThreshold,
    MSUnit::MetersPerSecond haltingSpeedThreshold,
    SUMOReal jamDistThreshold,
    SUMOTime deleteDataAfterSeconds)
        : startPosM(startPos),
        deleteDataAfterSecondsM(deleteDataAfterSeconds),
        haltingTimeThresholdM(haltingTimeThreshold),
        haltingSpeedThresholdM(haltingSpeedThreshold),
        jamDistThresholdM(jamDistThreshold),
        myID(id), myStartLaneID(lane->getID()),
        myUsage(usage)
{}


void
MS_E2_ZS_CollectorOverLanes::init(
    MSLane *lane,
    SUMOReal detLength,
    const MSEdgeContinuations &edgeContinuations)
{
    myLength = detLength;
    if (startPosM==0) {
        startPosM = (SUMOReal) 0.1;
    }
    SUMOReal length = lane->length() - startPosM - (SUMOReal) 0.1;
    SUMOReal dlength = detLength;
    if (length>dlength) {
        length = dlength;
    }
    myLengths.push_back(length);
    myLaneCombinations.push_back(LaneVector());
    myLaneCombinations[0].push_back(lane);
    myDetectorCombinations.push_back(DetectorVector());
    MSE2Collector *c =
        buildCollector(0, 0, lane, startPosM, length);
    myDetectorCombinations[0].push_back(c);
    myAlreadyBuild[lane] = c;
    extendTo(detLength, edgeContinuations);
}


MS_E2_ZS_CollectorOverLanes::~MS_E2_ZS_CollectorOverLanes(void)
{}


void
MS_E2_ZS_CollectorOverLanes::extendTo(
    SUMOReal length,
    const MSEdgeContinuations &edgeContinuations)
{
    bool done = false;
    while (!done) {
        done = true;
        LengthVector::iterator leni = myLengths.begin();
        LaneVectorVector::iterator lanei = myLaneCombinations.begin();
        DetectorVectorVector::iterator deti = myDetectorCombinations.begin();
        for (; leni!=myLengths.end(); leni++, lanei++, deti++) {
            if ((*leni)<length) {
                done = false;
                // copy current values
                LaneVector lv = *lanei;
                DetectorVector dv = *deti;
                SUMOReal clength = *leni;
                assert(lv.size()>0);
                assert(dv.size()>0);
                // erase previous elements
                assert(leni!=myLengths.end());
                myLengths.erase(leni);
                myLaneCombinations.erase(lanei);
                myDetectorCombinations.erase(deti);
                // get the lane to look before
                MSLane *toExtend = lv[lv.size()-1];
                // and her predecessors
                std::vector<MSLane*> predeccessors =
                    getLanePredeccessorLanes(toExtend, edgeContinuations);
                if (predeccessors.size()==0) {
                    int off = 1;
                    const MSEdge * const e = toExtend->getEdge();
                    const std::vector<MSLane*> *lanes = e->getLanes();
                    int idx =
                        distance(lanes->begin(), find(lanes->begin(), lanes->end(), toExtend));
                    while (predeccessors.size()==0) {
                        if (idx-off>=0) {
                            MSLane *tryMe = (*lanes)[idx-off];
                            predeccessors =
                                getLanePredeccessorLanes(tryMe, edgeContinuations);
                        }
                        if (predeccessors.size()==0&&idx+off<(int) lanes->size()) {
                            MSLane *tryMe = (*lanes)[idx+off];
                            predeccessors =
                                getLanePredeccessorLanes(tryMe, edgeContinuations);
                        }
                        off++;
                    }
                }

                /*                LaneContinuations::const_iterator conts =
                                    laneContinuations.find(toExtend->id());
                                assert(conts!=laneContinuations.end());
                                const std::vector<std::string> &predeccessors =
                                    (*conts).second;*/
                // go through the predeccessors and extend the detector
                for (std::vector<MSLane*>::const_iterator i=predeccessors.begin(); i!=predeccessors.end(); i++) {
                    // get the lane
                    MSLane *l = *i;
                    // compute detector length
                    SUMOReal lanelen = length - clength;
                    if (lanelen>l->length()) {
                        lanelen = l->length() - (SUMOReal) 0.2;
                    }
                    // build new info
                    LaneVector nlv = lv;
                    nlv.push_back(l);
                    DetectorVector ndv = dv;
                    MSE2Collector *coll = 0;
                    if (myAlreadyBuild.find(l)==myAlreadyBuild.end()) {
                        coll = buildCollector(0, 0, l, (SUMOReal) 0.1, lanelen);
                    } else {
                        coll = myAlreadyBuild.find(l)->second;
                    }
                    myAlreadyBuild[l] = coll;
                    ndv.push_back(coll);
                    // store new info
                    myLaneCombinations.push_back(nlv);
                    myDetectorCombinations.push_back(ndv);
                    myLengths.push_back(clength+lanelen);
                }
                // restart
                leni = myLengths.end() - 1;
            }
        }
    }
}


std::vector<MSLane*>
MS_E2_ZS_CollectorOverLanes::getLanePredeccessorLanes(MSLane *l,
        const MSEdgeContinuations &edgeContinuations)
{
    string eid = l->getEdge()->getID();
    // check whether any exist
    if (!edgeContinuations.hasFurther(*l->getEdge())) {
        return std::vector<MSLane*>();
    }
    // get predecessing edges
    const std::vector<MSEdge*> &predEdges =
        edgeContinuations.getInFrontOfEdge(*l->getEdge());
    std::vector<MSLane*> ret;
    // find predecessing lanes
    for (std::vector<MSEdge*>::const_iterator i=predEdges.begin(); i!=predEdges.end(); i++) {
        MSEdge *e = *i;
        assert(e!=0);
        typedef std::vector<MSLane*> LaneVector;
        const LaneVector *cl = e->allowedLanes(*l->getEdge(), SVC_UNKNOWN);
        bool fastAbort = false;
        if (cl!=0) {
            for (LaneVector::const_iterator j=cl->begin(); !fastAbort&&j!=cl->end(); j++) {
                const MSLinkCont &lc = (*j)->getLinkCont();
                for (MSLinkCont::const_iterator k=lc.begin(); !fastAbort&&k!=lc.end(); k++) {
                    if ((*k)->getLane()==l) {
                        ret.push_back(*j);
                        fastAbort = true;
                    }
                }
            }
        }
    }
    return ret;
}


MSE2Collector *
MS_E2_ZS_CollectorOverLanes::buildCollector(size_t c, size_t r, MSLane *l,
        SUMOReal start, SUMOReal end)
{
    string id = makeID(l->getID(), c, r);
    if (start+end<l->length()) {
        start = l->length() - end - (SUMOReal) 0.1;
    }
    return new MSE2Collector(id, myUsage,
                             l, start, end, haltingTimeThresholdM,
                             haltingSpeedThresholdM, jamDistThresholdM, deleteDataAfterSecondsM);
}


SUMOReal
MS_E2_ZS_CollectorOverLanes::getCurrent(E2::DetType type)
{
    switch (type) {
    case E2::DENSITY:
    case E2::MAX_JAM_LENGTH_IN_VEHICLES:
    case E2::MAX_JAM_LENGTH_IN_METERS:
    case E2::JAM_LENGTH_SUM_IN_VEHICLES:
    case E2::JAM_LENGTH_SUM_IN_METERS:
    case E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES:
    case E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS:
    case E2::N_VEHICLES:
    case E2::OCCUPANCY_DEGREE:
    case E2::SPACE_MEAN_SPEED:
    case E2::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE:
    default:
        SUMOReal myMax = 0;
        for (DetectorVectorVector::iterator i=myDetectorCombinations.begin(); i!=myDetectorCombinations.end(); i++) {
            SUMOReal value = 0;
            for (DetectorVector::iterator j=(*i).begin(); j!=(*i).end(); j++) {
                value += (*j)->getCurrent(type);
            }
            if (myMax<value) {
                myMax = value;
            }
        }
        return myMax;
    }
    return -1;
}


SUMOReal
MS_E2_ZS_CollectorOverLanes::getAggregate(E2::DetType type,
        MSUnit::Seconds lastNSeconds)
{
    switch (type) {
    case E2::DENSITY:
    case E2::MAX_JAM_LENGTH_IN_VEHICLES:
    case E2::MAX_JAM_LENGTH_IN_METERS:
    case E2::JAM_LENGTH_SUM_IN_VEHICLES:
    case E2::JAM_LENGTH_SUM_IN_METERS:
    case E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES:
    case E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS:
    case E2::N_VEHICLES:
    case E2::OCCUPANCY_DEGREE:
    case E2::SPACE_MEAN_SPEED:
    case E2::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE:
    default:
        SUMOReal myMax = 0;
        for (DetectorVectorVector::iterator i=myDetectorCombinations.begin(); i!=myDetectorCombinations.end(); i++) {
            SUMOReal value = 0;
            for (DetectorVector::iterator j=(*i).begin(); j!=(*i).end(); j++) {
                value += (*j)->getAggregate(type, lastNSeconds);
            }
            if (value>myMax) {
                myMax = value;
            }
        }
        return myMax;
    }
    return -1;
}


bool
MS_E2_ZS_CollectorOverLanes::hasDetector(E2::DetType type)
{
    return myDetectorCombinations[0][0]->hasDetector(type);
}


void
MS_E2_ZS_CollectorOverLanes::addDetector(E2::DetType type,
        std::string detId)
{
    size_t c = 0;
    for (LaneDetMap::iterator i=myAlreadyBuild.begin(); i!=myAlreadyBuild.end(); i++) {
        (*i).second->addDetector(type, makeID(detId, c, c));
        c++;
    }
}


void
MS_E2_ZS_CollectorOverLanes::writeXMLOutput(OutputDevice &dev,
        SUMOTime startTime, SUMOTime stopTime)
{
    dev.writeString("<interval begin=\"").writeString(
        toString(startTime)).writeString("\" end=\"").writeString(
            toString(stopTime)).writeString("\" ");
    if (dev.needsDetectorName()) {
        dev.writeString("id=\"").writeString(myID).writeString("\" ");
    }
    if (hasDetector(E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES)) {
        dev.writeString("collQueueLengthAheadOfTrafficLightsInVehiclesMax=\"");
        dev.writeString(
            toString(getCurrent(E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES)));
        dev.writeString("\" ");
        resetQueueLengthAheadOfTrafficLights();
    }
    myDetectorCombinations[0][0]->writeXMLOutput(dev, startTime, stopTime);
    dev.writeString("/>");
}


void
MS_E2_ZS_CollectorOverLanes::writeXMLDetectorInfoStart(OutputDevice &dev) const
{
    dev.writeString("<detector type=\"E2_ZS_Collector\" id=\"").writeString(
        myID).writeString("\" startlane=\"").writeString(
            myStartLaneID).writeString("\" startpos=\"").writeString(
                toString(startPosM)).writeString("\" length=\"").writeString(
                    toString(myLength)).writeString("\" >\n");
}


size_t bla = 0;

std::string
MS_E2_ZS_CollectorOverLanes::makeID(const std::string &baseID ,
                                    size_t /*col*/, size_t /*row*/) const
{
    string add;
    switch (myUsage) {
    case DU_USER_DEFINED:
        add = "(u)";
        break;
    case DU_SUMO_INTERNAL:
        add = "(i)";
        break;
    case DU_TL_CONTROL:
        add = "(c)";
        break;
    default:
        break;
    }
    string ret =  baseID + add + toString<size_t>(bla++);
    return ret;
}


const std::string &
MS_E2_ZS_CollectorOverLanes::getID() const
{
    return myID;
}


const std::string &
MS_E2_ZS_CollectorOverLanes::getStartLaneID() const
{
    return myStartLaneID;
}


void
MS_E2_ZS_CollectorOverLanes::resetQueueLengthAheadOfTrafficLights(void)
{
    for (DetectorVectorVector::iterator i=myDetectorCombinations.begin(); i!=myDetectorCombinations.end(); i++) {
        for (DetectorVector::iterator j=(*i).begin(); j!=(*i).end(); j++) {
            (*j)->resetQueueLengthAheadOfTrafficLights();
        }
    }
}



/****************************************************************************/

