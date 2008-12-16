/****************************************************************************/
/// @file    NLDetectorBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 15 Apr 2002
/// @version $Id$
///
// Builds detectors for microsim
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
#include <iostream>
#include <microsim/MSNet.h>
#include <microsim/output/MSInductLoop.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/output/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/output/MSVTypeProbe.h>
#include <microsim/output/MSRouteProbe.h>
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

#ifdef _MESSAGES
#include <microsim/output/MSMsgInductLoop.h>
#endif

#ifdef HAVE_MESOSIM
#include <mesosim/MEInductLoop.h>
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
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * NLDetectorBuilder::E3DetectorDefinition-methods
 * ----------------------------------------------------------------------- */
NLDetectorBuilder::E3DetectorDefinition::E3DetectorDefinition(const std::string &id,
        OutputDevice& device, MetersPerSecond haltingSpeedThreshold,
        SUMOTime haltingTimeThreshold, int splInterval) throw()
        : myID(id), myDevice(device),
        myHaltingSpeedThreshold(haltingSpeedThreshold),
        myHaltingTimeThreshold(haltingTimeThreshold),
        mySampleInterval(splInterval)
{}


NLDetectorBuilder::E3DetectorDefinition::~E3DetectorDefinition() throw()
{}


#ifdef _MESSAGES
void
NLDetectorBuilder::buildMsgDetector(const std::string &id,
                                    const std::string &lane, SUMOReal pos, int splInterval,
                                    const std::string &msg,
                                    OutputDevice& device, bool friendlyPos) throw(InvalidArgument)
{
#ifdef _DEBUG
    cout << "building the e4-detector..." << endl;
#endif
    if (splInterval<0) {
        throw InvalidArgument("Negative sampling frequency (in e4-detector '" + id + "').");
    }
    if (splInterval==0) {
        throw InvalidArgument("Sampling frequency must not be zero (in e4-detector '" + id + "').");
    }
    if (msg == "") {
        throw InvalidArgument("No Message given (in e4-detector '" + id + "').");
    }
    MSLane *clane = getLaneChecking(lane, id);
    if (pos<0) {
        pos = clane->length() + pos;
    }
    pos = getPositionChecking(pos, clane, friendlyPos, id);
    MSMsgInductLoop *msgloop = createMsgInductLoop(id, msg, clane, pos);
    myNet.getDetectorControl().add(msgloop, device, splInterval);
}
#endif


/* -------------------------------------------------------------------------
 * NLDetectorBuilder-methods
 * ----------------------------------------------------------------------- */
NLDetectorBuilder::NLDetectorBuilder(MSNet &net) throw()
        : myNet(net), myE3Definition(0)
{}


NLDetectorBuilder::~NLDetectorBuilder() throw()
{}


void
NLDetectorBuilder::buildInductLoop(const std::string &id,
                                   const std::string &lane, SUMOReal pos, int splInterval,
                                   OutputDevice& device, bool friendlyPos) throw(InvalidArgument)
{
    if (splInterval<0) {
        throw InvalidArgument("Negative sampling frequency (in e1-detector '" + id + "').");
    }
    if (splInterval==0) {
        throw InvalidArgument("Sampling frequency must not be zero (in e1-detector '" + id + "').");
    }
    // get and check the lane
    MSLane *clane = getLaneChecking(lane, id);
    if (pos<0) {
        pos = clane->length() + pos;
    }
#ifdef HAVE_MESOSIM
    if (!MSGlobals::gUseMesoSim) {
#endif
        // get and check the position
        pos = getPositionChecking(pos, clane, friendlyPos, id);
        // build the loop
        MSInductLoop *loop = createInductLoop(id, clane, pos);
        // add the file output
        myNet.getDetectorControl().add(loop, device, splInterval);
#ifdef HAVE_MESOSIM
    } else {
        if (pos<0) {
            pos = clane->length() + pos;
        }
        MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(clane->getEdge());
        MESegment *prev = s;
        SUMOReal cpos = 0;
        while (cpos+prev->getLength()<pos&&s!=0) {
            prev = s;
            cpos += s->getLength();
            s = s->getNextSegment();
        }
        SUMOReal rpos = pos-cpos;//-prev->getLength();
        if (rpos>prev->getLength()||rpos<0) {
            if (friendlyPos) {
                rpos = prev->getLength() - (SUMOReal) 0.1;
            } else {
                throw InvalidArgument("The position of detector '" + id + "' lies beyond the lane's '" + lane + "' length.");
            }
        }
        MEInductLoop *loop =
            createMEInductLoop(id, prev, rpos);
        myNet.getDetectorControl().add(loop, device, splInterval);
    }
#endif
}


