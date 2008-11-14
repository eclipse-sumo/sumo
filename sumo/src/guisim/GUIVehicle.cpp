/****************************************************************************/
/// @file    GUIVehicle.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
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
#include <guisim/GUIRoute.h>
#include <utils/common/RandHelper.h>
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/gui/div/GLHelper.h>
#include <foreign/polyfonts/polyfonts.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


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
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIVehicle::GUIVehiclePopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIVehicle::GUIVehiclePopupMenu::GUIVehiclePopupMenu(
    GUIMainWindow &app, GUISUMOAbstractView &parent,
    GUIGlObject &o)
        : GUIGLObjectPopupMenu(app, parent, o)
{
}


GUIVehicle::GUIVehiclePopupMenu::~GUIVehiclePopupMenu()
{}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->showRoute(static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->hideRoute(static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->showRoute(static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowBestLanes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->showBestLanes(static_cast<GUIVehicle*>(myObject));
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->hideRoute(static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideBestLanes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->hideBestLanes(static_cast<GUIVehicle*>(myObject));
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdStartTrack(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    myParent->startTrack(static_cast<GUIVehicle*>(myObject)->getGlID());
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdStopTrack(FXObject*,FXSelector,void*)
{
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
                       int vehicleIndex) throw()
        : MSVehicle(pars, route, type, vehicleIndex),
        GUIGlObject(idStorage, "vehicle:"+pars->id)
{
    myIntCORNMap[MSCORN::CORN_VEH_BLINKER] = 0;
}


GUIVehicle::~GUIVehicle() throw()
{
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


SUMOReal
GUIVehicle::getTimeSinceLastLaneChangeAsReal() const
{
    return (SUMOReal) myLastLaneChangeOffset;
}


GUIGLObjectPopupMenu *
GUIVehicle::getPopUpMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent) throw()
{
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
    buildShowParamsPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIVehicle::getParameterWindow(GUIMainWindow &app,
                               GUISUMOAbstractView &) throw()
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 9);
    // add items
    ret->mkItem("type [NAME]", false, myType->getID());
    ret->mkItem("left same route [#]", false, getParameter().repetitionNumber);
    ret->mkItem("emission period [s]", false, getParameter().repetitionOffset);
    ret->mkItem("waiting time [s]", true,
                new CastingFunctionBinding<MSVehicle, SUMOReal, unsigned int>(this, &MSVehicle::getWaitingTime));
    ret->mkItem("last lane change [s]", true,
                new CastingFunctionBinding<GUIVehicle, SUMOReal, unsigned int>(this, &GUIVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, getDesiredDepart());
    ret->mkItem("position [m]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getPositionOnLane));
    ret->mkItem("speed [m/s]", true,
                new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getSpeed));
    // close building
    ret->closeBuilding();
    return ret;
}


const std::string &
GUIVehicle::getMicrosimID() const throw()
{
    return getID();
}


bool
GUIVehicle::active() const throw()
{
    return isOnRoad();
}


void
GUIVehicle::setRemoved()
{
    myLane = 0;
}


unsigned int
GUIVehicle::getLastLaneChangeOffset() const
{
    return myLastLaneChangeOffset;
}


Boundary
GUIVehicle::getCenteringBoundary() const throw()
{
    Boundary b;
    b.add(getPosition());
    b.grow(20);
    return b;
}










inline void
drawAction_drawVehicleAsTrianglePlus(const GUIVehicle &veh, SUMOReal upscale)
{
    SUMOReal length = veh.getLength();
    glPushMatrix();
    glScaled(upscale, upscale, upscale);
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

#define BLINKER_POS_FRONT 1.
#define BLINKER_POS_BACK 1.

inline void
drawAction_drawVehicleBlinker(const GUIVehicle &veh)
{
    int dir = veh.getCORNIntValue(MSCORN::CORN_VEH_BLINKER);
    if (dir==0) {
        return;
    }
    glColor3f(1.f, .8f, 0);
    glTranslated(dir, BLINKER_POS_FRONT, 0);
    GLHelper::drawFilledCircle(.5, 6);
    glTranslated(0, -BLINKER_POS_FRONT-BLINKER_POS_BACK+veh.getLength(), 0);
    GLHelper::drawFilledCircle(.5, 6);
    glTranslated(-dir, +BLINKER_POS_BACK-veh.getLength(), 0);
}


inline void
drawAction_drawVehicleName(const GUIVehicle &veh, SUMOReal size)
{
    glPushMatrix();
    glTranslated(0, veh.getLength() / 2., 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    pfSetPosition(0, 0);
    pfSetScale(size);
    SUMOReal w = pfdkGetStringWidth(veh.getMicrosimID().c_str());
    glRotated(180, 0, 1, 0);
    glTranslated(-w/2., 0.4, 0);
    pfDrawString(veh.getMicrosimID().c_str());
    glPopMatrix();
}


#ifdef HAVE_BOYOM_C2C
inline void
drawAction_C2CdrawVehicleRadius(const GUIVehicle &veh)
{
    if (veh.isEquipped()) {
        int cluster = veh.getClusterId();
        if (veh.getConnections().size()==0) {
            cluster = -1;
        }
        if (colorBla.find(cluster)==colorBla.end()) {
            int r = RandHelper::rand(256);
            int g = RandHelper::rand(256);
            int b = RandHelper::rand(256);
            colorBla[cluster] = FXRGB(r, g, b);
        }
        FXColor c = colorBla[cluster];
        glColor3f(
            (float)((float) FXREDVAL(c) /255.),
            (float)((float) FXGREENVAL(c) /255.),
            (float)((float) FXBLUEVAL(c) /255.));
        GLHelper::drawOutlineCircle(MSGlobals::gLANRange, MSGlobals::gLANRange-2, 24);
    }
}
#endif


#include <gui/GUIViewTraffic.h>

void
GUIVehicle::drawGL(const GUIVisualizationSettings &s) const throw()
{
    glPolygonOffset(0, -4);
    // set lane color
    GUIColoringSchemesMap<GUIVehicle> &sm = GUIViewTraffic::getVehiclesSchemesMap(); //!!!
    sm.getColorer(s.vehicleMode)->setGlColor(*this);
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
    drawAction_drawVehicleAsTrianglePlus(*this, upscale);
    // draw the blinker if wished
    if (s.showBlinker) {
        glPolygonOffset(0, -5);
        drawAction_drawVehicleBlinker(*this);
    }
    // draw the c2c-circle
#ifdef HAVE_BOYOM_C2C
    if (s.drawcC2CRadius) {
        drawAction_C2CdrawVehicleRadius(*veh);
    }
#endif
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
        glPolygonOffset(0, -6);
        // compute name colors
        glColor3f(s.vehicleNameColor.red(), s.vehicleNameColor.green(), s.vehicleNameColor.blue());
        drawAction_drawVehicleName(*this, s.vehicleNameSize / s.scale);
    }
    // (optional) clear id
    if (s.needsGlID) {
        glPopName();
    }
}

const std::vector<MSVehicle::LaneQ> &
GUIVehicle::getBestLanes() const throw()
{
    myLock.lock();
    const std::vector<MSVehicle::LaneQ> &ret = MSVehicle::getBestLanes();
    myLock.unlock();
    return ret;
}


void
GUIVehicle::setBlinkerInformation()
{
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
            if (link!=lane.getLinkCont().end()&&lane.length()-getPositionOnLane()<lane.maxSpeed()*(SUMOReal) 7.) {
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




/****************************************************************************/

