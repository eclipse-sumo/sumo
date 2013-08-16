/****************************************************************************/
/// @file    RODFDetector.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Class representing a detector within the DFROUTER
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include "RODFDetector.h"
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <router/ROEdge.h>
#include "RODFEdge.h"
#include "RODFRouteDesc.h"
#include "RODFRouteCont.h"
#include "RODFDetectorFlow.h"
#include <utils/common/RandomDistributor.h>
#include <utils/common/StdDefs.h>
#include <utils/common/TplConvert.h>
#include <utils/geom/GeomHelper.h>
#include "RODFNet.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RODFDetector::RODFDetector(const std::string& id, const std::string& laneID,
                           SUMOReal pos, const RODFDetectorType type)
    : Named(id), myLaneID(laneID), myPosition(pos), myType(type), myRoutes(0) {}


RODFDetector::RODFDetector(const std::string& id, const RODFDetector& f)
    : Named(id), myLaneID(f.myLaneID), myPosition(f.myPosition),
      myType(f.myType), myRoutes(0) {
    if (f.myRoutes != 0) {
        myRoutes = new RODFRouteCont(*(f.myRoutes));
    }
}


RODFDetector::~RODFDetector() {
    delete myRoutes;
}


void
RODFDetector::setType(RODFDetectorType type) {
    myType = type;
}


SUMOReal
RODFDetector::computeDistanceFactor(const RODFRouteDesc& rd) const {
    SUMOReal distance = rd.edges2Pass[0]->getFromNode()->getPosition().distanceTo(rd.edges2Pass.back()->getToNode()->getPosition());
    SUMOReal length = 0;
    for (std::vector<ROEdge*>::const_iterator i = rd.edges2Pass.begin(); i != rd.edges2Pass.end(); ++i) {
        length += (*i)->getLength();
    }
    return (distance / length);
}


void
RODFDetector::computeSplitProbabilities(const RODFNet* net, const RODFDetectorCon& detectors,
                                        const RODFDetectorFlows& flows,
                                        SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset) {
    if (myRoutes == 0) {
        return;
    }
    // compute edges to determine split probabilities
    const std::vector<RODFRouteDesc>& routes = myRoutes->get();
    std::vector<RODFEdge*> nextDetEdges;
    for (std::vector<RODFRouteDesc>::const_iterator i = routes.begin(); i != routes.end(); ++i) {
        const RODFRouteDesc& rd = *i;
        bool hadSplit = false;
        bool hadDetectorAfterSplit = false;
        for (std::vector<ROEdge*>::const_iterator j = rd.edges2Pass.begin(); !hadDetectorAfterSplit && j != rd.edges2Pass.end(); ++j) {
            if (hadSplit && !hadDetectorAfterSplit && net->hasDetector(*j)) {
                hadDetectorAfterSplit = true;
                if (find(nextDetEdges.begin(), nextDetEdges.end(), *j) == nextDetEdges.end()) {
                    nextDetEdges.push_back(static_cast<RODFEdge*>(*j));
                }
                myRoute2Edge[rd.routename] = static_cast<RODFEdge*>(*j);
            }
            if ((*j)->getNoFollowing() > 1) {
                hadSplit = true;
            }
        }
    }
    // compute the probabilities to use a certain direction
    int index = 0;
    for (SUMOTime time = startTime; time < endTime; time += stepOffset, ++index) {
        mySplitProbabilities.push_back(std::map<RODFEdge*, SUMOReal>());
        SUMOReal overallProb = 0;
        // retrieve the probabilities
        for (std::vector<RODFEdge*>::const_iterator i = nextDetEdges.begin(); i != nextDetEdges.end(); ++i) {
            SUMOReal flow = detectors.getAggFlowFor(*i, time, 60, flows);
            overallProb += flow;
            mySplitProbabilities[index][*i] = flow;
        }
        // norm probabilities
        if (overallProb > 0) {
            for (std::vector<RODFEdge*>::const_iterator i = nextDetEdges.begin(); i != nextDetEdges.end(); ++i) {
                mySplitProbabilities[index][*i] = mySplitProbabilities[index][*i] / overallProb;
            }
        }
    }
}