void
NLDetectorBuilder::buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                                   const std::string &id,
                                   const std::string &lane, SUMOReal pos, SUMOReal length,
                                   bool cont, int splInterval,
                                   OutputDevice& device,
                                   SUMOTime haltingTimeThreshold,
                                   MetersPerSecond haltingSpeedThreshold,
                                   SUMOReal jamDistThreshold, bool friendlyPos) throw(InvalidArgument)
{
    if (splInterval<0) {
        throw InvalidArgument("Negative sampling frequency (in e2-detector '" + id + "').");
    }
    if (splInterval==0) {
        throw InvalidArgument("Sampling frequency must not be zero (in e2-detector '" + id + "').");
    }
    MSLane *clane = getLaneChecking(lane, id);
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput *det = 0;
    if (!cont) {
        convUncontE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED,
                                   clane, pos, length,
                                   haltingTimeThreshold, haltingSpeedThreshold,
                                   jamDistThreshold);
        myNet.getDetectorControl().add(
            static_cast<MSE2Collector*>(det), device, splInterval);
    } else {
        convContE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildMultiLaneE2Det(edgeContinuations, id, DU_USER_DEFINED,
                                  clane, pos, length,
                                  haltingTimeThreshold, haltingSpeedThreshold,
                                  jamDistThreshold);
        myNet.getDetectorControl().add(
            static_cast<MS_E2_ZS_CollectorOverLanes*>(det), device, splInterval);
    }
}


void
NLDetectorBuilder::buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                                   const std::string &id,
                                   const std::string &lane, SUMOReal pos, SUMOReal length,
                                   bool cont,
                                   MSTLLogicControl::TLSLogicVariants &tlls,
                                   OutputDevice& device,
                                   SUMOTime haltingTimeThreshold,
                                   MetersPerSecond haltingSpeedThreshold,
                                   SUMOReal jamDistThreshold, bool friendlyPos) throw(InvalidArgument)
{
    if (tlls.getActive()==0) {
        throw InvalidArgument("The detector '" + id + "' refers to the unknown lsa.");
    }
    MSLane *clane = getLaneChecking(lane, id);
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput *det = 0;
    if (!cont) {
        convUncontE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED,
                                   clane, pos, length,
                                   haltingTimeThreshold, haltingSpeedThreshold,
                                   jamDistThreshold);
        myNet.getDetectorControl().add(
            static_cast<MSE2Collector*>(det));
    } else {
        convContE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildMultiLaneE2Det(edgeContinuations, id, DU_USER_DEFINED,
                                  clane, pos, length,
                                  haltingTimeThreshold, haltingSpeedThreshold,
                                  jamDistThreshold);
        myNet.getDetectorControl().add(
            static_cast<MS_E2_ZS_CollectorOverLanes*>(det));
    }
    // add the file output
    new Command_SaveTLCoupledDet(tlls, det,
                                 myNet.getCurrentTimeStep(), device);
}


