/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIEdge.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
///
// A road/street connecting two junctions (gui-version)
/****************************************************************************/
#include <config.h>

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/fxheader.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <microsim/MSBaseVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLaneChanger.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSGlobals.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIDesigns.h>
#include <mesogui/GUIMEVehicleControl.h>
#include <mesogui/GUIMEVehicle.h>
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#include <mesosim/MEVehicle.h>

#include "GUITriggeredRerouter.h"
#include "GUIEdge.h"
#include "GUIVehicle.h"
#include "GUINet.h"
#include "GUILane.h"
#include "GUIPerson.h"
#include "GUIContainer.h"


GUIEdge::GUIEdge(const std::string& id, int numericalID,
                 const SumoXMLEdgeFunc function,
                 const std::string& streetName, const std::string& edgeType, int priority,
                 double distance) :
    MSEdge(id, numericalID, function, streetName, edgeType, priority, distance),
    GUIGlObject(GLO_EDGE, id, GUIIconSubSys::getIcon(GUIIcon::EDGE)),
    myLock(true)
{}


GUIEdge::~GUIEdge() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}

void
GUIEdge::closeBuilding() {
    MSEdge::closeBuilding();
    bool hasNormalSuccessors = false;
    for (const MSEdge* out : getSuccessors()) {
        if (!out->isTazConnector()) {
            hasNormalSuccessors = true;
            break;
        }
    }
    myShowDeadEnd = (!isTazConnector() && !hasNormalSuccessors && getToJunction()->getOutgoing().size() > 0
                     && (getPermissions() & ~SVC_PEDESTRIAN) != 0
                     && (getToJunction()->getOutgoing().size() > 1 ||
                         getToJunction()->getOutgoing().front()->getToJunction() != getFromJunction()));
}

MSLane&
GUIEdge::getLane(int laneNo) {
    assert(laneNo < (int)myLanes->size());
    return *((*myLanes)[laneNo]);
}


std::vector<GUIGlID>
GUIEdge::getIDs(bool includeInternal) {
    std::vector<GUIGlID> ret;
    ret.reserve(MSEdge::myDict.size());
    for (MSEdge::DictType::const_iterator i = MSEdge::myDict.begin(); i != MSEdge::myDict.end(); ++i) {
        const GUIEdge* edge = dynamic_cast<const GUIEdge*>(i->second);
        assert(edge);
        if (includeInternal || edge->isNormal()) {
            ret.push_back(edge->getGlID());
        }
    }
    return ret;
}


double
GUIEdge::getTotalLength(bool includeInternal, bool eachLane) {
    double result = 0;
    for (MSEdge::DictType::const_iterator i = MSEdge::myDict.begin(); i != MSEdge::myDict.end(); ++i) {
        const MSEdge* edge = i->second;
        if (includeInternal || !edge->isInternal()) {
            // @note needs to be change once lanes may have different length
            result += edge->getLength() * (eachLane ? (double)edge->getLanes().size() : 1.);
        }
    }
    return result;
}


Boundary
GUIEdge::getBoundary() const {
    Boundary ret;
    if (!isTazConnector()) {
        for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
            ret.add((*i)->getShape().getBoxBoundary());
        }
    } else {
        // take the starting coordinates of all follower edges and the endpoints
        // of all successor edges
        for (MSEdgeVector::const_iterator it = mySuccessors.begin(); it != mySuccessors.end(); ++it) {
            const std::vector<MSLane*>& lanes = (*it)->getLanes();
            for (std::vector<MSLane*>::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); ++it_lane) {
                ret.add((*it_lane)->getShape().front());
            }
        }
        for (MSEdgeVector::const_iterator it = myPredecessors.begin(); it != myPredecessors.end(); ++it) {
            const std::vector<MSLane*>& lanes = (*it)->getLanes();
            for (std::vector<MSLane*>::const_iterator it_lane = lanes.begin(); it_lane != lanes.end(); ++it_lane) {
                ret.add((*it_lane)->getShape().back());
            }
        }
    }
    ret.grow(10);
    return ret;
}


