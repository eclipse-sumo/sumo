#include "MS_E2_ZS_CollectorOverLanes.h"
#include "MSDetectorSubSys.h"
//#include <utils/common/DOubleVector.h>

using namespace std;

MS_E2_ZS_CollectorOverLanes::MS_E2_ZS_CollectorOverLanes(
        std::string id, MSLane* lane, MSUnit::Meters startPos,
        //MSUnit::Meters detLength, const LaneContinuations &laneContinuations,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds)
        : startPosM(startPos),
        deleteDataAfterSecondsM(deleteDataAfterSeconds),
        haltingTimeThresholdM(haltingTimeThreshold),
        haltingSpeedThresholdM(haltingSpeedThreshold),
        jamDistThresholdM(jamDistThreshold),
        myID(id), myStartLaneID(lane->id())
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

//    myTmpArray.reserve(myLengths.size());
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
                LaneContinuations::const_iterator conts =
                    laneContinuations.find(toExtend->id());
                assert(conts!=laneContinuations.end());
                const std::vector<std::string> &predeccessors =
                    (*conts).second;
                // go through the predeccessors and extend the detector
                for(std::vector<std::string>::const_iterator i=predeccessors.begin(); i!=predeccessors.end(); i++) {
                    // get the lane
                    MSLane *l = MSLane::dictionary(*i);
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


MSE2Collector *
MS_E2_ZS_CollectorOverLanes::buildCollector(size_t c, size_t r, MSLane *l,
                                            double start, double end)
{
    string id = makeID(l->id(), c, r);
    if(start+end<l->length()) {
        start = l->length() - end - 0.1;
    }
    return new MSE2Collector(id,
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
string ret =  baseID + toString<size_t>(bla++);
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
