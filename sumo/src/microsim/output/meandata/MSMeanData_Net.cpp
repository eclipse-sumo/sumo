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
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Net.h"
//#include "MSMeanData_EdgeControl.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSMeanData_Net::MSMeanData_Net(unsigned int  t, OutputDevice* of,
                               bool addHeaderTail )
    : myInterval( t ), myOutputDevice( of ), myUseHeader(addHeaderTail)
{
    if(myUseHeader) {
        myOutputDevice->getOStream() << "<netstats>\n";
    }
}


MSMeanData_Net::~MSMeanData_Net()
{
    if(myUseHeader) {
        myOutputDevice->getOStream() << "</netstats>\n";
    }
    myOutputDevice->close();
}


void
MSMeanData_Net::write(unsigned int passedSteps,
                      unsigned int start, unsigned int step,
                      MSEdgeControl &edges, unsigned int idx)
{
    // interval begin
    myOutputDevice->getOStream()
        << "<interval begin=\""
        << (passedSteps - myInterval + start)
        << "\" end=\"" << step << "\">" << endl;
    // edges
    MSEdgeControl::EdgeCont::const_iterator edg;
        // single lane edges
    const MSEdgeControl::EdgeCont &ec1 = edges.getSingleLaneEdges();
    for ( edg = ec1.begin(); edg != ec1.end(); ++edg ) {
        writeEdge(*(*edg), idx, start, passedSteps);
    }
        // multi lane edges
    const MSEdgeControl::EdgeCont &ec2 = edges.getMultiLaneEdges();
    for ( edg = ec2.begin(); edg != ec2.end(); ++edg ) {
        writeEdge(*(*edg), idx, start, passedSteps);
    }
    // interval end
    myOutputDevice->getOStream() << "</interval>" << endl;
}


void
MSMeanData_Net::writeEdge(const MSEdge &edge, unsigned int idx,
                          unsigned int start, unsigned int passedSteps)
{
    myOutputDevice->getOStream()
        << "   <edge id=\"" << edge.id() << "\">" << endl;
    MSEdge::LaneCont *lanes = edge.getLanes();
    MSEdge::LaneCont::const_iterator lane;
    for ( lane = lanes->begin(); lane != lanes->end(); ++lane) {
        writeLane(*(*lane), idx, start, passedSteps);
    }
    myOutputDevice->getOStream() << "   </edge>\n";
}


void
MSMeanData_Net::writeLane(const MSLane &lane, unsigned int idx,
                          unsigned int start, unsigned int passedSteps)
{
    assert(lane.myMeanData.size()>idx);
    const MSLaneMeanDataValues& meanData = lane.myMeanData[ idx ];

    const_cast< MSLane& >( lane ).collectVehicleData( idx );

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
    myOutputDevice->getOStream()
        << "      <lane id=\""      << lane.id()
        << "\" traveltime=\""  << traveltime
        << "\" speed=\""       << meanSpeed
        << "\" speedsquare=\"" << meanSpeedSquare
        << "\" density=\""     << meanDensity
        << "\" noVehContrib=\""  << meanData.nVehContributed
        << "\" noVehEntire=\""  << meanData.nVehEntireLane
        << "\" noVehEntered=\"" << meanData.nVehEnteredLane
        << "\" noVehLeft=\"" << meanData.nVehLeftLane
        << "\"/>\n";

    const_cast< MSLane& >( lane ).resetMeanData( idx );
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
