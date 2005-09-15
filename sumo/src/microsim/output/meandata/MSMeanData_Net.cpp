//---------------------------------------------------------------------------//
//                        MSMeanData_Net.cpp -
//  Redirector for mean data output (net->edgecontrol)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 10.05.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.10  2005/09/15 11:08:51  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/07/15 07:18:40  dkrajzew
// code style applied
//
// Revision 1.8  2005/07/12 12:21:56  dkrajzew
// debugging vehicle emission
//
// Revision 1.7  2005/07/12 12:14:39  dkrajzew
// edge-based mean data implemented; previous lane-based is now optional
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <cassert>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <utils/convert/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Net.h"
#include <limits>

#ifdef HAVE_MESOSIM
#include <microsim/MSGlobals.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSMeanData_Net::MSMeanData_Net(unsigned int t, unsigned int index,
                               MSEdgeControl &edges,
                               const std::vector<int> &dumpBegins,
                               const std::vector<int> &dumpEnds,
                               bool useLanes,
                               bool addHeaderTail )
    : myInterval( t ), myUseHeader(addHeaderTail), myIndex(index),
    myEdges(edges), myAmEdgeBased(!useLanes),
    myDumpBegins(dumpBegins), myDumpEnds(dumpEnds)
{
}


MSMeanData_Net::~MSMeanData_Net()
{
}


