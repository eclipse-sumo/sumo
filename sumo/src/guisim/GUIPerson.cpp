/****************************************************************************/
/// @file    GUIPerson.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
    GUIGlObject& o, std::map<GUISUMOAbstractView*, int> &additionalVisualizations)
    : GUIGLObjectPopupMenu(app, parent, o), myVehiclesAdditionalVisualizations(additionalVisualizations) {
}


GUIPerson::GUIPersonPopupMenu::~GUIPersonPopupMenu() {}



/* -------------------------------------------------------------------------
 * GUIPerson - methods
 * ----------------------------------------------------------------------- */
GUIPerson::GUIPerson(const SUMOVehicleParameter* pars, MSPerson::MSPersonPlan* plan)
      : MSPerson(pars, plan), GUIGlObject(GLO_PERSON, pars->id)  {
}


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
        new GUIParameterTableWindow(app, *this, 1);
    // add items
    //ret->mkItem("type [NAME]", false, myType->getID());
    ret->mkItem("stage", false, getCurrentStageTypeName());
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
    Position p1 = getPosition(MSNet::getInstance()->getCurrentTimeStep());
    glTranslated(p1.x(), p1.y(), getType());
    // set lane color
    setColor(s);
    // scale
    SUMOReal upscale = s.vehicleExaggeration;
    glScaled(upscale, upscale, 1);
    glBegin(GL_TRIANGLES);
    glVertex2d(0., 0.);
    glVertex2d(-.5, 1.);
    glVertex2d( .5, 1.);
    glEnd();
    //*/
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
    /*
    switch (s.vehicleQuality) {
        case 0:
            drawAction_drawVehicleAsTrianglePlus(*this);
            break;
        case 1:
            drawAction_drawVehicleAsBoxPlus(*this);
            break;
        case 2:
        default:
            drawAction_drawVehicleAsPoly(*this);
            break;
    }
    if (s.drawMinGap) {
        SUMOReal minGap = -getVehicleType().getMinGap();
        glColor3d(0., 1., 0.);
        glBegin(GL_LINES);
        glVertex2d(0., 0);
        glVertex2d(0., minGap);
        glVertex2d(-.5, minGap);
        glVertex2d(.5, minGap);
        glEnd();
    }
    // draw the blinker and brakelights if wished
    if (s.showBlinker) {
        glTranslated(0, 0, .1);
        switch (getVehicleType().getGuiShape()) {
            case SVS_PEDESTRIAN:
            case SVS_BICYCLE:
            case SVS_ANT:
            case SVS_RAIL:
            case SVS_RAIL_LIGHT:
            case SVS_RAIL_SLOW:
            case SVS_RAIL_FAST:
            case SVS_RAIL_CARGO:
            // only SVS_RAIL_CITY has blinkers and brake lights
                break;
            default:
                drawAction_drawVehicleBlinker(*this);
                drawAction_drawVehicleBrakeLight(*this);
                break;
        }
    }
    */
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
    glPopMatrix();
    /*
    drawName(myLane->getShape().positionAtLengthPosition(myState.pos() - getVehicleType().getLength() / 2),
             s.scale, s.vehicleName);*/
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
    const GUIColorer& c = s.vehicleColorer;
    if (!setFunctionalColor(c.getActive())) {
        GLHelper::setColor(c.getScheme().getColor(getColorValue(c.getActive())));
    }
}


bool
GUIPerson::setFunctionalColor(size_t /* activeScheme */) const {
    /*
    switch (activeScheme) {
        case 1: {
            GLHelper::setColor(getParameter().color);
            return true;
        }
        case 2: {
            GLHelper::setColor(getVehicleType().getColor());
            return true;
        }
        case 3: {
            GLHelper::setColor(getRoute().getColor());
            return true;
        }
        case 4: {
            Position p = getRoute().getEdges()[0]->getLanes()[0]->getShape()[0];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            Position center = b.getCenter();
            SUMOReal hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / PI;
            SUMOReal sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
        case 5: {
            Position p = getRoute().getEdges().back()->getLanes()[0]->getShape()[-1];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            Position center = b.getCenter();
            SUMOReal hue = 180. + atan2(center.x() - p.x(), center.y() - p.y()) * 180. / PI;
            SUMOReal sat = p.distanceTo(center) / center.distanceTo(Position(b.xmin(), b.ymin()));
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
        case 6: {
            Position pb = getRoute().getEdges()[0]->getLanes()[0]->getShape()[0];
            Position pe = getRoute().getEdges().back()->getLanes()[0]->getShape()[-1];
            const Boundary& b = ((GUINet*) MSNet::getInstance())->getBoundary();
            SUMOReal hue = 180. + atan2(pb.x() - pe.x(), pb.y() - pe.y()) * 180. / PI;
            Position minp(b.xmin(), b.ymin());
            Position maxp(b.xmax(), b.ymax());
            SUMOReal sat = pb.distanceTo(pe) / minp.distanceTo(maxp);
            GLHelper::setColor(RGBColor::fromHSV(hue, sat, 1.));
            return true;
        }
    }
    */
    return false;
}


SUMOReal
GUIPerson::getColorValue(size_t /* activeScheme */) const {
    /*
    switch (activeScheme) {
        case 7:
            return getSpeed();
        case 8:
            return getWaitingSeconds();
        case 9:
            return getLastLaneChangeOffset();
        case 10:
            return getMaxSpeed();
        case 11:
            return getHBEFA_CO2Emissions();
        case 12:
            return getHBEFA_COEmissions();
        case 13:
            return getHBEFA_PMxEmissions();
        case 14:
            return getHBEFA_NOxEmissions();
        case 15:
            return getHBEFA_HCEmissions();
        case 16:
            return getHBEFA_FuelConsumption();
        case 17:
            return getHarmonoise_NoiseEmissions();
        case 18:
            if (getNumberReroutes() == 0) {
                return -1;
            }
            return getNumberReroutes();
    }
    */
    return 0;
}




/****************************************************************************/