void
RODFDetector::buildDestinationDistribution(const RODFDetectorCon& detectors,
        SUMOTime startTime, SUMOTime endTime, SUMOTime stepOffset,
        const RODFNet& net,
        std::map<size_t, RandomDistributor<size_t>* >& into) const {
    if (myRoutes == 0) {
        if (myType != DISCARDED_DETECTOR && myType != BETWEEN_DETECTOR) {
            WRITE_ERROR("Missing routes for detector '" + myID + "'.");
        }
        return;
    }
    std::vector<RODFRouteDesc>& descs = myRoutes->get();
    // iterate through time (in output interval steps)
    for (SUMOTime time = startTime; time < endTime; time += stepOffset) {
        into[time] = new RandomDistributor<size_t>();
        std::map<ROEdge*, SUMOReal> flowMap;
        // iterate through the routes
        size_t index = 0;
        for (std::vector<RODFRouteDesc>::iterator ri = descs.begin(); ri != descs.end(); ++ri, index++) {
            SUMOReal prob = 1.;
            for (std::vector<ROEdge*>::iterator j = (*ri).edges2Pass.begin(); j != (*ri).edges2Pass.end() && prob > 0; ++j) {
                if (!net.hasDetector(*j)) {
                    continue;
                }
                const RODFDetector& det = detectors.getAnyDetectorForEdge(static_cast<RODFEdge*>(*j));
                const std::vector<std::map<RODFEdge*, SUMOReal> >& probs = det.getSplitProbabilities();
                if (probs.size() == 0) {
                    prob = 0;
                    continue;
                }
                const std::map<RODFEdge*, SUMOReal>& tprobs = probs[(time - startTime) / stepOffset];
                for (std::map<RODFEdge*, SUMOReal>::const_iterator k = tprobs.begin(); k != tprobs.end(); ++k) {
                    if (find(j, (*ri).edges2Pass.end(), (*k).first) != (*ri).edges2Pass.end()) {
                        prob *= (*k).second;
                    }
                }
            }
            into[time]->add(prob, index);
            (*ri).overallProb = prob;
        }
    }
}


const std::vector<RODFRouteDesc>&
RODFDetector::getRouteVector() const {
    return myRoutes->get();
}


void
RODFDetector::addPriorDetector(RODFDetector* det) {
    myPriorDetectors.push_back(det);
}


void
RODFDetector::addFollowingDetector(RODFDetector* det) {
    myFollowingDetectors.push_back(det);
}


const std::vector<RODFDetector*>&
RODFDetector::getPriorDetectors() const {
    return myPriorDetectors;
}


const std::vector<RODFDetector*>&
RODFDetector::getFollowerDetectors() const {
    return myFollowingDetectors;
}



void
RODFDetector::addRoutes(RODFRouteCont* routes) {
    delete myRoutes;
    myRoutes = routes;
}


void
RODFDetector::addRoute(RODFRouteDesc& nrd) {
    if (myRoutes == 0) {
        myRoutes = new RODFRouteCont();
    }
    myRoutes->addRouteDesc(nrd);
}


bool
RODFDetector::hasRoutes() const {
    return myRoutes != 0 && myRoutes->get().size() != 0;
}


