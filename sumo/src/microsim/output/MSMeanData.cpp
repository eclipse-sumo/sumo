/****************************************************************************/
/// @file    MSMeanData.cpp
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
#include "MSMeanData.h"
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
// MSMeanData::MeanDataValues - methods
// ---------------------------------------------------------------------------
MSMeanData::MeanDataValues::MeanDataValues(MSLane * const lane, const std::set<std::string>* const vTypes) throw()
        : MSMoveReminder(lane), sampleSeconds(0), travelledDistance(0), myVehicleTypes(vTypes) {}


MSMeanData::MeanDataValues::~MeanDataValues() throw() {
}


bool
MSMeanData::MeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw() {
    return myVehicleTypes == 0 || myVehicleTypes->empty() ||
           myVehicleTypes->find(veh.getVehicleType().getID()) != myVehicleTypes->end();
}


bool
MSMeanData::MeanDataValues::notifyEnter(MSVehicle& veh, bool isEmit, bool isLaneChange) throw() {
    return myVehicleTypes == 0 || myVehicleTypes->empty() ||
           myVehicleTypes->find(veh.getVehicleType().getID()) != myVehicleTypes->end();
}


bool
MSMeanData::MeanDataValues::isEmpty() const throw() {
    return sampleSeconds == 0;
}


void
MSMeanData::MeanDataValues::update() throw() {
}



// ---------------------------------------------------------------------------
// MSMeanData - methods
// ---------------------------------------------------------------------------
MSMeanData::MSMeanData(const std::string &id,
                       const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                       const bool useLanes, const bool withEmpty,
                       const SUMOReal maxTravelTime, const SUMOReal minSamples,
                       const std::set<std::string> vTypes) throw()
        : myID(id), myAmEdgeBased(!useLanes), myDumpBegin(dumpBegin), myDumpEnd(dumpEnd),
        myDumpEmpty(withEmpty), myMaxTravelTime(maxTravelTime), myMinSamples(minSamples), myVehicleTypes(vTypes) {
}


void
MSMeanData::init(const MSEdgeControl &ec, const bool withInternal) throw() {
    const std::vector<MSEdge*> &edges = ec.getEdges();
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        if (withInternal || (*e)->getPurpose() != MSEdge::EDGEFUNCTION_INTERNAL) {
            std::vector<MeanDataValues*> v;
#ifdef HAVE_MESOSIM
            if (MSGlobals::gUseMesoSim) {
                MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(**e);
                while (s!=0) {
                    v.push_back(createValues(0));
                    s->addDetector(v.back());
                    s = s->getNextSegment();
                }
                continue;
            }
#endif
            const std::vector<MSLane*> &lanes = (*e)->getLanes();
            for (std::vector<MSLane*>::const_iterator lane = lanes.begin(); lane != lanes.end(); ++lane) {
                v.push_back(createValues(*lane));
            }
            myMeasures.push_back(v);
            myEdges.push_back(*e);
        }
    }
}

    
MSMeanData::~MSMeanData() throw() {}


void
MSMeanData::resetOnly(SUMOTime stopTime) throw() {
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        std::vector<MSEdge*>::iterator edge = myEdges.begin();
        for (std::vector<std::vector<MeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i, ++edge) {
            MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(**edge);
            for (std::vector<MeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
                s->prepareMeanDataForWriting(*(*j), (SUMOReal) stopTime);
                (*j)->reset();
                s = s->getNextSegment();
            }
        }
        return;
    }
#endif
    for (std::vector<std::vector<MeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
        for (std::vector<MeanDataValues*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            (*j)->reset();
        }
    }
}


void
MSMeanData::writeEdge(OutputDevice &dev,
                            const std::vector<MeanDataValues*> &edgeValues,
                            MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    std::vector<MeanDataValues*>::const_iterator lane;
    if (!myAmEdgeBased) {
        bool writeCheck = myDumpEmpty;
        if (!writeCheck) {
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                if (!(*lane)->isEmpty()) {
                    writeCheck = true;
                    break;
                }
            }
        }
        if (writeCheck) {
            dev.openTag("edge")<<" id=\""<<edge->getID()<<"\">\n";
            for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
                MeanDataValues& meanData = **lane;
                writeValues(dev, "<lane id=\""+meanData.getLane()->getID(),
                            meanData, (SUMOReal)(stopTime - startTime), 1.f, meanData.getLane()->getLength());
                meanData.reset();
            }
            dev.closeTag();
        }
    } else {
        MeanDataValues* sumData = createValues(0);
        for (lane = edgeValues.begin(); lane != edgeValues.end(); ++lane) {
            MeanDataValues& meanData = **lane;
            sumData->add(meanData);
            meanData.reset();
        }
        writeValues(dev, "<edge id=\""+edge->getID(),
                    *sumData, (SUMOReal)(stopTime - startTime),
                    (SUMOReal)edge->getLanes().size(), edge->getLanes()[0]->getLength());
        delete sumData;
    }
}


void
MSMeanData::writeXMLOutput(OutputDevice &dev,
                                 SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    // check whether this dump shall be written for the current time
    if (myDumpBegin < stopTime && myDumpEnd-DELTA_T >= startTime) {
        dev.openTag("interval")<<" begin=\""<<startTime<<"\" end=\""<<
        stopTime<<"\" "<<"id=\""<<myID<<"\">\n";
        std::vector<MSEdge*>::iterator edge = myEdges.begin();
        for (std::vector<std::vector<MeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i, ++edge) {
            writeEdge(dev, (*i), *edge, startTime, stopTime);
        }
        dev.closeTag();
    } else {
        resetOnly(stopTime);
    }
}


void
MSMeanData::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("netstats");
}


void
MSMeanData::update() throw() {
    for (std::vector<std::vector<MeanDataValues*> >::const_iterator i=myMeasures.begin(); i!=myMeasures.end(); ++i) {
        const std::vector<MeanDataValues*> &lm = *i;
        for (std::vector<MeanDataValues*>::const_iterator j=lm.begin(); j!=lm.end(); ++j) {
            (*j)->update();
        }
    }
}


/****************************************************************************/

