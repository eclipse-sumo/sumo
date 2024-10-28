/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIContainer.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Wed, 01.08.2014
///
// A MSContainer extended by some values for usage within the gui
/****************************************************************************/
#include <config.h>

#include <cmath>
#include <vector>
#include <string>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <utils/geom/PositionVector.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GUIContainer.h"
#include "GUINet.h"
#include "GUIEdge.h"

//#define GUIContainer_DEBUG_DRAW_WALKING_AREA_SHAPE

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIContainer::GUIContainerPopupMenu) GUIContainerPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_START_TRACK, GUIContainer::GUIContainerPopupMenu::onCmdStartTrack),
    FXMAPFUNC(SEL_COMMAND, MID_STOP_TRACK,  GUIContainer::GUIContainerPopupMenu::onCmdStopTrack),
    FXMAPFUNC(SEL_COMMAND, MID_SHOWPLAN,    GUIContainer::GUIContainerPopupMenu::onCmdShowPlan),
};

// Object implementation
FXIMPLEMENT(GUIContainer::GUIContainerPopupMenu, GUIGLObjectPopupMenu, GUIContainerPopupMenuMap, ARRAYNUMBER(GUIContainerPopupMenuMap))

#define WATER_WAY_OFFSET 6.0

// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIContainer::GUIContainerPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIContainer::GUIContainerPopupMenu::GUIContainerPopupMenu(
    GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject& o)
    : GUIGLObjectPopupMenu(app, parent, o) {
}


GUIContainer::GUIContainerPopupMenu::~GUIContainerPopupMenu() {}


long
GUIContainer::GUIContainerPopupMenu::onCmdShowPlan(FXObject*, FXSelector, void*) {
    GUIContainer* p = dynamic_cast<GUIContainer*>(myObject);
    if (p == nullptr) {
        return 1;
    }
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(*myApplication, *p);
    // add items
    for (int stage = 1; stage < p->getNumStages(); stage++) {
        ret->mkItem(toString(stage).c_str(), false, p->getStageSummary(stage));
    }
    // close building (use an object that is not Parameterised as argument)
    Parameterised dummy;
    ret->closeBuilding(&dummy);
    return 1;
}


long
GUIContainer::GUIContainerPopupMenu::onCmdStartTrack(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_PERSON);
    if (myParent->getTrackedID() != static_cast<GUIContainer*>(myObject)->getGlID()) {
        myParent->startTrack(static_cast<GUIContainer*>(myObject)->getGlID());
    }
    return 1;
}

long
GUIContainer::GUIContainerPopupMenu::onCmdStopTrack(FXObject*, FXSelector, void*) {
    assert(myObject->getType() == GLO_PERSON);
    myParent->stopTrack();
    return 1;
}




/* -------------------------------------------------------------------------
 * GUIContainer - methods
 * ----------------------------------------------------------------------- */
GUIContainer::GUIContainer(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan) :
    MSTransportable(pars, vtype, plan, false),
    GUIGlObject(GLO_CONTAINER, pars->id, GUIIconSubSys::getIcon(GUIIcon::CONTAINER)) {
}


GUIContainer::~GUIContainer() {
}


GUIGLObjectPopupMenu*
GUIContainer::getPopUpMenu(GUIMainWindow& app,
                           GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIContainerPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    new FXMenuSeparator(ret);
    if (parent.getTrackedID() != getGlID()) {
        GUIDesigns::buildFXMenuCommand(ret, "Start Tracking", nullptr, ret, MID_START_TRACK);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, "Stop Tracking", nullptr, ret, MID_STOP_TRACK);
    }
    //

    buildShowParamsPopupEntry(ret);
    buildShowTypeParamsPopupEntry(ret);
    GUIDesigns::buildFXMenuCommand(ret, "Show Plan", GUIIconSubSys::getIcon(GUIIcon::APP_TABLE), ret, MID_SHOWPLAN);
    new FXMenuSeparator(ret);
    buildPositionCopyEntry(ret, app);
    return ret;
}