void
NLDetectorBuilder::buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                                   const std::string &id,
                                   const std::string &lane, SUMOReal pos, SUMOReal length,
                                   bool cont,
                                   MSTLLogicControl::TLSLogicVariants &tlls,
                                   const std::string &tolane,
                                   OutputDevice& device,
                                   SUMOTime haltingTimeThreshold,
                                   MetersPerSecond haltingSpeedThreshold,
                                   SUMOReal jamDistThreshold, bool friendlyPos) throw(InvalidArgument)
{
    if (tlls.getActive()==0) {
        throw InvalidArgument("The detector '" + id + "' refers to the unknown lsa.");
    }
    MSLane *clane = getLaneChecking(lane, id);
    MSLane *ctoLane = getLaneChecking(tolane, id);
    MSLink *link = MSLinkContHelper::getConnectingLink(*clane, *ctoLane);
    if (link==0) {
        throw InvalidArgument(
            "The detector output can not be build as no connection between lanes '"
            + lane + "' and '" + tolane + "' exists.");
    }
    if (pos<0) {
        pos = -pos;
    }
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput *det = 0;
    if (!cont) {
        convUncontE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED,
                                   clane, pos, length,
                                   haltingTimeThreshold, haltingSpeedThreshold,
                                   jamDistThreshold);
        myNet.getDetectorControl().add(static_cast<MSE2Collector*>(det));
    } else {
        convContE2PosLength(id, clane, pos, length, friendlyPos);
        det = buildMultiLaneE2Det(edgeContinuations, id, DU_USER_DEFINED,
                                  clane, pos, length,
                                  haltingTimeThreshold, haltingSpeedThreshold,
                                  jamDistThreshold);
        myNet.getDetectorControl().add(static_cast<MS_E2_ZS_CollectorOverLanes*>(det));
    }
    // add the file output
    new Command_SaveTLCoupledLaneDet(tlls, det,
                                     myNet.getCurrentTimeStep(), device, link);
}


void
NLDetectorBuilder::convUncontE2PosLength(const std::string &id, MSLane *clane,
        SUMOReal &pos, SUMOReal &length,
        bool friendlyPos) throw(InvalidArgument)
{
    // get and check the position
    pos = getPositionChecking(pos, clane, friendlyPos, id);
    // check length
    if (length<0) {
        length = clane->length() + length;
    }
    if (length+pos>clane->length()) {
        if (friendlyPos) {
            length = clane->length() - pos - (SUMOReal) 0.1;
        } else {
            throw InvalidArgument("The length of detector '" + id + "' lies beyond the lane's '" + clane->getID() + "' length.");
        }
    }
    if (length<0) {
        if (friendlyPos) {
            length = (SUMOReal) 0.1;
        } else {
            throw InvalidArgument("The length of detector '" + id + "' is almost 0.");
        }
    }
}


void
NLDetectorBuilder::convContE2PosLength(const std::string &id, MSLane * clane,
                                       SUMOReal &pos, SUMOReal &length,
                                       bool friendlyPos) throw(InvalidArgument)
{
    // get and check the position
    pos = getPositionChecking(pos, clane, friendlyPos, id);
    // length will be kept as is
}


void
NLDetectorBuilder::beginE3Detector(const std::string &id,
                                   OutputDevice& device, int splInterval,
                                   MetersPerSecond haltingSpeedThreshold,
                                   SUMOTime haltingTimeThreshold) throw(InvalidArgument)
{
    if (splInterval<0) {
        throw InvalidArgument("Negative sampling frequency (in e3-detector '" + id + "').");
    }
    if (splInterval==0) {
        throw InvalidArgument("Sampling frequency must not be zero (in e3-detector '" + id + "').");
    }
    myE3Definition = new E3DetectorDefinition(id, device,
            haltingSpeedThreshold, haltingTimeThreshold,
            splInterval);
}