bool
RODFDetector::writeEmitterDefinition(const std::string& file,
                                     const std::map<size_t, RandomDistributor<size_t>* >& dists,
                                     const RODFDetectorFlows& flows,
                                     SUMOTime startTime, SUMOTime endTime,
                                     SUMOTime stepOffset,
                                     bool includeUnusedRoutes,
                                     SUMOReal scale,
                                     bool insertionsOnly,
                                     SUMOReal defaultSpeed) const {
    OutputDevice& out = OutputDevice::getDevice(file);
    OptionsCont& oc = OptionsCont::getOptions();
    if (getType() != SOURCE_DETECTOR) {
        out.writeXMLHeader("calibrator");
    }
    // routes
    if (myRoutes != 0 && myRoutes->get().size() != 0) {
        const std::vector<RODFRouteDesc>& routes = myRoutes->get();
        out.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_ID, myID);
        bool isEmptyDist = true;
        for (std::vector<RODFRouteDesc>::const_iterator i = routes.begin(); i != routes.end(); ++i) {
            if ((*i).overallProb > 0) {
                isEmptyDist = false;
            }
        }
        for (std::vector<RODFRouteDesc>::const_iterator i = routes.begin(); i != routes.end(); ++i) {
            if ((*i).overallProb > 0 || includeUnusedRoutes) {
                out.openTag(SUMO_TAG_ROUTE).writeAttr(SUMO_ATTR_REFID, (*i).routename).writeAttr(SUMO_ATTR_PROB, (*i).overallProb).closeTag();
            }
            if (isEmptyDist) {
                out.openTag(SUMO_TAG_ROUTE).writeAttr(SUMO_ATTR_REFID, (*i).routename).writeAttr(SUMO_ATTR_PROB, SUMOReal(1)).closeTag();
            }
        }
        out.closeTag(); // routeDistribution
    } else {
        WRITE_ERROR("Detector '" + getID() + "' has no routes!?");
        return false;
    }
    // insertions
    if (insertionsOnly || flows.knows(myID)) {
        // get the flows for this detector
        const std::vector<FlowDef>& mflows = flows.getFlowDefs(myID);
        // go through the simulation seconds
        unsigned int index = 0;
        for (SUMOTime time = startTime; time < endTime; time += stepOffset, index++) {
            // get own (departure flow)
            assert(index < mflows.size());
            const FlowDef& srcFD = mflows[index];  // !!! check stepOffset
            // get flows at end
            RandomDistributor<size_t>* destDist = dists.find(time) != dists.end() ? dists.find(time)->second : 0;
            // go through the cars
            size_t carNo = (size_t)((srcFD.qPKW + srcFD.qLKW) * scale);
            for (size_t car = 0; car < carNo; ++car) {
                // get the vehicle parameter
                SUMOReal v = -1;
                int destIndex = destDist != 0 && destDist->getOverallProb() > 0 ? (int) destDist->get() : -1;
                if (srcFD.isLKW >= 1) {
                    srcFD.isLKW = srcFD.isLKW - (SUMOReal) 1.;
                    v = srcFD.vLKW;
                } else {
                    v = srcFD.vPKW;
                }
                // compute insertion speed
                if (v <= 0 || v > 250) {
                    v = defaultSpeed;
                } else {
                    v = (SUMOReal)(v / 3.6);
                }
                // compute the departure time
                SUMOTime ctime = (SUMOTime)(time + ((SUMOReal) stepOffset * (SUMOReal) car / (SUMOReal) carNo));

                // write
                out.openTag(SUMO_TAG_VEHICLE);
                if (getType() == SOURCE_DETECTOR) {
                    out.writeAttr(SUMO_ATTR_ID, "emitter_" + myID + "_" + toString(ctime));
                } else {
                    out.writeAttr(SUMO_ATTR_ID, "calibrator_" + myID + "_" + toString(ctime));
                }
                out.writeAttr(SUMO_ATTR_DEPART, time2string(ctime));
                if (oc.isSet("departlane")) {
                    out.writeNonEmptyAttr(SUMO_ATTR_DEPARTLANE, oc.getString("departlane"));
                } else {
                    out.writeAttr(SUMO_ATTR_DEPARTLANE, TplConvert::_2int(myLaneID.substr(myLaneID.rfind("_") + 1).c_str()));
                }
                if (oc.isSet("departpos")) {
                    std::string posDesc = oc.getString("departpos");
                    if (posDesc.substr(0, 8) == "detector") {
                        SUMOReal position = myPosition;
                        if (posDesc.length() > 8) {
                            if (posDesc[8] == '+') {
                                position += TplConvert::_2SUMOReal(posDesc.substr(9).c_str());
                            } else if (posDesc[8] == '-') {
                                position -= TplConvert::_2SUMOReal(posDesc.substr(9).c_str());
                            } else {
                                throw NumberFormatException();
                            }
                        }
                        out.writeAttr(SUMO_ATTR_DEPARTPOS, position);
                    } else {
                        out.writeNonEmptyAttr(SUMO_ATTR_DEPARTPOS, posDesc);
                    }
                } else {
                    out.writeAttr(SUMO_ATTR_DEPARTPOS, myPosition);
                }
                if (oc.isSet("departspeed")) {
                    out.writeNonEmptyAttr(SUMO_ATTR_DEPARTSPEED, oc.getString("departspeed"));
                } else {
                    if (v > defaultSpeed) {
                        out.writeAttr(SUMO_ATTR_DEPARTSPEED, "max");
                    } else {
                        out.writeAttr(SUMO_ATTR_DEPARTSPEED, v);
                    }
                }
                if (oc.isSet("arrivallane")) {
                    out.writeNonEmptyAttr(SUMO_ATTR_ARRIVALLANE, oc.getString("arrivallane"));
                }
                if (oc.isSet("arrivalpos")) {
                    out.writeNonEmptyAttr(SUMO_ATTR_ARRIVALPOS, oc.getString("arrivalpos"));
                }
                if (oc.isSet("arrivalspeed")) {
                    out.writeNonEmptyAttr(SUMO_ATTR_ARRIVALSPEED, oc.getString("arrivalspeed"));
                }
                if (destIndex >= 0) {
                    out.writeAttr(SUMO_ATTR_ROUTE, myRoutes->get()[destIndex].routename);
                } else {
                    out.writeAttr(SUMO_ATTR_ROUTE, myID);
                }
                out.closeTag();
                srcFD.isLKW += srcFD.fLKW;
            }
        }
    }
    if (getType() != SOURCE_DETECTOR) {
        out.close();
    }
    return true;
}


