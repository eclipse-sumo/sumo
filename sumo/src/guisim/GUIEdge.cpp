/****************************************************************************/
/// @file    GUIEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A road/street connecting two junctions (gui-version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLaneChanger.h>
#include <microsim/MSGlobals.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
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
GUIEdge::GUIEdge(const std::string &id, unsigned int numericalID,
                 GUIGlObjectStorage &idStorage) throw()
        : MSEdge(id, numericalID),
        GUIGlObject(idStorage, "edge:" + id) {}


GUIEdge::~GUIEdge() throw() {
    for (LaneWrapperVector::iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end(); ++i) {
        delete(*i);
    }
}


void
GUIEdge::initGeometry(GUIGlObjectStorage &idStorage) throw() {
    // don't do this twice
    if (myLaneGeoms.size()>0) {
        return;
    }
    // build the lane wrapper
    myLaneGeoms.reserve(myLanes->size());
    for (std::vector<MSLane*>::const_iterator i=myLanes->begin(); i<myLanes->end(); ++i) {
        myLaneGeoms.push_back((*i)->buildLaneWrapper(idStorage));
    }
}


MSLane &
GUIEdge::getLane(size_t laneNo) {
    assert(laneNo<myLanes->size());
    return *((*myLanes)[laneNo]);
}


GUILaneWrapper &
GUIEdge::getLaneGeometry(size_t laneNo) const {
    assert(laneNo<myLanes->size());
    return *(myLaneGeoms[laneNo]);
}


GUILaneWrapper &
GUIEdge::getLaneGeometry(const MSLane *lane) const {
    LaneWrapperVector::const_iterator i=
        find_if(myLaneGeoms.begin(), myLaneGeoms.end(), lane_wrapper_finder(*lane));
    assert(i!=myLaneGeoms.end());
    return *(*i);
}


std::vector<GLuint>
GUIEdge::getIDs() {
    std::vector<GLuint> ret;
    ret.reserve(MSEdge::myDict.size());
    for (MSEdge::DictType::iterator i=MSEdge::myDict.begin(); i!=MSEdge::myDict.end(); ++i) {
        ret.push_back(static_cast<GUIEdge*>((*i).second)->getGlID());
    }
    return ret;
}


Boundary
GUIEdge::getBoundary() const {
    Boundary ret;
    for (LaneWrapperVector::const_iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end(); ++i) {
        const Position2DVector &g = (*i)->getShape();
        for (unsigned int j=0; j<g.size(); j++) {
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
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); ++i) {
        if (i->second->getPurpose() != MSEdge::EDGEFUNCTION_DISTRICT) {
            netsWrappers.push_back(static_cast<GUIEdge*>((*i).second));
        }
    }
}



