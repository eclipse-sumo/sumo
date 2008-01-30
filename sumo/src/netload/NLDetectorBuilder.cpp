/****************************************************************************/
/// @file    NLDetectorBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 15 Apr 2002
/// @version $Id$
///
// A building helper for the detectors
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
#include <microsim/MSGlobals.h>
#include <microsim/actions/Command_SaveTLCoupledDet.h>
#include <microsim/actions/Command_SaveTLCoupledLaneDet.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include <utils/common/UtilExceptions.h>
#include "NLDetectorBuilder.h"
#include <microsim/output/MSDetectorControl.h>

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
NLDetectorBuilder::E3DetectorDefinition::E3DetectorDefinition(
    const std::string &id,
    OutputDevice& device,
    MetersPerSecond haltingSpeedThreshold,
    int splInterval) :
        myID(id), myDevice(device),
        myHaltingSpeedThreshold(haltingSpeedThreshold),
        mySampleInterval(splInterval)
{}


NLDetectorBuilder::E3DetectorDefinition::~E3DetectorDefinition()
{}


/* -------------------------------------------------------------------------
 * NLDetectorBuilder-methods
 * ----------------------------------------------------------------------- */
NLDetectorBuilder::NLDetectorBuilder(MSNet &net)
        : myNet(net), myE3Definition(0)
{}


NLDetectorBuilder::~NLDetectorBuilder()
{}


void
NLDetectorBuilder::buildInductLoop(const std::string &id,
                                   const std::string &lane, SUMOReal pos, int splInterval,
                                   OutputDevice& device, bool friendly_pos,
                                   const std::string &/*style*/)
{
    if (splInterval<0) {
        throw InvalidArgument("Negative sampling frequency (in detector '" + id + "').");
    }
    // get and check the lane
    MSLane *clane = getLaneChecking(lane, id);
    if (pos<0) {
        pos = clane->length() + pos;
    }
#ifdef HAVE_MESOSIM
    if (!MSGlobals::gUseMesoSim) {
#endif
        // get the output style
//   MSDetector::OutputStyle cstyle = convertStyle(id, style);
        // check whether the file must be converted into a relative path
        // compute position
        if (pos>clane->length()) {
            if (friendly_pos) {
                pos = clane->length() - (SUMOReal) 0.1;
            } else {
                throw InvalidArgument("The position of detector '" + id + "' lies beyond the lane's '" + lane + "' length.");
            }
        }
        // build the loop
        MSInductLoop *loop = createInductLoop(id, clane, pos);
        // add the file output
        myNet.getDetectorControl().add(loop, device, splInterval);
#ifdef HAVE_MESOSIM
    } else {
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
            if (friendly_pos) {
                rpos = prev->getLength() - (SUMOReal) 0.1;
            } else {
                throw InvalidArgument("The position of detector '" + id + "' lies beyond the lane's '" + lane + "' length.");
            }
        }
        MEInductLoop *loop =
            createMEInductLoop(id, prev, rpos, splInterval);
        myNet.getDetectorControl().add(loop, device, splInterval);
    }
#endif
}


void
NLDetectorBuilder::buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                                   const std::string &id,
                                   const std::string &lane, SUMOReal pos, SUMOReal length,
                                   bool cont, int splInterval,
                                   const std::string &/*style*/,
                                   OutputDevice& device,
                                   const std::string &measures,
                                   SUMOTime haltingTimeThreshold,
                                   MetersPerSecond haltingSpeedThreshold,
                                   SUMOReal jamDistThreshold)
{
    if (splInterval<0) {
        throw InvalidArgument("Negative sampling frequency (in detector '" + id + "').");
    }
    MSLane *clane = getLaneChecking(lane, id);
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput *det = 0;
    if (!cont) {
        convUncontE2PosLength(id, clane, pos, length);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED,
                                   clane, pos, length,
                                   haltingTimeThreshold, haltingSpeedThreshold,
                                   jamDistThreshold, measures);
        myNet.getDetectorControl().add(
            static_cast<MSE2Collector*>(det), device, splInterval);
    } else {
        convContE2PosLength(id, clane, pos, length);
        det = buildMultiLaneE2Det(edgeContinuations, id, DU_USER_DEFINED,
                                  clane, pos, length,
                                  haltingTimeThreshold, haltingSpeedThreshold,
                                  jamDistThreshold, measures);
        myNet.getDetectorControl().add(
            static_cast<MS_E2_ZS_CollectorOverLanes*>(det), device, splInterval);
    }
}


