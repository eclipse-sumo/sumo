/****************************************************************************/
/// @file    GUI_E2_ZS_CollectorOverLanes.cpp
/// @author  Daniel Krajzewicz
/// @date    Okt 2003
/// @version $Id$
///
// The gui-version of a MS_E2_ZS_CollectorOverLanes.
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

#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/Position2DVector.h>
#include "GUILaneWrapper.h"
#include "GUI_E2_ZS_CollectorOverLanes.h"
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <guisim/GUIEdge.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>      /* OpenGL header file */
#endif // _WIN32

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
/* -------------------------------------------------------------------------
 * GUI_E2_ZS_CollectorOverLanes-methods
 * ----------------------------------------------------------------------- */
GUI_E2_ZS_CollectorOverLanes::GUI_E2_ZS_CollectorOverLanes(std::string id,
        DetectorUsage usage, MSLane* lane, SUMOReal startPos,
        SUMOTime haltingTimeThreshold,
        MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold) throw()
        : MS_E2_ZS_CollectorOverLanes(id, usage, lane, startPos,
                                      haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold) {}



GUI_E2_ZS_CollectorOverLanes::~GUI_E2_ZS_CollectorOverLanes() throw() {}


GUIDetectorWrapper *
GUI_E2_ZS_CollectorOverLanes::buildDetectorWrapper(GUIGlObjectStorage &,
        GUILaneWrapper &) {
    throw 1;
}


GUIDetectorWrapper *
GUI_E2_ZS_CollectorOverLanes::buildDetectorWrapper(GUIGlObjectStorage &idStorage) {
    return new MyWrapper(*this, idStorage, myAlreadyBuild);
}


MSE2Collector *
GUI_E2_ZS_CollectorOverLanes::buildCollector(size_t c, size_t r, MSLane *l,
        SUMOReal start, SUMOReal end) throw() {
    string id = makeID(myID, c, r);
    if (start+end<l->length()) {
        start = l->length() - end - (SUMOReal) 0.1;
    }
    return new GUI_E2_ZS_Collector(id, myUsage,
                                   l, start, end, haltingTimeThresholdM,
                                   haltingSpeedThresholdM, jamDistThresholdM);
}


/* -------------------------------------------------------------------------
 * GUI_E2_ZS_CollectorOverLanes::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUI_E2_ZS_CollectorOverLanes::MyWrapper::MyWrapper(
    GUI_E2_ZS_CollectorOverLanes &detector,
    GUIGlObjectStorage &idStorage,
    const LaneDetMap &detectors) throw()
        : GUIDetectorWrapper(idStorage, "E2OverLanes detector:"+detector.getID()),
        myDetector(detector) {
    GLuint glID = idStorage.getUniqueID();
    for (LaneDetMap::const_iterator i=detectors.begin(); i!=detectors.end(); ++i) {
        MSLane *l = (*i).first;
        const GUIEdge * const edge = static_cast<const GUIEdge* const>(l->getEdge());
        GUILaneWrapper &w = edge->getLaneGeometry(l);
        GUI_E2_ZS_Collector *c =
            static_cast<GUI_E2_ZS_Collector*>((*i).second);
        GUIDetectorWrapper *dw =
            c->buildDetectorWrapper(idStorage, w, detector, glID);
        mySubWrappers.push_back(dw);
        myBoundary.add(dw->getCenteringBoundary());
    }
}


GUI_E2_ZS_CollectorOverLanes::MyWrapper::~MyWrapper() throw() {
    for (std::vector<GUIDetectorWrapper*>::iterator i=mySubWrappers.begin(); i!=mySubWrappers.end(); ++i) {
        delete(*i);
    }
}


Boundary
GUI_E2_ZS_CollectorOverLanes::MyWrapper::getCenteringBoundary() const throw() {
    Boundary b(myBoundary);
    return b;
}


GUIParameterTableWindow *
GUI_E2_ZS_CollectorOverLanes::MyWrapper::getParameterWindow(GUIMainWindow &app,
        GUISUMOAbstractView &) throw() {
    GUIParameterTableWindow *ret = new GUIParameterTableWindow(app, *this, 12);
    // add items
    /*
    myMkExistingItem(*ret, "density [?]", E2::DENSITY);
    myMkExistingItem(*ret, "jam lengths [veh]", E2::MAX_JAM_LENGTH_IN_VEHICLES);
    myMkExistingItem(*ret, "jam length [m]", E2::MAX_JAM_LENGTH_IN_METERS);
    myMkExistingItem(*ret, "jam len sum [veh]", E2::JAM_LENGTH_SUM_IN_VEHICLES);
    myMkExistingItem(*ret, "jam len sum [m]", E2::JAM_LENGTH_SUM_IN_METERS);
    myMkExistingItem(*ret, "queue length [veh]", E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_VEHICLES);
    myMkExistingItem(*ret, "queue length [m]", E2::QUEUE_LENGTH_AHEAD_OF_TRAFFIC_LIGHTS_IN_METERS);
    myMkExistingItem(*ret, "vehicles [veh]", E2::N_VEHICLES);
    myMkExistingItem(*ret, "occupancy degree [?]", E2::OCCUPANCY_DEGREE);
    myMkExistingItem(*ret, "space mean speed [?]", E2::SPACE_MEAN_SPEED);
    myMkExistingItem(*ret, "halting duration [?]", E2::CURRENT_HALTING_DURATION_SUM_PER_VEHICLE);
    */
    //
    ret->mkItem("length [m]", false, myDetector.getLength());
    /*
    ret->mkItem("position [m]", false,
        myDetector.getStartPos());*/
//    ret->mkItem("", false, myDetector.getStartLaneID());
    // close building
    ret->closeBuilding();
    return ret;
}


/*
void
GUI_E2_ZS_CollectorOverLanes::MyWrapper::myMkExistingItem(GUIParameterTableWindow &ret,
    const std::string &name,
    E2::DetType type)
{
if (!myDetector.hasDetector(type)) {
    return;
}
MyValueRetriever *binding =
    new MyValueRetriever(myDetector, type, 1);
ret.mkItem(name.c_str(), true, binding);
}
*/


const std::string &
GUI_E2_ZS_CollectorOverLanes::MyWrapper::getMicrosimID() const throw() {
    return myDetector.getID();
}


void
GUI_E2_ZS_CollectorOverLanes::MyWrapper::drawGL(const GUIVisualizationSettings &s) const throw() {
    for (std::vector<GUIDetectorWrapper*>::const_iterator i=mySubWrappers.begin(); i!=mySubWrappers.end(); ++i) {
        (*i)->drawGL(s);
    }
}


GUI_E2_ZS_CollectorOverLanes &
GUI_E2_ZS_CollectorOverLanes::MyWrapper::getLoop() {
    return myDetector;
}



/****************************************************************************/

