/****************************************************************************/
/// @file    GUIEdge.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Sept 2002
/// @version $Id$
///
// A road/street connecting two junctions (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/geom/GeomHelper.h>
#include <utils/foxtools/MFXMutex.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <microsim/MSBaseVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLaneChanger.h>
#include <microsim/MSGlobals.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include "GUIEdge.h"
#include "GUINet.h"
#include "GUILane.h"
#include "GUIPerson.h"

#ifdef HAVE_INTERNAL
#include <mesogui/GUIMEVehicleControl.h>
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#include <mesosim/MEVehicle.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// included modules
// ===========================================================================
GUIEdge::GUIEdge(const std::string& id, int numericalID,
                 const EdgeBasicFunction function, const std::string& streetName, const std::string& edgeType)
    : MSEdge(id, numericalID, function, streetName, edgeType),
      GUIGlObject(GLO_EDGE, id) {}


GUIEdge::~GUIEdge() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


MSLane&
GUIEdge::getLane(size_t laneNo) {
    assert(laneNo < myLanes->size());
    return *((*myLanes)[laneNo]);
}


std::vector<GUIGlID>
GUIEdge::getIDs(bool includeInternal) {
    std::vector<GUIGlID> ret;
    ret.reserve(MSEdge::myDict.size());
    for (MSEdge::DictType::iterator i = MSEdge::myDict.begin(); i != MSEdge::myDict.end(); ++i) {
        GUIEdge* edge = dynamic_cast<GUIEdge*>(i->second);
        assert(edge);
        if (edge->getPurpose() != EDGEFUNCTION_INTERNAL || includeInternal) {
            ret.push_back(edge->getGlID());
        }
    }
    return ret;
}


Boundary
GUIEdge::getBoundary() const {
    Boundary ret;
    for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
        ret.add((*i)->getShape().getBoxBoundary());
    }
    ret.grow(10);
    return ret;
}


void
GUIEdge::fill(std::vector<GUIEdge*>& netsWrappers) {
    size_t size = MSEdge::dictSize();
    netsWrappers.reserve(size);
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); ++i) {
        if (i->second->getPurpose() != MSEdge::EDGEFUNCTION_DISTRICT) {
            netsWrappers.push_back(static_cast<GUIEdge*>((*i).second));
        }
    }
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
    }
    const SUMOReal pos = getLanes()[0]->getShape().nearest_offset_to_point2D(parent.getPositionInformation());
    new FXMenuCommand(ret, ("pos: " + toString(pos)).c_str(), 0, 0, 0);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUIEdge::getParameterWindow(GUIMainWindow& app,
                            GUISUMOAbstractView& parent) {
    GUIParameterTableWindow* ret = 0;
#ifdef HAVE_INTERNAL
    ret = new GUIParameterTableWindow(app, *this, 16);
    // add edge items
    ret->mkItem("length [m]", false, (*myLanes)[0]->getLength());
    ret->mkItem("allowed speed [m/s]", false, getAllowedSpeed());
    ret->mkItem("occupancy [%]", true, new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getBruttoOccupancy));
    ret->mkItem("mean vehicle speed [m/s]", true, new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getMeanSpeed));
    ret->mkItem("flow [veh/h/lane]", true, new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getFlow));
    ret->mkItem("#vehicles", true, new CastingFunctionBinding<GUIEdge, SUMOReal, unsigned int>(this, &GUIEdge::getVehicleNo));
    ret->mkItem("vehicle ids", false, getVehicleIDs());
    // add segment items
    MESegment* segment = getSegmentAtPosition(parent.getPositionInformation());
    ret->mkItem("segment index", false, segment->getIndex());
    ret->mkItem("segment length [m]", false, segment->getLength());
    ret->mkItem("segment allowed speed [m/s]", false, segment->getMaxSpeed());
    ret->mkItem("segment jam threshold [%]", false, segment->getRelativeJamThreshold());
    ret->mkItem("segment occupancy [%]", true, new FunctionBinding<MESegment, SUMOReal>(segment, &MESegment::getRelativeOccupancy));
    ret->mkItem("segment mean vehicle speed [m/s]", true, new FunctionBinding<MESegment, SUMOReal>(segment, &MESegment::getMeanSpeed));
    ret->mkItem("segment flow [veh/h/lane]", true, new FunctionBinding<MESegment, SUMOReal>(segment, &MESegment::getFlow));
    ret->mkItem("segment #vehicles", true, new CastingFunctionBinding<MESegment, SUMOReal, size_t>(segment, &MESegment::getCarNumber));
    ret->mkItem("segment leader leave time", true, new FunctionBinding<MESegment, SUMOReal>(segment, &MESegment::getEventTimeSeconds));

    // close building
    ret->closeBuilding();
