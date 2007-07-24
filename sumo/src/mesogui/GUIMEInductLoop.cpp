/****************************************************************************/
/// @file    GUIMEInductLoop.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 21.07.2005
/// @version $Id: GUIMEInductLoop.cpp 96 2007-06-06 07:40:46Z behr_mi $
///
// The gui-version of the MEInductLoop, together with the according
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

#include <mesosim/MEInductLoop.h>
#include <mesosim/MESegment.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/Position2DVector.h>
#include <guisim/GUILaneWrapper.h>
#include "GUIMEInductLoop.h"
#include <utils/glutils/GLHelper.h>
#include <utils/geom/Line2D.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FuncBinding_IntParam.h>
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
 * GUIMEInductLoop-methods
 * ----------------------------------------------------------------------- */
GUIMEInductLoop::GUIMEInductLoop(const std::string &id, MESegment * s,
                                 SUMOReal position,
                                 SUMOTime deleteDataAfterSeconds)
        : MEInductLoop(id, s, position, deleteDataAfterSeconds)
{}


GUIMEInductLoop::~GUIMEInductLoop()
{}


GUIDetectorWrapper *
GUIMEInductLoop::buildDetectorWrapper(GUIGlObjectStorage &idStorage,
                                      GUILaneWrapper &wrapper)
{
    return new MyWrapper(*this, idStorage, wrapper, posM);
}


/* -------------------------------------------------------------------------
 * GUIMEInductLoop::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUIMEInductLoop::MyWrapper::MyWrapper(GUIMEInductLoop &detector,
                                      GUIGlObjectStorage &idStorage,
                                      GUILaneWrapper &wrapper, SUMOReal pos)
        : GUIDetectorWrapper(idStorage, "induct loop:"+detector.getID()),
        myDetector(detector), myPosition(pos)
{
    const Position2DVector &v = wrapper.getShape();
    myFGPosition = v.positionAtLengthPosition(pos);
    Line2D l(v.getBegin(), v.getEnd());
    SUMOReal sgPos = pos / v.length() * l.length();
    mySGPosition = l.getPositionAtDistance(sgPos);
    myBoundary.add(myFGPosition.x()+(SUMOReal) 5.5, myFGPosition.y()+(SUMOReal) 5.5);
    myBoundary.add(myFGPosition.x()-(SUMOReal) 5.5, myFGPosition.y()-(SUMOReal) 5.5);
    myBoundary.add(mySGPosition.x()+(SUMOReal) 5.5, mySGPosition.y()+(SUMOReal) 5.5);
    myBoundary.add(mySGPosition.x()-(SUMOReal) 5.5, mySGPosition.y()-(SUMOReal) 5.5);
    myFGRotation = -v.rotationDegreeAtLengthPosition(pos);
    mySGRotation = -l.atan2DegreeAngle();
}


GUIMEInductLoop::MyWrapper::~MyWrapper()
{}


Boundary
GUIMEInductLoop::MyWrapper::getBoundary() const
{
    return myBoundary;
}



GUIParameterTableWindow *
GUIMEInductLoop::MyWrapper::getParameterWindow(GUIMainWindow &app,
        GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 2);
    // add items
    /*
    ret->mkItem("flow [veh/h]", true,
        new FuncBinding_IntParam<GUIMEInductLoop, SUMOReal>(
            &(getLoop()), &GUIMEInductLoop::getFlow, 1));
    ret->mkItem("mean speed [m/s]", true,
        new FuncBinding_IntParam<GUIMEInductLoop, SUMOReal>(
            &(getLoop()), &GUIMEInductLoop::getMeanSpeed, 1));
    ret->mkItem("occupancy [%]", true,
        new FuncBinding_IntParam<GUIMEInductLoop, SUMOReal>(
            &(getLoop()), &GUIMEInductLoop::getOccupancy, 1));
    ret->mkItem("mean vehicle length [m]", true,
        new FuncBinding_IntParam<GUIMEInductLoop, SUMOReal>(
            &(getLoop()), &GUIMEInductLoop::getMeanVehicleLength, 1));
    ret->mkItem("empty time [s]", true,
        new FunctionBinding<GUIMEInductLoop, SUMOReal>(
            &(getLoop()), &GUIMEInductLoop::getTimestepsSinceLastDetection));
            */
    //
    ret->mkItem("position [m]", false, myPosition);
    ret->mkItem("lane", false, myDetector.getSegment().getID());
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string &
GUIMEInductLoop::MyWrapper::microsimID() const
{
    return myDetector.getID();
}



bool
GUIMEInductLoop::MyWrapper::active() const
{
    return true;
}


void
GUIMEInductLoop::MyWrapper::drawGL(SUMOReal scale, SUMOReal upscale)
{
    SUMOReal width = (SUMOReal) 2.0 * scale;
    glLineWidth(1.0);
    // shape
    glColor3f(1, 1, 0);
    glPushMatrix();
    glScaled(upscale, upscale, upscale);
    glTranslated(myFGPosition.x(), myFGPosition.y(), 0);
    glRotated(myFGRotation, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2d(0-1.0, 2);
    glVertex2d(-1.0, -2);
    glVertex2d(1.0, -2);
    glVertex2d(1.0, 2);
    glEnd();
    glBegin(GL_LINES);
    // without the substracted offsets, lines are partially longer
    //  than the boxes
    glVertex2d(0, 2-.1);
    glVertex2d(0, -2+.1);
    glEnd();


    // outline
    if (width>1) {
        glColor3f(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glVertex2f(0-1.0, 2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0, 2);
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // position indicator
    if (width>1) {
        glRotated(90, 0, 0, -1);
        glColor3f(1, 1, 1);
        glBegin(GL_LINES);
        glVertex2d(0, 1.7);
        glVertex2d(0, -1.7);
        glEnd();
    }
    glPopMatrix();
}


Position2D
GUIMEInductLoop::MyWrapper::getPosition() const
{
    return myFGPosition;
}


GUIMEInductLoop &
GUIMEInductLoop::MyWrapper::getLoop()
{
    return myDetector;
}


/****************************************************************************/

