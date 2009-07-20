/****************************************************************************/
/// @file    GUIEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A road/street connecting two junctions (gui-version)
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
#include <foreign/polyfonts/polyfonts.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#include <microsim/MSGlobals.h>
#include <gui/GUIColorer_LaneByPurpose.h>
#include <utils/gui/drawer/GUIColorer_LaneBySelection.h>
#include <gui/GUIColorer_LaneByVehKnowledge.h>
#include <gui/GUIColorer_LaneNeighEdges.h>
#include <utils/gui/drawer/GUIColorer_SingleColor.h>
#include <utils/gui/drawer/GUIColorer_ShadeByFunctionValue.h>
#include <utils/gui/drawer/GUIColorer_ColorSettingFunction.h>
#include <utils/gui/drawer/GUIColorer_ByDeviceNumber.h>
#include <utils/gui/drawer/GUIColorer_ByOptCORNValue.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================
#ifdef HAVE_MESOSIM
GUIColoringSchemesMap<GUIEdge> GUIEdge::myLaneColoringSchemes;
#endif


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
    for (LaneCont::reverse_iterator i=myLanes->rbegin(); i<myLanes->rend(); ++i) {
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


std::vector<std::string>
GUIEdge::getNames() {
    std::vector<std::string> ret;
    ret.reserve(MSEdge::myDict.size());
    for (MSEdge::DictType::iterator i=MSEdge::myDict.begin(); i!=MSEdge::myDict.end(); ++i) {
        ret.push_back((*i).first);
    }
    return ret;
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
        netsWrappers.push_back(static_cast<GUIEdge*>((*i).second));
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
    ret->mkItem("length [m]", false, myLaneGeoms[0]->getLength());
    ret->mkItem("allowed speed [m/s]", false, getAllowedSpeed());
    ret->mkItem("occupancy [%]", true,
                new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getDensity));
    ret->mkItem("mean vehicle speed [m/s]", true,
                new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getMeanSpeed));
    ret->mkItem("flow [veh/h/lane]", true,
                new FunctionBinding<GUIEdge, SUMOReal>(this, &GUIEdge::getFlow));
    ret->mkItem("#vehicles", true,
                new CastingFunctionBinding<GUIEdge, SUMOReal, unsigned int>(this, &GUIEdge::getVehicleNo));
    // close building
    ret->closeBuilding();
#endif
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
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        myLaneColoringSchemes.getColorer(s.laneEdgeMode)->setGlColor(*this);
    }
#endif
    // draw the lanes
    for (LaneWrapperVector::const_iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end(); ++i) {
        (*i)->drawGL(s);
    }
    // check whether lane boundaries shall be drawn
    if (s.scale>1.&&s.laneShowBorders&&myFunction!=MSEdge::EDGEFUNCTION_INTERNAL) {
        glPolygonOffset(0, 1);
        for (LaneWrapperVector::const_iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end()-1; ++i) {
            (*i)->drawBordersGL(s);
        }
        // draw white boundings
        glPolygonOffset(0, 2);
        glColor3d(1,1,1);
        for (LaneWrapperVector::const_iterator i=myLaneGeoms.begin(); i!=myLaneGeoms.end()-1; ++i) {
            GLHelper::drawBoxLines((*i)->getShape(), (*i)->getShapeRotations(), (*i)->getShapeLengths(), SUMO_const_halfLaneAndOffset);
        }
    }
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        const Position2DVector& shape = myLaneGeoms[0]->getShape();
        const DoubleVector& shapeRotations = myLaneGeoms[0]->getShapeRotations();
        const DoubleVector& shapeLengths = myLaneGeoms[0]->getShapeLengths();
        const Position2D &laneBeg = shape[0];

        glColor3d(1,1,0);
        glPushMatrix();
        glTranslated(laneBeg.x(), laneBeg.y(), 0);
        glRotated(shapeRotations[0], 0, 0, 1);
        // go through the vehicles
        int shapePos = 0;
        SUMOReal positionOffset = 0;
        SUMOReal position = 0;
        MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge(this);
        do {
            const size_t numCars = first->getCarNumber();
            const SUMOReal occupancy = first->occupancy();
            for (int i = 0; i < numCars; i++) {
                SUMOReal vehiclePosition = position + i * occupancy / numCars;
                while (shapePos<(int)shapeRotations.size()-1 && vehiclePosition>positionOffset+shapeLengths[shapePos]) {
                    glPopMatrix();
                    positionOffset += shapeLengths[shapePos];
                    shapePos++;
                    glPushMatrix();
                    glTranslated(shape[shapePos].x(), shape[shapePos].y(), 0);
                    glRotated(shapeRotations[shapePos], 0, 0, 1);
                }
                glPushMatrix();
                glTranslated(0, -(vehiclePosition-positionOffset), 0);
                glPushMatrix();
                glScaled(1, occupancy / numCars, 1);
                glBegin(GL_TRIANGLES);
                glVertex2d(0, 0);
                glVertex2d(0-1.25, 1);
                glVertex2d(0+1.25, 1);
                glEnd();
                glPopMatrix();
                glPopMatrix();
            }
            position += first->getLength();
            first = first->getNextSegment();
        } while (first!=0);
        glPopMatrix();
    }