void
NLDetectorBuilder::addE3Entry(const std::string &lane,
                              SUMOReal pos, bool friendlyPos) throw(InvalidArgument)
{
    if (myE3Definition==0) {
        return;
    }
    MSLane *clane = getLaneChecking(lane, myE3Definition->myID);
    // get and check the position
    pos = getPositionChecking(pos, clane, friendlyPos, myE3Definition->myID);
    // build and save the entry
    myE3Definition->myEntries.push_back(MSCrossSection(clane, pos));
}


void
NLDetectorBuilder::addE3Exit(const std::string &lane,
                             SUMOReal pos, bool friendlyPos) throw(InvalidArgument)
{
    if (myE3Definition==0) {
        return;
    }
    MSLane *clane = getLaneChecking(lane, myE3Definition->myID);
    // get and check the position
    pos = getPositionChecking(pos, clane, friendlyPos, myE3Definition->myID);
    // build and save the exit
    myE3Definition->myExits.push_back(MSCrossSection(clane, pos));
}


std::string
NLDetectorBuilder::getCurrentE3ID() const throw()
{
    if (myE3Definition==0) {
        return "<unknown>";
    }
    return myE3Definition->myID;
}


void
NLDetectorBuilder::endE3Detector() throw(InvalidArgument)
{
    if (myE3Definition==0) {
        return;
    }
    MSE3Collector *det = createE3Detector(myE3Definition->myID,
                                          myE3Definition->myEntries, myE3Definition->myExits,
                                          myE3Definition->myHaltingSpeedThreshold, myE3Definition->myHaltingTimeThreshold);
    // add to net
    myNet.getDetectorControl().add(
        static_cast<MSE3Collector*>(det), myE3Definition->myDevice, myE3Definition->mySampleInterval);
    // clean up
    delete myE3Definition;
    myE3Definition = 0;
}


void
NLDetectorBuilder::buildVTypeProbe(const std::string &id,
                                   const std::string &vtype, SUMOTime frequency,
                                   OutputDevice& device) throw(InvalidArgument)
{
    if (frequency<0) {
        throw InvalidArgument("Negative frequency (in vtypeprobe '" + id + "').");
    }
    if (frequency==0) {
        throw InvalidArgument("Frequency must not be zero (in vtypeprobe '" + id + "').");
    }
    MSVTypeProbe *probe = new MSVTypeProbe(id, vtype);
    // add the file output
    myNet.getDetectorControl().add(probe, device, frequency);
}


void
NLDetectorBuilder::buildRouteProbe(const std::string &id,
                                   const std::string &edge, SUMOTime frequency,
                                   OutputDevice& device) throw(InvalidArgument)
{
    if (frequency<=0) {
        throw InvalidArgument("Frequency must be larger than zero (in routeprobe '" + id + "').");
    }
    MSEdge *e = MSEdge::dictionary(edge);
    if (e==0) {
        throw InvalidArgument("The edge with the id '" + edge + "' is not known (in routeprobe '" + id + "').");
    }
    MSRouteProbe *probe = new MSRouteProbe(id, e);
    // add the file output
    myNet.getDetectorControl().add(probe, device, frequency);
}


void 
NLDetectorBuilder::buildEdgeMeanData(const std::string &id, SUMOTime frequency,
                                     const std::string &begins, const std::string &ends,
                                     const std::string &type, 
                                     const std::string &edges, bool excludeEmpty, 
                                     OutputDevice& device) throw(InvalidArgument)
{
    createEdgeMeanData(id, frequency, begins, ends, type, edges, excludeEmpty, device);
}


void 
NLDetectorBuilder::buildLaneMeanData(const std::string &id, SUMOTime frequency,
                                     const std::string &begins, const std::string &ends,
                                     const std::string &type, 
                                     const std::string &edges, bool excludeEmpty, 
                                     OutputDevice& device) throw(InvalidArgument)
{
    createLaneMeanData(id, frequency, begins, ends, type, edges, excludeEmpty, device);
}




