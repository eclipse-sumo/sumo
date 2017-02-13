/****************************************************************************/
/// @file    GUIBaseVehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <foreign/polyfonts/polyfonts.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeomHelper.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_BTreceiver.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include "GUIBaseVehicle.h"
#include "GUIPerson.h"
#include "GUIContainer.h"
#include "GUINet.h"
#include "GUIEdge.h"
#include "GUILane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIBaseVehicle::GUIBaseVehiclePopupMenu) GUIBaseVehiclePopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_ALLROUTES, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_ALLROUTES, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_CURRENTROUTE, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_CURRENTROUTE, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_BEST_LANES, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_BEST_LANES, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_START_TRACK, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdStartTrack),
    FXMAPFUNC(SEL_COMMAND, MID_STOP_TRACK, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdStopTrack),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_LFLINKITEMS, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowLFLinkItems),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_LFLINKITEMS, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideLFLinkItems),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_FOES, GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowFoes),
};

// Object implementation
FXIMPLEMENT(GUIBaseVehicle::GUIBaseVehiclePopupMenu, GUIGLObjectPopupMenu, GUIBaseVehiclePopupMenuMap, ARRAYNUMBER(GUIBaseVehiclePopupMenuMap))



// ===========================================================================
// data definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * drawed shapes
 * ----------------------------------------------------------------------- */
double vehiclePoly_PassengerCarBody[] = { .5, 0,  0, 0,  0, .3,  0.08, .44,  0.25, .5,  0.95, .5,  1., .4,  1., -.4,  0.95, -.5,  0.25, -.5,  0.08, -.44,  0, -.3,  0, 0,  -10000 };
double vehiclePoly_PassengerCarBodyFront[] = { 0.1, 0,  0.025, 0,  0.025, 0.25,  0.27, 0.4,  0.27, -.4,  0.025, -0.25,  0.025, 0,  -10000 };
double vehiclePoly_PassengerFrontGlass[] = { 0.35, 0,  0.3, 0,  0.3, 0.4,  0.43, 0.3,  0.43, -0.3,  0.3, -0.4,  0.3, 0,  -10000 };
double vehiclePoly_PassengerSedanRightGlass[] = { 0.36, -.43,  0.34, -.47,  0.77, -.47,  0.67, -.37,  0.45, -.37,  0.34, -.47,  -10000 };
double vehiclePoly_PassengerSedanLeftGlass[] = { 0.36, .43,  0.34, .47,  0.77, .47,  0.67, .37,  0.45, .37,  0.34, .47,  -10000 };
double vehiclePoly_PassengerSedanBackGlass[] = { 0.80, 0,  0.70, 0,  0.70, 0.3,  0.83, 0.4,  0.83, -.4,  0.70, -.3,  0.70, 0,  -10000 };
double vehiclePoly_PassengerHatchbackRightGlass[] = { 0.36, -.43,  0.34, -.47,  0.94, -.47,  0.80, -.37,  0.45, -.37,  0.34, -.47,  -10000 };
double vehiclePoly_PassengerHatchbackLeftGlass[] = { 0.36, .43,  0.34, .47,  0.94, .47,  0.80, .37,  0.45, .37,  0.34, .47,  -10000 };
double vehiclePoly_PassengerHatchbackBackGlass[] = { 0.92, 0,  0.80, 0,  0.80, 0.3,  0.95, 0.4,  0.95, -.4,  0.80, -.3,  0.80, 0,  -10000 };
double vehiclePoly_PassengerWagonRightGlass[] = { 0.36, -.43,  0.34, -.47,  0.94, -.47,  0.87, -.37,  0.45, -.37,  0.34, -.47,  -10000 };
double vehiclePoly_PassengerWagonLeftGlass[] = { 0.36, .43,  0.34, .47,  0.94, .47,  0.87, .37,  0.45, .37,  0.34, .47,  -10000 };
double vehiclePoly_PassengerWagonBackGlass[] = { 0.92, 0,  0.90, 0,  0.90, 0.3,  0.95, 0.4,  0.95, -.4,  0.90, -.3,  0.90, 0,  -10000 };

double vehiclePoly_PassengerVanBody[] = { .5, 0,  0, 0,  0, .4,  0.1, .5,  0.97, .5,  1., .47,  1., -.47,  0.97, -.5,  0.1, -.5,  0, -.4,  0, 0,  -10000 };
double vehiclePoly_PassengerVanBodyFront[] = { 0.1, 0,  0.025, 0,  0.025, 0.25,  0.13, 0.4,  0.13, -.4,  0.025, -0.25,  0.025, 0,  -10000 };
double vehiclePoly_PassengerVanFrontGlass[] = { 0.21, 0,  0.16, 0,  0.16, 0.4,  0.29, 0.3,  0.29, -0.3,  0.16, -0.4,  0.16, 0,  -10000 };
double vehiclePoly_PassengerVanRightGlass[] = { 0.36, -.43,  0.20, -.47,  0.98, -.47,  0.91, -.37,  0.31, -.37,  0.20, -.47,  -10000 };
double vehiclePoly_PassengerVanLeftGlass[] = { 0.36, .43,  0.20, .47,  0.98, .47,  0.91, .37,  0.31, .37,  0.20, .47,  -10000 };
double vehiclePoly_PassengerVanBackGlass[] = { 0.95, 0,  0.94, 0,  0.94, 0.3,  0.98, 0.4,  0.98, -.4,  0.94, -.3,  0.94, 0,  -10000 };