GUIParameterTableWindow*
GUIContainer::getParameterWindow(GUIMainWindow& app,
                                 GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem(TL("stage"), false, getCurrentStageDescription());
    // there is always the "start" stage which we do not count here because it is not strictly part of the plan
    ret->mkItem(TL("stage index"), false, toString(getCurrentStageIndex()) + " of " + toString(getNumStages() - 1));
    ret->mkItem(TL("start edge [id]"), false, getFromEdge()->getID());
    ret->mkItem(TL("dest edge [id]"), false, getDestination()->getID());
    ret->mkItem(TL("arrivalPos [m]"), false, toString(getCurrentStage()->getArrivalPos()));
    ret->mkItem(TL("edge [id]"), false, getEdge()->getID());
    ret->mkItem(TL("position [m]"), true, new FunctionBinding<GUIContainer, double>(this, &GUIContainer::getEdgePos));
    ret->mkItem(TL("speed [m/s]"), true, new FunctionBinding<GUIContainer, double>(this, &GUIContainer::getSpeed));
    ret->mkItem(TL("speed factor"), false, getChosenSpeedFactor());
    ret->mkItem(TL("angle [degree]"), true, new FunctionBinding<GUIContainer, double>(this, &GUIContainer::getAngle));
    ret->mkItem(TL("waiting time [s]"), true, new FunctionBinding<GUIContainer, double>(this, &GUIContainer::getWaitingSeconds));
    ret->mkItem(TL("desired depart [s]"), false, time2string(getParameter().depart));
    // close building
    ret->closeBuilding(&getParameter());
    return ret;
}


GUIParameterTableWindow*
GUIContainer::getTypeParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, "vType:" + myVType->getID());
    ret->mkItem(TL("length"), false, myVType->getLength());
    ret->mkItem(TL("width"), false, myVType->getWidth());
    ret->mkItem(TL("height"), false, myVType->getHeight());
    ret->mkItem(TL("minGap"), false, myVType->getMinGap());
    ret->mkItem(TL("mass [kg]"), false, myVType->getMass());
    ret->mkItem(TL("maximum speed [m/s]"), false, myVType->getMaxSpeed());
    ret->closeBuilding(&(myVType->getParameter()));
    return ret;
}


double
GUIContainer::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.containerSize.getExaggeration(s, this);
}


Boundary
GUIContainer::getCenteringBoundary() const {
    Boundary b;
    // ensure that the vehicle is drawn, otherwise myPositionInVehicle will not be updated
    b.add(getPosition());
    b.grow(20);
    return b;
}


void
GUIContainer::drawGL(const GUIVisualizationSettings& s) const {
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    Position p1 = getPosition();
    double angle = getAngle();
    if (getCurrentStageType() == MSStageType::DRIVING && !isWaiting4Vehicle()) {
        p1 = myPositionInVehicle.pos;
        angle = myPositionInVehicle.angle;
    }
    glTranslated(p1.x(), p1.y(), getType());
    glRotated(RAD2DEG(angle), 0, 0, 1);
    // set container color
    setColor(s);
    // scale
    const double upscale = getExaggeration(s);
    glScaled(upscale, upscale, 1);
    switch (s.containerQuality) {
        case 0:
        case 1:
        case 2:
            drawAction_drawAsPoly(s);
            break;
        case 3:
        default:
            drawAction_drawAsImage(s);
            break;
    }
    GLHelper::popMatrix();

    drawName(p1, s.scale, s.containerName, s.angle);
    GLHelper::popName();
}


void
GUIContainer::drawGLAdditional(GUISUMOAbstractView* const /* parent */, const GUIVisualizationSettings& /* s */) const {
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    /*
    glTranslated(0, 0, getType() - .1); // don't draw on top of other cars
    if (hasActiveAddVisualisation(parent, VO_SHOW_BEST_LANES)) {
        drawBestLanes();
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_ROUTE)) {
        drawRoute(s, 0, 0.25);
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_ALL_ROUTES)) {
        if (getNumberReroutes() > 0) {
            const int noReroutePlus1 = getNumberReroutes() + 1;
            for (int i = noReroutePlus1 - 1; i >= 0; i--) {
                double darken = double(0.4) / double(noReroutePlus1) * double(i);
                drawRoute(s, i, darken);
            }
        } else {
            drawRoute(s, 0, 0.25);
        }
    }
    if (hasActiveAddVisualisation(parent, VO_SHOW_LFLINKITEMS)) {
        for (DriveItemVector::const_iterator i = myLFLinkLanes.begin(); i != myLFLinkLanes.end(); ++i) {
            if((*i).myLink==0) {
                continue;
            }
            MSLink* link = (*i).myLink;
            MSLane *via = link->getViaLane();
            if (via == 0) {
                via = link->getLane();
            }
            if (via != 0) {
                Position p = via->getShape()[0];
                if((*i).mySetRequest) {
                    glColor3f(0, 1, 0);
                } else {
                    glColor3f(1, 0, 0);
                }
                glTranslated(p.x(), p.y(), -.1);
                GLHelper::drawFilledCircle(1);
                glTranslated(-p.x(), -p.y(), .1);
            }
        }
    }
    */
    GLHelper::popMatrix();
    GLHelper::popName();
}




void
GUIContainer::setColor(const GUIVisualizationSettings& s) const {
    const GUIColorer& c = s.containerColorer;
    if (!setFunctionalColor(c.getActive())) {
        GLHelper::setColor(c.getScheme().getColor(getColorValue(s, c.getActive())));
    }
}