GUIGLObjectPopupMenu*
GUIEdge::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    if (MSGlobals::gUseMesoSim) {
        buildShowParamsPopupEntry(ret);
        buildShowTypeParamsPopupEntry(ret);
    }
    MESegment* segment = getSegmentAtPosition(parent.getPositionInformation());
    GUIDesigns::buildFXMenuCommand(ret, "segment: " + toString(segment->getIndex()), nullptr, nullptr, 0);
    buildPositionCopyEntry(ret, app);
    return ret;
}


GUIParameterTableWindow*
GUIEdge::getParameterWindow(GUIMainWindow& app,
                            GUISUMOAbstractView& parent) {
    GUIParameterTableWindow* ret = nullptr;
    ret = new GUIParameterTableWindow(app, *this);
    // add edge items
    ret->mkItem(TL("max speed [m/s]"), false, getAllowedSpeed());
    ret->mkItem(TL("length [m]"), false, (*myLanes)[0]->getLength());
    ret->mkItem(TL("street name"), false, getStreetName());
    ret->mkItem(TL("pending insertions [#]"), true, new FunctionBinding<GUIEdge, double>(this, &GUIEdge::getPendingEmits));
    ret->mkItem(TL("mean friction [%]"), true, new FunctionBinding<GUIEdge, double>(this, &MSEdge::getMeanFriction, 100.));
    ret->mkItem(TL("mean vehicle speed [m/s]"), true, new FunctionBinding<GUIEdge, double>(this, &GUIEdge::getMeanSpeed));
    ret->mkItem(TL("routing speed [m/s]"), true, new FunctionBinding<MSEdge, double>(this, &MSEdge::getRoutingSpeed));
    ret->mkItem(TL("time penalty [s]"), true, new FunctionBinding<MSEdge, double>(this, &MSEdge::getTimePenalty));
    ret->mkItem(TL("brutto occupancy [%]"), true, new FunctionBinding<GUIEdge, double>(this, &GUIEdge::getBruttoOccupancy, 100.));
    ret->mkItem(TL("flow [veh/h/lane]"), true, new FunctionBinding<GUIEdge, double>(this, &GUIEdge::getFlow));
    ret->mkItem(TL("vehicles [#]"), true, new CastingFunctionBinding<GUIEdge, int, int>(this, &MSEdge::getVehicleNumber));
    // add segment items
    MESegment* segment = getSegmentAtPosition(parent.getPositionInformation());
    ret->mkItem(TL("segment index"), false, segment->getIndex());
    ret->mkItem(TL("segment queues"), false, segment->numQueues());
    ret->mkItem(TL("segment length [m]"), false, segment->getLength());
    ret->mkItem(TL("segment allowed speed [m/s]"), false, segment->getEdge().getSpeedLimit());
    ret->mkItem(TL("segment jam threshold [%]"), false, segment->getRelativeJamThreshold() * 100);
    ret->mkItem(TL("segment brutto occupancy [%]"), true, new FunctionBinding<MESegment, double>(segment, &MESegment::getRelativeOccupancy, 100));
    ret->mkItem(TL("segment mean vehicle speed [m/s]"), true, new FunctionBinding<MESegment, double>(segment, &MESegment::getMeanSpeed));
    ret->mkItem(TL("segment flow [veh/h/lane]"), true, new FunctionBinding<MESegment, double>(segment, &MESegment::getFlow));
    ret->mkItem(TL("segment vehicles [#]"), true, new CastingFunctionBinding<MESegment, int, int>(segment, &MESegment::getCarNumber));
    ret->mkItem(TL("segment leader leave time"), true, new FunctionBinding<MESegment, double>(segment, &MESegment::getEventTimeSeconds));
    ret->mkItem(TL("segment headway [s]"), true, new FunctionBinding<MESegment, double>(segment, &MESegment::getLastHeadwaySeconds));
    ret->mkItem(TL("segment entry block time [s]"), true, new FunctionBinding<MESegment, double>(segment, &MESegment::getEntryBlockTimeSeconds));
    // lane params
    for (MSLane* lane : *myLanes) {
        for (const auto& kv : lane->getParametersMap()) {
            ret->mkItem(("laneParam " + toString(lane->getIndex()) + ":" + kv.first).c_str(), false, kv.second);
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}

GUIParameterTableWindow*
GUIEdge::getTypeParameterWindow(GUIMainWindow& app,
                                GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    const MESegment::MesoEdgeType& edgeType = MSNet::getInstance()->getMesoType(getEdgeType());
    // add items
    ret->mkItem(TL("Type Information:"), false, "");
    ret->mkItem(TL("type [id]"), false, getEdgeType());
    ret->mkItem(TL("tauff"), false, STEPS2TIME(edgeType.tauff));
    ret->mkItem(TL("taufj"), false, STEPS2TIME(edgeType.taufj));
    ret->mkItem(TL("taujf"), false, STEPS2TIME(edgeType.taujf));
    ret->mkItem(TL("taujj"), false, STEPS2TIME(edgeType.taujj));
    ret->mkItem(TL("jam threshold"), false, edgeType.jamThreshold);
    ret->mkItem(TL("junction control"), false, edgeType.junctionControl);
    ret->mkItem(TL("tls penalty"), false, edgeType.tlsPenalty);
    ret->mkItem(TL("tls flow penalty"), false, edgeType.tlsFlowPenalty);
    ret->mkItem(TL("minor penalty"), false, STEPS2TIME(edgeType.minorPenalty));
    ret->mkItem(TL("overtaking"), false, edgeType.overtaking);
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GUIEdge::getCenteringBoundary() const {
    Boundary b = getBoundary();
    // ensure that vehicles and persons on the side are drawn even if the edge
    // is outside the view
    b.grow(10);
    return b;
}

const std::string
GUIEdge::getOptionalName() const {
    return myStreetName;
}

void
GUIEdge::drawGL(const GUIVisualizationSettings& s) const {
    if (s.hideConnectors && myFunction == SumoXMLEdgeFunc::CONNECTOR) {
        return;
    }
    GLHelper::pushName(getGlID());
    // draw the lanes
    if (MSGlobals::gUseMesoSim) {
        setColor(s);
    }
    for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
        static_cast<GUILane*>(*i)->drawGL(s);
    }
    if (MSGlobals::gUseMesoSim) {
        if (s.scale * s.vehicleSize.getExaggeration(s, nullptr) > s.vehicleSize.minSize) {
            drawMesoVehicles(s);
        }
    }
    GLHelper::popName();
    // (optionally) draw the name and/or the street name
    GUILane* lane2 = dynamic_cast<GUILane*>((*myLanes).back());
    const GUIGlObject* selCheck = gSelected.isSelected(this) ? (GUIGlObject*)this : (GUIGlObject*)lane2;
    const bool drawEdgeName = s.edgeName.show(selCheck) && myFunction == SumoXMLEdgeFunc::NORMAL;
    const bool drawInternalEdgeName = s.internalEdgeName.show(selCheck) && myFunction == SumoXMLEdgeFunc::INTERNAL;
    const bool drawCwaEdgeName = s.cwaEdgeName.show(selCheck) && (myFunction == SumoXMLEdgeFunc::CROSSING || myFunction == SumoXMLEdgeFunc::WALKINGAREA);
    const bool drawStreetName = s.streetName.show(selCheck) && myStreetName != "";
    const bool drawEdgeValue = s.edgeValue.show(selCheck) && (myFunction == SumoXMLEdgeFunc::NORMAL
                               || (myFunction == SumoXMLEdgeFunc::INTERNAL && !s.drawJunctionShape)
                               || ((myFunction == SumoXMLEdgeFunc::CROSSING || myFunction == SumoXMLEdgeFunc::WALKINGAREA) && s.drawCrossingsAndWalkingareas));
    const bool drawEdgeScaleValue = s.edgeScaleValue.show(selCheck) && (myFunction == SumoXMLEdgeFunc::NORMAL
                                    || (myFunction == SumoXMLEdgeFunc::INTERNAL && !s.drawJunctionShape)
                                    || ((myFunction == SumoXMLEdgeFunc::CROSSING || myFunction == SumoXMLEdgeFunc::WALKINGAREA) && s.drawCrossingsAndWalkingareas));
    if (drawEdgeName || drawInternalEdgeName || drawCwaEdgeName || drawStreetName || drawEdgeValue || drawEdgeScaleValue) {
        GUILane* lane1 = dynamic_cast<GUILane*>((*myLanes)[0]);
        if (lane1 != nullptr && lane2 != nullptr) {
            const bool s2 = s.secondaryShape;
            const bool spreadSuperposed = s.spreadSuperposed && getBidiEdge() != nullptr;
            Position p = lane1->getShape(s2).positionAtOffset(lane1->getShape(s2).length() / (double) 2.);
            p.add(lane2->getShape(s2).positionAtOffset(lane2->getShape(s2).length() / (double) 2.));
            p.mul(.5);
            if (spreadSuperposed) {
                // move name to the right of the edge and towards its beginning
                const double dist = 0.6 * s.edgeName.scaledSize(s.scale);
                const double shiftA = lane1->getShape(s2).rotationAtOffset(lane1->getShape(s2).length() / (double) 2.) - DEG2RAD(135);
                Position shift(dist * cos(shiftA), dist * sin(shiftA));
                p.add(shift);
            }
            double angle = s.getTextAngle(lane1->getShape(s2).rotationDegreeAtOffset(lane1->getShape(s2).length() / (double) 2.) + 90);
            if (drawEdgeName) {
                drawName(p, s.scale, s.edgeName, angle, true);
            } else if (drawInternalEdgeName) {
                drawName(p, s.scale, s.internalEdgeName, angle, true);
            } else if (drawCwaEdgeName) {
                drawName(p, s.scale, s.cwaEdgeName, angle, true);
            }
            if (drawStreetName) {
                GLHelper::drawTextSettings(s.streetName, getStreetName(), p, s.scale, angle);
            }
            if (drawEdgeValue) {
                const int activeScheme = s.getLaneEdgeMode();
                std::string value = "";
                if (activeScheme == 31) {
                    // edge param, could be non-numerical
                    value = getParameter(s.edgeParam, "");
                } else if (activeScheme == 32) {
                    // lane param, could be non-numerical
                    value = lane2->getParameter(s.laneParam, "");
                } else {
                    // use numerical value value of leftmost lane to hopefully avoid sidewalks, bikelanes etc
                    const double doubleValue = (MSGlobals::gUseMesoSim
                                                ? getColorValue(s, activeScheme)
                                                : lane2->getColorValueWithFunctional(s, activeScheme));
                    const RGBColor color = (MSGlobals::gUseMesoSim ? s.edgeColorer : s.laneColorer).getScheme().getColor(doubleValue);
                    if (doubleValue != s.MISSING_DATA
                            && color.alpha() != 0
                            && (!s.edgeValueRainBow.hideMin || doubleValue > s.edgeValueRainBow.minThreshold)
                            && (!s.edgeValueRainBow.hideMax || doubleValue < s.edgeValueRainBow.maxThreshold)
                       ) {
                        value = toString(doubleValue);
                    }
                }
                if (value != "") {
                    if (drawEdgeName || drawInternalEdgeName || drawCwaEdgeName) {
                        const double dist = 0.4 * (s.edgeName.scaledSize(s.scale) + s.edgeValue.scaledSize(s.scale));
                        const double shiftA = lane1->getShape(s2).rotationAtOffset(lane1->getShape(s2).length() / (double) 2.) - DEG2RAD(90);
                        Position shift(dist * cos(shiftA), dist * sin(shiftA));
                        p.add(shift);
                    }
                    GLHelper::drawTextSettings(s.edgeValue, value, p, s.scale, angle);
                }
            }
            if (drawEdgeScaleValue) {
                const int activeScheme = s.getLaneEdgeScaleMode();
                std::string value = "";
                // use numerical value value of leftmost lane to hopefully avoid sidewalks, bikelanes etc
                const double doubleValue = (MSGlobals::gUseMesoSim
                                            ? getScaleValue(s, activeScheme)
                                            : lane2->getScaleValue(s, activeScheme, s2));
                if (doubleValue != s.MISSING_DATA) {
                    value = toString(doubleValue);
                }
                if (value != "") {
                    if (drawEdgeName || drawInternalEdgeName || drawCwaEdgeName || drawEdgeValue) {
                        const double dist = 0.4 * (s.edgeName.scaledSize(s.scale) + s.edgeScaleValue.scaledSize(s.scale));
                        const double shiftA = lane1->getShape(s2).rotationAtOffset(lane1->getShape(s2).length() / (double) 2.) - DEG2RAD(90);
                        Position shift(dist * cos(shiftA), dist * sin(shiftA));
                        p.add(shift);
                    }
                    GLHelper::drawTextSettings(s.edgeScaleValue, value, p, s.scale, angle);
                }
            }
        }
    }
    if (s.scale * s.personSize.getExaggeration(s, nullptr) > s.personSize.minSize) {
        FXMutexLock locker(myLock);
        for (MSTransportable* t : myPersons) {
            GUIPerson* person = dynamic_cast<GUIPerson*>(t);
            assert(person != 0);
            person->drawGL(s);
        }
    }
    if (s.scale * s.containerSize.getExaggeration(s, nullptr) > s.containerSize.minSize) {
        FXMutexLock locker(myLock);
        for (MSTransportable* t : myContainers) {
            GUIContainer* container = dynamic_cast<GUIContainer*>(t);
            assert(container != 0);
            container->drawGL(s);
        }
    }
}


void
GUIEdge::drawMesoVehicles(const GUIVisualizationSettings& s) const {
    GUIMEVehicleControl* vehicleControl = GUINet::getGUIInstance()->getGUIMEVehicleControl();
    const double now = SIMTIME;
    if (vehicleControl != nullptr) {
        // draw the meso vehicles
        vehicleControl->secureVehicles();
        FXMutexLock locker(myLock);
        int laneIndex = 0;
        for (std::vector<MSLane*>::const_iterator msl = myLanes->begin(); msl != myLanes->end(); ++msl, ++laneIndex) {
            GUILane* l = static_cast<GUILane*>(*msl);
            // go through the vehicles
            double segmentOffset = 0; // offset at start of current segment
            for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this);
                    segment != nullptr; segment = segment->getNextSegment()) {
                const double length = segment->getLength();
                if (laneIndex < segment->numQueues()) {
                    // make a copy so we don't have to worry about synchronization
                    std::vector<MEVehicle*> queue = segment->getQueue(laneIndex);
                    const int queueSize = (int)queue.size();
                    double vehiclePosition = segmentOffset + length;
                    // draw vehicles beginning with the leader at the end of the segment
                    double latOff = 0.;
                    for (int i = 0; i < queueSize; ++i) {
                        const GUIMEVehicle* const veh = static_cast<GUIMEVehicle*>(queue[queueSize - i - 1]);
                        const double intendedLeave = MIN2(veh->getEventTimeSeconds(), veh->getBlockTimeSeconds());
                        const double entry = veh->getLastEntryTimeSeconds();
                        const double relPos = segmentOffset + length * (now - entry) / (intendedLeave - entry);
                        if (relPos < vehiclePosition) {
                            vehiclePosition = relPos;
                        }
                        while (vehiclePosition < segmentOffset) {
                            // if there is only a single queue for a
                            // multi-lane edge shift vehicles and start
                            // drawing again from the end of the segment
                            vehiclePosition += length;
                            latOff += 0.2;
                        }
                        /// @fixme use correct shape for geometryPositionAtOffset
                        const Position p = l->geometryPositionAtOffset(vehiclePosition, latOff);
                        const double angle = l->getShape(s.secondaryShape).rotationAtOffset(l->interpolateLanePosToGeometryPos(vehiclePosition));
                        veh->drawOnPos(s, p, angle);
                        vehiclePosition -= veh->getVehicleType().getLengthWithGap();
                    }
                }
                segmentOffset += length;
            }
            GLHelper::popMatrix();
        }
        vehicleControl->releaseVehicles();
    }
}