double vehiclePoly_DeliveryMediumRightGlass[] = { 0.21, -.43,  0.20, -.47,  0.38, -.47,  0.38, -.37,  0.31, -.37,  0.20, -.47,  -10000 };
double vehiclePoly_DeliveryMediumLeftGlass[] = { 0.21, .43,  0.20, .47,  0.38, .47,  0.38, .37,  0.31, .37,  0.20, .47,  -10000 };

double vehiclePoly_TransportBody[] = { .5, 0,  0, 0,  0, .45,  0.05, .5,  2.25, .5,  2.25, -.5,  0.05, -.5,  0, -.45,  0, 0,  -10000 };
double vehiclePoly_TransportFrontGlass[] = { 0.1, 0,  0.05, 0,  0.05, 0.45,  0.25, 0.4,  0.25, -.4,  0.05, -0.45,  0.05, 0,  -10000 };
double vehiclePoly_TransportRightGlass[] = { 0.36, -.47,  0.10, -.48,  1.25, -.48,  1.25, -.4,  0.3, -.4,  0.10, -.48,  -10000 };
double vehiclePoly_TransportLeftGlass[] = { 0.36, .47,  0.10, .48,  1.25, .48,  1.25, .4,  0.3, .4,  0.10, .48,  -10000 };

double vehiclePoly_EVehicleBody[] = { .5, 0,  0, 0,  0, .3,  0.08, .44,  0.25, .5,  0.75, .5,  .92, .44,  1, .3,  1, -.3,  .92, -.44,  .75, -.5,  .25, -.5,  0.08, -.44,  0, -.3,  0, 0,  -1000 };
double vehiclePoly_EVehicleFrontGlass[] = { .5, 0,  0.05, .05,  0.05, .25,  0.13, .39,  0.3, .45,  0.70, .45,  .87, .39,  .95, .25,  .95, -.25,  .87, -.39,  .70, -.45,  .3, -.45,  0.13, -.39,  0.05, -.25,  0.05, 0.05,  -1000 };
//double vehiclePoly_EVehicleFrontGlass[] = { 0.35,0,  0.1,0,  0.1,0.4,  0.43,0.3,  0.43,-0.3,  0.1,-0.4,  0.1,0,  -10000 };
double vehiclePoly_EVehicleBackGlass[] =  { 0.65, 0,  0.9, 0,  0.9, 0.4,  0.57, 0.3,  0.57, -0.3,  0.9, -0.4,  0.9, 0,  -10000 };

double vehiclePoly_Ship[] =  { 0.25, 0,  0, 0,  0.1, 0.25, 0.2, 0.45, 0.25, 0.5,  0.95, 0.5, 1.0, 0.45,   1.0, -0.45, 0.95, -0.5,  0.25, -0.5, 0.2, -0.45,  0.1, -0.25, 0, 0,   -10000 };
double vehiclePoly_ShipDeck[] =  { 0.5, 0,  0.25, 0.4,  0.95, 0.4, 0.95, -0.4, 0.25, -0.4, 0.25, 0.4, -10000 };
double vehiclePoly_ShipSuperStructure[] =  { 0.8, 0,  0.5, 0.3,  0.85, 0.3,  0.85, -0.3, 0.5, -0.3,  0.5, 0.3,  -10000 };

double vehiclePoly_Cyclist[] =  { 0.5, 0,  0.25, 0.45,  0.25, 0.5, 0.8, 0.15,     0.8, -0.15, 0.25, -0.5, 0.25, -0.45,     -10000 };

double vehiclePoly_EmergencySign[] =   { .2, .5,  -.2, .5,  -.2, -.5,  .2, -.5, -10000 };
double vehiclePoly_Emergency[] =   { .1, .1,  -.1, .1,  -.1, -.1,  .1, -.1, -10000 };
double vehiclePoly_Emergency2[] =   { .04, .3,  -.04, .3,  -.04, -.3,  .04, -.3, -10000 };

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIBaseVehicle::GUIBaseVehiclePopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIBaseVehicle::GUIBaseVehiclePopupMenu::GUIBaseVehiclePopupMenu(
    GUIMainWindow& app, GUISUMOAbstractView& parent,
    GUIGlObject& o, std::map<GUISUMOAbstractView*, int>& additionalVisualizations)
    : GUIGLObjectPopupMenu(app, parent, o), myVehiclesAdditionalVisualizations(additionalVisualizations) {
}


GUIBaseVehicle::GUIBaseVehiclePopupMenu::~GUIBaseVehiclePopupMenu() {}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowAllRoutes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_ALL_ROUTES)) {
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_ALL_ROUTES);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideAllRoutes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_ALL_ROUTES);
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowCurrentRoute(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_ROUTE)) {
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_ROUTE);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideCurrentRoute(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_ROUTE);
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowBestLanes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_BEST_LANES)) {
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_BEST_LANES);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideBestLanes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_BEST_LANES);
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdStartTrack(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_TRACKED)) {
        myParent->startTrack(static_cast<GUIBaseVehicle*>(myObject)->getGlID());
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_TRACKED);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdStopTrack(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_TRACKED);
    myParent->stopTrack();
    return 1;
}


