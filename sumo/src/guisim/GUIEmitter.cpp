//---------------------------------------------------------------------------//
//                        GUIEmitter.cpp -
//  The gui-version of MSEmitter
//                           -------------------
//  begin                : Thu, 21.07.2005
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
// Revision 1.3  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.2  2005/12/01 07:33:44  dkrajzew
// introducing bus stops: eased building vehicles; vehicles may now have nested elements
//
// Revision 1.1  2005/11/09 06:35:03  dkrajzew
// Emitters reworked
//
// Revision 1.5  2005/10/17 08:31:59  dksumo
// emitter rework#1
//
// Revision 1.4  2005/10/06 13:39:12  dksumo
// using of a configuration file rechecked
//
// Revision 1.3  2005/09/20 06:10:40  dksumo
// floats and doubles replaced by SUMOReal; warnings removed
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
#include "GUIEmitter.h"
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
 * GUIEmitter::GUIEmitterPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUIEmitter::GUIEmitterPopupMenu)
    GUIEmitterPopupMenuMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUIEmitter::GUIEmitterPopupMenu::onCmdOpenManip),

};

// Object implementation
FXIMPLEMENT(GUIEmitter::GUIEmitterPopupMenu, GUIGLObjectPopupMenu, GUIEmitterPopupMenuMap, ARRAYNUMBER(GUIEmitterPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUIEmitter::GUIManip_TriggeredEmitter - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUIEmitter::GUIManip_TriggeredEmitter) GUIManip_TriggeredEmitterMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  GUIEmitter::GUIManip_TriggeredEmitter::MID_USER_DEF, GUIEmitter::GUIManip_TriggeredEmitter::onCmdUserDef),
    FXMAPFUNC(SEL_UPDATE,   GUIEmitter::GUIManip_TriggeredEmitter::MID_USER_DEF, GUIEmitter::GUIManip_TriggeredEmitter::onUpdUserDef),
    FXMAPFUNC(SEL_COMMAND,  GUIEmitter::GUIManip_TriggeredEmitter::MID_OPTION,   GUIEmitter::GUIManip_TriggeredEmitter::onCmdChangeOption),
    FXMAPFUNC(SEL_COMMAND,  GUIEmitter::GUIManip_TriggeredEmitter::MID_CLOSE,    GUIEmitter::GUIManip_TriggeredEmitter::onCmdClose),
};

FXIMPLEMENT(GUIEmitter::GUIManip_TriggeredEmitter, GUIManipulator, GUIManip_TriggeredEmitterMap, ARRAYNUMBER(GUIManip_TriggeredEmitterMap))



/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUIEmitter::GUIManip_TriggeredEmitter - methods
 * ----------------------------------------------------------------------- */