double
GUIEdge::getAllowedSpeed() const {
    return (*myLanes)[0]->getSpeedLimit();
}


double
GUIEdge::getRelativeSpeed() const {
    return getMeanSpeed() / getAllowedSpeed();
}


void
GUIEdge::setColor(const GUIVisualizationSettings& s) const {
    myMesoColor = RGBColor(0, 0, 0); // default background color when using multiColor
    const GUIColorer& c = s.edgeColorer;
    if (!setFunctionalColor(c) && !setMultiColor(c)) {
        myMesoColor = c.getScheme().getColor(getColorValue(s, c.getActive()));
    }
}


bool
GUIEdge::setFunctionalColor(const GUIColorer& c) const {
    const int activeScheme = c.getActive();
    int activeMicroScheme = -1;
    switch (activeScheme) {
        case 0:
            activeMicroScheme = 0; // color uniform
            break;
        case 9:
            activeMicroScheme = 18; // color by angle
            break;
        case 17:
            activeMicroScheme = 30; // color by TAZ
            break;
        default:
            return false;
    }
    GUILane* guiLane = static_cast<GUILane*>(getLanes()[0]);
    return guiLane->setFunctionalColor(c, myMesoColor, activeMicroScheme);
}


bool
GUIEdge::setMultiColor(const GUIColorer& c) const {
    const int activeScheme = c.getActive();
    mySegmentColors.clear();
    switch (activeScheme) {
        case 10: // alternating segments
            for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this);
                    segment != nullptr; segment = segment->getNextSegment()) {
                mySegmentColors.push_back(c.getScheme().getColor(segment->getIndex() % 2));
            }
            //std::cout << getID() << " scheme=" << c.getScheme().getName() << " schemeCols=" << c.getScheme().getColors().size() << " thresh=" << toString(c.getScheme().getThresholds()) << " segmentColors=" << mySegmentColors.size() << " [0]=" << mySegmentColors[0] << " [1]=" << mySegmentColors[1] <<  "\n";
            return true;
        case 11: // by segment jammed state
            for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this);
                    segment != nullptr; segment = segment->getNextSegment()) {
                mySegmentColors.push_back(
                    c.getScheme().getColor(segment->getRelativeOccupancy() > segment->getRelativeJamThreshold() ? 2 :
                                           (segment->getRelativeOccupancy() * 2 < segment->getRelativeJamThreshold() ? 0 : 1)));
            }
            return true;
        case 12: // by segment occupancy
            for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this);
                    segment != nullptr; segment = segment->getNextSegment()) {
                mySegmentColors.push_back(c.getScheme().getColor(segment->getRelativeOccupancy()));
            }
            return true;
        case 13: // by segment speed
            for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this);
                    segment != nullptr; segment = segment->getNextSegment()) {
                mySegmentColors.push_back(c.getScheme().getColor(segment->getMeanSpeed()));
            }
            return true;
        case 14: // by segment flow
            for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this);
                    segment != nullptr; segment = segment->getNextSegment()) {
                mySegmentColors.push_back(c.getScheme().getColor(3600 * segment->getCarNumber() * segment->getMeanSpeed() / segment->getLength()));
            }
            return true;
        case 15: // by segment relative speed
            for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this);
                    segment != nullptr; segment = segment->getNextSegment()) {
                mySegmentColors.push_back(c.getScheme().getColor(segment->getMeanSpeed() / getAllowedSpeed()));
            }
            return true;
        default:
            return false;
    }
}