long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowLFLinkItems(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    if (!static_cast<GUIBaseVehicle*>(myObject)->hasActiveAddVisualisation(myParent, VO_SHOW_LFLINKITEMS)) {
        static_cast<GUIBaseVehicle*>(myObject)->addActiveAddVisualisation(myParent, VO_SHOW_LFLINKITEMS);
    }
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdHideLFLinkItems(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->removeActiveAddVisualisation(myParent, VO_SHOW_LFLINKITEMS);
    return 1;
}

long
GUIBaseVehicle::GUIBaseVehiclePopupMenu::onCmdShowFoes(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_VEHICLE);
    static_cast<GUIBaseVehicle*>(myObject)->selectBlockingFoes();
    myParent->update();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIBaseVehicle - methods
 * ----------------------------------------------------------------------- */

GUIBaseVehicle::GUIBaseVehicle(MSBaseVehicle& vehicle) :
    GUIGlObject(GLO_VEHICLE, vehicle.getID()),
    myVehicle(vehicle) {
    // as it is possible to show all vehicle routes, we have to store them... (bug [ 2519761 ])
    myRoutes = MSDevice_Vehroutes::buildVehicleDevices(myVehicle, myVehicle.myDevices, 5);
    myVehicle.myMoveReminders.push_back(std::make_pair(myRoutes, 0.));
    mySeatPositions.push_back(Position(0, 0)); // ensure length 1
}


GUIBaseVehicle::~GUIBaseVehicle() {
    myLock.lock();
    for (std::map<GUISUMOAbstractView*, int>::iterator i = myAdditionalVisualizations.begin(); i != myAdditionalVisualizations.end(); ++i) {
        while (i->first->removeAdditionalGLVisualisation(this));
    }
    myLock.unlock();
    GLObjectValuePassConnector<SUMOReal>::removeObject(*this);
    delete myRoutes;
}


GUIGLObjectPopupMenu*
GUIBaseVehicle::getPopUpMenu(GUIMainWindow& app,
                             GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIBaseVehiclePopupMenu(app, parent, *this, myAdditionalVisualizations);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
    if (hasActiveAddVisualisation(&parent, VO_SHOW_ROUTE)) {
        new FXMenuCommand(ret, "Hide Current Route", 0, ret, MID_HIDE_CURRENTROUTE);
    } else {
        new FXMenuCommand(ret, "Show Current Route", 0, ret, MID_SHOW_CURRENTROUTE);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_ALL_ROUTES)) {
        new FXMenuCommand(ret, "Hide All Routes", 0, ret, MID_HIDE_ALLROUTES);
    } else {
        new FXMenuCommand(ret, "Show All Routes", 0, ret, MID_SHOW_ALLROUTES);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_BEST_LANES)) {
        new FXMenuCommand(ret, "Hide Best Lanes", 0, ret, MID_HIDE_BEST_LANES);
    } else {
        new FXMenuCommand(ret, "Show Best Lanes", 0, ret, MID_SHOW_BEST_LANES);
    }
    if (hasActiveAddVisualisation(&parent, VO_SHOW_LFLINKITEMS)) {
        new FXMenuCommand(ret, "Hide Link Items", 0, ret, MID_HIDE_LFLINKITEMS);
    } else {
        new FXMenuCommand(ret, "Show Link Items", 0, ret, MID_SHOW_LFLINKITEMS);
    }
    new FXMenuSeparator(ret);
    if (parent.getTrackedID() != getGlID()) {
        new FXMenuCommand(ret, "Start Tracking", 0, ret, MID_START_TRACK);
    } else {
        new FXMenuCommand(ret, "Stop Tracking", 0, ret, MID_STOP_TRACK);
    }
    new FXMenuCommand(ret, "Select Foes", 0, ret, MID_SHOW_FOES);

    new FXMenuSeparator(ret);
    //
    buildShowParamsPopupEntry(ret, false);
    buildShowTypeParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


Boundary
GUIBaseVehicle::getCenteringBoundary() const {
    Boundary b;
    b.add(getPosition());
    b.grow(myVehicle.getVehicleType().getLength());
    return b;
}


void
GUIBaseVehicle::drawAction_drawVehicleAsBoxPlus() const {
    glPushMatrix();
    glScaled(getVType().getWidth(), getVType().getLength(), 1.);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2d(0., 0.);
    glVertex2d(-.5, .15);
    glVertex2d(.5, .15);
    glVertex2d(-.5, 1.);
    glVertex2d(.5, 1.);
    glEnd();
    glPopMatrix();
}


void
GUIBaseVehicle::drawAction_drawVehicleAsTrianglePlus() const {
    const SUMOReal length = getVType().getLength();
    if (length >= 8.) {
        drawAction_drawVehicleAsBoxPlus();
        return;
    }
    glPushMatrix();
    glScaled(getVType().getWidth(), length, 1.);
    glBegin(GL_TRIANGLES);
    glVertex2d(0., 0.);
    glVertex2d(-.5, 1.);
    glVertex2d(.5, 1.);
    glEnd();
    glPopMatrix();
}


