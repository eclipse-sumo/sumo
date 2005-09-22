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
// Revision 1.16  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.15  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.14  2005/05/04 07:59:59  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.13  2005/02/01 10:10:39  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.12  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.11  2004/07/02 08:42:13  dkrajzew
// changes in the detector drawer applied
//
// Revision 1.10  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.9  2004/02/16 13:57:15  dkrajzew
// tried to patch a sometimes occuring visualisation bug
//
// Revision 1.8  2004/01/26 06:59:37  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets
//  and lengths for lsa-detectors; coupling of detectors to tl-logics;
//  different detector visualistaion in dependence to his controller
//
// Revision 1.7  2003/11/12 14:00:19  dkrajzew
// commets added; added parameter windows to all detectors
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <microsim/output/MSInductLoop.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/Position2DVector.h>
#include "GUILaneWrapper.h"
#include "GUIInductLoop.h"
#include <utils/glutils/GLHelper.h>
#include <utils/geom/Line2D.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FuncBinding_IntParam.h>
#include <microsim/logging/FunctionBinding.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
                             SUMOReal position,
                             SUMOTime deleteDataAfterSeconds)
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
                                    GUILaneWrapper &wrapper, SUMOReal pos)
    : GUIDetectorWrapper(idStorage, string("induct loop:")+detector.getId()),
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


GUIInductLoop::MyWrapper::~MyWrapper()
{
}


Boundary
GUIInductLoop::MyWrapper::getBoundary() const
{
    return myBoundary;
}



GUIParameterTableWindow *
GUIInductLoop::MyWrapper::getParameterWindow(GUIMainWindow &app,
                                             GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 7);
    // add items
    ret->mkItem("flow [veh/h]", true,
        new FuncBinding_IntParam<GUIInductLoop, SUMOReal>(
            &(getLoop()), &GUIInductLoop::getFlow, 1));
    ret->mkItem("mean speed [m/s]", true,
        new FuncBinding_IntParam<GUIInductLoop, SUMOReal>(
            &(getLoop()), &GUIInductLoop::getMeanSpeed, 1));
    ret->mkItem("occupancy [%]", true,
        new FuncBinding_IntParam<GUIInductLoop, SUMOReal>(
            &(getLoop()), &GUIInductLoop::getOccupancy, 1));
    ret->mkItem("mean vehicle length [m]", true,
        new FuncBinding_IntParam<GUIInductLoop, SUMOReal>(
            &(getLoop()), &GUIInductLoop::getMeanVehicleLength, 1));
    ret->mkItem("empty time [s]", true,
        new FunctionBinding<GUIInductLoop, SUMOReal>(
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
GUIInductLoop::MyWrapper::drawGL_SG(SUMOReal scale)
{
    SUMOReal width = (SUMOReal) 2.0 * scale;
    glLineWidth(1.0);
    // shape
    glColor3f(1, 1, 0);
    glPushMatrix();
    glTranslated(mySGPosition.x(), mySGPosition.y(), 0);
    glRotated( mySGRotation, 0, 0, 1 );
    glBegin( GL_QUADS );
    glVertex2d(0-1.0, 2);
    glVertex2d(-1.0, -2);
    glVertex2d(1.0, -2);
    glVertex2d(1.0, 2);
    glEnd();
    glBegin( GL_LINES);
    // without the substracted offsets, lines are partially longer
    //  than the boxes
    glVertex2d(0, 2.0-.1);
    glVertex2d(0, -2.0+.1);
    glEnd();


    // outline
    if(width>1) {
        glColor3f(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin( GL_QUADS );
        glVertex2f(0-1.0, 2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0, 2);
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // position indicator
    if(width>1) {
        glRotated( 90, 0, 0, -1 );
        glColor3f(1, 1, 1);
        glBegin( GL_LINES);
        glVertex2d(0, 1.7);
        glVertex2d(0, -1.7);
        glEnd();
    }
    glPopMatrix();
}


void
GUIInductLoop::MyWrapper::drawGL_FG(SUMOReal scale)
{
    SUMOReal width = (SUMOReal) 2.0 * scale;
    glLineWidth(1.0);
    // shape
    glColor3f(1, 1, 0);
    glPushMatrix();
    glTranslated(myFGPosition.x(), myFGPosition.y(), 0);
    glRotated( myFGRotation, 0, 0, 1 );
    glBegin( GL_QUADS );
    glVertex2d(0-1.0, 2);
    glVertex2d(-1.0, -2);
    glVertex2d(1.0, -2);
    glVertex2d(1.0, 2);
    glEnd();
    glBegin( GL_LINES);
    // without the substracted offsets, lines are partially longer
    //  than the boxes
    glVertex2d(0, 2-.1);
    glVertex2d(0, -2+.1);
    glEnd();


    // outline
    if(width>1) {
        glColor3f(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin( GL_QUADS );
        glVertex2f(0-1.0, 2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0, 2);
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // position indicator
    if(width>1) {
        glRotated( 90, 0, 0, -1 );
        glColor3f(1, 1, 1);
        glBegin( GL_LINES);
        glVertex2d(0, 1.7);
        glVertex2d(0, -1.7);
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

// Local Variables:
// mode:C++
// End:

