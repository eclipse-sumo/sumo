//---------------------------------------------------------------------------//
//                        GUITriggeredRerouter.cpp -
//  The gui-version of MSTriggeredRerouter
//                           -------------------
//  begin                : Mon, 25.07.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.5  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.4  2005/11/09 06:35:34  dkrajzew
// debugging
//
// Revision 1.3  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.2  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.1  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/09/09 12:50:30  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.1  2005/08/01 13:06:51  dksumo
// further triggers added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
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
#include <utils/common/ToString.h>
#include <utils/helpers/Command.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include "GUITriggeredRerouter.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/glutils/polyfonts.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUITriggeredRerouterPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITriggeredRerouter::GUITriggeredRerouterPopupMenu)
    GUITriggeredRerouterPopupMenuMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUITriggeredRerouter::GUITriggeredRerouterPopupMenu::onCmdOpenManip),

};

// Object implementation
FXIMPLEMENT(GUITriggeredRerouter::GUITriggeredRerouterPopupMenu, GUIGLObjectPopupMenu, GUITriggeredRerouterPopupMenuMap, ARRAYNUMBER(GUITriggeredRerouterPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUIManip_TriggeredRerouter - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITriggeredRerouter::GUIManip_TriggeredRerouter) GUIManip_TriggeredRerouterMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_USER_DEF, GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdUserDef),
    FXMAPFUNC(SEL_UPDATE,   GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_USER_DEF, GUITriggeredRerouter::GUIManip_TriggeredRerouter::onUpdUserDef),
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_OPTION,   GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdChangeOption),
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_CLOSE,    GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdClose),
};

