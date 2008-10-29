/****************************************************************************/
/// @file    GUITriggeredRerouter.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 25.07.2005
/// @version $Id$
///
// Reroutes vehicles passing an edge (gui version)
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#pragma warning(disable: 4355)
#endif


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
#include <utils/common/Command.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include "GUITriggeredRerouter.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
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
/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUITriggeredRerouterPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITriggeredRerouter::GUITriggeredRerouterPopupMenu)
GUITriggeredRerouterPopupMenuMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUITriggeredRerouter::GUITriggeredRerouterPopupMenu::onCmdOpenManip),

};

// Object implementation
FXIMPLEMENT(GUITriggeredRerouter::GUITriggeredRerouterPopupMenu, GUIGLObjectPopupMenu, GUITriggeredRerouterPopupMenuMap, ARRAYNUMBER(GUITriggeredRerouterPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUIManip_TriggeredRerouter - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITriggeredRerouter::GUIManip_TriggeredRerouter) GUIManip_TriggeredRerouterMap[]= {
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_USER_DEF, GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdUserDef),
    FXMAPFUNC(SEL_UPDATE,   GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_USER_DEF, GUITriggeredRerouter::GUIManip_TriggeredRerouter::onUpdUserDef),
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_OPTION,   GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdChangeOption),
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_CLOSE,    GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdClose),
};

FXIMPLEMENT(GUITriggeredRerouter::GUIManip_TriggeredRerouter, GUIManipulator, GUIManip_TriggeredRerouterMap, ARRAYNUMBER(GUIManip_TriggeredRerouterMap))


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUIManip_TriggeredRerouter - methods
 * ----------------------------------------------------------------------- */
GUITriggeredRerouter::GUIManip_TriggeredRerouter::GUIManip_TriggeredRerouter(
    GUIMainWindow &app,
    const std::string &name, GUITriggeredRerouter &o,
    int /*xpos*/, int /*ypos*/)
        : GUIManipulator(app, name, 0, 0), myParent(&app),
        myChosenValue(0), myChosenTarget(myChosenValue, this, MID_OPTION),
        myUsageProbability(o.getProbability()), myUsageProbabilityTarget(myUsageProbability),
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
{}


long
GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdClose(FXObject*,FXSelector,void*)
{
    destroy();
    return 1;
}


long
GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdUserDef(FXObject*,FXSelector,void*)
{
    myUsageProbability = (SUMOReal)(myUsageProbabilityDial->getValue());
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
    switch (myChosenValue) {
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
{}


GUITriggeredRerouter::GUITriggeredRerouterPopupMenu::~GUITriggeredRerouterPopupMenu()
{}


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
        const std::vector<MSEdge*> &edges,
        SUMOReal prob, const std::string &aXMLFilename)
        : MSTriggeredRerouter(id, edges, prob, aXMLFilename),
        GUIGlObject_AbstractAdd(gIDStorage, "rerouter:" + id, GLO_TRIGGER)
{
    size_t k;
    size_t no = 0;
    for (k=0; k<edges.size(); k++) {
        GUIEdge *gedge = static_cast<GUIEdge*>(edges[k]);
        no += gedge->getLanes()->size();
    }
    myFGPositions.reserve(no);
    myFGRotations.reserve(no);
    for (k=0; k<edges.size(); k++) {
        GUIEdge *gedge = static_cast<GUIEdge*>(edges[k]);
        const MSEdge::LaneCont * const lanes = gedge->getLanes();
        size_t noLanes = lanes->size();
        for (size_t i=0; i<noLanes; ++i) {
            const Position2DVector &v =
                gedge->getLaneGeometry((size_t) i).getShape();
            SUMOReal pos = v.length() - (SUMOReal) 6.;
            myFGPositions.push_back(v.positionAtLengthPosition(pos));
            myBoundary.add(v.positionAtLengthPosition(pos));
            Line2D l(v.getBegin(), v.getEnd());
            myFGRotations.push_back(-v.rotationDegreeAtLengthPosition(pos));
        }
    }
}


GUITriggeredRerouter::~GUITriggeredRerouter() throw()
{}


GUIGLObjectPopupMenu *
GUITriggeredRerouter::getPopUpMenu(GUIMainWindow &app,
                                   GUISUMOAbstractView &parent) throw()
{
    GUIGLObjectPopupMenu *ret = new GUITriggeredRerouterPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildShowManipulatorPopupEntry(ret, false);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret, false);
    //(nothing to show, see below) buildShowParamsPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUITriggeredRerouter::getParameterWindow(GUIMainWindow &,
        GUISUMOAbstractView &) throw()
{
    return 0;
}