#else
    UNUSED_PARAMETER(app);
    UNUSED_PARAMETER(parent);
#endif
    return ret;
}


Boundary
GUIEdge::getCenteringBoundary() const {
    Boundary b = getBoundary();
    b.grow(20);
    return b;
}


void
GUIEdge::drawGL(const GUIVisualizationSettings& s) const {
    if (s.hideConnectors && myFunction == MSEdge::EDGEFUNCTION_CONNECTOR) {
        return;
    }
    if (MSGlobals::gUseMesoSim) {
        glPushName(getGlID());
    }
    // draw the lanes
    for (std::vector<MSLane*>::const_iterator i = myLanes->begin(); i != myLanes->end(); ++i) {
#ifdef HAVE_INTERNAL
        if (MSGlobals::gUseMesoSim) {
            setColor(s);
        }
#endif
        GUILane* l = dynamic_cast<GUILane*>(*i);
        if (l != 0) {
            l->drawGL(s);
        }
    }
#ifdef HAVE_INTERNAL
    if (MSGlobals::gUseMesoSim) {
        const GUIVisualizationTextSettings& nameSettings = s.vehicleName;
        GUIMEVehicleControl* vehicleControl = GUINet::getGUIInstance()->getGUIMEVehicleControl();
        if (vehicleControl != 0) {
            // draw the meso vehicles
            vehicleControl->secureVehicles();
            size_t laneIndex = 0;
            MESegment::Queue queue;
            for (std::vector<MSLane*>::const_iterator msl = myLanes->begin(); msl != myLanes->end(); ++msl, ++laneIndex) {
                GUILane* l = static_cast<GUILane*>(*msl);
                const PositionVector& shape = l->getShape();
                const std::vector<SUMOReal>& shapeRotations = l->getShapeRotations();
                const std::vector<SUMOReal>& shapeLengths = l->getShapeLengths();
                const Position& laneBeg = shape[0];
                glPushMatrix();
                glTranslated(laneBeg.x(), laneBeg.y(), 0);
                glRotated(shapeRotations[0], 0, 0, 1);
                // go through the vehicles
                int shapeIndex = 0;
                SUMOReal shapeOffset = 0; // ofset at start of current shape
                SUMOReal segmentOffset = 0; // offset at start of current segment
                for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this);
                        segment != 0; segment = segment->getNextSegment()) {
                    const SUMOReal length = segment->getLength();
                    if (laneIndex < segment->numQueues()) {
                        // make a copy so we don't have to worry about synchronization
                        queue = segment->getQueue(laneIndex);
                        const SUMOReal avgCarSize = segment->getBruttoOccupancy() / segment->getCarNumber();
                        const size_t queueSize = queue.size();
                        for (size_t i = 0; i < queueSize; ++i) {
                            MSBaseVehicle* veh = queue[queueSize - i - 1];
                            setVehicleColor(s, veh);
                            SUMOReal vehiclePosition = segmentOffset + length - i * avgCarSize;
                            SUMOReal xOff = 0.f;
                            while (vehiclePosition < segmentOffset) {
                                // if there is only a single queue for a
                                // multi-lane edge shift vehicles and start
                                // drawing again from the end of the segment
                                vehiclePosition += length;
                                xOff += 0.5f;
                            }
                            while (shapeIndex < (int)shapeRotations.size() - 1 && vehiclePosition > shapeOffset + shapeLengths[shapeIndex]) {
                                glPopMatrix();
                                shapeOffset += shapeLengths[shapeIndex];
                                shapeIndex++;
                                glPushMatrix();
                                glTranslated(shape[shapeIndex].x(), shape[shapeIndex].y(), 0);
                                glRotated(shapeRotations[shapeIndex], 0, 0, 1);
                            }
                            glPushMatrix();
                            glTranslated(xOff, -(vehiclePosition - shapeOffset), GLO_VEHICLE);
                            glPushMatrix();
                            glScaled(1, avgCarSize, 1);
                            glBegin(GL_TRIANGLES);
                            glVertex2d(0, 0);
                            glVertex2d(0 - 1.25, 1);
                            glVertex2d(0 + 1.25, 1);
                            glEnd();
                            glPopMatrix();
                            glPopMatrix();
                            if (nameSettings.show) {
                                GLHelper::drawText(veh->getID(),
                                                   Position(xOff, -(vehiclePosition - shapeOffset)),
                                                   GLO_MAX, nameSettings.size / s.scale, nameSettings.color, 0);
                            }
                        }
                    }
                    segmentOffset += length;
                }
                glPopMatrix();
            }
            vehicleControl->releaseVehicles();
        }
        glPopName();
    }
