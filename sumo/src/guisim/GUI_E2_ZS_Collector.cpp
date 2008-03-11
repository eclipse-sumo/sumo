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
#include <utils/gui/div/GLHelper.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FunctionBinding.h>

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
GUI_E2_ZS_Collector::GUI_E2_ZS_Collector(const std::string &id, DetectorUsage usage,
        MSLane* lane, SUMOReal startPos, SUMOReal detLength,
        SUMOTime haltingTimeThreshold, SUMOReal haltingSpeedThreshold,
        SUMOReal jamDistThreshold) throw()
        : MSE2Collector(id, usage, lane, startPos, detLength, haltingTimeThreshold,
                        haltingSpeedThreshold, jamDistThreshold)
{}



GUI_E2_ZS_Collector::~GUI_E2_ZS_Collector() throw()
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
        GUILaneWrapper &wrapper) throw()
        : GUIDetectorWrapper(idStorage, "E2 detector:"+detector.getID()),
        myDetector(detector)
{
    myConstruct(detector, wrapper);
}


GUI_E2_ZS_Collector::MyWrapper::MyWrapper(
    GUI_E2_ZS_Collector &detector, GUIGlObjectStorage &idStorage,
    size_t glID, GUI_E2_ZS_CollectorOverLanes &,
    GUILaneWrapper &wrapper) throw()
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


GUI_E2_ZS_Collector::MyWrapper::~MyWrapper() throw()
{}


Boundary
GUI_E2_ZS_Collector::MyWrapper::getCenteringBoundary() const throw()
{
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}


GUIParameterTableWindow *
GUI_E2_ZS_Collector::MyWrapper::getParameterWindow(GUIMainWindow &app,
        GUISUMOAbstractView &) throw()
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 13);
    // add items
    // parameter
    ret->mkItem("length [m]", false, myDetector.getEndPos()-myDetector.getStartPos());
    ret->mkItem("position [m]", false, myDetector.getStartPos());
    ret->mkItem("lane", false, myDetector.getLane()->getID());
    // values
    ret->mkItem("vehicles [#]", true,
                new FunctionBinding<MSE2Collector, unsigned>(&myDetector, &MSE2Collector::getCurrentVehicleNumber));
    ret->mkItem("occupancy [%]", true,
                new FunctionBinding<MSE2Collector, SUMOReal>(&myDetector, &MSE2Collector::getCurrentOccupancy));
    ret->mkItem("mean speed [m/s]", true,
                new FunctionBinding<MSE2Collector, SUMOReal>(&myDetector, &MSE2Collector::getCurrentMeanSpeed));
    ret->mkItem("mean vehicle length [m]", true,
                new FunctionBinding<MSE2Collector, SUMOReal>(&myDetector, &MSE2Collector::getCurrentMeanLength));
    ret->mkItem("jam number [#]", true,
                new FunctionBinding<MSE2Collector, unsigned>(&myDetector, &MSE2Collector::getCurrentJamNumber));
    ret->mkItem("max jam length [veh]", true,
                new FunctionBinding<MSE2Collector, unsigned>(&myDetector, &MSE2Collector::getCurrentMaxJamLengthInVehicles));
    ret->mkItem("max jam length [m]", true,
                new FunctionBinding<MSE2Collector, SUMOReal>(&myDetector, &MSE2Collector::getCurrentMaxJamLengthInMeters));
    ret->mkItem("jam length sum [veh]", true,
                new FunctionBinding<MSE2Collector, unsigned>(&myDetector, &MSE2Collector::getCurrentJamLengthInVehicles));
    ret->mkItem("jam length sum [m]", true,
                new FunctionBinding<MSE2Collector, SUMOReal>(&myDetector, &MSE2Collector::getCurrentJamLengthInMeters));
    ret->mkItem("started halts [#]", true,
                new FunctionBinding<MSE2Collector, unsigned>(&myDetector, &MSE2Collector::getCurrentStartedHalts));
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string &
GUI_E2_ZS_Collector::MyWrapper::microsimID() const throw()
{
    return myDetector.getID();
}


void
GUI_E2_ZS_Collector::MyWrapper::drawGL(SUMOReal, SUMOReal upscale) throw()
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


GUI_E2_ZS_Collector &
GUI_E2_ZS_Collector::MyWrapper::getDetector()
{
    return myDetector;
}



/****************************************************************************/

