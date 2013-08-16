/****************************************************************************/
/// @file    GUIPerson.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A MSPerson extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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
#include <microsim/MSPerson.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSAbstractLaneChangeModel.h>
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <utils/common/StringUtils.h>
#include <utils/common/SUMOVehicleParameter.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GLObjectValuePassConnector.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include "GUIPerson.h"
#include "GUINet.h"
#include "GUIEdge.h"
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
/*
FXDEFMAP(GUIPerson::GUIPersonPopupMenu) GUIPersonPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_ALLROUTES, GUIPerson::GUIPersonPopupMenu::onCmdShowAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_ALLROUTES, GUIPerson::GUIPersonPopupMenu::onCmdHideAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_CURRENTROUTE, GUIPerson::GUIPersonPopupMenu::onCmdShowCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_CURRENTROUTE, GUIPerson::GUIPersonPopupMenu::onCmdHideCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_BEST_LANES, GUIPerson::GUIPersonPopupMenu::onCmdShowBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_BEST_LANES, GUIPerson::GUIPersonPopupMenu::onCmdHideBestLanes),
    FXMAPFUNC(SEL_COMMAND, MID_START_TRACK, GUIPerson::GUIPersonPopupMenu::onCmdStartTrack),
    FXMAPFUNC(SEL_COMMAND, MID_STOP_TRACK, GUIPerson::GUIPersonPopupMenu::onCmdStopTrack),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_LFLINKITEMS, GUIPerson::GUIPersonPopupMenu::onCmdShowLFLinkItems),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_LFLINKITEMS, GUIPerson::GUIPersonPopupMenu::onCmdHideLFLinkItems),
};

// Object implementation
FXIMPLEMENT(GUIPerson::GUIPersonPopupMenu, GUIGLObjectPopupMenu, GUIPersonPopupMenuMap, ARRAYNUMBER(GUIPersonPopupMenuMap))
*/



// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIPerson::GUIPersonPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIPerson::GUIPersonPopupMenu::GUIPersonPopupMenu(
    GUIMainWindow& app, GUISUMOAbstractView& parent,
    GUIGlObject& o, std::map<GUISUMOAbstractView*, int>& additionalVisualizations)
    : GUIGLObjectPopupMenu(app, parent, o), myVehiclesAdditionalVisualizations(additionalVisualizations) {
}


GUIPerson::GUIPersonPopupMenu::~GUIPersonPopupMenu() {}



/* -------------------------------------------------------------------------
 * GUIPerson - methods
 * ----------------------------------------------------------------------- */
GUIPerson::GUIPerson(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSPerson::MSPersonPlan* plan) :
    MSPerson(pars, vtype, plan),
    GUIGlObject(GLO_PERSON, pars->id),
    myAmVisualizedAsStopped(false)
{ }


GUIPerson::~GUIPerson() {
}


GUIGLObjectPopupMenu*
GUIPerson::getPopUpMenu(GUIMainWindow& app,
                        GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIPersonPopupMenu(app, parent, *this, myAdditionalVisualizations);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUIPerson::getParameterWindow(GUIMainWindow& app,
                              GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 3);
    // add items
    //ret->mkItem("type [NAME]", false, myType->getID());
    ret->mkItem("stage", false, getCurrentStageTypeName());
    ret->mkItem("from", false, getFromEdge()->getID());
    ret->mkItem("to", false, getDestination().getID());
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GUIPerson::getCenteringBoundary() const {
    Boundary b;
    b.add(getPosition(MSNet::getInstance()->getCurrentTimeStep()));
    b.grow(20);
    return b;
}


void
GUIPerson::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    glPushMatrix();
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    Position p1 = getPosition(now);
    glTranslated(p1.x(), p1.y(), getType());
    // XXX use person specific gui settings
    // set person color
    setColor(s);
    // scale
    SUMOReal upscale = s.personExaggeration;
    glScaled(upscale, upscale, 1);
    switch (s.personQuality) {
        case 0:
        case 1:
            drawAction_drawAsTriangle(s);
            break;
        case 2:
            drawAction_drawAsPoly(s);
            break;
        case 3:
        default:
            drawAction_drawAsImage(s);
            break;
    }
    glPopMatrix();
    drawName(p1, s.scale, s.personName);
    glPopName();
}


