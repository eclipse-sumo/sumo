/****************************************************************************/
/// @file    GUIEmitter.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 21.07.2005
/// @version $Id$
///
// A vehicle emitting device (gui version)
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
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/common/WrappingCommand.h>

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
 * GUIEmitter::GUIEmitterPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUIEmitter::GUIEmitterPopupMenu)
GUIEmitterPopupMenuMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUIEmitter::GUIEmitterPopupMenu::onCmdOpenManip),
    FXMAPFUNC(SEL_COMMAND,  MID_DRAWROUTE,     GUIEmitter::GUIEmitterPopupMenu::onCmdDrawRoute),

};

// Object implementation
FXIMPLEMENT(GUIEmitter::GUIEmitterPopupMenu, GUIGLObjectPopupMenu, GUIEmitterPopupMenuMap, ARRAYNUMBER(GUIEmitterPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUIEmitter::GUIManip_TriggeredEmitter - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUIEmitter::GUIManip_TriggeredEmitter) GUIManip_TriggeredEmitterMap[]= {
    FXMAPFUNC(SEL_COMMAND,  GUIEmitter::GUIManip_TriggeredEmitter::MID_USER_DEF, GUIEmitter::GUIManip_TriggeredEmitter::onCmdUserDef),
    FXMAPFUNC(SEL_UPDATE,   GUIEmitter::GUIManip_TriggeredEmitter::MID_USER_DEF, GUIEmitter::GUIManip_TriggeredEmitter::onUpdUserDef),
    FXMAPFUNC(SEL_COMMAND,  GUIEmitter::GUIManip_TriggeredEmitter::MID_OPTION,   GUIEmitter::GUIManip_TriggeredEmitter::onCmdChangeOption),
    FXMAPFUNC(SEL_COMMAND,  GUIEmitter::GUIManip_TriggeredEmitter::MID_CLOSE,    GUIEmitter::GUIManip_TriggeredEmitter::onCmdClose),
};

FXIMPLEMENT(GUIEmitter::GUIManip_TriggeredEmitter, GUIManipulator, GUIManip_TriggeredEmitterMap, ARRAYNUMBER(GUIManip_TriggeredEmitterMap))



// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIEmitter::GUIManip_TriggeredEmitter - methods
 * ----------------------------------------------------------------------- */
GUIEmitter::GUIEmitterChild_UserTriggeredChild::GUIEmitterChild_UserTriggeredChild(
    MSEmitter_FileTriggeredChild &s,
    MSEmitter &parent, MSVehicleControl &vc,
    SUMOReal flow) throw()
        : MSEmitter::MSEmitterChild(parent, vc), myUserFlow(flow),
        myVehicle(0), mySource(s), myDescheduleVehicle(false)
{
    if (myUserFlow>0) {
        Command* c = new WrappingCommand< GUIEmitterChild_UserTriggeredChild >(this, &GUIEmitterChild_UserTriggeredChild::wrappedExecute);
        MSNet::getInstance()->getEmissionEvents().addEvent(
            c, (SUMOTime)(1. / (flow / 3600.))+MSNet::getInstance()->getCurrentTimeStep(),
            MSEventControl::ADAPT_AFTER_EXECUTION);
        MSNet::getInstance()->getVehicleControl().newUnbuildVehicleLoaded();
        myDescheduleVehicle = true;
    }
}


GUIEmitter::GUIEmitterChild_UserTriggeredChild::~GUIEmitterChild_UserTriggeredChild() throw()
{
    if (myDescheduleVehicle) {
        MSNet::getInstance()->getVehicleControl().newUnbuildVehicleBuild();
    }
}


SUMOTime
GUIEmitter::GUIEmitterChild_UserTriggeredChild::wrappedExecute(SUMOTime currentTime) throw(ProcessError)
{
    if (myUserFlow<=0) {
        return 0;
    }
    if (!mySource.isInitialised()) {
        mySource.init();
    }
    if (myVehicle==0) {
        SUMOVehicleParameter* p = new SUMOVehicleParameter();
        p->id = myParent.getID() + "_user_" +  toString(currentTime);
        MSRoute *aRoute = myRouteDist.getOverallProb()!=0
                          ? myRouteDist.get()
                          : mySource.hasRoutes()
                          ? mySource.getRndRoute()
                          : 0;
        if (aRoute==0) {
            MsgHandler::getErrorInstance()->inform("Emitter '" + myParent.getID() + "' has no valid route.");
            return 0;
        }
        MSVehicleType *aType = myVTypeDist.getOverallProb()!=0
                               ? myVTypeDist.get()
                               : mySource.hasVTypes()
                               ? mySource.getRndVType()
                               : MSNet::getInstance()->getVehicleControl().getRandomVType();
        if (aType==0) {
            MsgHandler::getErrorInstance()->inform("Emitter '" + myParent.getID() + "' has no valid vehicle type.");
            return 0;
        }
        p->depart = currentTime;
        myVehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(p, aRoute, aType);
        myParent.schedule(this, myVehicle, -1);
        if (myDescheduleVehicle) {
            MSNet::getInstance()->getVehicleControl().newUnbuildVehicleBuild();
            myDescheduleVehicle = false;
        }
    }
    if (myParent.childCheckEmit(this)) {
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
    int /*xpos*/, int /*ypos*/)
        : GUIManipulator(app, name, 0, 0), myParent(&app),
        myChosenValue(0), myChosenTarget(myChosenValue, this, MID_OPTION),
        myFlowFactor(o.getUserFlow()), myFlowFactorTarget(myFlowFactor),
        myObject(&o)
{
    FXVerticalFrame *f1 =
        new FXVerticalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

    myChosenValue = (FXint) o.getActiveChildIndex();

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
{}


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
        (SUMOReal)(myFlowFactorDial->getValue()));
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
    switch (myChosenValue) {
    case 0:
        static_cast<GUIEmitter*>(myObject)->setActiveChild(0);
        break;
    case 1:
        static_cast<GUIEmitter*>(myObject)->setActiveChild(1);
        break;
    default:
        // hmmm, should not happen
        break;
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
{}


GUIEmitter::GUIEmitterPopupMenu::~GUIEmitterPopupMenu()
{}


long
GUIEmitter::GUIEmitterPopupMenu::onCmdOpenManip(FXObject*,
        FXSelector,
        void*)
{
    static_cast<GUIEmitter*>(myObject)->openManipulator(
        *myApplication, *myParent);
    return 1;
}


long
GUIEmitter::GUIEmitterPopupMenu::onCmdDrawRoute(FXObject*,
        FXSelector,
        void*)
{
    static_cast<GUIEmitter*>(myObject)->toggleDrawRoutes();
    myParent->update();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIEmitter - methods
 * ----------------------------------------------------------------------- */
GUIEmitter::GUIEmitter(const std::string &id,
                       MSNet &net, MSLane *destLanes, SUMOReal pos,
                       const std::string &file) throw()
        : MSEmitter(id, net, destLanes, pos, file),
        GUIGlObject_AbstractAdd(gIDStorage,
                                "emitter:" + id, GLO_TRIGGER), myUserFlow(-1), myDrawRoutes(false)
{
    const GUIEdge * const edge = static_cast<const GUIEdge * const>(destLanes->getEdge());
    const Position2DVector &v =
        edge->getLaneGeometry(destLanes).getShape();
    if (pos<0) {
        pos = destLanes->length()+ pos;
    }
    myFGPosition = v.positionAtLengthPosition(pos);
    Line2D l(v.getBegin(), v.getEnd());
    myFGRotation = -v.rotationDegreeAtLengthPosition(pos);

    myUserEmitChild =
        new GUIEmitterChild_UserTriggeredChild(
        static_cast<MSEmitter_FileTriggeredChild&>(*myFileBasedEmitter),
        *this, net.getVehicleControl(), 0);
}


GUIEmitter::~GUIEmitter() throw()
{}


void
GUIEmitter::setUserFlow(SUMOReal factor)
{
    // !!! the commands should be adapted to current flow imediatly
    myUserFlow = factor;
    if (myUserFlow>0) {
        delete myUserEmitChild;
        myUserEmitChild =
            new GUIEmitterChild_UserTriggeredChild(
            static_cast<MSEmitter_FileTriggeredChild&>(*myFileBasedEmitter),
            *this, myNet.getVehicleControl(), factor);
    }
}


SUMOReal
GUIEmitter::getUserFlow() const
{
    if (myUserFlow<0&&static_cast<MSEmitter_FileTriggeredChild*>(myFileBasedEmitter)->getLoadedFlow()>0) {
        myUserFlow = static_cast<MSEmitter_FileTriggeredChild*>(myFileBasedEmitter)->getLoadedFlow();
    }
    return myUserFlow;
}


GUIGLObjectPopupMenu *
GUIEmitter::getPopUpMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent) throw()
{
    GUIGLObjectPopupMenu *ret = new GUIEmitterPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    //
    buildShowManipulatorPopupEntry(ret, false);
    if (!myDrawRoutes) {
        new FXMenuCommand(ret, "Show Routes...", GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_DRAWROUTE);
    } else {
        new FXMenuCommand(ret, "Hide Routes...", GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_DRAWROUTE);
    }
    new FXMenuSeparator(ret);
    //
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //(nothing to show, see below) buildShowParamsPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIEmitter::getParameterWindow(GUIMainWindow &,
                               GUISUMOAbstractView &) throw()
{
    return 0;
}


const std::string &
GUIEmitter::microsimID() const throw()
{
    return getID();
}


void
GUIEmitter::toggleDrawRoutes()
{
    myDrawRoutes = !myDrawRoutes;
}


std::map<const MSEdge*, SUMOReal>
GUIEmitter::getEdgeProbs() const
{
    std::map<const MSEdge*, SUMOReal> ret;
    const std::vector<MSRoute*> &routes = myFileBasedEmitter->getRouteDist().getVals();
    const std::vector<SUMOReal> &probs = myFileBasedEmitter->getRouteDist().getProbs();
    size_t j;

    for (j=0; j<routes.size(); ++j) {
        SUMOReal prob = probs[j];
        MSRoute *r = routes[j];
        MSRouteIterator i = r->begin();
        for (; i!=r->end(); ++i) {
            const MSEdge *e = *i;
            if (ret.find(e)==ret.end()) {
                ret[e] = 0;
            }
            ret[e] = ret[e] + prob;
        }
    }
    return ret;
}


void
GUIEmitter::drawGL(const GUIVisualizationSettings &s) const throw()
{
    // (optional) set id
    if (s.needsGlID) {
        glPushName(getGlID());
    }
    glPolygonOffset(0, -2);
    glPushMatrix();
    glTranslated(myFGPosition.x(), myFGPosition.y(), 0);
    glScaled(s.addExaggeration, s.addExaggeration, s.addExaggeration);
    glRotated(myFGRotation, 0, 0, 1);

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

    if (myDrawRoutes) {
        std::map<const MSEdge*, SUMOReal> e2prob = getEdgeProbs();
        for (std::map<const MSEdge*, SUMOReal>::iterator k=e2prob.begin(); k!=e2prob.end(); ++k) {
            double c = (*k).second;
            glColor3d(1.-c, 1.-c, 0);
            const MSEdge *e = (*k).first;
            const GUIEdge *ge = static_cast<const GUIEdge*>(e);
            const GUILaneWrapper &lane = ge->getLaneGeometry((size_t) 0);
            GLHelper::drawBoxLines(lane.getShape(), lane.getShapeRotations(), lane.getShapeLengths(), 0.5);
        }
    }
    // (optional) draw name
    if (s.drawAddName) {
        drawGLName(getCenteringBoundary().getCenter(), microsimID(), s.addNameSize / s.scale);
    }
    // (optional) clear id
    if (s.needsGlID) {
        glPopName();
    }
}


Boundary
GUIEmitter::getCenteringBoundary() const throw()
{
    Boundary b(myFGPosition.x(), myFGPosition.y(), myFGPosition.x(), myFGPosition.y());
    b.grow(20);
    return b;
}


GUIManipulator *
GUIEmitter::openManipulator(GUIMainWindow &app,
                            GUISUMOAbstractView &)
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
    switch (index) {
    case 0:
        myActiveChild = myFileBasedEmitter;
        break;
    case 1:
        myActiveChild = myUserEmitChild;
        break;
    }
}



/****************************************************************************/