#endif
    // (optionally) draw the name and/or the street name
    const bool drawEdgeName = s.edgeName.show && myFunction == EDGEFUNCTION_NORMAL;
    const bool drawInternalEdgeName = s.internalEdgeName.show && myFunction == EDGEFUNCTION_INTERNAL;
    const bool drawCwaEdgeName = s.cwaEdgeName.show && (myFunction == EDGEFUNCTION_CROSSING || myFunction == EDGEFUNCTION_WALKINGAREA);
    const bool drawStreetName = s.streetName.show && myStreetName != "";
    if (drawEdgeName || drawInternalEdgeName || drawCwaEdgeName || drawStreetName) {
        GUILane* lane1 = dynamic_cast<GUILane*>((*myLanes)[0]);
        GUILane* lane2 = dynamic_cast<GUILane*>((*myLanes).back());
        if (lane1 != 0 && lane2 != 0) {
            Position p = lane1->getShape().positionAtOffset(lane1->getShape().length() / (SUMOReal) 2.);
            p.add(lane2->getShape().positionAtOffset(lane2->getShape().length() / (SUMOReal) 2.));
            p.mul(.5);
            SUMOReal angle = lane1->getShape().rotationDegreeAtOffset(lane1->getShape().length() / (SUMOReal) 2.);
            angle += 90;
            if (angle > 90 && angle < 270) {
                angle -= 180;
            }
            if (drawEdgeName) {
                drawName(p, s.scale, s.edgeName, angle);
            } else if (drawInternalEdgeName) {
                drawName(p, s.scale, s.internalEdgeName, angle);
            } else if (drawCwaEdgeName) {
                drawName(p, s.scale, s.cwaEdgeName, angle);
            }
            if (drawStreetName) {
                GLHelper::drawText(getStreetName(), p, GLO_MAX,
                                   s.streetName.size / s.scale, s.streetName.color, angle);
            }
        }
    }
    if (s.scale > s.minPersonSize) {
        myLock.lock();
        for (std::set<MSPerson*>::const_iterator i = myPersons.begin(); i != myPersons.end(); ++i) {
            GUIPerson* person = dynamic_cast<GUIPerson*>(*i);
            assert(person != 0);
            person->drawGL(s);
        }
        myLock.unlock();
    }
}

