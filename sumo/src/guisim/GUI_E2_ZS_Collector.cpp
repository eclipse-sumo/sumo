/****************************************************************************/
/// @file    GUI_E2_ZS_Collector.cpp
/// @author  Daniel Krajzewicz
/// @date    Okt 2003
/// @version $Id$
///
// The gui-version of the MS_E2_ZS_Collector, together with the according
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include "GUI_E2_ZS_Collector.h"
#include <utils/glutils/GLHelper.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

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
 * GUI_E2_ZS_Collector-methods
 * ----------------------------------------------------------------------- */
GUI_E2_ZS_Collector::GUI_E2_ZS_Collector(std::string id, DetectorUsage usage,
        MSLane* lane, SUMOReal startPos, SUMOReal detLength,
        SUMOReal haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold,
        SUMOTime deleteDataAfterSeconds)
        : MSE2Collector(id, usage, lane, startPos, detLength, haltingTimeThreshold,
                        haltingSpeedThreshold, jamDistThreshold, deleteDataAfterSeconds)
{}



GUI_E2_ZS_Collector::~GUI_E2_ZS_Collector()
{}


GUIDetectorWrapper *
GUI_E2_ZS_Collector::buildDetectorWrapper(GUIGlObjectStorage &idStorage,
        GUILaneWrapper &wrapper)
{
    return new MyWrapper(*this, idStorage, wrapper);
}

GUIDetectorWrapper *
GUI_E2_ZS_Collector::buildDetectorWrapper(GUIGlObjectStorage &idStorage,
        GUILaneWrapper &wrapper,
        GUI_E2_ZS_CollectorOverLanes& p,
        size_t glID)
{
    return new MyWrapper(*this, idStorage, glID, p, wrapper);
}



/* -------------------------------------------------------------------------
 * GUI_E2_ZS_Collector::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUI_E2_ZS_Collector::MyWrapper::MyWrapper(GUI_E2_ZS_Collector &detector,
        GUIGlObjectStorage &idStorage,
        GUILaneWrapper &wrapper)
        : GUIDetectorWrapper(idStorage, "E2 detector:"+detector.getID()),
        myDetector(detector)
{
    myConstruct(detector, wrapper);
}


GUI_E2_ZS_Collector::MyWrapper::MyWrapper(
    GUI_E2_ZS_Collector &detector, GUIGlObjectStorage &idStorage,
    size_t glID, GUI_E2_ZS_CollectorOverLanes &,
    GUILaneWrapper &wrapper)
        : GUIDetectorWrapper(idStorage, "E2 detector:"+detector.getID(), glID),
        myDetector(detector)
{
    myConstruct(detector, wrapper);
}

void
GUI_E2_ZS_Collector::MyWrapper::myConstruct(GUI_E2_ZS_Collector &detector,
        GUILaneWrapper &wrapper)
{
    const Position2DVector &v = wrapper.getShape();
    Line2D l(v.getBegin(), v.getEnd());
    // build geometry
    myFullGeometry = v.getSubpart(detector.getStartPos(), detector.getEndPos());
    //
    myShapeRotations.reserve(myFullGeometry.size()-1);
    myShapeLengths.reserve(myFullGeometry.size()-1);
    for (size_t i=0; i<myFullGeometry.size()-1; ++i) {
        const Position2D &f = myFullGeometry[i];
        const Position2D &s = myFullGeometry[i+1];
        myShapeLengths.push_back(GeomHelper::distance(f, s));
        myShapeRotations.push_back((SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) 3.14159265);
    }
    //
    myBoundary = myFullGeometry.getBoxBoundary();
}


GUI_E2_ZS_Collector::MyWrapper::~MyWrapper()
{}


Boundary
GUI_E2_ZS_Collector::MyWrapper::getBoundary() const
{
    return myBoundary;
}


GUIParameterTableWindow *
GUI_E2_ZS_Collector::MyWrapper::getParameterWindow(GUIMainWindow &app,
        GUISUMOAbstractView &)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 14);
    // add items
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
    //
    ret->mkItem("length [m]", false, myDetector.getEndPos()-myDetector.getStartPos());
    ret->mkItem("position [m]", false, myDetector.getStartPos());
    ret->mkItem("lane", false, myDetector.getLane()->getID());
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUI_E2_ZS_Collector::MyWrapper::myMkExistingItem(GUIParameterTableWindow &ret,
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


const std::string &
GUI_E2_ZS_Collector::MyWrapper::microsimID() const
{
    return myDetector.getID();
}


bool
GUI_E2_ZS_Collector::MyWrapper::active() const
{
    return true;
}


void
GUI_E2_ZS_Collector::MyWrapper::drawGL(SUMOReal /*scale*/, SUMOReal upscale)
{
    SUMOReal myWidth = 1;
    if (myDetector.getUsageType()==DU_TL_CONTROL) {
        myWidth = (SUMOReal) 0.3;
        glColor3d(0, (SUMOReal) .6, (SUMOReal) .8);
    } else {
        glColor3d(0, (SUMOReal) .8, (SUMOReal) .8);
    }
    SUMOReal width=2; // !!!
    if (width*upscale>1.0) {
        glScaled(upscale, upscale, upscale);
        GLHelper::drawBoxLines(myFullGeometry, myShapeRotations, myShapeLengths, myWidth);
    } else {
        for (size_t i=0; i<myFullGeometry.size()-1; ++i) {
            GLHelper::drawLine(myFullGeometry[i],
                               myShapeRotations[i], myShapeLengths[i]);
        }
    }
}


Position2D
GUI_E2_ZS_Collector::MyWrapper::getPosition() const
{
    return myBoundary.getCenter();
}


GUI_E2_ZS_Collector &
GUI_E2_ZS_Collector::MyWrapper::getDetector()
{
    return myDetector;
}



/****************************************************************************/