void
GUIPerson::drawGLAdditional(GUISUMOAbstractView* const /* parent */, const GUIVisualizationSettings& /* s */) const {
    glPushName(getGlID());
    glPushMatrix();
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
                SUMOReal darken = SUMOReal(0.4) / SUMOReal(noReroutePlus1) * SUMOReal(i);
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
    #ifdef HAVE_INTERNAL_LANES
            MSLane *via = link->getViaLane();
            if (via == 0) {
                via = link->getLane();
            }
    #else
            MSLane *via = link->getLane();
    #endif
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
    glPopMatrix();
    glPopName();
}




void
GUIPerson::setColor(const GUIVisualizationSettings& s) const {
    const GUIColorer& c = s.personColorer;
    if (!setFunctionalColor(c.getActive())) {
        GLHelper::setColor(c.getScheme().getColor(getColorValue(c.getActive())));
    }
}


bool
GUIPerson::setFunctionalColor(size_t activeScheme) const {
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
        default:
            return false;
    }
}


SUMOReal
GUIPerson::getColorValue(size_t activeScheme) const {
    switch (activeScheme) {
        case 4:
            if (isWaiting4Vehicle()) {
                return 3;
            } else {
                return (SUMOReal)getCurrentStageType();
            }
        case 5:
            return STEPS2TIME(timeWaiting4Vehicle(MSNet::getInstance()->getCurrentTimeStep()));
    }
    return 0;
}


Position
GUIPerson::getPosition(SUMOTime now) const {
    if (getCurrentStageType() == DRIVING && !isWaiting4Vehicle()) {
        return myPositionInVehicle;
    }
    return MSPerson::getPosition(now);
}


void
GUIPerson::drawAction_drawAsTriangle(const GUIVisualizationSettings& /* s */) const {
    // draw triangle pointing down
    glBegin(GL_TRIANGLES);
    glVertex2d(0., 0.);
    glVertex2d(-.5, 1.);
    glVertex2d(.5, 1.);
    glEnd();
}


void
GUIPerson::drawAction_drawAsPoly(const GUIVisualizationSettings& /* s */) const {
    // draw pedestrian shape
    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    glRotated(getAngle(now), 0, 0, 1);
    RGBColor lighter = GLHelper::getColor().changedBrightness(51);
    glTranslated(0, 0, .045);
    GLHelper::drawFilledCircle(0.3);
    glTranslated(0, 0, -.045);
    glScaled(.7, 2, 1);
    glTranslated(0, 0, .04);
    GLHelper::setColor(lighter);
    GLHelper::drawFilledCircle(0.3);
    glTranslated(0, 0, -.04);
}


void
GUIPerson::drawAction_drawAsImage(const GUIVisualizationSettings& s) const {
    if (getVehicleType().getGuiShape() == SVS_PEDESTRIAN) {
        const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
        glRotated(getAngle(now), 0, 0, 1);
    }
    const std::string& file = getVehicleType().getImgFile();
    if (file != "") {
        int textureID = GUITexturesHelper::getTextureID(file);
        if (textureID > 0) {
            const SUMOReal halfLength = getVehicleType().getLength() / 2.0 * s.vehicleExaggeration;
            const SUMOReal halfWidth = getVehicleType().getWidth() / 2.0 * s.vehicleExaggeration;
            GUITexturesHelper::drawTexturedBox(textureID, -halfWidth, -halfLength, halfWidth, halfLength);
        }
    } else {
        // fallback if no image is defined
        drawAction_drawAsPoly(s);
    }
}
/****************************************************************************/