void
NLDetectorBuilder::buildE2Detector(const MSEdgeContinuations &edgeContinuations,
                                   const std::string &id,
                                   const std::string &lane, SUMOReal pos, SUMOReal length,
                                   bool cont,
                                   const MSTLLogicControl::TLSLogicVariants &tlls,
                                   const std::string &/*style*/, OutputDevice& device,
                                   const std::string &measures,
                                   SUMOTime haltingTimeThreshold,
                                   MetersPerSecond haltingSpeedThreshold,
                                   SUMOReal jamDistThreshold)
{
    MSLane *clane = getLaneChecking(lane, id);
    // check whether the detector may lie over more than one lane
    MSDetectorFileOutput *det = 0;
    if (!cont) {
        convUncontE2PosLength(id, clane, pos, length);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED,
                                   clane, pos, length,
                                   haltingTimeThreshold, haltingSpeedThreshold,
                                   jamDistThreshold, measures);
        myNet.getDetectorControl().add(
            static_cast<MSE2Collector*>(det));
    } else {
        convContE2PosLength(id, clane, pos, length);
        det = buildMultiLaneE2Det(edgeContinuations, id, DU_USER_DEFINED,
                                  clane, pos, length,
                                  haltingTimeThreshold, haltingSpeedThreshold,
                                  jamDistThreshold, measures);
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
                                   const MSTLLogicControl::TLSLogicVariants &tlls,
                                   const std::string &tolane,
                                   const std::string &/*style*/, OutputDevice& device,
                                   const std::string &measures,
                                   SUMOTime haltingTimeThreshold,
                                   MetersPerSecond haltingSpeedThreshold,
                                   SUMOReal jamDistThreshold)
{
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
        convUncontE2PosLength(id, clane, pos, length);
        det = buildSingleLaneE2Det(id, DU_USER_DEFINED,
                                   clane, pos, length,
                                   haltingTimeThreshold, haltingSpeedThreshold,
                                   jamDistThreshold, measures);
        myNet.getDetectorControl().add(
            static_cast<MSE2Collector*>(det));
    } else {
        convContE2PosLength(id, clane, pos, length);
        det = buildMultiLaneE2Det(edgeContinuations, id, DU_USER_DEFINED,
                                  clane, pos, length,
                                  haltingTimeThreshold, haltingSpeedThreshold,
                                  jamDistThreshold, measures);
        myNet.getDetectorControl().add(
            static_cast<MS_E2_ZS_CollectorOverLanes*>(det));
    }
    // add the file output
    new Command_SaveTLCoupledLaneDet(tlls, det,
                                     myNet.getCurrentTimeStep(), device, link);
}


void
NLDetectorBuilder::convUncontE2PosLength(const std::string &id,
        MSLane *clane,
        SUMOReal &pos,
        SUMOReal &length)
{
    if (pos<0) {
        pos = clane->length() + pos;
    }
    if (pos>clane->length()) {
        throw InvalidArgument("Detector '" + id + "' lies beyond lane's '" + clane->getID() + "' end.");
    }
    // compute length
    if (length<0) {
        pos = pos + length;
        length *= -1;
    }
    // patch position
    if (pos<0.1) {
        pos = (SUMOReal) 0.1;
    }
    // patch length
    /* !!! friendly_pos
    if (pos+length>clane->length()-(SUMOReal) 0.1) {
        length = clane->length() - (SUMOReal) 0.1 - pos;
    }
    */
    if (length<=0) {
        throw InvalidArgument("The length of detector '" + id + "' is not positive.");
    }
    if (pos+length>clane->length()) {
        throw InvalidArgument("Detector's '" + id + "' end lies beyond lane's '" + clane->getID() + "' end.");
    }
}


void
NLDetectorBuilder::convContE2PosLength(const std::string &id,
                                       MSLane * /*clane*/,
                                       SUMOReal &pos,
                                       SUMOReal &length)
{
    if (pos<0) {
        pos *= -1.0;//clane->length() + pos;
    }
    if (length<=0) {
        throw InvalidArgument("The length of the continuated detector " + id + " is not positive.");
    }
}


void
NLDetectorBuilder::beginE3Detector(const std::string &id,
                                   OutputDevice& device, int splInterval,
                                   const std::string &measures,
                                   MetersPerSecond haltingSpeedThreshold)
{
    if (splInterval<0) {
        throw InvalidArgument("Negative sampling frequency (in detector '" + id + "').");
    }
    myE3Definition = new E3DetectorDefinition(id, device,
            haltingSpeedThreshold,
            splInterval);
}