bool
RODFDetector::writeRoutes(std::vector<std::string>& saved,
                          OutputDevice& out) {
    if (myRoutes != 0) {
        return myRoutes->save(saved, "", out);
    }
    return false;
}


void
RODFDetector::writeSingleSpeedTrigger(const std::string& file,
                                      const RODFDetectorFlows& flows,
                                      SUMOTime startTime, SUMOTime endTime,
                                      SUMOTime stepOffset, SUMOReal defaultSpeed) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("vss");
    const std::vector<FlowDef>& mflows = flows.getFlowDefs(myID);
    unsigned int index = 0;
    for (SUMOTime t = startTime; t < endTime; t += stepOffset, index++) {
        assert(index < mflows.size());
        const FlowDef& srcFD = mflows[index];
        SUMOReal speed = MAX2(srcFD.vLKW, srcFD.vPKW);
        if (speed <= 0 || speed > 250) {
            speed = defaultSpeed;
        } else {
            speed = (SUMOReal)(speed / 3.6);
        }
        out.openTag(SUMO_TAG_STEP).writeAttr(SUMO_ATTR_TIME, time2string(t)).writeAttr(SUMO_ATTR_SPEED, speed).closeTag();
    }
    out.close();
}










RODFDetectorCon::RODFDetectorCon() {}


RODFDetectorCon::~RODFDetectorCon() {
    for (std::vector<RODFDetector*>::iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        delete *i;
    }
}


bool
RODFDetectorCon::addDetector(RODFDetector* dfd) {
    if (myDetectorMap.find(dfd->getID()) != myDetectorMap.end()) {
        return false;
    }
    myDetectorMap[dfd->getID()] = dfd;
    myDetectors.push_back(dfd);
    std::string edgeid = dfd->getLaneID().substr(0, dfd->getLaneID().rfind('_'));
    if (myDetectorEdgeMap.find(edgeid) == myDetectorEdgeMap.end()) {
        myDetectorEdgeMap[edgeid] = std::vector<RODFDetector*>();
    }
    myDetectorEdgeMap[edgeid].push_back(dfd);
    return true; // !!!
}


bool
RODFDetectorCon::detectorsHaveCompleteTypes() const {
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        if ((*i)->getType() == TYPE_NOT_DEFINED) {
            return false;
        }
    }
    return true;
}


bool
RODFDetectorCon::detectorsHaveRoutes() const {
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        if ((*i)->hasRoutes()) {
            return true;
        }
    }
    return false;
}


const std::vector< RODFDetector*>&
RODFDetectorCon::getDetectors() const {
    return myDetectors;
}


void
RODFDetectorCon::save(const std::string& file) const {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("detectors");
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        out.openTag(SUMO_TAG_DETECTOR_DEFINITION).writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML((*i)->getID())).writeAttr(SUMO_ATTR_LANE, (*i)->getLaneID()).writeAttr(SUMO_ATTR_POSITION, (*i)->getPos());
        switch ((*i)->getType()) {
            case BETWEEN_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "between");
                break;
            case SOURCE_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "source");
                break;
            case SINK_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "sink");
                break;
            case DISCARDED_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "discarded");
                break;
            default:
                throw 1;
        }
        out.closeTag();
    }
    out.close();
}


