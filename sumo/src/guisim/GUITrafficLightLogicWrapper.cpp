#include <gui/GUIGlObject.h>
#include <gui/GUIGlObjectStorage.h>
#include <gui/GUIApplicationWindow.h>
#include <microsim/MSTrafficLightLogic.h>
#include <gui/popup/QGLObjectPopupMenu.h>
#include <gui/popup/QGLObjectPopupMenuItem.h>
#include <guisim/guilogging/GLObjectValuePassConnector.h>
#include <microsim/logging/FunctionBinding.h>
#include <gui/tlstracker/GUITLLogicPhasesTrackerWindow.h>
#include "GUITrafficLightLogicWrapper.h"

using namespace std;

GUITrafficLightLogicWrapper::GUITrafficLightLogicWrapper(
        GUIGlObjectStorage &idStorage, MSTrafficLightLogic &tll)
    : GUIGlObject(idStorage, string("tl-logic:")+tll.id()), myTLLogic(tll)
{
}


GUITrafficLightLogicWrapper::~GUITrafficLightLogicWrapper()
{
}


QGLObjectPopupMenu *
GUITrafficLightLogicWrapper::getPopUpMenu(GUIApplicationWindow &app,
                                          GUISUMOAbstractView &parent)
{
    myApp = &app;
    QGLObjectPopupMenu *ret = new QGLObjectPopupMenu(app, parent, *this);
    int id;
    // insert name
    id = ret->insertItem(
        new QGLObjectPopupMenuItem(ret, getFullName().c_str(), true));
    ret->insertSeparator();
    // add showing option
    id = ret->insertItem("Show Phases", ret, SLOT(showPhases()));
    // add view option
    id = ret->insertItem("Center", ret, SLOT(center()));
    ret->setItemEnabled(id, TRUE);
    ret->insertSeparator();
    return ret;
}


void
GUITrafficLightLogicWrapper::showPhases()
{
    new GLObjectValuePassConnector<SimplePhaseDef>
        (*this,
        new FunctionBinding<GUITrafficLightLogicWrapper, SimplePhaseDef>
                (this, &GUITrafficLightLogicWrapper::getPhaseDef),
        new GUITLLogicPhasesTrackerWindow(*myApp, myTLLogic));
}


SimplePhaseDef
GUITrafficLightLogicWrapper::getPhaseDef() const
{
    return SimplePhaseDef(myTLLogic.allowed(), myTLLogic.yellowMask());
}


GUIParameterTableWindow *
GUITrafficLightLogicWrapper::getParameterWindow(GUIApplicationWindow &app,
                                                GUISUMOAbstractView &parent)
{
    return 0;
}


GUIGlObjectType
GUITrafficLightLogicWrapper::getType() const
{
    return GLO_TLLOGIC;
}


bool
GUITrafficLightLogicWrapper::active() const
{
    return true;
}


std::string
GUITrafficLightLogicWrapper::microsimID() const
{
    return myTLLogic.id();
}