void
NLDetectorBuilder::addE3Entry(const std::string &lane,
                              SUMOReal pos)
{
    if (myE3Definition==0) {
        return;
    }
    MSLane *clane = getLaneChecking(lane, myE3Definition->myID);
    if (pos<0) {
        pos = clane->length() + pos;
    }
    if (pos>clane->length()) {
        throw InvalidArgument("The position of detector '" + myE3Definition->myID + "' lies beyond the lane's '" + lane + "' length.");
    }
    myE3Definition->myEntries.push_back(MSCrossSection(clane, pos));
}


void
NLDetectorBuilder::addE3Exit(const std::string &lane,
                             SUMOReal pos)
{
    if (myE3Definition==0) {
        return;
    }
    MSLane *clane = getLaneChecking(lane, myE3Definition->myID);
    if (pos<0) {
        pos = clane->length() + pos;
    }
    if (pos>clane->length()) {
        throw InvalidArgument("The position of detector '" + myE3Definition->myID + "' lies beyond the lane's '" + lane + "' length.");
    }
    myE3Definition->myExits.push_back(MSCrossSection(clane, pos));
}


std::string
NLDetectorBuilder::getCurrentE3ID() const
{
    if (myE3Definition==0) {
        return "<unknown>";
    }
    return myE3Definition->myID;
}


void
NLDetectorBuilder::endE3Detector()
{
    if (myE3Definition==0) {
        return;
    }
    MSE3Collector *det = createE3Detector(
                             myE3Definition->myID,
                             myE3Definition->myEntries,
                             myE3Definition->myExits,
                             myE3Definition->myHaltingSpeedThreshold);
    // add to net
    myNet.getDetectorControl().add(
        static_cast<MSE3Collector*>(det), myE3Definition->myDevice, myE3Definition->mySampleInterval);
    // clean up
    delete myE3Definition;
    myE3Definition = 0;
}

// -------------------

MSE2Collector *
NLDetectorBuilder::buildSingleLaneE2Det(const std::string &id,
                                        DetectorUsage usage,
                                        MSLane *lane, SUMOReal pos, SUMOReal length,
                                        SUMOTime haltingTimeThreshold,
                                        MetersPerSecond haltingSpeedThreshold,
                                        SUMOReal jamDistThreshold,
                                        const std::string &measures)
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
                                       SUMOReal jamDistThreshold ,
                                       const std::string &measures)
{
    MS_E2_ZS_CollectorOverLanes *ret = createMultiLaneE2Detector(id, usage,
                                       lane, pos, haltingTimeThreshold, haltingSpeedThreshold,
                                       jamDistThreshold);
    ret->init(lane, length, edgeContinuations);
    return ret;
}


MSInductLoop *
NLDetectorBuilder::createInductLoop(const std::string &id,
                                    MSLane *lane, SUMOReal pos)
{
    return new MSInductLoop(id, lane, pos);
}


#ifdef HAVE_MESOSIM
MEInductLoop *
NLDetectorBuilder::createMEInductLoop(const std::string &id,
                                      MESegment *s, SUMOReal pos,
                                      int splInterval)
{
    return new MEInductLoop(id, s, pos);
}
#endif


MSE2Collector *
NLDetectorBuilder::createSingleLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, SUMOReal pos, SUMOReal length,
        SUMOTime haltingTimeThreshold,
        MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold)
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
        SUMOReal jamDistThreshold)
{
    return new MS_E2_ZS_CollectorOverLanes(id, usage, lane, pos,
                                           haltingTimeThreshold, haltingSpeedThreshold,
                                           jamDistThreshold);
}


MSE3Collector *
NLDetectorBuilder::createE3Detector(const std::string &id,
                                    const CrossSectionVector &entries,
                                    const CrossSectionVector &exits,
                                    MetersPerSecond haltingSpeedThreshold)
{
    return new MSE3Collector(id, entries, exits, haltingSpeedThreshold);
}



/*
MSDetector::OutputStyle NLDetectorBuilder::convertStyle(const std::string &id,
const std::string &style)
{
if(style=="GNUPLOT" || style=="GPLOT")
return MSDetector::GNUPLOT;
if(style=="CSV")
return MSDetector::CSV;
throw InvalidArgument("Unknown output style '" + style + "' while parsing the detector '" + id + "' occured.");
}
*/


MSLane *
NLDetectorBuilder::getLaneChecking(const std::string &id,
                                   const std::string &detid)
{
    // get and check the lane
    MSLane *clane = MSLane::dictionary(id);
    if (clane==0) {
        throw InvalidArgument("The lane with the id '" + id + "' is not known (while building detector '" + detid + "').");
    }
    return clane;
}



/****************************************************************************/

