//---------------------------------------------------------------------------//
//                        MS_E2_ZS_CollectorOverLanes.cpp -
//  A detector which joins E2Collectors over consecutive lanes (backward)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Oct 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.16  2004/07/02 09:53:05  dkrajzew
// tried to patch an error on detector loading
//
// Revision 1.15  2004/03/19 13:09:40  dkrajzew
// debugging
//
// Revision 1.14  2004/02/18 05:25:21  dkrajzew
// error on computation of the preceding lane using neighbour lane patched
//
// Revision 1.13  2004/02/16 14:04:54  dkrajzew
// bug on extension of lanes that do not have a predeccesor patched
//
// Revision 1.12  2004/02/05 16:34:25  dkrajzew
// made the usage of the detector output end more usable
//
// Revision 1.11  2004/01/26 07:31:22  dkrajzew
// differnt detector usage types added
//
// Revision 1.10  2004/01/13 14:25:02  dkrajzew
// corrected the output description
//
// Revision 1.9  2004/01/12 15:04:16  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.8  2004/01/12 14:35:10  dkrajzew
// documentation added; allowed the writing to files
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MS_E2_ZS_CollectorOverLanes.h"
#include "MSDetectorSubSys.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member variable definitions
 * ======================================================================= */
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

std::string MS_E2_ZS_CollectorOverLanes::infoEndM = std::string("</detector>");

/* =========================================================================
 * method definitions
 * ======================================================================= */
