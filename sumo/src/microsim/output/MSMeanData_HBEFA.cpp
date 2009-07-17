/****************************************************************************/
/// @file    MSMeanData_HBEFA.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Emission data collector for edges/lanes
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
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// MSMeanData_HBEFA::MSLaneMeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData_HBEFA::MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane * const lane) throw()
        : MSMoveReminder(lane), sampleSeconds(0), CO2(0), CO(0), HC(0),
        NOx(0), PMx(0), fuel(0) {}


MSMeanData_HBEFA::MSLaneMeanDataValues::~MSLaneMeanDataValues() throw() {
}


void
MSMeanData_HBEFA::MSLaneMeanDataValues::reset() throw() {
    sampleSeconds = 0.;
    CO2 = 0;
    CO = 0;
    HC = 0;
    NOx = 0;
    PMx = 0;
    fuel = 0;
}


bool
MSMeanData_HBEFA::MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    bool ret = true;
    SUMOReal fraction = 1.;
    if (oldPos<0&&newSpeed!=0) {
        fraction = (oldPos+SPEED2DIST(newSpeed)) / newSpeed;
        ++vehicleNo;
    }
    if (oldPos+SPEED2DIST(newSpeed)>getLane()->length()&&newSpeed!=0) {
        fraction -= (oldPos+SPEED2DIST(newSpeed) - getLane()->length()) / newSpeed;
        ret = false;
    }
    if(fraction<0) {
        MsgHandler::getErrorInstance()->inform("Negative vehicle step fraction on lane '" + getLane()->getID() + "'.");
        return false;
    }
    if(fraction==0) {
        return false;
    }
    sampleSeconds += fraction;
    SUMOReal a = veh.getPreDawdleAcceleration();
    CO += (fraction * HelpersHBEFA::computeCO(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    CO2 += (fraction * HelpersHBEFA::computeCO2(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    HC += (fraction * HelpersHBEFA::computeHC(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    NOx += (fraction * HelpersHBEFA::computeNOx(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    PMx += (fraction * HelpersHBEFA::computePMx(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    fuel += (fraction * HelpersHBEFA::computeFuel(veh.getVehicleType().getEmissionClass(), (double) newSpeed, (double) a));
    return ret;
}


void
MSMeanData_HBEFA::MSLaneMeanDataValues::dismissByLaneChange(MSVehicle& veh) throw() {
    SUMOReal pos = veh.getPositionOnLane();
    vehicleNo -= ((myLane->length()-pos) / myLane->length());
}


bool
MSMeanData_HBEFA::MSLaneMeanDataValues::isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw() {
    SUMOReal fraction = 1.;
    SUMOReal l = veh.getVehicleType().getLength();
    SUMOReal pos = veh.getPositionOnLane();
    if (veh.getPositionOnLane()+l>getLane()->length()) {
        fraction = l - (getLane()->length()-pos);
    }
    if(isEmit) {
        pos -= veh.getSpeed();
        if(pos<0) {
            pos = 0;
        }
    }
    vehicleNo += ((myLane->length()-pos) / myLane->length());
    if(fraction<0) {
        MsgHandler::getErrorInstance()->inform("Negative vehicle step fraction on lane '" + getLane()->getID() + "'.");
        return false;
    }
    if(fraction==0) {
        return false;
    }
    sampleSeconds += fraction;
    SUMOReal a = veh.getPreDawdleAcceleration();
    CO += (fraction * HelpersHBEFA::computeCO(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) a));
    CO2 += (fraction * HelpersHBEFA::computeCO2(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) a));
    HC += (fraction * HelpersHBEFA::computeHC(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) a));
    NOx += (fraction * HelpersHBEFA::computeNOx(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) a));
    PMx += (fraction * HelpersHBEFA::computePMx(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) a));
    fuel += (fraction * HelpersHBEFA::computeFuel(veh.getVehicleType().getEmissionClass(), (double) veh.getSpeed(), (double) a));
    return true;
}



// ---------------------------------------------------------------------------
// MSMeanData_HBEFA - methods
// ---------------------------------------------------------------------------
MSMeanData_HBEFA::MSMeanData_HBEFA(const std::string &id,
                                   MSEdgeControl &ec,
                                   SUMOTime dumpBegin,
                                   SUMOTime dumpEnd,
                                   bool useLanes,
                                   bool withEmptyEdges, bool withEmptyLanes) throw()
        : myID(id),
        myAmEdgeBased(!useLanes), myDumpBegin(dumpBegin), myDumpEnd(dumpEnd),
        myDumpEmptyEdges(withEmptyEdges), myDumpEmptyLanes(withEmptyLanes) {
    const std::vector<MSEdge*> &edges = ec.getEdges();
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        std::vector<MSLaneMeanDataValues*> v;
        const MSEdge::LaneCont * const lanes = (*e)->getLanes();
        MSEdge::LaneCont::const_iterator lane;
        for (lane = lanes->begin(); lane != lanes->end(); ++lane) {
            v.push_back(new MSLaneMeanDataValues(*lane));
        }
        myMeasures.push_back(v);
        myEdges.push_back(*e);
    }
}


MSMeanData_HBEFA::~MSMeanData_HBEFA() throw() {}


void
MSMeanData_HBEFA::resetOnly(SUMOTime stopTime) throw() {
    for (std::vector<std::vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
        for (std::vector<MSLaneMeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            SUMOReal nVehNo = 0.;
            SUMOReal laneLength = (*j)->getLane()->length();
            const MSLane::VehCont &vehs = (*j)->getLane()->getVehiclesSecure();
            for(MSLane::VehCont::const_iterator k=vehs.begin(); k!=vehs.end(); ++k) {
                SUMOReal pos = (*k)->getPositionOnLane();
                nVehNo += ((laneLength-pos) / laneLength);
            }
            (*j)->vehicleNo = nVehNo;
            (*j)->getLane()->releaseVehicles();
            (*j)->reset();
        }
    }
}


void
MSMeanData_HBEFA::writeEdge(OutputDevice &dev,
                            const std::vector<MSLaneMeanDataValues*> &edgeValues,
                            MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    std::vector<MSLaneMeanDataValues*>::const_iterator lane;
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
        SUMOReal coS = 0.;
        SUMOReal co2S = 0.;
        SUMOReal hcS = 0.;
        SUMOReal pmxS = 0.;
        SUMOReal noxS = 0.;
        SUMOReal fuelS = 0.;
        SUMOReal samplesS = 0.;
        SUMOReal vehicleNoS = 0.;
        for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
            MSLaneMeanDataValues& meanData = *(*lane);
            // calculate mean data
            coS += meanData.CO;
            co2S += meanData.CO2;
            hcS += meanData.HC;
            pmxS += meanData.PMx;
            noxS += meanData.NOx;
            fuelS += meanData.fuel;
            samplesS += meanData.sampleSeconds;
            SUMOReal oVehNo = meanData.vehicleNo;
            SUMOReal nVehNo = 0.;
            SUMOReal laneLength = meanData.getLane()->length();
            const MSLane::VehCont &vehs = meanData.getLane()->getVehiclesSecure();
            for(MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
                SUMOReal pos = (*i)->getPositionOnLane();
                oVehNo -= (laneLength-pos) / laneLength;
                nVehNo += (laneLength-pos) / laneLength;
            }
            vehicleNoS += oVehNo;
            meanData.vehicleNo = nVehNo;
            meanData.getLane()->releaseVehicles();
            meanData.reset();
        }
        if (myDumpEmptyEdges||samplesS>0) {
            SUMOReal length = (*edge->getLanes())[0]->length();
            dev<<"      <edge id=\""<<edge->getID()<<
            "\" sampledSeconds=\""<< samplesS <<
            "\" CO_abs=\""<< SUMOReal(coS*1000.) <<
            "\" CO2_abs=\""<<SUMOReal(co2S*1000.) <<
            "\" HC_abs=\""<<SUMOReal(hcS*1000.) <<
            "\" PMx_abs=\""<<SUMOReal(pmxS*1000.) <<
            "\" NOx_abs=\""<<SUMOReal(noxS*1000.) <<
            "\" fuel_abs=\""<<SUMOReal(fuelS*1000.) <<
            "\" CO_normed=\""<<normKMH(coS, (SUMOReal)(stopTime-startTime), length) <<
            "\" CO2_normed=\""<<normKMH(co2S, (SUMOReal)(stopTime-startTime), length)<<
            "\" HC_normed=\""<<normKMH(hcS, (SUMOReal)(stopTime-startTime), length)<<
            "\" PMx_normed=\""<<normKMH(pmxS, (SUMOReal)(stopTime-startTime), length)<<
            "\" NOx_normed=\""<<normKMH(noxS, (SUMOReal)(stopTime-startTime), length)<<
            "\" fuel_normed=\""<<normKMH(fuelS, (SUMOReal)(stopTime-startTime), length);
            if(samplesS!=0) {
                dev<<"\" CO_perVeh=\""<<SUMOReal(normPerVeh(coS, samplesS, vehicleNoS)*1000.) <<
                "\" CO2_perVeh=\""<<SUMOReal(normPerVeh(co2S, samplesS, vehicleNoS)*1000.)<<
                "\" HC_perVeh=\""<<SUMOReal(normPerVeh(hcS, samplesS, vehicleNoS)*1000.)<<
                "\" PMx_perVeh=\""<<SUMOReal(normPerVeh(pmxS, samplesS, vehicleNoS)*1000.)<<
                "\" NOx_perVeh=\""<<SUMOReal(normPerVeh(noxS, samplesS, vehicleNoS)*1000.)<<
                "\" fuel_perVeh=\""<<SUMOReal(normPerVeh(fuelS, samplesS, vehicleNoS)*1000.);
            }
            dev<<"\"/>\n";
        }
    }
}


void
MSMeanData_HBEFA::writeLane(OutputDevice &dev,
                            MSLaneMeanDataValues &laneValues,
                            SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    if (myDumpEmptyLanes||laneValues.sampleSeconds>0) {
        const MSLane::VehCont &vehs = laneValues.getLane()->getVehiclesSecure();
        SUMOReal oVehNo = laneValues.vehicleNo;
        SUMOReal nVehNo = 0;
        for(MSLane::VehCont::const_iterator i=vehs.begin(); i!=vehs.end(); ++i) {
            SUMOReal pos = (*i)->getPositionOnLane();
            oVehNo -= ((laneValues.getLane()->length()-pos) / laneValues.getLane()->length());
            nVehNo += ((laneValues.getLane()->length()-pos) / laneValues.getLane()->length());
        }
        laneValues.getLane()->releaseVehicles();
        SUMOReal length = laneValues.getLane()->length();
        dev<<"         <lane id=\""<<laneValues.getLane()->getID()<<
        "\" sampledSeconds=\""<< laneValues.sampleSeconds <<
        "\" CO_abs=\""<<SUMOReal(laneValues.CO*1000.) <<
        "\" CO2_abs=\""<<SUMOReal(laneValues.CO2*1000.) <<
        "\" HC_abs=\""<<SUMOReal(laneValues.HC*1000.) <<
        "\" PMx_abs=\""<<SUMOReal(laneValues.PMx*1000.) <<
        "\" NOx_abs=\""<<SUMOReal(laneValues.NOx*1000.) <<
        "\" fuel_abs=\""<<SUMOReal(laneValues.fuel*1000.) <<
        "\" CO_normed=\""<<normKMH(laneValues.CO, (SUMOReal)(stopTime-startTime), length) <<
        "\" CO2_normed=\""<<normKMH(laneValues.CO2, (SUMOReal)(stopTime-startTime), length)<<
        "\" HC_normed=\""<<normKMH(laneValues.HC, (SUMOReal)(stopTime-startTime), length)<<
        "\" PMx_normed=\""<<normKMH(laneValues.PMx, (SUMOReal)(stopTime-startTime), length)<<
        "\" NOx_normed=\""<<normKMH(laneValues.NOx, (SUMOReal)(stopTime-startTime), length)<<
        "\" fuel_normed=\""<<normKMH(laneValues.fuel, (SUMOReal)(stopTime-startTime), length);
        if(laneValues.sampleSeconds!=0) {
            dev<<"\" CO_perVeh=\""<<SUMOReal(normPerVeh(laneValues.CO, laneValues.sampleSeconds, oVehNo)*1000.)<<
            "\" CO2_perVeh=\""<<SUMOReal(normPerVeh(laneValues.CO2, laneValues.sampleSeconds, oVehNo)*1000.)<<
            "\" HC_perVeh=\""<<SUMOReal(normPerVeh(laneValues.HC, laneValues.sampleSeconds, oVehNo)*1000.)<<
            "\" PMx_perVeh=\""<<SUMOReal(normPerVeh(laneValues.PMx, laneValues.sampleSeconds, oVehNo)*1000.)<<
            "\" NOx_perVeh=\""<<SUMOReal(normPerVeh(laneValues.NOx, laneValues.sampleSeconds, oVehNo)*1000.)<<
            "\" fuel_perVeh=\""<<SUMOReal(normPerVeh(laneValues.fuel, laneValues.sampleSeconds, oVehNo)*1000.);
        }
        dev<<"\"/>\n";
        laneValues.vehicleNo = nVehNo;
    }
    laneValues.reset();
}


void
MSMeanData_HBEFA::writeXMLOutput(OutputDevice &dev,
                                 SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    // check whether this dump shall be written for the current time
    if (myDumpBegin < stopTime && myDumpEnd-DELTA_T >= startTime) {
        dev<<"   <interval begin=\""<<startTime<<"\" end=\""<<
        stopTime<<"\" "<<"id=\""<<myID<<"\">\n";
        std::vector<MSEdge*>::iterator edge = myEdges.begin();
        for (std::vector<std::vector<MSLaneMeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i, ++edge) {
            writeEdge(dev, (*i), *edge, startTime, stopTime);
        }
        dev<<"   </interval>\n";
    } else {
        resetOnly(stopTime);
    }
}


void
MSMeanData_HBEFA::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("netstats");
}


/****************************************************************************/