void
RODFDetectorCon::saveAsPOIs(const std::string& file) const {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("pois");
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        out.openTag(SUMO_TAG_POI).writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML((*i)->getID()));
        switch ((*i)->getType()) {
            case BETWEEN_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "between_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor::BLUE);
                break;
            case SOURCE_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "source_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor::GREEN);
                break;
            case SINK_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "sink_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor::RED);
                break;
            case DISCARDED_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "discarded_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor(51, 51, 51, 255));
                break;
            default:
                throw 1;
        }
        out.writeAttr(SUMO_ATTR_LANE, (*i)->getLaneID()).writeAttr(SUMO_ATTR_POSITION, (*i)->getPos()).closeTag();
    }
    out.close();
}


void
RODFDetectorCon::saveRoutes(const std::string& file) const {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("routes");
    std::vector<std::string> saved;
    // write for source detectors
    bool lastWasSaved = true;
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        if ((*i)->getType() != SOURCE_DETECTOR) {
            // do not build routes for other than sources
            continue;
        }
        if (lastWasSaved) {
            out << "\n";
        }
        lastWasSaved = (*i)->writeRoutes(saved, out);
    }
    out << "\n";
    out.close();
}


const RODFDetector&
RODFDetectorCon::getDetector(const std::string& id) const {
    return *(myDetectorMap.find(id)->second);
}


bool
RODFDetectorCon::knows(const std::string& id) const {
    return myDetectorMap.find(id) != myDetectorMap.end();
}


void
RODFDetectorCon::writeEmitters(const std::string& file,
                               const RODFDetectorFlows& flows,
                               SUMOTime startTime, SUMOTime endTime,
                               SUMOTime stepOffset, const RODFNet& net,
                               bool writeCalibrators,
                               bool includeUnusedRoutes,
                               SUMOReal scale,
                               bool insertionsOnly) {
    // compute turn probabilities at detector
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        (*i)->computeSplitProbabilities(&net, *this, flows, startTime, endTime, stepOffset);
    }
    //
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional");
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        RODFDetector* det = *i;
        // get file name for values (emitter/calibrator definition)
        std::string escapedID = StringUtils::escapeXML(det->getID());
        std::string defFileName;
        if (det->getType() == SOURCE_DETECTOR) {
            defFileName = file;
        } else if (writeCalibrators && det->getType() == BETWEEN_DETECTOR) {
            defFileName = FileHelpers::getFilePath(file) + "calibrator_" + escapedID + ".def.xml";
        } else {
            defFileName = FileHelpers::getFilePath(file) + "other_" + escapedID + ".def.xml";
            continue;
        }
        // try to write the definition
        SUMOReal defaultSpeed = net.getEdge(det->getEdgeID())->getSpeed();
        //  ... compute routes' distribution over time
        std::map<size_t, RandomDistributor<size_t>* > dists;
        if (!insertionsOnly && flows.knows(det->getID())) {
            det->buildDestinationDistribution(*this, startTime, endTime, stepOffset, net, dists);
        }
        //  ... write the definition
        if (!det->writeEmitterDefinition(defFileName, dists, flows, startTime, endTime, stepOffset, includeUnusedRoutes, scale, insertionsOnly, defaultSpeed)) {
            // skip if something failed... (!!!)
            continue;
        }
        //  ... clear temporary values
        clearDists(dists);
        // write the declaration into the file
        if (writeCalibrators && det->getType() == BETWEEN_DETECTOR) {
            out.openTag(SUMO_TAG_CALIBRATOR).writeAttr(SUMO_ATTR_ID, "calibrator_" + escapedID).writeAttr(SUMO_ATTR_POSITION, det->getPos());
            out.writeAttr(SUMO_ATTR_LANE, det->getLaneID()).writeAttr(SUMO_ATTR_FRIENDLY_POS, true).writeAttr(SUMO_ATTR_FILE, defFileName).closeTag();
        }
    }
    out.close();
}


void
RODFDetectorCon::writeEmitterPOIs(const std::string& file,
                                  const RODFDetectorFlows& flows) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional");
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        RODFDetector* det = *i;
        SUMOReal flow = flows.getFlowSumSecure(det->getID());
        const unsigned char col = static_cast<unsigned char>(128 * flow / flows.getMaxDetectorFlow() + 128);
        out.openTag(SUMO_TAG_POI).writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML((*i)->getID()) + ":" + toString(flow));
        switch ((*i)->getType()) {
            case BETWEEN_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "between_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor(0, 0, col, 255));
                break;
            case SOURCE_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "source_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor(0, col, 0, 255));
                break;
            case SINK_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "sink_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor(col, 0, 0, 255));
                break;
            case DISCARDED_DETECTOR:
                out.writeAttr(SUMO_ATTR_TYPE, "discarded_detector_position").writeAttr(SUMO_ATTR_COLOR, RGBColor(51, 51, 51, 255));
                break;
            default:
                throw 1;
        }
        out.writeAttr(SUMO_ATTR_LANE, (*i)->getLaneID()).writeAttr(SUMO_ATTR_POSITION, (*i)->getPos()).closeTag();
    }
    out.close();
}