// -------------------
MSE2Collector *
NLDetectorBuilder::buildSingleLaneE2Det(const std::string &id,
                                        DetectorUsage usage,
                                        MSLane *lane, SUMOReal pos, SUMOReal length,
                                        SUMOTime haltingTimeThreshold,
                                        MetersPerSecond haltingSpeedThreshold,
                                        SUMOReal jamDistThreshold) throw()
{
    return createSingleLaneE2Detector(id, usage, lane, pos,
                                      length, haltingTimeThreshold, haltingSpeedThreshold,
                                      jamDistThreshold);
}


MS_E2_ZS_CollectorOverLanes *
NLDetectorBuilder::buildMultiLaneE2Det(const MSEdgeContinuations &edgeContinuations,
                                       const std::string &id, DetectorUsage usage,
                                       MSLane *lane, SUMOReal pos, SUMOReal length,
                                       SUMOTime haltingTimeThreshold,
                                       MetersPerSecond haltingSpeedThreshold,
                                       SUMOReal jamDistThreshold) throw()
{
    MS_E2_ZS_CollectorOverLanes *ret = createMultiLaneE2Detector(id, usage,
                                       lane, pos, haltingTimeThreshold, haltingSpeedThreshold,
                                       jamDistThreshold);
    ret->init(lane, length, edgeContinuations);
    return ret;
}

#ifdef _MESSAGES
MSMsgInductLoop *
NLDetectorBuilder::createMsgInductLoop(const std::string &id, const std::string &msg,
                                       MSLane *lane, SUMOReal pos) throw()
{
    return new MSMsgInductLoop(id, msg, lane, pos);
}
#endif


MSInductLoop *
NLDetectorBuilder::createInductLoop(const std::string &id,
                                    MSLane *lane, SUMOReal pos) throw()
{
    return new MSInductLoop(id, lane, pos);
}


#ifdef HAVE_MESOSIM
MEInductLoop *
NLDetectorBuilder::createMEInductLoop(const std::string &id,
                                      MESegment *s, SUMOReal pos) throw()
{
    return new MEInductLoop(id, s, pos);
}
#endif


MSE2Collector *
NLDetectorBuilder::createSingleLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, SUMOReal pos, SUMOReal length,
        SUMOTime haltingTimeThreshold,
        MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold) throw()
{
    return new MSE2Collector(id, usage, lane, pos, length,
                             haltingTimeThreshold, haltingSpeedThreshold,
                             jamDistThreshold);

}


MS_E2_ZS_CollectorOverLanes *
NLDetectorBuilder::createMultiLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, SUMOReal pos,
        SUMOTime haltingTimeThreshold,
        MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold) throw()
{
    return new MS_E2_ZS_CollectorOverLanes(id, usage, lane, pos,
                                           haltingTimeThreshold, haltingSpeedThreshold,
                                           jamDistThreshold);
}


MSE3Collector *
NLDetectorBuilder::createE3Detector(const std::string &id,
                                    const CrossSectionVector &entries,
                                    const CrossSectionVector &exits,
                                    MetersPerSecond haltingSpeedThreshold,
                                    SUMOTime haltingTimeThreshold) throw()
{
    return new MSE3Collector(id, entries, exits, haltingSpeedThreshold, haltingTimeThreshold);
}


MSLane *
NLDetectorBuilder::getLaneChecking(const std::string &id,
                                   const std::string &detid) throw(InvalidArgument)
{
    // get and check the lane
    MSLane *clane = MSLane::dictionary(id);
    if (clane==0) {
        throw InvalidArgument("The lane with the id '" + id + "' is not known (while building detector '" + detid + "').");
    }
    return clane;
}


