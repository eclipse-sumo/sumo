/****************************************************************************/
/// @file    MSMeanData_HBEFA.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id:MSMeanData_HBEFA.cpp 4976 2008-01-30 14:23:39Z dkrajzew $
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
#include "MSMeanData_HBEFA.h"
#include "HelpersHBEFA.h"
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
// MSMeanData_HBEFA::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_HBEFA::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane) throw()
        : MSMoveReminder(lane), sampleSeconds(0), CO2(0), CO(0),
        NOx(0), PMx(0), fuel(0){}


void
MSMeanData_HBEFA::MSLaneMeanDataValues::reset() throw()
{
    sampleSeconds = 0.;
    CO2 = 0;
    CO = 0;
    NOx = 0;
    PMx = 0;
    fuel = 0;
}


bool
MSMeanData_HBEFA::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw()
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
    sampleSeconds += fraction;
    CO += (fraction * HelpersHBEFA::computeCO(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) veh.getAcceleration()));
    CO2 += (fraction * HelpersHBEFA::computeCO2(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) veh.getAcceleration()));
    NOx += (fraction * HelpersHBEFA::computeNOx(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) veh.getAcceleration()));
    PMx += (fraction * HelpersHBEFA::computePMx(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) veh.getAcceleration()));
    fuel += (fraction * HelpersHBEFA::computeFuel(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) veh.getAcceleration()));
    return ret;
}


void
MSMeanData_HBEFA::MSLaneMeanDataValues::dismissByLaneChange(MSVehicle& veh) throw()
{
}


bool
MSMeanData_HBEFA::MSLaneMeanDataValues::isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw()
{
    SUMOReal fraction = 1.;
    SUMOReal l = veh.getVehicleType().getLength();
    if (veh.getPositionOnLane()+l>getLane()->length()) {
        fraction = l - (getLane()->length()-veh.getPositionOnLane());
    }
    sampleSeconds += fraction;
    CO += (fraction * HelpersHBEFA::computeCO(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) veh.getAcceleration()));
    CO2 += (fraction * HelpersHBEFA::computeCO2(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) veh.getAcceleration()));
    NOx += (fraction * HelpersHBEFA::computeNOx(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) veh.getAcceleration()));
    PMx += (fraction * HelpersHBEFA::computePMx(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) veh.getAcceleration()));
    fuel += (fraction * HelpersHBEFA::computeFuel(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) veh.getAcceleration()));
    return true;
}



// ---------------------------------------------------------------------------
// MSMeanData_HBEFA - methods
// ---------------------------------------------------------------------------
MSMeanData_HBEFA::MSMeanData_HBEFA(const std::string &id, 
                               unsigned int t, MSEdgeControl &edges,
                               const std::vector<int> &dumpBegins,
                               const std::vector<int> &dumpEnds,
                               bool useLanes,
                               bool withEmptyEdges, bool withEmptyLanes) throw()
        : myID(id), myInterval(t), 
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


MSMeanData_HBEFA::~MSMeanData_HBEFA() throw()
{}


void
MSMeanData_HBEFA::resetOnly(SUMOTime stopTime) throw()
{
        for (vector<vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
            for (vector<MSLaneMeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
                (*j)->reset();
            }
        }
}


void
MSMeanData_HBEFA::write(OutputDevice &dev,
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
MSMeanData_HBEFA::writeEdge(OutputDevice &dev,
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
            SUMOReal coS = 0;
            SUMOReal co2S = 0;
            SUMOReal pmxS = 0;
            SUMOReal noxS = 0;
            SUMOReal fuelS = 0;
            SUMOReal nVehS = 0;
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                MSLaneMeanDataValues& meanData = *(*lane);
                // calculate mean data
                coS += meanData.CO;
                co2S += meanData.CO2;
                pmxS += meanData.PMx;
                noxS += meanData.NOx;
                fuelS += meanData.fuel;
                nVehS += meanData.sampleSeconds;
                meanData.reset();
            }
            if (myDumpEmptyEdges||nVehS>0) {
                dev<<"      <edge id=\""<<edge->getID()<<
                "\" sampledSeconds=\""<< nVehS <<
                "\" CO_abs=\""<< coS <<
                "\" CO2_abs=\""<<co2S<<
                "\" PMx_abs=\""<<pmxS<<
                "\" NOx_abs=\""<<noxS<<
                "\" fuel_abs=\""<<fuelS<<
                "\" CO_normed=\""<<norm(coS, (SUMOReal) (stopTime-startTime+1), (*edge->getLanes())[0]->length()) <<
                "\" CO2_normed=\""<<norm(co2S, (SUMOReal) (stopTime-startTime+1), (*edge->getLanes())[0]->length())<<
                "\" PMx_normed=\""<<norm(pmxS, (SUMOReal) (stopTime-startTime+1), (*edge->getLanes())[0]->length())<<
                "\" NOx_normed=\""<<norm(noxS, (SUMOReal) (stopTime-startTime+1), (*edge->getLanes())[0]->length())<<
                "\" fuel_normed=\""<<norm(fuelS, (SUMOReal) (stopTime-startTime+1), (*edge->getLanes())[0]->length())<<
                "\"/>\n";
            }
        }
}


void
MSMeanData_HBEFA::writeLane(OutputDevice &dev,
                          MSLaneMeanDataValues &laneValues,
                          SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    if (myDumpEmptyLanes||laneValues.sampleSeconds>0) {
        // calculate mean data
        dev<<"         <lane id=\""<<laneValues.getLane()->getID()<<
                "\" sampledSeconds=\""<< laneValues.sampleSeconds <<
                "\" CO_abs=\""<< laneValues.CO <<
                "\" CO2_abs=\""<<laneValues.CO2<<
                "\" PMx_abs=\""<<laneValues.PMx<<
                "\" NOx_abs=\""<<laneValues.NOx<<
                "\" fuel_abs=\""<<laneValues.fuel<<
                "\" CO_normed=\""<<norm(laneValues.CO, (SUMOReal) (stopTime-startTime+1), laneValues.getLane()->length()) <<
                "\" CO2_normed=\""<<norm(laneValues.CO2, (SUMOReal) (stopTime-startTime+1), laneValues.getLane()->length())<<
                "\" PMx_normed=\""<<norm(laneValues.PMx, (SUMOReal) (stopTime-startTime+1), laneValues.getLane()->length())<<
                "\" NOx_normed=\""<<norm(laneValues.NOx, (SUMOReal) (stopTime-startTime+1), laneValues.getLane()->length())<<
                "\" fuel_normed=\""<<norm(laneValues.fuel, (SUMOReal) (stopTime-startTime+1), laneValues.getLane()->length())<<
                "\"/>\n";
    }
    laneValues.reset();
}


void
MSMeanData_HBEFA::writeXMLOutput(OutputDevice &dev,
                               SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    dev<<"   <interval begin=\""<<startTime<<"\" end=\""<<
    stopTime<<"\" "<<"id=\""<<myID<<"\">\n";
    write(dev, startTime, stopTime);
    dev<<"   </interval>\n";
}


void
MSMeanData_HBEFA::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError)
{
    dev.writeXMLHeader("netstats");
}


/****************************************************************************/