int
RODFDetectorCon::getAggFlowFor(const ROEdge* edge, SUMOTime time, SUMOTime period,
                               const RODFDetectorFlows&) const {
    UNUSED_PARAMETER(period);
    UNUSED_PARAMETER(time);
    if (edge == 0) {
        return 0;
    }
//    SUMOReal stepOffset = 60; // !!!
//    SUMOReal startTime = 0; // !!!
//    cout << edge->getID() << endl;
    assert(myDetectorEdgeMap.find(edge->getID()) != myDetectorEdgeMap.end());
    const std::vector<FlowDef>& flows = static_cast<const RODFEdge*>(edge)->getFlows();
    SUMOReal agg = 0;
    for (std::vector<FlowDef>::const_iterator i = flows.begin(); i != flows.end(); ++i) {
        const FlowDef& srcFD = *i;
        if (srcFD.qLKW >= 0) {
            agg += srcFD.qLKW;
        }
        if (srcFD.qPKW >= 0) {
            agg += srcFD.qPKW;
        }
    }
    return (int) agg;
    /* !!! make this time variable
    if (flows.size()!=0) {
        SUMOReal agg = 0;
        size_t beginIndex = (int)((time/stepOffset) - startTime);  // !!! falsch!!!
        for (SUMOTime t=0; t<period&&beginIndex<flows.size(); t+=(SUMOTime) stepOffset) {
            const FlowDef &srcFD = flows[beginIndex++];
            if (srcFD.qLKW>=0) {
                agg += srcFD.qLKW;
            }
            if (srcFD.qPKW>=0) {
                agg += srcFD.qPKW;
            }
        }
        return (int) agg;
    }
    */
//    return -1;
}


void
RODFDetectorCon::writeSpeedTrigger(const RODFNet* const net,
                                   const std::string& file,
                                   const RODFDetectorFlows& flows,
                                   SUMOTime startTime, SUMOTime endTime,
                                   SUMOTime stepOffset) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional");
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        RODFDetector* det = *i;
        // write the declaration into the file
        if (det->getType() == SINK_DETECTOR && flows.knows(det->getID())) {
            std::string filename = FileHelpers::getFilePath(file) + "vss_" + det->getID() + ".def.xml";
            out.openTag(SUMO_TAG_VSS).writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML(det->getID())).writeAttr(SUMO_ATTR_LANES, det->getLaneID()).writeAttr(SUMO_ATTR_FILE, filename).closeTag();
            SUMOReal defaultSpeed = net != 0 ? net->getEdge(det->getEdgeID())->getSpeed() : (SUMOReal) 200.;
            det->writeSingleSpeedTrigger(filename, flows, startTime, endTime, stepOffset, defaultSpeed);
        }
    }
    out.close();
}


void
RODFDetectorCon::writeEndRerouterDetectors(const std::string& file) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional");
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        RODFDetector* det = *i;
        // write the declaration into the file
        if (det->getType() == SINK_DETECTOR) {
            out.openTag(SUMO_TAG_REROUTER).writeAttr(SUMO_ATTR_ID, "endrerouter_" + StringUtils::escapeXML(det->getID())).writeAttr(SUMO_ATTR_EDGES, det->getLaneID());
            out.writeAttr(SUMO_ATTR_POSITION, SUMOReal(0)).writeAttr(SUMO_ATTR_FILE, "endrerouter_" + det->getID() + ".def.xml").closeTag();
        }
    }
    out.close();
}


