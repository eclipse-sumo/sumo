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
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cassert>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <utils/convert/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Net.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSMeanData_Net::MSMeanData_Net(unsigned int t, unsigned int index,
                               MSEdgeControl &edges,
                               bool addHeaderTail )
    : myInterval( t ), myUseHeader(addHeaderTail), myIndex(index),
    myEdges(edges)
{
}


MSMeanData_Net::~MSMeanData_Net()
{
}


void
MSMeanData_Net::write(XMLDevice &dev,
                      SUMOTime startTime, SUMOTime stopTime)
{
    // the folowing may happen on closure
    if(stopTime==startTime) {
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
    dev.writeString("   <edge id=\"").writeString(edge.id()).writeString("\">\n");
    MSEdge::LaneCont *lanes = edge.getLanes();
    MSEdge::LaneCont::const_iterator lane;
    for ( lane = lanes->begin(); lane != lanes->end(); ++lane) {
        writeLane(dev, *(*lane), startTime, stopTime);
    }
    dev.writeString("   </edge>\n");
}


void
MSMeanData_Net::writeLane(XMLDevice &dev,
                          const MSLane &lane,
                          SUMOTime startTime, SUMOTime stopTime)
{
    assert(lane.myMeanData.size()>myIndex);
    const MSLaneMeanDataValues& meanData = lane.myMeanData[ myIndex ];

    const_cast< MSLane& >( lane ).collectVehicleData( myIndex );

    // calculate mean data
    double traveltime = -42;
    double meanSpeed = -43;
    double meanSpeedSquare = -44;
    double meanDensity = -45;

    assert( meanData.nVehEntireLane <= meanData.nVehContributed );

    if ( meanData.nVehContributed > 0 ) {

        double intervallLength = myInterval * MSNet::deltaT();

        if(meanData.contTimestepSum!=0) {
            meanSpeed   = meanData.speedSum / meanData.contTimestepSum;
            meanSpeedSquare = meanData.speedSquareSum / meanData.contTimestepSum;
        } else {
            meanSpeed   = lane.myMaxSpeed;
            meanSpeedSquare = -1;
        }

        meanDensity = ( meanData.discreteTimestepSum * MSNet::deltaT() ) /
            intervallLength / lane.myLength * 1000.0;

        // only vehicles that used the lane entirely contribute to traveltime
        if ( meanData.nVehEntireLane > 0 ) {
            assert(meanData.nVehEntireLane!=0);
            traveltime = meanData.traveltimeStepSum * MSNet::deltaT() /
                meanData.nVehEntireLane;
            assert( traveltime >= lane.myLength / lane.myMaxSpeed );

        }
        else {
            // no vehicle left the lane within intervall.
            // Calculate the traveltime using the measured meanSpeed
            if(meanSpeed==0) {
                meanSpeed   = lane.myMaxSpeed;
                meanSpeedSquare = -1;
                meanDensity = 0;
            }
            assert(meanSpeed!=0);
            traveltime  = lane.myLength / meanSpeed;
        }
    }
    else { // no vehicles visited the lane within intervall

        meanSpeed   = lane.myMaxSpeed;
        traveltime  = lane.myLength / meanSpeed;
        meanSpeedSquare = -1;
        meanDensity = 0;

    }
    dev.writeString("      <lane id=\"").writeString(lane.id()).writeString(
        "\" traveltime=\"").writeString(toString(traveltime)).writeString(
        "\" speed=\"").writeString(toString(meanSpeed)).writeString(
        "\" speedsquare=\"").writeString(toString(meanSpeedSquare)).writeString(
        "\" density=\"").writeString(toString(meanDensity)).writeString(
        "\" noVehContrib=\"").writeString(toString(meanData.nVehContributed)).writeString(
        "\" noVehEntire=\"").writeString(toString(meanData.nVehEntireLane)).writeString(
        "\" noVehEntered=\"").writeString(toString(meanData.nVehEnteredLane)).writeString(
        "\" noVehLeft=\"").writeString(toString(meanData.nVehLeftLane)).writeString(
        "\"/>\n");
    const_cast< MSLane& >( lane ).resetMeanData( myIndex );
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


MSUnit::IntSteps
MSMeanData_Net::getDataCleanUpSteps( void ) const
{
    return myInterval;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