const std::string &
GUITriggeredRerouter::microsimID() const throw()
{
    return getID();
}


void
GUITriggeredRerouter::drawGL(const GUIVisualizationSettings &s) const throw()
{
    // (optional) set id
    if (s.needsGlID) {
        glPushName(getGlID());
    }
    glPolygonOffset(0, -2);
    for (size_t i=0; i<myFGPositions.size(); ++i) {
        const Position2D &pos = myFGPositions[i];
        SUMOReal rot = myFGRotations[i];
        glPushMatrix();
        glScaled(s.addExaggeration, s.addExaggeration, s.addExaggeration);
        glTranslated(pos.x(), pos.y(), 0);
        glRotated(rot, 0, 0, 1);
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
        prob = (SUMOReal)((int) prob);
        string str = toString(prob) + "%";
        pfSetPosition(0, 0);
        pfSetScale(1.4f);
        w = pfdkGetStringWidth(str.c_str());
        glTranslated(-w/2., 4, 0);
        pfDrawString(str.c_str());
        glPopMatrix();
    }
    if (hasCurrentReroute(MSNet::getInstance()->getCurrentTimeStep())&&getProbability()>0) {
        const RerouteInterval & ri =
            getCurrentReroute(MSNet::getInstance()->getCurrentTimeStep());
        for (std::vector<MSEdge*>::const_iterator i=ri.closed.begin(); i!=ri.closed.end(); ++i) {
            GUIEdge *gedge = static_cast<GUIEdge*>(*i);
            const MSEdge::LaneCont * const lanes = gedge->getLanes();
            size_t noLanes = lanes->size();
            SUMOReal prob = getProbability()*360;
            for (size_t j=0; j<noLanes; ++j) {
                const Position2DVector &v =
                    gedge->getLaneGeometry((size_t) j).getShape();
                SUMOReal d = 3.;
                Position2D pos = v.positionAtLengthPosition(d);
                SUMOReal rot = -v.rotationDegreeAtLengthPosition(d);

                glPushMatrix();
                glTranslated(pos.x(), pos.y(), 0);
                glRotated(rot, 0, 0, 1);
                glTranslated(0, -1.5, 0);

                int noPoints = 9;
                if (s.scale>25) {
                    noPoints = (int)(9.0 + s.scale / 10.0);
                    if (noPoints>36) {
                        noPoints = 36;
                    }
                }
                glColor3f((SUMOReal) 0.7, (SUMOReal) 0, (SUMOReal) 0);
                GLHelper::drawFilledCircle((SUMOReal) 1.3, noPoints);
                glColor3f(1, 0, 0);
                GLHelper::drawFilledCircle((SUMOReal) 1.3, noPoints, 0, prob);
                glColor3f(1, 1, 1);
                glRotated(-90, 0, 0, 1);
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
    // (optional) clear id
    if (s.needsGlID) {
        glPopName();
    }
}


Boundary
GUITriggeredRerouter::getCenteringBoundary() const throw()
{
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}



GUIManipulator *
GUITriggeredRerouter::openManipulator(GUIMainWindow &app,
                                      GUISUMOAbstractView &)
{
    GUIManip_TriggeredRerouter *gui =
        new GUIManip_TriggeredRerouter(app, getFullName(), *this, 0, 0);
    gui->create();
    gui->show();
    return gui;
}



/****************************************************************************/

