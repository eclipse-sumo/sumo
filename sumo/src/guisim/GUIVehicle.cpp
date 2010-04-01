/****************************************************************************/
/// @file    GUIVehicle.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
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

#include <cmath>
#include <vector>
#include <string>
#include <utils/common/StringUtils.h>
#include <utils/common/SUMOVehicleParameter.h>
#include <microsim/MSVehicle.h>
#include "GUINet.h"
#include "GUIVehicle.h"
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSVehicleControl.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/common/RandHelper.h>
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/gui/div/GLHelper.h>
#include <foreign/polyfonts/polyfonts.h>


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIVehicle::GUIVehiclePopupMenu) GUIVehiclePopupMenuMap[]= {
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_BEST_LANES, GUIVehicle::GUIVehiclePopupMenu::onCmdShowBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_BEST_LANES, GUIVehicle::GUIVehiclePopupMenu::onCmdHideBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_START_TRACK, GUIVehicle::GUIVehiclePopupMenu::onCmdStartTrack),
    FXMAPFUNC(SEL_COMMAND, MID_STOP_TRACK, GUIVehicle::GUIVehiclePopupMenu::onCmdStopTrack),
};

// Object implementation
FXIMPLEMENT(GUIVehicle::GUIVehiclePopupMenu, GUIGLObjectPopupMenu, GUIVehiclePopupMenuMap, ARRAYNUMBER(GUIVehiclePopupMenuMap))



// ===========================================================================
// data definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * drawed shapes
 * ----------------------------------------------------------------------- */
double vehiclePoly_PassengerCarBody[] = { .5,0,  0,0,  0,.3,  0.08,.44,  0.25,.5,  0.95,.5,  1.,.4,  1.,-.4,  0.95,-.5,  0.25,-.5,  0.08,-.44,  0,-.3,  0,0,  -10000 };
double vehiclePoly_PassengerCarBodyFront[] = { 0.1,0,  0.025,0,  0.025,0.25,  0.27,0.4,  0.27,-.4,  0.025,-0.25,  0.025,0,  -10000 };
double vehiclePoly_PassengerFrontGlass[] = { 0.35,0,  0.3,0,  0.3,0.4,  0.43,0.3,  0.43,-0.3,  0.3,-0.4,  0.3,0,  -10000 };
double vehiclePoly_PassengerSedanRightGlass[] = { 0.36,-.43,  0.34,-.47,  0.77,-.47,  0.67,-.37,  0.45,-.37,  0.34,-.47,  -10000 };
double vehiclePoly_PassengerSedanLeftGlass[] = { 0.36,.43,  0.34,.47,  0.77,.47,  0.67,.37,  0.45,.37,  0.34,.47,  -10000 };
double vehiclePoly_PassengerSedanBackGlass[] = { 0.80,0,  0.70,0,  0.70,0.3,  0.83,0.4,  0.83,-.4,  0.70,-.3,  0.70,0,  -10000 };
double vehiclePoly_PassengerHatchbackRightGlass[] = { 0.36,-.43,  0.34,-.47,  0.94,-.47,  0.80,-.37,  0.45,-.37,  0.34,-.47,  -10000 };
double vehiclePoly_PassengerHatchbackLeftGlass[] = { 0.36,.43,  0.34,.47,  0.94,.47,  0.80,.37,  0.45,.37,  0.34,.47,  -10000 };
double vehiclePoly_PassengerHatchbackBackGlass[] = { 0.92,0,  0.80,0,  0.80,0.3,  0.95,0.4,  0.95,-.4,  0.80,-.3,  0.80,0,  -10000 };
double vehiclePoly_PassengerWagonRightGlass[] = { 0.36,-.43,  0.34,-.47,  0.94,-.47,  0.87,-.37,  0.45,-.37,  0.34,-.47,  -10000 };
double vehiclePoly_PassengerWagonLeftGlass[] = { 0.36,.43,  0.34,.47,  0.94,.47,  0.87,.37,  0.45,.37,  0.34,.47,  -10000 };
double vehiclePoly_PassengerWagonBackGlass[] = { 0.92,0,  0.90,0,  0.90,0.3,  0.95,0.4,  0.95,-.4,  0.90,-.3,  0.90,0,  -10000 };

double vehiclePoly_PassengerVanBody[] = { .5,0,  0,0,  0,.4,  0.1,.5,  0.97,.5,  1.,.47,  1.,-.47,  0.97,-.5,  0.1,-.5,  0,-.4,  0,0,  -10000 };
double vehiclePoly_PassengerVanBodyFront[] = { 0.1,0,  0.025,0,  0.025,0.25,  0.13,0.4,  0.13,-.4,  0.025,-0.25,  0.025,0,  -10000 };
double vehiclePoly_PassengerVanFrontGlass[] = { 0.21,0,  0.16,0,  0.16,0.4,  0.29,0.3,  0.29,-0.3,  0.16,-0.4,  0.16,0,  -10000 };
double vehiclePoly_PassengerVanRightGlass[] = { 0.36,-.43,  0.20,-.47,  0.98,-.47,  0.91,-.37,  0.31,-.37,  0.20,-.47,  -10000 };
double vehiclePoly_PassengerVanLeftGlass[] = { 0.36,.43,  0.20,.47,  0.98,.47,  0.91,.37,  0.31,.37,  0.20,.47,  -10000 };
double vehiclePoly_PassengerVanBackGlass[] = { 0.95,0,  0.94,0,  0.94,0.3,  0.98,0.4,  0.98,-.4,  0.94,-.3,  0.94,0,  -10000 };

