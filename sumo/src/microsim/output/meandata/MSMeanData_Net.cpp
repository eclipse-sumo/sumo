/****************************************************************************/
/// @file    MSMeanData_Net.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Redirector for mean data output (net->edgecontrol)
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

#include <cassert>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Net.h"
#include <limits>

#ifdef HAVE_MESOSIM
#include <microsim/MSGlobals.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
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
MSMeanData_Net::MSMeanData_Net(unsigned int t, unsigned int index,
                               MSEdgeControl &edges,
                               const std::vector<int> &dumpBegins,
                               const std::vector<int> &dumpEnds,
                               bool useLanes,
                               bool addHeaderTail)
        : myInterval(t), myUseHeader(addHeaderTail), myIndex(index),
        myEdges(edges), myAmEdgeBased(!useLanes),
        myDumpBegins(dumpBegins), myDumpEnds(dumpEnds)
{}


MSMeanData_Net::~MSMeanData_Net()
{}


void
MSMeanData_Net::resetOnly(const MSEdge &edge, SUMOTime /*stopTime*/)
{
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(&edge);
        while (s!=0) {
            MSLaneMeanDataValues& meanData = s->getMeanData(myIndex);
            s = s->getNextSegment();
            meanData.reset();
        }
    } else {
#endif
        const MSEdge::LaneCont * const lanes = edge.getLanes();
        MSEdge::LaneCont::const_iterator lane;
        for (lane = lanes->begin(); lane != lanes->end(); ++lane) {
            MSLaneMeanDataValues& meanData = (*lane)->getMeanData(myIndex);
            meanData.reset();
        }
#ifdef HAVE_MESOSIM
    }
#endif
}


void
MSMeanData_Net::resetOnly(SUMOTime stopTime)
{
    // reset data
    MSEdgeControl::EdgeCont::const_iterator edg;
    // single lane edges
    const MSEdgeControl::EdgeCont &ec1 = myEdges.getSingleLaneEdges();
    for (edg = ec1.begin(); edg != ec1.end(); ++edg) {
        resetOnly(*(*edg), stopTime);
    }
    // multi lane edges
    const MSEdgeControl::EdgeCont &ec2 = myEdges.getMultiLaneEdges();
    for (edg = ec2.begin(); edg != ec2.end(); ++edg) {
        resetOnly(*(*edg), stopTime);
    }
}


void
MSMeanData_Net::write(OutputDevice &dev,
                      SUMOTime startTime, SUMOTime stopTime)
{
    // the folowing may happen on closure
    if (stopTime==startTime) {
        return;
    }
    bool found = myDumpBegins.size()==0;
    for (unsigned int i=0; i<myDumpBegins.size()&&!found; ++i) {
        if (!((myDumpBegins[i]>=0&&myDumpBegins[i]>stopTime)||(myDumpEnds[i]>=0&&myDumpEnds[i]<startTime))) {
            found = true;
        }
    }
    if (!found) {
        resetOnly(stopTime);
        return;
    }
    // interval begin
    // edges
    MSEdgeControl::EdgeCont::const_iterator edg;
    // single lane edges
    const MSEdgeControl::EdgeCont &ec1 = myEdges.getSingleLaneEdges();
    for (edg = ec1.begin(); edg != ec1.end(); ++edg) {
        writeEdge(dev, *(*edg), startTime, stopTime);
    }
    // multi lane edges
    const MSEdgeControl::EdgeCont &ec2 = myEdges.getMultiLaneEdges();
    for (edg = ec2.begin(); edg != ec2.end(); ++edg) {
        writeEdge(dev, *(*edg), startTime, stopTime);
    }
    // interval end
}