MS_E2_ZS_CollectorOverLanes::MS_E2_ZS_CollectorOverLanes(
        std::string id, DetectorUsage usage, MSLane* lane,
        MSUnit::Meters startPos,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
        : startPosM(startPos),
        deleteDataAfterSecondsM(deleteDataAfterSeconds),
        haltingTimeThresholdM(haltingTimeThreshold),
        haltingSpeedThresholdM(haltingSpeedThreshold),
        jamDistThresholdM(jamDistThreshold),
        myID(id), myStartLaneID(lane->id()),
        myUsage(usage)
{
    // insert object into dictionary
    if ( ! MSDetectorSubSys::E2ZSOLDict::getInstance()->isInsertSuccess(myID, this ) ) {
        assert( false );
    }
}


void
MS_E2_ZS_CollectorOverLanes::init(
        MSLane *lane,
        MSUnit::Meters detLength,
        const LaneContinuations &laneContinuations)
{
    myLength = detLength;
    if(startPosM==0) {
        startPosM = 0.1;
    }
    double length = lane->length() - startPosM - 0.1;
    double dlength = detLength;
    if(length>dlength) {
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
    extendTo(detLength, laneContinuations);
}


MS_E2_ZS_CollectorOverLanes::~MS_E2_ZS_CollectorOverLanes( void )
{
}


void
MS_E2_ZS_CollectorOverLanes::extendTo(
        double length,
        const LaneContinuations &laneContinuations)
{
    bool done = false;
    while(!done) {
        done = true;
        LengthVector::iterator leni = myLengths.begin();
        LaneVectorVector::iterator lanei = myLaneCombinations.begin();
        DetectorVectorVector::iterator deti = myDetectorCombinations.begin();
        size_t c = 0;
        for(; leni!=myLengths.end(); leni++, lanei++, deti++) {
            if((*leni)<length) {
                done = false;
                // copy current values
                LaneVector lv = *lanei;
                DetectorVector dv = *deti;
                double clength = *leni;
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
                    getLanePredeccessorLanes(toExtend, laneContinuations);
                if(predeccessors.size()==0) {
                    int off = 1;
                    const MSEdge &e = toExtend->edge();
                    const std::vector<MSLane*> *lanes = e.getLanes();
                    int idx =
                        distance(lanes->begin(), find(lanes->begin(), lanes->end(), toExtend));
                    while(predeccessors.size()==0) {
                        if(idx-off>=0) {
                            MSLane *tryMe = (*lanes)[idx-off];
                            predeccessors =
                                getLanePredeccessorLanes(tryMe, laneContinuations);
                        }
                        if(predeccessors.size()==0&&idx+off<lanes->size()) {
                            MSLane *tryMe = (*lanes)[idx+off];
                            predeccessors =
                                getLanePredeccessorLanes(tryMe, laneContinuations);
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
                for(std::vector<MSLane*>::const_iterator i=predeccessors.begin(); i!=predeccessors.end(); i++) {
                    // get the lane
                    MSLane *l = *i;
                    // compute detector length
                    double lanelen = length - clength;
                    if(lanelen>l->length()) {
                        lanelen = l->length() - 0.2;
                    }
                    // build new info
                    LaneVector nlv = lv;
                    nlv.push_back(l);
                    DetectorVector ndv = dv;
                    MSE2Collector *coll = 0;
                    if(myAlreadyBuild.find(l)==myAlreadyBuild.end()) {
                        coll = buildCollector(0, 0, l, 0.1, lanelen);
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
        const LaneContinuations &laneContinuations)
{
    string eid = l->edge().id();
    // check whether any exist
    if(laneContinuations.find(eid)==laneContinuations.end()) {
        return std::vector<MSLane*>();
    }
    // get predecessing edges
    typedef std::vector<std::string> StringVector;
    const StringVector &predIDs = laneContinuations.find(eid)->second;
    std::vector<MSLane*> ret;
    // find predecessing lanes
    for(StringVector::const_iterator i=predIDs.begin(); i!=predIDs.end(); i++) {
        MSEdge *e = MSEdge::dictionary(*i);
        assert(e!=0);
        typedef std::vector<MSLane*> LaneVector;
        const LaneVector *cl = e->allowedLanes(l->edge());
        bool fastAbort = false;
        if(cl!=0) {
            for(LaneVector::const_iterator j=cl->begin(); !fastAbort&&j!=cl->end(); j++) {
                const MSLinkCont &lc = (*j)->getLinkCont();
                for(MSLinkCont::const_iterator k=lc.begin(); !fastAbort&&k!=lc.end(); k++) {
                    if((*k)->getLane()==l) {
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
                                            double start, double end)
{
    string id = makeID(l->id(), c, r);
    if(start+end<l->length()) {
        start = l->length() - end - 0.1;
    }
    return new MSE2Collector(id, myUsage,
        l, start, end, haltingTimeThresholdM,
        haltingSpeedThresholdM, jamDistThresholdM, deleteDataAfterSecondsM);
}


double
MS_E2_ZS_CollectorOverLanes::getCurrent( E2::DetType type )
{
    switch(type) {
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
        double myMax = 0;
        for(DetectorVectorVector::iterator i=myDetectorCombinations.begin(); i!=myDetectorCombinations.end(); i++) {
            double value = 0;
            for(DetectorVector::iterator j=(*i).begin(); j!=(*i).end(); j++) {
                value += (*j)->getCurrent(type);
            }
            if(myMax<value) {
                myMax = value;
            }
        }
        return myMax;
    }
    return -1;
}


double
MS_E2_ZS_CollectorOverLanes::getAggregate( E2::DetType type,
                                          MSUnit::Seconds lastNSeconds )
{
    switch(type) {
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
        double myMax = 0;
        for(DetectorVectorVector::iterator i=myDetectorCombinations.begin(); i!=myDetectorCombinations.end(); i++) {
            double value = 0;
            for(DetectorVector::iterator j=(*i).begin(); j!=(*i).end(); j++) {
                value += (*j)->getAggregate(type, lastNSeconds);
            }
            if(value>myMax) {
                myMax = value;
            }
        }
        return myMax;
    }
    return -1;
}


bool
MS_E2_ZS_CollectorOverLanes::hasDetector( E2::DetType type )
{
    return myDetectorCombinations[0][0]->hasDetector(type);
}


void
MS_E2_ZS_CollectorOverLanes::addDetector( E2::DetType type,
                                         std::string detId)
{
    size_t c = 0;
    for(LaneDetMap::iterator i=myAlreadyBuild.begin(); i!=myAlreadyBuild.end(); i++) {
        (*i).second->addDetector(type, makeID(detId, c, c));
        c++;
    }
}


std::string
MS_E2_ZS_CollectorOverLanes::getXMLOutput( MSUnit::IntSteps lastNTimesteps )
{
    std::string result;
    MSUnit::Seconds lastNSeconds =
        MSUnit::getInstance()->getSeconds( lastNTimesteps );
    if(hasDetector(E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES)) {
        result += std::string("collQueueLengthAheadOfTrafficLightsInVehiclesMax=\"") +
            toString( getCurrent(E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES) ) +
            std::string("\" ");
        resetQueueLengthAheadOfTrafficLights();
    }
    result += myDetectorCombinations[0][0]->getXMLOutput(lastNTimesteps);
    return result;
}


std::string
MS_E2_ZS_CollectorOverLanes::getXMLDetectorInfoStart( void ) const
{
    std::string
        detectorInfo("<detector type=\"E2_ZS_Collector\" id=\"" + myID +
                "\" startlane=\"" +
                myStartLaneID + "\" startpos=\"" +
                toString(startPosM) + "\" length=\"" +
                toString(myLength) +
                "\" >\n");
    return detectorInfo;
}


size_t bla = 0;

std::string
MS_E2_ZS_CollectorOverLanes::makeID( const std::string &baseID ,
                                    size_t c, size_t r ) const
{
    string add;
    switch(myUsage) {
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
MS_E2_ZS_CollectorOverLanes::getId() const
{
    return myID;
}


const std::string &
MS_E2_ZS_CollectorOverLanes::getStartLaneID() const
{
    return myStartLaneID;
}


void
MS_E2_ZS_CollectorOverLanes::resetQueueLengthAheadOfTrafficLights( void )
{
    for(DetectorVectorVector::iterator i=myDetectorCombinations.begin(); i!=myDetectorCombinations.end(); i++) {
        for(DetectorVector::iterator j=(*i).begin(); j!=(*i).end(); j++) {
            (*j)->resetQueueLengthAheadOfTrafficLights();
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