double vehiclePoly_DeliveryMediumRightGlass[] = { 0.21,-.43,  0.20,-.47,  0.38,-.47,  0.38,-.37,  0.31,-.37,  0.20,-.47,  -10000 };
double vehiclePoly_DeliveryMediumLeftGlass[] = { 0.21,.43,  0.20,.47,  0.38,.47,  0.38,.37,  0.31,.37,  0.20,.47,  -10000 };

double vehiclePoly_TransportBody[] = { .5,0,  0,0,  0,.45,  0.05,.5,  2.25,.5,  2.25,-.5,  0.05,-.5,  0,-.45,  0,0,  -10000 };
double vehiclePoly_TransportFrontGlass[] = { 0.1,0,  0.05,0,  0.05,0.45,  0.25,0.4,  0.25,-.4,  0.05,-0.45,  0.05,0,  -10000 };
double vehiclePoly_TransportRightGlass[] = { 0.36,-.47,  0.10,-.48,  1.25,-.48,  1.25,-.4,  0.3,-.4,  0.10,-.48,  -10000 };
double vehiclePoly_TransportLeftGlass[] = { 0.36,.47,  0.10,.48,  1.25,.48,  1.25,.4,  0.3,.4,  0.10,.48,  -10000 };

double vehiclePoly_EVehicleBody[] = { .5,0,  0,0,  0,.3,  0.08,.44,  0.25,.5,  0.75,.5,  .92,.44,  1,.3,  1,-.3,  .92,-.44,  .75,-.5,  .25,-.5,  0.08,-.44,  0,-.3,  0,0,  -1000 };
double vehiclePoly_EVehicleFrontGlass[] = { .5,0,  0.05,.05,  0.05,.25,  0.13,.39,  0.3,.45,  0.70,.45,  .87,.39,  .95,.25,  .95,-.25,  .87,-.39,  .70,-.45,  .3,-.45,  0.13,-.39,  0.05,-.25,  0.05,0.05,  -1000 };
//double vehiclePoly_EVehicleFrontGlass[] = { 0.35,0,  0.1,0,  0.1,0.4,  0.43,0.3,  0.43,-0.3,  0.1,-0.4,  0.1,0,  -10000 };
double vehiclePoly_EVehicleBackGlass[] =  { 0.65,0,  0.9,0,  0.9,0.4,  0.57,0.3,  0.57,-0.3,  0.9,-0.4,  0.9,0,  -10000 };


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIVehicle::GUIVehiclePopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIVehicle::GUIVehiclePopupMenu::GUIVehiclePopupMenu(
    GUIMainWindow &app, GUISUMOAbstractView &parent,
    GUIGlObject &o)
        : GUIGLObjectPopupMenu(app, parent, o) {
}


