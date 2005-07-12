//---------------------------------------------------------------------------//
//                        GUILaneSpeedTrigger.cpp -
//  Class that realises the setting of a lane's maximum speed triggered by
//      values read from a file
//                           -------------------
//  begin                : Mon, 26.04.2004
//  copyright            : (C) 2004 by DLR http://ivf.dlr.de/
//  author               : Daniel Krajzewicz
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
// Revision 1.7  2005/07/12 12:17:39  dkrajzew
// we're using polyfonts, now
//
// Revision 1.6  2005/05/04 07:59:59  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2005/02/17 10:33:29  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.4  2004/12/16 12:14:59  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.3  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.2  2004/08/02 13:15:21  dkrajzew
// missing "showAsKMH"-initialisation added
//
// Revision 1.1  2004/07/02 08:55:10  dkrajzew
// visualisation of vss added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/Boundary.h>
#include <utils/glutils/GLHelper.h>
#include <utils/convert/ToString.h>
#include <helpers/Command.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include "GUILaneSpeedTrigger.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <gui/manipulators/GUIManip_LaneSpeedTrigger.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/glutils/polyfonts.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu)
    GUILaneSpeedTriggerPopupMenuMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu::onCmdOpenManip),

};

// Object implementation
FXIMPLEMENT(GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu, GUIGLObjectPopupMenu, GUILaneSpeedTriggerPopupMenuMap, ARRAYNUMBER(GUILaneSpeedTriggerPopupMenuMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu::GUILaneSpeedTriggerPopupMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent,
        GUIGlObject &o)
    : GUIGLObjectPopupMenu(app, parent, o)
{
}


GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu::~GUILaneSpeedTriggerPopupMenu()
{
}


long
GUILaneSpeedTrigger::GUILaneSpeedTriggerPopupMenu::onCmdOpenManip(FXObject*,
                                                                  FXSelector,
                                                                  void*)
{
    static_cast<GUILaneSpeedTrigger*>(myObject)->openManipulator(
        *myApplication, *myParent);
    return 1;
}


/* -------------------------------------------------------------------------
 * GUILaneSpeedTrigger - methods
 * ----------------------------------------------------------------------- */
GUILaneSpeedTrigger::GUILaneSpeedTrigger(const std::string &id,
            MSNet &net, const std::vector<MSLane*> &destLanes,
            const std::string &aXMLFilename)
    : MSLaneSpeedTrigger(id, net, destLanes, aXMLFilename),
    GUIGlObject_AbstractAdd(gIDStorage,
        string("speedtrigger:") + id, GLO_LANESPEEDTRIGGER),
    myShowAsKMH(true), myLastValue(-1)
{
    mySGPositions.reserve(destLanes.size());
    myFGPositions.reserve(destLanes.size());
    myFGRotations.reserve(destLanes.size());
    mySGRotations.reserve(destLanes.size());
    vector<MSLane*>::const_iterator i;
    for(i=destLanes.begin(); i!=destLanes.end(); ++i) {
        GUIEdge *edge =
            static_cast<GUIEdge*>(MSEdge::dictionary((*i)->edge().id()));
        const Position2DVector &v =
            edge->getLaneGeometry((const MSLane *) (*i)).getShape();
        myFGPositions.push_back(v.positionAtLengthPosition(0));
        Line2D l(v.getBegin(), v.getEnd());
        mySGPositions.push_back(l.getPositionAtDistance(0));
        myFGRotations.push_back(-v.rotationDegreeAtLengthPosition(0));
        mySGRotations.push_back(-l.atan2DegreeAngle());
        myDefaultSpeed = (*i)->maxSpeed();
        mySpeedOverrideValue = (*i)->maxSpeed();
        myLoadedSpeed = (*i)->maxSpeed();
    }
}


GUILaneSpeedTrigger::~GUILaneSpeedTrigger()
{
}


GUIGLObjectPopupMenu *
GUILaneSpeedTrigger::getPopUpMenu(GUIMainWindow &app,
                                  GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret =
        new GUILaneSpeedTriggerPopupMenu(app, parent, *this);
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Open Manipulator...",
        GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_MANIP);
/*    FXMenuPane *manualSpeed = new FXMenuPane(ret);
    new FXMenuTitle(ret,"&Arrange",NULL,manualSpeed);
    new FXMenuCommand(manualSpeed,"20 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);// MID_020KMH);
    new FXMenuCommand(manualSpeed,"40 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_040KMH);
    new FXMenuCommand(manualSpeed,"60 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_060KMH);
    new FXMenuCommand(manualSpeed,"80 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_080KMH);
    new FXMenuCommand(manualSpeed,"100 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_100KMH);
    new FXMenuCommand(manualSpeed,"120 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_120KMH);
    new FXMenuCommand(manualSpeed,"140 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_140KMH);
    new FXMenuCommand(manualSpeed,"160 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_160KMH);
    new FXMenuCommand(manualSpeed,"180 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_180KMH);
    new FXMenuCommand(manualSpeed,"200 km/h\t\t",
        GUIIconSubSys::getIcon(ICON_FLAG_MINUS), 0, 0);//MID_200KMH);
        */
    //
    if(gSelected.isSelected(GLO_LANESPEEDTRIGGER, getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Show Parameter", 0, ret, MID_SHOWPARS);
    return ret;
}


GUIParameterTableWindow *
GUILaneSpeedTrigger::getParameterWindow(GUIMainWindow &app,
                                        GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 7);
    // add items
    ret->mkItem("speed [m/s]", true,
        new FunctionBinding<GUILaneSpeedTrigger, double>(this, &GUILaneSpeedTrigger::getCurrentSpeed));
    // close building
    ret->closeBuilding();
    return ret;
}


std::string
GUILaneSpeedTrigger::microsimID() const
{
    return getID();
}


bool
GUILaneSpeedTrigger::active() const
{
    return true;
}


Position2D
GUILaneSpeedTrigger::getPosition() const
{
    return (*(myFGPositions.begin())); // !!!
}


void
GUILaneSpeedTrigger::drawGL_FG(double scale)
{
    doPaint(myFGPositions, myFGRotations, scale);
}


void
GUILaneSpeedTrigger::drawGL_SG(double scale)
{
    doPaint(mySGPositions, mySGRotations, scale);
}


void
GUILaneSpeedTrigger::doPaint(const PosCont &poss, const RotCont rots,
                             double scale)
{
    for(size_t i=0; i<poss.size(); ++i) {
        const Position2D &pos = poss[i];
        double rot = rots[i];
        glPushMatrix();
        glTranslated(pos.x(), pos.y(), 0);
        glRotated( rot, 0, 0, 1 );
        glTranslated(0, -1.5, 0);

        int noPoints = 9;
        if(scale>25) {
            noPoints = (int) (9.0 + scale / 10.0);
            if(noPoints>36) {
                noPoints = 36;
            }
        }
        glColor3f(1, 0, 0);
        GLHelper::drawFilledCircle(1.3, noPoints);
        if(scale<10) {
            glPopMatrix();
            return;
        }
        glColor3f(0, 0, 0);
        GLHelper::drawFilledCircle(1.1, noPoints);
        // draw the speed string
            // not if scale to low
        if(scale<4.5) {
            glPopMatrix();
            return;
        }
            // compute
        float value = (float) getCurrentSpeed();
        if(myShowAsKMH) {
            value *= 3.6f;
            // !!!
            if(((int) value+1)%10==0) {
                value = (float) (((int) value+1) / 10 * 10);
            }
        }
        if(value!=myLastValue) {
            myLastValue = value;
            myLastValueString = toString<float>(myLastValue);
            size_t idx = myLastValueString.find('.');
            if(idx!=string::npos) {
                if(idx>myLastValueString.length()) {
                    idx = myLastValueString.length();
                }
                myLastValueString = myLastValueString.substr(0, idx);
            }
        }
            //draw
        glColor3f(1, 1, 0);

        float w = pfdkGetStringWidth(myLastValueString.c_str());
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(1.2f);
        glRotated(180, 0, 1, 0);
        glTranslated(-w/2., 0.3, 0);
        pfDrawString(myLastValueString.c_str());
        glTranslated(w/2., -0.3, 0);
        glColor3f(0, 1, 0);
        glPopMatrix();
    }
}

Boundary
GUILaneSpeedTrigger::getBoundary() const
{
    Position2D pos = getPosition();
    Boundary ret(pos.x(), pos.y(), pos.x(), pos.y());
    ret.grow(2.0);
    return ret;
}


GUIManipulator *
GUILaneSpeedTrigger::openManipulator(GUIMainWindow &app,
                                     GUISUMOAbstractView &parent)
{
    GUIManip_LaneSpeedTrigger *gui =
        new GUIManip_LaneSpeedTrigger(app, getFullName(), *this, 0, 0);
    gui->create();
    gui->show();
    return gui;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


