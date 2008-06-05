/****************************************************************************/
/// @file    MSMeanData_Net.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id:MSMeanData_Net.cpp 4976 2008-01-30 14:23:39Z dkrajzew $
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
                               bool withEmptyEdges, bool withEmptyLanes) throw()
        : myInterval(t), myIndex(index),
        myEdges(edges), myAmEdgeBased(!useLanes),
        myDumpBegins(dumpBegins), myDumpEnds(dumpEnds), 
        myDumpEmptyEdges(withEmptyEdges), myDumpEmptyLanes(withEmptyLanes)
{}


MSMeanData_Net::~MSMeanData_Net() throw()
{}


void
MSMeanData_Net::resetOnly(const MSEdge &edge) throw()
{
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(&edge);
        while (s!=0) {
            MSLaneMeanDataValues& meanData = s->getMeanData(myIndex);
            meanData.reset();
            s = s->getNextSegment();
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
MSMeanData_Net::resetOnly() throw()
{
    // reset data
    MSEdgeControl::EdgeCont::const_iterator edg;
    // single lane edges
    const MSEdgeControl::EdgeCont &ec1 = myEdges.getSingleLaneEdges();
    for (edg = ec1.begin(); edg != ec1.end(); ++edg) {
        resetOnly(*(*edg));
    }
    // multi lane edges
    const MSEdgeControl::EdgeCont &ec2 = myEdges.getMultiLaneEdges();
    for (edg = ec2.begin(); edg != ec2.end(); ++edg) {
        resetOnly(*(*edg));
    }
}


void
MSMeanData_Net::write(OutputDevice &dev,
                      SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    bool found = myDumpBegins.size()==0;
    for (unsigned int i=0; i<myDumpBegins.size()&&!found; ++i) {
        if (!((myDumpBegins[i]>=0&&myDumpBegins[i]>stopTime)||(myDumpEnds[i]>=0&&myDumpEnds[i]<startTime))) {
            found = true;
        }
    }
    if (!found) {
        resetOnly();
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
                          SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(&edge);

        SUMOReal flowOut = 0;
        SUMOReal flowMean = 0;
        SUMOReal meanDensityS = 0;
        SUMOReal meanOccupancyS = 0;
        SUMOReal meanSpeedS = 0;
        SUMOReal traveltimeS = 0;
        unsigned noStopsS = 0;
        unsigned noEmissionsS = 0;
        unsigned noLeftS = 0;
        unsigned noEnteredS = 0;
        SUMOReal nVehS = 0;
        SUMOReal absLen = 0;
        int noSegments = 0;
        int noNotEmpty = 0;
        while (s!=0) {
            SUMOReal traveltime = -42;
            SUMOReal meanSpeed = -43;
            SUMOReal meanDensity = -45;
            SUMOReal meanOccupancy = -46;
            MSLaneMeanDataValues& meanData = s->getMeanData(myIndex);
            s->prepareMeanDataForWriting(meanData, (SUMOReal) stopTime/* + (SUMOReal) DELTA_T*/);
            conv(meanData, (stopTime-startTime+1),
                s->getLength(), s->getMaxSpeed(),
                traveltime, meanSpeed, meanDensity, meanOccupancy);
            meanDensityS += meanDensity;
            meanOccupancyS += meanOccupancy;
            traveltimeS += traveltime;
            noStopsS += meanData.haltSum;
            noEmissionsS += meanData.emitted;
            noEnteredS += meanData.nVehEnteredLane;
            nVehS += meanData.sampleSeconds;
            flowMean += meanData.nVehLeftLane;
            if(meanData.sampleSeconds>0) {
                meanSpeedS += meanSpeed;
                noNotEmpty++;
            }
            //s->flushMeanData(myIndex, stopTime+1);
            noSegments++;
            absLen += s->getLength();
            if(s->getNextSegment()==0) {
                flowOut = meanData.nVehLeftLane;
                noLeftS = meanData.nVehLeftLane;
            }
            s = s->getNextSegment();
            meanData.reset();
        }
        if(myDumpEmptyEdges||nVehS>0) {
            meanDensityS = meanDensityS / (SUMOReal) noSegments;
            meanOccupancyS = meanOccupancyS / (SUMOReal) noSegments / (SUMOReal) edge.nLanes();
            meanSpeedS = noNotEmpty!=0 ? meanSpeedS / (SUMOReal) noNotEmpty : 0;
            if(nVehS==0) {
                meanSpeedS = MSGlobals::gMesoNet->getSegmentForEdge(&edge)->getMaxSpeed();
            } else {
                traveltimeS = absLen / meanSpeedS;
            }
            flowMean /= (SUMOReal) noSegments;
            dev<<"      <edge id=\""<<edge.getID()<<
            "\" traveltime=\""<<traveltimeS<<
            "\" nSamples=\""<< nVehS <<
            "\" density=\""<<meanDensityS<<
            "\" occupancy=\""<<meanOccupancyS<<
            //"\" noStops=\""<<noStopsS<<
            "\" speed=\""<<meanSpeedS<<
            "\" entered=\""<<noEnteredS<<
            "\" emitted=\""<<noEmissionsS<<
            "\" left=\""<<noLeftS<<
            "\" flowMean=\""<<(flowMean*3600./((SUMOReal)(stopTime-startTime+1)))<<
            "\" flow=\""<<(flowOut*3600./((SUMOReal)(stopTime-startTime+1)))<<  
            "\"/>\n";
        }
    } else {
#endif
        const MSEdge::LaneCont * const lanes = edge.getLanes();
        MSEdge::LaneCont::const_iterator lane;
        if (!myAmEdgeBased) {
            bool writeCheck = myDumpEmptyEdges;
            if(!writeCheck) {
                for (lane = lanes->begin(); lane != lanes->end(); ++lane) {
                    if((*lane)->getMeanData(myIndex).sampleSeconds>0) {
                        writeCheck = true;
                        break;
                    }
                }
            }
            if(writeCheck) {
                dev<<"      <edge id=\""<<edge.getID()<<"\">\n";
                for (lane = lanes->begin(); lane != lanes->end(); ++lane) {
                    writeLane(dev, *(*lane), startTime, stopTime);
                }
                dev<<"      </edge>\n";
            }
        } else {
            SUMOReal traveltimeS = 0;
            SUMOReal meanSpeedS = 0;
            SUMOReal meanDensityS = 0;
            unsigned noStopsS = 0;
            SUMOReal nVehS = 0;
            SUMOReal meanOccupancyS = 0;
            unsigned noEmissionsS = 0;
            unsigned noLeftS = 0;
            unsigned noEnteredS = 0;
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
                noEmissionsS += meanData.emitted;
                noLeftS += meanData.nVehLeftLane;
                noEnteredS += meanData.nVehEnteredLane;
                nVehS += meanData.sampleSeconds;
                meanData.reset();
            }
            assert(lanes->size()!=0);
            if(myDumpEmptyEdges||nVehS>0) {
                dev<<"      <edge id=\""<<edge.getID()<<
                "\" traveltime=\""<<(traveltimeS/(SUMOReal) lanes->size())<<
                "\" nSamples=\""<< nVehS <<
                "\" density=\""<<meanDensityS<<
                "\" occupancy=\""<<(meanOccupancyS/(SUMOReal) lanes->size())<<
                "\" noStops=\""<<noStopsS<<
                "\" speed=\""<<(meanSpeedS/(SUMOReal) lanes->size())<<
                "\" entered=\""<<noEnteredS<<
                "\" emitted=\""<<noEmissionsS<<
                "\" left=\""<<noLeftS<<
                "\"/>\n";
            }
        }
#ifdef HAVE_MESOSIM
    }
#endif
}


void
MSMeanData_Net::writeLane(OutputDevice &dev,
                          const MSLane &lane,
                          SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    assert(lane.myMeanData.size()>myIndex);
    MSLaneMeanDataValues& meanData = lane.getMeanData(myIndex);
    if(myDumpEmptyLanes||meanData.sampleSeconds>0) {
        // calculate mean data
        SUMOReal traveltime = -42;
        SUMOReal meanSpeed = -43;
        SUMOReal meanDensity = -44;
        SUMOReal meanOccupancy = -45;
        conv(meanData, (stopTime-startTime+1),
             lane.myLength, lane.myMaxSpeed,
             traveltime, meanSpeed, meanDensity, meanOccupancy);
        dev<<"         <lane id=\""<<lane.getID()<<
        "\" traveltime=\""<<traveltime<<
        "\" nSamples=\""<< meanData.sampleSeconds <<
        "\" density=\""<<meanDensity<<
        "\" occupancy=\""<<meanOccupancy<<
        "\" noStops=\""<<meanData.haltSum<<
        "\" speed=\""<<meanSpeed<<
        "\" entered=\""<<meanData.nVehEnteredLane<<
        "\" emitted=\""<<meanData.emitted<<
        "\" left=\""<<meanData.nVehLeftLane<<
        "\"/>\n";
    }
    meanData.reset();
}


void
MSMeanData_Net::writeXMLOutput(OutputDevice &dev,
                               SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    dev<<"   <interval begin=\""<<startTime<<"\" end=\""<<
    stopTime<<"\" "<<"id=\"dump_"<<myInterval<<"\">\n";
    write(dev, startTime, stopTime);
    dev<<"   </interval>\n";
}


void
MSMeanData_Net::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError)
{
    dev.writeXMLHeader("netstats");
}


/****************************************************************************/

