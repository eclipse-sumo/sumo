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
// Revision 1.11  2005/09/22 13:45:52  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
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
#include <utils/common/ToString.h>
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
    for(unsigned int i=0; i<myDumpBegins.size()&&!found; ++i) {
        if(!((myDumpBegins[i]>=0&&myDumpBegins[i]>stopTime)||(myDumpEnds[i]>=0&&myDumpEnds[i]<startTime))) {
            found = true;
        }
    }
    if(!found) {
        resetOnly(stopTime);
        return;
    }
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
    MSEdge::LaneCont *lanes = edge.getLanes();
    MSEdge::LaneCont::const_iterator lane;
    if(!myAmEdgeBased) {
        dev.writeString("   <edge id=\"").writeString(edge.id()).writeString("\">\n");
        for ( lane = lanes->begin(); lane != lanes->end(); ++lane) {
            writeLane(dev, *(*lane), startTime, stopTime);
        }
        dev.writeString("   </edge>\n");
    } else {
        SUMOReal traveltimeS = 0;
        SUMOReal meanSpeedS = 0;
        SUMOReal meanDensityS = 0;
        SUMOReal noStopsS = 0;
        SUMOReal nVehS = 0;
        for ( lane = lanes->begin(); lane != lanes->end(); ++lane) {
            MSLaneMeanDataValues& meanData = (*lane)->getMeanData(myIndex);
            // calculate mean data
            SUMOReal traveltime = -42;
            SUMOReal meanSpeed = -43;
            SUMOReal meanDensity = -45;
            if(meanData.nVehContributed==0) {
                assert((*lane)->myMaxSpeed>=0);
                traveltime = (*lane)->myLength / (*lane)->myMaxSpeed;
                meanSpeed = (*lane)->myMaxSpeed;
                meanDensity = 0;
            } else {
                meanSpeed = meanData.speedSum / (SUMOReal) meanData.nVehContributed;
                if(meanSpeed==0) {
                    traveltime = std::numeric_limits<SUMOReal>::max() / (SUMOReal) 100.;
                } else {
                    traveltime = (*lane)->myLength / meanSpeed;
                }
                assert((SUMOReal) (stopTime-startTime+1)!=0);
                assert((*lane)->myLength!=0);
                meanDensity = (SUMOReal) meanData.vehLengthSum / //.nVehContributed /
                    (SUMOReal) (stopTime-startTime+1) / (*lane)->myLength;
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
            "\" traveltime=\"").writeString(toString(traveltimeS/(SUMOReal) lanes->size())).writeString(
            "\" noVehContrib=\"").writeString(toString(nVehS)).writeString(
            "\" density=\"").writeString(toString(meanDensityS/(SUMOReal) lanes->size())).writeString(
            "\" noStops=\"").writeString(toString(noStopsS)).writeString(
            "\" speed=\"").writeString(toString(meanSpeedS/(SUMOReal) lanes->size())).writeString(
            "\"/>\n");
    }
}


void
MSMeanData_Net::writeLane(XMLDevice &dev,
                          const MSLane &lane,
                          SUMOTime startTime, SUMOTime stopTime)
{
    assert(lane.myMeanData.size()>myIndex);
    MSLaneMeanDataValues& meanData = lane.getMeanData(myIndex);
    // calculate mean data
    SUMOReal traveltime = -42;
    SUMOReal meanSpeed = -43;
    SUMOReal meanSpeedSquare = -44;
    SUMOReal meanDensity = -45;

    if(meanData.nVehContributed==0) {
        traveltime = lane.myLength / lane.myMaxSpeed;
        meanSpeed = lane.myMaxSpeed;
        meanDensity = 0;
    } else {
        meanSpeed = meanData.speedSum / (SUMOReal) meanData.nVehContributed;
        if(meanSpeed==0) {
            traveltime = std::numeric_limits<SUMOReal>::max() / (SUMOReal) 100.;
        } else {
            traveltime = lane.myLength / meanSpeed;
        }
        meanDensity = (SUMOReal) meanData.vehLengthSum / //.nVehContributed /
            (SUMOReal) (stopTime-startTime+1) / lane.myLength;
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