#ifdef HAVE_INTERNAL
unsigned int
GUIEdge::getVehicleNo() const {
    size_t vehNo = 0;
    for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment != 0; segment = segment->getNextSegment()) {
        vehNo += segment->getCarNumber();
    }
    return (unsigned int)vehNo;
}


std::string
GUIEdge::getVehicleIDs() const {
    std::string result = " ";
    std::vector<const MEVehicle*> vehs;
    for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment != 0; segment = segment->getNextSegment()) {
        std::vector<const MEVehicle*> segmentVehs = segment->getVehicles();
        vehs.insert(vehs.end(), segmentVehs.begin(), segmentVehs.end());
    }
    for (std::vector<const MEVehicle*>::const_iterator it = vehs.begin(); it != vehs.end(); it++) {
        result += (*it)->getID() + " ";
    }
    return result;
}


SUMOReal
GUIEdge::getFlow() const {
    SUMOReal flow = 0;
    for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment != 0; segment = segment->getNextSegment()) {
        flow += (SUMOReal) segment->getCarNumber() * segment->getMeanSpeed();
    }
    return 3600 * flow / (*myLanes)[0]->getLength();
}


SUMOReal
GUIEdge::getBruttoOccupancy() const {
    SUMOReal occ = 0;
    for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment != 0; segment = segment->getNextSegment()) {
        occ += segment->getBruttoOccupancy();
    }
    return occ / (*myLanes)[0]->getLength() / (SUMOReal)(myLanes->size());
}


SUMOReal
GUIEdge::getMeanSpeed() const {
    SUMOReal v = 0;
    SUMOReal no = 0;
    for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment != 0; segment = segment->getNextSegment()) {
        SUMOReal vehNo = (SUMOReal) segment->getCarNumber();
        v += vehNo * segment->getMeanSpeed();
        no += vehNo;
    }
    if (no == 0) {
        return getSpeedLimit();
    }
    return v / no;
}


SUMOReal
GUIEdge::getAllowedSpeed() const {
    return (*myLanes)[0]->getSpeedLimit();
}


SUMOReal
GUIEdge::getRelativeSpeed() const {
    return getMeanSpeed() / getAllowedSpeed();
}


void
GUIEdge::setColor(const GUIVisualizationSettings& s) const {
    GLHelper::setColor(s.edgeColorer.getScheme().getColor(getColorValue(s.edgeColorer.getActive())));
}


SUMOReal
GUIEdge::getColorValue(size_t activeScheme) const {
    switch (activeScheme) {
        case 1:
            return gSelected.isSelected(getType(), getGlID());
        case 2:
            return getPurpose();
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
    }
    return 0;
}


MESegment*
GUIEdge::getSegmentAtPosition(const Position& pos) {
    const PositionVector& shape = getLanes()[0]->getShape();
    const SUMOReal lanePos = shape.nearest_offset_to_point2D(pos);
    return MSGlobals::gMesoNet->getSegmentForEdge(*this, lanePos);
}


void
GUIEdge::setVehicleColor(const GUIVisualizationSettings& s, MSBaseVehicle* veh) const {
    const GUIColorer& c = s.vehicleColorer;
    switch (c.getActive()) {
        case 0:
            if (veh->getParameter().wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(veh->getParameter().color);
            }
            if (veh->getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(veh->getVehicleType().getColor());
            }
            if (veh->getRoute().getColor() != RGBColor::DEFAULT_COLOR) {
                GLHelper::setColor(veh->getRoute().getColor());
            }
            break;
        case 2:
            GLHelper::setColor(veh->getParameter().color);
            break;
        case 4:
            GLHelper::setColor(veh->getVehicleType().getColor());
            break;
        case 5:
            GLHelper::setColor(veh->getRoute().getColor());
            break;
        default:
            GLHelper::setColor(c.getScheme().getColor(0));
    }
}

#endif

/****************************************************************************/