double
GUIEdge::getColorValue(const GUIVisualizationSettings& s, int activeScheme) const {
    switch (activeScheme) {
        case 1:
            return gSelected.isSelected(getType(), getGlID());
        case 2:
            return (double)getFunction();
        case 3:
            return getAllowedSpeed();
        case 4:
            return getBruttoOccupancy();
        case 5:
            return getMeanSpeed();
        case 6:
            return getFlow();
        case 7:
            return getRelativeSpeed();
        case 8:
            return getRoutingSpeed();
        case 16:
            return getPendingEmits();
        case 18:
            // by numerical edge param value
            try {
                return StringUtils::toDouble(getParameter(s.edgeParam, "0"));
            } catch (NumberFormatException&) {
                try {
                    return StringUtils::toBool(getParameter(s.edgeParam, "0"));
                } catch (BoolFormatException&) {
                    return -1;
                }
            }
        case 19:
            // by edge data value
            return GUINet::getGUIInstance()->getEdgeData(this, s.edgeData);
    }
    return 0;
}


double
GUIEdge::getScaleValue(const GUIVisualizationSettings& s, int activeScheme) const {
    switch (activeScheme) {
        case 1:
            return gSelected.isSelected(getType(), getGlID());
        case 2:
            return getAllowedSpeed();
        case 3:
            return getBruttoOccupancy();
        case 4:
            return getMeanSpeed();
        case 5:
            return getFlow();
        case 6:
            return getRelativeSpeed();
        case 7:
            return getPendingEmits();
        case 8:
            // by edge data value
            return GUINet::getGUIInstance()->getEdgeData(this, s.edgeDataScaling);
    }
    return 0;
}