void
MSMeanData_Net::writeEdge(OutputDevice &dev,
                          const MSEdge &edge,
                          SUMOTime startTime, SUMOTime stopTime)
{
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(&edge);
        SUMOReal flowS = 0;
        SUMOReal meanDensityS = 0;
        SUMOReal meanOccupancyS = 0;
        SUMOReal meanSpeedS = 0;
        SUMOReal traveltimeS = 0;
        SUMOReal noStopsS = 0;
        SUMOReal nVehS = 0;
        SUMOReal absLen = 0;
        int noSegments = 0;
        while (s!=0) {
            SUMOReal traveltime = -42;
            SUMOReal meanSpeed = -43;
            SUMOReal meanDensity = -45;
            SUMOReal meanOccupancy = -46;
            MSLaneMeanDataValues& meanData = s->getMeanData(myIndex);
            conv(meanData, (stopTime-startTime+1),
                 s->getLength(), s->getMaxSpeed(),
                 traveltime, meanSpeed, meanDensity, meanOccupancy);
            meanDensityS += meanDensity;
            meanOccupancyS += meanOccupancy;
            meanSpeedS += meanSpeed;
            traveltimeS += traveltime;
            noStopsS += meanData.haltSum;
            nVehS += meanData.nSamples;
            flowS += s->getMeanData(myIndex).nVehEntireLane;
            absLen += s->getLength();
            s->flushMeanData(myIndex, stopTime+1);
            s = s->getNextSegment();
            meanData.reset();
            noSegments++;
        }
        meanDensityS = meanDensityS / (SUMOReal) noSegments;
        meanOccupancyS = meanOccupancyS / (SUMOReal) noSegments / (SUMOReal) edge.nLanes();
        meanSpeedS = meanSpeedS / (SUMOReal) noSegments;
        flowS = flowS / (SUMOReal) noSegments;
//        flowS = flowS * 10 * (1000./absLen);
        dev<<"      <edge id=\""<<edge.getID()<<
            "\" traveltime=\""<<toString(traveltimeS)<<
                "\" nSamples=\""<<toString((size_t) nVehS)<<
                    "\" density=\""<<toString(meanDensityS)<<
                        "\" occupancy=\""<<toString(meanOccupancyS)<<
                            "\" noStops=\""<<toString(noStopsS)<<
                                "\" speed=\""<<toString(meanSpeedS)<<
                                    "\" flow=\""<<toString(flowS*3600./((SUMOReal)(stopTime-startTime+1)))<<  //!!!
                                        "\"/>\n";
    } else {
#endif
        const MSEdge::LaneCont * const lanes = edge.getLanes();
        MSEdge::LaneCont::const_iterator lane;
        if (!myAmEdgeBased) {
            dev<<"      <edge id=\""<<edge.getID()<<"\">\n";
            for (lane = lanes->begin(); lane != lanes->end(); ++lane) {
                writeLane(dev, *(*lane), startTime, stopTime);
            }
            dev<<"      </edge>\n";
        } else {
            SUMOReal traveltimeS = 0;
            SUMOReal meanSpeedS = 0;
            SUMOReal meanDensityS = 0;
            SUMOReal noStopsS = 0;
            SUMOReal nVehS = 0;
            SUMOReal meanOccupancyS = 0;
            for (lane = lanes->begin(); lane != lanes->end(); ++lane) {
                MSLaneMeanDataValues& meanData = (*lane)->getMeanData(myIndex);
                // calculate mean data
                SUMOReal traveltime = -42;
                SUMOReal meanSpeed = -43;
                SUMOReal meanDensity = -45;
                SUMOReal meanOccupancy = -46;
                conv(meanData, (stopTime-startTime+1),
                     (*lane)->myLength, (*lane)->myMaxSpeed,
                     traveltime, meanSpeed, meanDensity, meanOccupancy);
                traveltimeS += traveltime;
                meanSpeedS += meanSpeed;
                meanDensityS += meanDensity;
                meanOccupancyS += meanOccupancy;
                noStopsS += meanData.haltSum;
                nVehS += meanData.nSamples;
                meanData.reset();
            }
            assert(lanes->size()!=0);
            dev<<"      <edge id=\""<<edge.getID()<<
                "\" traveltime=\""<<toString(traveltimeS/(SUMOReal) lanes->size())<<
                    "\" nSamples=\""<<toString((size_t) nVehS)<<
                        "\" density=\""<<toString(meanDensityS)<<
                            "\" occupancy=\""<<toString(meanOccupancyS/(SUMOReal) lanes->size())<<
                                "\" noStops=\""<<toString(noStopsS)<<
                                    "\" speed=\""<<toString(meanSpeedS/(SUMOReal) lanes->size())<<
                                        "\"/>\n";
        }
#ifdef HAVE_MESOSIM
    }
#endif
}


void
MSMeanData_Net::writeLane(OutputDevice &dev,
                          const MSLane &lane,
                          SUMOTime startTime, SUMOTime stopTime)
{
    assert(lane.myMeanData.size()>myIndex);
    MSLaneMeanDataValues& meanData = lane.getMeanData(myIndex);
    // calculate mean data
    SUMOReal traveltime = -42;
    SUMOReal meanSpeed = -43;
    SUMOReal meanDensity = -44;
    SUMOReal meanOccupancy = -45;
    conv(meanData, (stopTime-startTime+1),
         lane.myLength, lane.myMaxSpeed,
         traveltime, meanSpeed, meanDensity, meanOccupancy);
    dev<<"         <lane id=\""<<lane.getID()<<
        "\" traveltime=\""<<toString(traveltime)<<
            "\" nSamples=\""<<toString((size_t) meanData.nSamples)<<
                "\" density=\""<<toString(meanDensity)<<
                    "\" occupancy=\""<<toString(meanOccupancy)<<
                        "\" noStops=\""<<toString(meanData.haltSum)<<
                            "\" speed=\""<<toString(meanSpeed)<<
                                "\"/>\n";
    meanData.reset();
}


void
MSMeanData_Net::writeXMLHeader(OutputDevice &) const
    {}


void
MSMeanData_Net::writeXMLOutput(OutputDevice &dev,
                               SUMOTime startTime, SUMOTime stopTime)
{
    dev<<"   <interval begin=\""<<
        toString(startTime)<<"\" end=\""<<
            toString(stopTime)<<"\" ";
    if (dev.needsDetectorName()) {
        dev<<"id=\"dump_"<<toString(myInterval)<<"\" ";
    }
    dev<<">\n";
    write(dev, startTime, stopTime);
    dev<<"   </interval>";
}


void
MSMeanData_Net::writeXMLDetectorInfoStart(OutputDevice &dev) const
{
    dev<<"<netstats>";
}


void
MSMeanData_Net::writeXMLDetectorInfoEnd(OutputDevice &dev) const
{
    dev<<"</netstats>";
}


/****************************************************************************/