SUMOReal
NLDetectorBuilder::getPositionChecking(SUMOReal pos, MSLane *lane, bool friendlyPos,
                                       const std::string &detid) throw(InvalidArgument)
{
    // check whether it is given from the end
    if (pos<0) {
        pos = lane->length() + pos;
    }
    // check whether it is on the lane
    if (pos>lane->length()) {
        if (friendlyPos) {
            pos = lane->length() - (SUMOReal) 0.1;
        } else {
            throw InvalidArgument("The position of detector '" + detid + "' lies beyond the lane's '" + lane->getID() + "' length.");
        }
    }
    if (pos<0) {
        if (friendlyPos) {
            pos = (SUMOReal) 0.1;
        } else {
            throw InvalidArgument("The position of detector '" + detid + "' lies beyond the lane's '" + lane->getID() + "' length.");
        }
    }
    return pos;
}


void 
NLDetectorBuilder::createEdgeMeanData(const std::string &id, SUMOTime frequency,
                                      const std::string &begins, const std::string &ends,
                                      const std::string &type, 
                                      const std::string &edges, bool excludeEmpty,
                                      OutputDevice& device) throw(InvalidArgument)
{
    pair<vector<SUMOTime>, vector<SUMOTime> > timeBounds = getTimeBounds(begins, ends, id, "meandata-lane");
    MSMeanData_Net *det = new MSMeanData_Net(id, frequency, MSNet::getInstance()->getEdgeControl(), 
        timeBounds.first, timeBounds.second, false, !excludeEmpty, !excludeEmpty);
    MSNet::getInstance()->getDetectorControl().addDetectorAndInterval(det, &device, frequency);
}


void 
NLDetectorBuilder::createLaneMeanData(const std::string &id, SUMOTime frequency,
                                      const std::string &begins, const std::string &ends,
                                      const std::string &type, 
                                      const std::string &edges, bool excludeEmpty,
                                      OutputDevice& device) throw(InvalidArgument)
{
    pair<vector<SUMOTime>, vector<SUMOTime> > timeBounds = getTimeBounds(begins, ends, id, "meandata-lane");
    MSMeanData_Net *det = new MSMeanData_Net(id, frequency, MSNet::getInstance()->getEdgeControl(), 
        timeBounds.first, timeBounds.second, true, !excludeEmpty, !excludeEmpty);
    MSNet::getInstance()->getDetectorControl().addDetectorAndInterval(det, &device, frequency);
}


std::vector<SUMOTime>
NLDetectorBuilder::parseTimeList(const std::string &l) throw(EmptyData, NumberFormatException)
{
    std::vector<SUMOTime> ret;
    if(l.length()!=0) {
        StringTokenizer st(l, ",");
        while(st.hasNext()) {
            ret.push_back(TplConvert<char>::_2int(st.next().c_str()));
        }
    }
    return ret;
}

std::pair<std::vector<SUMOTime>, std::vector<SUMOTime> >
NLDetectorBuilder::getTimeBounds(const std::string &begins, const std::string &ends,
                                 const std::string &id, const std::string &type) throw(InvalidArgument)
{
    StringTokenizer st(begins, ";");
    vector<SUMOTime> dumpBegins, dumpEnds;
    try {
        dumpBegins = parseTimeList(begins);
    } catch (NumberFormatException &) {
        throw InvalidArgument("Not numeric 'begin' in definition of " + type + " '" + id + "'.");
    }
    try {
        dumpEnds = parseTimeList(ends);
    } catch (NumberFormatException &) {
        throw InvalidArgument("Not numeric 'end' in definition of " + type + " '" + id + "'.");
    }
    if (dumpBegins.size()!=dumpEnds.size()) {
        throw InvalidArgument("The number of entries in 'begin' differs from the number in 'end' for " + type + " '" + id + "'.");
    }
    size_t noConstraints = dumpBegins.size();
    for (size_t i=0; i<noConstraints; i++) {
        if (dumpBegins[i]>=dumpEnds[i]) {
            throw InvalidArgument("The 'begin' at position " + toString(i+1) + " is not smaller than the according 'end' for " + type + " '" + id + "'.");
        }
    }
    return pair<vector<SUMOTime>, vector<SUMOTime> >(dumpBegins, dumpEnds);
}


/****************************************************************************/