void
GUIBaseVehicle::drawPoly(double* poses, SUMOReal offset) {
    glPushMatrix();
    glTranslated(0, 0, offset * .1);
    glPolygonOffset(0, (GLfloat) - offset);
    glBegin(GL_TRIANGLE_FAN);
    int i = 0;
    while (poses[i] > -999) {
        glVertex2d(poses[i], poses[i + 1]);
        i = i + 2;
    }
    glEnd();
    glPopMatrix();
}


void
GUIBaseVehicle::drawAction_drawVehicleAsPoly(const GUIVisualizationSettings& s) const {
    RGBColor current = GLHelper::getColor();
    RGBColor lighter = current.changedBrightness(51);
    RGBColor darker = current.changedBrightness(-51);

    const SUMOReal length = getVType().getLength();
    const SUMOReal width = getVType().getWidth();
    glPushMatrix();
    glRotated(90, 0, 0, 1);
    glScaled(length, width, 1.);
    SUMOVehicleShape shape = getVType().getGuiShape();

    // draw main body
    switch (shape) {
        case SVS_UNKNOWN:
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerCarBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerFrontGlass, 4.5);
            break;
        case SVS_PEDESTRIAN:
            glTranslated(0, 0, .045);
            GLHelper::drawFilledCircle(0.25);
            glTranslated(0, 0, -.045);
            glScaled(.7, 2, 1);
            glTranslated(0, 0, .04);
            GLHelper::setColor(lighter);
            GLHelper::drawFilledCircle(0.25);
            glTranslated(0, 0, -.04);
            break;
        case SVS_BICYCLE:
        case SVS_MOPED:
        case SVS_MOTORCYCLE: {
            RGBColor darker = current.changedBrightness(-50);
            // body
            drawPoly(vehiclePoly_Cyclist, 4);
            // head
            glPushMatrix();
            glTranslated(0.4, 0, .5);
            glScaled(0.1, 0.2, 1);
            GLHelper::setColor(darker);
            GLHelper::drawFilledCircle(1);
            glPopMatrix();
            // bike frame
            GLHelper::setColor(RGBColor::GREY);
            glPushMatrix();
            glTranslated(0.5, 0, .3);
            glScaled(0.5, 0.05, 1);
            GLHelper::drawFilledCircle(1);
            glPopMatrix();
            // handle bar
            glPushMatrix();
            glTranslated(0.25, 0, .3);
            glScaled(0.02, 0.5, 1);
            GLHelper::drawFilledCircle(1);
            glPopMatrix();
        }
        break;
        case SVS_PASSENGER:
        case SVS_PASSENGER_SEDAN:
        case SVS_PASSENGER_HATCHBACK:
        case SVS_PASSENGER_WAGON:
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerCarBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerFrontGlass, 4.5);
            break;
        case SVS_PASSENGER_VAN:
            drawPoly(vehiclePoly_PassengerVanBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerVanBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerVanFrontGlass, 4.5);
            drawPoly(vehiclePoly_PassengerVanRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerVanLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerVanBackGlass, 4.5);
            break;
        case SVS_DELIVERY:
            drawPoly(vehiclePoly_PassengerVanBody, 4);
            GLHelper::setColor(lighter);
            drawPoly(vehiclePoly_PassengerVanBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerVanFrontGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumRightGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumLeftGlass, 4.5);
            break;
        case SVS_TRUCK:
        case SVS_TRUCK_SEMITRAILER:
        case SVS_TRUCK_1TRAILER:
            glScaled(1. / (length), 1, 1.);
            drawPoly(vehiclePoly_TransportBody, 4);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_TransportFrontGlass, 4.5);
            drawPoly(vehiclePoly_TransportRightGlass, 4.5);
            drawPoly(vehiclePoly_TransportLeftGlass, 4.5);
            break;
        case SVS_BUS:
        case SVS_BUS_COACH:
        case SVS_BUS_TROLLEY: {
            SUMOReal ml = length;
            glScaled(1. / (length), 1, 1.);
            glTranslated(0, 0, .04);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(ml / 2., 0);
            glVertex2d(0, 0);
            glVertex2d(0, -.45);
            glVertex2d(0 + .05, -.5);
            glVertex2d(ml - .05, -.5);
            glVertex2d(ml, -.45);
            glVertex2d(ml, .45);
            glVertex2d(ml - .05, .5);
            glVertex2d(0 + .05, .5);
            glVertex2d(0, .45);
            glVertex2d(0, 0);
            glEnd();
            glTranslated(0, 0, -.04);

            glTranslated(0, 0, .045);
            glColor3d(0, 0, 0);
            glBegin(GL_QUADS);
            glVertex2d(0 + .05, .48);
            glVertex2d(0 + .05, -.48);
            glVertex2d(0 + .15, -.48);
            glVertex2d(0 + .15, .48);

            glVertex2d(ml - .1, .45);
            glVertex2d(ml - .1, -.45);
            glVertex2d(ml - .05, -.45);
            glVertex2d(ml - .05, .45);

            glVertex2d(0 + .20, .49);
            glVertex2d(0 + .20, .45);
            glVertex2d(ml - .20, .45);
            glVertex2d(ml - .20, .49);

            glVertex2d(0 + .20, -.49);
            glVertex2d(0 + .20, -.45);
            glVertex2d(ml - .20, -.45);
            glVertex2d(ml - .20, -.49);

            glEnd();
            glTranslated(0, 0, -.045);
        }
        break;
        case SVS_BUS_FLEXIBLE:
        case SVS_RAIL:
        case SVS_RAIL_CAR:
        case SVS_RAIL_CARGO:
            drawAction_drawCarriageClass(s, shape, false);
            break;
        case SVS_E_VEHICLE:
            drawPoly(vehiclePoly_EVehicleBody, 4);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_EVehicleFrontGlass, 4.5);
            glTranslated(0, 0, .048);
            GLHelper::setColor(current);
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
            glTranslated(0, 0, -.048);
            //drawPoly(vehiclePoly_EVehicleBackGlass, 4.5);
            break;
        case SVS_ANT:
            glPushMatrix();
            // ant is stretched via vehicle length
            GLHelper::setColor(darker);
            // draw left side
            GLHelper::drawBoxLine(Position(-0.2, -.10), 350, 0.5, .02);
            GLHelper::drawBoxLine(Position(-0.3, -.50), 240, 0.4, .03);
            GLHelper::drawBoxLine(Position(0.3, -.10), 340, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.05, -.80), 290, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.4, -.10),  20, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.65, -.80),  75, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.5, -.10),  55, 0.8, .04);
            GLHelper::drawBoxLine(Position(1.1, -.55),  90, 0.6, .04);
            // draw right side
            GLHelper::drawBoxLine(Position(-0.2,  .10), 190, 0.5, .02);
            GLHelper::drawBoxLine(Position(-0.3,  .50), 300, 0.4, .03);
            GLHelper::drawBoxLine(Position(0.3,  .10), 200, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.05, .80), 250, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.4,  .10), 160, 0.8, .03);
            GLHelper::drawBoxLine(Position(0.65, .80), 105, 0.6, .04);
            GLHelper::drawBoxLine(Position(0.5,  .10), 125, 0.8, .04);
            GLHelper::drawBoxLine(Position(1.1,  .55),  90, 0.6, .04);
            // draw body
            GLHelper::setColor(current);
            glTranslated(0, 0, 0.1);
            GLHelper::drawFilledCircle(.25, 16);
            glTranslated(.4, 0, 0);
            GLHelper::drawFilledCircle(.2, 16);
            glTranslated(.4, 0, 0);
            GLHelper::drawFilledCircle(.3, 16);
            glPopMatrix();
            break;
        case SVS_SHIP: {
            RGBColor darker = current.changedBrightness(-30);
            RGBColor darker2 = current.changedBrightness(-70);
            drawPoly(vehiclePoly_Ship, 4);
            GLHelper::setColor(darker);
            drawPoly(vehiclePoly_ShipDeck, 5);
            GLHelper::setColor(darker2);
            drawPoly(vehiclePoly_ShipSuperStructure, 6);
            break;
        }
        case SVS_EMERGENCY: // similar to delivery
            glColor3d(1, 1, 1);
            drawPoly(vehiclePoly_PassengerVanBody, 4);
            glColor3d(1, 0, 0);
            drawPoly(vehiclePoly_PassengerVanBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerVanFrontGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumRightGlass, 4.5);
            drawPoly(vehiclePoly_DeliveryMediumLeftGlass, 4.5);
            // first aid sign
            glTranslated(0.7, 0, 0);
            glColor3d(.18, .55, .34);
            drawPoly(vehiclePoly_EmergencySign, 4.5);
            glColor3d(1, 1, 1);
            drawPoly(vehiclePoly_Emergency, 5);
            drawPoly(vehiclePoly_Emergency2, 5);
            break;
        default: // same as passenger
            drawPoly(vehiclePoly_PassengerCarBody, 4);
            glColor3d(1, 1, 1);
            drawPoly(vehiclePoly_PassengerCarBodyFront, 4.5);
            glColor3d(0, 0, 0);
            drawPoly(vehiclePoly_PassengerFrontGlass, 4.5);
            break;
    }

    // draw decorations
    switch (shape) {
        case SVS_PEDESTRIAN:
            break;
        case SVS_BICYCLE:
            //glScaled(length, 1, 1.);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(1 / 2., 0);
            glVertex2d(0, 0);
            glVertex2d(0, -.03);
            glVertex2d(0 + .05, -.05);
            glVertex2d(1 - .05, -.05);
            glVertex2d(1, -.03);
            glVertex2d(1, .03);
            glVertex2d(1 - .05, .05);
            glVertex2d(0 + .05, .05);
            glVertex2d(0, .03);
            glVertex2d(0, 0);
            glEnd();
            break;
        case SVS_MOPED:
        case SVS_MOTORCYCLE:
            //glScaled(length, 1, 1.);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2d(1 / 2., 0);
            glVertex2d(0, 0);
            glVertex2d(0, -.03);
            glVertex2d(0 + .05, -.2);
            glVertex2d(1 - .05, -.2);
            glVertex2d(1, -.03);
            glVertex2d(1, .03);
            glVertex2d(1 - .05, .2);
            glVertex2d(0 + .05, .2);
            glVertex2d(0, .03);
            glVertex2d(0, 0);
            glEnd();
            break;
        case SVS_PASSENGER:
        case SVS_PASSENGER_SEDAN:
            drawPoly(vehiclePoly_PassengerSedanRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanBackGlass, 4.5);
            break;
        case SVS_PASSENGER_HATCHBACK:
            drawPoly(vehiclePoly_PassengerHatchbackRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerHatchbackLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerHatchbackBackGlass, 4.5);
            break;
        case SVS_PASSENGER_WAGON:
            drawPoly(vehiclePoly_PassengerWagonRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerWagonLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerWagonBackGlass, 4.5);
            break;
        case SVS_PASSENGER_VAN:
        case SVS_DELIVERY:
            break;
        case SVS_TRUCK:
            GLHelper::setColor(current);
            GLHelper::drawBoxLine(Position(2.3, 0), 90., length - 2.3, .5);
            break;
        case SVS_TRUCK_SEMITRAILER:
            GLHelper::setColor(current);
            GLHelper::drawBoxLine(Position(2.8, 0), 90., length - 2.8, .5);
            break;
        case SVS_TRUCK_1TRAILER: {
            GLHelper::setColor(current);
            SUMOReal l = length - 2.3;
            l = l / 2.;
            GLHelper::drawBoxLine(Position(2.3, 0), 90., l, .5);
            GLHelper::drawBoxLine(Position(2.3 + l + .5, 0), 90., l - .5, .5);
            break;
        }
        case SVS_BUS_TROLLEY:
            glPushMatrix();
            glTranslated(0, 0, .1);
            GLHelper::setColor(darker);
            GLHelper::drawBoxLine(Position(3.8, 0), 90., 1, .3);
            glTranslated(0, 0, .1);
            glColor3d(0, 0, 0);
            GLHelper::drawBoxLine(Position(4.3, .2), 90., 1, .06);
            GLHelper::drawBoxLine(Position(4.3, -.2), 90., 1, .06);
            GLHelper::drawBoxLine(Position(5.3, .2), 90., 3, .03);
            GLHelper::drawBoxLine(Position(5.3, -.2), 90., 3, .03);
            glPopMatrix();
            break;
        case SVS_BUS:
        case SVS_BUS_COACH:
        case SVS_BUS_FLEXIBLE:
        case SVS_RAIL:
        case SVS_RAIL_CAR:
        case SVS_RAIL_CARGO:
        case SVS_E_VEHICLE:
        case SVS_ANT:
        case SVS_SHIP:
        case SVS_EMERGENCY:
            break;
        default: // same as passenger/sedan
            drawPoly(vehiclePoly_PassengerSedanRightGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanLeftGlass, 4.5);
            drawPoly(vehiclePoly_PassengerSedanBackGlass, 4.5);
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
    glColor3d(1, 1, 1); // front
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(0.1,0.5);
    glVertex2d(0.025,0.5);
    glVertex2d(0.025,0.75);
    glVertex2d(0.27,0.9);
    glVertex2d(0.27,0.1);
    glVertex2d(0.025,0.25);
    glVertex2d(0.025,0.5);
    glEnd();

    glColor3d(0, 0, 0); // front glass
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


bool
GUIBaseVehicle::drawAction_drawVehicleAsImage(const GUIVisualizationSettings& s, SUMOReal length) const {
    const std::string& file = getVType().getImgFile();
    if (file != "") {
        int textureID = GUITexturesHelper::getTextureID(file);
        if (textureID > 0) {
            const SUMOReal exaggeration = s.vehicleSize.getExaggeration(s);
            if (length < 0) {
                length = getVType().getLength() * exaggeration;
            }
            const SUMOReal halfWidth = getVType().getWidth() / 2.0 * exaggeration;
            GUITexturesHelper::drawTexturedBox(textureID, -halfWidth, 0, halfWidth, length);
            return true;
        }
    }
    return false;
}


void
GUIBaseVehicle::drawOnPos(const GUIVisualizationSettings& s, const Position& pos, const SUMOReal angle) const {
    glPushName(getGlID());
    glPushMatrix();
    Position p1 = pos;
    const SUMOReal degAngle = RAD2DEG(angle + PI / 2.);
    // one seat in the center of the vehicle by default
    if (myVehicle.getLane() != 0) {
        mySeatPositions[0] = myVehicle.getLane()->geometryPositionAtOffset(myVehicle.getPositionOnLane() - getVType().getLength() / 2);
    } else {
        mySeatPositions[0] = p1;
    }
    glTranslated(p1.x(), p1.y(), getType());
    glRotated(degAngle, 0, 0, 1);
    // set lane color
    setColor(s);
    // scale
    const SUMOReal upscale = s.vehicleSize.getExaggeration(s);
    glScaled(upscale, upscale, 1);
    /*
        MSLCM_DK2004 &m2 = static_cast<MSLCM_DK2004&>(veh->getLaneChangeModel());
        if((m2.getState()&LCA_URGENT)!=0) {
            glColor3d(1, .4, .4);
        } else if((m2.getState()&LCA_SPEEDGAIN)!=0) {
            glColor3d(.4, .4, 1);
        } else {
            glColor3d(.4, 1, .4);
        }
        */
    // draw the vehicle
    myCarriageLength = getVType().getLength();
    switch (s.vehicleQuality) {
        case 0:
            drawAction_drawVehicleAsTrianglePlus();
            break;
        case 1:
            drawAction_drawVehicleAsBoxPlus();
            break;
        case 2:
            drawAction_drawVehicleAsPoly(s);
            // draw flashing blue light for emergency vehicles
            if (getVType().getGuiShape() == SVS_EMERGENCY) {
                glTranslated(0, 0, .1);
                drawAction_drawVehicleBlueLight();
            }
            break;
        case 3:
        default:
            // draw as image but take special care for drawing trains
            // fallback to simple shapes
            if (!drawAction_drawCarriageClass(s, getVType().getGuiShape(), true)) {
                if (!drawAction_drawVehicleAsImage(s)) {
                    drawAction_drawVehicleAsPoly(s);
                };
            }
            break;
    }
    if (s.drawMinGap) {
        const SUMOReal minGap = -getVType().getMinGap();
        glColor3d(0., 1., 0.);
        glBegin(GL_LINES);
        glVertex2d(0., 0);
        glVertex2d(0., minGap);
        glVertex2d(-.5, minGap);
        glVertex2d(.5, minGap);
        glEnd();
    }
    MSDevice_BTreceiver* dev = static_cast<MSDevice_BTreceiver*>(myVehicle.getDevice(typeid(MSDevice_BTreceiver)));
    if (dev != 0 && s.showBTRange) {
        glColor3d(1., 0., 0.);
        GLHelper::drawOutlineCircle(dev->getRange(), dev->getRange() - .2, 32);
    }
    // draw the blinker and brakelights if wished
    if (s.showBlinker) {
        glTranslated(0, 0, .1);
        switch (getVType().getGuiShape()) {
            case SVS_PEDESTRIAN:
            case SVS_BICYCLE:
            case SVS_ANT:
            case SVS_SHIP:
            case SVS_RAIL:
            case SVS_RAIL_CARGO:
                // only SVS_RAIL_CAR has blinkers and brake lights
                break;
            case SVS_MOTORCYCLE:
            case SVS_MOPED:
                drawAction_drawVehicleBlinker(myCarriageLength);
                drawAction_drawVehicleBrakeLight(myCarriageLength, true);
                break;
            default:
                drawAction_drawVehicleBlinker(myCarriageLength);
                drawAction_drawVehicleBrakeLight(myCarriageLength);
                break;
        }
    }
    // draw the wish to change the lane
    if (s.drawLaneChangePreference) {
        /*
                if(gSelected.isSelected(GLO_VEHICLE, veh->getGlID())) {
                MSLCM_DK2004 &m = static_cast<MSLCM_DK2004&>(veh->getLaneChangeModel());
                glColor3d(.5, .5, 1);
                glBegin(GL_LINES);
                glVertex2f(0, 0);
                glVertex2f(m.getChangeProbability(), .5);
                glEnd();

                glColor3d(1, 0, 0);
                glBegin(GL_LINES);
                glVertex2f(0.1, 0);
                glVertex2f(0.1, m.myMaxJam1);
                glEnd();

                glColor3d(0, 1, 0);
                glBegin(GL_LINES);
                glVertex2f(-0.1, 0);
                glVertex2f(-0.1, m.myTDist);
                glEnd();
                }
                */
    }
    // draw best lanes
    /*
    if (true) {
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
    }
    */
    glTranslated(0, MIN2(getVType().getLength() / 2, SUMOReal(5)), -getType()); // drawing name at GLO_MAX fails unless translating z
    glRotated(-degAngle, 0, 0, 1);
    glScaled(1 / upscale, 1 / upscale, 1);
    drawName(Position(0, 0), s.scale,
             getVType().getGuiShape() == SVS_PEDESTRIAN ? s.personName : s.vehicleName);
    if (s.vehicleName.show && myVehicle.getParameter().line != "") {
        glTranslated(0, 0.6 * s.vehicleName.size / s.scale, 0);
        GLHelper::drawText("line:" + myVehicle.getParameter().line, Position(0, 0),
                           GLO_MAX, s.vehicleName.size / s.scale, s.vehicleName.color);
    }
    glPopMatrix();
    glPopName();
    drawAction_drawPersonsAndContainers(s);
}


void
GUIBaseVehicle::drawGL(const GUIVisualizationSettings& s) const {
    drawOnPos(s, getPosition(), getAngle());
}


void
GUIBaseVehicle::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    if (!myVehicle.isOnRoad()) {
        drawGL(s);
    }
    glPushName(getGlID());
    glPushMatrix();
    glTranslated(0, 0, getType() - .1); // don't draw on top of other cars
    if (hasActiveAddVisualisation(parent, VO_SHOW_BEST_LANES)) {
        drawBestLanes();
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_ROUTE)) {
        drawRoute(s, 0, 0.25);
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_ALL_ROUTES)) {
        if (myVehicle.getNumberReroutes() > 0) {
            const int noReroutePlus1 = myVehicle.getNumberReroutes() + 1;
            for (int i = noReroutePlus1 - 1; i >= 0; i--) {
                SUMOReal darken = SUMOReal(0.4) / SUMOReal(noReroutePlus1) * SUMOReal(i);
                drawRoute(s, i, darken);
            }
        } else {
            drawRoute(s, 0, 0.25);
        }
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_LFLINKITEMS)) {
        drawAction_drawLinkItems(s);
    }
    glPopMatrix();
    glPopName();
}


