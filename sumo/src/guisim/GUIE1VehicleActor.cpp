/****************************************************************************/
/// @file    GUIE1VehicleActor.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 07.12.2005
/// @version $Id$
///
// Class that realises the setting of a lane's maximum speed triggered by
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

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/common/ToString.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include "GUIE1VehicleActor.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/GeomHelper.h>
#include <gui/GUIApplicationWindow.h>
#include <guisim/GUIE1VehicleActor.h>

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
GUIE1VehicleActor::GUIE1VehicleActor(const std::string& id, MSLane* lane,
                                     SUMOReal positionInMeters,
                                     unsigned int laid, unsigned int cellid,
                                     unsigned int type) throw()
        : MSE1VehicleActor(id, lane, positionInMeters, laid, cellid, type),
        GUIGlObject_AbstractAdd(gIDStorage, "actor:" + id, GLO_TRIGGER)
{
    const Position2DVector &v = lane->getShape();
    myFGPosition = v.positionAtLengthPosition(positionInMeters);
    Line2D l(v.getBegin(), v.getEnd());
    myFGRotation = -v.rotationDegreeAtLengthPosition(positionInMeters);
}


GUIE1VehicleActor::~GUIE1VehicleActor() throw()
{}


GUIGLObjectPopupMenu *
GUIE1VehicleActor::getPopUpMenu(GUIMainWindow &app,
                                GUISUMOAbstractView &parent) throw()
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIE1VehicleActor::getParameterWindow(GUIMainWindow &app,
                                      GUISUMOAbstractView &) throw()
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 9);
    // add items
    ret->mkItem("_ActorType", false, myLAId);
    ret->mkItem("LA-ID", false, myLAId);
    ret->mkItem("Area-ID", false, myAreaId);
    ret->mkItem("passed vehicles [#]", true,
                new CastingFunctionBinding<MSE1VehicleActor, SUMOReal, unsigned int>(this, &MSE1VehicleActor::getPassedVehicleNumber));
    ret->mkItem("passed cphones [#]", true,
                new CastingFunctionBinding<MSE1VehicleActor, SUMOReal, unsigned int>(this, &MSE1VehicleActor::getPassedCPhoneNumber));
    ret->mkItem("passed connected cphones [#]", true,
                new CastingFunctionBinding<MSE1VehicleActor, SUMOReal, unsigned int>(this, &MSE1VehicleActor::getPassedConnectedCPhoneNumber));
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string &
GUIE1VehicleActor::microsimID() const throw()
{
    return getID();
}


void
GUIE1VehicleActor::drawGL(SUMOReal scale, SUMOReal upscale) throw()
{
    SUMOReal width = (SUMOReal) 2.0 * scale;
    glLineWidth(1.0);
    // shape
    if (myActorType==1) {
        glColor3f(0, 1, 1);
    } else if (myActorType==2) {
        glColor3f(1, 0, 1);
    } else {
        glColor3f(1, 0, 0);
    }
    glPushMatrix();
    glTranslated(myFGPosition.x(), myFGPosition.y(), 0);
    glRotated(myFGRotation, 0, 0, 1);
    glScaled(upscale, upscale, upscale);
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
    if (width*upscale>1) {
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
    if (width*upscale>1) {
        glRotated(90, 0, 0, -1);
        glColor3f(1, 1, 1);
        glBegin(GL_LINES);
        glVertex2d(0, 1.7);
        glVertex2d(0, -1.7);
        glEnd();
    }
    if (myActorType==1) {
        glColor3f(0, 1, 1);
    } else if (myActorType==2) {
        glColor3f(1, 0, 1);
    } else {
        glColor3f(1, 0, 0);
    }
    glPopMatrix();
}


Boundary
GUIE1VehicleActor::getCenteringBoundary() const throw()
{
    Boundary b(myFGPosition.x(), myFGPosition.y(), myFGPosition.x(), myFGPosition.y());
    b.grow(20);
    return b;
}



/****************************************************************************/

