/****************************************************************************/
/// @file    MSMeanData_Harmonoise.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id: MSMeanData_Harmonoise.cpp 6726 2009-02-05 08:33:51Z dkrajzew $
///
// Redirector for mean data output (net->edgecontrol)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSMeanData_Harmonoise.h"
#include "HelpersHarmonoise.h"
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
// ---------------------------------------------------------------------------
// MSMeanData_Harmonoise::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_Harmonoise::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane) throw()
        : MSMoveReminder(lane), sampleSeconds(0), 
        currentTimeN(0), meanNTemp(0), myLastTimeStep((SUMOTime) -1)
{}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::reset() throw()
{
    sampleSeconds = 0;
    currentTimeN = 0;
    meanNTemp = 0;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::add(SUMOReal sn, SUMOReal fraction, bool flushOnly) throw()
{
    SUMOTime step = MSNet::getInstance()->getCurrentTimeStep();
    if(step!=myLastTimeStep||flushOnly) {
        if(currentTimeN!=0) {
            meanNTemp += (SUMOReal) pow(10., HelpersHarmonoise::sum(currentTimeN)/10.);
        }
        currentTimeN = 0;
        if(!flushOnly) {
            myLastTimeStep = step;
        }
    }
    if(!flushOnly) {
        currentTimeN += (SUMOReal) pow(10., (sn/10.));
        sampleSeconds += fraction;
    }
}


bool
MSMeanData_Harmonoise::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw()
{
    bool ret = true;
    SUMOReal fraction = 1.;
    if (oldPos<0&&newSpeed!=0) {
        fraction = (oldPos+SPEED2DIST(newSpeed)) / newSpeed;
    }
    if (oldPos+SPEED2DIST(newSpeed)>getLane()->length()&&newSpeed!=0) {
        fraction -= (oldPos+SPEED2DIST(newSpeed) - getLane()->length()) / newSpeed;
        ret = false;
    }
    SUMOReal a = veh.getPreDawdleAcceleration();
    SUMOReal sn = HelpersHarmonoise::computeNoise(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a);
    add(sn, fraction);
    return ret;
}


void
MSMeanData_Harmonoise::MSLaneMeanDataValues::dismissByLaneChange(MSVehicle& veh) throw()
{
}


bool
MSMeanData_Harmonoise::MSLaneMeanDataValues::isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw()
{
    SUMOReal fraction = 1.;
    SUMOReal l = veh.getVehicleType().getLength();
    if (veh.getPositionOnLane()+l>getLane()->length()) {
        fraction = l - (getLane()->length()-veh.getPositionOnLane());
    }
    SUMOReal a = veh.getPreDawdleAcceleration();
    SUMOReal sn = (fraction * HelpersHarmonoise::computeNoise(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) a));
    add(sn, fraction);
    return true;
}



// ---------------------------------------------------------------------------
// MSMeanData_Harmonoise - methods
// ---------------------------------------------------------------------------
MSMeanData_Harmonoise::MSMeanData_Harmonoise(const std::string &id,
                                   MSEdgeControl &edges,
                                   const std::vector<SUMOTime> &dumpBegins,
                                   const std::vector<SUMOTime> &dumpEnds,
                                   bool useLanes,
                                   bool withEmptyEdges, bool withEmptyLanes) throw()
        : myID(id),
        myAmEdgeBased(!useLanes), myDumpBegins(dumpBegins), myDumpEnds(dumpEnds),
        myDumpEmptyEdges(withEmptyEdges), myDumpEmptyLanes(withEmptyLanes)
{
    // interval begin
    // edges
    MSEdgeControl::EdgeCont::const_iterator edg;
    // single lane edges
    const MSEdgeControl::EdgeCont &ec1 = edges.getSingleLaneEdges();
    for (edg = ec1.begin(); edg != ec1.end(); ++edg) {
        std::vector<MSLaneMeanDataValues*> v;
        const MSEdge::LaneCont * const lanes = (*edg)->getLanes();
        MSEdge::LaneCont::const_iterator lane;
        for (lane = lanes->begin(); lane != lanes->end(); ++lane) {
            v.push_back(new MSLaneMeanDataValues(*lane));
        }
        myMeasures.push_back(v);
        myEdges.push_back(*edg);
    }
    // multi lane edges
    const MSEdgeControl::EdgeCont &ec2 = edges.getMultiLaneEdges();
    for (edg = ec2.begin(); edg != ec2.end(); ++edg) {
        std::vector<MSLaneMeanDataValues*> v;
        const MSEdge::LaneCont * const lanes = (*edg)->getLanes();
        MSEdge::LaneCont::const_iterator lane;
        for (lane = lanes->begin(); lane != lanes->end(); ++lane) {
            v.push_back(new MSLaneMeanDataValues(*lane));
        }
        myMeasures.push_back(v);
        myEdges.push_back(*edg);
    }
}


MSMeanData_Harmonoise::~MSMeanData_Harmonoise() throw()
{}


void
MSMeanData_Harmonoise::resetOnly(SUMOTime stopTime) throw()
{
    for (vector<vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
        for (vector<MSLaneMeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            (*j)->reset();
        }
    }
}


void
MSMeanData_Harmonoise::write(OutputDevice &dev,
                        SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    // check whether this dump shall be written for the current time
    bool found = myDumpBegins.size()==0;
    for (unsigned int i=0; i<myDumpBegins.size()&&!found; ++i) {
        if (!((myDumpBegins[i]>=0&&myDumpBegins[i]>stopTime)||(myDumpEnds[i]>=0&&myDumpEnds[i]<startTime))) {
            found = true;
        }
    }
    if (!found) {
        // no -> reset only
        resetOnly(stopTime);
        return;
    }
    // yes -> write
    vector<MSEdge*>::iterator edge = myEdges.begin();
    for (vector<vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i, ++edge) {
        writeEdge(dev, (*i), *edge, startTime, stopTime);
    }
}


void
MSMeanData_Harmonoise::writeEdge(OutputDevice &dev,
                            const vector<MSLaneMeanDataValues*> &edgeValues,
                            MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    vector<MSLaneMeanDataValues*>::const_iterator lane;
    if (!myAmEdgeBased) {
        bool writeCheck = myDumpEmptyEdges;
        if (!writeCheck) {
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                if ((*lane)->sampleSeconds>0) {
                    writeCheck = true;
                    break;
                }
            }
        }
        if (writeCheck) {
            dev<<"      <edge id=\""<<edge->getID()<<"\">\n";
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                writeLane(dev, *(*lane), startTime, stopTime);
            }
            dev<<"      </edge>\n";
        }
    } else {
        SUMOReal nS = 0;
        SUMOReal nVehS = 0;
        for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
            MSLaneMeanDataValues& meanData = *(*lane);
            meanData.add(0, 0, true);
            // calculate mean data
            nS += meanData.meanNTemp;
            nVehS += meanData.sampleSeconds;
            meanData.reset();
        }
        SUMOReal v = (SUMOReal) (10. * log10(nS/(SUMOReal)(stopTime-startTime+1)));
        if (myDumpEmptyEdges||nVehS>0) {
            dev<<"      <edge id=\""<<edge->getID()<<
            "\" sampledSeconds=\""<< nVehS <<
            "\" noise=\""<< v <<
            "\"/>\n";
        }
    }
}


void
MSMeanData_Harmonoise::writeLane(OutputDevice &dev,
                            MSLaneMeanDataValues &laneValues,
                            SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    if (myDumpEmptyLanes||laneValues.sampleSeconds>0) {
        // calculate mean data
        laneValues.add(0, 0, true);
        SUMOReal v = (SUMOReal) (10. * log10(laneValues.meanNTemp/(SUMOReal)(stopTime-startTime+1)));
        dev<<"         <lane id=\""<<laneValues.getLane()->getID()<<
        "\" sampledSeconds=\""<< laneValues.sampleSeconds <<
        "\" noise=\""<< v <<
        "\"/>\n";
    }
    laneValues.reset();
}


void
MSMeanData_Harmonoise::writeXMLOutput(OutputDevice &dev,
                                 SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    dev<<"   <interval begin=\""<<startTime<<"\" end=\""<<
    stopTime<<"\" "<<"id=\""<<myID<<"\">\n";
    write(dev, startTime, stopTime);
    dev<<"   </interval>\n";
}


void
MSMeanData_Harmonoise::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError)
{
    dev.writeXMLHeader("netstats");
}


/****************************************************************************/