MESegment*
GUIEdge::getSegmentAtPosition(const Position& pos) {
    const PositionVector& shape = getLanes()[0]->getShape();
    const double lanePos = shape.nearest_offset_to_point2D(pos);
    return MSGlobals::gMesoNet->getSegmentForEdge(*this, lanePos);
}



void
GUIEdge::closeTraffic(const GUILane* lane) {
    const std::vector<MSLane*>& lanes = getLanes();
    const bool isClosed = lane->isClosed();
    for (std::vector<MSLane*>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
        GUILane* l = dynamic_cast<GUILane*>(*i);
        if (l->isClosed() == isClosed) {
            l->closeTraffic(false);
        }
    }
    rebuildAllowedLanes();
}


void
GUIEdge::addRerouter() {
    MSEdgeVector edges;
    edges.push_back(this);
    GUITriggeredRerouter* rr = new GUITriggeredRerouter(getID() + "_dynamic_rerouter", edges, 1, false, false, 0, "", Position::INVALID,
            GUINet::getGUIInstance()->getVisualisationSpeedUp());

    MSTriggeredRerouter::RerouteInterval ri;
    ri.begin = MSNet::getInstance()->getCurrentTimeStep();
    ri.end = SUMOTime_MAX;
    ri.edgeProbs.add(&MSTriggeredRerouter::mySpecialDest_keepDestination, 1.);
    rr->myIntervals.push_back(ri);

    // trigger rerouting for vehicles already on this edge
    const std::vector<MSLane*>& lanes = getLanes();
    for (std::vector<MSLane*>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
        const MSLane::VehCont& vehicles = (*i)->getVehiclesSecure();
        for (MSLane::VehCont::const_iterator v = vehicles.begin(); v != vehicles.end(); ++v) {
            if ((*v)->getLane() == (*i)) {
                rr->notifyEnter(**v, MSMoveReminder::NOTIFICATION_JUNCTION);
            } // else: this is the shadow during a continuous lane change
        }
        (*i)->releaseVehicles();
    }
}


bool
GUIEdge::isSelected() const {
    return gSelected.isSelected(GLO_EDGE, getGlID());
}

double
GUIEdge::getPendingEmits() const {
    return MSNet::getInstance()->getInsertionControl().getPendingEmits(getLanes()[0]);
}

double
GUIEdge::getClickPriority() const {
    if (!MSGlobals::gUseMesoSim) {
        // do not select edgse in meso mode
        return INVALID_PRIORITY;
    }
    return GLO_EDGE;
}
/****************************************************************************/