void
RODFDetectorCon::writeValidationDetectors(const std::string& file,
        bool includeSources,
        bool singleFile, bool friendly) {
    OutputDevice& out = OutputDevice::getDevice(file);
    out.writeXMLHeader("additional");
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        RODFDetector* det = *i;
        // write the declaration into the file
        if (det->getType() != SOURCE_DETECTOR || includeSources) {
            SUMOReal pos = det->getPos();
            if (det->getType() == SOURCE_DETECTOR) {
                pos += 1;
            }
            out.openTag(SUMO_TAG_E1DETECTOR).writeAttr(SUMO_ATTR_ID, "validation_" + StringUtils::escapeXML(det->getID())).writeAttr(SUMO_ATTR_LANE, det->getLaneID());
            out.writeAttr(SUMO_ATTR_POSITION, pos).writeAttr(SUMO_ATTR_FREQUENCY, 60);
            if (friendly) {
                out.writeAttr(SUMO_ATTR_FRIENDLY_POS, true);
            }
            if (!singleFile) {
                out.writeAttr(SUMO_ATTR_FILE, "validation_det_" + StringUtils::escapeXML(det->getID()) + ".xml");
            } else {
                out.writeAttr(SUMO_ATTR_FILE, "validation_dets.xml");
            }
            out.closeTag();
        }
    }
    out.close();
}


void
RODFDetectorCon::removeDetector(const std::string& id) {
    //
    std::map<std::string, RODFDetector*>::iterator ri1 = myDetectorMap.find(id);
    RODFDetector* oldDet = (*ri1).second;
    myDetectorMap.erase(ri1);
    //
    std::vector<RODFDetector*>::iterator ri2 =
        find(myDetectors.begin(), myDetectors.end(), oldDet);
    myDetectors.erase(ri2);
    //
    bool found = false;
    for (std::map<std::string, std::vector<RODFDetector*> >::iterator rr3 = myDetectorEdgeMap.begin(); !found && rr3 != myDetectorEdgeMap.end(); ++rr3) {
        std::vector<RODFDetector*>& dets = (*rr3).second;
        for (std::vector<RODFDetector*>::iterator ri3 = dets.begin(); !found && ri3 != dets.end();) {
            if (*ri3 == oldDet) {
                found = true;
                ri3 = dets.erase(ri3);
            } else {
                ++ri3;
            }
        }
    }
    delete oldDet;
}


void
RODFDetectorCon::guessEmptyFlows(RODFDetectorFlows& flows) {
    // routes must be built (we have ensured this in main)
    // detector followers/prior must be build (we have ensured this in main)
    //
    bool changed = true;
    while (changed) {
        for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
            RODFDetector* det = *i;
            const std::vector<RODFDetector*>& prior = det->getPriorDetectors();
            const std::vector<RODFDetector*>& follower = det->getFollowerDetectors();
            size_t noFollowerWithRoutes = 0;
            size_t noPriorWithRoutes = 0;
            // count occurences of detectors with/without routes
            std::vector<RODFDetector*>::const_iterator j;
            for (j = prior.begin(); j != prior.end(); ++j) {
                if (flows.knows((*j)->getID())) {
                    ++noPriorWithRoutes;
                }
            }
            assert(noPriorWithRoutes <= prior.size());
            for (j = follower.begin(); j != follower.end(); ++j) {
                if (flows.knows((*j)->getID())) {
                    ++noFollowerWithRoutes;
                }
            }
            assert(noFollowerWithRoutes <= follower.size());

            // do not process detectors which have no routes
            if (!flows.knows(det->getID())) {
                continue;
            }

            // plain case: some of the following detectors have no routes
            if (noFollowerWithRoutes == follower.size()) {
                // the number of vehicles is the sum of all vehicles on prior
                continue;
            }

        }
    }
}


const RODFDetector&
RODFDetectorCon::getAnyDetectorForEdge(const RODFEdge* const edge) const {
    for (std::vector<RODFDetector*>::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        if ((*i)->getEdgeID() == edge->getID()) {
            return **i;
        }
    }
    throw 1;
}


void
RODFDetectorCon::clearDists(std::map<size_t, RandomDistributor<size_t>* >& dists) const {
    for (std::map<size_t, RandomDistributor<size_t>* >::iterator i = dists.begin(); i != dists.end(); ++i) {
        delete(*i).second;
    }
}


void
RODFDetectorCon::mesoJoin(const std::string& nid,
                          const std::vector<std::string>& oldids) {
    // build the new detector
    const RODFDetector& first = getDetector(*(oldids.begin()));
    RODFDetector* newDet = new RODFDetector(nid, first);
    addDetector(newDet);
    // delete previous
    for (std::vector<std::string>::const_iterator i = oldids.begin(); i != oldids.end(); ++i) {
        removeDetector(*i);
    }
}


/****************************************************************************/