void
GUIBaseVehicle::drawLinkItem(const Position& pos, SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal exagerate) {
    glTranslated(pos.x(), pos.y(), -.1);
    GLHelper::drawFilledCircle(1);
    std::string times = toString(STEPS2TIME(arrivalTime)) + "/" + toString(STEPS2TIME(leaveTime));
    GLHelper::drawText(times.c_str(), Position(), .1, 1.6 * exagerate, RGBColor::GREEN, 0);
    glTranslated(-pos.x(), -pos.y(), .1);
}


void
GUIBaseVehicle::setColor(const GUIVisualizationSettings& s) const {
    const GUIColorer& c = s.vehicleColorer;
    if (!setFunctionalColor(c.getActive(), &myVehicle)) {
        GLHelper::setColor(c.getScheme().getColor(getColorValue(c.getActive())));
    }
}


bool
GUIBaseVehicle::setFunctionalColor(int activeScheme, const MSBaseVehicle* veh) {
    switch (activeScheme) {
        case 0: {
            if (veh->getParameter().wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(veh->getParameter().color);
                return true;
            }
            if (veh->getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(veh->getVehicleType().getColor());
                return true;
            }
            if (&(veh->getRoute().getColor()) != &RGBColor::DEFAULT_COLOR) {
                GLHelper::setColor(veh->getRoute().getColor());
                return true;
            }
            return false;
        }
        case 2: {
            if (veh->getParameter().wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(veh->getParameter().color);
                return true;
            }
            return false;
        }
        case 3: {
            if (veh->getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(veh->getVehicleType().getColor());
                return true;
            }
            return false;
        }
        case 4: {
            if (&(veh->getRoute().getColor()) != &RGBColor::DEFAULT_COLOR) {
                GLHelper::setColor(veh->getRoute().getColor());
                return true;
            }
            return false;
        }
        case 5: {
            Position p = veh->getRoute().getEdges()[0]->getLanes()[0]->getShape()[0];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            Position center = b.getCenter();
            SUMOReal hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / PI;
            SUMOReal sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
        case 6: {
            Position p = veh->getRoute().getEdges().back()->getLanes()[0]->getShape()[-1];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            Position center = b.getCenter();
            SUMOReal hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / PI;
            SUMOReal sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
        case 7: {
            Position pb = veh->getRoute().getEdges()[0]->getLanes()[0]->getShape()[0];
            Position pe = veh->getRoute().getEdges().back()->getLanes()[0]->getShape()[-1];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            SUMOReal hue = 180. + atan2(pb.x() - pe.x(), pb.y() - pe.y()) * 180. / PI;
            Position minp(b.xmin(), b.ymin());
            Position maxp(b.xmax(), b.ymax());
            SUMOReal sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
        case 28: { // color randomly (by pointer)
            const SUMOReal hue = (long)veh % 360; // [0-360]
            const SUMOReal sat = (((long)veh / 360) % 67) / 100.0 + 0.33; // [0.33-1]
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
    }
    return false;
}


// ------------ Additional visualisations
bool
GUIBaseVehicle::hasActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) const {
    return myAdditionalVisualizations.find(parent) != myAdditionalVisualizations.end() && (myAdditionalVisualizations.find(parent)->second & which) != 0;
}


void
GUIBaseVehicle::addActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) {
    if (myAdditionalVisualizations.find(parent) == myAdditionalVisualizations.end()) {
        myAdditionalVisualizations[parent] = 0;
    }
    myAdditionalVisualizations[parent] |= which;
    parent->addAdditionalGLVisualisation(this);
}


void
GUIBaseVehicle::removeActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) {
    myAdditionalVisualizations[parent] &= ~which;
    parent->removeAdditionalGLVisualisation(this);
}


void
GUIBaseVehicle::drawRoute(const GUIVisualizationSettings& s, int routeNo, SUMOReal darken) const {
    setColor(s);
    GLdouble colors[4];
    glGetDoublev(GL_CURRENT_COLOR, colors);
    colors[0] -= darken;
    if (colors[0] < 0) {
        colors[0] = 0;
    }
    colors[1] -= darken;
    if (colors[1] < 0) {
        colors[1] = 0;
    }
    colors[2] -= darken;
    if (colors[2] < 0) {
        colors[2] = 0;
    }
    colors[3] -= darken;
    if (colors[3] < 0) {
        colors[3] = 0;
    }
    glColor3dv(colors);
    const SUMOReal exaggeration = s.vehicleSize.getExaggeration(s);
    if (routeNo == 0) {
        drawRouteHelper(myVehicle.getRoute(), exaggeration);
        return;
    }
    --routeNo; // only prior routes are stored
    const MSRoute* route = myRoutes->getRoute(routeNo);
    if (route != 0) {
        drawRouteHelper(*route, exaggeration);
    }
}


const Position&
GUIBaseVehicle::getSeatPosition(int personIndex) const {
    /// if there are not enough seats in the vehicle people have to squeeze onto the last seat
    return mySeatPositions[MIN2(personIndex, (int)mySeatPositions.size() - 1)];
}


/****************************************************************************/
