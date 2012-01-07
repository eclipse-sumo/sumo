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
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLaneChanger.h>
#include <microsim/MSGlobals.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/geom/GeomHelper.h>
#include "GUIEdge.h"
#include "GUINet.h"
#include "GUILane.h"
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <foreign/polyfonts/polyfonts.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#include <microsim/MSGlobals.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// included modules
// ===========================================================================
GUIEdge::GUIEdge(const std::string& id, unsigned int numericalID, const std::string& streetName)
    : MSEdge(id, numericalID, streetName),
      GUIGlObject(GLO_EDGE, id) {}


GUIEdge::~GUIEdge() {
    for (LaneWrapperVector::iterator i = myLaneGeoms.begin(); i != myLaneGeoms.end(); ++i) {
        delete(*i);
    }
}


void
GUIEdge::initGeometry() {
    // don't do this twice
    if (myLaneGeoms.size() > 0) {
        return;
    }
    // build the lane wrapper
    myLaneGeoms.reserve(myLanes->size());
    for (unsigned int i = 0; i < myLanes->size(); i++) {
        myLaneGeoms.push_back(myLanes->at(i)->buildLaneWrapper(i));
    }
}


MSLane&
GUIEdge::getLane(size_t laneNo) {
    assert(laneNo < myLanes->size());
    return *((*myLanes)[laneNo]);
}


GUILaneWrapper&
GUIEdge::getLaneGeometry(size_t laneNo) const {
    assert(laneNo < myLanes->size());
    return *(myLaneGeoms[laneNo]);
}


GUILaneWrapper&
GUIEdge::getLaneGeometry(const MSLane* lane) const {
    LaneWrapperVector::const_iterator i =
        find_if(myLaneGeoms.begin(), myLaneGeoms.end(), lane_wrapper_finder(*lane));
    assert(i != myLaneGeoms.end());
    return *(*i);
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
    for (LaneWrapperVector::const_iterator i = myLaneGeoms.begin(); i != myLaneGeoms.end(); ++i) {
        const PositionVector& g = (*i)->getShape();
        for (unsigned int j = 0; j < g.size(); j++) {
            ret.add(g[j]);
        }
    }
    ret.grow(10);
    return ret;
}


void
GUIEdge::fill(std::vector<GUIEdge*> &netsWrappers) {
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
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        buildShowParamsPopupEntry(ret);
    }
#endif
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUIEdge::getParameterWindow(GUIMainWindow& app,
                            GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = 0;
#ifdef HAVE_MESOSIM
    ret = new GUIParameterTableWindow(app, *this, 5);
    // add items
    ret->mkItem("length [m]", false, (*myLanes)[0]->getLength());
    ret->mkItem("allowed speed [m/s]", false, getAllowedSpeed());
    ret->mkItem("occupancy [%]", true,
                new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getOccupancy));
    ret->mkItem("mean vehicle speed [m/s]", true,
                new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getMeanSpeed));
    ret->mkItem("flow [veh/h/lane]", true,
                new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getFlow));
    ret->mkItem("#vehicles", true,
                new CastingFunctionBinding<GUIEdge, SUMOReal, unsigned int>(this, &GUIEdge::getVehicleNo));
    // close building
    ret->closeBuilding();
#endif
    UNUSED_PARAMETER(&app);
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
    // draw the lanes
    for (LaneWrapperVector::const_iterator i = myLaneGeoms.begin(); i != myLaneGeoms.end(); ++i) {
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            setColor(s);
        }