#endif
    // (optionally) draw the name
    if ((s.drawEdgeName && myFunction == EDGEFUNCTION_NORMAL) || (s.drawInternalEdgeName && myFunction != EDGEFUNCTION_NORMAL)) {
        float nameSize = s.edgeNameSize;
        if (myFunction == EDGEFUNCTION_NORMAL) {
            glColor3f(s.edgeNameColor.red(), s.edgeNameColor.green(), s.edgeNameColor.blue());
        } else {
            glColor3f(s.internalEdgeNameColor.red(), s.internalEdgeNameColor.green(), s.internalEdgeNameColor.blue());
            nameSize = s.internalEdgeNameSize;
        }
        glPolygonOffset(0, -6);
        GUILaneWrapper *lane1 = myLaneGeoms[0];
        GUILaneWrapper *lane2 = myLaneGeoms[myLaneGeoms.size()-1];
        glPushMatrix();
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
        glPopMatrix();
    }
}

#ifdef HAVE_MESOSIM
unsigned int
GUIEdge::getVehicleNo() const {
    MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge((GUIEdge*)this);
    assert(first!=0);
    unsigned int vehNo = 0;
    do {
        vehNo += first->getCarNumber();
        first = first->getNextSegment();
    } while (first!=0);
    return vehNo;
}


SUMOReal
GUIEdge::getFlow() const {
    MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge((GUIEdge*)this);
    assert(first!=0);
    SUMOReal flow = -1;
    int no = 0;
    do {
        SUMOReal vehNo = (SUMOReal) first->getCarNumber();
        SUMOReal v = first->getMeanSpeed();
        if (vehNo!=0) {
            if (no==0) {
                flow = (vehNo * (SUMOReal) 1000. / first->getLength()) * v / (SUMOReal) 3.6;
            } else {
                flow += (vehNo * (SUMOReal) 1000. / first->getLength()) * v / (SUMOReal) 3.6;
            }
            no++;
        }
        first = first->getNextSegment();
    } while (first!=0);
    if (flow>=0) {
        return flow/(SUMOReal)myLanes->size();
    }
    return -1;
}


SUMOReal
GUIEdge::getDensity() const {
    MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge(this);
    assert(first!=0);
    SUMOReal occ = 0;
    int no = 0;
    do {
        occ += first->occupancy() / first->getLength() / (SUMOReal) nLanes();
        no++;
        first = first->getNextSegment();
    } while (first!=0);
    if (no!=0) {
        return occ/(SUMOReal) no;
    }
    return -1;
}


SUMOReal
GUIEdge::getMeanSpeed() const {
    MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge(this);
    assert(first!=0);
    SUMOReal v = 0;
    int no = 0;
    do {
        v += first->getMeanSpeed();
        no++;
        first = first->getNextSegment();
    } while (first!=0);
    if (no!=0) {
        return v/(SUMOReal) no;
    }
    return -1;
}


SUMOReal
GUIEdge::getAllowedSpeed() const {
    return (*myLanes)[0]->maxSpeed();
}


GUIColoringSchemesMap<GUIEdge> &
GUIEdge::getSchemesMap() {
    return myLaneColoringSchemes;
}


void
GUIEdge::initColoringSchemes() {
    // insert possible edge coloring schemes
    myLaneColoringSchemes.add("uniform",
           new GUIColorer_SingleColor<GUIEdge>(RGBColor(0, 0, 0)));
    myLaneColoringSchemes.add("by selection (lanewise)",
           new GUIColorer_LaneBySelection<GUIEdge>());
    myLaneColoringSchemes.add("by purpose (lanewise)",
           new GUIColorer_LaneByPurpose<GUIEdge>());
    // from a lane's standard values
    myLaneColoringSchemes.add("by allowed speed (lanewise)",
           new GUIColorer_ShadeByFunctionValue<GUIEdge, SUMOReal>(
               0, (SUMOReal)(150.0/3.6),
               RGBColor(1, 0, 0), RGBColor(0, 0, 1),
               (SUMOReal(GUIEdge::*)() const) &GUIEdge::getAllowedSpeed));
    myLaneColoringSchemes.add("by current density (lanewise)",
           new GUIColorer_ShadeByFunctionValue<GUIEdge, SUMOReal>(
               0, (SUMOReal) .95,
               RGBColor(0, 1, 0), RGBColor(1, 0, 0),
               (SUMOReal(GUIEdge::*)() const) &GUIEdge::getDensity));
}

GUIColorer<GUIEdge>*
GUIEdge::createColorer() {
    return new GUIEdge::Colorer();
}

GUIEdge::Colorer::Colorer() {
    mySchemes.push_back(GUIColorScheme("uniform", RGBColor(0,0,0)));
}

SUMOReal
GUIEdge::Colorer::getColorValue(const GUIEdge& edge) const {
    switch (myActiveScheme) {
        case 0:
            return 0;
        case 1:
            return gSelected.isSelected(edge.getType(), edge.getGlID());
    }
    return 0;
}

#endif

/****************************************************************************/