GUIGLObjectPopupMenu *
GUIEdge::getPopUpMenu(GUIMainWindow &app, GUISUMOAbstractView &parent) throw() {
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
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


GUIParameterTableWindow *
GUIEdge::getParameterWindow(GUIMainWindow &app,
                            GUISUMOAbstractView &) throw() {
    GUIParameterTableWindow *ret = 0;
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


const std::string &
GUIEdge::getMicrosimID() const throw() {
    return getID();
}


Boundary
GUIEdge::getCenteringBoundary() const throw() {
    Boundary b = getBoundary();
    b.grow(20);
    return b;
}


void
GUIEdge::drawGL(const GUIVisualizationSettings &s) const throw() {
    if (s.hideConnectors&&myFunction==MSEdge::EDGEFUNCTION_CONNECTOR) {
        return;
    }
    // draw the lanes
    for (LaneWrapperVector::const_iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end(); ++i) {
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            s.edgeColorer.setGlColor(*this);
        }
#endif
        (*i)->drawGL(s);
    }
    // check whether lane boundaries shall be drawn
    if (s.scale>1.&&s.laneShowBorders&&myFunction!=MSEdge::EDGEFUNCTION_INTERNAL) {
        glTranslated(0, 0, .01);
        for (LaneWrapperVector::const_iterator i=myLaneGeoms.begin()+1; i!=myLaneGeoms.end(); ++i) {
            (*i)->drawBordersGL(s);
        }
        glTranslated(0, 0, -.01);
        // draw white boundings
        glTranslated(0, 0, .02);
        glColor3d(1,1,1);
        for (LaneWrapperVector::const_iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end(); ++i) {
            GLHelper::drawBoxLines((*i)->getShape(), (*i)->getShapeRotations(), (*i)->getShapeLengths(), SUMO_const_halfLaneAndOffset);
        }
        glTranslated(0, 0, -.02);
    }
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        size_t idx = 0;
        for (LaneWrapperVector::const_iterator l=myLaneGeoms.begin(); l!=myLaneGeoms.end(); ++l,++idx) {
            const Position2DVector& shape = (*l)->getShape();
            const DoubleVector& shapeRotations = (*l)->getShapeRotations();
            const DoubleVector& shapeLengths = (*l)->getShapeLengths();
            const Position2D &laneBeg = shape[0];

            glColor3d(1,1,0);
            glPushMatrix();
            glTranslated(laneBeg.x(), laneBeg.y(), 0);
            glRotated(shapeRotations[0], 0, 0, 1);
            // go through the vehicles
            int shapePos = 0;
            SUMOReal positionOffset = 0;
            SUMOReal position = 0;
            for (MESegment *segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment!=0; segment = segment->getNextSegment()) {
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
                        while (shapePos<(int)shapeRotations.size()-1 && vehiclePosition>positionOffset+shapeLengths[shapePos]) {
                            glPopMatrix();
                            positionOffset += shapeLengths[shapePos];
                            shapePos++;
                            glPushMatrix();
                            glTranslated(shape[shapePos].x(), shape[shapePos].y(), 0);
                            glRotated(shapeRotations[shapePos], 0, 0, 1);
                        }
                        glPushMatrix();
                        glTranslated(xOff, -(vehiclePosition-positionOffset), 0);
                        glPushMatrix();
                        glScaled(1, avgCarSize, 1);
                        glBegin(GL_TRIANGLES);
                        glVertex2d(0, 0);
                        glVertex2d(0-1.25, 1);
                        glVertex2d(0+1.25, 1);
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
    // (optionally) draw the name
    if ((s.drawEdgeName && myFunction == EDGEFUNCTION_NORMAL) || (s.drawInternalEdgeName && myFunction != EDGEFUNCTION_NORMAL)) {
        float nameSize = s.edgeNameSize;
        if (myFunction == EDGEFUNCTION_NORMAL) {
            glColor3d(s.edgeNameColor.red(), s.edgeNameColor.green(), s.edgeNameColor.blue());
        } else {
            glColor3d(s.internalEdgeNameColor.red(), s.internalEdgeNameColor.green(), s.internalEdgeNameColor.blue());
            nameSize = s.internalEdgeNameSize;
        }
        GUILaneWrapper *lane1 = myLaneGeoms[0];
        GUILaneWrapper *lane2 = myLaneGeoms[myLaneGeoms.size()-1];
        glPushMatrix();
        glTranslated(0, 0, -.06);
        Position2D p = lane1->getShape().positionAtLengthPosition(lane1->getShape().length()/(SUMOReal) 2.);
        p.add(lane2->getShape().positionAtLengthPosition(lane2->getShape().length()/(SUMOReal) 2.));
        p.mul(.5);
        glTranslated(p.x(), p.y(), 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(nameSize / s.scale);
        SUMOReal w = pfdkGetStringWidth(getMicrosimID().c_str());
        glRotated(180, 1, 0, 0);
        SUMOReal angle = lane1->getShape().rotationDegreeAtLengthPosition(lane1->getShape().length()/(SUMOReal) 2.);
        angle += 90;
        if (angle>90&&angle<270) {
            angle -= 180;
        }
        glRotated(angle, 0, 0, 1);
        glTranslated(-w/2., .2*nameSize / s.scale, 0);
        pfDrawString(getMicrosimID().c_str());
        glTranslated(0, 0, .06);
        glPopMatrix();
    }
}

#ifdef HAVE_MESOSIM
unsigned int
GUIEdge::getVehicleNo() const {
    size_t vehNo = 0;
    for (MESegment *segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment!=0; segment = segment->getNextSegment()) {
        vehNo += segment->getCarNumber();
    }
    return (unsigned int)vehNo;
}


SUMOReal
GUIEdge::getFlow() const {
    SUMOReal flow = 0;
    for (MESegment *segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment!=0; segment = segment->getNextSegment()) {
        flow += (SUMOReal) segment->getCarNumber() * segment->getMeanSpeed();
    }
    return flow *(SUMOReal) 1000. / (*myLanes)[0]->getLength() / (SUMOReal) 3.6;
}


SUMOReal
GUIEdge::getOccupancy() const {
    SUMOReal occ = 0;
    for (MESegment *segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment!=0; segment = segment->getNextSegment()) {
        occ += segment->getOccupancy();
    }
    return occ/(*myLanes)[0]->getLength()/(SUMOReal)(myLanes->size());
}


SUMOReal
GUIEdge::getMeanSpeed() const {
    SUMOReal v = 0;
    SUMOReal no = 0;
    for (MESegment *segment = MSGlobals::gMesoNet->getSegmentForEdge(*this); segment!=0; segment = segment->getNextSegment()) {
        SUMOReal vehNo = (SUMOReal) segment->getCarNumber();
        v += vehNo * segment->getMeanSpeed();
        no += vehNo;
    }
    return v/no;
}


SUMOReal
GUIEdge::getAllowedSpeed() const {
    return (*myLanes)[0]->getMaxSpeed();
}


GUIEdge::Colorer::Colorer() {
    mySchemes.push_back(GUIColorScheme("uniform (streetwise)", RGBColor(0,0,0), "", true));
    mySchemes.push_back(GUIColorScheme("by selection (streetwise)", RGBColor(0.7f, 0.7f, 0.7f), "unselected", true));
    mySchemes.back().addColor(RGBColor(0, .4f, .8f), 1, "selected");
    mySchemes.push_back(GUIColorScheme("by purpose (streetwise)", RGBColor(0,0,0), "normal", true));
    mySchemes.back().addColor(RGBColor(.5, 0, .5), MSEdge::EDGEFUNCTION_CONNECTOR, "connector");
    mySchemes.back().addColor(RGBColor(0, 0, 1), MSEdge::EDGEFUNCTION_INTERNAL, "internal");
    mySchemes.push_back(GUIColorScheme("by allowed speed (streetwise)", RGBColor(1,0,0)));
    mySchemes.back().addColor(RGBColor(0, 0, 1), (SUMOReal)(150.0/3.6));
    mySchemes.push_back(GUIColorScheme("by current occupancy (streetwise)", RGBColor(0,0,1)));
    mySchemes.back().addColor(RGBColor(1, 0, 0), (SUMOReal)0.95);
    mySchemes.push_back(GUIColorScheme("by current speed (streetwise)", RGBColor(1,0,0)));
    mySchemes.back().addColor(RGBColor(0, 0, 1), (SUMOReal)(150.0/3.6));
    mySchemes.push_back(GUIColorScheme("by current flow (streetwise)", RGBColor(0,0,1)));
    mySchemes.back().addColor(RGBColor(1, 0, 0), (SUMOReal)5000);
}


SUMOReal
GUIEdge::Colorer::getColorValue(const GUIEdge& edge) const {
    switch (myActiveScheme) {
    case 1:
        return gSelected.isSelected(edge.getType(), edge.getGlID());
    case 2:
        return edge.getPurpose();
    case 3:
        return edge.getAllowedSpeed();
    case 4:
        return edge.getOccupancy();
    case 5:
        return edge.getMeanSpeed();
    case 6:
        return edge.getFlow();
    }
    return 0;
}

#endif

/****************************************************************************/