FXIMPLEMENT(GUITriggeredRerouter::GUIManip_TriggeredRerouter, GUIManipulator, GUIManip_TriggeredRerouterMap, ARRAYNUMBER(GUIManip_TriggeredRerouterMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUIManip_TriggeredRerouter - methods
 * ----------------------------------------------------------------------- */
GUITriggeredRerouter::GUIManip_TriggeredRerouter::GUIManip_TriggeredRerouter(
        GUIMainWindow &app,
        const std::string &name, GUITriggeredRerouter &o,
        int xpos, int ypos)
    : GUIManipulator(app, name, 0, 0), myChosenValue(0),
    myParent(&app),
    myChosenTarget(myChosenValue, this, MID_OPTION), myUsageProbabilityTarget(myUsageProbability),
    myUsageProbability(o.getProbability()),
    myObject(&o)
{
    FXVerticalFrame *f1 =
        new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

    FXGroupBox *gp = new FXGroupBox(f1, "Change Probability",
        GROUPBOX_TITLE_LEFT|FRAME_SUNKEN|FRAME_RIDGE,
        0, 0, 0, 0,  4, 4, 1, 1, 2, 0);
    {
        // default
        FXHorizontalFrame *gf1 =
            new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf1, "Default", &myChosenTarget, FXDataTarget::ID_OPTION+0,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    }
    {
        // free
        FXHorizontalFrame *gf12 =
            new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf12, "User Given: ", &myChosenTarget, FXDataTarget::ID_OPTION+1,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP|LAYOUT_CENTER_Y,
            0, 0, 0, 0,   2, 2, 0, 0);
        myUsageProbabilityDial =
            new FXRealSpinDial(gf12, 10, this, MID_USER_DEF,
                LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myUsageProbabilityDial->setFormatString("%.2f");
        myUsageProbabilityDial->setIncrements(.1,.1,.1);
        myUsageProbabilityDial->setRange(0,1);
        myUsageProbabilityDial->setValue(myObject->getUserProbability());
    }
    {
        // off
        FXHorizontalFrame *gf13 =
            new FXHorizontalFrame(gp, LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0, 10,10,5,5);
        new FXRadioButton(gf13, "Off", &myChosenTarget, FXDataTarget::ID_OPTION+2,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP,
            0, 0, 0, 0,   2, 2, 0, 0);
    }
    myChosenValue = myObject->inUserMode()
        ? myObject->getUserProbability()>0
            ? 1 : 2
        : 0;
    new FXButton(f1,"Close",NULL,this,MID_CLOSE,
        BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
}


GUITriggeredRerouter::GUIManip_TriggeredRerouter::~GUIManip_TriggeredRerouter()
{
}


long
GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdClose(FXObject*,FXSelector,void*)
{
    destroy();
    return 1;
}


long
GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdUserDef(FXObject*,FXSelector,void*)
{
    myUsageProbability = (SUMOReal) (myUsageProbabilityDial->getValue());
    static_cast<GUITriggeredRerouter*>(myObject)->setUserUsageProbability(myUsageProbability);
    static_cast<GUITriggeredRerouter*>(myObject)->setUserMode(true);
    myParent->updateChildren();
    return 1;
}


long
GUITriggeredRerouter::GUIManip_TriggeredRerouter::onUpdUserDef(FXObject *sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myChosenValue!=1 ? FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    myParent->updateChildren();
    return 1;
}


long
GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdChangeOption(FXObject*,FXSelector,void*)
{
    static_cast<GUITriggeredRerouter*>(myObject)->setUserUsageProbability(myUsageProbability);
    switch(myChosenValue) {
    case 0:
        static_cast<GUITriggeredRerouter*>(myObject)->setUserMode(false);
        break;
    case 1:
        static_cast<GUITriggeredRerouter*>(myObject)->setUserMode(true);
        break;
    case 2:
        static_cast<GUITriggeredRerouter*>(myObject)->setUserUsageProbability(0);
        static_cast<GUITriggeredRerouter*>(myObject)->setUserMode(true);
        break;
    default:
        throw 1;
    }
    myParent->updateChildren();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUITriggeredRerouterPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUITriggeredRerouter::GUITriggeredRerouterPopupMenu::GUITriggeredRerouterPopupMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent,
        GUIGlObject &o)
    : GUIGLObjectPopupMenu(app, parent, o)
{
}


GUITriggeredRerouter::GUITriggeredRerouterPopupMenu::~GUITriggeredRerouterPopupMenu()
{
}


long
GUITriggeredRerouter::GUITriggeredRerouterPopupMenu::onCmdOpenManip(FXObject*,
                                                                  FXSelector,
                                                                  void*)
{
    static_cast<GUITriggeredRerouter*>(myObject)->openManipulator(
        *myApplication, *myParent);
    return 1;
}


/* -------------------------------------------------------------------------
 * GUITriggeredRerouter - methods
 * ----------------------------------------------------------------------- */
GUITriggeredRerouter::GUITriggeredRerouter(const std::string &id,
            MSNet &net, const std::vector<MSEdge*> &edges,
            SUMOReal prob, const std::string &aXMLFilename)
    : MSTriggeredRerouter(id, net, edges, prob, aXMLFilename),
    GUIGlObject_AbstractAdd(gIDStorage,
        string("rerouter:") + id, GLO_TRIGGER)
{
    size_t k;
    size_t no = 0;
    for(k=0; k<edges.size(); k++) {
        GUIEdge *gedge = static_cast<GUIEdge*>(edges[k]);
        no += gedge->getLanes()->size();
    }
    mySGPositions.reserve(no);
    myFGPositions.reserve(no);
    myFGRotations.reserve(no);
    mySGRotations.reserve(no);
    for(k=0; k<edges.size(); k++) {
        GUIEdge *gedge = static_cast<GUIEdge*>(edges[k]);
        MSEdge::LaneCont *lanes = gedge->getLanes();
        size_t noLanes = lanes->size();
        for(size_t i=0; i<noLanes; ++i) {
            const Position2DVector &v =
                gedge->getLaneGeometry((size_t) i).getShape();
            SUMOReal pos = v.length() - (SUMOReal) 6.;
            myFGPositions.push_back(v.positionAtLengthPosition(pos));
            Line2D l(v.getBegin(), v.getEnd());
            mySGPositions.push_back(l.getPositionAtDistance(pos));
            myFGRotations.push_back(-v.rotationDegreeAtLengthPosition(pos));
            mySGRotations.push_back(-l.atan2DegreeAngle());
        }
    }
}


GUITriggeredRerouter::~GUITriggeredRerouter()
{
}


GUIGLObjectPopupMenu *
GUITriggeredRerouter::getPopUpMenu(GUIMainWindow &app,
                                  GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret =
        new GUITriggeredRerouterPopupMenu(app, parent, *this);
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Open Manipulator...",
        GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_MANIP);
    //
    if(gSelected.isSelected(GLO_TRIGGER, getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Show Parameter",
        GUIIconSubSys::getIcon(ICON_APP_TABLE), ret, MID_SHOWPARS);
    return ret;
}


GUIParameterTableWindow *
GUITriggeredRerouter::getParameterWindow(GUIMainWindow &app,
                                        GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 7);
    // add items
    /*
    ret->mkItem("speed [m/s]", true,
        new FunctionBinding<GUITriggeredRerouter, SUMOReal>(this, &GUITriggeredRerouter::getCurrentSpeed));
        */
    // close building
    ret->closeBuilding();
    return ret;
}


std::string
GUITriggeredRerouter::microsimID() const
{
    return getID();
}


bool
GUITriggeredRerouter::active() const
{
    return true;
}


Position2D
GUITriggeredRerouter::getPosition() const
{
    return (*(myFGPositions.begin())); // !!!
}


void
GUITriggeredRerouter::drawGL_FG(SUMOReal scale, SUMOReal upscale)
{
    doPaint(myFGPositions, myFGRotations, scale, upscale);
}


void
GUITriggeredRerouter::drawGL_SG(SUMOReal scale, SUMOReal upscale)
{
    doPaint(mySGPositions, mySGRotations, scale, upscale);
}


void
GUITriggeredRerouter::doPaint(const PosCont &poss,
                              const RotCont rots,
                              SUMOReal scale, SUMOReal upscale)
{
    for(size_t i=0; i<poss.size(); ++i) {
        const Position2D &pos = poss[i];
        SUMOReal rot = rots[i];
        glPushMatrix();
        glScaled(upscale, upscale, upscale);
        glTranslated(pos.x(), pos.y(), 0);
        glRotated( rot, 0, 0, 1 );
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBegin(GL_TRIANGLES);
        glColor3f(1, .8f, 0);
        // base
        glVertex2d(0-1.4, 0);
        glVertex2d(0-1.4, 6);
        glVertex2d(0+1.4, 6);
        glVertex2d(0+1.4, 0);
        glVertex2d(0-1.4, 0);
        glVertex2d(0+1.4, 6);
        glEnd();

        glColor3f(0, 0, 0);
        pfSetPosition(0, 0);
        pfSetScale(3.f);
        SUMOReal w = pfdkGetStringWidth("U");
        glRotated(180, 0, 1, 0);
        glTranslated(-w/2., 2, 0);
        pfDrawString("U");

        glTranslated(w/2., -2, 0);
        SUMOReal prob = myAmInUserMode ? myUserProbability : myProbability;
        prob *= 100.;
        prob = (SUMOReal) ((int) prob);
        string str = toString(prob) + "%";
        pfSetPosition(0, 0);
        pfSetScale(1.4f);
        w = pfdkGetStringWidth(str.c_str());
        glTranslated(-w/2., 4, 0);
        pfDrawString(str.c_str());
        glPopMatrix();
    }
    if(hasCurrentReroute(MSNet::getInstance()->getCurrentTimeStep())&&getProbability()>0) {
        const RerouteInterval & ri =
            getCurrentReroute(MSNet::getInstance()->getCurrentTimeStep());
        for(std::vector<MSEdge*>::const_iterator i=ri.closed.begin(); i!=ri.closed.end(); ++i) {
            GUIEdge *gedge = static_cast<GUIEdge*>(*i);
            MSEdge::LaneCont *lanes = gedge->getLanes();
            size_t noLanes = lanes->size();
            SUMOReal prob = getProbability()*360;
            for(size_t j=0; j<noLanes; ++j) {
                const Position2DVector &v =
                    gedge->getLaneGeometry((size_t) j).getShape();
                SUMOReal d = 3.;
                Position2D pos = v.positionAtLengthPosition(d);
                SUMOReal rot = -v.rotationDegreeAtLengthPosition(d);

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
                glColor3f((SUMOReal) 0.7, (SUMOReal) 0, (SUMOReal) 0);
                GLHelper::drawFilledCircle((SUMOReal) 1.3, noPoints);
                glColor3f(1, 0, 0);
                GLHelper::drawFilledCircle((SUMOReal) 1.3, noPoints, 0, prob);
                glColor3f(1, 1, 1);
                glRotated( -90, 0, 0, 1 );
                glBegin(GL_TRIANGLES);
                glVertex2d(0-.3, -1.);
                glVertex2d(0-.3, 1.);
                glVertex2d(0+.3, 1.);
                glVertex2d(0+.3, -1.);
                glVertex2d(0-.3, -1.);
                glVertex2d(0+.3, 1.);
                glEnd();
                glPopMatrix();
            }
        }
    }
}


Boundary
GUITriggeredRerouter::getBoundary() const
{
    Position2D pos = getPosition();
    Boundary ret(pos.x(), pos.y(), pos.x(), pos.y());
    ret.grow(2.0);
    return ret;
}


GUIManipulator *
GUITriggeredRerouter::openManipulator(GUIMainWindow &app,
                                     GUISUMOAbstractView &parent)
{
    GUIManip_TriggeredRerouter *gui =
        new GUIManip_TriggeredRerouter(app, getFullName(), *this, 0, 0);
    gui->create();
    gui->show();
    return gui;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