bool
GUIContainer::setFunctionalColor(int activeScheme) const {
    switch (activeScheme) {
        case 0: {
            if (getParameter().wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(getParameter().color);
                return true;
            }
            if (getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(getVehicleType().getColor());
                return true;
            }
            return false;
        }
        case 2: {
            if (getParameter().wasSet(VEHPARS_COLOR_SET)) {
                GLHelper::setColor(getParameter().color);
                return true;
            }
            return false;
        }
        case 3: {
            if (getVehicleType().wasSet(VTYPEPARS_COLOR_SET)) {
                GLHelper::setColor(getVehicleType().getColor());
                return true;
            }
            return false;
        }
        case 8: { // color by angle
            double hue = GeomHelper::naviDegree(getAngle());
            GLHelper::setColor(RGBColor::fromHSV(hue, 1., 1.));
            return true;
        }
        case 9: { // color randomly (by pointer)
            const double hue = (double)((long long int)this % 360); // [0-360]
            const double sat = (double)(((long long int)this / 360) % 67) / 100. + 0.33; // [0.33-1]
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
        default:
            return false;
    }
}


double
GUIContainer::getColorValue(const GUIVisualizationSettings& /* s */, int activeScheme) const {
    switch (activeScheme) {
        case 4:
            return getSpeed();
        case 5:
            if (isWaiting4Vehicle()) {
                return 5;
            } else {
                return (double)getCurrentStageType();
            }
        case 6:
            return getWaitingSeconds();
        case 7:
            return gSelected.isSelected(GLO_CONTAINER, getGlID());
    }
    return 0;
}


double
GUIContainer::getEdgePos() const {
    FXMutexLock locker(myLock);
    return MSTransportable::getEdgePos();
}

int
GUIContainer::getDirection() const {
    FXMutexLock locker(myLock);
    return MSTransportable::getDirection();
}


Position
GUIContainer::getPosition() const {
    FXMutexLock locker(myLock);
    if (getCurrentStageType() == MSStageType::WAITING && getEdge()->getPermissions() == SVC_SHIP) {
        MSLane* lane = getEdge()->getLanes().front();   //the most right lane of the water way
        PositionVector laneShape = lane->getShape();
        return laneShape.positionAtOffset2D(getEdgePos(), WATER_WAY_OFFSET);
    }
    return MSTransportable::getPosition();
}


double
GUIContainer::getAngle() const {
    FXMutexLock locker(myLock);
    return MSTransportable::getAngle();
}


double
GUIContainer::getWaitingSeconds() const {
    FXMutexLock locker(myLock);
    return MSTransportable::getWaitingSeconds();
}


double
GUIContainer::getSpeed() const {
    FXMutexLock locker(myLock);
    return MSTransportable::getSpeed();
}


void
GUIContainer::drawAction_drawAsPoly(const GUIVisualizationSettings& /* s */) const {
    // draw pedestrian shape
    glScaled(getVehicleType().getLength(), getVehicleType().getWidth(), 1);
    glBegin(GL_QUADS);
    glVertex2d(0, 0.5);
    glVertex2d(0, -0.5);
    glVertex2d(-1, -0.5);
    glVertex2d(-1, 0.5);
    glEnd();
    GLHelper::setColor(GLHelper::getColor().changedBrightness(-30));
    glTranslated(0, 0, .045);
    glBegin(GL_QUADS);
    glVertex2d(-0.1, 0.4);
    glVertex2d(-0.1, -0.4);
    glVertex2d(-0.9, -0.4);
    glVertex2d(-0.9, 0.4);
    glEnd();
}


void
GUIContainer::drawAction_drawAsImage(const GUIVisualizationSettings& s) const {
    const std::string& file = getVehicleType().getImgFile();
    if (file != "") {
        // @todo invent an option for controlling whether images should be rotated or not
        //if (getVehicleType().getGuiShape() == SVS_CONTAINER) {
        //    glRotated(RAD2DEG(getAngle() + M_PI / 2.), 0, 0, 1);
        //}
        int textureID = GUITexturesHelper::getTextureID(file);
        if (textureID > 0) {
            const double exaggeration = s.personSize.getExaggeration(s, this);
            const double halfLength = getVehicleType().getLength() / 2.0 * exaggeration;
            const double halfWidth = getVehicleType().getWidth() / 2.0 * exaggeration;
            GUITexturesHelper::drawTexturedBox(textureID, -halfWidth, -halfLength, halfWidth, halfLength);
        }
    } else {
        // fallback if no image is defined
        drawAction_drawAsPoly(s);
    }
}

bool
GUIContainer::isSelected() const {
    return gSelected.isSelected(GLO_CONTAINER, getGlID());
}

/****************************************************************************/