GUIVehicle::GUIVehiclePopupMenu::~GUIVehiclePopupMenu() throw() {}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes(FXObject*,FXSelector,void*) {
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->showRoute(static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes(FXObject*,FXSelector,void*) {
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->hideRoute(static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute(FXObject*,FXSelector,void*) {
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->showRoute(static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowBestLanes(FXObject*,FXSelector,void*) {
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->showBestLanes(static_cast<GUIVehicle*>(myObject));
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute(FXObject*,FXSelector,void*) {
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->hideRoute(static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideBestLanes(FXObject*,FXSelector,void*) {
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->hideBestLanes(static_cast<GUIVehicle*>(myObject));
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdStartTrack(FXObject*,FXSelector,void*) {
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->startTrack(static_cast<GUIVehicle*>(myObject)->getGlID());
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdStopTrack(FXObject*,FXSelector,void*) {
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->stopTrack();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIVehicle - methods
 * ----------------------------------------------------------------------- */
GUIVehicle::GUIVehicle(GUIGlObjectStorage &idStorage,
                       SUMOVehicleParameter* pars, const MSRoute* route,
                       const MSVehicleType* type,
                       int vehicleIndex) throw(ProcessError)
        : MSVehicle(pars, route, type, vehicleIndex),
        GUIGlObject(idStorage, "vehicle:"+pars->id) {
    myIntCORNMap[MSCORN::CORN_VEH_BLINKER] = 0;
}


GUIVehicle::~GUIVehicle() throw() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


SUMOReal
GUIVehicle::getTimeSinceLastLaneChangeAsReal() const {
    return (SUMOReal) myLastLaneChangeOffset;
}


GUIGLObjectPopupMenu *
GUIVehicle::getPopUpMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent) throw() {
    GUIGLObjectPopupMenu *ret = new GUIVehiclePopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
    if (parent.amShowingRouteFor(this, 0)) {
        new FXMenuCommand(ret, "Hide Current Route", 0, ret, MID_HIDE_CURRENTROUTE);
    } else {
        new FXMenuCommand(ret, "Show Current Route", 0, ret, MID_SHOW_CURRENTROUTE);
    }
    if (parent.amShowingRouteFor(this, -1)) {
        new FXMenuCommand(ret, "Hide All Routes", 0, ret, MID_HIDE_ALLROUTES);
    } else {
        new FXMenuCommand(ret, "Show All Routes", 0, ret, MID_SHOW_ALLROUTES);
    }
    if (parent.amShowingBestLanesFor(this)) {
        new FXMenuCommand(ret, "Hide Best Lanes", 0, ret, MID_HIDE_BEST_LANES);
    } else {
        new FXMenuCommand(ret, "Show Best Lanes", 0, ret, MID_SHOW_BEST_LANES);
    }
    new FXMenuSeparator(ret);
    int trackedID = parent.getTrackedID();
    if (trackedID<0||(size_t)trackedID!=getGlID()) {
        new FXMenuCommand(ret, "Start Tracking", 0, ret, MID_START_TRACK);
    } else {
        new FXMenuCommand(ret, "Stop Tracking", 0, ret, MID_STOP_TRACK);
    }
    new FXMenuSeparator(ret);
    //
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIVehicle::getParameterWindow(GUIMainWindow &app,
                               GUISUMOAbstractView &) throw() {
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 15);
    // add items
    ret->mkItem("type [NAME]", false, myType->getID());
    if (getParameter().repetitionNumber>0) {
        ret->mkItem("left same route [#]", false, (unsigned int) getParameter().repetitionNumber);
    }
    if (getParameter().repetitionOffset>0) {
        ret->mkItem("emission period [s]", false, (SUMOTime) getParameter().repetitionOffset);
    }
    ret->mkItem("waiting time [s]", true,
                new CastingFunctionBinding<MSVehicle, SUMOReal, unsigned int>(this, &MSVehicle::getWaitingTime));
    ret->mkItem("last lane change [s]", true,
                new CastingFunctionBinding<GUIVehicle, SUMOReal, unsigned int>(this, &GUIVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, getDesiredDepart());
    ret->mkItem("position [m]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getPositionOnLane));
    ret->mkItem("speed [m/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getSpeed));
    ret->mkItem("CO2 (HBEFA) [g/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_CO2Emissions));
    ret->mkItem("CO (HBEFA) [g/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_COEmissions));
    ret->mkItem("HC (HBEFA) [g/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_HCEmissions));
    ret->mkItem("NOx (HBEFA) [g/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_NOxEmissions));
    ret->mkItem("PMx (HBEFA) [g/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_PMxEmissions));
    ret->mkItem("fuel (HBEFA) [l/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHBEFA_FuelConsumption));
    ret->mkItem("noise (Harmonoise) [dB]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getHarmonoise_NoiseEmissions));
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string &
GUIVehicle::getMicrosimID() const throw() {
    return getID();
}


bool
GUIVehicle::active() const throw() {
    return isOnRoad();
}


void
GUIVehicle::setRemoved() {
    myLane = 0;
}


unsigned int
GUIVehicle::getLastLaneChangeOffset() const {
    return myLastLaneChangeOffset;
}


Boundary
GUIVehicle::getCenteringBoundary() const throw() {
    Boundary b;
    b.add(getPosition());
    b.grow(20);
    return b;
}










inline void
drawAction_drawVehicleAsTrianglePlus(const GUIVehicle &veh, SUMOReal upscale) {
    SUMOReal length = veh.getVehicleType().getLength();
    glPushMatrix();
    glScaled(upscale, upscale, 1);
    if (length<8) {
        glScaled(1, length, 1);
        glBegin(GL_TRIANGLES);
        glVertex2d(0, 0);
        glVertex2d(0-1.25, 1);
        glVertex2d(0+1.25, 1);
        glEnd();
    } else {
        glBegin(GL_TRIANGLES);
        glVertex2d(0, 0);
        glVertex2d(0-1.25, 0+2);
        glVertex2d(0+1.25, 0+2);
        glVertex2d(0-1.25, 2);
        glVertex2d(0-1.25, length);
        glVertex2d(0+1.25, length);
        glVertex2d(0+1.25, 2);
        glVertex2d(0-1.25, 2);
        glVertex2d(0+1.25, length);
        glEnd();
    }
    glPopMatrix();
}

inline void
drawAction_drawVehicleAsBoxPlus(const GUIVehicle &veh, SUMOReal upscale) {
    SUMOReal length = veh.getVehicleType().getLength();
    SUMOReal offset = veh.getVehicleType().getGuiOffset();
    glPushMatrix();
    glRotated(90, 0, 0, 1);
    //glTranslated(veh.getVehicleType().getGuiOffset(), 0, 0);
    glScaled(1, veh.getVehicleType().getGuiWidth(), 1.);
    glScaled(upscale, upscale, 1);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d((length-offset)/2., 0);
    glVertex2d(offset, 0);
    glVertex2d(offset, -.4);
    glVertex2d(offset+.1, -.5);
    glVertex2d(length, -.5);
    glVertex2d(length, .5);
    glVertex2d(offset+.1, .5);
    glVertex2d(offset, .4);
    glVertex2d(offset, 0);
    glEnd();
    glPopMatrix();
}


void
drawPoly(double *poses, SUMOReal offset) {
    glPushMatrix();
    glTranslated(0, 0, offset*.01);
    glPolygonOffset(0, offset);
    glBegin(GL_TRIANGLE_FAN);
    int i = 0;
    while (poses[i]>-999) {
        glVertex2d(poses[i],poses[i+1]);
        i = i + 2;
    }
    glEnd();
    glPopMatrix();
}


inline void
drawAction_drawVehicleAsPoly(const GUIVehicle &veh, SUMOReal upscale) {
    GLdouble current[4], lighter[4];
    glGetDoublev(GL_CURRENT_COLOR, current);
    lighter[0] = current[0]+.2;
    if (lighter[0]>1) lighter[0] = 1;
    lighter[1] = current[1]+.2;
    if (lighter[1]>1) lighter[1] = 1;
    lighter[2] = current[2]+.2;
    if (lighter[2]>1) lighter[2] = 1;
    lighter[3] = current[3]+.2;
    if (lighter[3]>1) lighter[3] = 1;


    SUMOReal length = veh.getVehicleType().getLength();
    glPushMatrix();
    glRotated(90, 0, 0, 1);
    glTranslated(veh.getVehicleType().getGuiOffset(), 0, 0);
    glScaled(length-veh.getVehicleType().getGuiOffset(), veh.getVehicleType().getGuiWidth(), 1.);
    glScaled(upscale, upscale, 1);
    SUMOVehicleShape shape = veh.getVehicleType().getGuiShape();
    switch (shape) {
    case SVS_UNKNOWN:
        drawPoly(vehiclePoly_PassengerCarBody, -4);
        glColor3dv(lighter);
        drawPoly(vehiclePoly_PassengerCarBodyFront, -4.5);
        glColor3f(0, 0, 0);
        drawPoly(vehiclePoly_PassengerFrontGlass, -4.5);
        break;
    case SVS_PEDESTRIAN:
        //glScaled(1./(length-veh.getVehicleType().getGuiOffset()), 1, 1.);
        glTranslated(0, 0, -.045);
        GLHelper::drawFilledCircle(1);
        glTranslated(0, 0, .045);
        glScaled(.7, 2, 1);
        glTranslated(0, 0, -.04);
        glColor3dv(lighter);
        GLHelper::drawFilledCircle(1);
        glTranslated(0, 0, .04);
        break;
    case SVS_BICYCLE:
    case SVS_MOTORCYCLE: {
        SUMOReal offset = 0;
        glPushMatrix();
        glTranslated(.5, 0, 0);
        glScaled(.25/(length-veh.getVehicleType().getGuiOffset()), 1, 1.);
        glTranslated(0, 0, -.045);
        GLHelper::drawFilledCircle(1);
        glScaled(.7, 2, 1);
        glTranslated(0, 0, .045);
        glTranslated(0, 0, -.04);
        glColor3dv(lighter);
        GLHelper::drawFilledCircle(1);
        glTranslated(0, 0, .04);
        glPopMatrix();
    }
    break;
    case SVS_PASSENGER:
    case SVS_PASSENGER_SEDAN:
    case SVS_PASSENGER_HATCHBACK:
    case SVS_PASSENGER_WAGON:
        drawPoly(vehiclePoly_PassengerCarBody, -4);
        glColor3dv(lighter);
        drawPoly(vehiclePoly_PassengerCarBodyFront, -4.5);
        glColor3f(0, 0, 0);
        drawPoly(vehiclePoly_PassengerFrontGlass, -4.5);
        break;
    case SVS_PASSENGER_VAN:
        drawPoly(vehiclePoly_PassengerVanBody, -4);
        glColor3dv(lighter);
        drawPoly(vehiclePoly_PassengerVanBodyFront, -4.5);
        glColor3f(0, 0, 0);
        drawPoly(vehiclePoly_PassengerVanFrontGlass, -4.5);
        drawPoly(vehiclePoly_PassengerVanRightGlass, -4.5);
        drawPoly(vehiclePoly_PassengerVanLeftGlass, -4.5);
        drawPoly(vehiclePoly_PassengerVanBackGlass, -4.5);
        break;
    case SVS_DELIVERY:
        drawPoly(vehiclePoly_PassengerVanBody, -4);
        glColor3dv(lighter);
        drawPoly(vehiclePoly_PassengerVanBodyFront, -4.5);
        glColor3f(0, 0, 0);
        drawPoly(vehiclePoly_PassengerVanFrontGlass, -4.5);
        drawPoly(vehiclePoly_DeliveryMediumRightGlass, -4.5);
        drawPoly(vehiclePoly_DeliveryMediumLeftGlass, -4.5);
        break;
    case SVS_TRANSPORT:
    case SVS_TRANSPORT_SEMITRAILER:
    case SVS_TRANSPORT_1TRAILER:
        glScaled(1./(length-veh.getVehicleType().getGuiOffset()), 1, 1.);
        drawPoly(vehiclePoly_TransportBody, -4);
        glColor3f(0, 0, 0);
        drawPoly(vehiclePoly_TransportFrontGlass, -4.5);
        drawPoly(vehiclePoly_TransportRightGlass, -4.5);
        drawPoly(vehiclePoly_TransportLeftGlass, -4.5);
        break;
    case SVS_BUS:
    case SVS_BUS_CITY: {
        SUMOReal ml = length - veh.getVehicleType().getGuiOffset();
        glScaled(1./(length-veh.getVehicleType().getGuiOffset()), 1, 1.);
        glTranslated(0, 0, -.04);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(ml/2., 0);
        glVertex2d(0, 0);
        glVertex2d(0, -.45);
        glVertex2d(0+.05, -.5);
        glVertex2d(ml-.05, -.5);
        glVertex2d(ml, -.45);
        glVertex2d(ml, .45);
        glVertex2d(ml-.05, .5);
        glVertex2d(0+.05, .5);
        glVertex2d(0, .45);
        glVertex2d(0, 0);
        glEnd();
        glTranslated(0, 0, .04);

        glTranslated(0, 0, -.045);
        glColor3f(0, 0, 0);
        glBegin(GL_QUADS);
        glVertex2d(0+.05, .48);
        glVertex2d(0+.05, -.48);
        glVertex2d(0+.15, -.48);
        glVertex2d(0+.15, .48);

        glVertex2d(ml-.1, .45);
        glVertex2d(ml-.1, -.45);
        glVertex2d(ml-.05, -.45);
        glVertex2d(ml-.05, .45);

        glVertex2d(0+.20, .49);
        glVertex2d(0+.20, .45);
        glVertex2d(ml-.20, .45);
        glVertex2d(ml-.20, .49);

        glVertex2d(0+.20, -.49);
        glVertex2d(0+.20, -.45);
        glVertex2d(ml-.20, -.45);
        glVertex2d(ml-.20, -.49);

        glEnd();
        glTranslated(0, 0, .045);
    }
    break;
    case SVS_BUS_OVERLAND:
        glScaled(1./(length-veh.getVehicleType().getGuiOffset()), 1, 1.);
        glTranslated(0, 0, -.04);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(length/2., 0);
        glVertex2d(0, 0);
        glVertex2d(0, -.45);
        glVertex2d(.05, -.5);
        glVertex2d(length-.05, -.5);
        glVertex2d(length, -.45);
        glVertex2d(length, .45);
        glVertex2d(length-.05, .5);
        glVertex2d(.05, .5);
        glVertex2d(0, .45);
        glVertex2d(0, 0);
        glEnd();
        glTranslated(0, 0, .04);
        break;
    case SVS_RAIL:
        break;
    case SVS_RAIL_LIGHT:
        break;
    case SVS_RAIL_CITY:
        break;
    case SVS_RAIL_SLOW:
        break;
    case SVS_RAIL_FAST:
        break;
    case SVS_RAIL_CARGO:
        break;
    case SVS_E_VEHICLE:
        drawPoly(vehiclePoly_EVehicleBody, -4);
        glColor3f(0, 0, 0);
        drawPoly(vehiclePoly_EVehicleFrontGlass, -4.5);
        glTranslated(0, 0, -.048);
        glColor3dv(current);
        glBegin(GL_QUADS);
        glVertex2d(.3, .5);
        glVertex2d(.35, .5);
        glVertex2d(.35, -.5);
        glVertex2d(.3, -.5);

        glVertex2d(.3, -.05);
        glVertex2d(.7, -.05);
        glVertex2d(.7, .05);
        glVertex2d(.3, .05);

        glVertex2d(.7, .5);
        glVertex2d(.65, .5);
        glVertex2d(.65, -.5);
        glVertex2d(.7, -.5);
        glEnd();
        glTranslated(0, 0, .048);
        //drawPoly(vehiclePoly_EVehicleBackGlass, -4.5);
        break;
    default: // same as passenger
        drawPoly(vehiclePoly_PassengerCarBody, -4);
        glColor3f(1, 1, 1);
        drawPoly(vehiclePoly_PassengerCarBodyFront, -4.5);
        glColor3f(0, 0, 0);
        drawPoly(vehiclePoly_PassengerFrontGlass, -4.5);
        break;
    }

    switch (shape) {
    case SVS_PEDESTRIAN:
        break;
    case SVS_BICYCLE:
        //glScaled(length-veh.getVehicleType().getGuiOffset(), 1, 1.);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(1/2., 0);
        glVertex2d(0, 0);
        glVertex2d(0, -.03);
        glVertex2d(0+.05, -.05);
        glVertex2d(1-.05, -.05);
        glVertex2d(1, -.03);
        glVertex2d(1, .03);
        glVertex2d(1-.05, .05);
        glVertex2d(0+.05, .05);
        glVertex2d(0, .03);
        glVertex2d(0, 0);
        glEnd();
        break;
    case SVS_MOTORCYCLE:
        //glScaled(length-veh.getVehicleType().getGuiOffset(), 1, 1.);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(1/2., 0);
        glVertex2d(0, 0);
        glVertex2d(0, -.03);
        glVertex2d(0+.05, -.2);
        glVertex2d(1-.05, -.2);
        glVertex2d(1, -.03);
        glVertex2d(1, .03);
        glVertex2d(1-.05, .2);
        glVertex2d(0+.05, .2);
        glVertex2d(0, .03);
        glVertex2d(0, 0);
        glEnd();
        break;
    case SVS_PASSENGER:
    case SVS_PASSENGER_SEDAN:
        drawPoly(vehiclePoly_PassengerSedanRightGlass, -4.5);
        drawPoly(vehiclePoly_PassengerSedanLeftGlass, -4.5);
        drawPoly(vehiclePoly_PassengerSedanBackGlass, -4.5);
        break;
    case SVS_PASSENGER_HATCHBACK:
        drawPoly(vehiclePoly_PassengerHatchbackRightGlass, -4.5);
        drawPoly(vehiclePoly_PassengerHatchbackLeftGlass, -4.5);
        drawPoly(vehiclePoly_PassengerHatchbackBackGlass, -4.5);
        break;
    case SVS_PASSENGER_WAGON:
        drawPoly(vehiclePoly_PassengerWagonRightGlass, -4.5);
        drawPoly(vehiclePoly_PassengerWagonLeftGlass, -4.5);
        drawPoly(vehiclePoly_PassengerWagonBackGlass, -4.5);
        break;
    case SVS_PASSENGER_VAN:
    case SVS_DELIVERY:
        break;
    case SVS_TRANSPORT:
        glColor3dv(current);
        GLHelper::drawBoxLine(Position2D(2.3, 0), 90., length-veh.getVehicleType().getGuiOffset()-2.3, .5);
        break;
    case SVS_TRANSPORT_SEMITRAILER:
        glColor3dv(current);
        GLHelper::drawBoxLine(Position2D(2.8, 0), 90., length-veh.getVehicleType().getGuiOffset()-2.8, .5);
        break;
    case SVS_TRANSPORT_1TRAILER: {
        glColor3dv(current);
        SUMOReal l = length-veh.getVehicleType().getGuiOffset()-2.3;
        l = l/2.;
        GLHelper::drawBoxLine(Position2D(2.3, 0), 90., l, .5);
        GLHelper::drawBoxLine(Position2D(2.3+l+.5, 0), 90., l-.5, .5);
        break;
    }
    case SVS_BUS:
    case SVS_BUS_CITY:
    case SVS_BUS_CITY_FLEXIBLE:
    case SVS_BUS_OVERLAND:
    case SVS_RAIL:
    case SVS_RAIL_LIGHT:
    case SVS_RAIL_CITY:
    case SVS_RAIL_SLOW:
    case SVS_RAIL_FAST:
    case SVS_RAIL_CARGO:
    case SVS_E_VEHICLE:
        break;
    default: // same as passenger/sedan
        drawPoly(vehiclePoly_PassengerSedanRightGlass, -4.5);
        drawPoly(vehiclePoly_PassengerSedanLeftGlass, -4.5);
        drawPoly(vehiclePoly_PassengerSedanBackGlass, -4.5);
        break;
    }
    /*
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(.5,.5); // center - strip begin
    glVertex2d(0,    .5); // center, front
    glVertex2d(0,    .8); // ... clockwise ... (vehicle right side)
    glVertex2d(0.08, .94);
    glVertex2d(0.25, 1.);
    glVertex2d(0.95, 1.);
    glVertex2d(1.,   .9);
    glVertex2d(1.,   .1); // (vehicle left side)
    glVertex2d(0.95, 0.);
    glVertex2d(0.25, 0.);
    glVertex2d(0.08, .06);
    glVertex2d(0,    .2); //
    glVertex2d(0,    .5); // center, front (close)
    glEnd();

    glPolygonOffset(0, -4.5);
    glColor3f(1, 1, 1); // front
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(0.1,0.5);
    glVertex2d(0.025,0.5);
    glVertex2d(0.025,0.75);
    glVertex2d(0.27,0.9);
    glVertex2d(0.27,0.1);
    glVertex2d(0.025,0.25);
    glVertex2d(0.025,0.5);
    glEnd();

    glColor3f(0, 0, 0); // front glass
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(0.35,0.5);
    glVertex2d(0.3,0.5);
    glVertex2d(0.3,0.9);
    glVertex2d(0.43,0.8);
    glVertex2d(0.43,0.2);
    glVertex2d(0.3,0.1);
    glVertex2d(0.3,0.5);
    glEnd();

    glBegin(GL_TRIANGLE_FAN); // back glass
    glVertex2d(0.92,0.5);
    glVertex2d(0.90,0.5);
    glVertex2d(0.90,0.8);
    glVertex2d(0.95,0.9);
    glVertex2d(0.95,0.1);
    glVertex2d(0.90,0.2);
    glVertex2d(0.90,0.5);
    glEnd();

    glBegin(GL_TRIANGLE_FAN); // right glass
    glVertex2d(0.36,0.07);
    glVertex2d(0.34,0.03);
    glVertex2d(0.94,0.03);
    glVertex2d(0.87,0.13);
    glVertex2d(0.45,0.13);
    glVertex2d(0.34,0.03);
    glEnd();

    glBegin(GL_TRIANGLE_FAN); // left glass
    glVertex2d(0.36,1.-0.07);
    glVertex2d(0.34,1.-0.03);
    glVertex2d(0.94,1.-0.03);
    glVertex2d(0.87,1.-0.13);
    glVertex2d(0.45,1.-0.13);
    glVertex2d(0.34,1.-0.03);
    glEnd();
    */

    glPopMatrix();
}


#define BLINKER_POS_FRONT .5
#define BLINKER_POS_BACK .5

inline void
drawAction_drawVehicleBlinker(const GUIVehicle &veh) {
    double dir = (double) veh.getCORNIntValue(MSCORN::CORN_VEH_BLINKER)*veh.getVehicleType().getGuiWidth()*.5;
    if (dir==0) {
        return;
    }
    if (veh.getVehicleType().getGuiWidth()<.5) {
        return;
    }
    glColor3f(1.f, .8f, 0);
    glPushMatrix();
    glTranslated(dir, BLINKER_POS_FRONT+veh.getVehicleType().getGuiOffset(), 0);
    GLHelper::drawFilledCircle(.5, 6);
    glPopMatrix();
    glPushMatrix();
    glTranslated(dir, veh.getVehicleType().getLength()-BLINKER_POS_BACK, 0);
    GLHelper::drawFilledCircle(.5, 6);
    glPopMatrix();
}


inline void
drawAction_drawVehicleName(const GUIVehicle &veh, SUMOReal size) {
    glPushMatrix();
    glTranslated(0, veh.getVehicleType().getLength() / 2., 0);
    glTranslated(0, 0, -.07);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    pfSetPosition(0, 0);
    pfSetScale(size);
    SUMOReal w = pfdkGetStringWidth(veh.getMicrosimID().c_str());
    glRotated(180, 0, 1, 0);
    glTranslated(-w/2., 0.4, 0);
    pfDrawString(veh.getMicrosimID().c_str());
    glPopMatrix();
}


void
GUIVehicle::drawGL(const GUIVisualizationSettings &s) const throw() {
    glPushMatrix();
    Position2D p1 = myLane->getShape().positionAtLengthPosition(myState.pos());
    Position2D p2 = myFurtherLanes.size()>0 
        ? myFurtherLanes.front()->getShape().positionAtLengthPosition(myFurtherLanes.front()->getPartialOccupatorEnd())
        : myLane->getShape().positionAtLengthPosition(myState.pos()-myType->getLength());
    glTranslated(p1.x(), p1.y(), 0);
    glRotated(atan2(p1.x()-p2.x(), p2.y()-p1.y())*180./PI, 0, 0, 1);

    glTranslated(0, 0, -.04);
    // set lane color
    s.vehicleColorer.setGlColor(*this);
    // (optional) set id
    if (s.needsGlID) {
        glPushName(getGlID());
    }
    /*
        MSLCM_DK2004 &m2 = static_cast<MSLCM_DK2004&>(veh->getLaneChangeModel());
        if((m2.getState()&LCA_URGENT)!=0) {
            glColor3f(1, .4, .4);
        } else if((m2.getState()&LCA_SPEEDGAIN)!=0) {
            glColor3f(.4, .4, 1);
        } else {
            glColor3f(.4, 1, .4);
        }
        */
    // draw the vehicle
    SUMOReal upscale = s.vehicleExaggeration;
    switch (s.vehicleQuality) {
    case 0:
        drawAction_drawVehicleAsTrianglePlus(*this, upscale);
        break;
    case 1:
        drawAction_drawVehicleAsBoxPlus(*this, upscale);
        break;
    case 2:
    default:
        drawAction_drawVehicleAsPoly(*this, upscale);
        break;
    }
    glTranslated(0, 0, .04);
    // draw the blinker if wished
    if (s.showBlinker) {
        glTranslated(0, 0, -.05);
        drawAction_drawVehicleBlinker(*this);
        glTranslated(0, 0, .05);
    }
    // draw the wish to change the lane
    if (s.drawLaneChangePreference) {
        /*
                if(gSelected.isSelected(GLO_VEHICLE, veh->getGlID())) {
                MSLCM_DK2004 &m = static_cast<MSLCM_DK2004&>(veh->getLaneChangeModel());
                glColor3f(.5, .5, 1);
                glBegin(GL_LINES);
                glVertex2f(0, 0);
                glVertex2f(m.getChangeProbability(), .5);
                glEnd();

                glColor3f(1, 0, 0);
                glBegin(GL_LINES);
                glVertex2f(0.1, 0);
                glVertex2f(0.1, m.myMaxJam1);
                glEnd();

                glColor3f(0, 1, 0);
                glBegin(GL_LINES);
                glVertex2f(-0.1, 0);
                glVertex2f(-0.1, m.myTDist);
                glEnd();
                }
                */
    }
    // draw best lanes
    if (true) {
        /*
        const MSLane &l = veh->getLane();
        SUMOReal r1 = veh->allowedContinuationsLength(&l, 0);
        SUMOReal r2 = l.getLeftLane()!=0 ? veh->allowedContinuationsLength(l.getLeftLane(), 0) : 0;
        SUMOReal r3 = l.getRightLane()!=0 ? veh->allowedContinuationsLength(l.getRightLane(), 0) : 0;
        SUMOReal mmax = MAX3(r1, r2, r3);
        glBegin(GL_LINES);
        glVertex2f(0, 0);
        glVertex2f(0, r1/mmax/2.);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(.4, 0);
        glVertex2f(.4, r2/mmax/2.);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(-.4, 0);
        glVertex2f(-.4, r3/mmax/2.);
        glEnd();
        */
    }
    if (s.drawVehicleName) {
        glTranslated(0, 0, -.06);
        // compute name colors
        glColor3f(s.vehicleNameColor.red(), s.vehicleNameColor.green(), s.vehicleNameColor.blue());
        drawAction_drawVehicleName(*this, s.vehicleNameSize / s.scale);
        glTranslated(0, 0, .06);
    }
    // (optional) clear id
    if (s.needsGlID) {
        glPopName();
    }
    glPopMatrix();
}


const std::vector<MSVehicle::LaneQ> &
GUIVehicle::getBestLanes() const throw() {
    myLock.lock();
    const std::vector<MSVehicle::LaneQ> &ret = MSVehicle::getBestLanes();
    myLock.unlock();
    return ret;
}


void
GUIVehicle::setBlinkerInformation() {
    if (hasCORNIntValue(MSCORN::CORN_VEH_BLINKER)) {
        int blinker = 0;
        int state = getLaneChangeModel().getState();
        if ((state&LCA_LEFT)!=0) {
            blinker = 1;
        } else if ((state&LCA_RIGHT)!=0) {
            blinker = -1;
        } else {
            const MSLane &lane = getLane();
            MSLinkCont::const_iterator link = lane.succLinkSec(*this, 1, lane, getBestLanesContinuation());
            if (link!=lane.getLinkCont().end()&&lane.getLength()-getPositionOnLane()<lane.getMaxSpeed()*(SUMOReal) 7.) {
                switch ((*link)->getDirection()) {
                case MSLink::LINKDIR_TURN:
                case MSLink::LINKDIR_LEFT:
                case MSLink::LINKDIR_PARTLEFT:
                    blinker = 1;
                    break;
                case MSLink::LINKDIR_RIGHT:
                case MSLink::LINKDIR_PARTRIGHT:
                    blinker = -1;
                    break;
                default:
                    break;
                }
            }
        }
        myIntCORNMap[MSCORN::CORN_VEH_BLINKER] = blinker;
    }
}


void
GUIVehicle::initShapes() throw() {
}


GUIVehicle::Colorer::Colorer() {
    mySchemes.push_back(GUIColorScheme("uniform", RGBColor(1,1,0), "", true));
    mySchemes.push_back(GUIColorScheme("given/assigned vehicle color", RGBColor(1,1,0), "", true));
    mySchemes.push_back(GUIColorScheme("given/assigned type color", RGBColor(1,1,0), "", true));
    mySchemes.push_back(GUIColorScheme("given/assigned route color", RGBColor(1,1,0), "", true));
    mySchemes.push_back(GUIColorScheme("depart position as HSV", RGBColor(1,1,0), "", true));
    mySchemes.push_back(GUIColorScheme("arrival position as HSV", RGBColor(1,1,0), "", true));
    mySchemes.push_back(GUIColorScheme("direction/distance as HSV", RGBColor(1,1,0), "", true));
    mySchemes.push_back(GUIColorScheme("by speed", RGBColor(1,0,0)));
    mySchemes.back().addColor(RGBColor(0,0,1), (SUMOReal)(150.0/3.6));
    mySchemes.push_back(GUIColorScheme("by waiting time", RGBColor(0,0,1)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)(5*60));
    mySchemes.push_back(GUIColorScheme("by time since last lanechange", RGBColor(1,1,1)));
    mySchemes.back().addColor(RGBColor(.5,.5,.5), (SUMOReal)(5*60));
    mySchemes.push_back(GUIColorScheme("by max speed", RGBColor(1,0,0)));
    mySchemes.back().addColor(RGBColor(0,0,1), (SUMOReal)(150.0/3.6));
    // ... emissions ...
    mySchemes.push_back(GUIColorScheme("by CO2 emissions (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)5.);
    mySchemes.push_back(GUIColorScheme("by CO emissions (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)0.05);
    mySchemes.push_back(GUIColorScheme("by PMx emissions (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal).005);
    mySchemes.push_back(GUIColorScheme("by NOx emissions (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal).125);
    mySchemes.push_back(GUIColorScheme("by HC emissions (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal).02);
    mySchemes.push_back(GUIColorScheme("by fuel consumption (HBEFA)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal).005);
    mySchemes.push_back(GUIColorScheme("by noise emissions (Harmonoise)", RGBColor(0,1,0)));
    mySchemes.back().addColor(RGBColor(1,0,0), (SUMOReal)100.);
    mySchemes.push_back(GUIColorScheme("by reroute number", RGBColor(1,0,0)));
    mySchemes.back().addColor(RGBColor(1,1,0), (SUMOReal)1.);
    mySchemes.back().addColor(RGBColor(1,1,1), (SUMOReal)10.);
}


bool
GUIVehicle::Colorer::setFunctionalColor(const GUIVehicle& vehicle) const {
    switch (myActiveScheme) {
    case 1:
        vehicle.setOwnDefinedColor();
        return true;
    case 2:
        vehicle.setOwnTypeColor();
        return true;
    case 3:
        vehicle.setOwnRouteColor();
        return true;
    case 4: {
        Position2D p = vehicle.getRoute().getEdges()[0]->getLanes()[0]->getShape()[0];
        const Boundary &b = ((GUINet*) MSNet::getInstance())->getBoundary();
        Position2D center = b.getCenter();
        SUMOReal hue = 180. + atan2(center.x()-p.x(), center.y()-p.y()) * 180. / PI;
        SUMOReal sat = p.distanceTo(center) / center.distanceTo(Position2D(b.xmin(), b.ymin()));
        RGBColor c = RGBColor::fromHSV(hue, sat, 1.);
        glColor3f(c.red(), c.green(), c.blue());
        return true;
    }
    case 5: {
        Position2D p = vehicle.getRoute().getEdges().back()->getLanes()[0]->getShape()[-1];
        const Boundary &b = ((GUINet*) MSNet::getInstance())->getBoundary();
        Position2D center = b.getCenter();
        SUMOReal hue = 180. + atan2(center.x()-p.x(), center.y()-p.y()) * 180. / PI;
        SUMOReal sat = p.distanceTo(center) / center.distanceTo(Position2D(b.xmin(), b.ymin()));
        RGBColor c = RGBColor::fromHSV(hue, sat, 1.);
        glColor3f(c.red(), c.green(), c.blue());
        return true;
    }
    case 6: {
        Position2D pb = vehicle.getRoute().getEdges()[0]->getLanes()[0]->getShape()[0];
        Position2D pe = vehicle.getRoute().getEdges().back()->getLanes()[0]->getShape()[-1];
        const Boundary &b = ((GUINet*) MSNet::getInstance())->getBoundary();
        SUMOReal hue = 180. + atan2(pb.x()-pe.x(), pb.y()-pe.y()) * 180. / PI;
        Position2D minp(b.xmin(), b.ymin());
        Position2D maxp(b.xmax(), b.ymax());
        SUMOReal sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
        RGBColor c = RGBColor::fromHSV(hue, sat, 1.);
        glColor3f(c.red(), c.green(), c.blue());
        return true;
    }
    }
    return false;
}


SUMOReal
GUIVehicle::Colorer::getColorValue(const GUIVehicle& vehicle) const {
    switch (myActiveScheme) {
    case 7:
        return vehicle.getSpeed();
    case 8:
        return vehicle.getWaitingTime();
    case 9:
        return vehicle.getLastLaneChangeOffset();
    case 10:
        return vehicle.getMaxSpeed();
    case 11:
        return vehicle.getHBEFA_CO2Emissions();
    case 12:
        return vehicle.getHBEFA_COEmissions();
    case 13:
        return vehicle.getHBEFA_PMxEmissions();
    case 14:
        return vehicle.getHBEFA_NOxEmissions();
    case 15:
        return vehicle.getHBEFA_HCEmissions();
    case 16:
        return vehicle.getHBEFA_FuelConsumption();
    case 17:
        return vehicle.getHarmonoise_NoiseEmissions();
    case 18:
        if (!vehicle.hasCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE)) {
            return -1;
        }
        return vehicle.getCORNIntValue(MSCORN::CORN_VEH_NUMBERROUTE);
    }
    return 0;
}


/****************************************************************************/