void
MSMeanData_Net::resetOnly(const MSEdge &edge, SUMOTime stopTime)
{
#ifdef HAVE_MESOSIM
    if(MSGlobals::gUseMesoSim) {
        MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(&edge);
        s->updateMeanData((SUMOReal) stopTime);
        while(s!=0) {
            MSLaneMeanDataValues& meanData = s->getMeanData(myIndex);
            s = s->getNextSegment();
            meanData.reset();
        }
    } else {
#endif
    MSEdge::LaneCont *lanes = edge.getLanes();
    MSEdge::LaneCont::const_iterator lane;
    for ( lane = lanes->begin(); lane != lanes->end(); ++lane) {
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
    for ( edg = ec1.begin(); edg != ec1.end(); ++edg ) {
        resetOnly(*(*edg), stopTime);
    }
    // multi lane edges
    const MSEdgeControl::EdgeCont &ec2 = myEdges.getMultiLaneEdges();
    for ( edg = ec2.begin(); edg != ec2.end(); ++edg ) {
        resetOnly(*(*edg), stopTime);
    }
}


void
MSMeanData_Net::write(XMLDevice &dev,
                      SUMOTime startTime, SUMOTime stopTime)
{
    // the folowing may happen on closure
    if(stopTime==startTime) {
        return;
    }
    bool found = myDumpBegins.size()==0;
    for(int i=0; i<myDumpBegins.size()&&!found; ++i) {
        if(!((myDumpBegins[i]>=0&&myDumpBegins[i]>stopTime)||(myDumpEnds[i]>=0&&myDumpEnds[i]<startTime))) {
            found = true;
        }
    }
    if(!found) {
        resetOnly(stopTime);
        return;
    }
    /*
    if((myDumpBegin>=0&&myDumpBegin>stopTime)||(myDumpEnd>=0&&myDumpEnd<startTime)) {
        resetOnly(stopTime);
        return;
    }
    */
    // interval begin
    // edges
    MSEdgeControl::EdgeCont::const_iterator edg;
        // single lane edges
    const MSEdgeControl::EdgeCont &ec1 = myEdges.getSingleLaneEdges();
    for ( edg = ec1.begin(); edg != ec1.end(); ++edg ) {
        writeEdge(dev, *(*edg), startTime, stopTime);
    }
        // multi lane edges
    const MSEdgeControl::EdgeCont &ec2 = myEdges.getMultiLaneEdges();
    for ( edg = ec2.begin(); edg != ec2.end(); ++edg ) {
        writeEdge(dev, *(*edg), startTime, stopTime);
    }
    // interval end
}


void
MSMeanData_Net::writeEdge(XMLDevice &dev,
                          const MSEdge &edge,
                          SUMOTime startTime, SUMOTime stopTime)
{
#ifdef HAVE_MESOSIM
    if(MSGlobals::gUseMesoSim) {
        MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(&edge);
        double flowS = 0;
        s->updateMeanData((SUMOReal) stopTime);
        double meanDensityS = 0;
        double meanSpeedS = 0;
        double traveltimeS = 0;
        double noStopsS = 0;
        double nVehS = 0;
        int noSegments = 0;
        while(s!=0) {
            double traveltime = -42;
            double meanSpeed = -43;
            double meanDensity = -45;
            MSLaneMeanDataValues& meanData = s->getMeanData(myIndex);
            if(meanData.nVehContributed==0) {
                traveltime = s->getLength() / s->getMaxSpeed();
                meanSpeed = s->getMaxSpeed();
                meanDensity = 0;
            } else {
                meanSpeed = meanData.speedSum / (double) meanData.nVehContributed;
                if(meanSpeed==0) {
                    traveltime = std::numeric_limits<float>::max() / 100.;
                } else {
                    traveltime = s->getLength() / meanSpeed;
                }
                meanDensity = (double) meanData.vehLengthSum / //.nVehContributed /
                    (double) (stopTime-startTime+1) / s->getLength();
            }
            meanDensityS += meanDensity;
            meanSpeedS += meanSpeed;
            traveltimeS += traveltime;
            noStopsS += meanData.haltSum;
            nVehS += meanData.nVehContributed;
            flowS += s->getMeanData(myIndex).nVehEntireLane;
            s = s->getNextSegment();
            meanData.reset();
            noSegments++;
        }
        meanDensityS = meanDensityS / (float) noSegments / (float) edge.nLanes();
        meanSpeedS = meanSpeedS / (float) noSegments;
        flowS = flowS / (float) noSegments;
        dev.writeString("      <edge id=\"").writeString(edge.id()).writeString(
            "\" traveltime=\"").writeString(toString(traveltimeS)).writeString(
            "\" noVehContrib=\"").writeString(toString(nVehS)).writeString(
            "\" density=\"").writeString(toString(meanDensityS)).writeString(
            "\" noStops=\"").writeString(toString(noStopsS)).writeString(
            "\" speed=\"").writeString(toString(meanSpeedS)).writeString(
            "\" flow=\"").writeString(toString(flowS*3600./((float) (stopTime-startTime+1)))).writeString( //!!!
            "\"/>\n");
    } else {
#endif
    MSEdge::LaneCont *lanes = edge.getLanes();
    MSEdge::LaneCont::const_iterator lane;
    if(!myAmEdgeBased) {
        dev.writeString("   <edge id=\"").writeString(edge.id()).writeString("\">\n");
        for ( lane = lanes->begin(); lane != lanes->end(); ++lane) {
            writeLane(dev, *(*lane), startTime, stopTime);
        }
        dev.writeString("   </edge>\n");
    } else {
        double traveltimeS = 0;
        double meanSpeedS = 0;
        double meanDensityS = 0;
        float noStopsS = 0;
        float nVehS = 0;
        for ( lane = lanes->begin(); lane != lanes->end(); ++lane) {
            MSLaneMeanDataValues& meanData = (*lane)->getMeanData(myIndex);
            // calculate mean data
            double traveltime = -42;
            double meanSpeed = -43;
            double meanDensity = -45;
            if(meanData.nVehContributed==0) {
                assert((*lane)->myMaxSpeed>=0);
                traveltime = (*lane)->myLength / (*lane)->myMaxSpeed;
                meanSpeed = (*lane)->myMaxSpeed;
                meanDensity = 0;
            } else {
                meanSpeed = meanData.speedSum / (double) meanData.nVehContributed;
                if(meanSpeed==0) {
                    traveltime = std::numeric_limits<float>::max() / 100.;
                } else {
                    traveltime = (*lane)->myLength / meanSpeed;
                }
                assert((double) (stopTime-startTime+1)!=0);
                assert((*lane)->myLength!=0);
                meanDensity = (double) meanData.vehLengthSum / //.nVehContributed /
                    (double) (stopTime-startTime+1) / (*lane)->myLength;
            }
            traveltimeS += traveltime;
            meanSpeedS += meanSpeed;
            meanDensityS += meanDensity;
            noStopsS += meanData.haltSum;
            nVehS += meanData.nVehContributed;
            meanData.reset();
        }
        assert(lanes->size()!=0);
        dev.writeString("      <edge id=\"").writeString(edge.id()).writeString(
            "\" traveltime=\"").writeString(toString(traveltimeS/(float) lanes->size())).writeString(
            "\" noVehContrib=\"").writeString(toString(nVehS)).writeString(
            "\" density=\"").writeString(toString(meanDensityS/(float) lanes->size())).writeString(
            "\" noStops=\"").writeString(toString(noStopsS)).writeString(
            "\" speed=\"").writeString(toString(meanSpeedS/(float) lanes->size())).writeString(
            "\"/>\n");
    }
#ifdef HAVE_MESOSIM
    }
#endif
}


void
MSMeanData_Net::writeLane(XMLDevice &dev,
                          const MSLane &lane,
                          SUMOTime startTime, SUMOTime stopTime)
{
    assert(lane.myMeanData.size()>myIndex);
    MSLaneMeanDataValues& meanData = lane.getMeanData(myIndex);
    // calculate mean data
    double traveltime = -42;
    double meanSpeed = -43;
    double meanSpeedSquare = -44;
    double meanDensity = -45;

    if(meanData.nVehContributed==0) {
        traveltime = lane.myLength / lane.myMaxSpeed;
        meanSpeed = lane.myMaxSpeed;
        meanDensity = 0;
    } else {
        meanSpeed = meanData.speedSum / (double) meanData.nVehContributed;
        if(meanSpeed==0) {
            traveltime = std::numeric_limits<float>::max() / 100.;
        } else {
            traveltime = lane.myLength / meanSpeed;
        }
        meanDensity = (double) meanData.vehLengthSum / //.nVehContributed /
            (double) (stopTime-startTime+1) / lane.myLength;
    }

    dev.writeString("      <lane id=\"").writeString(lane.id()).writeString(
        "\" traveltime=\"").writeString(toString(traveltime)).writeString(
        "\" noVehContrib=\"").writeString(toString(meanData.nVehContributed)).writeString(
        "\" density=\"").writeString(toString(meanDensity)).writeString(
        "\" noStops=\"").writeString(toString(meanData.haltSum)).writeString(
        "\" speed=\"").writeString(toString(meanSpeed)).writeString(
        "\"/>\n");
    meanData.reset();
}


std::string
MSMeanData_Net::getNamePrefix( void ) const
{
    return string("WeightsDump");
}


void
MSMeanData_Net::writeXMLHeader( XMLDevice &dev ) const
{
}


void
MSMeanData_Net::writeXMLOutput(XMLDevice &dev,
                               SUMOTime startTime, SUMOTime stopTime)
{
    dev.writeString("<interval begin=\"").writeString(
        toString(startTime)).writeString("\" end=\"").writeString(
        toString(stopTime)).writeString("\" ");
    if(dev.needsDetectorName()) {
        dev.writeString("id=\"dump_").writeString(toString(myInterval)).writeString("\" ");
    }
    dev.writeString(">\n");
    write(dev, startTime, stopTime);
    dev.writeString("</interval>");
}


void
MSMeanData_Net::writeXMLDetectorInfoStart( XMLDevice &dev ) const
{
    dev.writeString("<netstats>");
}


void
MSMeanData_Net::writeXMLDetectorInfoEnd( XMLDevice &dev ) const
{
    dev.writeString("</netstats>");
}


SUMOTime
MSMeanData_Net::getDataCleanUpSteps( void ) const
{
    return myInterval;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