GUIEmitter::GUIEmitterChild_UserTriggeredChild::GUIEmitterChild_UserTriggeredChild(
                MSEmitter_FileTriggeredChild &s,
                MSEmitter &parent,
                SUMOReal flow)
    : MSEmitter::MSEmitterChild(parent), myUserFlow(flow),
    myVehicle(0), mySource(s)
{
    if(myUserFlow>0) {
        MSEventControl::getBeginOfTimestepEvents()->addEvent(
            this, (SUMOTime) (1. / (flow / 3600.))+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
    }
}


GUIEmitter::GUIEmitterChild_UserTriggeredChild::~GUIEmitterChild_UserTriggeredChild()
{
}


SUMOTime
GUIEmitter::GUIEmitterChild_UserTriggeredChild::execute()
{
    if(myUserFlow==0) {
        return 0;
    }
    if(!mySource.isInitialised()) {
        mySource.init();
    }
    if(myVehicle==0) {
        string aVehicleId = myParent.getID() + string( "_user_" ) +  toString(MSNet::getInstance()->getCurrentTimeStep());
        MSRoute *aRoute = myRouteDist.getOverallProb()!=0
            ? myRouteDist.get()
            : mySource.hasRoutes()
                ? mySource.getRndRoute()
                : 0;
        if(aRoute==0) {
            MsgHandler::getErrorInstance()->inform("Emitter '" + myParent.getID() + "' has no valid route.");
            return 0;
        }
        MSVehicleType *aType = myVTypeDist.getOverallProb()!=0
            ? myVTypeDist.get()
            : mySource.hasVTypes()
                ? mySource.getRndVType()
                : MSVehicleType::dict_Random();
        if(aType==0) {
            MsgHandler::getErrorInstance()->inform("Emitter '" + myParent.getID() + "' has no valid vehicle type.");
            return 0;
        }
        SUMOTime aEmitTime = MSNet::getInstance()->getCurrentTimeStep();
        myVehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(
            aVehicleId, aRoute, aEmitTime, aType, 0, 0);
        myParent.schedule(this, myVehicle, -1);
    }
    if(myParent.childCheckEmit(this)) {
        myVehicle = 0;
        return (SUMOTime) computeOffset(myUserFlow);
    }
    return 1;
}


SUMOReal
GUIEmitter::GUIEmitterChild_UserTriggeredChild::getUserFlow() const
{
    return myUserFlow;
}



GUIEmitter::GUIManip_TriggeredEmitter::GUIManip_TriggeredEmitter(
        GUIMainWindow &app,
        const std::string &name, GUIEmitter &o,
        int xpos, int ypos)
    : GUIManipulator(app, name, 0, 0), myChosenValue(0),
    myParent(&app), myFlowFactor(o.getUserFlow()),
    myChosenTarget(myChosenValue, this, MID_OPTION), myFlowFactorTarget(myFlowFactor),
    myObject(&o)
{
    FXVerticalFrame *f1 =
        new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

    myChosenValue = o.getActiveChildIndex();

    FXGroupBox *gp = new FXGroupBox(f1, "Change Flow",
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
        new FXRadioButton(gf12, "User Defined Flow: ", &myChosenTarget, FXDataTarget::ID_OPTION+1,
            ICON_BEFORE_TEXT|LAYOUT_SIDE_TOP|LAYOUT_CENTER_Y,
            0, 0, 0, 0,   2, 2, 0, 0);
        myFlowFactorDial =
            new FXRealSpinDial(gf12, 8, this, MID_USER_DEF,
                LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myFlowFactorDial->setFormatString("%.0f");
        myFlowFactorDial->setIncrements(1,10,100);
        myFlowFactorDial->setRange(0,4000);
        myFlowFactorDial->setValue(myObject->getUserFlow());
    }
    new FXButton(f1,"Close",NULL,this,MID_CLOSE,
        BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
}


GUIEmitter::GUIManip_TriggeredEmitter::~GUIManip_TriggeredEmitter()
{
}


long
GUIEmitter::GUIManip_TriggeredEmitter::onCmdClose(FXObject*,FXSelector,void*)
{
    destroy();
    return 1;
}


long
GUIEmitter::GUIManip_TriggeredEmitter::onCmdUserDef(FXObject*,FXSelector,void*)
{
    static_cast<GUIEmitter*>(myObject)->setUserFlow(
        (SUMOReal) (myFlowFactorDial->getValue()));
    static_cast<GUIEmitter*>(myObject)->setActiveChild(1);
    myParent->updateChildren();
    return 1;
}


long
GUIEmitter::GUIManip_TriggeredEmitter::onUpdUserDef(FXObject *sender,FXSelector,void*ptr)
{
    sender->handle(this,
        myChosenValue!=1 ? FXSEL(SEL_COMMAND,ID_DISABLE):FXSEL(SEL_COMMAND,ID_ENABLE),
        ptr);
    myParent->updateChildren();
    return 1;
}


long
GUIEmitter::GUIManip_TriggeredEmitter::onCmdChangeOption(FXObject*,FXSelector,void*)
{
    static_cast<GUIEmitter*>(myObject)->setUserFlow((SUMOReal) myFlowFactorDial->getValue());
    switch(myChosenValue) {
    case 0:
        static_cast<GUIEmitter*>(myObject)->setActiveChild(0);
        break;
    case 1:
        static_cast<GUIEmitter*>(myObject)->setActiveChild(1);
        break;
    default:
        throw 1;
    }
    myParent->updateChildren();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIEmitter::GUIEmitterPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIEmitter::GUIEmitterPopupMenu::GUIEmitterPopupMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent,
        GUIGlObject &o)
    : GUIGLObjectPopupMenu(app, parent, o)
{
}


GUIEmitter::GUIEmitterPopupMenu::~GUIEmitterPopupMenu()
{
}


long
GUIEmitter::GUIEmitterPopupMenu::onCmdOpenManip(FXObject*,
                                                                  FXSelector,
                                                                  void*)
{
    static_cast<GUIEmitter*>(myObject)->openManipulator(
        *myApplication, *myParent);
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIEmitter - methods
 * ----------------------------------------------------------------------- */
GUIEmitter::GUIEmitter(const std::string &id,
            MSNet &net, MSLane *destLanes, SUMOReal pos,
            const std::string &aXMLFilename)
    : MSEmitter(id, net, destLanes, pos, aXMLFilename),
    GUIGlObject_AbstractAdd(gIDStorage,
        string("emitter:") + id, GLO_TRIGGER), myUserFlow(-1)
{
    GUIEdge *edge =
        static_cast<GUIEdge*>(MSEdge::dictionary(destLanes->edge().id()));
    const Position2DVector &v =
        edge->getLaneGeometry(destLanes).getShape();
    if(pos<0) {
        pos = destLanes->length()+ pos;
    }
    myFGPosition = v.positionAtLengthPosition(pos);
    Line2D l(v.getBegin(), v.getEnd());
    mySGPosition = l.getPositionAtDistance(0);
    myFGRotation = -v.rotationDegreeAtLengthPosition(pos);
    mySGRotation = -l.atan2DegreeAngle();

    myUserEmitChild =
        new GUIEmitterChild_UserTriggeredChild(
            static_cast<MSEmitter_FileTriggeredChild&>(*myFileBasedEmitter),
            *this, 10000);
}


GUIEmitter::~GUIEmitter()
{
}


void
GUIEmitter::setUserFlow(SUMOReal factor)
{
    // !!! the commands should be adapted to current flow imediatly
    myUserFlow = factor;
    if(myUserFlow>0) {
        myUserEmitChild =
            new GUIEmitterChild_UserTriggeredChild(
                static_cast<MSEmitter_FileTriggeredChild&>(*myFileBasedEmitter),
                *this, factor);
    }
}


SUMOReal
GUIEmitter::getUserFlow() const
{
    if(myUserFlow<0&&static_cast<MSEmitter_FileTriggeredChild*>(myFileBasedEmitter)->getLoadedFlow()>0) {
        myUserFlow = static_cast<MSEmitter_FileTriggeredChild*>(myFileBasedEmitter)->getLoadedFlow();
    }
    return myUserFlow;
}


GUIGLObjectPopupMenu *
GUIEmitter::getPopUpMenu(GUIMainWindow &app,
                                  GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret =
        new GUIEmitterPopupMenu(app, parent, *this);
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
GUIEmitter::getParameterWindow(GUIMainWindow &app,
                                        GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 7);
    // add items
    /*
    ret->mkItem("speed [m/s]", true,
        new FunctionBinding<GUIEmitter, SUMOReal>(this, &GUIEmitter::getCurrentSpeed));
        */
    // close building
    ret->closeBuilding();
    return ret;
}


std::string
GUIEmitter::microsimID() const
{
    return getID();
}


bool
GUIEmitter::active() const
{
    return true;
}


Position2D
GUIEmitter::getPosition() const
{
    return myFGPosition;
}


void
GUIEmitter::drawGL_FG(SUMOReal scale, SUMOReal upscale)
{
    doPaint(myFGPosition, myFGRotation, scale, upscale);
}


void
GUIEmitter::drawGL_SG(SUMOReal scale, SUMOReal upscale)
{
    doPaint(mySGPosition, mySGRotation, scale, upscale);
}


void
GUIEmitter::doPaint(const Position2D &pos, SUMOReal rot,
                    SUMOReal scale, SUMOReal upscale)
{
    glPushMatrix();
    glScaled(upscale, upscale, upscale);
    glTranslated(pos.x(), pos.y(), 0);
    glRotated( rot, 0, 0, 1 );

    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    // base
            glVertex2d(0-1.5, 0);
            glVertex2d(0-1.5, 8);
            glVertex2d(0+1.5, 8);
            glVertex2d(0+1.5, 0);
            glVertex2d(0-1.5, 0);
            glVertex2d(0+1.5, 8);

    glColor3f(1, 1, 0);
    glVertex2d(0, 1-.5);
    glVertex2d(0-1.25, 1+2-.5);
    glVertex2d(0+1.25, 1+2-.5);

    glVertex2d(0, 3-.5);
    glVertex2d(0-1.25, 3+2-.5);
    glVertex2d(0+1.25, 3+2-.5);

    glColor3f(1, 1, 0);
    glVertex2d(0, 5-.5);
    glVertex2d(0-1.25, 5+2-.5);
    glVertex2d(0+1.25, 5+2-.5);

    glEnd();
    glPopMatrix();
}


Boundary
GUIEmitter::getBoundary() const
{
    Position2D pos = getPosition();
    Boundary ret(pos.x(), pos.y(), pos.x(), pos.y());
    ret.grow(2.0);
    return ret;
}


GUIManipulator *
GUIEmitter::openManipulator(GUIMainWindow &app,
                                     GUISUMOAbstractView &parent)
{
    GUIManip_TriggeredEmitter *gui =
        new GUIManip_TriggeredEmitter(app, getFullName(), *this, 0, 0);
    gui->create();
    gui->show();
    return gui;
}


void
GUIEmitter::setActiveChild(int index)
{
    switch(index) {
    case 0:
        myActiveChild = myFileBasedEmitter;
        break;
    case 1:
        myActiveChild = myUserEmitChild;
        break;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