#endif
        (*i)->drawGL(s);
    }
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        size_t idx = 0;
        for (LaneWrapperVector::const_iterator l = myLaneGeoms.begin(); l != myLaneGeoms.end(); ++l, ++idx) {
            const PositionVector& shape = (*l)->getShape();
            const DoubleVector& shapeRotations = (*l)->getShapeRotations();
            const DoubleVector& shapeLengths = (*l)->getShapeLengths();
            const Position& laneBeg = shape[0];

            glColor3d(1, 1, 0);
            glPushMatrix();
            glTranslated(laneBeg.x(), laneBeg.y(), 0);
            glRotated(shapeRotations[0], 0, 0, 1);
            // go through the vehicles
            int shapePos = 0;
            SUMOReal positionOffset = 0;
            SUMOReal position = 0;
            for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment != 0; segment = segment->getNextSegment()) {
                const std::vector<size_t> numCars = segment->getQueSizes();
                const SUMOReal length = segment->getLength();
                if (idx < numCars.size()) {
                    const SUMOReal avgCarSize = segment->getOccupancy() / segment->getCarNumber();
                    for (size_t i = 0; i < numCars[idx]; i++) {
                        SUMOReal vehiclePosition = position + length - i * avgCarSize;
                        SUMOReal xOff = 0.f;
                        while (vehiclePosition < position) {
                            vehiclePosition += length;
                            xOff += 0.5f;
                        }
                        while (shapePos < (int)shapeRotations.size() - 1 && vehiclePosition > positionOffset + shapeLengths[shapePos]) {
                            glPopMatrix();
                            positionOffset += shapeLengths[shapePos];
                            shapePos++;
                            glPushMatrix();
                            glTranslated(shape[shapePos].x(), shape[shapePos].y(), 0);
                            glRotated(shapeRotations[shapePos], 0, 0, 1);
                        }
                        glPushMatrix();
                        glTranslated(xOff, -(vehiclePosition - positionOffset), 0);
                        glPushMatrix();
                        glScaled(1, avgCarSize, 1);
                        glBegin(GL_TRIANGLES);
                        glVertex2d(0, 0);
                        glVertex2d(0 - 1.25, 1);
                        glVertex2d(0 + 1.25, 1);
                        glEnd();
                        glPopMatrix();
                        glPopMatrix();
                    }
                }
                position += length;
            }
            glPopMatrix();
        }
    }
#endif
    // (optionally) draw the name and/or the street name
    const bool drawEdgeName = s.edgeName.show && myFunction == EDGEFUNCTION_NORMAL;
    const bool drawInternalEdgeName = s.internalEdgeName.show && myFunction != EDGEFUNCTION_NORMAL;
    const bool drawStreetName = s.streetName.show && myStreetName != "";
    if (drawEdgeName || drawInternalEdgeName || drawStreetName) {
        GUILaneWrapper* lane1 = myLaneGeoms[0];
        GUILaneWrapper* lane2 = myLaneGeoms[myLaneGeoms.size() - 1];
        Position p = lane1->getShape().positionAtLengthPosition(lane1->getShape().length() / (SUMOReal) 2.);
        p.add(lane2->getShape().positionAtLengthPosition(lane2->getShape().length() / (SUMOReal) 2.));
        p.mul(.5);
        SUMOReal angle = lane1->getShape().rotationDegreeAtLengthPosition(lane1->getShape().length() / (SUMOReal) 2.);
        angle += 90;
        if (angle > 90 && angle < 270) {
            angle -= 180;
        }
        if (drawEdgeName) {
            drawName(p, s.scale, s.edgeName, angle);
        } else if (drawInternalEdgeName) {
            drawName(p, s.scale, s.internalEdgeName, angle);
        }
        if (drawStreetName) {
            GLHelper::drawText(getStreetName(), p, GLO_MAX,
                               s.streetName.size / s.scale, s.streetName.color, angle);
        }
    }
}

#ifdef HAVE_MESOSIM
unsigned int
GUIEdge::getVehicleNo() const {
    size_t vehNo = 0;
    for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment != 0; segment = segment->getNextSegment()) {
        vehNo += segment->getCarNumber();
    }
    return (unsigned int)vehNo;
}


SUMOReal
GUIEdge::getFlow() const {
    SUMOReal flow = 0;
    for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment != 0; segment = segment->getNextSegment()) {
        flow += (SUMOReal) segment->getCarNumber() * segment->getMeanSpeed();
    }
    return flow * (SUMOReal) 1000. / (*myLanes)[0]->getLength() / (SUMOReal) 3.6;
}


SUMOReal
GUIEdge::getOccupancy() const {
    SUMOReal occ = 0;
    for (MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment != 0; segment = segment->getNextSegment()) {
        occ += segment->getOccupancy();
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
    return v / no;
}


SUMOReal
GUIEdge::getAllowedSpeed() const {
    return (*myLanes)[0]->getMaxSpeed();
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
            return getOccupancy();
        case 5:
            return getMeanSpeed();
        case 6:
            return getFlow();
    }
    return 0;
}

#endif

/****************************************************************************/

