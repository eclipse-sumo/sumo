//---------------------------------------------------------------------------//
//                        GUIInductLoop.cpp -
//  The gui-version of the MSInductLoop, together with the according
//   wrapper
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Aug 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.8  2004/01/26 06:59:37  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics; different detector visualistaion in dependence to his controller
//
// Revision 1.7  2003/11/12 14:00:19  dkrajzew
// commets added; added parameter windows to all detectors
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <microsim/MSInductLoop.h>
#include <gui/GUIGlObject.h>
#include <utils/geom/Position2DVector.h>
#include "GUILaneWrapper.h"
#include "GUIInductLoop.h"
#include <utils/glutils/GLHelper.h>
#include <utils/geom/Line2D.h>
#include <gui/partable/GUIParameterTableWindow.h>
#include <microsim/logging/FuncBinding_UIntParam.h>
#include <microsim/logging/FunctionBinding.h>
#include <qgl.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUIInductLoop-methods
 * ----------------------------------------------------------------------- */
GUIInductLoop::GUIInductLoop(const std::string &id, MSLane* lane,
                             double position,
                             MSNet::Time deleteDataAfterSeconds)
    : MSInductLoop(id, lane, position, deleteDataAfterSeconds)
{
}


GUIInductLoop::~GUIInductLoop()
{
}


GUIDetectorWrapper *
GUIInductLoop::buildDetectorWrapper(GUIGlObjectStorage &idStorage,
                                    GUILaneWrapper &wrapper)
{
    return new MyWrapper(*this, idStorage, wrapper, posM);
}


/* -------------------------------------------------------------------------
 * GUIInductLoop::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUIInductLoop::MyWrapper::MyWrapper(GUIInductLoop &detector,
                                    GUIGlObjectStorage &idStorage,
                                    GUILaneWrapper &wrapper, double pos)
    : GUIDetectorWrapper(idStorage, string("induct loop:")+detector.getId()),
    myDetector(detector), myPosition(pos)
{
    const Position2DVector &v = wrapper.getShape();
    myFGPosition = v.positionAtLengthPosition(pos);
    Line2D l(v.getBegin(), v.getEnd());
    double sgPos = pos / v.length() * l.length();
    mySGPosition = l.getPositionAtDistance(sgPos);
    myBoundery.add(myFGPosition.x()+5.5, myFGPosition.y()+5.5);
    myBoundery.add(myFGPosition.x()-5.5, myFGPosition.y()-5.5);
    myBoundery.add(mySGPosition.x()+5.5, mySGPosition.y()+5.5);
    myBoundery.add(mySGPosition.x()-5.5, mySGPosition.y()-5.5);
    myFGRotation = -v.rotationDegreeAtLengthPosition(pos);
    mySGRotation = -l.atan2DegreeAngle();
}


GUIInductLoop::MyWrapper::~MyWrapper()
{
}


Boundery
GUIInductLoop::MyWrapper::getBoundery() const
{
    return myBoundery;
}



GUIParameterTableWindow *
GUIInductLoop::MyWrapper::getParameterWindow(GUIApplicationWindow &app,
                                             GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem("flow [veh/h]", true,
        new FuncBinding_UIntParam<GUIInductLoop, double>(
            &(getLoop()), &GUIInductLoop::getFlow, 1));
    ret->mkItem("mean speed [m/s]", true,
        new FuncBinding_UIntParam<GUIInductLoop, double>(
            &(getLoop()), &GUIInductLoop::getMeanSpeed, 1));
    ret->mkItem("occupancy [%]", true,
        new FuncBinding_UIntParam<GUIInductLoop, double>(
            &(getLoop()), &GUIInductLoop::getOccupancy, 1));
    ret->mkItem("mean vehicle length [m]", true,
        new FuncBinding_UIntParam<GUIInductLoop, double>(
            &(getLoop()), &GUIInductLoop::getMeanVehicleLength, 1));
    ret->mkItem("empty time [s]", true,
        new FunctionBinding<GUIInductLoop, double>(
            &(getLoop()), &GUIInductLoop::getTimestepsSinceLastDetection));
    //
    ret->mkItem("position [m]", false, myPosition);
    ret->mkItem("lane", false, myDetector.getLane()->id());
    // close building
    ret->closeBuilding();
    return ret;
}


std::string
GUIInductLoop::MyWrapper::microsimID() const
{
    return myDetector.getId();
}



bool
GUIInductLoop::MyWrapper::active() const
{
    return true;
}


void
GUIInductLoop::MyWrapper::drawGL_SG(double scale,
                                    GUISUMOAbstractView::GUIDetectorDrawer &drawer) const
{
    double width = 2.0 * scale;
    glLineWidth(1.0);
    // shape
    glColor3f(1, 1, 0);
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // !!!
    glTranslated(mySGPosition.x(), mySGPosition.y(), 0);
    glRotated( mySGRotation, 0, 0, 1 );
    glBegin( GL_QUADS );
    glVertex2f(0-1.0, 2);
    glVertex2f(-1.0, -2);
    glVertex2f(1.0, -2);
    glVertex2f(1.0, 2);
    glEnd();
    glBegin( GL_LINES);
    // without the substracted offsets, lines are partially longer
    //  than the boxes
    glVertex2f(0, 2-.1);
    glVertex2f(0, -2+.1);
    glEnd();


    // outline
    if(width>1) {
        glColor3f(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // !!!
        glBegin( GL_QUADS );
        glVertex2f(0-1.0, 2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0, 2);
        glEnd();
    }

    // position indicator
    if(width>1) {
        glRotated( 90, 0, 0, -1 );
        glColor3f(1, 1, 1);
        glBegin( GL_LINES);
        glVertex2f(0, 1.7);
        glVertex2f(0, -1.7);
        glEnd();
    }
    glPopMatrix();
}


void
GUIInductLoop::MyWrapper::drawGL_FG(double scale,
                                    GUISUMOAbstractView::GUIDetectorDrawer &drawer) const
{
    double width = 2.0 * scale;
    glLineWidth(1.0);
    // shape
    glColor3f(1, 1, 0);
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // !!!
    glTranslated(myFGPosition.x(), myFGPosition.y(), 0);
    glRotated( myFGRotation, 0, 0, 1 );
    glBegin( GL_QUADS );
    glVertex2f(0-1.0, 2);
    glVertex2f(-1.0, -2);
    glVertex2f(1.0, -2);
    glVertex2f(1.0, 2);
    glEnd();
    glBegin( GL_LINES);
    // without the substracted offsets, lines are partially longer
    //  than the boxes
    glVertex2f(0, 2-.1);
    glVertex2f(0, -2+.1);
    glEnd();


    // outline
    if(width>1) {
        glColor3f(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // !!!
        glBegin( GL_QUADS );
        glVertex2f(0-1.0, 2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0, 2);
        glEnd();
    }

    // position indicator
    if(width>1) {
        glRotated( 90, 0, 0, -1 );
        glColor3f(1, 1, 1);
        glBegin( GL_LINES);
        glVertex2f(0, 1.7);
        glVertex2f(0, -1.7);
        glEnd();
    }
    glPopMatrix();
}


Position2D
GUIInductLoop::MyWrapper::getPosition() const
{
    return myFGPosition;
}


GUIInductLoop &
GUIInductLoop::MyWrapper::getLoop()
{
    return myDetector;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIInductLoop.icc"
//#endif

// Local Variables:
// mode:C++
// End:

